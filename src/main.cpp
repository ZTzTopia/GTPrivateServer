#include <iostream>
#include <csignal>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "config.h"
#include "database/database.h"
#include "enetwrapper/enetserver.h"
#include "http/http.h"
#include "include/backtrace-cpp/backtrace.hpp"
#include "items/itemsdb.h"
#include "server/serverpool.h"
#include "server/servergateway.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

static std::atomic<bool> running{ true };
static std::function<void()> shutdown_callback;

int main() {
#ifndef _WIN32
    std::cout << "Other platforms not tested." << std::endl;
    return EXIT_FAILURE;
#endif

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("server.log", 1024 * 1024, 8));
    auto logger = std::make_shared<spdlog::logger>("server", sinks.begin(), sinks.end());
    logger->set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");
    logger->set_level(config::dev ? spdlog::level::debug : spdlog::level::info);
    logger->flush_on(spdlog::level::debug);
    spdlog::set_default_logger(logger);

    spdlog::info("Growtopia private server starting..");

    const auto signal_handler{
        [](int sig) {
            spdlog::info("Received signal {}. Exiting.", sig);

            if (sig != SIGINT) {
                shutdown_callback();

                backward::TraceResolver tr;
                backward::StackTrace st;
                st.load_here(32);
                tr.load_stacktrace(st);
                for (size_t i = 0; i < st.size(); ++i) {
                    backward::ResolvedTrace trace = tr.resolve(st[i]);
                    spdlog::error("#{} {} {} [{}]", i, trace.object_filename, trace.object_function, trace.addr);
                }
            }
            else {
                running.store(false);
            }
        }
    };

    std::signal(SIGILL, signal_handler);
    std::signal(SIGFPE, signal_handler);
    std::signal(SIGSEGV, signal_handler);
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGBREAK, signal_handler);
    std::signal(SIGABRT, signal_handler);
#ifndef _WIN32
    std::signal(SIGHUP, signal_handler);
    std::signal(SIGBUS, signal_handler);
#endif

    if (!database::get_database()->connect()) {
        spdlog::error("Failed to connect to database.");

        delete database::get_database();
        return EXIT_FAILURE;
    }

    if (!items::get_items_db()->initialize()) {
        spdlog::error("Failed to load items database.");

        delete database::get_database();
        delete items::get_items_db();
        return EXIT_FAILURE;
    }

    // TODO: Change me to a good server_data.php.
    std::thread http{ http::HTTP::create_server_data, std::ref(running), config::server_gateway::port, config::server_gateway::count };

    if (!enetwrapper::ENetServer::one_time_init()) {
        spdlog::error("Failed to initialize ENet.");

        delete database::get_database();
        delete items::get_items_db();
        return EXIT_FAILURE;
    }

    // Server gateway should not be here.
    std::vector<server::ServerGateway *> servers_gateway{};
    for (unsigned int i = 1; i <= config::server_gateway::count; i++) {
        spdlog::debug("Starting server gateway {}.", i);
        auto *server_gateway = new server::ServerGateway{};
        if (server_gateway->initialize(static_cast<enet_uint16>(config::server_gateway::port + i), config::server_gateway::max_peer)) {
            servers_gateway.emplace_back(server_gateway);
        }
        else {
            delete server_gateway;
        }
    }

    if (servers_gateway.empty()) {
        spdlog::error("Failed to initialize any server gateway.");
        return EXIT_FAILURE;
    }

    std::vector<std::thread> server_threads(config::server_game::count);
    for (unsigned int i = 1; i <= config::server_game::count; i++) {
        spdlog::debug("Starting {} servers game.", i);

        std::atomic<bool> success{ true };
        std::thread thread([i, &success]() {
            if (!server::get_server_pool()->new_server(config::server_game::port + i, config::server_game::max_peer)) {
                success.store(false);
            }

            while (running.load() && success.load()) {
                for (auto &server : server::get_server_pool()->get_servers()) {
                    server->on_update();
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        });

        if (!success.load()) {
            continue;
        }

        if (config::server_game::use_hardware_concurrency) {
#ifdef _WIN32
            auto mask = (static_cast<DWORD_PTR>(1) << (i - 1));
            DWORD_PTR dw = SetThreadAffinityMask(thread.native_handle(), mask);
            if (dw == 0) {
                DWORD dwErr = GetLastError();
                spdlog::error("Error calling SetThreadAffinityMask: {}", dwErr);
                return EXIT_FAILURE;
            }
#else
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(i - 1, &cpuset);
            int rc = pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpuset);
            if (rc != 0) {
                spdlog::error("Error calling pthread_setaffinity_np: {}", rc);
                return EXIT_FAILURE;
            }
#endif
        }

        server_threads.emplace_back(std::move(thread));
    }

    if (server_threads.empty()) {
        spdlog::error("Failed to initialize any game server.");
        return EXIT_FAILURE;
    }

    std::thread exit_thread{
        [] {
            std::string command;
            while (std::cin >> command) {
                if (command == "exit" || command == "quit" || command == "stop") {
                    running.store(false);
                    break;
                }
            }
        }
    };

    shutdown_callback = [&exit_thread, servers_gateway, &server_threads, &http]{
        spdlog::info("Shutting down..");

        delete database::get_database();
        delete items::get_items_db();

        http.join();

        for (auto &server_gateway : servers_gateway) {
            delete server_gateway;
        }

        if (server::get_server_pool()) {
            delete server::get_server_pool();
        }

        for (auto &thread : server_threads) {
            thread.join();
        }

        exit_thread.join();
    };

    while (running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    shutdown_callback();
    return EXIT_SUCCESS;
}
