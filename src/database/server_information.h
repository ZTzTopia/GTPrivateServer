#pragma once
#include <cstdint>
#include <sqlpp11/table.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/column_types.h>

namespace database {
    namespace server_information_ {
        struct Port {
            struct _alias_t {
                static constexpr const char _literal[] = "port";
                using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
                template <typename T>
                struct _member_t {
                    T port;
                    T& operator()() {
                        return port;
                    }

                    const T& operator()() const {
                        return port;
                    }
                };
            };

            using _traits = ::sqlpp::make_traits<::sqlpp::integral, ::sqlpp::tag::must_not_update>;
        };

        struct Host {
            struct _alias_t {
                static constexpr const char _literal[] = "host";
                using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
                template <typename T>
                struct _member_t {
                    T host;
                    T& operator()() {
                        return host;
                    }

                    const T& operator()() const {
                        return host;
                    }
                };
            };

            using _traits = ::sqlpp::make_traits<::sqlpp::varchar>;
        };

        struct ConnectedPeer {
            struct _alias_t {
                static constexpr const char _literal[] = "connected_peer";
                using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
                template <typename T>
                struct _member_t {
                    T connected_peer;
                    T& operator()() {
                        return connected_peer;
                    }

                    const T& operator()() const {
                        return connected_peer;
                    }
                };
            };

            using _traits = ::sqlpp::make_traits<::sqlpp::integral>;
        };

        struct MaxPeer {
            struct _alias_t {
                static constexpr const char _literal[] = "max_peer";
                using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
                template <typename T>
                struct _member_t {
                    T max_peer;
                    T& operator()() {
                        return max_peer;
                    }

                    const T& operator()() const {
                        return max_peer;
                    }
                };
            };

            using _traits = ::sqlpp::make_traits<::sqlpp::integral>;
        };
    }

    struct ServerInformation : sqlpp::table_t<ServerInformation, server_information_::Port, server_information_::Host, server_information_::ConnectedPeer, server_information_::MaxPeer> {
        struct _alias_t {
            static constexpr const char _literal[] = "server_information";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template <typename T>
            struct _member_t {
                T server_information;
                T& operator()() {
                    return server_information;
                }
                const T& operator()() const {
                    return server_information;
                }
            };
        };
    };
}
