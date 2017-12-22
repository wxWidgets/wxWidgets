#############################################################################
# Name:        build/cmake/lib/html/CMakeLists.txt
# Purpose:     CMake file for html library
# Author:      Tobias Taschner
# Created:     2016-10-03
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_append_sources(HTML_FILES HTML_CMN)

if(WIN32)
    wx_append_sources(HTML_FILES HTML_MSW)
endif()

wx_add_library(html ${HTML_FILES})

wx_finalize_lib(html)
