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
            set(CEF_SHA1 "ea0807407c6d9dc1bb58cc0d4c67e9c3a1208e93")
        else()
            set(CEF_PLATFORM "macosx64")
            set(CEF_SHA1 "91fbc0347bacaadb20816c0b14a5a2c06c5a58f6")
        endif()
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
            set(CEF_PLATFORM "linuxarm")
            set(CEF_SHA1 "267658ad627828a8482f69600f7f295190902124")
        else()
            message(ERROR "Unsupported Linux system")
        endif()
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "linuxarm64")
            set(CEF_SHA1 "6457fc5dd3a847728f7b36f19a8d7291a3c5295d")
        else()
            set(CEF_PLATFORM "linux64")
            set(CEF_SHA1 "e41bb03c6cd04d23e6a562b7783ec1da8eaa666c")
        endif()
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_SIZEOF_VOID_P LESS 8)
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
            message(ERROR "Unsupported Windows system")
        else()
            set(CEF_PLATFORM "windows32")
            set(CEF_SHA1 "28ba21dfc7eb68764c8a658ab7d0b6904122ab33")
        endif()
    else()
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            set(CEF_PLATFORM "windowsarm64")
            set(CEF_SHA1 "34e99674098383de12b67bb9242f64c9d10633ed")
        else()
            set(CEF_PLATFORM "windows64")
            set(CEF_SHA1 "f44288f1bb32ae88e3384874970352048267d9af")
        endif()
    endif()
else()
    message(ERROR "Unsupported CEF system")
endif()
set(CEF_URL "https://cef-builds.spotifycdn.com/cef_binary_121.3.13+g5c4a81b+chromium-121.0.6167.184_${CEF_PLATFORM}${CEF_DISTRIBUTION}.tar.bz2")
