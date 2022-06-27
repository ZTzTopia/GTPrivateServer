#include <enet/enet.h>
#include <spdlog/spdlog.h>

#include "enet_wrapper.h"

namespace enetwrapper {
    bool ENetWrapper::one_time_init() {
        spdlog::info("Starting ENet v{}.{}.{}...", ENET_VERSION_MAJOR, ENET_VERSION_MINOR, ENET_VERSION_PATCH);
#ifdef _WIN32
        if (enet_initialize() != 0)
            return false;

        atexit(enet_deinitialize);
#endif
        return true;
    }
}
