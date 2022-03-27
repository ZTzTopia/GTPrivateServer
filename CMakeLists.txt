cmake_minimum_required(VERSION 3.0..3.21)
project(GTPrivateServer)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(src)
add_subdirectory(vendor/enet)
