# Findbrotli.cmake

if (NOT DEFINED WIN32)
    find_path(brotli_INCLUDE_DIR
        NAMES decode.h
        PATH_SUFFIXES brotli)
    find_library(brotli_common_LIBRARY NAMES brotlicommon)
    find_library(brotli_enc_LIBRARY NAMES brotlienc)
    find_library(brotli_dec_LIBRARY NAMES brotlidec)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    brotli
    brotli_INCLUDE_DIR
    brotli_common_LIBRARY
    brotli_enc_LIBRARY
    brotli_dec_LIBRARY
)

mark_as_advanced(brotli_FOUND brotli_INCLUDE_DIR brotli_common_LIBRARY brotli_enc_LIBRARY brotli_dec_LIBRARY)
