#pragma once
#include <string>
#include <uvw/tcp.h>

namespace http {
    class HTTP {
    public:
        explicit HTTP(uvw::Loop &loop);
        ~HTTP();

        void listen(const std::string &host, int port);

    private:
        std::shared_ptr<uvw::TCPHandle> m_server;
    };
}