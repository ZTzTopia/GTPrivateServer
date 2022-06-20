#include <spdlog/spdlog.h>

#include "config.h"
#include "logger/logger.h"
#include "server/server.h"

int main(int argc, char *argv[])
{
    if (!logger::Logger::init()) {
        return 1;
    }

    spdlog::info("Starting Growtopia private server v{}...", "0.1.0");

    auto config{ std::make_shared<Config>() };
    if (!config->load("./config.json")) {
        return 1;
    }

    auto server{ std::make_shared<server::Server>(config) };
    if (!server->start()) {
        spdlog::error("Failed to start server!");
        return 1;
    }

    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

#if 0
    cluster::Primary *primary{};
    http::HTTP *http{};
    cluster::Child *child{};

    // Initialize libuv event loop.
    auto loop = uvw::Loop::getDefault();

    // Initialize logger.
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(fmt::format("{}server{}.log", config::data::log, cluster::Cluster::is_primary() ? "" : "_worker"), 1024 * 1024, 8)); // Using the port??

    auto logger = std::make_shared<spdlog::logger>(fmt::format("{}server{}.log", config::data::log, cluster::Cluster::is_primary() ? "" : "_worker"), sinks.begin(), sinks.end());
    logger->set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");
    logger->set_level(config::debug ? spdlog::level::debug : spdlog::level::info);
    logger->flush_on(spdlog::level::debug);

    spdlog::set_default_logger(logger);

    // Primary and workers program.
    if (cluster::Cluster::is_primary()) {
        spdlog::info("Primary {} is running", uvw::Utilities::OS::pid());
        spdlog::info("Number of cores: {}", uvw::Utilities::cpuInfo().size());

        primary = new cluster::Primary{ loop, config::dev ? false : true };

        primary->on("online", [](cluster::Worker *worker) {
            spdlog::info("Worker {} online", worker->id());

            static int port_offset = 0;
            worker->send(fmt::format("action|start_server\nport_offset|{}\n", port_offset++));
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
                return;
            }

            loop->stop();
        });

        for (int i = 0; i < (config::dev ? 1 : uvw::Utilities::cpuInfo().size()); i++) {
            primary->fork(std::string{ argv[0] });
        }

        http = new http::HTTP{ loop };
        http->listen("0.0.0.0", 80);
        http->set_server_data(std::string{ config::server::host }, config::server::start_port + (config::dev ? 1 : uvw::Utilities::cpuInfo().size()));
    }
    else {
        child = new cluster::Child{ loop };

        child->on("message", [&](const std::string &message) {
            if (message.find("action|start_server") != std::string::npos) {
                spdlog::info("Growtopia private server starting..");

                auto database = std::make_shared<database::Database>();
                if (!database->redis_connect()) {
                    loop->stop();
                    return;
                }

                if (!database->mysql_connect()) {
                    loop->stop();
                    return;
                }

                // child->worker()->send(fmt::format("action|server_started\nport_offset|{}", port_offset));
            }
        });
    }

    loop->run();
    loop->close();

    delete primary;
    delete http;
    delete child;
#endif
    return 0;
}