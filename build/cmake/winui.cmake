#############################################################################
# Name:        build/cmake/winui.cmake
# Purpose:     Experimental wxWinUI / Windows App SDK integration
# Author:      wxWidgets development team
# Created:     2026-05-31
# Copyright:   (c) wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

if(NOT WXWINUI OR NOT wxUSE_WINUI3)
    return()
endif()

set(wxWINUI3_WINDOWSAPPSDK_VERSION "2.1.3" CACHE STRING
    "Windows App SDK runtime package version used by wxWinUI")
set(wxWINUI3_WINUI_VERSION "2.1.0" CACHE STRING
    "Microsoft.WindowsAppSDK.WinUI NuGet package version used by wxWinUI")
set(wxWINUI3_FOUNDATION_VERSION "2.0.21" CACHE STRING
    "Microsoft.WindowsAppSDK.Foundation NuGet package version used by wxWinUI")
set(wxWINUI3_INTERACTIVEEXPERIENCES_VERSION "2.0.13" CACHE STRING
    "Microsoft.WindowsAppSDK.InteractiveExperiences NuGet package version used by wxWinUI")
set(wxWINUI3_BASE_VERSION "2.0.4" CACHE STRING
    "Microsoft.WindowsAppSDK.Base NuGet package version used by wxWinUI")
set(wxWINUI3_WEBVIEW2_VERSION "1.0.3719.77" CACHE STRING
    "Microsoft.Web.WebView2 NuGet package version used for WinUI metadata projection")
set(wxWINUI3_PACKAGE_ROOT "${wxBINARY_DIR}/packages" CACHE PATH
    "Directory used to cache wxWinUI NuGet packages")
set(wxWINUI3_CPPWINRT_EXE "" CACHE FILEPATH
    "Path to cppwinrt.exe used by wxWinUI")
set(wxWINUI3_CPPWINRT_OUTPUT_DIR "${wxBINARY_DIR}/winui/cppwinrt" CACHE PATH
    "Directory used for generated wxWinUI C++/WinRT headers")

function(wx_winui3_latest_file out_var glob_pattern description)
    file(GLOB candidates "${glob_pattern}")
    list(SORT candidates)
    list(LENGTH candidates candidates_count)
    if(candidates_count EQUAL 0)
        message(FATAL_ERROR "wxWinUI requires ${description}, but none was found using: ${glob_pattern}")
    endif()
    math(EXPR last_index "${candidates_count} - 1")
    list(GET candidates ${last_index} selected)
    set(${out_var} "${selected}" PARENT_SCOPE)
endfunction()

function(wx_winui3_find_nuget_package out_var package version marker)
    string(REGEX REPLACE "[^A-Za-z0-9]" "_" package_var "${package}")
    string(TOUPPER "${package_var}" package_var)
    set(cache_var "wxWINUI3_${package_var}_PACKAGE_DIR")
    set(default_dir "${wxWINUI3_PACKAGE_ROOT}/${package}.${version}")

    if(DEFINED ${cache_var} AND NOT "${${cache_var}}" STREQUAL "")
        if(NOT EXISTS "${${cache_var}}/${marker}")
            unset(${cache_var} CACHE)
        endif()
    endif()

    find_path(${cache_var}
        NAMES "${marker}"
        PATHS
            "${wxSOURCE_DIR}/3rdparty/winui/${package}.${version}"
            "${default_dir}"
        NO_DEFAULT_PATH
        )

    if(NOT ${cache_var})
        message(STATUS "wxWinUI: ${package} ${version} not found, downloading from NuGet...")

        file(MAKE_DIRECTORY "${default_dir}")
        file(MAKE_DIRECTORY "${wxWINUI3_PACKAGE_ROOT}")
        set(download_file "${wxWINUI3_PACKAGE_ROOT}/${package}.${version}.nupkg")
        set(download_url "https://www.nuget.org/api/v2/package/${package}/${version}")

        file(DOWNLOAD
            "${download_url}"
            "${download_file}"
            STATUS download_status
            SHOW_PROGRESS
            )
        list(GET download_status 0 download_status_code)
        if(NOT download_status_code EQUAL 0)
            list(GET download_status 1 download_status_text)
            message(FATAL_ERROR
                "Error ${download_status_code} downloading ${package} ${version}: ${download_status_text}")
        endif()

        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E tar x "${download_file}"
            WORKING_DIRECTORY "${default_dir}"
            RESULT_VARIABLE extract_result
            )
        if(NOT extract_result EQUAL 0)
            message(FATAL_ERROR "Failed to extract ${download_file}")
        endif()

        if(NOT EXISTS "${default_dir}/${marker}")
            message(FATAL_ERROR
                "NuGet package ${package} ${version} does not contain expected file ${marker}")
        endif()

        set(${cache_var} "${default_dir}" CACHE PATH "${package} NuGet package path" FORCE)
    endif()

    set(${out_var} "${${cache_var}}" PARENT_SCOPE)
endfunction()

function(wx_winui3_glob_winmd out_var glob_pattern description)
    file(GLOB winmd_files "${glob_pattern}")
    list(SORT winmd_files)
    list(LENGTH winmd_files winmd_count)
    if(winmd_count EQUAL 0)
        message(FATAL_ERROR "wxWinUI requires ${description}, but none was found using: ${glob_pattern}")
    endif()
    set(${out_var} ${winmd_files} PARENT_SCOPE)
endfunction()

if(NOT MSVC)
    message(FATAL_ERROR "wxWinUI currently requires MSVC")
endif()

set(wxWINUI3_WINDOWS_KITS_DIR "C:/Program Files (x86)/Windows Kits/10")
if(NOT EXISTS "${wxWINUI3_WINDOWS_KITS_DIR}")
    set(wxWINUI3_WINDOWS_KITS_DIR "$ENV{ProgramFiles\(x86\)}/Windows Kits/10")
endif()
file(TO_CMAKE_PATH "${wxWINUI3_WINDOWS_KITS_DIR}" wxWINUI3_WINDOWS_KITS_DIR)

if(NOT wxWINUI3_CPPWINRT_EXE)
    wx_winui3_latest_file(wxWINUI3_CPPWINRT_EXE
        "${wxWINUI3_WINDOWS_KITS_DIR}/bin/*/x64/cppwinrt.exe"
        "cppwinrt.exe from the Windows 10 SDK")
    set(wxWINUI3_CPPWINRT_EXE "${wxWINUI3_CPPWINRT_EXE}" CACHE FILEPATH
        "Path to cppwinrt.exe used by wxWinUI" FORCE)
endif()

wx_winui3_latest_file(wxWINUI3_WINDOWS_WINMD
    "${wxWINUI3_WINDOWS_KITS_DIR}/UnionMetadata/10.*/Windows.winmd"
    "Windows.winmd from the Windows 10 SDK")
get_filename_component(wxWINUI3_WINDOWS_SDK_METADATA_DIR "${wxWINUI3_WINDOWS_WINMD}" DIRECTORY)
get_filename_component(wxWINUI3_WINDOWS_SDK_VERSION "${wxWINUI3_WINDOWS_SDK_METADATA_DIR}" NAME)
if(wxWINUI3_WINDOWS_SDK_VERSION VERSION_LESS "10.0.18362.0")
    message(FATAL_ERROR
        "wxWinUI requires Windows 10 SDK 10.0.18362.0 or newer, found ${wxWINUI3_WINDOWS_SDK_VERSION}")
endif()

set(wxWINUI3_CPPWINRT_INCLUDE_DIR
    "${wxWINUI3_WINDOWS_KITS_DIR}/Include/${wxWINUI3_WINDOWS_SDK_VERSION}/cppwinrt")
if(NOT EXISTS "${wxWINUI3_CPPWINRT_INCLUDE_DIR}/winrt/base.h")
    wx_winui3_latest_file(wxWINUI3_CPPWINRT_BASE
        "${wxWINUI3_WINDOWS_KITS_DIR}/Include/10.*/cppwinrt/winrt/base.h"
        "C++/WinRT headers from the Windows 10 SDK")
    get_filename_component(wxWINUI3_CPPWINRT_INCLUDE_DIR
        "${wxWINUI3_CPPWINRT_BASE}" DIRECTORY)
    get_filename_component(wxWINUI3_CPPWINRT_INCLUDE_DIR
        "${wxWINUI3_CPPWINRT_INCLUDE_DIR}" DIRECTORY)
endif()

wx_winui3_find_nuget_package(wxWINUI3_WINUI_PACKAGE_DIR
    "Microsoft.WindowsAppSDK.WinUI" "${wxWINUI3_WINUI_VERSION}"
    "metadata/Microsoft.UI.Xaml.winmd")
wx_winui3_find_nuget_package(wxWINUI3_FOUNDATION_PACKAGE_DIR
    "Microsoft.WindowsAppSDK.Foundation" "${wxWINUI3_FOUNDATION_VERSION}"
    "include/MddBootstrap.h")
wx_winui3_find_nuget_package(wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR
    "Microsoft.WindowsAppSDK.InteractiveExperiences" "${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}"
    "include/winrt/Microsoft.UI.Interop.h")
wx_winui3_find_nuget_package(wxWINUI3_BASE_PACKAGE_DIR
    "Microsoft.WindowsAppSDK.Base" "${wxWINUI3_BASE_VERSION}"
    "build/native/Microsoft.WindowsAppSDK.Base.props")
wx_winui3_find_nuget_package(wxWINUI3_RUNTIME_PACKAGE_DIR
    "Microsoft.WindowsAppSDK.Runtime" "${wxWINUI3_WINDOWSAPPSDK_VERSION}"
    "include/WindowsAppSDK-VersionInfo.h")
wx_winui3_find_nuget_package(wxWINUI3_WEBVIEW2_PACKAGE_DIR
    "Microsoft.Web.WebView2" "${wxWINUI3_WEBVIEW2_VERSION}"
    "lib/Microsoft.Web.WebView2.Core.winmd")

wx_winui3_glob_winmd(wxWINUI3_FOUNDATION_WINMD
    "${wxWINUI3_FOUNDATION_PACKAGE_DIR}/metadata/*.winmd"
    "Windows App SDK Foundation metadata")
wx_winui3_glob_winmd(wxWINUI3_INTERACTIVE_WINMD
    "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}/metadata/10.0.18362.0/*.winmd"
    "Windows App SDK Interactive Experiences metadata")

set(wxWINUI3_CPPWINRT_INPUTS
    "${wxWINUI3_WINUI_PACKAGE_DIR}/metadata/Microsoft.UI.Xaml.winmd"
    "${wxWINUI3_WINUI_PACKAGE_DIR}/metadata/Microsoft.UI.Text.winmd"
    ${wxWINUI3_FOUNDATION_WINMD}
    ${wxWINUI3_INTERACTIVE_WINMD}
    "${wxWINUI3_WEBVIEW2_PACKAGE_DIR}/lib/Microsoft.Web.WebView2.Core.winmd"
    )

set(wxWINUI3_CPPWINRT_STAMP "${wxWINUI3_CPPWINRT_OUTPUT_DIR}/wxwinui_cppwinrt.stamp")
file(MAKE_DIRECTORY "${wxWINUI3_CPPWINRT_OUTPUT_DIR}")
add_custom_command(
    OUTPUT "${wxWINUI3_CPPWINRT_STAMP}"
    COMMAND "${CMAKE_COMMAND}" -E remove_directory "${wxWINUI3_CPPWINRT_OUTPUT_DIR}/winrt"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${wxWINUI3_CPPWINRT_OUTPUT_DIR}"
    COMMAND "${wxWINUI3_CPPWINRT_EXE}"
        -in ${wxWINUI3_CPPWINRT_INPUTS}
        -ref "${wxWINUI3_WINDOWS_WINMD}"
        -out "${wxWINUI3_CPPWINRT_OUTPUT_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E touch "${wxWINUI3_CPPWINRT_STAMP}"
    DEPENDS ${wxWINUI3_CPPWINRT_INPUTS} "${wxWINUI3_WINDOWS_WINMD}"
    COMMENT "Generating wxWinUI C++/WinRT projection headers"
    VERBATIM
    )
add_custom_target(wxwinui_cppwinrt DEPENDS "${wxWINUI3_CPPWINRT_STAMP}")

if(wxPLATFORM_ARCH)
    set(wxWINUI3_ARCH "${wxPLATFORM_ARCH}")
else()
    set(wxWINUI3_ARCH "x86")
endif()
if(wxWINUI3_ARCH STREQUAL "win32")
    set(wxWINUI3_ARCH "x86")
endif()

set(wxWINUI3_BOOTSTRAP_LIB
    "${wxWINUI3_FOUNDATION_PACKAGE_DIR}/lib/native/${wxWINUI3_ARCH}/Microsoft.WindowsAppRuntime.Bootstrap.lib")
if(NOT EXISTS "${wxWINUI3_BOOTSTRAP_LIB}")
    message(FATAL_ERROR "wxWinUI bootstrap library not found: ${wxWINUI3_BOOTSTRAP_LIB}")
endif()
set(wxWINUI3_BOOTSTRAP_DLL
    "${wxWINUI3_FOUNDATION_PACKAGE_DIR}/runtimes/win-${wxWINUI3_ARCH}/native/Microsoft.WindowsAppRuntime.Bootstrap.dll")
if(NOT EXISTS "${wxWINUI3_BOOTSTRAP_DLL}")
    message(FATAL_ERROR "wxWinUI bootstrap DLL not found: ${wxWINUI3_BOOTSTRAP_DLL}")
endif()
set(wxWINUI3_DISPATCHING_LIB
    "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}/lib/native/${wxWINUI3_ARCH}/Microsoft.UI.Dispatching.lib")
if(NOT EXISTS "${wxWINUI3_DISPATCHING_LIB}")
    message(FATAL_ERROR "wxWinUI dispatching library not found: ${wxWINUI3_DISPATCHING_LIB}")
endif()

list(APPEND wxTOOLKIT_INCLUDE_DIRS
    "${wxWINUI3_CPPWINRT_OUTPUT_DIR}"
    "${wxWINUI3_CPPWINRT_INCLUDE_DIR}"
    "${wxWINUI3_WINUI_PACKAGE_DIR}/include"
    "${wxWINUI3_FOUNDATION_PACKAGE_DIR}/include"
    "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}/include"
    "${wxWINUI3_RUNTIME_PACKAGE_DIR}/include"
    )
list(APPEND wxTOOLKIT_LIBRARIES
    "${wxWINUI3_BOOTSTRAP_LIB}"
    "${wxWINUI3_DISPATCHING_LIB}"
    windowsapp
    runtimeobject
    dwmapi
    )
list(APPEND wxTOOLKIT_EXTRA "WinUI 3 ${wxWINUI3_WINDOWSAPPSDK_VERSION}")

function(wx_winui3_configure_target target_name)
    if(NOT TARGET ${target_name} OR NOT WXWINUI OR NOT wxUSE_WINUI3)
        return()
    endif()

    add_dependencies(${target_name} wxwinui_cppwinrt)
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${wxWINUI3_BOOTSTRAP_DLL}"
            "$<TARGET_FILE_DIR:${target_name}>/Microsoft.WindowsAppRuntime.Bootstrap.dll"
        )
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /bigobj)
    endif()
endfunction()
