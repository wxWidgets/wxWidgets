#############################################################################
# Name:        build/cmake/lib/webview_chromium/CMakeLists.txt
# Purpose:     CMake file for webview_chromium library
# Author:      Tobias Taschner
# Created:     2018-02-03
# Copyright:   (c) 2018 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

include(cef_version_info.cmake)

ExternalProject_Add(
    cef
    URL ${CEF_URL}
    URL_HASH SHA1=${CEF_SHA1}
    PATCH_COMMAND
        ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/fixup_cef.cmake
    CMAKE_ARGS -DUSE_ATL=OFF -DUSE_SANDBOX=OFF
    BUILD_COMMAND
        ${CMAKE_COMMAND} --build . --target libcef_dll_wrapper --config $<CONFIG>
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(cef SOURCE_DIR)
ExternalProject_Get_property(cef BINARY_DIR)
add_library(libcef_dll_wrapper STATIC IMPORTED)
get_property(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(HAVE_MULTI_CONFIG_GENERATOR)
    set_target_properties(libcef_dll_wrapper PROPERTIES
        IMPORTED_LOCATION_DEBUG ${BINARY_DIR}/libcef_dll_wrapper/Debug/libcef_dll_wrapper${CMAKE_STATIC_LIBRARY_SUFFIX}
        IMPORTED_LOCATION_RELEASE ${BINARY_DIR}/libcef_dll_wrapper/Release/libcef_dll_wrapper${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
else()
    set_target_properties(libcef_dll_wrapper PROPERTIES
        IMPORTED_LOCATION ${BINARY_DIR}/libcef_dll_wrapper/libcef_dll_wrapper${CMAKE_STATIC_LIBRARY_SUFFIX}
    )
endif()
add_library(libcef STATIC IMPORTED)
if(APPLE)
    set_target_properties(libcef PROPERTIES
        IMPORTED_LOCATION_DEBUG "${SOURCE_DIR}/Debug/Chromium Embedded Framework.framework/Chromium Embedded Framework"
        IMPORTED_LOCATION_RELEASE "${SOURCE_DIR}/Release/Chromium Embedded Framework.framework/Chromium Embedded Framework"
    )
else()
    set_target_properties(libcef PROPERTIES
        IMPORTED_LOCATION_DEBUG ${SOURCE_DIR}/Debug/libcef${CMAKE_LINK_LIBRARY_SUFFIX}
        IMPORTED_LOCATION_RELEASE ${SOURCE_DIR}/Release/libcef${CMAKE_LINK_LIBRARY_SUFFIX}
    )
endif()

wx_append_sources(WEBVIEW_CHROMIUM_FILES WEBVIEWCHROMIUM)
wx_add_library(webview_chromium ${WEBVIEW_CHROMIUM_FILES})
wx_lib_include_directories(webview_chromium PRIVATE ${SOURCE_DIR})
wx_lib_link_libraries(webview_chromium PUBLIC webview PRIVATE libcef libcef_dll_wrapper)
add_dependencies(webview_chromium cef)

wx_finalize_lib(webview)
