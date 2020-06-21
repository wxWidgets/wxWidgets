#############################################################################
# Name:        build/cmake/lib/core/CMakeLists.txt
# Purpose:     CMake file for core library
# Author:      Tobias Taschner
# Created:     2016-10-01
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_append_sources(CORE_SRC GUI_CMN)
wx_append_sources(CORE_SRC BASE_AND_GUI_CMN)
if(WIN32)
    wx_append_sources(CORE_SRC BASE_AND_GUI_WIN32)
elseif(UNIX)
    wx_append_sources(CORE_SRC UNIX)
    if(wxUSE_LIBSDL)
        wx_append_sources(CORE_SRC UNIX_SOUND_SDL)
    endif()
endif()

if(WXMSW)
    wx_append_sources(CORE_SRC MSW_LOWLEVEL)
    wx_append_sources(CORE_SRC MSW_DESKTOP_LOWLEVEL)
    wx_append_sources(CORE_SRC MSW)
    wx_append_sources(CORE_SRC MSW_DESKTOP)
    if(NOT wxUSE_OLE)
        wx_list_add_prefix(CORE_SRC "${wxSOURCE_DIR}/" "src/generic/dirdlgg.cpp")
    endif()
elseif(WXGTK)
    if(WXGTK2)
        wx_append_sources(CORE_SRC GTK2_LOWLEVEL)
        wx_append_sources(CORE_SRC GTK2)
    else()
        wx_append_sources(CORE_SRC GTK_LOWLEVEL)
        wx_append_sources(CORE_SRC GTK)
    endif()

    if(UNIX)
        wx_append_sources(CORE_SRC XWIN_LOWLEVEL)
    elseif(WIN32)
        wx_append_sources(CORE_SRC GTK_WIN32)
    endif()
elseif(WXOSX_COCOA)
    wx_append_sources(CORE_SRC BASE_AND_GUI_OSX_COCOA)
    wx_append_sources(CORE_SRC OSX_LOWLEVEL)
    wx_append_sources(CORE_SRC OSX_SHARED)
    wx_append_sources(CORE_SRC OSX_COCOA)
elseif(WXOSX_IPHONE)
    wx_append_sources(CORE_SRC OSX_LOWLEVEL)
    wx_append_sources(CORE_SRC OSX_IPHONE)
elseif(WXQT)
    wx_append_sources(CORE_SRC QT)
    if(WIN32)
        wx_append_sources(CORE_SRC QT_WIN32)
    endif()
endif()

wx_add_library(wxcore ${CORE_SRC})
foreach(lib JPEG PNG TIFF)
    if(${lib}_LIBRARIES)
        if(lib STREQUAL JPEG)
            wx_lib_include_directories(wxcore PRIVATE ${${lib}_INCLUDE_DIR})
        else()
            wx_lib_include_directories(wxcore PRIVATE ${${lib}_INCLUDE_DIRS})
        endif()

        wx_lib_link_libraries(wxcore PRIVATE ${${lib}_LIBRARIES})
    endif()
endforeach()

if(WIN32)
    wx_lib_link_libraries(wxcore PRIVATE winmm)
endif()
if(WXOSX_COCOA)
    wx_lib_link_libraries(wxcore PUBLIC "-framework AudioToolbox")
    if(wxUSE_WEBKIT)
        wx_lib_link_libraries(wxcore PUBLIC "-framework WebKit")
    endif()
endif()
if(WXOSX_IPHONE)
    wx_lib_link_libraries(wxcore
        PUBLIC
        "-framework AudioToolbox"
        "-framework CoreGraphics"
        "-framework CoreText"
        "-framework UIKit"
        )
endif()
if(WXGTK AND wxUSE_PRIVATE_FONTS)
    wx_lib_include_directories(wxcore PUBLIC ${FONTCONFIG_INCLUDE_DIRS} ${PANGOFT2_INCLUDE_DIRS})
    wx_lib_link_libraries(wxcore PUBLIC ${FONTCONFIG_LIBRARIES} ${PANGOFT2_LIBRARIES})
endif()
if(wxUSE_LIBSDL)
    if(SDL2_FOUND)
        wx_lib_include_directories(wxcore PUBLIC ${SDL2_INCLUDE_DIR})
        wx_lib_link_libraries(wxcore PUBLIC ${SDL2_LIBRARY})
    elseif(SDL_FOUND)
        wx_lib_include_directories(wxcore PUBLIC ${SDL_INCLUDE_DIR})
        wx_lib_link_libraries(wxcore PUBLIC ${SDL_LIBRARY})
    endif()
endif()
if(wxUSE_LIBNOTIFY)
    wx_lib_include_directories(wxcore PUBLIC ${LIBNOTIFY_INCLUDE_DIRS})
    wx_lib_link_libraries(wxcore PUBLIC ${LIBNOTIFY_LIBRARIES})
endif()

wx_finalize_lib(wxcore)
