#############################################################################
# Name:        build/cmake/lib/qa/CMakeLists.txt
# Purpose:     CMake file for qa library
# Author:      Tobias Taschner
# Created:     2016-10-03
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(NOT wxUSE_DEBUGREPORT)
    wx_append_install_headers(QA_HDR)
    return()
endif()

wx_append_sources(QA_FILES QA)

wx_add_library(wxqa ${QA_FILES})
wx_lib_link_libraries(wxqa PUBLIC wxxml)
