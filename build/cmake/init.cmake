#############################################################################
# Name:        build/cmake/init.cmake
# Purpose:     Initialize variables based on user selection and system
#              information before creating build targets
# Author:      Tobias Taschner
# Created:     2016-09-24
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(DEFINED wxBUILD_USE_STATIC_RUNTIME AND wxBUILD_USE_STATIC_RUNTIME)
    # Set MSVC runtime flags for all configurations
    foreach(cfg "" ${CMAKE_CONFIGURATION_TYPES})
        set(flag_var CMAKE_CXX_FLAGS)
        if(cfg)
            string(TOUPPER ${cfg} cfg_upper)
            wx_string_append(flag_var "_${cfg_upper}")
        endif()
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()

if(wxBUILD_MSVC_MULTIPROC)
    wx_string_append(CMAKE_C_FLAGS " /MP")
    wx_string_append(CMAKE_CXX_FLAGS " /MP")
endif()

if(wxBUILD_COMPATIBILITY VERSION_LESS 3.0)
    set(WXWIN_COMPATIBILITY_2_8 ON)
endif()
if(wxBUILD_COMPATIBILITY VERSION_LESS 3.1)
    set(WXWIN_COMPATIBILITY_3_0 ON)
endif()

# Build wxBUILD_FILE_ID used for config and setup path
#TODO: build different id for WIN32
set(wxBUILD_FILE_ID "${wxBUILD_TOOLKIT}${wxBUILD_WIDGETSET}-")
if(wxUSE_UNICODE)
    wx_string_append(wxBUILD_FILE_ID "unicode")
else()
    wx_string_append(wxBUILD_FILE_ID "ansi")
endif()
if(NOT wxBUILD_SHARED)
    wx_string_append(wxBUILD_FILE_ID "-static")
endif()
wx_string_append(wxBUILD_FILE_ID "-${wxMAJOR_VERSION}.${wxMINOR_VERSION}")

set(wxARCH_SUFFIX)

# TODO: include compiler version in wxCOMPILER_PREFIX ?
if(WIN32)
    if(MSVC)
        set(wxCOMPILER_PREFIX "vc")
    elseif(CMAKE_COMPILER_IS_GNUCXX)
        set(wxCOMPILER_PREFIX "gcc")
    else()
        message(FATAL_ERROR "Unknown WIN32 compiler type")
    endif()

    if(CMAKE_CL_64)
        set(wxARCH_SUFFIX "_x64")
    endif()
else()
    set(wxCOMPILER_PREFIX)
endif()

if(MSVC OR MINGW)
    if(wxBUILD_SHARED)
        set(lib_suffix "dll")
    else()
        set(lib_suffix "lib")
    endif()

    if(MSVC)
        # Include generator expression to supress default Debug/Release pair
        set(wxPLATFORM_LIB_DIR "$<1:/>${wxCOMPILER_PREFIX}${wxARCH_SUFFIX}_${lib_suffix}")
    else()
        set(wxPLATFORM_LIB_DIR "/${wxCOMPILER_PREFIX}${wxARCH_SUFFIX}_${lib_suffix}")
    endif()
else()
    set(wxPLATFORM_LIB_DIR)
endif()

if(wxBUILD_CUSTOM_SETUP_HEADER_PATH)
    if(NOT EXISTS "${wxBUILD_CUSTOM_SETUP_HEADER_PATH}/wx/setup.h")
        message(FATAL_ERROR "wxBUILD_CUSTOM_SETUP_HEADER_PATH needs to contain a wx/setup.h file")
    endif()
    set(wxSETUP_HEADER_PATH ${wxBUILD_CUSTOM_SETUP_HEADER_PATH})
else()
    # Set path where setup.h will be created
    if(MSVC OR MINGW)
        if(wxUSE_UNICODE)
            set(lib_unicode u)
        else()
            set(lib_unicode)
        endif()
        set(wxSETUP_HEADER_PATH
            ${wxOUTPUT_DIR}/${wxCOMPILER_PREFIX}${wxARCH_SUFFIX}_${lib_suffix}/${wxBUILD_TOOLKIT}${lib_unicode})
        file(MAKE_DIRECTORY ${wxSETUP_HEADER_PATH}/wx)
        file(MAKE_DIRECTORY ${wxSETUP_HEADER_PATH}d/wx)
        set(wxSETUP_HEADER_FILE_DEBUG ${wxSETUP_HEADER_PATH}d/wx/setup.h)
    else()
        set(wxSETUP_HEADER_PATH
            ${wxOUTPUT_DIR}/wx/include/${wxBUILD_FILE_ID})
        file(MAKE_DIRECTORY ${wxSETUP_HEADER_PATH}/wx)
    endif()
endif()
set(wxSETUP_HEADER_FILE ${wxSETUP_HEADER_PATH}/wx/setup.h)

if(NOT wxBUILD_CUSTOM_SETUP_HEADER_PATH AND MSVC)
    # Append configuration specific suffix to setup header path
    wx_string_append(wxSETUP_HEADER_PATH "$<$<CONFIG:Debug>:d>")
endif()

if(wxUSE_ON_FATAL_EXCEPTION AND MSVC AND (MSVC_VERSION GREATER 1800) )
    # see include/wx/msw/seh.h for more details
    add_compile_options("/EHa")
endif()

# Constants for setup.h creation
set(wxUSE_STD_DEFAULT ON)
if(wxUSE_UNICODE)
    set(wxUSE_WCHAR_T ON)
endif()
if(NOT wxUSE_EXPAT)
    set(wxUSE_XRC OFF)
endif()
set(wxUSE_XML ${wxUSE_XRC})
if(wxUSE_CONFIG)
    set(wxUSE_CONFIG_NATIVE ON)
endif()

if(DEFINED wxUSE_OLE AND wxUSE_OLE)
    set(wxUSE_OLE_AUTOMATION ON)
endif()

if(DEFINED wxUSE_GRAPHICS_DIRECT2D AND NOT wxUSE_GRAPHICS_CONTEXT)
    set(wxUSE_GRAPHICS_DIRECT2D OFF)
endif()

if(wxUSE_OPENGL)
    set(wxUSE_GLCANVAS ON)
endif()

if(wxUSE_THREADS)
    find_package(Threads REQUIRED)
endif()

if(wxUSE_GUI)
    if(WIN32 AND wxUSE_METAFILE)
        # this one should probably be made separately configurable
        set(wxUSE_ENH_METAFILE ON)
    endif()

    if(wxUSE_OPENGL)
        find_package(OpenGL)
        if(NOT OPENGL_FOUND)
            message(WARNING "opengl not found, wxGLCanvas won't be available")
            wx_option_force_value(wxUSE_OPENGL OFF)
        endif()
    endif()

    if(wxUSE_WEBVIEW AND WXGTK)
        find_package(LibSoup)
        find_package(Webkit)
        if(NOT WEBKIT_FOUND OR NOT LIBSOUP_FOUND)
            message(WARNING "webkit not found, wxWebview won't be available")
            wx_option_force_value(wxUSE_WEBVIEW OFF)
        endif()
    endif()

    if(wxUSE_MEDIACTRL AND UNIX AND NOT APPLE AND NOT WIN32)
        find_package(GStreamer)
        if(NOT GSTREAMER_FOUND)
            message(WARNING "GStreamer not found, wxMediaCtrl won't be available")
            wx_option_force_value(wxUSE_MEDIACTRL OFF)
        endif()
    endif()
endif()
