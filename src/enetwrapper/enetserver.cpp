#include <string>

#include "enetserver.h"

namespace gtps {
    ENetServer::ENetServer() : m_host(nullptr) {}

    ENetServer::~ENetServer() {}

    int ENetServer::one_time_init() {
#ifdef _WIN32
        if (enet_initialize() != 0) {
            return -1;
        }

        atexit(enet_deinitialize);
#endif
        return 0;
    }

    int ENetServer::create_host(enet_uint16 port, size_t peer_count) {
        if (m_host) {
			enet_host_destroy(m_host);
			m_host = nullptr;
		}

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = port;

        m_host = enet_host_create(nullptr, peer_count, 2, 0, 0);
        if (m_host == nullptr) {
            return -1;
        }

        m_host->checksum = enet_crc32;
		if (enet_host_compress_with_range_coder(m_host) != 0) {
			return -2;
		}

        return 0;
    }
}
