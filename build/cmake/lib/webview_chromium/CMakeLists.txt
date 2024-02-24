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

set(KNOWN_CONFIGS "Debug;Release;RelWithDebInfo;MinSizeRel")
if(CMAKE_CONFIGURATION_TYPES)
    foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
        if (NOT cfg IN_LIST KNOWN_CONFIGS)
            message(WARNING "Unknown build configuration '${cfg}', this might cause issues with libcef_dll_wrapper")
        endif()
    endforeach()
elseif(CMAKE_BUILD_TYPE)
    if (NOT CMAKE_BUILD_TYPE IN_LIST KNOWN_CONFIGS)
        message(WARNING "Unknown build configuration '${cfg}', this might cause issues with libcef_dll_wrapper")
    endif()
endif()


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

# prevent libcef_dll_wrapper from creating only Release and Debug configs
# in multi-configuration generators
# variable_watch does not seem to be scoped, and we can't unset it, or replace it,
# and we don't care if it is changed later, so use enable_guard
# to stop the guard from working after libcef_dll_wrapper is added.
set(enable_guard 1)
macro(set_readonly VAR)
    set(_${VAR}_ ${${VAR}})
    variable_watch(${VAR} readonly_guard)
endmacro()
macro(readonly_guard VAR access value)
    if (enable_guard AND "${access}" STREQUAL "MODIFIED_ACCESS" AND NOT "${value}" STREQUAL "${_${VAR}_}")
        set(${VAR} ${_${VAR}_})
        message(WARNING "Blocked changing variable '${VAR}' to '${value}', reset to '${${VAR}}'")
    endif()
endmacro()
set_readonly(CMAKE_CONFIGURATION_TYPES)

add_subdirectory(${CEF_ROOT} ${CEF_BUILD_DIR} EXCLUDE_FROM_ALL)

set(enable_guard 0)
set(CMAKE_MESSAGE_LOG_LEVEL ${_saved_CMAKE_MESSAGE_LOG_LEVEL})

set_target_properties(libcef_dll_wrapper PROPERTIES
    FOLDER "Third Party Libraries"
    OUTPUT_NAME "libcef_dll_wrapper"
)

if(NOT MSVC)
    target_compile_options(libcef_dll_wrapper PRIVATE "-Wno-extra")
endif()

# libcef_dll_wrapper only sets properties for Debug and Release.
# Extract the release options/flags and apply them to RelWithDebInfo and MinSizeRel.
macro(rls_flags property)
    get_target_property(props libcef_dll_wrapper ${property})
    string(FIND "${props}" "$<CONFIG:Release>:" index)
    math(EXPR index "${index}+18")
    string(SUBSTRING "${props}" ${index} -1 props)
    string(FIND "${props}" ">" index)
    string(SUBSTRING "${props}" 0 ${index} props)
    if ("${property}" STREQUAL "COMPILE_DEFINITIONS")
        target_compile_definitions(libcef_dll_wrapper PRIVATE
            $<$<CONFIG:RelWithDebInfo>:${props}>
            $<$<CONFIG:MinSizeRel>:${props}>
        )
    else()
        target_compile_options(libcef_dll_wrapper PRIVATE
            $<$<CONFIG:RelWithDebInfo>:${props}>
            $<$<CONFIG:MinSizeRel>:${props}>
        )
    endif()
endmacro()
rls_flags(COMPILE_DEFINITIONS)
rls_flags(COMPILE_OPTIONS)


add_library(libcef SHARED IMPORTED GLOBAL)
if(APPLE)
    set_target_properties(libcef PROPERTIES
        IMPORTED_LOCATION "${CEF_ROOT}/Release/Chromium Embedded Framework.framework/Chromium Embedded Framework"
    )
else()
    set_target_properties(libcef PROPERTIES
        IMPORTED_LOCATION "${CEF_ROOT}/Release/libcef${CMAKE_SHARED_LIBRARY_SUFFIX}"
        IMPORTED_IMPLIB "${CEF_ROOT}/Release/libcef${CMAKE_IMPORT_LIBRARY_SUFFIX}"
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
