cmake_minimum_required(VERSION 3.0)
project(unit_tests)

include(FetchContent)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

# Google testing.
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
    GIT_SHALLOW 1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Google benchmark.
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG main
    GIT_SHALLOW 1
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

function (add_argon2_cpp_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE gtest_main argon2_cpp)
    add_test(NAME argon2_cpp_${TEST_NAME} COMMAND ${TEST_NAME})
endfunction ()

function (add_argon2_cpp_benchmark TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE benchmark::benchmark gtest_main argon2_cpp)
    add_test(NAME argon2_cpp_${TEST_NAME} COMMAND ${TEST_NAME})
endfunction ()

add_argon2_cpp_test(unit-config src/unit-config.cpp)
add_argon2_cpp_test(unit-hash src/unit-hash.cpp)
add_argon2_cpp_test(unit-verify src/unit-verify.cpp)
add_argon2_cpp_benchmark(benchmark-hash src/benchmark-hash.cpp)
add_argon2_cpp_benchmark(benchmark-verify src/benchmark-verify.cpp)
