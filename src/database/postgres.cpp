#include <spdlog/spdlog.h>

#include "postgres.h"

namespace database {
    bool Postgres::connect(const std::string& host, uint32_t port)
    {
        auto config{ std::make_shared<sqlpp::postgresql::connection_config>() };
        config->host = "localhost";
        config->user = "postgres";
        config->dbname = "gtps";
        config->debug = true;
        try {
            m_db = std::make_shared<sqlpp::postgresql::connection>(config);
            spdlog::info("Connected to postgres database.");
        }
        catch (const sqlpp::postgresql::broken_connection& ex) {
            spdlog::critical("{}", ex.what());
            return false;
        }

        prepare_statements();
        return true;
    }

    void Postgres::prepare_statements()
    {
        m_player.accounts_i = std::make_unique<PreparedStatement<player::AccountsInsert>>(prepare(player::accounts_insert()));
        m_player.accounts_s_password = std::make_unique<PreparedStatement<player::AccountsSelectPassword>>(prepare(player::accounts_select_password()));
        m_player.accounts_s_all = std::make_unique<PreparedStatement<player::AccountsSelectAll>>(prepare(player::accounts_select_all()));
    }
}
