#pragma once
#include "../include/randutils.hpp"

namespace random {
    inline randutils::mt19937_rng get_generator_static() {
        static randutils::mt19937_rng mt19937_rng;
        return mt19937_rng;
    }

    inline randutils::mt19937_rng get_generator_thread_local() {
        thread_local randutils::mt19937_rng mt19937_rng;
        return mt19937_rng;
    }

    inline randutils::mt19937_rng get_generator_local() {
        randutils::mt19937_rng mt19937_rng;
        return mt19937_rng;
    }
}