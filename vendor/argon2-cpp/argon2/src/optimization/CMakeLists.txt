project(check_cpu_features)
cmake_minimum_required(VERSION 3.5)

add_library(test_cpu_features STATIC ${CMAKE_CURRENT_SOURCE_DIR}/../opt.c)
target_include_directories(test_cpu_features PRIVATE ${CMAKE_SOURCE_DIR}/../../include)
