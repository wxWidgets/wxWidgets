#############################################################################
# Name:        build/cmake/lib/net/CMakeLists.txt
# Purpose:     CMake file for net library
# Author:      Tobias Taschner
# Created:     2016-09-21
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

wx_append_sources(NET_FILES NET_CMN)

if(WIN32)
    wx_append_sources(NET_FILES NET_WIN32)
elseif(APPLE)
    wx_append_sources(NET_FILES NET_OSX)
endif()

if(UNIX AND NOT WIN32)
    wx_append_sources(NET_FILES NET_UNIX)
endif()

wx_add_library(wxnet IS_BASE ${NET_FILES})

if(WIN32)
    wx_lib_link_libraries(wxnet PRIVATE ws2_32)

    if(wxUSE_WEBREQUEST_WINHTTP)
        wx_lib_link_libraries(wxnet PRIVATE winhttp)
    endif()
endif()

if (wxUSE_WEBREQUEST_CURL)
    wx_lib_include_directories(wxnet ${CURL_INCLUDE_DIRS})
    wx_lib_link_libraries(wxnet PRIVATE ${CURL_LIBRARIES})
endif()
