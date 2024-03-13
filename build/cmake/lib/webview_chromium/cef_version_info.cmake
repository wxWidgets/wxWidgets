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
# To update this file, use cef_update_version_info.cmake

set(CEF_SHA1_macosarm64     "d299d467508b970e7227c4d8171985a9942b08a7")
set(CEF_SHA1_macosx64       "a71cef7ec7d8230fcc24d97a09c023e77b2b8608")
set(CEF_SHA1_linuxarm       "ba3bb572064da216d2a6e3aafbbcda5a96f6f204")
set(CEF_SHA1_linuxarm64     "700404f6972200eee834e1f94b42b4df4aefe266")
set(CEF_SHA1_linux64        "696f09deb86fd7a220004101521e07381b7dec4b")
set(CEF_SHA1_windows32      "850c8f5ff35ad36c447e2492292dc965d4f13ebc")
set(CEF_SHA1_windowsarm64   "a1e318fe1dc56d9e4014bf1e3b283a2ee70915d0")
set(CEF_SHA1_windows64      "93e6ccdd093da457ae98fdf63f84becc2388bdb8")

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        message(FATAL_ERROR "Unsupported macOS system")
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "macosarm64")
        else()
            set(CEF_PLATFORM "macosx64")
        endif()
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
            set(CEF_PLATFORM "linuxarm")
        else()
            message(FATAL_ERROR "Unsupported Linux system")
        endif()
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "linuxarm64")
        else()
            set(CEF_PLATFORM "linux64")
        endif()
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
            message(FATAL_ERROR "Unsupported Windows system")
        else()
            set(CEF_PLATFORM "windows32")
        endif()
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "windowsarm64")
        else()
            set(CEF_PLATFORM "windows64")
        endif()
    endif()
else()
    message(FATAL_ERROR "Unsupported CEF system")
endif()
set(CEF_SHA1 "${CEF_SHA1_${CEF_PLATFORM}}")

set(CEF_URL "https://cef-builds.spotifycdn.com/cef_binary_122.1.10+gc902316+chromium-122.0.6261.112_${CEF_PLATFORM}_minimal.tar.bz2")
