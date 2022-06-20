#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <argon2.hpp>

using namespace argon2;

static void BM_HashRaw(benchmark::State &state) {
    Argon2 argon2{};
    argon2.m_config.memory_cost = 32 * 1024; // 2^(8) (32MiB of RAM)
    argon2.m_config.time_cost = 1;
    argon2.m_config.parallelism = 1;

    for (auto _ : state) {
        argon2.hash_raw("password", "password_salt");
    }
}

static void BM_HashRawRandomSalt(benchmark::State &state) {
    Argon2 argon2{};
    argon2.m_config.memory_cost = 32 * 1024; // 2^(8) (32MiB of RAM)
    argon2.m_config.time_cost = 1;
    argon2.m_config.parallelism = 1;

    for (auto _ : state) {
        argon2.hash_raw("password");
    }
}

static void BM_HashEncoded(benchmark::State &state) {
    Argon2 argon2{};
    argon2.m_config.memory_cost = 32 * 1024; // 2^(8) (32MiB of RAM)
    argon2.m_config.time_cost = 1;
    argon2.m_config.parallelism = 1;

    for (auto _ : state) {
        argon2.hash_encoded("password", "password_salt");
    }
}

static void BM_HashEncodedRandomSalt(benchmark::State &state) {
    Argon2 argon2{};
    argon2.m_config.memory_cost = 32 * 1024; // 2^(8) (32MiB of RAM)
    argon2.m_config.time_cost = 1;
    argon2.m_config.parallelism = 1;

    for (auto _ : state) {
        argon2.hash_encoded("password");
    }
}

BENCHMARK(BM_HashRaw)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_HashRawRandomSalt)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_HashEncoded)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_HashEncodedRandomSalt)->Unit(benchmark::kMillisecond);
BENCHMARK_MAIN();
