# Find the PCRE2 headers and libraries.
#
#  Optionally define the following variables:
#     PCRE2_CODE_UNIT_WIDTH - code unit width: 8 (default), 16 or 32 bit.
#
#  This module defines the following variables:
#     PCRE2_FOUND        - true if PCRE2 is found.
#     PCRE2_INCLUDE_DIRS - list of PCRE2 include directories.
#     PCRE2_LIBRARIES    - list of PCRE2 libraries.

if(NOT PCRE2_CODE_UNIT_WIDTH)
    set(PCRE2_CODE_UNIT_WIDTH 8)
endif()

if(NOT PCRE2_CODE_UNIT_WIDTH EQUAL PCRE2_CODE_UNIT_WIDTH_USED)
    unset(PCRE2_CODE_UNIT_WIDTH_USED CACHE)
    unset(PCRE2_FOUND CACHE)
    unset(PCRE2_INCLUDE_DIRS CACHE)
    unset(PCRE2_LIBRARIES CACHE)
endif()

set(PCRE2_CODE_UNIT_WIDTH_USED "${PCRE2_CODE_UNIT_WIDTH}" CACHE INTERNAL "")


find_package(PkgConfig QUIET)
pkg_check_modules(PC_PCRE2 QUIET libpcre2-${PCRE2_CODE_UNIT_WIDTH})

find_path(PCRE2_INCLUDE_DIRS
    NAMES pcre2.h
    HINTS ${PC_PCRE2_INCLUDEDIR}
          ${PC_PCRE2_INCLUDE_DIRS}
)

find_library(PCRE2_LIBRARIES
    NAMES pcre2-${PCRE2_CODE_UNIT_WIDTH}
    HINTS ${PC_PCRE2_LIBDIR}
          ${PC_PCRE2_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PCRE2 REQUIRED_VARS PCRE2_LIBRARIES PCRE2_INCLUDE_DIRS VERSION_VAR PC_PCRE2_VERSION)

mark_as_advanced(PCRE2_LIBRARIES PCRE2_INCLUDE_DIRS)
