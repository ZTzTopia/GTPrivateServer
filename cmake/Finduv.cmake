# Finduv.cmake

if (NOT DEFINED WIN32)
    find_path(uv_INCLUDE_DIR
        NAMES uv.h
        PATH_SUFFIXES uv)
    find_library(uv_LIBRARY NAMES uv)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    uv
    uv_INCLUDE_DIR
    uv_LIBRARY
)

mark_as_advanced(uv_FOUND uv_INCLUDE_DIR uv_LIBRARY)
