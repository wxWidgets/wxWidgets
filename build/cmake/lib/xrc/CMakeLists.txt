#############################################################################
# Name:        build/cmake/lib/xrc/CMakeLists.txt
# Purpose:     CMake file for xrc library
# Author:      Tobias Taschner
# Created:     2016-10-03
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

wx_append_sources(XRC_FILES XRC)

wx_add_library(wxxrc ${XRC_FILES})
wx_lib_link_libraries(wxxrc PRIVATE wxhtml wxxml)
