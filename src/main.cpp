#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <uvw/async.h>
#include <uvw/loop.h>

#include "config.h"
#include "cluster/cluster.h"
#include "cluster/primary.h"
#include "cluster/child.h"
#include "http/http.h"
#include "include/backward-cpp/backward.hpp"
#include "items/itemsdb.h"
#include "database/database.h"
#include "enetwrapper/enetserver.h"
#include "server/server.h"
#include "utils/textparse.h"

int main(int argc, char *argv[]) {
#if (!defined(_WIN32) || defined(__MINGW32__)) && !defined(__linux__)
    std::cout << "Other platforms are not tested." << std::endl;
    return EXIT_FAILURE;
#endif

    cluster::Primary *primary = nullptr;
    cluster::Child *child = nullptr;
    http::HTTP *http = nullptr;
    server::Server *server = nullptr;

    // Initialize libuv event loop.
    auto loop = uvw::Loop::getDefault();

    // Initialize logger.
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(fmt::format("{}server{}.log", config::data::log, cluster::Cluster::is_primary() ? "" : "_worker"), 1024 * 1024, 8)); // Using the port??

    auto logger = std::make_shared<spdlog::logger>("server", sinks.begin(), sinks.end());
    logger->set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");
    logger->set_level(config::debug ? spdlog::level::debug : spdlog::level::info);
    logger->flush_on(spdlog::level::debug);

    spdlog::set_default_logger(logger);

    if (cluster::Cluster::is_primary()) {
        spdlog::info("Primary {} is running", uvw::Utilities::OS::pid());
        spdlog::info("Number of cores: {}", uvw::Utilities::cpuInfo().size());

        static int port_offset = 0;

        primary = new cluster::Primary{ loop, config::dev ? false : true };

        primary->on("online", [](cluster::Worker *worker) {
            spdlog::info("Worker {} online", worker->id());
            worker->send(fmt::format("action|startserver\nportoffset|{}\n", port_offset++));
        });

        primary->on("message", [](cluster::Worker *worker, const std::string &message) {
            spdlog::info("Worker {} send message: {}", worker->id(), message);
        });

        primary->on("disconnect", [](cluster::Worker *worker) {
            spdlog::error("Worker {} disconnected", worker->id());
        });

        primary->on("exit", [&](cluster::Worker *worker, int64_t status, int signal) {
            spdlog::error("Worker {} exited, {}, {}", worker->id(), status, signal);

            if (!config::dev) {
                primary->fork(std::string{ argv[0] });
            }

            std::exit(EXIT_SUCCESS);
        });

        for (int i = 0; i < (config::dev ? 1 : uvw::Utilities::cpuInfo().size()); i++) {
            primary->fork(std::string{ argv[0] });
        }
    }
    else {
        child = new cluster::Child{ loop };

        child->on("message", [&](const std::string &message) {
            if (message.find("action|startserver") != std::string::npos) {
                spdlog::info("Growtopia private server starting..");

                http = new http::HTTP{ *loop };
                http->listen("0.0.0.0", 80);
                http->set_server_data(std::string{ config::server::host }, config::server::start_port);

                if (!database::get_database()->connect()) {
                    spdlog::error("Failed to connect to database.");
                    loop->stop();
                    return;
                }

                if (!items::get_items_db()->serialize()) {
                    spdlog::error("Failed to load items database.");
                    loop->stop();
                    return;
                }

                if (!enetwrapper::ENetServer::one_time_init()) {
                    spdlog::error("Failed to initialize ENet.");
                    loop->stop();
                    return;
                }

                TextParse text_parse{ message };
                auto port_offset = text_parse.get<uint16_t>("portoffset", 1);

                server = new server::Server{};
                if (!server->initialize(config::server::start_port + port_offset, config::server::max_peer)) {
                    spdlog::error("Failed to create server.");
                    loop->stop();
                    return;
                }
            }
        });
    }

    loop->run();
    loop->close();

    // Clean up.
    delete primary;
    delete child;
    sinks.clear();
    delete http;
    delete database::get_database();
    delete items::get_items_db();
    delete server;
    return EXIT_SUCCESS;
}