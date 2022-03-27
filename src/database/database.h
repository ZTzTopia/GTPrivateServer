#pragma once

#include <spdlog/spdlog.h>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>

#include "../config.h"
#include "worlds.h"
#include "guests.h"

namespace database {
    class Database {
    public:
        Database() : m_db_connection(nullptr) { sqlpp::mysql::global_library_init(); }
        ~Database() { delete m_db_connection; }

        bool connect() {
            auto config = std::make_shared<sqlpp::mysql::connection_config>();
            config->host = config::database::host;
            config->port = config::database::port;
            config->user = config::database::user;
            config->password = config::database::password;
            config->database = config::database::database;
            config->auto_reconnect = config::database::auto_reconnect;
            config->debug = config::database::debug;

            try {
                m_db_connection = new sqlpp::mysql::connection{ config };
            }
            catch (const sqlpp::exception &e) {
                spdlog::error("{}", e.what());
                return false;
            }

            return true;
        }

        sqlpp::mysql::connection *get_connection() {
            return m_db_connection;
        }

    private:
        sqlpp::mysql::connection *m_db_connection;
    };

    inline Database *get_database() {
        static auto *database = new Database{};
        return database;
    }
}