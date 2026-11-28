#############################################################################
# Name:        build/cmake/lib/ribbon/CMakeLists.txt
# Purpose:     CMake file for ribbon library
# Author:      Tobias Taschner
# Created:     2016-10-04
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

option(wxWINUI3_EXPERIMENTAL_RIBBON_TOOLKIT
    "Build the experimental native Toolkit adapter in wxribbon (static build tree only)" OFF)
option(wxWINUI3_TOOLKIT_ACCEPT_EXPERIMENTAL_PACKAGES
    "Explicitly accept the pinned experimental XamlToolkit native packages" OFF)
option(wxWINUI3_TOOLKIT_ACCEPT_WIN2D_PACKAGE_LICENSE
    "Explicitly accept the license requirement declared by the pinned Win2D package" OFF)

if(wxWINUI3_EXPERIMENTAL_RIBBON_TOOLKIT)
    if(NOT WIN32 OR NOT MSVC OR NOT WXWINUI OR NOT wxUSE_WINUI3 OR NOT wxUSE_RIBBON)
        message(FATAL_ERROR
            "wxWINUI3_EXPERIMENTAL_RIBBON_TOOLKIT requires MSVC wxWinUI with wxUSE_WINUI3 and wxUSE_RIBBON")
    endif()
    if(wxBUILD_SHARED OR wxBUILD_MONOLITHIC)
        message(FATAL_ERROR
            "The experimental Toolkit Ribbon adapter currently requires a non-monolithic static build")
    endif()
    if(wxBUILD_INSTALL)
        message(FATAL_ERROR
            "The experimental Toolkit Ribbon adapter currently requires wxBUILD_INSTALL=OFF: "
            "its installed consumer/runtime payload contract is not implemented yet")
    endif()
    if(NOT wxWINUI3_TOOLKIT_ACCEPT_EXPERIMENTAL_PACKAGES OR
            NOT wxWINUI3_TOOLKIT_ACCEPT_WIN2D_PACKAGE_LICENSE)
        message(FATAL_ERROR
            "Toolkit Ribbon requires explicit wxWINUI3_TOOLKIT_ACCEPT_EXPERIMENTAL_PACKAGES=ON "
            "and wxWINUI3_TOOLKIT_ACCEPT_WIN2D_PACKAGE_LICENSE=ON; no acceptance is inferred")
    endif()
    include("${wxSOURCE_DIR}/build/cmake/winui-toolkit-runtime.cmake")
    # The helper verifies the exact cached archives/extractions and the native
    # dependency closure. It never downloads or changes the default projection.
    wx_winui3_prepare_toolkit_runtime(
        ACCEPT_EXPERIMENTAL_PACKAGES ACCEPT_WIN2D_PACKAGE_LICENSE)
endif()

if(NOT wxUSE_RIBBON)
    wx_append_install_headers(RIBBON_HDR)
    return()
endif()

wx_append_sources(RIBBON_FILES RIBBON)

if(wxWINUI3_EXPERIMENTAL_RIBBON_TOOLKIT)
    list(APPEND RIBBON_FILES
        "${wxSOURCE_DIR}/src/ribbon/ribbontoolkitruntime.cpp"
        "${wxSOURCE_DIR}/src/ribbon/ribboncontainerswinui.cpp"
        "${wxSOURCE_DIR}/src/ribbon/ribbonflyoutwinui.cpp")
endif()

wx_add_library(wxribbon ${RIBBON_FILES})

if(WXWINUI AND wxUSE_WINUI3)
    # Native command peers share wxcore's private host and its pinned WinRT
    # projection. The default library has no Toolkit dependency.
    wx_winui3_configure_target(wxribbon)
    if(wxWINUI3_EXPERIMENTAL_RIBBON_TOOLKIT)
        wx_winui3_configure_toolkit_target(wxribbon)
        target_compile_definitions(wxribbon PRIVATE WXWINUI_RIBBON_TOOLKIT=1)
        message(STATUS
            "wxRibbon: experimental native Toolkit adapter enabled (pinned cache-only runtime, build-tree deployment)")
    endif()
endif()
