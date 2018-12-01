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
    if(wxUSE_WEBVIEW_WEBKIT2)
        wx_append_sources(WEBVIEW_FILES WEBVIEW2_GTK)
    elseif(wxUSE_WEBVIEW_WEBKIT)
        wx_append_sources(WEBVIEW_FILES WEBVIEW_GTK)
    endif()
elseif(APPLE)
    wx_append_sources(WEBVIEW_FILES WEBVIEW_OSX_SHARED)
endif()

wx_add_library(webview ${WEBVIEW_FILES})

if(wxUSE_WEBVIEW_WEBKIT2)
    wx_lib_compile_definitions(webview PRIVATE
        -DWX_WEB_EXTENSIONS_DIRECTORY="${CMAKE_INSTALL_PREFIX}/web-extensions"
        )
endif()

if(APPLE)
    wx_lib_link_libraries(webview PUBLIC
        "-framework WebKit"
        )
elseif(WXGTK)
    if(LIBSOUP_FOUND)
        wx_lib_include_directories(webview PUBLIC ${LIBSOUP_INCLUDE_DIRS})
        wx_lib_link_libraries(webview PUBLIC ${LIBSOUP_LIBRARIES})
    endif()
    if(wxUSE_WEBVIEW_WEBKIT2)
        wx_lib_include_directories(webview PUBLIC ${WEBKIT2_INCLUDE_DIR})
        wx_lib_link_libraries(webview PUBLIC ${WEBKIT2_LIBRARIES})
    elseif(wxUSE_WEBVIEW_WEBKIT)
        wx_lib_include_directories(webview PUBLIC ${WEBKIT_INCLUDE_DIR})
        wx_lib_link_libraries(webview PUBLIC ${WEBKIT_LIBRARIES})
    endif()
endif()

wx_finalize_lib(webview)
