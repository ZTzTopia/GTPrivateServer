cmake_minimum_required(VERSION 3.5)
project(phc-winner-argon2 VERSION 20190702)

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif ()

option(ARGON2_WITH_PARALLEL "Enable thread parallel" ON)
option(ARGON2_BUILD_TEST "Build tests" OFF)

set(ARGON2_SRC
    src/argon2.c
    src/core.c
    src/encoding.c
    src/thread.c
    src/blake2/blake2b.c)

# from https://github.com/matlo607/phc-winner-argon2/blob/aa165d6b545024b1719ee4ea33cf4f1bb1a4d1fa/src/CMakeLists.txt#L13
if (NOT ARGON2_CHECK_WITH_OPTIMIZATIONS)
    message(STATUS "Checking support for hardware optimization:")
    try_compile(ARGON2_WITH_OPTIMIZATIONS
        ${CMAKE_CURRENT_BINARY_DIR}/src/optimization
        ${CMAKE_CURRENT_SOURCE_DIR}/src/optimization
        check_cpu_features
        OUTPUT_VARIABLE ARGON2_OUTPUT_TEST_SUPPORT_OPTIMIZATION)
    message(STATUS "Build with hardware optimization? ${ARGON2_WITH_OPTIMIZATIONS}")
    option(ARGON2_CHECK_WITH_OPTIMIZATIONS "Check support for hardware optimization" ON)
endif ()

if (ARGON2_WITH_OPTIMIZATIONS)
    list(APPEND ARGON2_SRC src/opt.c)
else ()
    list(APPEND ARGON2_SRC src/ref.c)
endif ()

add_library(argon2 SHARED ${ARGON2_SRC})
add_library(argon2_static STATIC ${ARGON2_SRC})
list(APPEND ARGON2_SRC src/genkat.c)
add_library(argon2_genkat_static STATIC ${ARGON2_SRC})

target_include_directories(argon2
    PUBLIC
        "$<BUILD_INTERFACE:${phc-winner-argon2_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${phc-winner-argon2_SOURCE_DIR}/src/blake2>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_include_directories(argon2_static
    PUBLIC
        "$<BUILD_INTERFACE:${phc-winner-argon2_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${phc-winner-argon2_SOURCE_DIR}/src/blake2>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_include_directories(argon2_genkat_static
    PUBLIC
        "$<BUILD_INTERFACE:${phc-winner-argon2_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${phc-winner-argon2_SOURCE_DIR}/src/blake2>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if (UNIX)
    target_compile_definitions(argon2 PRIVATE "A2_VISCTL")
    target_compile_definitions(argon2_static PRIVATE "A2_VISCTL")
    target_compile_definitions(argon2_genkat_static PRIVATE "A2_VISCTL")
endif ()

target_compile_definitions(argon2_genkat_static PRIVATE "GENKAT")

if (ARGON2_WITH_PARALLEL)
    find_package(Threads REQUIRED)
    target_link_libraries(argon2 PUBLIC Threads::Threads)
    target_link_libraries(argon2_static PUBLIC Threads::Threads)
    target_link_libraries(argon2_genkat_static PUBLIC Threads::Threads)
else ()
    target_compile_definitions(argon2 PRIVATE "ARGON2_NO_THREADS=1")
    target_compile_definitions(argon2_static PRIVATE "ARGON2_NO_THREADS=1")
    target_compile_definitions(argon2_genkat_static PRIVATE "ARGON2_NO_THREADS=1")
endif()

# Testing.
if (ARGON2_BUILD_TEST)
    add_executable(argon2_test argon2_test.c)
    target_link_libraries(argon2_test PUBLIC argon2_static)

    install(TARGETS argon2_test
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Build argon2 executable.
add_executable(argon2_exe src/run.c)
target_link_libraries(argon2_exe PRIVATE argon2_static)

# Install argon2 binary.
if (MAIN_PROJECT)
    include(GNUInstallDirs)

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(TARGETS argon2 argon2_static argon2_genkat_static argon2_exe
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()