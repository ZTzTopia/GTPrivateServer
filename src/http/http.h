#pragma once
#include <string>
#include <atomic>
#ifdef _WIN32
#include <WinSock2.h>
#endif

#include "../utils/random.h"

namespace http {
#ifndef _WIN32
#define INVALID_SOCKET -1
#endif

    class HTTP {
    public:
        HTTP() = default;
        ~HTTP() = default;

        static void create_server_data(std::atomic<bool> &running, enet_uint16 server_gateway_port, uint8_t server_gateway_count) {
#ifdef _WIN32
            WSADATA wsa_data;
            if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
                return;
            }
#endif

#ifdef _WIN32
            SOCKET sockfd;
#else
            int sockfd;
#endif
            sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sockfd == INVALID_SOCKET) {
                return;
            }

            struct sockaddr_in addr = {};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(80);
            addr.sin_addr.S_un.S_addr = INADDR_ANY;

            if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) == -1) {
                closesocket(sockfd);
                return;
            }

            if (listen(sockfd, 32) == -1) {
                closesocket(sockfd);
                return;
            }

            // This not good right??
            while (running.load()) {
                fd_set set = {};
                FD_ZERO(&set);
                FD_SET(sockfd, &set);

                timeval timeout = {};
                timeout.tv_sec = 10;
                timeout.tv_usec = 0;

                int ret = select((int) sockfd + 1, &set, nullptr, nullptr, &timeout);
                if (ret == -1 || ret == 0) {
                    continue;
                }

                int sizeof_sin = sizeof(addr);
#ifdef _WIN32
                SOCKET new_sockfd;
#else
                int new_sockfd;
#endif
                new_sockfd = accept(sockfd, (SOCKADDR *)&addr, &sizeof_sin);

                char buffer[1024];
                if (recv(new_sockfd, buffer, sizeof(buffer), 0) > 0) {
                    if (strstr(buffer, "/growtopia/server_data.php") != nullptr) {
                        std::string http_body{
                            "server|25.38.51.142\r\n"
                            "port|"
                        };

                        http_body.append(std::to_string(server_gateway_port + random::get_generator_static().uniform<enet_uint16>(static_cast<uint8_t>(0), server_gateway_count - 1)));
                        http_body.append("\r\n");
                        http_body.append(
                            "type|1\r\n"
                            "#maint|Server is under maintenance. We will be back online shortly. Thank you for your patience!\r\n"
                            "type2|1\r\n" // Tell client to use new packet.
                            "meta|defined\r\n"
                            "RTENDMARKERBS1001"
                        );

                        std::string http_header{
                            "HTTP/1.0 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Content-Length: "
                        };

                        http_header.append(std::to_string(http_body.size()));
                        http_header.append("\r\n\r\n");
                        http_header.append(http_body);

                        send(new_sockfd, http_header.data(), static_cast<int>(http_header.size()), 0);
                    }
                    else {
                        std::string http_body{
                            "<!DOCTYPE html>\r\n<html>\r\n<body>\r\nHello, World!\r\n</body>\r\n</html>"
                        };

                        std::string http_header{
                            "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Content-Length: "
                        };

                        http_header.append(std::to_string(http_body.size()));
                        http_header.append("\r\n\r\n");
                        http_header.append(http_body);

                        send(new_sockfd, http_header.data(), static_cast<int>(http_header.size()), 0);
                    }
                }

                closesocket(new_sockfd);
            }

            closesocket(sockfd);
#ifdef _WIN32
            WSACleanup();
#endif
        }
    };
}