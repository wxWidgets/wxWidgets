#############################################################################
# Name:        build/cmake/lib/propgrid/CMakeLists.txt
# Purpose:     CMake file for propgrid library
# Author:      Tobias Taschner
# Created:     2016-10-04
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

wx_append_sources(PROPGRID_FILES PROPGRID)

wx_add_library(wxpropgrid ${PROPGRID_FILES})
