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

wx_add_library(qa ${QA_FILES})
wx_lib_link_libraries(qa PUBLIC xml)

wx_finalize_lib(qa)
