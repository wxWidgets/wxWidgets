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

if(WXGTK AND wxUSE_WEBVIEW_WEBKIT2)
    set(WX_WEB_EXTENSIONS_DIRECTORY "lib/wx/${wxMAJOR_VERSION}.${wxMINOR_VERSION}/web-extensions")
    wx_lib_compile_definitions(webview PRIVATE
        -DWX_WEB_EXTENSIONS_DIRECTORY="${CMAKE_INSTALL_PREFIX}/${WX_WEB_EXTENSIONS_DIRECTORY}"
        )
endif()

if(APPLE)
    wx_lib_link_libraries(webview PUBLIC "-framework WebKit")
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

# webkit extension plugin
# we can't use (all of the) macros and functions because this library should
# always be build as a shared libary, and not included in the monolithic build.
if(WXGTK AND wxUSE_WEBVIEW_WEBKIT2)
    wx_append_sources(WEBKIT2_EXT_FILES WEBVIEW_WEBKIT2_EXTENSION)
    add_library(webkit2_ext SHARED ${WEBKIT2_EXT_FILES})
    wx_set_target_properties(webkit2_ext false)

    # Change output name to match expected name in webview_webkit2.cpp: webkit2_ext*
    if(wxUSE_UNICODE)
        set(lib_unicode u)
    endif()
    set_target_properties(webkit2_ext PROPERTIES PREFIX "")
    set_target_properties(webkit2_ext PROPERTIES
        OUTPUT_NAME "webkit2_ext${lib_unicode}-${wxMAJOR_VERSION}.${wxMINOR_VERSION}"
        OUTPUT_NAME_DEBUG "webkit2_ext${lib_unicode}d-${wxMAJOR_VERSION}.${wxMINOR_VERSION}"
        )

    target_include_directories(webkit2_ext PUBLIC
        ${LIBSOUP_INCLUDE_DIRS}
        ${WEBKIT2_INCLUDE_DIR}
        )
    target_link_libraries(webkit2_ext PUBLIC
        ${LIBSOUP_LIBRARIES}
        ${WEBKIT2_LIBRARIES}
        )

    wx_install(TARGETS webkit2_ext LIBRARY DESTINATION ${WX_WEB_EXTENSIONS_DIRECTORY})

    add_dependencies(webview webkit2_ext)
endif()
