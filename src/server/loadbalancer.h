#pragma once
#include <unordered_map>

#include "server.h"

namespace server {
    class LoadBalancer {
    public:
        LoadBalancer() = default;
        ~LoadBalancer();

        void add_server(Server *server);
        void remove_server(Server *server);

        Server *get_server_with_minimum_player();

    private:
        std::vector<Server *> m_servers;
    };
}