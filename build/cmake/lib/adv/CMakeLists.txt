#############################################################################
# Name:        build/cmake/lib/adv/CMakeLists.txt
# Purpose:     CMake file for adv library
# Author:      Tobias Taschner
# Created:     2016-10-03
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_append_sources(ADV_FILES ADVANCED_CMN)

if(WXMSW)
    wx_append_sources(ADV_FILES ADVANCED_MSW)
    wx_append_sources(ADV_FILES ADVANCED_MSW_NATIVE)
    wx_append_sources(ADV_FILES ADVANCED_MSW_DESKTOP)
elseif(WXOSX_COCOA)
    wx_append_sources(ADV_FILES ADVANCED_OSX_COCOA)
elseif(WXGTK)
    if(WXGTK2)
        wx_append_sources(ADV_FILES ADVANCED_GTK2)
    else()
        wx_append_sources(ADV_FILES ADVANCED_GTK)
    endif()
    wx_append_sources(ADV_FILES ADVANCED_GTK_NATIVE)
    if(WIN32)
        wx_append_sources(ADV_FILES ADVANCED_GTK_WIN32)
    endif()
endif()

if(UNIX AND NOT APPLE AND NOT WIN32)
    wx_append_sources(ADV_FILES ADVANCED_UNIX)
endif()

wx_add_library(adv ${ADV_FILES})
if(WIN32)
    wx_lib_link_libraries(adv PRIVATE
        winmm
    )
endif()
if(WXOSX_COCOA)
    wx_lib_link_libraries(adv PUBLIC
        "-framework AudioToolbox"
        )
endif()

wx_finalize_lib(adv)
