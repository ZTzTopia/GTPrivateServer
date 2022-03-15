#include <string>

#include "enetserver.h"

namespace enetwrapper {
    ENetServer::ENetServer() : m_host(nullptr) {
        m_running.store(false);
    }

    ENetServer::~ENetServer() {
        if (!m_running.load()) {
            return;
        }

        m_running.store(false);
        m_service_thread.join();

        ENetPeer *currentPeer;
        for (currentPeer = m_host->peers; currentPeer < &m_host->peers[m_host->peerCount]; ++currentPeer) {
            enet_peer_disconnect_now(currentPeer, 0);
        }

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
        m_service_thread = std::move(thread);
    }

    void ENetServer::service_thread() {
        m_running.store(true);
        while (m_running.load()) {
            ENetEvent event;
            if (enet_host_service(m_host, &event, 0) > 0) {
                switch (event.type) {
                    case ENET_EVENT_TYPE_CONNECT:
                        on_connect(event.peer);
                        break;
                    case ENET_EVENT_TYPE_RECEIVE:
                        on_receive(event.peer, event.packet);
                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        on_disconnect(event.peer);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
