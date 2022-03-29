project(Main LANGUAGES CXX)

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
    SPDLOG_FMT_EXTERNAL)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/enet/include)
target_link_libraries(${PROJECT_NAME} enet)

if (MSVC)
    add_definitions(/EHsc)
else ()
    add_definitions(-fexceptions)
endif ()

if (CONAN_EXPORTED)
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else ()
    include(${CMAKE_CURRENT_SOURCE_DIR}/conan.cmake)

    conan_cmake_configure(REQUIRES
            spdlog/1.9.2
            openssl/1.1.1m
            zlib/1.2.11
            brotli/1.0.9
            libcurl/7.80.0
            mariadb-connector-c/3.1.12
            sqlpp11/0.61
            uvw/2.11.0
            hiredis/1.0.2
            redis-plus-plus/1.3.2
        GENERATORS cmake_find_package)

    conan_cmake_autodetect(settings)
    conan_cmake_install(PATH_OR_REFERENCE .
        BUILD missing
        REMOTE conancenter
        SETTINGS ${settings})
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")

find_package(spdlog REQUIRED)
if (spdlog_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${spdlog_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${spdlog_LIBRARIES})
endif ()

find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
endif ()

find_package(Brotli REQUIRED)
if (Brotli_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${Brotli_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${Brotli_LIBRARIES})
endif ()

find_package(mariadb-connector-c REQUIRED)
if (mariadb-connector-c_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${mariadb-connector-c_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${mariadb-connector-c_LIBRARIES})
endif ()

find_package(Sqlpp11 REQUIRED)
if (Sqlpp11_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${sqlpp11_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${sqlpp11_LIBRARIES})
endif ()

find_package(libuv REQUIRED)
if (libuv_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${libuv_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${libuv_LIBRARIES})
endif ()

find_package(uvw REQUIRED)
if (uvw_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${uvw_INCLUDE_DIR})
    # target_link_libraries(${PROJECT_NAME} ${uvw_LIBRARIES})
endif ()

find_package(redis++ QUIET)
if (redis++_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${redis++_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${redis++_LIBRARIES})
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 11
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)