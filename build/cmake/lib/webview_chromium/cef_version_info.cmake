#############################################################################
# Name:        build/cmake/lib/webview_chromium/cef_version_info.cmake
# Purpose:     CMake file CEF version information
# Author:      Tobias Taschner
# Created:     2018-02-03
# Copyright:   (c) 2018 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

#
# DO NOT MODIFY MANUALLY
#
# To update the include file see cef_update_version_info.cmake

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CEF_PLATFORM "macosx64")
    set(CEF_SHA1 "ca26459c5ef344cac04b2ed121514db79e067cff")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        set(CEF_PLATFORM "linux32")
        set(CEF_SHA1 "75bacba21804024a29ae2f0780d079b04f6b607f")
    else()
        set(CEF_PLATFORM "linux64")
        set(CEF_SHA1 "3d1674170e9b79c4d7c0749a0879db7d6abef7fe")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        set(CEF_PLATFORM "windows32")
        set(CEF_SHA1 "bbdfbb1733c07edbcaab75f963f82694a94c8968")
    else()
        set(CEF_PLATFORM "windows64")
        set(CEF_SHA1 "87fcdab9c20ae7494455460eb3f3e658abe95013")
    endif()
else()
    message(ERROR "Unsupported CEF system")
endif()
set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3239.1723.g071d1c1_${CEF_PLATFORM}.tar.bz2")
