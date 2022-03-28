#pragma once
#include <string>
#include <uvw/tcp.h>

namespace http {
    class HTTP {
    public:
        explicit HTTP(uvw::Loop &loop);
        ~HTTP();

        void listen(const std::string &host, int port);

        // server_data.php stuff.
        void set_server_data(const std::string &host, uint16_t port) {
            m_host = host;
            m_port = port;
        }

    private:
        std::shared_ptr<uvw::TCPHandle> m_server;

        // server_data.php stuff.
        std::string m_host;
        uint16_t m_port;
    };
}