# - Try to find gspell
# Once done this will define
#
#  GSPELL_FOUND - system has gspell
#  GSPELL_INCLUDE_DIRS - The include directory to use for the gspell headers
#  GSPELL_LIBRARIES - Link these to use gspell

find_package(PkgConfig)
pkg_check_modules(PC_GSPELL QUIET gspell-1)

find_path(GSPELL_INCLUDE_DIRS
    NAMES gspell/gspell.h
    HINTS ${PC_GSPELL_INCLUDEDIR}
          ${PC_GSPELL_INCLUDE_DIRS}
)

find_library(GSPELL_LIBRARIES
    NAMES gspell-1
    HINTS ${PC_GSPELL_LIBDIR}
          ${PC_GSPELL_LIBRARY_DIRS}
)

pkg_check_modules(PC_ENCHANT QUIET enchant-2 enchant)
find_path(ENCHANT_INCLUDE_DIRS
    NAMES enchant.h
    HINTS ${PC_ENCHANT_INCLUDEDIR}
          ${PC_ENCHANT_INCLUDE_DIRS}
    PATH_SUFFIXES enchant-2 enchant
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GSPELL DEFAULT_MSG GSPELL_INCLUDE_DIRS ENCHANT_INCLUDE_DIRS GSPELL_LIBRARIES)

if(GSPELL_FOUND)
    set(GSPELL_INCLUDE_DIRS ${GSPELL_INCLUDE_DIRS} ${ENCHANT_INCLUDE_DIRS})
endif()

mark_as_advanced(
    GSPELL_INCLUDE_DIRS
    GSPELL_LIBRARIES
    ENCHANT_INCLUDE_DIRS
)
