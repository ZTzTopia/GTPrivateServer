cmake_minimum_required(VERSION 3.0)
project(argon2_cpp)

set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif ()

# Options.
option(ARGON2_CPP_WITH_PARALLEL "Enable thread parallel" ON) #
option(ARGON2_CPP_SHARED_ARGON2 "Build argon2 shared instead of static" OFF)
option(ARGON2_CPP_BUILD_TEST "Build tests" ON)

# Argon2 C++.
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Argon2.
add_subdirectory(argon2)
if (ARGON2_CPP_SHARED_ARGON2)
    target_link_libraries(${PROJECT_NAME} INTERFACE argon2)
else ()
    target_link_libraries(${PROJECT_NAME} INTERFACE argon2_static)
endif ()

target_include_directories(${PROJECT_NAME}
    INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Testing.
if (ARGON2_CPP_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

# Install argon2 C++ binary.
if (MAIN_PROJECT)
    include(GNUInstallDirs)

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()
