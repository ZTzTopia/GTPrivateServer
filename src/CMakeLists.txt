cmake_minimum_required(VERSION 3.16..3.21)
project(GTPS)

include(FetchContent)

option(BUILD_SHARED_LIBS "Build shared library" ON)

file(GLOB INCLUDE_FILES
    *.h
    database/*.h
    enetwrapper/*.h
    http/*.h
    include/*.h
    include/backtrace-cpp/*.h
    items/*.h
    player/*.h
    player/events/tank/*.h
    player/events/text/*.h
    proton/shared/manager/*.h
    proton/shared/util/*.h
    server/*.h
    world/*.h)

file(GLOB SOURCE_FILES
    *.cpp
    enetwrapper/*.cpp
    items/*.cpp
    player/*.cpp
    player/events/tank/*.cpp
    player/events/text/*.cpp
    proton/shared/manager/*.cpp
    proton/shared/util/*.cpp
    server/*.cpp
    world/*.cpp)

add_executable(${PROJECT_NAME}
    ${INCLUDE_FILES}
    ${SOURCE_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(${PROJECT_NAME} enet)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/enet/include)

find_package(spdlog QUIET)
if (spdlog_FOUND)
    target_link_libraries(${PROJECT_NAME} ${spdlog_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${spdlog_INCLUDE_DIR})
else ()
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG origin/v1.x)

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
            GIT_TAG origin/develop)

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

        target_link_libraries(${PROJECT_NAME} zlib)
    endif ()
endif ()

find_package(brotli QUIET)
if (MINGW OR NOT WIN32)
    if (NOT brotli_FOUND)
        message(FATAL_ERROR "Please install brotli with your package manager.")
    endif ()
else ()
    if (NOT brotli_FOUND)
        FetchContent_Declare(
            brotli
            GIT_REPOSITORY https://github.com/google/brotli.git
            GIT_TAG origin/master)

        FetchContent_GetProperties(brotli)
        if (NOT brotli_POPULATED)
            FetchContent_Populate(brotli)
            target_include_directories(${PROJECT_NAME} PUBLIC
                ${brotli_SOURCE_DIR}/c/include)
            add_subdirectory(
                ${brotli_SOURCE_DIR}
                ${brotli_BINARY_DIR}
                EXCLUDE_FROM_ALL)
        endif ()

        target_link_libraries(${PROJECT_NAME}
            brotlienc
            brotlidec)
    endif ()
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

find_package(MariaDB QUIET)
if (NOT MariaDB_FOUND)
    # If you on windows, you need to install mariadb from https://mariadb.com/downloads/connectors/
    # Or you want the fetch content one?
    message(FATAL_ERROR "Please install MariaDB with your package manager.")
else ()
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${MariaDB_INCLUDE_DIR}
        ${MariaDB_INCLUDE_DIR}/mysql)

    target_link_libraries(${PROJECT_NAME}
        ${MariaDB_LIBRARIES})
endif ()

find_package(sqlpp11 QUIET)
if (sqlpp11_FOUND)
    target_link_libraries(${PROJECT_NAME} ${sqlpp11_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${sqlpp11_INCLUDE_DIR})
else ()
    if (NOT sqlpp11_FOUND)
        FetchContent_Declare(
            sqlpp11
            GIT_REPOSITORY https://github.com/rbock/sqlpp11.git
            GIT_TAG origin/main)

        FetchContent_GetProperties(sqlpp11)
        if (NOT sqlpp11_POPULATED)
            option(BUILD_MARIADB_CONNECTOR "Build MariaDB Connector" ON)

            # Is only me got the error or you too? so need to fix it.
            file(COPY ../cmake/FindMariaDB.cmake DESTINATION ${sqlpp11_SOURCE_DIR}/cmake)

            FetchContent_Populate(sqlpp11)
            target_include_directories(${PROJECT_NAME} PUBLIC
                ${sqlpp11_SOURCE_DIR}/include)
            add_subdirectory(
                ${sqlpp11_SOURCE_DIR}
                ${sqlpp11_BINARY_DIR}
                EXCLUDE_FROM_ALL)
        endif ()

        target_link_libraries(${PROJECT_NAME}
            sqlpp11::mariadb)
    endif ()
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)
