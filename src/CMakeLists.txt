project(Main)

include(FetchContent)

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_TESTING "Enable testing with ctest." OFF)

file(GLOB INCLUDE_FILES
    *.h
    cluster/*.h
    database/*.h
    enetwrapper/*.h
    http/*.h
    include/*.h
    include/backward-cpp/*.h
    include/pcg/*.hpp
    items/*.h
    player/*.h
    player/events/tank/*.h
    player/events/text/*.h
    ../vendor/proton/shared/manager/*.h
    ../vendor/proton/shared/util/*.h
    server/*.h
    world/*.h)
file(GLOB SOURCE_FILES
    *.cpp
    cluster/*.cpp
    enetwrapper/*.cpp
    http/*.cpp
    items/*.cpp
    player/*.cpp
    player/events/tank/*.cpp
    player/events/text/*.cpp
    ../vendor/proton/shared/manager/*.cpp
    ../vendor/proton/shared/util/*.cpp
    server/*.cpp
    world/*.cpp)
add_executable(${PROJECT_NAME}
    ${INCLUDE_FILES}
    ${SOURCE_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    UVW_AS_LIB)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/enet/include)
target_link_libraries(${PROJECT_NAME} enet)

find_package(spdlog QUIET)
if (spdlog_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${spdlog_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${spdlog_LIBRARIES})
else ()
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG origin/v1.x
        GIT_SHALLOW TRUE)

    FetchContent_GetProperties(spdlog)
    if (NOT spdlog_POPULATED)
        FetchContent_Populate(spdlog)
        target_include_directories(${PROJECT_NAME} PUBLIC ${spdlog_SOURCE_DIR}/include)
        add_subdirectory(
            ${spdlog_SOURCE_DIR}
            ${spdlog_BINARY_DIR}
            EXCLUDE_FROM_ALL)
    endif ()

    target_link_libraries(${PROJECT_NAME} spdlog::spdlog)
endif ()

find_package(ZLIB QUIET)
if (MINGW OR NOT WIN32)
    if (ZLIB_FOUND)
        target_include_directories(${PROJECT_NAME} PUBLIC ${ZLIB_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
    else ()
        message(FATAL_ERROR "Please install zlib with your package manager.")
    endif ()
else ()
    if (NOT ZLIB_FOUND)
        FetchContent_Declare(
            zlib
            GIT_REPOSITORY https://github.com/madler/zlib.git
            GIT_TAG origin/develop
            GIT_SHALLOW TRUE)

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
    if (brotli_FOUND)
        target_include_directories(${PROJECT_NAME} PUBLIC ${brotli_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME}
            ${brotli_common_LIBRARY}
            ${brotli_enc_LIBRARY}
            ${brotli_dec_LIBRARY})
    else ()
        message(FATAL_ERROR "Please install brotli with your package manager.")
    endif ()
else ()
    if (NOT brotli_FOUND)
        FetchContent_Declare(
            brotli
            GIT_REPOSITORY https://github.com/google/brotli.git
            GIT_TAG origin/master
            GIT_SHALLOW TRUE)

        FetchContent_GetProperties(brotli)
        if (NOT brotli_POPULATED)
            FetchContent_Populate(brotli)
            target_include_directories(${PROJECT_NAME} PUBLIC ${brotli_SOURCE_DIR}/c/include)
            add_subdirectory(
                ${brotli_SOURCE_DIR}
                ${brotli_BINARY_DIR}
                EXCLUDE_FROM_ALL)
        endif ()

        target_link_libraries(${PROJECT_NAME}
            brotlicommon
            brotlienc
            brotlidec)
    endif ()
endif ()

#[[find_package(MariaDB QUIET)
if (MariaDB_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${MariaDB_INCLUDE_DIR}
        ${MariaDB_INCLUDE_DIR}/mysql)
    target_link_libraries(${PROJECT_NAME} ${MariaDB_LIBRARIES})
else ()
    # If you on windows, you need to install mariadb from https://mariadb.com/downloads/connectors/
    message(FATAL_ERROR "Please install MariaDB connector c with your package manager.")
endif ()]]

find_package(sqlpp11 QUIET)
if (sqlpp11_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${sqlpp11_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${sqlpp11_LIBRARIES})
else ()
    if (NOT sqlpp11_FOUND)
        FetchContent_Declare(
            sqlpp11
            GIT_REPOSITORY https://github.com/rbock/sqlpp11.git
            GIT_TAG origin/main
            GIT_SHALLOW TRUE)

        FetchContent_GetProperties(sqlpp11)
        if (NOT sqlpp11_POPULATED)
            option(BUILD_MARIADB_CONNECTOR "Build MariaDB Connector" ON)

            FetchContent_Populate(sqlpp11)
            target_include_directories(${PROJECT_NAME} PUBLIC ${sqlpp11_SOURCE_DIR}/include)
            add_subdirectory(
                ${sqlpp11_SOURCE_DIR}
                ${sqlpp11_BINARY_DIR}
                EXCLUDE_FROM_ALL)

            # Is only me got the error or you too? so need to fix it.
            file(COPY ../cmake/FindMariaDB.cmake DESTINATION ${sqlpp11_SOURCE_DIR}/cmake)
        endif ()

        target_link_libraries(${PROJECT_NAME} sqlpp11::mariadb)
    endif ()
endif ()

#[[find_package(uv QUIET)
if (MINGW OR NOT WIN32)
    if (uv_FOUND)
        target_include_directories(${PROJECT_NAME} PUBLIC ${uv_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} ${uv_LIBRARY})
    else ()
        message(FATAL_ERROR "Please install libuv with your package manager.")
    endif ()
else ()
    if (NOT uv_FOUND)
        FetchContent_Declare(
            uv
            GIT_REPOSITORY https://github.com/libuv/libuv.git
            GIT_TAG origin/v1.x
            GIT_SHALLOW TRUE)

        FetchContent_GetProperties(uv)
        if (NOT uv_POPULATED)
            FetchContent_Populate(uv)
            target_include_directories(${PROJECT_NAME} PUBLIC ${uv_SOURCE_DIR}/include)
            add_subdirectory(
                ${uv_SOURCE_DIR}
                ${uv_BINARY_DIR}
                EXCLUDE_FROM_ALL)
        endif ()

        target_link_libraries(${PROJECT_NAME} uv)
    endif ()
endif ()]]

find_package(uvw QUIET)
if (uvw_FOUND2)
    target_include_directories(${PROJECT_NAME} PUBLIC ${uvw_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${uvw_LIBRARY})
else ()
    if (NOT uvw_FOUND)
        FetchContent_Declare(
            uvw
            GIT_REPOSITORY https://github.com/skypjack/uvw.git
            GIT_TAG origin/master
            GIT_SHALLOW TRUE)

        FetchContent_GetProperties(uvw)
        if (NOT uvw_POPULATED)
            option(BUILD_UVW_LIBS "Prepare targets for static library rather than for a header-only library." ON)

            FetchContent_Populate(uvw)
            target_include_directories(${PROJECT_NAME} PUBLIC ${uvw_SOURCE_DIR}/src)
            add_subdirectory(
                ${uvw_SOURCE_DIR}
                ${uvw_BINARY_DIR}
                EXCLUDE_FROM_ALL)
        endif ()

        target_link_libraries(${PROJECT_NAME} uvw)
    endif ()
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)