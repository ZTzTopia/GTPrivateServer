project(Main LANGUAGES CXX)

file(GLOB INCLUDE_FILES
    *.h
    **/*.h
    **/**/*.h
    ../vendor/proton/shared/*.h)

file(GLOB SOURCE_FILES
    *.cpp
    **/*.cpp
    **/**/*.cpp
    ../vendor/proton/shared/**/*.cpp)

add_executable(${PROJECT_NAME}
    ${INCLUDE_FILES}
    ${SOURCE_FILES})

if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        COMPILE_FLAGS "/EHsc")
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 11
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/proton/shared)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    CPPHTTPLIB_OPENSSL_SUPPORT
    SPDLOG_FMT_EXTERNAL)

target_link_libraries(${PROJECT_NAME}
    enet)
    # argon2_cpp)

# https://github.com/conan-io/conan-center-index/pull/10040
execute_process(COMMAND conan create --build=missing --test-folder=None
    ${CMAKE_CURRENT_SOURCE_DIR}/../conan/redis-plus-plus/all/conanfile.py redis-plus-plus/1.3.3@)

# Install all needed packages.
include(${CMAKE_CURRENT_SOURCE_DIR}/../conan/conan.cmake)

conan_cmake_configure(REQUIRES
        brotli/1.0.9
        cpp-httplib/0.10.8
        fmt/8.1.1
        hiredis/1.0.2
        libcurl/7.80.0
        libuv/1.44.1
        libressl/3.5.3
        mariadb-connector-c/3.1.12
        nlohmann_json/3.10.5
        redis-plus-plus/1.3.3
        spdlog/1.10.0
        sqlpp11/0.61
        uvw/2.12.1
        zlib/1.2.12
    GENERATORS cmake_find_package
    IMPORTS "bin, *.dll -> ./bin"
    OPTIONS
        brotli:shared=True
        fmt:shared=True
        hiredis:shared=True
        libcurl:shared=True
        libcurl:with_ssl=False
        libuv:shared=True
        libressl:shared=True
        mariadb-connector-c:shared=True
        mariadb-connector-c:with_ssl=False
        redis-plus-plus:shared=True
        redis-plus-plus:with_tls=False
        redis-plus-plus:with_async=True
        spdlog:shared=True
        zlib:shared=True)

conan_cmake_autodetect(settings)
conan_cmake_install(PATH_OR_REFERENCE .
    INSTALL_FOLDER ${CMAKE_BINARY_DIR}/conan
    BUILD missing
    REMOTE conancenter
    SETTINGS ${settings})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/conan")

find_package(Brotli REQUIRED)
target_link_libraries(${PROJECT_NAME} Brotli::Brotli)

find_package(httplib)
target_link_libraries(${PROJECT_NAME} httplib::httplib)

find_package(fmt REQUIRED)
target_link_libraries(${PROJECT_NAME} fmt::fmt)

find_package(LibreSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} LibreSSL::Crypto LibreSSL::SSL LibreSSL::TLS)

find_package(mariadb-connector-c REQUIRED)
target_link_libraries(${PROJECT_NAME} mariadb-connector-c::mariadb-connector-c)

find_package(nlohmann_json REQUIRED)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

find_package(uvw REQUIRED)
target_link_libraries(${PROJECT_NAME} uvw::uvw)

find_package(redis++ REQUIRED)
target_link_libraries(${PROJECT_NAME} redis++::redis++)

find_package(spdlog REQUIRED)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)

find_package(Sqlpp11 REQUIRED)
target_link_libraries(${PROJECT_NAME} sqlpp11::sqlpp11)

find_package(ZLIB REQUIRED)
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
