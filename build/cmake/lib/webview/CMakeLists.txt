#############################################################################
# Name:        build/cmake/lib/webview/CMakeLists.txt
# Purpose:     CMake file for webview library
# Author:      Tobias Taschner
# Created:     2016-10-03
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

wx_append_sources(WEBVIEW_FILES WEBVIEW_CMN)

if(WXMSW)
    wx_append_sources(WEBVIEW_FILES WEBVIEW_MSW)
elseif(WXGTK)
    wx_append_sources(WEBVIEW_FILES WEBVIEW_GTK)
elseif(APPLE)
    wx_append_sources(WEBVIEW_FILES WEBVIEW_OSX_SHARED)
endif()

wx_add_library(webview ${WEBVIEW_FILES})
if(APPLE)
    wx_lib_link_libraries(webview PUBLIC
        "-framework WebKit"
        )
elseif(WXGTK)
    wx_lib_include_directories(webview PUBLIC
        ${WEBKIT_INCLUDE_DIR}
        ${LIBSOUP_INCLUDE_DIRS}
        )
    wx_lib_link_libraries(webview PUBLIC
        ${WEBKIT_LIBRARIES}
        ${LIBSOUP_LIBRARIES}
        )
endif()

wx_finalize_lib(webview)
