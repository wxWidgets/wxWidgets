#############################################################################
# Name:        build/cmake/lib/qa/CMakeLists.txt
# Purpose:     CMake file for qa library
# Author:      Tobias Taschner
# Created:     2016-10-03
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_append_sources(QA_FILES QA)

wx_add_library(wxqa ${QA_FILES})
wx_lib_link_libraries(wxqa PUBLIC wxxml)

wx_finalize_lib(wxqa)
