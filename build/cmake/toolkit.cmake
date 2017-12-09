#############################################################################
# Name:        build/cmake/toolkit.cmake
# Purpose:     CMake platform toolkit options
# Author:      Tobias Taschner
# Created:     2016-10-03
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# Options required for toolkit selection/detection
wx_option(wxUSE_GUI "Use GUI" ON)

if(CMAKE_OSX_SYSROOT MATCHES iphoneos)
    set(IPHONE ON)
endif()

if(WIN32)
    set(wxDEFAULT_TOOLKIT msw)
    set(wxTOOLKIT_OPTIONS msw gtk2 gtk3 qt)
    set(wxPLATFORM WIN32)
elseif(APPLE AND IPHONE)
    set(wxDEFAULT_TOOLKIT osx_iphone)
    set(wxTOOLKIT_OPTIONS osx_iphone)
    set(wxPLATFORM OSX)
elseif(APPLE)
    set(wxDEFAULT_TOOLKIT osx_cocoa)
    set(wxTOOLKIT_OPTIONS osx_cocoa gtk2 gtk3 qt)
    set(wxPLATFORM OSX)
elseif(UNIX)
    set(wxDEFAULT_TOOLKIT gtk2)
    set(wxTOOLKIT_OPTIONS gtk2 gtk3 motif qt)
    set(wxPLATFORM UNIX)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

wx_option(wxBUILD_TOOLKIT "Toolkit used by wxWidgets" ${wxDEFAULT_TOOLKIT}
    STRINGS ${wxTOOLKIT_OPTIONS})
# TODO: set to univ for universal build
set(wxBUILD_WIDGETSET "")

if(NOT wxUSE_GUI)
    set(wxBUILD_TOOLKIT "base")
endif()

# Create shortcut variable for easy toolkit tests
string(TOUPPER ${wxBUILD_TOOLKIT} toolkit_upper)
set(WX${toolkit_upper} ON)
if(wxBUILD_TOOLKIT MATCHES "^gtk*")
    set(WXGTK ON)
elseif(wxBUILD_TOOLKIT MATCHES "^osx*")
    set(WXOSX ON)
endif()

set(wxTOOLKIT_DEFINITIONS __WX${toolkit_upper}__)

# Initialize toolkit variables
if(wxUSE_GUI)
set(wxTOOLKIT_INCLUDE_DIRS)
set(wxTOOLKIT_LIBRARIES)
set(wxTOOLKIT_VERSION)

if(UNIX AND NOT APPLE AND NOT WIN32)
    find_package(X11 REQUIRED)
    list(APPEND wxTOOLKIT_INCLUDE_DIRS ${X11_INCLUDE_DIR})
    list(APPEND wxTOOLKIT_LIBRARIES ${X11_LIBRARIES})
endif()

if(WXMSW)
    set(wxTOOLKIT_LIBRARIES
        gdi32
        comdlg32
        winspool
        shell32
        comctl32
        rpcrt4
        Oleacc
        )
elseif(WXGTK)
    if(WXGTK3)
        set(gtk_lib GTK3)
    elseif(WXGTK2)
        set(gtk_lib GTK2)
    endif()

    find_package(${gtk_lib} REQUIRED)
    list(APPEND wxTOOLKIT_INCLUDE_DIRS ${${gtk_lib}_INCLUDE_DIRS})
    list(APPEND wxTOOLKIT_LIBRARIES ${${gtk_lib}_LIBRARIES})
    list(APPEND wxTOOLKIT_DEFINITIONS ${${gtk_lib}_DEFINITIONS})
    list(APPEND wxTOOLKIT_DEFINITIONS __WXGTK__)
    set(wxTOOLKIT_VERSION ${${gtk_lib}_VERSION})
endif()

if(APPLE)
    list(APPEND wxTOOLKIT_DEFINITIONS __WXMAC__ __WXOSX__)
endif()
endif() # wxUSE_GUI
