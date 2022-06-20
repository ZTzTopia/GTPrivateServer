/**
 @file  argon2.h
 @brief argon2 public header file.
 */

#pragma once
#include <random>
#include <string>
#include <iostream>
#include <argon2.h>

namespace argon2 {
    /**
     * The argon2_config struct
     */
    typedef struct Argon2_config {
        argon2_type type;               /**< The type of Argon2 algorithm. */
        argon2_version version;         /**< The version of the Argon2 algorithm. */
        uint32_t memory_cost;           /**< The memory cost, in kibibytes (2^n). */
        uint32_t time_cost;             /**< The number of passes. */
        uint32_t parallelism;           /**< The parallelism degree. */
        std::string secret;             /**< The secret to be used in the hash. */
        std::string ad;                 /**< The additional data to be used in the hash. */
        uint32_t hash_length;           /**< The desired length of the hash. */
        uint32_t salt_length;           /**< The desired length of the salt. */
    } argon2_config;

    typedef struct Argon2_raw {
        argon2_context context;             /**< Internal use. */
        argon2_config config;
        argon2_error_codes error_codes;
        std::string salt;
        std::string hash;
    } argon2_raw;

    typedef struct Argon2_encoded {
        argon2_config config;               /**< Unused. */
        argon2_error_codes error_codes;
        std::string salt;
        std::string encoded;
    } argon2_encoded;

    typedef struct Argon2_verify {
        argon2_config config;               /**< Unused. */
        argon2_error_codes error_codes;
        bool success;
    } argon2_verify;

    class Argon2 {
    public:
        /**
         * Argon2 constructor.
         */
        Argon2() {
            m_config = std::move(default_config());
        }

        argon2_raw hash_raw(const std::string &pwd, const std::string &salt) {
            if (pwd.empty()) {
                return {
                    {},
                    m_config,
                    ARGON2_PWD_TOO_SHORT,
                    std::string{},
                    std::string{}
                };
            }

            std::string new_salt{ salt };
            if (new_salt.empty()) {
                new_salt.resize(m_config.salt_length);

                static std::random_device rd{};
                static std::mt19937 gen{ rd() };
                static std::uniform_int_distribution<> dis(0, 61);

                for (uint32_t i = 0; i < m_config.salt_length; i++) {
                    new_salt[i] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"[dis(gen)];
                }
            }

            auto out = new uint8_t[m_config.hash_length + 1];
            out[m_config.hash_length + 1] = '\0';

            argon2_context context = {
                out,
                static_cast<uint32_t>(m_config.hash_length),
                reinterpret_cast<uint8_t *>(const_cast<char *>(pwd.c_str())),
                static_cast<uint32_t>(pwd.length()),
                reinterpret_cast<uint8_t *>(const_cast<char *>(new_salt.c_str())),
                static_cast<uint32_t>(new_salt.length()),
                reinterpret_cast<uint8_t *>(const_cast<char *>(m_config.secret.c_str())),
                static_cast<uint32_t>(m_config.secret.length()),
                reinterpret_cast<uint8_t *>(const_cast<char *>(m_config.ad.c_str())),
                static_cast<uint32_t>(m_config.ad.length()),
                m_config.time_cost,
                m_config.memory_cost,
                m_config.parallelism,
                m_config.parallelism,
                static_cast<uint32_t>(m_config.version),
                ARGON2_DEFAULT_FLAGS
            };

            int result = argon2_ctx(&context, m_config.type);
            if (result != ARGON2_OK) {
                return {
                    {},
                    m_config,
                    static_cast<argon2_error_codes>(result),
                    std::string{},
                    std::string{}
                };
            }

            return {
                context,
                m_config,
                static_cast<argon2_error_codes>(result),
                new_salt,
                std::string(reinterpret_cast<const char *>(out), m_config.hash_length)
            };
        }

        argon2_raw hash_raw(const std::string &pwd) {
            return hash_raw(pwd, std::string{ "" });
        }

        argon2_encoded hash_encoded(const std::string &pwd, const std::string &salt) {
            argon2_raw raw{ hash_raw(pwd, salt) };
            if (raw.error_codes != ARGON2_OK) {
                return {
                    raw.config,
                    raw.error_codes,
                    salt,
                    std::string{ "" }
                };
            }

            std::size_t encoded_length = argon2_encodedlen(raw.config.time_cost, raw.config.memory_cost,
                                                           raw.config.parallelism,
                                                           static_cast<uint32_t>(raw.salt.length()),
                                                           raw.config.hash_length, raw.config.type);

            auto encoded = new char[encoded_length + 1];
            encoded[encoded_length + 1] = '\0';

            raw.context.salt = reinterpret_cast<uint8_t *>(const_cast<char *>(raw.salt.c_str()));
            raw.context.saltlen = static_cast<uint32_t>(raw.salt.length());

            int result = argon2_encode_string(encoded, encoded_length, &raw.context, raw.config.type);
            if (result != ARGON2_OK) {
                return {
                    raw.config,
                    static_cast<argon2_error_codes>(result),
                    salt,
                    std::string{ "" }
                };
            }

            return {
                raw.config,
                ARGON2_OK,
                salt,
                std::string{ encoded, encoded_length }
            };
        }

        argon2_encoded hash_encoded(const std::string &pwd) {
            return hash_encoded(pwd, std::string{ "" });
        }

        argon2_verify verify(const std::string &pwd, const std::string &hash, const std::string &salt) {
            if (pwd.empty()) {
                return {
                    m_config,
                    ARGON2_PWD_TOO_SHORT,
                    false
                };
            }

            auto out = new uint8_t[hash.length() + 1];
            out[hash.length() + 1] = '\0';

            argon2_context context = {
                out,
                static_cast<uint32_t>(hash.length()),
                reinterpret_cast<uint8_t *>(const_cast<char *>(pwd.c_str())),
                static_cast<uint32_t>(pwd.length()),
                reinterpret_cast<uint8_t *>(const_cast<char *>(salt.c_str())),
                static_cast<uint32_t>(salt.length()),
                reinterpret_cast<uint8_t *>(const_cast<char *>(m_config.secret.c_str())),
                static_cast<uint32_t>(m_config.secret.length()),
                reinterpret_cast<uint8_t *>(const_cast<char *>(m_config.ad.c_str())),
                static_cast<uint32_t>(m_config.ad.length()),
                m_config.time_cost,
                m_config.memory_cost,
                m_config.parallelism,
                m_config.parallelism,
                static_cast<uint32_t>(m_config.version),
                nullptr,
                nullptr,
                ARGON2_DEFAULT_FLAGS
            };

            int result = argon2_verify_ctx(&context, hash.c_str(), m_config.type);
            if (result != ARGON2_OK) {
                return {
                    m_config,
                    static_cast<argon2_error_codes>(result),
                    false
                };
            }

            return {
                m_config,
                ARGON2_OK,
                true
            };
        }

        argon2_verify verify_encoded(const std::string &pwd, const std::string &encoded) {
            if (pwd.empty()) {
                return {
                    m_config,
                    ARGON2_PWD_TOO_SHORT,
                    false
                };
            }

            auto out = new uint8_t[encoded.length() + 1];
            out[encoded.length() + 1] = '\0';

            auto salt_out = new uint8_t[encoded.length() + 1];
            salt_out[encoded.length() + 1] = '\0';

            argon2_context context = {
                out,
                static_cast<uint32_t>(encoded.length()),
                reinterpret_cast<uint8_t *>(const_cast<char *>(pwd.c_str())),
                static_cast<uint32_t>(pwd.length()),
                salt_out,
                static_cast<uint32_t>(encoded.length()),
            };

            int result = argon2_decode_string(&context, encoded.c_str(), m_config.type);
            if (result != ARGON2_OK) {
                return {
                    m_config,
                    static_cast<argon2_error_codes>(result),
                    false
                };
            }

            std::string hash{ reinterpret_cast<const char *>(context.out), context.outlen };
            std::string salt{ reinterpret_cast<const char *>(context.salt), context.saltlen };

            context.out = new uint8_t[context.outlen + 1];
            context.out[context.outlen + 1] = '\0';

            result = argon2_verify_ctx(&context, hash.c_str(), m_config.type);
            if (result != ARGON2_OK) {
                return {
                    m_config,
                    static_cast<argon2_error_codes>(result),
                    false
                };
            }

            return {
                m_config,
                ARGON2_OK,
                true
            };
        }

        /**
         * Construct a new default Argon2 config object.
         * @return  The new default Argon2 config object.
         */
        static argon2_config default_config() {
            argon2_config config{
                argon2_type::Argon2_id,
                argon2_version::ARGON2_VERSION_13,
                64 * 1024, // 2^(16) (64MiB of RAM)
                3,
                4,
                std::string{},
                std::string{},
                32, // 32 * 8 = 256-bits
                16 // 16 * 8 = 128 bits
            };
            config.secret.resize(0);
            config.ad.resize(0);
            return config;
        }

    public:
        argon2_config m_config;
    };
}
