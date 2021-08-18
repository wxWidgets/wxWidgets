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

if(WIN32 OR wxUSE_LIBMSPACK)
    wx_append_sources(HTML_FILES HTML_MSW)
endif()

wx_add_library(wxhtml ${HTML_FILES})

if(wxUSE_LIBMSPACK)
    wx_lib_include_directories(wxhtml PRIVATE ${MSPACK_INCLUDE_DIRS})
    wx_lib_link_libraries(wxhtml PRIVATE ${MSPACK_LIBRARIES})
endif()

wx_finalize_lib(wxhtml)
