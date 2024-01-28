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

find_path(CEF_ROOT
    NAMES libcef_dll
    HINTS
        $ENV{CEF_ROOT}
        ${wxSOURCE_DIR}/3rdparty/cef
    DOC "CEF Binary Root directory"
    )

set(CEF_DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/cef-download)
set(CEF_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/cef-source)
set(CEF_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/cef-build)

if(NOT CEF_ROOT)
    message("Downloading CEF binary distribution...")

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cef_download.cmake.in
        ${CEF_DOWNLOAD_DIR}/CMakeLists.txt
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CEF_DOWNLOAD_DIR}
    )
    if(result)
        message(FATAL_ERROR "CMake step for cef failed: ${result}")
    endif()

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CEF_DOWNLOAD_DIR}
    )
    if(result)
        message(FATAL_ERROR "Build step for cef failed: ${result}")
    endif()

    set(CEF_ROOT ${CEF_SOURCE_DIR} CACHE PATH "CEF Binary Root directory" FORCE)
endif()

# CEF settings
set(USE_ATL OFF)        # Disable usage of ATL in CEF
set(USE_SANDBOX OFF)    # Disable usage of sandbox on windows

if(MSVC)
    if(wxBUILD_USE_STATIC_RUNTIME)
        set(CEF_RUNTIME_LIBRARY_FLAG "/MT" CACHE STRING "" FORCE)
    else()
        set(CEF_RUNTIME_LIBRARY_FLAG "/MD" CACHE STRING "" FORCE)
    endif()
endif()

set(_saved_CMAKE_MESSAGE_LOG_LEVEL ${CMAKE_MESSAGE_LOG_LEVEL})
set(CEF_SHOW_RESULTS FALSE CACHE BOOL "Show CEF configuration results")
if(NOT CEF_SHOW_RESULTS)
    set(CMAKE_MESSAGE_LOG_LEVEL ERROR)
endif()

add_subdirectory(${CEF_ROOT} ${CEF_BUILD_DIR} EXCLUDE_FROM_ALL)

set(CMAKE_MESSAGE_LOG_LEVEL ${_saved_CMAKE_MESSAGE_LOG_LEVEL})

set_target_properties(libcef_dll_wrapper PROPERTIES
    FOLDER "Third Party Libraries"
    OUTPUT_NAME "libcef_dll_wrapper"
)

if(NOT MSVC)
    set_target_properties(libcef_dll_wrapper PROPERTIES COMPILE_OPTIONS "-Wno-extra")
endif()

add_library(libcef SHARED IMPORTED GLOBAL)
if(APPLE)
    set_target_properties(libcef PROPERTIES
        IMPORTED_LOCATION_DEBUG "${CEF_ROOT}/Debug/Chromium Embedded Framework.framework/Chromium Embedded Framework"
        IMPORTED_LOCATION_RELEASE "${CEF_ROOT}/Release/Chromium Embedded Framework.framework/Chromium Embedded Framework"
    )
else()
    set_target_properties(libcef PROPERTIES
        IMPORTED_LOCATION ${CEF_ROOT}/Release/libcef${CMAKE_SHARED_LIBRARY_SUFFIX}
        IMPORTED_LOCATION_DEBUG ${CEF_ROOT}/Debug/libcef${CMAKE_SHARED_LIBRARY_SUFFIX}
        IMPORTED_IMPLIB_DEBUG ${CEF_ROOT}/Debug/libcef${CMAKE_IMPORT_LIBRARY_SUFFIX}
        IMPORTED_LOCATION_RELEASE ${CEF_ROOT}/Release/libcef${CMAKE_SHARED_LIBRARY_SUFFIX}
        IMPORTED_IMPLIB_RELEASE ${CEF_ROOT}/Release/libcef${CMAKE_IMPORT_LIBRARY_SUFFIX}
    )
endif()

wx_install(TARGETS libcef_dll_wrapper
    EXPORT wxWidgetsTargets
    ARCHIVE DESTINATION "lib${GEN_EXPR_DIR}${wxPLATFORM_LIB_DIR}"
)

wx_lib_include_directories(wxwebview PRIVATE ${CEF_ROOT})
wx_add_dependencies(wxwebview libcef_dll_wrapper)
wx_lib_link_libraries(wxwebview PUBLIC libcef libcef_dll_wrapper)

mark_as_advanced(USE_ATL)
mark_as_advanced(USE_SANDBOX)
mark_as_advanced(OPTION_USE_ARC)
mark_as_advanced(CEF_ROOT)
mark_as_advanced(CEF_SHOW_RESULTS)
mark_as_advanced(CEF_DEBUG_INFO_FLAG)
mark_as_advanced(CEF_RUNTIME_LIBRARY_FLAG)
