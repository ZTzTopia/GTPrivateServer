# FindMariaDB.cmake

if (DEFINED WIN32)
    set(SEARCH_PATHS
        "$ENV{ProgramFiles}/MariaDB/MariaDB Connector C 64-bit"
        "$ENV{ProgramFiles\(x86\)}MariaDB/MariaDB Connector C 32-bit"
    )
    find_path(MariaDB_INCLUDE_DIR
        NAMES mariadb_version.h
        PATHS ${SEARCH_PATHS}
        PATH_SUFFIXES include
    )
    find_library(MariaDB_LIBRARY
        NAMES libmariadb
        PATHS ${SEARCH_PATHS}
        PATH_SUFFIXES lib
    )
else ()
    find_path(MariaDB_INCLUDE_DIR
        NAMES mariadb_version.h
        PATH_SUFFIXES mysql mariadb
    )
    find_library(MariaDB_LIBRARY NAMES mariadb)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    MariaDB
    MariaDB_INCLUDE_DIR
    MariaDB_LIBRARY
)

if (MariaDB_FOUND AND NOT TARGET MariaDB::MariaDB)
    add_library(MariaDB::MariaDB UNKNOWN IMPORTED)
    target_include_directories(MariaDB::MariaDB INTERFACE "${MariaDB_INCLUDE_DIR}")
    set_target_properties(MariaDB::MariaDB PROPERTIES
        IMPORTED_LOCATION "${MariaDB_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LANGUAGES "C")
endif()

mark_as_advanced(MariaDB_FOUND MariaDB_INCLUDE_DIR MariaDB_LIBRARY)
