#pragma once
#include <string>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/connection.h>
#include <sqlpp11/postgresql/exception.h>

#include "player/accounts_.h"

namespace database {
    class Postgres {
    public:
        Postgres() = default;
        ~Postgres() = default;

        bool connect(const std::string& host, uint32_t port);

        void prepare_statements();

    public:
        std::shared_ptr<sqlpp::postgresql::connection> db() { return m_db; }

        template <typename T>
        auto operator()(const T& t) -> decltype(this->db()->operator()(t)) { return this->db()->operator()(t); }

        template <typename T>
        auto prepare(const T& t) -> decltype(this->db()->prepare(t)) { return this->db()->prepare(t); }

    private:
        std::shared_ptr<sqlpp::postgresql::connection> m_db;

        template <typename T>
        using PreparedStatement = decltype(static_cast<sqlpp::postgresql::connection*>(nullptr)->prepare(static_cast<T&&>(T())));

    public:
        struct {
            std::unique_ptr<PreparedStatement<player::AccountsInsert>> accounts_i;
            std::unique_ptr<PreparedStatement<player::AccountsSelectPassword>> accounts_s_password;
            std::unique_ptr<PreparedStatement<player::AccountsSelectAll>> accounts_s_all;
        } m_player;
    };
}
