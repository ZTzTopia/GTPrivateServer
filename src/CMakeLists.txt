cmake_minimum_required(VERSION 3.0..3.21)
project(GTPS)

include(FetchContent)

add_executable(${PROJECT_NAME}
        main.cpp

        enetwrapper/enetserver.cpp

        items/itemsdb.cpp

        player/eventmanager.cpp
        player/eventtrigger.cpp
        player/generictextevent.cpp
        player/playerpool.cpp
        player/player.cpp

        proton/shared/manager/VariantDB.cpp
        proton/shared/util/ResourceUtils.cpp
        proton/shared/util/Variant.cpp

        server/server.cpp
        server/servergateway.cpp
        server/serverpool.cpp)

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
        GIT_TAG b1478d98f017f3a7644e6e3a16fab6a47a5c26ba)

    FetchContent_GetProperties(spdlog)
    if (NOT spdlog_POPULATED)
        FetchContent_Populate(spdlog)
        target_include_directories(${PROJECT_NAME} PUBLIC
            ${spdlog_SOURCE_DIR}/include)
        add_subdirectory(
            ${spdlog_SOURCE_DIR}
            ${spdlog_BINARY_DIR}
            EXCLUDE_FROM_ALL)
    endif ()

    target_link_libraries(${PROJECT_NAME} spdlog::spdlog)
endif ()

find_package(ZLIB QUIET)
if (MINGW OR NOT WIN32)
    if (NOT ZLIB_FOUND)
        message(FATAL_ERROR "Please install zlib with your package manager.")
    endif ()
else ()
    if (NOT ZLIB_FOUND)
        FetchContent_Declare(
            zlib
            GIT_REPOSITORY https://github.com/madler/zlib.git
            GIT_TAG 2014a993addbc8f1b9785d97f55fd189792c2f78)

        FetchContent_GetProperties(zlib)
        if (NOT zlib_POPULATED)
            FetchContent_Populate(zlib)
            target_include_directories(${PROJECT_NAME} PUBLIC
                ${zlib_BINARY_DIR} # For zconf.h
                ${zlib_SOURCE_DIR})
            add_subdirectory(
                ${zlib_SOURCE_DIR}
                ${zlib_BINARY_DIR}
                EXCLUDE_FROM_ALL)
        endif ()

        target_link_libraries(${PROJECT_NAME} zlibstatic)
    endif ()
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)
