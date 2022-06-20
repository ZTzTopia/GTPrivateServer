#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <argon2.hpp>

using namespace argon2;

static char salt[]{ "password_salt" };
static char hash[]{ -28, -118, -79, 70, 14, -4, -90, 121, 51, 115, 107, -100, 122, 74, 74, -71, -116, -34, 20, 73, -39, -43, 103, -18, -45, -100, -68, -93, -31, -70, -28, -55, 0 };
static char encoded[]{ "$argon2id$v=19$m=32768,t=1,p=1$cGFzc3dvcmRfc2FsdA$5IqxRg78pnkzc2ucekpKuYzeFEnZ1Wfu05y8o+G65Mk" };

static void BM_VerifyHash(benchmark::State &state) {
    Argon2 argon2{};
    argon2.m_config.memory_cost = 32 * 1024; // 2^(8) (32MiB of RAM)
    argon2.m_config.time_cost = 1;
    argon2.m_config.parallelism = 1;

    for (auto _ : state) {
        argon2::argon2_verify verify{ argon2.verify("password", hash, salt) };
        argon2_error_codes error_codes{ verify.error_codes };
        bool success{ verify.success };

        ASSERT_EQ(error_codes, ARGON2_OK);
        ASSERT_EQ(success, true);
    }
}

static void BM_VerifyEncoded(benchmark::State &state) {
    Argon2 argon2{};
    argon2.m_config.memory_cost = 32 * 1024; // 2^(8) (32MiB of RAM)
    argon2.m_config.time_cost = 1;
    argon2.m_config.parallelism = 1;

    for (auto _ : state) {
        argon2::argon2_verify verify{ argon2.verify_encoded("password", encoded) };
        argon2_error_codes error_codes{ verify.error_codes };
        bool success{ verify.success };

        ASSERT_EQ(error_codes, ARGON2_OK);
        ASSERT_EQ(success, true);
    }
}

BENCHMARK(BM_VerifyHash)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_VerifyEncoded)->Unit(benchmark::kMillisecond);
BENCHMARK_MAIN();
