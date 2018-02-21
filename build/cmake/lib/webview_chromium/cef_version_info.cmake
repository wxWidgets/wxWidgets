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
    set(CEF_SHA1 "c0e6185629077400630ca942f67b892f5a56be5b")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        set(CEF_PLATFORM "linux32")
        set(CEF_SHA1 "d73fad2f014090baddc2d964b747a78473169faf")
    else()
        set(CEF_PLATFORM "linux64")
        set(CEF_SHA1 "2ff83db27479ec0d9b5d360bb9a3a52323f536c0")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        set(CEF_PLATFORM "windows32")
        set(CEF_SHA1 "8d2fff739d05adde4b49fa7e2a52d5fc65ccc532")
    else()
        set(CEF_PLATFORM "windows64")
        set(CEF_SHA1 "87603cc75641e84d12c97dd7dbad09031dd7af68")
    endif()
else()
    message(ERROR "Unsupported CEF system")
endif()
set(CEF_URL "http://opensource.spotify.com/cefbuilds/cef_binary_3.3282.1733.g9091548_${CEF_PLATFORM}.tar.bz2")
