#pragma once

#ifdef _WIN32
#define NOMINMAX
#endif

#include <cstdint>
#include <sqlpp11/table.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/column_types.h>

namespace worlds_ {
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

    struct Width {
        struct _alias_t {
            static constexpr const char _literal[] = "width";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template <typename T>
            struct _member_t {
                T width;
                T& operator()() {
                    return width;
                }

                const T& operator()() const {
                    return width;
                }
            };
        };

        using _traits = ::sqlpp::make_traits<::sqlpp::integral>;
    };

    struct Height {
        struct _alias_t {
            static constexpr const char _literal[] = "height";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template <typename T>
            struct _member_t {
                T height;
                T& operator()() {
                    return height;
                }

                const T& operator()() const {
                    return height;
                }
            };
        };

        using _traits = ::sqlpp::make_traits<::sqlpp::integral>;
    };

    struct DecompressedSizeData {
        struct _alias_t {
            static constexpr const char _literal[] = "decompressed_size_data";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template <typename T>
            struct _member_t {
                T decompressed_size_data;
                T& operator()() {
                    return decompressed_size_data;
                }

                const T& operator()() const {
                    return decompressed_size_data;
                }
            };
        };

        using _traits = ::sqlpp::make_traits<::sqlpp::integral>;
    };

    struct CompressedSizeData {
        struct _alias_t {
            static constexpr const char _literal[] = "compressed_size_data";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template <typename T>
            struct _member_t {
                T compressed_size_data;
                T& operator()() {
                    return compressed_size_data;
                }

                const T& operator()() const {
                    return compressed_size_data;
                }
            };
        };

        using _traits = ::sqlpp::make_traits<::sqlpp::integral>;
    };

    struct Data {
        struct _alias_t {
            static constexpr const char _literal[] = "data";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template <typename T>
            struct _member_t {
                T data;
                T& operator()() {
                    return data;
                }

                const T& operator()() const {
                    return data;
                }
            };
        };

        using _traits = ::sqlpp::make_traits<::sqlpp::blob>;
    };
}

struct Worlds : sqlpp::table_t<Worlds, worlds_::Name, worlds_::Width, worlds_::Height, worlds_::DecompressedSizeData, worlds_::CompressedSizeData, worlds_::Data> {
    struct _alias_t {
        static constexpr const char _literal[] = "worlds";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T>
        struct _member_t {
            T worlds;
            T& operator()() {
                return worlds;
            }
            const T& operator()() const {
                return worlds;
            }
        };
    };
};
