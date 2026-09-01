#############################################################################
# Name:        build/cmake/lib/aui/CMakeLists.txt
# Purpose:     CMake file for aui library
# Author:      Tobias Taschner
# Created:     2016-10-04
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(NOT wxUSE_AUI)
    wx_append_install_headers(AUI_CMN_HDR)
    return()
endif()

wx_append_sources(AUI_FILES AUI_CMN)

if(WXWINUI)
    # The MSW sources are still built: only the tool bar art provider is
    # replaced, and wxAuiMSWToolBarArt remains reachable by name.
    wx_append_sources(AUI_FILES AUI_WINUI)
elseif(WXMSW)
    wx_append_sources(AUI_FILES AUI_MSW)
elseif(WXGTK2)
    wx_append_sources(AUI_FILES AUI_GTK)
endif()

if(wxBUILD_TESTS STREQUAL "ALL")
    # Compile the deterministic notebook seam only into in-tree test builds.
    # Source properties must be visible here and to the parent monolithic
    # target, without publishing the support include or definition.
    set(wx_aui_notebook_test_support_source
        "${wxSOURCE_DIR}/src/aui/auibook.cpp")
    set_property(SOURCE ${wx_aui_notebook_test_support_source}
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  "${wxSOURCE_DIR}/build/cmake/lib"
        APPEND PROPERTY INCLUDE_DIRECTORIES
            "${wxSOURCE_DIR}/tests/winui/test-support")
    set_property(SOURCE ${wx_aui_notebook_test_support_source}
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  "${wxSOURCE_DIR}/build/cmake/lib"
        APPEND PROPERTY COMPILE_DEFINITIONS
            WXAUI_NOTEBOOK_TEST_SUPPORT=1)
endif()

wx_add_library(wxaui ${AUI_FILES})
