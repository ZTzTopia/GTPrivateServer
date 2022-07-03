cmake_minimum_required(VERSION 3.0..3.23)
project(Main LANGUAGES CXX)

file(GLOB INCLUDE_FILES
    *.h
    **/*.h
    **/*.hpp
    **/**/*.h
    **/**/*.hpp
    ../vendor/proton/shared/*.h)

file(GLOB SOURCE_FILES
    *.cpp
    **/*.cpp
    **/**/*.cpp
    ../vendor/proton/shared/**/*.cpp)

add_executable(${PROJECT_NAME}
    ${INCLUDE_FILES}
    ${SOURCE_FILES} database/postgres.cpp database/postgres.h)

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
    SPDLOG_FMT_EXTERNAL
    GTPRIVATESERVER_VERSION="${CMAKE_PROJECT_VERSION}")

target_link_libraries(${PROJECT_NAME}
    enet)
    # argon2_cpp)

# Install all needed packages.
include(${CMAKE_CURRENT_SOURCE_DIR}/../conan/conan.cmake)

conan_cmake_configure(REQUIRES
        brotli/1.0.9
        cpp-httplib/0.10.8
        date/3.0.1
        fmt/8.1.1
        glm/0.9.9.8
        libpq/14.2
        libressl/3.5.3
        magic_enum/0.8.0
        nlohmann_json/3.10.5
        spdlog/1.10.0
        sqlpp11/0.61
        zlib/1.2.12
    GENERATORS cmake_find_package
    IMPORTS "bin, *.dll -> ./bin"
    OPTIONS
        brotli:shared=True
        date:header_only=True
        fmt:shared=True
        libpq:shared=True
        libpq:with_openssl=False
        libressl:shared=True
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

find_package(glm)
target_link_libraries(${PROJECT_NAME} glm::glm)

find_package(PostgreSQL)
target_link_libraries(${PROJECT_NAME} PostgreSQL::PostgreSQL)

find_package(LibreSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} LibreSSL::LibreSSL)

find_package(magic_enum REQUIRED)
target_link_libraries(${PROJECT_NAME} magic_enum::magic_enum)

find_package(nlohmann_json REQUIRED)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

find_package(spdlog REQUIRED)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)

find_package(Sqlpp11 REQUIRED)
target_link_libraries(${PROJECT_NAME} sqlpp11::sqlpp11)

find_package(ZLIB REQUIRED)
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
