cmake_minimum_required(VERSION 3.0..3.21)
project(GTPS)

include(FetchContent)

add_executable(${PROJECT_NAME}
        main.cpp
        enetwrapper/enetserver.cpp)

target_link_libraries(${PROJECT_NAME} enet)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/enet/include)

find_package(spdlog QUIET)
if (spdlog_FOUND)
    target_link_libraries(${PROJECT_NAME} ${spdlog_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${spdlog_INCLUDE_DIR})
else()
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG 2f2d04b3e840428a18942ca2d3d65203ec564647)

    FetchContent_GetProperties(spdlog)
    if(NOT spdlog_POPULATED)
        FetchContent_Populate(spdlog)
        target_include_directories(${PROJECT_NAME} PUBLIC
                ${spdlog_SOURCE_DIR}/include
                )
        add_subdirectory(
                ${spdlog_SOURCE_DIR}
                ${spdlog_BINARY_DIR}
                EXCLUDE_FROM_ALL)
    endif()

    target_link_libraries(${PROJECT_NAME} spdlog::spdlog)
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
)
