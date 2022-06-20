#include <gtest/gtest.h>
#include <argon2.hpp>

using namespace argon2;

TEST(Config, ConfigValidate) {
    argon2_config config{ Argon2::default_config() };

    ASSERT_EQ(config.type, argon2_type::Argon2_id);
    ASSERT_EQ(config.version, argon2_version::ARGON2_VERSION_13);
    ASSERT_EQ(config.memory_cost, 64 * 1024);
    ASSERT_EQ(config.time_cost, 3);
    ASSERT_EQ(config.parallelism, 4);
    ASSERT_EQ(config.secret.length(), 0);
    ASSERT_EQ(config.ad.length(), 0);
    ASSERT_EQ(config.hash_length, 32);
    ASSERT_EQ(config.salt_length, 16);
}

TEST(Config, TryChangeConfig) {
    argon2_config config{ Argon2::default_config() };

    config.type = argon2_type::Argon2_i;
    config.version = argon2_version::ARGON2_VERSION_10;

    ASSERT_EQ(config.type, argon2_type::Argon2_i);
    ASSERT_EQ(config.version, argon2_version::ARGON2_VERSION_10);
}
