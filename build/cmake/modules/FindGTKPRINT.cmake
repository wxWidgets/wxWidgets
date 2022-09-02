# - Try to find GTK Print
# Provide the GTK version as argument
# Once done this will define
#
#  GTKPRINT_FOUND - system has GTK Print
#  GTKPRINT_INCLUDE_DIRS - The include directory to use for the GTK Print headers

if(NOT GTKPRINT_FIND_VERSION EQUAL GTKPRINT_FIND_VERSION_USED)
    unset(GTKPRINT_FOUND CACHE)
    unset(GTKPRINT_INCLUDE_DIRS CACHE)
    unset(GTKPRINT_FIND_VERSION_USED CACHE)
endif()
set(GTKPRINT_FIND_VERSION_USED "${GTKPRINT_FIND_VERSION}" CACHE INTERNAL "")

if(GTKPRINT_FIND_VERSION VERSION_LESS 3.0)
    set(GTKPRINT_LIB_NAME "gtk+-unix-print-2.0")
else()
    set(GTKPRINT_LIB_NAME "gtk+-unix-print-3.0")
endif()

find_package(PkgConfig)
pkg_check_modules(PC_GTKPRINT QUIET ${GTKPRINT_LIB_NAME})

find_path(GTKPRINT_INCLUDE_DIRS
    NAMES gtk/gtkunixprint.h
    HINTS ${PC_GTKPRINT_INCLUDEDIR}
          ${PC_GTKPRINT_INCLUDE_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GTKPRINT DEFAULT_MSG GTKPRINT_INCLUDE_DIRS)

mark_as_advanced(GTKPRINT_INCLUDE_DIRS)
