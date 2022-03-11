#include <spdlog/spdlog.h>

#include "enetwrapper/enetserver.h"

int main() {
    // For enet new header, since we not use c random generator.
    srand(static_cast<uint32_t>(time(nullptr)) + clock());

    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    spdlog::info("Project GTPS by ztz.");

    if (gtps::ENetServer::one_time_init() != 0) {
        spdlog::error("ENetServer init failed.");
        return 1;
    }

    return 0;
}
