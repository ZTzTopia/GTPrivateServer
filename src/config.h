#pragma once
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

class Config {
public:
    Config() : debug{ false }, dev{ false } {}
    ~Config() = default;

    void default_config()
    {
        debug = true;
        dev = false;
    }

    bool create(const std::string& file)
    {
        default_config();

        nlohmann::json j{};
        j["debug"] = debug;
        j["dev"] = dev;

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
            debug = j["debug"];
            dev = j["dev"];
        }
        catch (const nlohmann::json::exception& ex) {
            spdlog::error("{}", ex.what());
            return false;
        }

        return true;
    }

public:
    bool debug;
    bool dev;
};