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

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        message(ERROR "Unsupported macOS system")
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "macosarm64")
            set(CEF_SHA1 "d299d467508b970e7227c4d8171985a9942b08a7")
        else()
            set(CEF_PLATFORM "macosx64")
            set(CEF_SHA1 "a71cef7ec7d8230fcc24d97a09c023e77b2b8608")
        endif()
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
            set(CEF_PLATFORM "linuxarm")
            set(CEF_SHA1 "ba3bb572064da216d2a6e3aafbbcda5a96f6f204")
        else()
            message(ERROR "Unsupported Linux system")
        endif()
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "linuxarm64")
            set(CEF_SHA1 "700404f6972200eee834e1f94b42b4df4aefe266")
        else()
            set(CEF_PLATFORM "linux64")
            set(CEF_SHA1 "696f09deb86fd7a220004101521e07381b7dec4b")
        endif()
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
            message(ERROR "Unsupported Windows system")
        else()
            set(CEF_PLATFORM "windows32")
            set(CEF_SHA1 "850c8f5ff35ad36c447e2492292dc965d4f13ebc")
        endif()
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "windowsarm64")
            set(CEF_SHA1 "a1e318fe1dc56d9e4014bf1e3b283a2ee70915d0")
        else()
            set(CEF_PLATFORM "windows64")
            set(CEF_SHA1 "93e6ccdd093da457ae98fdf63f84becc2388bdb8")
        endif()
    endif()
else()
    message(ERROR "Unsupported CEF system")
endif()
set(CEF_URL "https://cef-builds.spotifycdn.com/cef_binary_122.1.10+gc902316+chromium-122.0.6261.112_${CEF_PLATFORM}_minimal.tar.bz2")
