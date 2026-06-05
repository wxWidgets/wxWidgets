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
