#############################################################################
# Name:        build/cmake/lib/webview_chromium/cef_update_version_info.cmake
# Purpose:     Script to update
# Author:      Tobias Taschner
# Created:     2018-02-03
# Copyright:   (c) 2018 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# Use this script to automatically update cef_version_info.cmake
#
#  Run this script with cmake script mode
#   cmake -D CEF_VERSION=x.y.z+b -P cef_update_version_info.cmake

if(NOT DEFINED CEF_VERSION)
    message(FATAL_ERROR "CEF_VERSION not defined")
endif()

set(CEF_BASE_URL "https://cef-builds.spotifycdn.com/cef_binary_")
set(CEF_DISTRIBUTION "_minimal")
set(CEF_FILE_EXT ".tar.bz2")

set(sha_file ${CMAKE_BINARY_DIR}/__info_sha.txt)
foreach(platform windows64 windowsarm64 windows32 macosx64 macosarm64 linux64 linuxarm64 linuxarm)
    file(DOWNLOAD "${CEF_BASE_URL}${CEF_VERSION}_${platform}${CEF_DISTRIBUTION}${CEF_FILE_EXT}.sha1" "${sha_file}")
    file(READ "${sha_file}" CEF_SHA1_${platform})
endforeach()

file(REMOVE ${sha_file})

configure_file(
    cef_version_info.cmake.in
    cef_version_info.cmake
    @ONLY NEWLINE_STYLE LF
)
