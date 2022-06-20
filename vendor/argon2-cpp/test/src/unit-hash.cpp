#include <gtest/gtest.h>
#include <argon2.hpp>

using namespace argon2;

static char excepted_salt[]{ "password_salt" };
static char excepted_hash[]{ -28, -118, -79, 70, 14, -4, -90, 121, 51, 115, 107, -100, 122, 74, 74, -71, -116, -34, 20, 73, -39, -43, 103, -18, -45, -100, -68, -93, -31, -70, -28, -55, 0 };
static char excepted_encoded[]{ "$argon2id$v=19$m=32768,t=1,p=1$cGFzc3dvcmRfc2FsdA$5IqxRg78pnkzc2ucekpKuYzeFEnZ1Wfu05y8o+G65Mk" };

TEST(Hash, HashRaw) {
    Argon2 argon2{};
    argon2.m_config.memory_cost = 32 * 1024; // 2^(8) (32MiB of RAM)
    argon2.m_config.time_cost = 1;
    argon2.m_config.parallelism = 1;

    argon2_raw raw{ argon2.hash_raw("password", "password_salt") };
    argon2_error_codes error_codes{ raw.error_codes };
    std::string salt{ raw.salt };
    std::string hash{ raw.hash };

    ASSERT_EQ(error_codes, ARGON2_OK);
    ASSERT_STREQ(salt.c_str(), excepted_salt);
    ASSERT_STREQ(hash.c_str(), excepted_hash);
}

TEST(Hash, HashRawRandomSalt) {
    Argon2 argon2{};
    argon2.m_config.memory_cost = 32 * 1024; // 2^(8) (32MiB of RAM)
    argon2.m_config.time_cost = 1;
    argon2.m_config.parallelism = 1;

    argon2_raw raw{ argon2.hash_raw("password") };
    argon2_error_codes error_codes{ raw.error_codes };
    std::string salt{ raw.salt };
    std::string hash{ raw.hash };

    ASSERT_EQ(error_codes, ARGON2_OK);
}

TEST(Hash, HashEncoded) {
    Argon2 argon2{};
    argon2.m_config.memory_cost = 32 * 1024; // 2^(8) (32MiB of RAM)
    argon2.m_config.time_cost = 1;
    argon2.m_config.parallelism = 1;

    argon2_encoded encoded{ argon2.hash_encoded("password", "password_salt") };
    argon2_error_codes error_codes{ encoded.error_codes };
    std::string salt{ encoded.salt };
    std::string encoded_{ encoded.encoded };

    ASSERT_EQ(error_codes, ARGON2_OK);
    ASSERT_STREQ(salt.c_str(), excepted_salt);
    ASSERT_STREQ(encoded_.c_str(), excepted_encoded);
}

TEST(Hash, HashEncodedRandomSalt) {
    Argon2 argon2{};
    argon2.m_config.memory_cost = 32 * 1024; // 2^(8) (32MiB of RAM)
    argon2.m_config.time_cost = 1;
    argon2.m_config.parallelism = 1;

    argon2_encoded encoded{ argon2.hash_encoded("password") };
    argon2_error_codes error_codes{ encoded.error_codes };
    std::string salt{ encoded.salt };
    std::string encoded_{ encoded.encoded };

    ASSERT_EQ(error_codes, ARGON2_OK);
}
