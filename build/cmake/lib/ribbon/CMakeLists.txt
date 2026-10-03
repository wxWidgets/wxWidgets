#############################################################################
# Name:        build/cmake/lib/ribbon/CMakeLists.txt
# Purpose:     CMake file for ribbon library
# Author:      Tobias Taschner
# Created:     2016-10-04
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(NOT wxUSE_RIBBON)
    wx_append_install_headers(RIBBON_HDR)
    return()
endif()

wx_append_sources(RIBBON_FILES RIBBON)

wx_add_library(wxribbon ${RIBBON_FILES})

if(WXWINUI AND wxUSE_WINUI3)
    # Native command peers share wxcore's private host and its pinned WinRT
    # projection. This adds no Toolkit dependency to the shipping library.
    wx_winui3_configure_target(wxribbon)
endif()
