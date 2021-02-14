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

wx_add_library(wxwebview ${WEBVIEW_FILES})

if(WXGTK AND wxUSE_WEBVIEW_WEBKIT2)
    if(wxVERSION_IS_DEV)
        set(WX_WEB_EXTENSIONS_DIRECTORY "lib/wx/${wxMAJOR_VERSION}.${wxMINOR_VERSION}.${wxRELEASE_NUMBER}/web-extensions")
    else()
        set(WX_WEB_EXTENSIONS_DIRECTORY "lib/wx/${wxMAJOR_VERSION}.${wxMINOR_VERSION}/web-extensions")
    endif()
endif()

if(APPLE)
    wx_lib_link_libraries(wxwebview PUBLIC "-framework WebKit")
elseif(WXMSW)
    if(wxUSE_WEBVIEW_EDGE)
        # Update the following variables if updating WebView2 SDK
        set(WEBVIEW2_VERSION "1.0.705.50")
        set(WEBVIEW2_URL "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${WEBVIEW2_VERSION}")
        set(WEBVIEW2_SHA256 "6a34bb553e18cfac7297b4031f3eac2558e439f8d16a45945c22945ac404105d")

        set(WEBVIEW2_DEFAULT_PACKAGE_DIR "${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2.${WEBVIEW2_VERSION}")

        find_path(WEBVIEW2_PACKAGE_DIR
            NAMES build/native/include/WebView2.h
            PATHS
                "${PROJECT_SOURCE_DIR}/3rdparty/webview2"
                ${WEBVIEW2_DEFAULT_PACKAGE_DIR}
        )
        mark_as_advanced(WEBVIEW2_PACKAGE_DIR)

        if (NOT WEBVIEW2_PACKAGE_DIR)
            message(STATUS "WebView2 SDK not found locally, downloading...")
            set(WEBVIEW2_PACKAGE_DIR ${WEBVIEW2_DEFAULT_PACKAGE_DIR} CACHE PATH "WebView2 SDK PATH" FORCE)
            file(DOWNLOAD
                ${WEBVIEW2_URL}
                ${CMAKE_BINARY_DIR}/webview2.nuget
                EXPECTED_HASH SHA256=${WEBVIEW2_SHA256})
            file(MAKE_DIRECTORY ${WEBVIEW2_PACKAGE_DIR})
            execute_process(COMMAND
                ${CMAKE_COMMAND} -E tar x ${CMAKE_BINARY_DIR}/webview2.nuget
                WORKING_DIRECTORY ${WEBVIEW2_PACKAGE_DIR}
            )
        endif()

        wx_lib_include_directories(wxwebview PRIVATE "${WEBVIEW2_PACKAGE_DIR}/build/native/include")

        if (CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(WEBVIEW2_ARCH x86)
        else()
            set(WEBVIEW2_ARCH x64)
        endif()

        add_custom_command(TARGET wxwebview POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${WEBVIEW2_PACKAGE_DIR}/build/native/${WEBVIEW2_ARCH}/WebView2Loader.dll"
            "$<TARGET_FILE_DIR:wxwebview>/WebView2Loader.dll")
    endif()
elseif(WXGTK)
    if(LIBSOUP_FOUND)
        wx_lib_include_directories(wxwebview PUBLIC ${LIBSOUP_INCLUDE_DIRS})
        wx_lib_link_libraries(wxwebview PUBLIC ${LIBSOUP_LIBRARIES})
    endif()
    if(wxUSE_WEBVIEW_WEBKIT2)
        wx_lib_include_directories(wxwebview PUBLIC ${WEBKIT2_INCLUDE_DIR})
        wx_lib_link_libraries(wxwebview PUBLIC ${WEBKIT2_LIBRARIES})
    elseif(wxUSE_WEBVIEW_WEBKIT)
        wx_lib_include_directories(wxwebview PUBLIC ${WEBKIT_INCLUDE_DIR})
        wx_lib_link_libraries(wxwebview PUBLIC ${WEBKIT_LIBRARIES})
    endif()
endif()

wx_finalize_lib(wxwebview)

# webkit extension plugin
# we can't use (all of the) macros and functions because this library should
# always be build as a shared libary, and not included in the monolithic build.
if(WXGTK AND wxUSE_WEBVIEW_WEBKIT2)
    wx_append_sources(WEBKIT2_EXT_FILES WEBVIEW_WEBKIT2_EXTENSION)
    add_library(wxwebkit2_ext SHARED ${WEBKIT2_EXT_FILES})
    wx_set_target_properties(wxwebkit2_ext false)

    # Change output name to match expected name in webview_webkit2.cpp: webkit2_ext*
    if(wxUSE_UNICODE)
        set(lib_unicode u)
    endif()
    set_target_properties(wxwebkit2_ext PROPERTIES PREFIX "")
    set_target_properties(wxwebkit2_ext PROPERTIES
        OUTPUT_NAME "webkit2_ext${lib_unicode}-${wxMAJOR_VERSION}.${wxMINOR_VERSION}"
        OUTPUT_NAME_DEBUG "webkit2_ext${lib_unicode}d-${wxMAJOR_VERSION}.${wxMINOR_VERSION}"
        )

    target_include_directories(wxwebkit2_ext PUBLIC
        ${LIBSOUP_INCLUDE_DIRS}
        ${WEBKIT2_INCLUDE_DIR}
        )
    target_link_libraries(wxwebkit2_ext PUBLIC
        ${LIBSOUP_LIBRARIES}
        ${WEBKIT2_LIBRARIES}
        )

    wx_install(TARGETS wxwebkit2_ext LIBRARY DESTINATION ${WX_WEB_EXTENSIONS_DIRECTORY})

    add_dependencies(wxwebview wxwebkit2_ext)
endif()
