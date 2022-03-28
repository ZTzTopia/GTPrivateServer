#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <uvw/async.h>
#include <uvw/loop.h>
#include <uvw/thread.h>
#include <uvw/util.h>

#include "config.h"
#include "cluster/cluster.h"
#include "http/http.h"
#include "include/backward-cpp/backward.hpp"
#include "items/itemsdb.h"
#include "database/database.h"
#include "enetwrapper/enetserver.h"
#include "server/server.h"

int main(int argc, char *argv[]) {
#if (!defined(_WIN32) || defined(__MINGW32__)) && !defined(__linux__)
    std::cout << "Other platforms are not tested." << std::endl;
    return EXIT_FAILURE;
#endif

    http::HTTP *http = nullptr;
    server::Server *server = nullptr;

    // Initialize libuv event loop.
    auto loop = uvw::Loop::getDefault();

    // Stop event loop on different thread.
    auto async = loop->resource<uvw::AsyncHandle>();
    async->on<uvw::AsyncEvent>([loop](const uvw::AsyncEvent &, uvw::AsyncHandle &handle) {
        handle.close();
        loop->stop();
    });

    // Initialize cluster.
    auto *cluster = new cluster::Cluster{ loop };

    // Initialize logger.
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(fmt::format("{}server{}.log", config::data::log, cluster->is_primary() ? "" : "_worker"), 1024 * 1024, 8)); // Using the port??

    auto logger = std::make_shared<spdlog::logger>("server", sinks.begin(), sinks.end());
    logger->set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");
    logger->set_level(config::debug ? spdlog::level::debug : spdlog::level::info);
    logger->flush_on(spdlog::level::debug);

    spdlog::set_default_logger(logger);

    if (cluster->is_primary()) {
        spdlog::info("Primary {} is running", uvw::Utilities::OS::pid());
        spdlog::info("Number of cores: {}", uvw::Utilities::cpuInfo().size());

        // Inherit the primary logger to worker (stdout and stderr).
        cluster->setup_primary(false); // Set silent to false. (Is not good if you have two worker!, I don't implement it yet.)

        // We use one worker for developing mode.
        for (uint8_t i = 0; i < (config::dev ? 1 : uvw::Utilities::cpuInfo().size()); ++i) {
            cluster->fork(argv[0]);
        }

        cluster->on("error", [](cluster::Worker *worker, const std::string &error) {
            spdlog::error("Worker {} error: {}", worker->id, error);
        });

        cluster->on("disconnect", [](cluster::Worker *worker) {
            spdlog::error("Worker {} disconnected", worker->id);
        });

        cluster->on("message", [](cluster::Worker *worker, const std::string &message) {
            spdlog::info("Message from {}: {}", worker->id, message);
        });

        cluster->on("online", [](cluster::Worker *worker) {
            spdlog::info("Worker {} online", worker->id);

            // Start the server.
            worker->send("action|start");
        });

        cluster->on("exit", [&cluster, argv](const cluster::Worker *worker, int64_t exit_status, int term_signal) {
            spdlog::error("Worker {} exited with status {} and signal {}", worker->id, exit_status, term_signal);

            // If we on developing mode and the worker is exiting, we exit the primary.
            if (config::dev) {
                std::exit(EXIT_SUCCESS);
            }

            cluster->fork(argv[0]);
        });
    }
    else {
        cluster->on("error", [](const std::string &error) {
            spdlog::error("Worker error: {}", error);
        });

        cluster->on("disconnect", []() {
            spdlog::error("Worker disconnected from primary!");
        });

        cluster->on("message", [&](const std::string &message) {
            if (message.find("action|start") != std::string::npos) {
                spdlog::info("Growtopia private server starting..");

                http = new http::HTTP{ *loop };
                http->listen("0.0.0.0", 80);

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

                server = new server::Server{};
                if (!server->initialize(config::server::start_port, config::server::max_peer)) {
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
    delete cluster;
    sinks.clear();
    delete http;
    delete database::get_database();
    delete items::get_items_db();
    delete server;
    return EXIT_SUCCESS;
}