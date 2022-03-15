#include <iostream>
#include <csignal>
#include <thread>
#include <spdlog/spdlog.h>

#include "enetwrapper/enetserver.h"
#include "http/http.h"
#include "items/itemsdb.h"
#include "server/serverpool.h"
#include "server/servergateway.h"

static std::atomic<bool> running{ true };

int main() {
    // ==================================================================================================
    // spd log

    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");

    spdlog::info("Growtopia Private Server.");

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

    constexpr enet_uint16 server_gateway_port{ 17256 };
    constexpr uint8_t server_gateway_count{ 4 };
    constexpr size_t server_gateway_max_peer{ 32 };

    std::vector<server::ServerGateway *> servers_gateway{};
    for (int i = 0; i < server_gateway_count; i++) {
        servers_gateway.emplace_back(new server::ServerGateway{ static_cast<enet_uint16>(server_gateway_port + i), server_gateway_max_peer });
    }

    // Growtopia error log about server:
    // `4OOPS: ``Too many people logging in at once. Please click `5CANCEL ``and try again in a few seconds.
    // `4 SERVER OVERLOADED : ``Sorry, our servers are currently at max capacity with 80000 online, please try again later. We are working to improve this!
    // Server is currently initializing or re-syncing with sub-server. Please try again in a minute. (ok is server restart?)
    // Sub-server 1 is currently experiencing high load. Please try again in 30 seconds, hopefully load balancing will fix it.
    // The system is currently experiencing high loads and is not allowing server moves, please try again later.

    constexpr enet_uint16 sub_server_port{ static_cast<enet_uint16>(server_gateway_port + server_gateway_count) };
    constexpr uint8_t sub_server_count{ 8 };
    constexpr size_t sub_server_max_peer{ 32 };

    for (int i = 0; i < sub_server_count; i++) {
        server::get_server_pool()->new_server(sub_server_port + i + 1, sub_server_max_peer);
    }

    // ==================================================================================================
    // HTTP server data

    std::thread http{ http::HTTP::create_server_data, std::ref(running), server_gateway_port, server_gateway_count };

    // ==================================================================================================
    // Signal handler

    const auto signal_handler{
        [](int sig) {
            spdlog::info("Received signal {}. Exiting.", sig);
            running.store(false);
        }
    };

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
#ifndef _WIN32
    std::signal(SIGHUP, signal_handler);
#endif

    // ==================================================================================================
    // Main loop

    while (running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ==================================================================================================
    // Cleanup

    spdlog::info("Cleaning up..");

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

    getchar();

    return EXIT_SUCCESS;
}
