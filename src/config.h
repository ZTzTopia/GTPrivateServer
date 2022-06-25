#pragma once
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

class Config {
public:
    Config() : m_server{} {};
    ~Config() = default;

    void default_config()
    {
        m_server.max_players = 32;
        m_server.login_per_second = 1;
    }

    bool create(const std::string& file)
    {
        default_config();

        nlohmann::json j{};
        j["server"] = {
            { "max_players", m_server.max_players },
            { "login_per_second", m_server.login_per_second }
        };

        std::ofstream ofs{ file };
        if (!ofs.is_open()) {
            spdlog::error("Failed to open config file.");
            return false;
        }

        ofs << std::setw(4) << j;
        ofs.close();
        return true;
    }

    bool load(const std::string& file)
    {
        std::ifstream ifs{ file };
        if (!ifs.is_open()) {
            return create(file);
        }

        nlohmann::json j{};
        ifs >> j;
        ifs.close();

        try {
            m_server.max_players = j["server"]["max_players"].get<uint16_t>();
            m_server.login_per_second = j["server"]["login_per_second"].get<uint8_t>();
        }
        catch (const nlohmann::json::exception& ex) {
            spdlog::error("{}", ex.what());
            return false;
        }

        return true;
    }

public:
    struct {
        uint16_t max_players;
        uint8_t login_per_second;
    } m_server;
};