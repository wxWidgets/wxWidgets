#############################################################################
# Name:        build/cmake/tests/CMakeLists.txt
# Purpose:     Build test executables
# Author:      Tobias Taschner
# Created:     2016-09-24
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

add_subdirectory(base)

# Build GUI tests
if(wxUSE_GUI AND wxBUILD_TESTS STREQUAL "ALL")

add_subdirectory(drawing)
add_subdirectory(gui)

endif()
