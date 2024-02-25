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
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        message(ERROR "Unsupported macOS system")
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "macosarm64")
            set(CEF_SHA1 "289ba3c54d801f757ef527f581ce49bd135c37d6")
        else()
            set(CEF_PLATFORM "macosx64")
            set(CEF_SHA1 "d52703aa67772ef8cbb9d1a264a454f930df795b")
        endif()
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
            set(CEF_PLATFORM "linuxarm")
            set(CEF_SHA1 "4173bc576d34258e7de8624b216c36bfd5b7cbf0")
        else()
            message(ERROR "Unsupported Linux system")
        endif()
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "linuxarm64")
            set(CEF_SHA1 "1a375eeb5d5c8a6df2e5911d5f6cfa25719ec140")
        else()
            set(CEF_PLATFORM "linux64")
            set(CEF_SHA1 "a6b4166a5622a650165fa09c553ddaa9ade2338b")
        endif()
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
            message(ERROR "Unsupported Windows system")
        else()
            set(CEF_PLATFORM "windows32")
            set(CEF_SHA1 "f7a9f407108eeefc1469b0b51ada59c38b135de9")
        endif()
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "windowsarm64")
            set(CEF_SHA1 "0a6286fafdf30f560bfff6a9bd6604ddaa9ff433")
        else()
            set(CEF_PLATFORM "windows64")
            set(CEF_SHA1 "5053b25559fbb310d0858e21fd81a2067e7b79b6")
        endif()
    endif()
else()
    message(ERROR "Unsupported CEF system")
endif()
set(CEF_URL "https://cef-builds.spotifycdn.com/cef_binary_119.4.7+g55e15c8+chromium-119.0.6045.199_${CEF_PLATFORM}.tar.bz2")
