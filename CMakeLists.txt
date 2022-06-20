cmake_minimum_required(VERSION 3.0..3.21)
project(GTPrivateServer VERSION 0.1.0)

# option(ARGON2_CPP_BUILD_TEST "Build tests" OFF)

add_subdirectory(src)
add_subdirectory(vendor/enet)
# add_subdirectory(vendor/argon2-cpp)
