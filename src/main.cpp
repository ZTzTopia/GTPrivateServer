#include <csignal>
#include <thread>
#include <spdlog/spdlog.h>

#include "enetwrapper/enetserver.h"
#include "http/http.h"
#include "server/serverpool.h"

std::atomic<bool> running{ true };

int main() {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%dT%TZ] [%n] [%^%l%$] [thread %t] %v");

    spdlog::info("Growtopia Private Server.");

    std::thread http{ http::HTTP::create_server_data, std::ref(running) };
    http.detach();

    if (enetwrapper::ENetServer::one_time_init() != 0) {
        spdlog::error("Failed to initialize ENet.");
        return EXIT_FAILURE;
    }

    server::get_server_pool()->new_server();

    const auto signal_handler{
        [](int sig) {
            running.store(false);
        }
    };

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    while (running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    http.join();

    return EXIT_SUCCESS;
}
