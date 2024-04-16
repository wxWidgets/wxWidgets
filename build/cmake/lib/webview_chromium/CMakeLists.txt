#############################################################################
# Name:        build/cmake/lib/webview_chromium/CMakeLists.txt
# Purpose:     CMake file for webview_chromium library
# Author:      Tobias Taschner
# Created:     2018-02-03
# Copyright:   (c) 2018 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

include(../../source_groups.cmake)

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

# We may need to create multiple arch-specific directories, so define the
# variables containing the common root for all of them.
set(CEF_DOWNLOAD_ROOT ${CMAKE_CURRENT_BINARY_DIR}/cef-download)
set(CEF_SOURCE_ROOT ${CMAKE_CURRENT_BINARY_DIR}/cef-source)
set(CEF_BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR}/cef-build)

# But when we don't need multiple directories, just use them directly.
set(CEF_DOWNLOAD_DIR ${CEF_DOWNLOAD_ROOT})
set(CEF_SOURCE_DIR ${CEF_SOURCE_ROOT})
set(CEF_BUILD_DIR ${CEF_BUILD_ROOT})

# This function downloads and builds CEF for the specified platform.
function(wx_download_cef CEF_PLATFORM)
    message("Downloading CEF binary distribution for ${CEF_PLATFORM}...")

    set(CEF_URL "${CEF_BASE_URL}${CEF_VERSION}_${CEF_PLATFORM}${CEF_DISTRIBUTION}${CEF_FILE_EXT}")
    set(CEF_SHA1 "${CEF_SHA1_${CEF_PLATFORM}}")
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
endfunction()

if(NOT CEF_ROOT)
    include(cef_version_info.cmake)

    # Note that CMAKE_OSX_ARCHITECTURES is always defined under macOS, but is
    # empty if not explicitly specified, so do _not_ use if(DEFINED ...) here.
    if(CMAKE_OSX_ARCHITECTURES)
        foreach(osx_arch IN LISTS CMAKE_OSX_ARCHITECTURES)
            # Determine the matching CEF platform name.
            if(${osx_arch} STREQUAL "arm64")
                set(CEF_PLATFORM "macosarm64")
            elseif(${osx_arch} STREQUAL "x86_64")
                set(CEF_PLATFORM "macosx64")
            else()
                message(FATAL_ERROR "Building CEF for macOS architecture ${osx_arch} is not supported")
            endif()

            if(NOT first_cef_platform)
                # Download/unpack CEF files in the root directory and remember
                # that we did it by setting this variable.
                set(first_cef_platform ${CEF_PLATFORM})
            else()
                list(APPEND other_cef_platforms ${CEF_PLATFORM})

                # Use different subdirectories for the other architectures.
                set(CEF_DOWNLOAD_DIR "${CEF_DOWNLOAD_ROOT}/${CEF_PLATFORM}")
                set(CEF_SOURCE_DIR "${CEF_SOURCE_ROOT}/${CEF_PLATFORM}")
                set(CEF_BUILD_DIR "${CEF_BUILD_ROOT}/${CEF_PLATFORM}")
            endif()

            wx_download_cef(${CEF_PLATFORM})
        endforeach()

        # Now lipo all the binaries together unless this is a degenerate case
        # in which CMAKE_OSX_ARCHITECTURES contains only one element.
        if(other_cef_platforms)
            set(cef_binaries
                "cef_sandbox.a"
                "Chromium Embedded Framework.framework/Chromium Embedded Framework"
                "Chromium Embedded Framework.framework/Libraries/libEGL.dylib"
                "Chromium Embedded Framework.framework/Libraries/libGLESv2.dylib"
                "Chromium Embedded Framework.framework/Libraries/libvk_swiftshader.dylib"
            )

            foreach(cef_bin_file IN LISTS cef_binaries)
                set(lipo_command
                    lipo -create "Release/${cef_bin_file}"
                )

                foreach(cef_platform IN LISTS other_cef_platforms)
                    list(APPEND lipo_command "${cef_platform}/Release/${cef_bin_file}")
                endforeach()

                list(APPEND lipo_command "-output")
                list(APPEND lipo_command "Release/${cef_bin_file}.tmp")

                execute_process(
                    COMMAND ${lipo_command}
                    RESULT_VARIABLE result
                    WORKING_DIRECTORY ${CEF_SOURCE_ROOT}
                )
                if(result)
                    message(FATAL_ERROR "Running \"${lipo_command}\" in ${CEF_SOURCE_ROOT} failed: ${result}")
                endif()

                file(RENAME
                    "${CEF_SOURCE_ROOT}/Release/${cef_bin_file}.tmp"
                    "${CEF_SOURCE_ROOT}/Release/${cef_bin_file}"
                )
            endforeach()

            # Special case of arch-specific v8 snapshot file.
            foreach(cef_platform IN LISTS other_cef_platforms)
                # This one uses the standard arch name instead of CEF-specific
                # name used elsewhere just to make our life a bit more interesting.
                if(${cef_platform} STREQUAL "macosarm64")
                    set(cef_arch "arm64")
                elseif(${cef_platform} STREQUAL "macosx64")
                    set(cef_arch "x86_64")
                else()
                    message(FATAL_ERROR "Uknown v8 arch for CEF platform ${cef_platform}")
                endif()

                file(COPY_FILE
                    "${CEF_SOURCE_ROOT}/${cef_platform}/Release/Chromium Embedded Framework.framework/Resources/v8_context_snapshot.${cef_arch}.bin"
                    "${CEF_SOURCE_ROOT}/Release/Resources"
                )
            endforeach()

            # This file differs between the architectures, but has a fixed name, so
            # we can only remove it. FWIW it is not present in CEF included in
            # /Application/Google Chrome.app, so it's probably the right thing to do.
            file(REMOVE "${CEF_SOURCE_ROOT}/Release/Chrome Embedded Framework.framework/Resources/snapshot_blob.bin")
        endif()
    else()
        # Auto detect CEF platform.
        if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
            if(CMAKE_SIZEOF_VOID_P LESS 8)
                message(FATAL_ERROR "Unsupported macOS system")
            else()
                if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
                    set(CEF_PLATFORM "macosarm64")
                else()
                    set(CEF_PLATFORM "macosx64")
                endif()
            endif()
        elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            if(CMAKE_SIZEOF_VOID_P LESS 8)
                if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
                    set(CEF_PLATFORM "linuxarm")
                else()
                    message(FATAL_ERROR "Unsupported Linux system")
                endif()
            else()
                if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
                    set(CEF_PLATFORM "linuxarm64")
                else()
                    set(CEF_PLATFORM "linux64")
                endif()
            endif()
        elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
            if(CMAKE_SIZEOF_VOID_P LESS 8)
                if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm")
                    message(FATAL_ERROR "Unsupported Windows system")
                else()
                    set(CEF_PLATFORM "windows32")
                endif()
            else()
                if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
                    set(CEF_PLATFORM "windowsarm64")
                else()
                    set(CEF_PLATFORM "windows64")
                endif()
            endif()
        else()
            message(FATAL_ERROR "Unsupported CEF system")
        endif()

        wx_download_cef(${CEF_PLATFORM})
    endif()

    set(CEF_ROOT ${CEF_SOURCE_ROOT} CACHE PATH "CEF Binary Root directory" FORCE)
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
if(CEF_SHOW_RESULTS)
    if(CMAKE_OSX_ARCHITECTURES)
        # This is only used for the summary shown if CEF_SHOW_RESULTS is on.
        set(PROJECT_ARCH ${CMAKE_OSX_ARCHITECTURES})
    endif()
else()
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

# Prevent CEF from resetting CMAKE_OSX_ARCHITECTURES if it explicitly set, this
# is required in order to allow building universal binaries.
if(CMAKE_OSX_ARCHITECTURES)
    set_readonly(CMAKE_OSX_ARCHITECTURES)
endif()

add_subdirectory(${CEF_ROOT} ${CEF_BUILD_ROOT} EXCLUDE_FROM_ALL)

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
