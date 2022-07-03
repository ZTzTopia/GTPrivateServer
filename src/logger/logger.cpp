#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "logger.h"

namespace logger {
    bool Logger::init()
    {
        try {
            std::vector<spdlog::sink_ptr> sinks;
            sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
            sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("private_server.log", 1024 * 1024 * 5, 16));

            auto combined_logger{ std::make_shared<spdlog::logger>("GTPrivateServer", sinks.begin(), sinks.end()) };
            combined_logger->set_level(spdlog::level::trace);

            spdlog::register_logger(combined_logger);
            spdlog::set_default_logger(combined_logger);
        }
        catch (const spdlog::spdlog_ex& ex) {
            spdlog::error("Log initialization failed: {}", ex.what());
            return false;
        }

        return true;
    }
}
