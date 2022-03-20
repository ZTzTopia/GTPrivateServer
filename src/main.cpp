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

int main() {
#ifndef _WIN32
    std::cout << "Other platforms not tested." << std::endl;
    return EXIT_FAILURE;
#endif

    // ==================================================================================================
    // spd log

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("server.log", 1024 * 1024, 8));
    auto logger = std::make_shared<spdlog::logger>("server", sinks.begin(), sinks.end());
    logger->set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::debug);
    spdlog::set_default_logger(logger);

    spdlog::info("Growtopia Private Server.");

    // ==================================================================================================
    // Signal handler

    const auto signal_handler{
        [](int sig) {
            spdlog::info("Received signal {}. Exiting.", sig);
            running.store(false);

            if (sig != SIGINT) {
                backward::TraceResolver tr;
                backward::StackTrace st;
                st.load_here(32);
                tr.load_stacktrace(st);
                for (size_t i = 0; i < st.size(); ++i) {
                    backward::ResolvedTrace trace = tr.resolve(st[i]);
                    spdlog::error("#{} {} {} [{}]", i, trace.object_filename, trace.object_function, trace.addr);
                }
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

    // ==================================================================================================
    // Exit thread

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

    // ==================================================================================================
    // Database

    database::get_database()->connect();

    // ==================================================================================================
    // Item database

    if (items::get_items_db()->init() != 0) {
        spdlog::error("Failed to load items database.");
        return EXIT_FAILURE;
    }

    // ==================================================================================================
    // Server gateway + Sub server

    if (enetwrapper::ENetServer::one_time_init() != 0) {
        spdlog::error("Failed to initialize ENet.");
        return EXIT_FAILURE;
    }

    std::vector<server::ServerGateway *> servers_gateway{};
    for (int i = 1; i < config::server_gateway::count; i++) {
        servers_gateway.emplace_back(new server::ServerGateway{ static_cast<enet_uint16>(config::server_gateway::port + i), config::server_gateway::max_peer });
    }

    // Growtopia error log about server:
    // `4OOPS: ``Too many people logging in at once. Please click `5CANCEL ``and try again in a few seconds. 
    // `4 SERVER OVERLOADED : ``Sorry, our servers are currently at max capacity with 80000 online, please try again later. We are working to improve this! 
    // Server is currently initializing or re-syncing with sub-server. Please try again in a minute. (ok is server restart?) 
    // Sub-server 1 is currently experiencing high load. Please try again in 30 seconds, hopefully load balancing will fix it.
    // The system is currently experiencing high loads and is not allowing server moves, please try again later.
    // Sub-server 1 (where WORLD is located) is currently lagged out, please try again later.

    /*
     * So I figured out how growtopia sub-servers work so I think he creating a new sub-server in the same computer,
     * probably growtopia has a 4-6 computer. So yes I checked server stats with two different sub-servers (range 20-60)
     * I got the same server load as in sub-server 180 was 27.81, 26.10, 26.71 and in sub-server 164 I got the same server load
     * 27.81, 26.10, 26.71.
     * Ok so my way is right 🤣.
     */

    for (int i = 1; i < config::server_game::count; i++) {
        server::get_server_pool()->new_server(config::server_game::port + i + 1, config::server_game::max_peer);
        server::get_server_pool()->start();
    }

    // ==================================================================================================
    // HTTP server data

    std::thread http{ http::HTTP::create_server_data, std::ref(running), config::server_gateway::port, config::server_gateway::count };

    // ==================================================================================================
    // Main loop

    while (running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ==================================================================================================
    // Cleanup

    spdlog::info("Cleaning up..");

    // TODO: Move cleanup to signal handler.
    delete database::get_database();

    if (items::get_items_db()) {
        delete items::get_items_db();
    }

    for (auto &server_gateway : servers_gateway) {
        delete server_gateway;
    }

    if (server::get_server_pool()) {
        delete server::get_server_pool();
    }

    exit_thread.join();
    http.join();

    return EXIT_SUCCESS;
}
