#############################################################################
# Name:        build/cmake/lib/media/CMakeLists.txt
# Purpose:     CMake file for media library
# Author:      Tobias Taschner
# Created:     2016-10-03
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_append_sources(MEDIA_FILES MEDIA_CMN)

if(WXMSW)
    wx_append_sources(MEDIA_FILES MEDIA_MSW)
elseif(WXOSX_COCOA)
    wx_append_sources(MEDIA_FILES MEDIA_OSX_COCOA)
elseif(WXOSX_IPHONE)
    wx_append_sources(MEDIA_FILES MEDIA_OSX_IPHONE)
elseif(UNIX)
    wx_append_sources(MEDIA_FILES MEDIA_UNIX)
elseif(WXQT)
    wx_append_sources(MEDIA_FILES MEDIA_QT)
endif()

wx_add_library(wxmedia ${MEDIA_FILES})
if(WXOSX)
    wx_lib_link_libraries(wxmedia PUBLIC
        "-framework AVFoundation"
        "-framework CoreMedia"
        "-weak_framework AVKit"
        )
elseif(UNIX)
    wx_lib_include_directories(wxmedia PUBLIC ${GSTREAMER_INCLUDE_DIRS})
    if(GSTREAMER_INTERFACES_INCLUDE_DIRS)
        wx_lib_include_directories(wxmedia PUBLIC ${GSTREAMER_INTERFACES_INCLUDE_DIRS})
    endif()
    if(GSTREAMER_VIDEO_INCLUDE_DIRS)
        wx_lib_include_directories(wxmedia PUBLIC ${GSTREAMER_VIDEO_INCLUDE_DIRS})
    endif()
    if(GSTREAMER_PLAYER_INCLUDE_DIRS)
        wx_lib_include_directories(wxmedia PUBLIC ${GSTREAMER_PLAYER_INCLUDE_DIRS})
    endif()

    wx_lib_link_libraries(wxmedia PUBLIC ${GSTREAMER_LIBRARIES})
    if(GSTREAMER_INTERFACES_LIBRARIES)
        wx_lib_link_libraries(wxmedia PUBLIC ${GSTREAMER_INTERFACES_LIBRARIES})
    endif()
    if(GSTREAMER_VIDEO_LIBRARIES)
        wx_lib_link_libraries(wxmedia PUBLIC ${GSTREAMER_VIDEO_LIBRARIES})
    endif()
    if(GSTREAMER_PLAYER_LIBRARIES)
        wx_lib_link_libraries(wxmedia PUBLIC ${GSTREAMER_PLAYER_LIBRARIES})
    endif()
endif()

wx_finalize_lib(wxmedia)
