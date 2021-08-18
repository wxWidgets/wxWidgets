#############################################################################
# Name:        build/cmake/lib/ribbon/CMakeLists.txt
# Purpose:     CMake file for ribbon library
# Author:      Tobias Taschner
# Created:     2016-10-04
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_append_sources(RIBBON_FILES RIBBON)

wx_add_library(wxribbon ${RIBBON_FILES})

wx_finalize_lib(wxribbon)
