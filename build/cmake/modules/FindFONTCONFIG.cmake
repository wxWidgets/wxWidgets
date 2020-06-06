# - Try to find Fontconfig
# Once done this will define
#
#  FONTCONFIG_FOUND - system has Fontconfig
#  FONTCONFIG_INCLUDE_DIRS - The include directory to use for the Fontconfig headers
#  FONTCONFIG_LIBRARIES - Link these to use Fontconfig

find_package(PkgConfig)
pkg_check_modules(PC_FONTCONFIG QUIET fontconfig)

find_path(FONTCONFIG_INCLUDE_DIRS
    NAMES fontconfig.h
    HINTS ${PC_FONTCONFIG_INCLUDEDIR}
          ${PC_FONTCONFIG_INCLUDE_DIRS}
    PATH_SUFFIXES fontconfig
)

find_library(FONTCONFIG_LIBRARIES
    NAMES fontconfig
    HINTS ${PC_FONTCONFIG_LIBDIR}
          ${PC_FONTCONFIG_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FONTCONFIG DEFAULT_MSG FONTCONFIG_INCLUDE_DIRS FONTCONFIG_LIBRARIES)

mark_as_advanced(
    FONTCONFIG_INCLUDE_DIRS
    FONTCONFIG_LIBRARIES
)
