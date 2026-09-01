#############################################################################
# Name:        build/cmake/lib/propgrid/CMakeLists.txt
# Purpose:     CMake file for propgrid library
# Author:      Tobias Taschner
# Created:     2016-10-04
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(NOT wxUSE_PROPGRID)
    wx_append_install_headers(PROPGRID_HDR)
    return()
endif()

wx_append_sources(PROPGRID_FILES PROPGRID)

if(wxBUILD_TESTS STREQUAL "ALL")
    # Compile deterministic PropertyGrid seams only into in-tree tests.
    # Apply the properties to both the component and monolithic directories.
    set(wx_propgrid_test_support_sources
        "${wxSOURCE_DIR}/src/propgrid/manager.cpp")
    if(WXMSW)
        list(APPEND wx_propgrid_test_support_sources
            "${wxSOURCE_DIR}/src/propgrid/propgrid.cpp")
    endif()
    set_property(SOURCE ${wx_propgrid_test_support_sources}
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  "${wxSOURCE_DIR}/build/cmake/lib"
        APPEND PROPERTY INCLUDE_DIRECTORIES
            "${wxSOURCE_DIR}/tests/winui/test-support")
    set_property(SOURCE ${wx_propgrid_test_support_sources}
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  "${wxSOURCE_DIR}/build/cmake/lib"
        APPEND PROPERTY COMPILE_DEFINITIONS
            WXPROPGRID_TEST_SUPPORT=1)
endif()

wx_add_library(wxpropgrid ${PROPGRID_FILES})
