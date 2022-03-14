#pragma once
#include <string>
#include <atomic>
#include <WinSock2.h>

namespace http {
    class HTTP {
    public:
        HTTP() = default;
        ~HTTP() = default;

        // TODO: Linux support.
        static void create_server_data(std::atomic<bool> &running) {
            WSADATA wsa_data;
            if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
                return;
            }

            SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sockfd == -1) {
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
                SOCKET new_sockfd = accept(sockfd, (SOCKADDR *)&addr, &sizeof_sin);

                char buffer[1024];
                if (recv(new_sockfd, buffer, sizeof(buffer), 0) > 0) {
                    // Growtopia use http version 1.0.
                    if (strstr(buffer, "HTTP/1.0") != nullptr && strstr(buffer, "POST /growtopia/server_data.php") != nullptr) {
                        // We know growtopia not do like this, this why growtopia named the file is php.
                        std::string http_body{
                            "server|127.0.0.1\r\n"
                            "port|17256\r\n"
                            "type|1\r\n"
                            "#maint|Server is under maintenance. We will be back online shortly. Thank you for your patience!\r\n"
                            "type2|1\r\n" // Tell client to use new packet.
                            "meta|defined\r\n"
                            "RTENDMARKERBS1001"
                        };
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
            WSACleanup();
        }
    };
}