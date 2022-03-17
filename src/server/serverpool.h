#pragma once
#include <string>
#include <vector>
#include <enet/enet.h>

#include "server.h"

namespace server {
    class ServerPool {
    public:
        ServerPool();
        ~ServerPool();

        void start();
        void pool_thread();

        void new_server(enet_uint16 port, size_t max_peer);
        void add_server(Server *server);
        void remove_server(Server *server);
        void remove_server(int index);

        Server *get_server(int index);
        std::vector<Server *> get_servers();
        size_t get_server_count();

    private:
        std::vector<Server *> m_servers;
        std::thread m_pool_thread;
        std::atomic<bool> m_running;
    };

    inline ServerPool *get_server_pool() {
        static auto *server_pool = new ServerPool{};
        return server_pool;
    }
}
