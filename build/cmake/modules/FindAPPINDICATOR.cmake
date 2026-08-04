# - Try to find AppIndicator/Ayatana AppIndicator
# This module defines the following variables:
#
#  APPINDICATOR_FOUND        - AppIndicator was found
#  APPINDICATOR_INCLUDE_DIRS - the AppIndicator include directories
#  APPINDICATOR_LIBRARIES    - link these to use AppIndicator
#  wxUSE_AYATANA_APPINDICATOR - set to ON when the Ayatana variant is used

find_package(PkgConfig)
pkg_check_modules(AYATANA_APPINDICATOR QUIET ayatana-appindicator3-0.1)

if(AYATANA_APPINDICATOR_FOUND)
    set(APPINDICATOR_FOUND TRUE)
    set(APPINDICATOR_INCLUDE_DIRS ${AYATANA_APPINDICATOR_INCLUDE_DIRS})
    set(APPINDICATOR_LIBRARIES    ${AYATANA_APPINDICATOR_LIBRARIES})
    set(wxUSE_AYATANA_APPINDICATOR ON)
else()
    pkg_check_modules(APPINDICATOR3 QUIET appindicator3-0.1)
    if(APPINDICATOR3_FOUND)
        set(APPINDICATOR_FOUND TRUE)
        set(APPINDICATOR_INCLUDE_DIRS ${APPINDICATOR3_INCLUDE_DIRS})
        set(APPINDICATOR_LIBRARIES    ${APPINDICATOR3_LIBRARIES})
        set(wxUSE_AYATANA_APPINDICATOR OFF)
    endif()
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(APPINDICATOR REQUIRED_VARS APPINDICATOR_LIBRARIES)

mark_as_advanced(
    APPINDICATOR_INCLUDE_DIRS
    APPINDICATOR_LIBRARIES
)
