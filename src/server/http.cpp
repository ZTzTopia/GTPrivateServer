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

    void Http::bind_to_port(const std::string& host, int port)
    {
        m_server->bind_to_port(host.c_str(), port);
    }

    void Http::listen_after_bind()
    {
        std::thread{ &Http::listen_internal, this }.detach();
    }

    void Http::listen(const std::string& host, int port)
    {
        bind_to_port(host, port);
        listen_after_bind();
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
                spdlog::info("Headers:");
                for (auto& header : req.headers) {
                    spdlog::info("\t{}: {}", header.first, header.second);
                }
            }

            if (!req.params.empty()) {
                spdlog::info("Params:");
                spdlog::info("\t{}", httplib::detail::params_to_query_str(req.params));
            }
        });

        m_server->set_error_handler([](const httplib::Request& req, httplib::Response& res) {
            res.set_content("Hello, world!", "text/plain");
        });

        m_server->Post("/growtopia/server_data.php", [&](const httplib::Request& req, httplib::Response& res) {
            res.set_content("", "text/html");
            return true;
        });

        m_server->listen_after_bind();
    }
}
