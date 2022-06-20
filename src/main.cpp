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

    while (server.get()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}