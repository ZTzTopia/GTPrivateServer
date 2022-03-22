#pragma once
#include <cstdint>
#include <sqlpp11/table.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/column_types.h>

namespace database {
    namespace guests_ {
        struct Id {
            struct _alias_t {
                static constexpr const char _literal[] = "id";
                using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
                template <typename T>
                struct _member_t {
                    T id;
                    T& operator()() {
                        return id;
                    }

                    const T& operator()() const {
                        return id;
                    }
                };
            };

            using _traits = ::sqlpp::make_traits<::sqlpp::integral, ::sqlpp::tag::must_not_insert, ::sqlpp::tag::must_not_update>;
        };

        struct Name {
            struct _alias_t {
                static constexpr const char _literal[] = "name";
                using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
                template <typename T>
                struct _member_t {
                    T name;
                    T& operator()() {
                        return name;
                    }

                    const T& operator()() const {
                        return name;
                    }
                };
            };

            using _traits = ::sqlpp::make_traits<::sqlpp::varchar, ::sqlpp::tag::must_not_update>;
        };

        struct Mac {
            struct _alias_t {
                static constexpr const char _literal[] = "mac";
                using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
                template <typename T>
                struct _member_t {
                    T mac;
                    T& operator()() {
                        return mac;
                    }

                    const T& operator()() const {
                        return mac;
                    }
                };
            };

            using _traits = ::sqlpp::make_traits<::sqlpp::varchar, ::sqlpp::tag::must_not_update>;
        };

        struct Token {
            struct _alias_t {
                static constexpr const char _literal[] = "token";
                using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
                template <typename T>
                struct _member_t {
                    T token;
                    T& operator()() {
                        return token;
                    }

                    const T& operator()() const {
                        return token;
                    }
                };
            };

            using _traits = ::sqlpp::make_traits<::sqlpp::integral>;
        };

        struct CurrentWorld {
            struct _alias_t {
                static constexpr const char _literal[] = "current_world";
                using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
                template <typename T>
                struct _member_t {
                    T current_world;
                    T& operator()() {
                        return current_world;
                    }

                    const T& operator()() const {
                        return current_world;
                    }
                };
            };

            using _traits = ::sqlpp::make_traits<::sqlpp::varchar>;
        };
    }

    struct Guests : sqlpp::table_t<Guests, guests_::Id, guests_::Name, guests_::Mac, guests_::Token, guests_::CurrentWorld> {
        struct _alias_t {
            static constexpr const char _literal[] = "guests";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template <typename T>
            struct _member_t {
                T guests;
                T& operator()() {
                    return guests;
                }
                const T& operator()() const {
                    return guests;
                }
            };
        };
    };
}
