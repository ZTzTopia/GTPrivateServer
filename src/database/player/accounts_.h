#pragma once
#include "accounts.h"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/connection.h>

namespace player {
    inline const auto accounts_insert = []() {
        player::PlayerAccounts account{};
        return insert_into(account).set(account.name = parameter(account.name),
                                        account.guest = parameter(account.guest),
                                        account.registerIp = parameter(account.registerIp),
                                        account.loginIp = parameter(account.loginIp),
                                        account.mac = parameter(account.mac),
                                        account.password = parameter(account.password),
                                        account.rid = parameter(account.rid),
                                        account.wk = parameter(account.wk));
    };

    using AccountsInsert = decltype(accounts_insert());

    inline const auto accounts_select_password = []() {
        player::PlayerAccounts account{};
        return select(account.password).from(account).where(account.name == parameter(account.name) || account.rid == parameter(account.rid));
    };

    using AccountsSelectPassword = decltype(accounts_select_password());

    inline const auto accounts_select_all = []() {
        player::PlayerAccounts account{};
        return select(sqlpp::all_of(account)).from(account).where(account.name == parameter(account.name) || account.rid == parameter(account.rid));
    };

    using AccountsSelectAll = decltype(accounts_select_all());
}
