#############################################################################
# Name:        build/cmake/lib/adv/CMakeLists.txt
# Purpose:     CMake file for adv library
# Author:      Tobias Taschner
# Created:     2016-10-03
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_add_library(wxadv "${wxSOURCE_DIR}/src/common/dummy.cpp")

wx_finalize_lib(wxadv)
