#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <uvw/loop.h>
#include <uvw/util.h>

#include "config.h"
#include "cluster/cluster.h"
#include "include/backward-cpp/backward.hpp"
#include "items/itemsdb.h"

void start_server(std::shared_ptr<uvw::Loop> loop); // From server.cpp
void start_server_gateway(std::shared_ptr<uvw::Loop> loop); // From servergateway.cpp
void init_spdlog(bool is_primary);

int main(int argc, char *argv[]) {
#if (!defined(_WIN32) || defined(__MINGW32__)) && !defined(__linux__)
    std::cout << "Other platforms not tested." << std::endl;
    return EXIT_FAILURE;
#endif

    auto loop = uvw::Loop::getDefault();

    if (config::dev) {
        cluster::Cluster cluster{ loop };

        init_spdlog(cluster.is_primary());

        if (cluster.is_primary()) {
            spdlog::info("Number of cores: {}", uvw::Utilities::cpuInfo().size());
            spdlog::info("Master {} is running", uvw::Utilities::OS::pid());

            for (uint8_t i = 0; i < uvw::Utilities::cpuInfo().size(); ++i) {
                cluster.fork(argv[0]);
            }

            cluster.on("error", [](cluster::Worker *worker, const std::string &error) {
                spdlog::error("Worker {} error: {}", worker->id, error);
            });

            cluster.on("disconnect", [](cluster::Worker *worker) {
                spdlog::error("Worker {} disconnected", worker->id);
            });

            cluster.on("message", [](cluster::Worker *worker, const std::string &message) {
                spdlog::info("Message from {}: {}", worker->id, message);
            });

            cluster.on("online", [](cluster::Worker *worker) {
                spdlog::info("Worker {} online", worker->id);
                assert(worker->send("Hello from master!"));
            });

            cluster.on("exit", [&cluster, argv](const cluster::Worker *worker, int64_t exit_status, int term_signal) {
                spdlog::error("Worker {} exited with status {} and signal {}", worker->id, exit_status, term_signal);
                // cluster.fork(argv[0]);
            });

            loop->run();
            return EXIT_SUCCESS;
        }

        cluster.on("error", [](const std::string &error) {
            spdlog::error("Worker error: {}", error);
        });

        cluster.on("disconnect", []() {
            spdlog::error("Worker disconnected");
        });

        cluster.on("message", [](const std::string &message) {
            spdlog::info("Message from parent: {}", message);
        });

        assert(cluster.send("Hello from worker!"));

        loop->run();
        return EXIT_SUCCESS;
    }

    // start_server(loop);
    loop->run();
    return EXIT_SUCCESS;
}

void init_spdlog(bool is_primary = true) {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(fmt::format("{}server{}.log", config::data::log, is_primary ? "" : "_worker"), 1024 * 1024, 8)); // Using the port??
    auto logger = std::make_shared<spdlog::logger>("server", sinks.begin(), sinks.end());
    logger->set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");
    logger->set_level(config::debug ? spdlog::level::debug : spdlog::level::info);
    logger->flush_on(spdlog::level::debug);
    spdlog::set_default_logger(logger);
}