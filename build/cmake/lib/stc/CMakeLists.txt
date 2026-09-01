#############################################################################
# Name:        build/cmake/lib/stc/CMakeLists.txt
# Purpose:     CMake file for stc library
# Author:      Tobias Taschner
# Created:     2016-10-04
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(NOT wxUSE_STC)
    wx_append_install_headers(STC_CMN_HDR)
    return()
endif()

include(scintilla.cmake)
include(lexilla.cmake)

wx_append_sources(STC_FILES STC_CMN)
if(WXOSX_COCOA)
    wx_append_sources(STC_FILES STC_OSX_COCOA)
endif()

wx_add_library(wxstc ${STC_FILES})

if(WXWINUI AND wxUSE_WINUI3 AND wxBUILD_TESTS STREQUAL "ALL")
    # ScintillaWX exposes one native timer identity only to the in-tree WinUI
    # lifetime test. Keep the adapter out of installed headers and shipping
    # wxstc builds, including the monolithic target created by the parent.
    set(wx_winui_stc_test_support_source
        "${wxSOURCE_DIR}/src/stc/ScintillaWX.cpp")
    set_property(SOURCE ${wx_winui_stc_test_support_source}
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  "${wxSOURCE_DIR}/build/cmake/lib"
        APPEND PROPERTY INCLUDE_DIRECTORIES
            "${wxSOURCE_DIR}/tests/winui/test-support")
    set_property(SOURCE ${wx_winui_stc_test_support_source}
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  "${wxSOURCE_DIR}/build/cmake/lib"
        APPEND PROPERTY COMPILE_DEFINITIONS
            WXWINUI_TEST_SUPPORT=1)
endif()

get_target_property(SCINTILLA_INCLUDE wxscintilla INCLUDE_DIRECTORIES)
get_target_property(LEXILLA_INCLUDE wxlexilla INCLUDE_DIRECTORIES)
wx_lib_include_directories(wxstc PRIVATE ${SCINTILLA_INCLUDE} ${LEXILLA_INCLUDE})

get_target_property(SCINTILLA_DEFINITIONS wxscintilla COMPILE_DEFINITIONS)
get_target_property(LEXILLA_DEFINITIONS wxlexilla COMPILE_DEFINITIONS)
wx_lib_compile_definitions(wxstc PRIVATE ${SCINTILLA_DEFINITIONS} ${LEXILLA_DEFINITIONS})

wx_lib_link_libraries(wxstc PRIVATE wxscintilla wxlexilla)
if(WXMSW)
    wx_lib_link_libraries(wxstc PRIVATE imm32)
endif()
