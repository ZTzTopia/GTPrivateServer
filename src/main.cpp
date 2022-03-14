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
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");

    spdlog::info("Growtopia Private Server.");

    // ==================================================================================================

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

    items::get_items_db()->init();

    // ==================================================================================================

    std::thread http{ http::HTTP::create_server_data, std::ref(running) };

    // ==================================================================================================

    if (enetwrapper::ENetServer::one_time_init() != 0) {
        spdlog::error("Failed to initialize ENet.");
        return EXIT_FAILURE;
    }

    server::get_server_pool()->new_server();

    // ==================================================================================================

    const auto signal_handler{
        [](int sig) {
            spdlog::info("Received signal {}. Exiting.", sig);
            running.store(false);
        }
    };

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // ==================================================================================================

    while (running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ==================================================================================================

    spdlog::info("Shutting down...");

    exit_thread.join();
    http.join();

    return EXIT_SUCCESS;
}
