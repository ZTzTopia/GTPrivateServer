#include <spdlog/spdlog.h>

#include "config.h"
#include "logger/logger.h"
#include "server/server.h"

int main(int argc, char *argv[])
{
    if (!logger::Logger::init()) {
        return 1;
    }

    spdlog::info("Starting Growtopia private server v{}...", GTPRIVATESERVER_VERSION);

    auto config{ std::make_shared<Config>() };
    if (!config->load("./config.json")) {
        return 1;
    }

    auto server{ std::make_unique<server::Server>(config) };
    if (!server->start()) {
        return 1;
    }

    while (server.get()) { // Just to avoid compiler warning.
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
