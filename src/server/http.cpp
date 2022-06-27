#include <thread>
#include <spdlog/spdlog.h>

#include "http.h"
#include "../utils/text_parse.h"

namespace server {
    Http::Http(std::shared_ptr<Config> config) : m_config{ std::move(config) }
    {
        std::ofstream cert_file{ "./cert.pem" };
        cert_file << cert;
        cert_file.close();

        std::ofstream key_file{ "./key.pem" };
        key_file << key;
        key_file.close();

        m_server = std::make_unique<httplib::SSLServer>("./cert.pem", "./key.pem");

        std::remove("./cert.pem");
        std::remove("./key.pem");
    }

    Http::~Http()
    {
        stop();
    }

    bool Http::bind_to_port(const std::string& host, int port)
    {
        spdlog::info("HTTP(s) server listening on port {}.", port); // So we don't need to store port in a member variable.
        return m_server->bind_to_port(host.c_str(), port);
    }

    void Http::listen_after_bind()
    {
        std::thread{ &Http::listen_internal, this }.detach();
    }

    bool Http::listen(const std::string& host, int port)
    {
        if (!bind_to_port(host, port)) {
            return false;
        }

        listen_after_bind();
        return true;
    }

    void Http::stop()
    {
        m_server->stop();
    }

    void Http::listen_internal()
    {
        m_server->set_logger([](const httplib::Request& req, const httplib::Response& res) {
            spdlog::info("{} {} {}", req.method, req.path, res.status);

            if (!req.headers.empty()) {
                spdlog::debug("Headers:");
                for (auto& header : req.headers) {
                    spdlog::debug("\t{}: {}", header.first, header.second);
                }
            }

            if (!req.params.empty()) {
                spdlog::debug("Params:");
                spdlog::debug("\t{}", httplib::detail::params_to_query_str(req.params));
            }
        });

        m_server->set_error_handler([](const httplib::Request& req, httplib::Response& res) {
            res.set_content(fmt::format("Hello, world! {} ({})", httplib::detail::status_message(res.status), res.status), "text/plain");
        });

        m_server->set_exception_handler([](const httplib::Request& req, httplib::Response& res, std::exception& ex) {
            res.status = 500;
            res.set_content(fmt::format("Hello, world! {}", ex.what()), "text/plain");
        });

        m_server->Post("/growtopia/server_data.php", [&](const httplib::Request& req, httplib::Response& res) {
            if (req.params.empty() || req.get_header_value("User-Agent").find("UbiServices_SDK") == std::string::npos) {
                res.status = 403;
                return;
            }

            utils::TextParse text_parse{};
            text_parse.add("server", "127.0.0.1");
            text_parse.add<uint16_t>("port", 16999);
            text_parse.add<uint8_t>("type", 1);
            text_parse.add("#maint", "Server is under maintenance. We will be back online shortly. Thank you for your patience!");
            text_parse.add<uint8_t>("type2", 1);
            text_parse.add("meta", "e4a8RCkV3YYd6hQwYtZeVVgd66juXERH");

            res.set_content(fmt::format("{}\r\nRTENDMARKERBS1001\r\n", text_parse.get_all_raw()), "text/html");
            return;
        });

        m_server->listen_after_bind();
    }
}
