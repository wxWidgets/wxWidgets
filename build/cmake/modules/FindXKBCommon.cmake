# FindXKBCommon

find_package(PkgConfig)
pkg_check_modules(PC_XKBCOMMON QUIET xkbcommon)

find_path(XKBCOMMON_INCLUDE_DIRS
    NAMES xkbcommon.h
    HINTS ${PC_XKBCOMMON_INCLUDEDIR}
          ${PC_XKBCOMMON_INCLUDE_DIRS}
    PATH_SUFFIXES xkbcommon
)

find_library(XKBCOMMON_LIBRARIES
    NAMES xkbcommon
    HINTS ${PC_XKBCOMMON_LIBDIR}
          ${PC_XKBCOMMON_LIBRARY_DIRS}
)

set(XKBCOMMON_VERSION ${PC_XKBCOMMON_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XKBCommon REQUIRED_VARS XKBCOMMON_INCLUDE_DIRS XKBCOMMON_LIBRARIES
                                            VERSION_VAR   XKBCOMMON_VERSION)

mark_as_advanced(
    XKBCOMMON_INCLUDE_DIRS
    XKBCOMMON_LIBRARIES
)
