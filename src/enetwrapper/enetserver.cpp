#include <string>
#include <thread>

#include "enetserver.h"
#include "../server/serverpool.h"

namespace enetwrapper {
    ENetServer::ENetServer() : m_host(nullptr) {}

    ENetServer::~ENetServer() {
        if (m_host) {
            enet_host_destroy(m_host);
        }
    }

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

        m_host = enet_host_create(&address, peer_count, 2, 0, 0);
        if (m_host == nullptr) {
            return -1;
        }

        // m_host->usingNewPacket = 1;
        m_host->usingNewPacketForServer = 1;
        m_host->checksum = enet_crc32;
		if (enet_host_compress_with_range_coder(m_host) != 0) {
			return -2;
		}

        return 0;
    }

    void ENetServer::start_service() {
        std::thread thread{ &ENetServer::service_thread, this };
        thread.detach();
    }

    void ENetServer::service_thread() {
        while (true) {
            ENetEvent event;
            if (enet_host_service(m_host, &event, 1000) > 0) {
                switch (event.type) {
                    case ENET_EVENT_TYPE_CONNECT:
                        server::get_server_pool()->on_connect(event.peer);
                        break;
                    case ENET_EVENT_TYPE_RECEIVE:
                        server::get_server_pool()->on_receive(event.peer, event.packet);
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        server::get_server_pool()->on_disconnect(event.peer);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
