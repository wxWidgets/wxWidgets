#############################################################################
# Name:        build/cmake/lib/richtext/CMakeLists.txt
# Purpose:     CMake file for richtext library
# Author:      Tobias Taschner
# Created:     2016-10-04
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(NOT wxUSE_RICHTEXT)
    wx_append_install_headers(RICHTEXT_HDR)
    return()
endif()

wx_append_sources(RICHTEXT_FILES RICHTEXT)

wx_add_library(wxrichtext ${RICHTEXT_FILES})
wx_lib_link_libraries(wxrichtext PRIVATE wxhtml wxxml)
