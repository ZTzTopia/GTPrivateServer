//#pragma once
//#include "loadbalancer.h"
//
//namespace server {
//    LoadBalancer::~LoadBalancer() {
//        m_servers.clear();
//    }
//
//    void LoadBalancer::add_server(Server *server) {
//        if (std::find(m_servers.begin(), m_servers.end(), server) != m_servers.end()) {
//            return;
//        }
//
//        m_servers.emplace_back(server);
//    }
//
//    void LoadBalancer::remove_server(Server *server) {
//        m_servers.erase(std::remove(m_servers.begin(), m_servers.end(), server), m_servers.end());
//        m_servers.shrink_to_fit();
//    }
//
//    Server *LoadBalancer::get_server_with_minimum_player() {
//        size_t least_player_count = m_servers[0]->get_peer_count();
//        int least_player_count_index = 0;
//
//        for (int i = 0; i < m_servers.size(); i++) {
//            if (m_servers[i]->get_peer_count() >= m_servers[i]->get_max_peer()) {
//                continue;
//            }
//
//            if (m_servers[i]->get_peer_count() < least_player_count) {
//                least_player_count = m_servers[i]->get_peer_count();
//                least_player_count_index = i;
//            }
//        }
//
//        return m_servers[least_player_count_index];
//    }
//}