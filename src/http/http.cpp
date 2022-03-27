#include <iostream>
#include <memory>
#include <spdlog/fmt/fmt.h>

#include "http.h"
#include "../config.h"

namespace http {
    HTTP::HTTP(uvw::Loop &loop)
        : m_server()
    {
        m_server = loop.resource<uvw::TCPHandle>();
        m_server->simultaneousAccepts();
    }

    HTTP::~HTTP() {
        m_server->close();
    }

    void HTTP::listen(const std::string &host, int port) {
        m_server->bind(host, port);
        m_server->listen();

        m_server->on<uvw::ListenEvent>([](const uvw::ListenEvent &, uvw::TCPHandle &handle) {
            std::shared_ptr<uvw::TCPHandle> client = handle.loop().resource<uvw::TCPHandle>();

            client->on<uvw::DataEvent>([](const uvw::DataEvent &data_event, uvw::TCPHandle &client) {
                std::string http_request(data_event.data.get(), data_event.length);
                if (http_request.find("/growtopia/server_data.php") == std::string::npos) {
                    client.close();
                    return;
                }

                std::string host = std::string{ config::server::host };
                std::string body{
                    fmt::format(
                        "server|{}\r\n"
                        "port|{}\r\n"
                        "type|1\r\n"
                        "#maint|Server is under maintenance. We will be back online shortly. Thank you for your patience!\r\n"
                        "type2|1\r\n" // Tell client to use new packet.
                        "meta|defined\r\n"
                        "RTENDMARKERBS1001",
                        host,
                        17000
                    )
                };

                std::string http_header{
                    fmt::format(
                        "HTTP/1.0 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: {}\r\n"
                        "\r\n",
                        body.length()
                    )
                };

                std::string http_body{
                    fmt::format(
                        "{}"
                        "\r\n"
                        "{}",
                        http_header,
                        body
                    )
                };

                client.write(const_cast<char *>(http_body.c_str()), http_body.length());
                client.close();
            });

            handle.accept(*client);
            client->read();
        });
    }
}