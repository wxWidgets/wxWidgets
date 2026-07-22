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

# SHA-256 hashes of the pinned nupkg archives above.  Downloads and cached
# archives are both validated against them before extraction, so a corrupted
# or tampered package fails the configure instead of being silently used.
# When changing one of the versions, update the matching hash, e.g. with
# `Get-FileHash -Algorithm SHA256 <package>.<version>.nupkg`.
set(wxWINUI3_WINDOWSAPPSDK_SHA256
    "f3ed3c074b50952cc1f2975a35fbaa82196b79186c1535fb7065c8bdedf69e4b"
    CACHE STRING "SHA-256 of the pinned Microsoft.WindowsAppSDK.Runtime nupkg")
set(wxWINUI3_WINUI_SHA256
    "be85c72d5fb402167bb20710bcbc4854665fd60cdc1e530474e33f61c349ec0a"
    CACHE STRING "SHA-256 of the pinned Microsoft.WindowsAppSDK.WinUI nupkg")
set(wxWINUI3_FOUNDATION_SHA256
    "95262c02b885d228f2ccad5793dd2d2db49974ea014c060a83f52a4b0f05c5d4"
    CACHE STRING "SHA-256 of the pinned Microsoft.WindowsAppSDK.Foundation nupkg")
set(wxWINUI3_INTERACTIVEEXPERIENCES_SHA256
    "8f7864e22ac2fab67dcc858faa0789061aa4df8a7329111ba2823435fdeda273"
    CACHE STRING "SHA-256 of the pinned Microsoft.WindowsAppSDK.InteractiveExperiences nupkg")
set(wxWINUI3_BASE_SHA256
    "e3e13478c4c80c59ed5f8f89542fe49a2985daa484753e93a5858e90c2d46a4d"
    CACHE STRING "SHA-256 of the pinned Microsoft.WindowsAppSDK.Base nupkg")
set(wxWINUI3_WEBVIEW2_SHA256
    "2f6be3a10a1a8d6d1fde986af4131dab344f8181fffe75590824e0f4b037ed73"
    CACHE STRING "SHA-256 of the pinned Microsoft.Web.WebView2 nupkg")

set(wxWINUI3_PACKAGE_ROOT "${wxBINARY_DIR}/packages" CACHE PATH
    "Directory used to cache wxWinUI NuGet packages")
set(wxWINUI3_CPPWINRT_EXE "" CACHE FILEPATH
    "Path to cppwinrt.exe used by wxWinUI")
set(wxWINUI3_CPPWINRT_OUTPUT_DIR "${wxBINARY_DIR}/winui/cppwinrt" CACHE PATH
    "Directory used for generated wxWinUI C++/WinRT headers")

# Select one single Windows 10 SDK version under kits_dir and return it in
# out_version: the numerically highest version for which every needed
# artefact exists (Windows.winmd, the C++/WinRT headers and — unless
# need_cppwinrt is FALSE because the user provided their own tool —
# cppwinrt.exe).  Selecting the artefacts independently could silently mix
# several half-installed SDKs; comparing numerically matters because a
# lexical sort would order 10.0.9600.0 after 10.0.26100.0.
function(wx_winui3_select_windows_sdk out_version kits_dir need_cppwinrt)
    file(GLOB metadata_candidates "${kits_dir}/UnionMetadata/10.*")
    set(selected_version 0)
    set(rejected_versions)
    foreach(metadata_dir IN LISTS metadata_candidates)
        get_filename_component(candidate_version "${metadata_dir}" NAME)
        if(NOT candidate_version MATCHES "^[0-9]+(\\.[0-9]+)+$")
            continue()
        endif()
        set(missing)
        if(NOT EXISTS "${kits_dir}/UnionMetadata/${candidate_version}/Windows.winmd")
            list(APPEND missing "Windows.winmd")
        endif()
        if(NOT EXISTS "${kits_dir}/Include/${candidate_version}/cppwinrt/winrt/base.h")
            list(APPEND missing "cppwinrt headers")
        endif()
        if(need_cppwinrt AND NOT EXISTS "${kits_dir}/bin/${candidate_version}/x64/cppwinrt.exe")
            list(APPEND missing "cppwinrt.exe")
        endif()
        if(missing)
            string(REPLACE ";" ", " missing "${missing}")
            list(APPEND rejected_versions "${candidate_version} (missing: ${missing})")
        elseif(candidate_version VERSION_GREATER selected_version)
            set(selected_version "${candidate_version}")
        endif()
    endforeach()
    if(selected_version VERSION_EQUAL 0)
        string(REPLACE ";" "; " rejected_versions "${rejected_versions}")
        message(FATAL_ERROR
            "wxWinUI requires a complete Windows 10 SDK installation "
            "(Windows.winmd, C++/WinRT headers and cppwinrt.exe of one same "
            "version) under ${kits_dir}. Considered: ${rejected_versions}")
    endif()
    set(${out_version} "${selected_version}" PARENT_SCOPE)
endfunction()

function(wx_winui3_find_nuget_package out_var package version marker expected_sha256)
    string(REGEX REPLACE "[^A-Za-z0-9]" "_" package_var "${package}")
    string(TOUPPER "${package_var}" package_var)
    set(cache_var "wxWINUI3_${package_var}_PACKAGE_DIR")
    set(vendored_dir "${wxSOURCE_DIR}/3rdparty/winui/${package}.${version}")
    set(default_dir "${wxWINUI3_PACKAGE_ROOT}/${package}.${version}")
    # Written inside a cache extraction so its origin archive stays provable.
    set(stamp_name "wxwinui-nupkg-sha256.txt")

    string(TOLOWER "${expected_sha256}" expected_sha256)
    string(LENGTH "${expected_sha256}" expected_sha256_len)
    if(NOT expected_sha256_len EQUAL 64 OR NOT expected_sha256 MATCHES "^[0-9a-f]+$")
        message(FATAL_ERROR
            "wxWinUI: no valid pinned SHA-256 for ${package} ${version} "
            "(got \"${expected_sha256}\"). When changing the pinned "
            "package version, update the matching wxWINUI3_*_SHA256 value "
            "in build/cmake/winui.cmake.")
    endif()

    # Only the extraction cache managed by this file (directories directly
    # under wxWINUI3_PACKAGE_ROOT) may ever be invalidated or deleted below.
    # The vendored tree and any directory the user pointed the cache
    # variable at are trusted as-is on the strength of the marker file.
    file(TO_CMAKE_PATH "${wxWINUI3_PACKAGE_ROOT}" package_root_norm)
    file(TO_CMAKE_PATH "${default_dir}" default_dir_norm)

    # The unsets below clear both the cache entry and the plain variable, so
    # the invalidation also works in script mode (self-tests), where find_path
    # stores its result in a normal variable.
    if(DEFINED ${cache_var} AND NOT "${${cache_var}}" STREQUAL "")
        file(TO_CMAKE_PATH "${${cache_var}}" cached_dir_norm)
        get_filename_component(cached_parent "${cached_dir_norm}" DIRECTORY)
        get_filename_component(cached_dir_name "${cached_dir_norm}" NAME)
        if(cached_parent STREQUAL package_root_norm AND
                NOT cached_dir_name STREQUAL "${package}.${version}")
            # A managed cache entry of another version: never reuse it, the
            # pinned version is part of the directory name.
            unset(${cache_var} CACHE)
            unset(${cache_var})
        elseif(NOT EXISTS "${${cache_var}}/${marker}")
            unset(${cache_var} CACHE)
            unset(${cache_var})
        endif()
    endif()

    find_path(${cache_var}
        NAMES "${marker}"
        PATHS
            "${vendored_dir}"
            "${default_dir}"
        NO_DEFAULT_PATH
        )

    # A managed extraction must carry the stamp of the archive it came from.
    # A missing or different stamp — e.g. after changing the pinned hash, or
    # an extraction made before stamps existed — discards the cache so the
    # archive gets re-validated and re-extracted.
    if(${cache_var})
        file(TO_CMAKE_PATH "${${cache_var}}" cached_dir_norm)
        if(cached_dir_norm STREQUAL default_dir_norm)
            set(cached_stamp "")
            if(EXISTS "${${cache_var}}/${stamp_name}")
                file(READ "${${cache_var}}/${stamp_name}" cached_stamp)
                string(STRIP "${cached_stamp}" cached_stamp)
            endif()
            if(NOT cached_stamp STREQUAL expected_sha256)
                file(REMOVE_RECURSE "${default_dir}")
                unset(${cache_var} CACHE)
                unset(${cache_var})
            endif()
        endif()
    endif()

    if(NOT ${cache_var})
        file(MAKE_DIRECTORY "${wxWINUI3_PACKAGE_ROOT}")
        set(download_file "${wxWINUI3_PACKAGE_ROOT}/${package}.${version}.nupkg")

        if(EXISTS "${download_file}")
            # Validate the cached archive before ever extracting it: a
            # mismatch means corruption or tampering and must fail loudly
            # instead of being extracted or silently replaced.
            file(SHA256 "${download_file}" cached_sha256)
            if(NOT cached_sha256 STREQUAL expected_sha256)
                message(FATAL_ERROR
                    "wxWinUI: cached package ${download_file}\n"
                    "has SHA-256 ${cached_sha256}\n"
                    "instead of  ${expected_sha256}.\n"
                    "Delete the file to re-download it, or update the pinned "
                    "hash in build/cmake/winui.cmake if the pinned version "
                    "was changed on purpose.")
            endif()
        else()
            message(STATUS "wxWinUI: ${package} ${version} not found, downloading from NuGet...")

            set(download_url "https://www.nuget.org/api/v2/package/${package}/${version}")
            set(download_temp "${download_file}.download")

            file(DOWNLOAD
                "${download_url}"
                "${download_temp}"
                STATUS download_status
                SHOW_PROGRESS
                )
            list(GET download_status 0 download_status_code)
            if(NOT download_status_code EQUAL 0)
                list(GET download_status 1 download_status_text)
                file(REMOVE "${download_temp}")
                message(FATAL_ERROR
                    "Error ${download_status_code} downloading ${package} ${version}: ${download_status_text}")
            endif()

            file(SHA256 "${download_temp}" downloaded_sha256)
            if(NOT downloaded_sha256 STREQUAL expected_sha256)
                file(REMOVE "${download_temp}")
                message(FATAL_ERROR
                    "wxWinUI: downloaded package ${package} ${version}\n"
                    "has SHA-256 ${downloaded_sha256}\n"
                    "instead of  ${expected_sha256}.\n"
                    "This may indicate a compromised download; not extracting it.")
            endif()

            # Publish the validated archive atomically.
            file(RENAME "${download_temp}" "${download_file}")
        endif()

        # Extract into a temporary directory and rename it into place so an
        # interrupted configure never leaves a half-extracted tree behind as
        # a valid-looking cache.
        set(extract_dir "${default_dir}.extracting")
        file(REMOVE_RECURSE "${extract_dir}" "${default_dir}")
        file(MAKE_DIRECTORY "${extract_dir}")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E tar x "${download_file}"
            WORKING_DIRECTORY "${extract_dir}"
            RESULT_VARIABLE extract_result
            )
        if(NOT extract_result EQUAL 0)
            file(REMOVE_RECURSE "${extract_dir}")
            message(FATAL_ERROR "Failed to extract ${download_file}")
        endif()

        if(NOT EXISTS "${extract_dir}/${marker}")
            file(REMOVE_RECURSE "${extract_dir}")
            message(FATAL_ERROR
                "NuGet package ${package} ${version} does not contain expected file ${marker}")
        endif()
        file(WRITE "${extract_dir}/${stamp_name}" "${expected_sha256}\n")
        file(RENAME "${extract_dir}" "${default_dir}")

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

# Scripted self-tests (cmake -P) can load just the helper functions above
# without running the configure-time machinery below.
if(wxWINUI3_SELFTEST_FUNCTIONS_ONLY)
    return()
endif()

if(NOT MSVC)
    message(FATAL_ERROR "wxWinUI currently requires MSVC")
endif()

set(wxWINUI3_WINDOWS_KITS_DIR "C:/Program Files (x86)/Windows Kits/10")
if(NOT EXISTS "${wxWINUI3_WINDOWS_KITS_DIR}")
    set(wxWINUI3_WINDOWS_KITS_DIR "$ENV{ProgramFiles\(x86\)}/Windows Kits/10")
endif()
file(TO_CMAKE_PATH "${wxWINUI3_WINDOWS_KITS_DIR}" wxWINUI3_WINDOWS_KITS_DIR)

# One single SDK version supplies the tool, the metadata and the headers.
if(wxWINUI3_CPPWINRT_EXE)
    set(wxWINUI3_NEED_SDK_CPPWINRT FALSE)
else()
    set(wxWINUI3_NEED_SDK_CPPWINRT TRUE)
endif()
wx_winui3_select_windows_sdk(wxWINUI3_WINDOWS_SDK_VERSION
    "${wxWINUI3_WINDOWS_KITS_DIR}" ${wxWINUI3_NEED_SDK_CPPWINRT})
if(wxWINUI3_WINDOWS_SDK_VERSION VERSION_LESS "10.0.18362.0")
    message(FATAL_ERROR
        "wxWinUI requires Windows 10 SDK 10.0.18362.0 or newer, found ${wxWINUI3_WINDOWS_SDK_VERSION}")
endif()
message(STATUS "wxWinUI: using Windows 10 SDK ${wxWINUI3_WINDOWS_SDK_VERSION}")

# The tool is re-derived from the selected SDK version on every configure
# (not cached) so it can never come from a different version than the
# metadata and headers; wxWINUI3_CPPWINRT_EXE stays a pure user override.
if(wxWINUI3_CPPWINRT_EXE)
    set(wxWINUI3_CPPWINRT_TOOL "${wxWINUI3_CPPWINRT_EXE}")
else()
    set(wxWINUI3_CPPWINRT_TOOL
        "${wxWINUI3_WINDOWS_KITS_DIR}/bin/${wxWINUI3_WINDOWS_SDK_VERSION}/x64/cppwinrt.exe")
endif()
set(wxWINUI3_WINDOWS_WINMD
    "${wxWINUI3_WINDOWS_KITS_DIR}/UnionMetadata/${wxWINUI3_WINDOWS_SDK_VERSION}/Windows.winmd")
set(wxWINUI3_CPPWINRT_INCLUDE_DIR
    "${wxWINUI3_WINDOWS_KITS_DIR}/Include/${wxWINUI3_WINDOWS_SDK_VERSION}/cppwinrt")

wx_winui3_find_nuget_package(wxWINUI3_WINUI_PACKAGE_DIR
    "Microsoft.WindowsAppSDK.WinUI" "${wxWINUI3_WINUI_VERSION}"
    "metadata/Microsoft.UI.Xaml.winmd"
    "${wxWINUI3_WINUI_SHA256}")
wx_winui3_find_nuget_package(wxWINUI3_FOUNDATION_PACKAGE_DIR
    "Microsoft.WindowsAppSDK.Foundation" "${wxWINUI3_FOUNDATION_VERSION}"
    "include/MddBootstrap.h"
    "${wxWINUI3_FOUNDATION_SHA256}")
wx_winui3_find_nuget_package(wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR
    "Microsoft.WindowsAppSDK.InteractiveExperiences" "${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}"
    "include/winrt/Microsoft.UI.Interop.h"
    "${wxWINUI3_INTERACTIVEEXPERIENCES_SHA256}")
wx_winui3_find_nuget_package(wxWINUI3_BASE_PACKAGE_DIR
    "Microsoft.WindowsAppSDK.Base" "${wxWINUI3_BASE_VERSION}"
    "build/native/Microsoft.WindowsAppSDK.Base.props"
    "${wxWINUI3_BASE_SHA256}")
wx_winui3_find_nuget_package(wxWINUI3_RUNTIME_PACKAGE_DIR
    "Microsoft.WindowsAppSDK.Runtime" "${wxWINUI3_WINDOWSAPPSDK_VERSION}"
    "include/WindowsAppSDK-VersionInfo.h"
    "${wxWINUI3_WINDOWSAPPSDK_SHA256}")
wx_winui3_find_nuget_package(wxWINUI3_WEBVIEW2_PACKAGE_DIR
    "Microsoft.Web.WebView2" "${wxWINUI3_WEBVIEW2_VERSION}"
    "lib/Microsoft.Web.WebView2.Core.winmd"
    "${wxWINUI3_WEBVIEW2_SHA256}")

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
    COMMAND "${wxWINUI3_CPPWINRT_TOOL}"
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

# ---------------------------------------------------------------------------
# Unpackaged runtime payload
# ---------------------------------------------------------------------------
# The executables of this build run framework-dependent and unpackaged: the
# bootstrap DLL wires the process to the installed Windows App Runtime
# framework package, but the application's own ms-appx:/// resource
# resolution still happens against a resources.pri found next to the
# executable.  A fresh build therefore has to deploy one itself: without it
# the application starts, then fails to create XamlControlsResources
# ("Cannot locate resource from
# 'ms-appx:///Microsoft.UI.Xaml/Themes/themeresources.xaml'") and every WinUI
# control renders without its template.
#
# The payload is the bootstrap DLL plus the framework package's own resource
# index (resources.pri) and the loose asset files the theme brushes
# reference, the latter two extracted at configure time from the framework
# MSIX inside the pinned Microsoft.WindowsAppSDK.Runtime package, laid out
# exactly like in the framework package.  Its sufficiency is checked at
# runtime by the wx_winui_runtime_smoke probe (tests/winui/runtime-smoke.cpp;
# the probe does not exercise the asset files, which are included defensively
# because the theme resources reference them).
#
# LICENSING (see docs/winui/README.md): this is a DEVELOPMENT-ONLY profile.
# Only the bootstrap DLL is unambiguously redistributable ("binplaced"
# distributable code of the Foundation package); app-local extraction of
# resources.pri/assets from the framework MSIX is not part of the documented
# Windows App SDK deployment contract, and nothing from the
# Microsoft.WindowsAppSDK.WinUI Engineering Preview package may be
# redistributed at all (it contributes only build-time metadata/headers
# here, which its development/test grant covers).

string(REGEX MATCH "^[0-9]+" wxWINUI3_WINDOWSAPPSDK_MAJOR
    "${wxWINUI3_WINDOWSAPPSDK_VERSION}")
set(wxWINUI3_FRAMEWORK_MSIX
    "${wxWINUI3_RUNTIME_PACKAGE_DIR}/tools/MSIX/win10-${wxWINUI3_ARCH}/Microsoft.WindowsAppRuntime.${wxWINUI3_WINDOWSAPPSDK_MAJOR}.msix")
if(NOT EXISTS "${wxWINUI3_FRAMEWORK_MSIX}")
    message(FATAL_ERROR
        "wxWinUI: Windows App Runtime framework MSIX for win-${wxWINUI3_ARCH} "
        "not found: ${wxWINUI3_FRAMEWORK_MSIX}")
endif()

# The entries taken out of the framework MSIX, listed explicitly so the
# inventory stays deterministic.
set(wxWINUI3_MSIX_PAYLOAD_ENTRIES
    resources.pri
    Microsoft.UI.Xaml/Assets/NoiseAsset_256x256_PNG.png
    Microsoft.UI.Xaml/Assets/map.html
    )

# Extract them once per SDK version/architecture into a staging directory.
# The extraction goes to a temporary directory renamed into place, so an
# interrupted configure can never leave a truncated file behind as a
# valid-looking stage.  The stage records the pinned hash of the Runtime
# package it came from (the MSIX comes out of that hash-validated archive):
# re-pinning the hash without changing the version re-extracts instead of
# silently keeping the previous PRI.
string(TOLOWER "${wxWINUI3_WINDOWSAPPSDK_SHA256}" wxWINUI3_PAYLOAD_SOURCE_HASH)
set(wxWINUI3_PAYLOAD_STAGE_DIR
    "${wxBINARY_DIR}/winui/runtime-payload/${wxWINUI3_WINDOWSAPPSDK_VERSION}/win-${wxWINUI3_ARCH}")
set(wxWINUI3_PAYLOAD_STAGE_STAMP_NAME "wxwinui-msix-source-sha256.txt")
set(wxWINUI3_PAYLOAD_STAGE_COMPLETE TRUE)
foreach(wxWINUI3_MSIX_ENTRY IN LISTS wxWINUI3_MSIX_PAYLOAD_ENTRIES)
    if(NOT EXISTS "${wxWINUI3_PAYLOAD_STAGE_DIR}/${wxWINUI3_MSIX_ENTRY}")
        set(wxWINUI3_PAYLOAD_STAGE_COMPLETE FALSE)
    endif()
endforeach()
if(wxWINUI3_PAYLOAD_STAGE_COMPLETE)
    set(wxWINUI3_PAYLOAD_STAGE_STAMP "")
    if(EXISTS "${wxWINUI3_PAYLOAD_STAGE_DIR}/${wxWINUI3_PAYLOAD_STAGE_STAMP_NAME}")
        file(READ "${wxWINUI3_PAYLOAD_STAGE_DIR}/${wxWINUI3_PAYLOAD_STAGE_STAMP_NAME}"
            wxWINUI3_PAYLOAD_STAGE_STAMP)
        string(STRIP "${wxWINUI3_PAYLOAD_STAGE_STAMP}" wxWINUI3_PAYLOAD_STAGE_STAMP)
    endif()
    if(NOT wxWINUI3_PAYLOAD_STAGE_STAMP STREQUAL wxWINUI3_PAYLOAD_SOURCE_HASH)
        set(wxWINUI3_PAYLOAD_STAGE_COMPLETE FALSE)
    endif()
endif()
if(NOT wxWINUI3_PAYLOAD_STAGE_COMPLETE)
    set(wxWINUI3_PAYLOAD_STAGE_TMP "${wxWINUI3_PAYLOAD_STAGE_DIR}.extracting")
    file(REMOVE_RECURSE "${wxWINUI3_PAYLOAD_STAGE_TMP}" "${wxWINUI3_PAYLOAD_STAGE_DIR}")
    file(MAKE_DIRECTORY "${wxWINUI3_PAYLOAD_STAGE_TMP}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E tar x "${wxWINUI3_FRAMEWORK_MSIX}"
            -- ${wxWINUI3_MSIX_PAYLOAD_ENTRIES}
        WORKING_DIRECTORY "${wxWINUI3_PAYLOAD_STAGE_TMP}"
        RESULT_VARIABLE wxWINUI3_PRI_EXTRACT_RESULT
        )
    if(NOT wxWINUI3_PRI_EXTRACT_RESULT EQUAL 0)
        file(REMOVE_RECURSE "${wxWINUI3_PAYLOAD_STAGE_TMP}")
        message(FATAL_ERROR
            "wxWinUI: failed to extract the runtime payload from ${wxWINUI3_FRAMEWORK_MSIX}")
    endif()
    foreach(wxWINUI3_MSIX_ENTRY IN LISTS wxWINUI3_MSIX_PAYLOAD_ENTRIES)
        if(NOT EXISTS "${wxWINUI3_PAYLOAD_STAGE_TMP}/${wxWINUI3_MSIX_ENTRY}")
            file(REMOVE_RECURSE "${wxWINUI3_PAYLOAD_STAGE_TMP}")
            message(FATAL_ERROR
                "wxWinUI: entry ${wxWINUI3_MSIX_ENTRY} is missing from ${wxWINUI3_FRAMEWORK_MSIX}")
        endif()
    endforeach()
    file(WRITE "${wxWINUI3_PAYLOAD_STAGE_TMP}/${wxWINUI3_PAYLOAD_STAGE_STAMP_NAME}"
        "${wxWINUI3_PAYLOAD_SOURCE_HASH}\n")
    file(RENAME "${wxWINUI3_PAYLOAD_STAGE_TMP}" "${wxWINUI3_PAYLOAD_STAGE_DIR}")
endif()

set(wxWINUI3_RUNTIME_PAYLOAD_SRC)
set(wxWINUI3_RUNTIME_PAYLOAD_DST)

# Register one payload file; dst is relative to the executable directory.
function(wx_winui3_add_payload_file src dst)
    if(NOT EXISTS "${src}")
        message(FATAL_ERROR
            "wxWinUI runtime payload file for win-${wxWINUI3_ARCH} is missing "
            "from its package: ${src}")
    endif()
    list(APPEND wxWINUI3_RUNTIME_PAYLOAD_SRC "${src}")
    list(APPEND wxWINUI3_RUNTIME_PAYLOAD_DST "${dst}")
    set(wxWINUI3_RUNTIME_PAYLOAD_SRC "${wxWINUI3_RUNTIME_PAYLOAD_SRC}" PARENT_SCOPE)
    set(wxWINUI3_RUNTIME_PAYLOAD_DST "${wxWINUI3_RUNTIME_PAYLOAD_DST}" PARENT_SCOPE)
endfunction()

wx_winui3_add_payload_file(
    "${wxWINUI3_BOOTSTRAP_DLL}"
    "Microsoft.WindowsAppRuntime.Bootstrap.dll")

option(wxWINUI3_DEPLOY_RESOURCES_PRI
    "Deploy the Windows App Runtime resources.pri next to wxWinUI executables (turn off for applications shipping their own resources.pri)"
    ON)
if(wxWINUI3_DEPLOY_RESOURCES_PRI)
    foreach(wxWINUI3_MSIX_ENTRY IN LISTS wxWINUI3_MSIX_PAYLOAD_ENTRIES)
        wx_winui3_add_payload_file(
            "${wxWINUI3_PAYLOAD_STAGE_DIR}/${wxWINUI3_MSIX_ENTRY}"
            "${wxWINUI3_MSIX_ENTRY}")
    endforeach()
endif()

# Deterministic inventory of the deployed payload in the configure log.
message(STATUS "wxWinUI: unpackaged runtime payload (win-${wxWINUI3_ARCH}):")
list(LENGTH wxWINUI3_RUNTIME_PAYLOAD_SRC wxWINUI3_RUNTIME_PAYLOAD_COUNT)
math(EXPR wxWINUI3_RUNTIME_PAYLOAD_LAST "${wxWINUI3_RUNTIME_PAYLOAD_COUNT} - 1")
foreach(wxWINUI3_PAYLOAD_INDEX RANGE ${wxWINUI3_RUNTIME_PAYLOAD_LAST})
    list(GET wxWINUI3_RUNTIME_PAYLOAD_SRC ${wxWINUI3_PAYLOAD_INDEX} wxWINUI3_PAYLOAD_SRC)
    list(GET wxWINUI3_RUNTIME_PAYLOAD_DST ${wxWINUI3_PAYLOAD_INDEX} wxWINUI3_PAYLOAD_DST)
    message(STATUS "  ${wxWINUI3_PAYLOAD_DST} <= ${wxWINUI3_PAYLOAD_SRC}")
endforeach()

# Generate the deployment script once; every WinUI executable runs it as a
# POST_BUILD step against its own output directory.  copy_if_different keeps
# it idempotent and safe for many targets sharing one output directory.
set(wxWINUI3_DEPLOY_SCRIPT "${wxBINARY_DIR}/winui/wxWinUIDeployRuntime.cmake")
set(wxWINUI3_DEPLOY_SCRIPT_CONTENT
"# Generated by build/cmake/winui.cmake -- do not edit.
# Copies the unpackaged WinUI runtime payload into WX_WINUI_DEPLOY_DIR.
if(NOT DEFINED WX_WINUI_DEPLOY_DIR)
    message(FATAL_ERROR \"WX_WINUI_DEPLOY_DIR is required\")
endif()
# Several targets sharing one output directory deploy in parallel under
# /m; serialize per directory so the temp-file/rename dance on
# resources.pri (and every other copy) cannot race.  The lock is released
# when this cmake -P process exits.
file(MAKE_DIRECTORY \"\${WX_WINUI_DEPLOY_DIR}\")
file(LOCK \"\${WX_WINUI_DEPLOY_DIR}/.wx-winui-deploy.lock\"
    GUARD PROCESS TIMEOUT 120 RESULT_VARIABLE wx_winui_lock_result)
if(NOT wx_winui_lock_result EQUAL 0)
    message(FATAL_ERROR \"could not lock \${WX_WINUI_DEPLOY_DIR} for the WinUI runtime deployment: \${wx_winui_lock_result}\")
endif()
")

# Group the copies by destination directory: one copy_if_different call per
# directory handles all of its files.  The executable directory itself is
# represented as "." because empty strings vanish from CMake lists.
set(wxWINUI3_PAYLOAD_DST_DIRS)
foreach(wxWINUI3_PAYLOAD_DST_ITEM IN LISTS wxWINUI3_RUNTIME_PAYLOAD_DST)
    get_filename_component(wxWINUI3_PAYLOAD_DST_DIR "${wxWINUI3_PAYLOAD_DST_ITEM}" DIRECTORY)
    if(wxWINUI3_PAYLOAD_DST_DIR STREQUAL "")
        set(wxWINUI3_PAYLOAD_DST_DIR ".")
    endif()
    list(APPEND wxWINUI3_PAYLOAD_DST_DIRS "${wxWINUI3_PAYLOAD_DST_DIR}")
endforeach()
list(REMOVE_DUPLICATES wxWINUI3_PAYLOAD_DST_DIRS)
foreach(wxWINUI3_PAYLOAD_DST_DIR IN LISTS wxWINUI3_PAYLOAD_DST_DIRS)
    if(wxWINUI3_PAYLOAD_DST_DIR STREQUAL ".")
        set(wxWINUI3_PAYLOAD_DST_SUFFIX "")
    else()
        set(wxWINUI3_PAYLOAD_DST_SUFFIX "/${wxWINUI3_PAYLOAD_DST_DIR}")
    endif()
    set(wxWINUI3_PAYLOAD_GROUP_SRC)
    foreach(wxWINUI3_PAYLOAD_INDEX RANGE ${wxWINUI3_RUNTIME_PAYLOAD_LAST})
        list(GET wxWINUI3_RUNTIME_PAYLOAD_DST ${wxWINUI3_PAYLOAD_INDEX} wxWINUI3_PAYLOAD_DST)
        if(wxWINUI3_PAYLOAD_DST STREQUAL "resources.pri")
            # Deployed by the dedicated block below: it must never clobber an
            # application-provided resource index.
            continue()
        endif()
        get_filename_component(wxWINUI3_PAYLOAD_DIR "${wxWINUI3_PAYLOAD_DST}" DIRECTORY)
        if(wxWINUI3_PAYLOAD_DIR STREQUAL "")
            set(wxWINUI3_PAYLOAD_DIR ".")
        endif()
        if(wxWINUI3_PAYLOAD_DIR STREQUAL wxWINUI3_PAYLOAD_DST_DIR)
            list(GET wxWINUI3_RUNTIME_PAYLOAD_SRC ${wxWINUI3_PAYLOAD_INDEX} wxWINUI3_PAYLOAD_SRC)
            string(APPEND wxWINUI3_PAYLOAD_GROUP_SRC "\n        \"${wxWINUI3_PAYLOAD_SRC}\"")
        endif()
    endforeach()
    if(NOT wxWINUI3_PAYLOAD_GROUP_SRC)
        continue()
    endif()
    string(APPEND wxWINUI3_DEPLOY_SCRIPT_CONTENT
"file(MAKE_DIRECTORY \"\${WX_WINUI_DEPLOY_DIR}${wxWINUI3_PAYLOAD_DST_SUFFIX}\")
execute_process(
    COMMAND \"\${CMAKE_COMMAND}\" -E copy_if_different${wxWINUI3_PAYLOAD_GROUP_SRC}
        \"\${WX_WINUI_DEPLOY_DIR}${wxWINUI3_PAYLOAD_DST_SUFFIX}\"
    RESULT_VARIABLE wx_winui_copy_result
    )
if(NOT wx_winui_copy_result EQUAL 0)
    message(FATAL_ERROR \"deploying the WinUI runtime payload to \${WX_WINUI_DEPLOY_DIR}${wxWINUI3_PAYLOAD_DST_SUFFIX} failed\")
endif()
")
endforeach()

# resources.pri needs its own deployment logic: a sidecar tag remembers the
# hash of the index we deployed, so we update our own stale copies but never
# overwrite an index the application put there itself.
if(wxWINUI3_DEPLOY_RESOURCES_PRI)
    string(APPEND wxWINUI3_DEPLOY_SCRIPT_CONTENT
"set(wx_winui_pri_src \"${wxWINUI3_PAYLOAD_STAGE_DIR}/resources.pri\")
set(wx_winui_pri_dst \"\${WX_WINUI_DEPLOY_DIR}/resources.pri\")
set(wx_winui_pri_tag \"\${WX_WINUI_DEPLOY_DIR}/resources.pri.wxwinui\")
file(SHA256 \"\${wx_winui_pri_src}\" wx_winui_pri_src_hash)
set(wx_winui_pri_copy TRUE)
set(wx_winui_pri_write_tag TRUE)
if(EXISTS \"\${wx_winui_pri_dst}\")
    file(SHA256 \"\${wx_winui_pri_dst}\" wx_winui_pri_dst_hash)
    if(wx_winui_pri_dst_hash STREQUAL wx_winui_pri_src_hash)
        set(wx_winui_pri_copy FALSE)
    else()
        set(wx_winui_pri_owned FALSE)
        if(EXISTS \"\${wx_winui_pri_tag}\")
            file(READ \"\${wx_winui_pri_tag}\" wx_winui_pri_tag_hash)
            string(STRIP \"\${wx_winui_pri_tag_hash}\" wx_winui_pri_tag_hash)
            if(wx_winui_pri_tag_hash STREQUAL wx_winui_pri_dst_hash)
                set(wx_winui_pri_owned TRUE)
            endif()
        endif()
        if(NOT wx_winui_pri_owned)
            message(WARNING \"wxWinUI: \${wx_winui_pri_dst} was not deployed by this build system; keeping the application's own resources.pri. It must contain the Windows App Runtime resources or the WinUI controls will fail to load their templates (set wxWINUI3_DEPLOY_RESOURCES_PRI=OFF to silence this).\")
            set(wx_winui_pri_copy FALSE)
            set(wx_winui_pri_write_tag FALSE)
            file(REMOVE \"\${wx_winui_pri_tag}\")
        endif()
    endif()
endif()
if(wx_winui_pri_copy)
    execute_process(
        COMMAND \"\${CMAKE_COMMAND}\" -E copy \"\${wx_winui_pri_src}\" \"\${wx_winui_pri_dst}.wxtmp\"
        RESULT_VARIABLE wx_winui_copy_result
        )
    if(NOT wx_winui_copy_result EQUAL 0)
        message(FATAL_ERROR \"deploying resources.pri to \${WX_WINUI_DEPLOY_DIR} failed\")
    endif()
    file(RENAME \"\${wx_winui_pri_dst}.wxtmp\" \"\${wx_winui_pri_dst}\")
endif()
if(wx_winui_pri_write_tag)
    file(WRITE \"\${wx_winui_pri_tag}\" \"\${wx_winui_pri_src_hash}\\n\")
endif()
")
endif()
file(WRITE "${wxWINUI3_DEPLOY_SCRIPT}" "${wxWINUI3_DEPLOY_SCRIPT_CONTENT}")

# Deploy the unpackaged WinUI runtime payload next to an executable target.
# Executables only: libraries do not resolve resources, and deploying from a
# library target would hide per-executable deployment failures.  Idempotent:
# applying it twice to the same target adds a single deployment step, and
# concurrent deployments into a shared output directory converge because
# every copy is a copy_if_different.
function(wx_winui3_deploy_runtime target_name)
    if(NOT TARGET ${target_name} OR NOT WXWINUI OR NOT wxUSE_WINUI3)
        return()
    endif()

    get_target_property(target_type ${target_name} TYPE)
    if(NOT target_type STREQUAL "EXECUTABLE")
        message(FATAL_ERROR
            "wx_winui3_deploy_runtime() is only meant for executables, "
            "got ${target_type} target ${target_name}")
    endif()

    get_target_property(already_deployed ${target_name} wxWINUI3_RUNTIME_DEPLOYED)
    if(already_deployed)
        return()
    endif()
    set_target_properties(${target_name} PROPERTIES wxWINUI3_RUNTIME_DEPLOYED TRUE)

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND "${CMAKE_COMMAND}"
            -DWX_WINUI_DEPLOY_DIR=$<TARGET_FILE_DIR:${target_name}>
            -P "${wxWINUI3_DEPLOY_SCRIPT}"
        COMMENT "Deploying the WinUI runtime payload for ${target_name}"
        VERBATIM
        )
endfunction()

# Compile-time configuration shared by every target using the C++/WinRT
# projection (libraries and executables); runtime deployment is separate, see
# wx_winui3_deploy_runtime() above.
function(wx_winui3_configure_target target_name)
    if(NOT TARGET ${target_name} OR NOT WXWINUI OR NOT wxUSE_WINUI3)
        return()
    endif()

    add_dependencies(${target_name} wxwinui_cppwinrt)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /bigobj)
    endif()
endfunction()

# ---------------------------------------------------------------------------
# Runtime deployment smoke probe
# ---------------------------------------------------------------------------
# Standalone WinRT executable proving that the deployed payload is complete:
# it bootstraps the Windows App SDK, loads XamlControlsResources and realizes
# Button, TreeView and TabView in a real island (see the source for the exit
# codes).  It builds into its own directory so that it cannot accidentally
# rely on files deployed for wxcore or the samples.
if(NOT wxBUILD_TESTS STREQUAL "OFF")
    add_executable(wx_winui_runtime_smoke
        "${wxSOURCE_DIR}/tests/winui/runtime-smoke.cpp")
    add_dependencies(wx_winui_runtime_smoke wxwinui_cppwinrt)
    target_include_directories(wx_winui_runtime_smoke PRIVATE
        "${wxWINUI3_CPPWINRT_OUTPUT_DIR}"
        "${wxWINUI3_CPPWINRT_INCLUDE_DIR}"
        "${wxWINUI3_WINUI_PACKAGE_DIR}/include"
        "${wxWINUI3_FOUNDATION_PACKAGE_DIR}/include"
        "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}/include"
        "${wxWINUI3_RUNTIME_PACKAGE_DIR}/include"
        )
    target_link_libraries(wx_winui_runtime_smoke PRIVATE
        "${wxWINUI3_BOOTSTRAP_LIB}"
        "${wxWINUI3_DISPATCHING_LIB}"
        windowsapp
        runtimeobject
        )
    set_target_properties(wx_winui_runtime_smoke PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        FOLDER "Tests"
        RUNTIME_OUTPUT_DIRECTORY "${wxBINARY_DIR}/winui/runtime-smoke$<1:/>"
        )
    if(MSVC)
        target_compile_options(wx_winui_runtime_smoke PRIVATE /bigobj)
    endif()
    wx_winui3_deploy_runtime(wx_winui_runtime_smoke)

    add_test(NAME wx_winui_runtime_smoke
        COMMAND wx_winui_runtime_smoke
        WORKING_DIRECTORY "${wxBINARY_DIR}/winui/runtime-smoke")

    # Offline self-tests of the configure-time machinery above (they load
    # this file with wxWINUI3_SELFTEST_FUNCTIONS_ONLY).
    add_test(NAME wx_winui_selftest_sdk_selection
        COMMAND "${CMAKE_COMMAND}"
            -DWINUI_CMAKE=${wxSOURCE_DIR}/build/cmake/winui.cmake
            -DWORK_DIR=${wxBINARY_DIR}/winui/selftest/sdk-selection
            -P "${wxSOURCE_DIR}/tests/winui/selftest/sdk-selection.cmake")
    add_test(NAME wx_winui_selftest_nupkg_cache
        COMMAND "${CMAKE_COMMAND}"
            -DWINUI_CMAKE=${wxSOURCE_DIR}/build/cmake/winui.cmake
            -DWORK_DIR=${wxBINARY_DIR}/winui/selftest/nupkg-cache
            -P "${wxSOURCE_DIR}/tests/winui/selftest/nupkg-cache.cmake")
endif()
