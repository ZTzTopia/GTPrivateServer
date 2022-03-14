#include <iostream>
#include <csignal>
#include <thread>
#include <spdlog/spdlog.h>

#include "enetwrapper/enetserver.h"
#include "http/http.h"
#include "items/itemsdb.h"
#include "server/serverpool.h"

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
    // HTTP server data

    std::thread http{ http::HTTP::create_server_data, std::ref(running) };

    // ==================================================================================================
    // Server

    if (enetwrapper::ENetServer::one_time_init() != 0) {
        spdlog::error("Failed to initialize ENet.");
        return EXIT_FAILURE;
    }

    server::get_server_pool()->new_server();

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

    // ==================================================================================================
    // Main loop

    while (running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ==================================================================================================
    // Cleanup

    spdlog::info("Shutting down...");

    exit_thread.join();
    http.join();

    return EXIT_SUCCESS;
}
