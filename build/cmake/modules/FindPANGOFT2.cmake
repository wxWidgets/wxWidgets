# - Try to find PangoFT2
# Once done this will define
#
#  PANGOFT2_FOUND - system has PangoFT2
#  PANGOFT2_INCLUDE_DIRS - The include directory to use for the PangoFT2 headers
#  PANGOFT2_LIBRARIES - Link these to use PangoFT2

find_package(PkgConfig)
pkg_check_modules(PC_PANGOFT2 QUIET pangoft2)

find_path(PANGOFT2_INCLUDE_DIRS
    NAMES pango-context.h
    HINTS ${PC_PANGOFT2_INCLUDEDIR}
          ${PC_PANGOFT2_INCLUDE_DIRS}
    PATH_SUFFIXES pango
)

find_library(PANGOFT2_LIBRARIES
    NAMES pangoft2-1.0
    HINTS ${PC_PANGOFT2_LIBDIR}
          ${PC_PANGOFT2_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PANGOFT2 REQUIRED_VARS PANGOFT2_INCLUDE_DIRS PANGOFT2_LIBRARIES)

mark_as_advanced(
    PANGOFT2_INCLUDE_DIRS
    PANGOFT2_LIBRARIES
)
