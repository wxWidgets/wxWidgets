#############################################################################
# Name:        build/cmake/build_cfg.cmake
# Purpose:     Create and configure build.cfg
# Author:      Maarten Bent
# Created:     2021-06-17
# Copyright:   (c) 2021 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

macro(wx_buildfile_var var)
    # convert TRUE/FALSE to 1/0, add _bf suffix for use in build.cfg
    if(${var})
        set(${var}_bf 1)
    else()
        set(${var}_bf 0)
    endif()
endmacro()

wx_buildfile_var(wxBUILD_MONOLITHIC)
wx_buildfile_var(wxBUILD_SHARED)
wx_buildfile_var(wxUSE_EXCEPTIONS)
wx_buildfile_var(wxUSE_THREADS)
wx_buildfile_var(wxUSE_AUI)
wx_buildfile_var(wxUSE_GUI)
wx_buildfile_var(wxUSE_HTML)
wx_buildfile_var(wxUSE_MEDIACTRL)
wx_buildfile_var(wxUSE_OPENGL)
wx_buildfile_var(wxUSE_DEBUGREPORT)
wx_buildfile_var(wxUSE_PROPGRID)
wx_buildfile_var(wxUSE_RIBBON)
wx_buildfile_var(wxUSE_RICHTEXT)
wx_buildfile_var(wxUSE_STC)
wx_buildfile_var(wxUSE_WEBVIEW)
wx_buildfile_var(wxUSE_XRC)

if(wxUSE_NO_RTTI)
    set(wxUSE_RTTI 0)
else()
    set(wxUSE_RTTI 1)
endif()
if(wxBUILD_STRIPPED_RELEASE)
    set(wxDEBUG_INFO 0)
else()
    set(wxDEBUG_INFO 1)
endif()
if(wxBUILD_USE_STATIC_RUNTIME)
    set(wxRUNTIME_LIBS "static")
else()
    set(wxRUNTIME_LIBS "dynamic")
endif()

set(wxDEBUG_FLAG ${wxBUILD_DEBUG_LEVEL})
get_filename_component(wxCC ${CMAKE_C_COMPILER} NAME_WE)
get_filename_component(wxCXX ${CMAKE_CXX_COMPILER} NAME_WE)
set(wxCFLAGS ${CMAKE_C_FLAGS})
set(wxCPPFLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")
set(wxCXXFLAGS ${CMAKE_CXX_FLAGS})
set(wxLDFLAGS ${CMAKE_EXE_LINKER_FLAGS})

# These are currently not used by CMake
set(wxCFG "")
set(wxUNIV 0)
set(wxOFFICIAL_BUILD 0)
set(wxCOMPILER_VERSION "")

set(wxBUILD "release")
configure_file(build/cmake/build.cfg.in ${wxBUILD_FILE})

set(wxBUILD "debug")
configure_file(build/cmake/build.cfg.in ${wxBUILD_FILE_DEBUG})
