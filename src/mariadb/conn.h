#include <iostream>
#include <mariadb/conncpp.hpp>

#include "../config.h"

namespace mariadb {
    class conn {
    public:
        conn() {
            try {
                static std::string host = std::string{ config::database::host };
                static std::string user = std::string{ config::database::user };
                static std::string password = std::string{ config::database::password };
                static std::string database = std::string{ config::database::database };

                sql::Driver *driver = sql::mariadb::get_driver_instance();
                sql::SQLString url(fmt::format("{}/{}", host, database).c_str());
                m_sql_connection = driver->connect(url, user.c_str(), password.c_str());
            }
            catch(sql::SQLException &e) {
                std::cerr << "Error Connecting to MariaDB Platform: " << e.what() << std::endl;
            }
        }

        ~conn() {
            m_sql_connection->close();
        }

        sql::Connection *get_sql_connection() {
            return m_sql_connection;
        }

    private:
        sql::Connection *m_sql_connection;
    };

    sql::Connection *get_sql_connection() {
        static conn *connection = new conn{};
        return connection->get_sql_connection();
    }
}