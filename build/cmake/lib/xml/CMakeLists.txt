#############################################################################
# Name:        build/cmake/lib/xml/CMakeLists.txt
# Purpose:     CMake file for xml library
# Author:      Tobias Taschner
# Created:     2016-09-20
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_append_sources(XML_FILES XML)
wx_add_library(wxxml IS_BASE ${XML_FILES})
wx_lib_link_libraries(wxxml PRIVATE ${EXPAT_LIBRARIES})
wx_lib_include_directories(wxxml PRIVATE ${EXPAT_INCLUDE_DIRS})

wx_finalize_lib(wxxml)
