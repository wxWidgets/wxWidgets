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

if(CMAKE_VERSION VERSION_LESS "3.21")
    message(FATAL_ERROR
        "wxWinUI requires CMake 3.21 or newer for its locked, transactional "
        "runtime/package deployment machinery (found ${CMAKE_VERSION}).")
endif()

if(wxUSE_UNICODE_UTF8)
    message(FATAL_ERROR
        "wxWinUI currently requires the native UTF-16 wxString build "
        "(wxUSE_UNICODE_UTF8=OFF): XAML TextBox selection/caret offsets are "
        "UTF-16 code-unit indices. UTF-8 wxString position mapping has not "
        "yet been qualified.")
endif()

set(wxWINUI3_WINDOWSAPPSDK_VERSION "1.8.260710003" CACHE STRING
    "Windows App SDK runtime package version used by wxWinUI")
set(wxWINUI3_WINUI_VERSION "1.8.260709004" CACHE STRING
    "Microsoft.WindowsAppSDK.WinUI NuGet package version used by wxWinUI")
set(wxWINUI3_FOUNDATION_VERSION "1.8.260709000" CACHE STRING
    "Microsoft.WindowsAppSDK.Foundation NuGet package version used by wxWinUI")
set(wxWINUI3_INTERACTIVEEXPERIENCES_VERSION "1.8.260708001" CACHE STRING
    "Microsoft.WindowsAppSDK.InteractiveExperiences NuGet package version used by wxWinUI")
set(wxWINUI3_BASE_VERSION "1.8.251216001" CACHE STRING
    "Microsoft.WindowsAppSDK.Base NuGet package version used by wxWinUI")
set(wxWINUI3_WEBVIEW2_VERSION "1.0.3719.77" CACHE STRING
    "Microsoft.Web.WebView2 NuGet package version used for WinUI metadata projection")

# SHA-256 hashes of the pinned nupkg archives above.  Downloads and cached
# archives are both validated against them before extraction, so a corrupted
# or tampered package fails the configure instead of being silently used.
# When changing one of the versions, update the matching hash, e.g. with
# `Get-FileHash -Algorithm SHA256 <package>.<version>.nupkg`.
set(wxWINUI3_WINDOWSAPPSDK_SHA256
    "f8f065c40fe1f11743af19eaeb7132c9c347869421bda3e8cc3f067e020f475b"
    CACHE STRING "SHA-256 of the pinned Microsoft.WindowsAppSDK.Runtime nupkg")
set(wxWINUI3_WINUI_SHA256
    "9c93fad70b93d46e19309d10d6cbd8f9a0f409a5f84395cb95d2896e0c09b7ba"
    CACHE STRING "SHA-256 of the pinned Microsoft.WindowsAppSDK.WinUI nupkg")
set(wxWINUI3_FOUNDATION_SHA256
    "ddd6ce38dbbc38afc923bc4805db543b558c921d7b9ff1b7452de1885543e71e"
    CACHE STRING "SHA-256 of the pinned Microsoft.WindowsAppSDK.Foundation nupkg")
set(wxWINUI3_INTERACTIVEEXPERIENCES_SHA256
    "496eea92d353b5d3601b67353f06dcadd6d2d9b635575acebe6e42587dbfad76"
    CACHE STRING "SHA-256 of the pinned Microsoft.WindowsAppSDK.InteractiveExperiences nupkg")
set(wxWINUI3_BASE_SHA256
    "58f0c69ad99293e7efd36b7f8c6ead0e20940a5e91866b16901b9610b08642c9"
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

option(wxWINUI3_DEPLOY_RESOURCES_PRI
    "Deploy the generated Windows App SDK application resources.pri next to wxWinUI executables (turn off for applications generating their own merged resources.pri)"
    ON)

function(wx_winui3_validate_package_token value description)
    if("${value}" STREQUAL "" OR
            NOT "${value}" MATCHES "^[A-Za-z0-9_.-]+$")
        message(FATAL_ERROR
            "wxWinUI: unsafe ${description} \"${value}\". Only ASCII letters, "
            "digits, dot, underscore and hyphen are allowed.")
    endif()
endfunction()

function(wx_winui3_validate_package_version version description)
    if(NOT "${version}" MATCHES
            "^[0-9]+\\.[0-9]+\\.[0-9]+(\\.[0-9]+)?$")
        message(FATAL_ERROR
            "wxWinUI: unsafe or unsupported ${description} \"${version}\". "
            "A three- or four-component numeric NuGet version is required.")
    endif()
endfunction()

set(wxWINUI3_NUPKG_INTEGRITY_CONTRACT "wx-nupkg-extraction-v1")
set(wxWINUI3_NUPKG_STAMP_NAME "wxwinui-nupkg-sha256.txt")
set(wxWINUI3_NUPKG_MANIFEST_NAME "wxwinui-nupkg-files.txt")

function(wx_winui3_write_nupkg_integrity package_dir archive_sha256)
    file(GLOB_RECURSE package_files
        RELATIVE "${package_dir}"
        "${package_dir}/*")
    list(REMOVE_ITEM package_files
        "${wxWINUI3_NUPKG_STAMP_NAME}"
        "${wxWINUI3_NUPKG_MANIFEST_NAME}")
    list(SORT package_files)

    set(manifest
        "${package_dir}/${wxWINUI3_NUPKG_MANIFEST_NAME}")
    file(WRITE "${manifest}" "")
    foreach(relative_path IN LISTS package_files)
        set(file_path "${package_dir}/${relative_path}")
        string(FIND "${relative_path}" "\\" backslash_index)
        if(IS_DIRECTORY "${file_path}" OR
                NOT backslash_index EQUAL -1 OR
                relative_path MATCHES "[\r\n|]")
            message(FATAL_ERROR
                "wxWinUI: NuGet package contains an unsupported manifest "
                "path: ${relative_path}")
        endif()
        file(SHA256 "${file_path}" file_hash)
        file(SIZE "${file_path}" file_size)
        file(APPEND "${manifest}"
            "${file_hash}|${file_size}|${relative_path}\n")
    endforeach()

    file(SHA256 "${manifest}" manifest_hash)
    file(WRITE "${package_dir}/${wxWINUI3_NUPKG_STAMP_NAME}"
        "contract=${wxWINUI3_NUPKG_INTEGRITY_CONTRACT}\n"
        "archive=${archive_sha256}\n"
        "manifest=${manifest_hash}\n")
endfunction()

function(wx_winui3_validate_nupkg_integrity out_var package_dir
         expected_archive_sha256)
    set(valid TRUE)
    set(stamp "${package_dir}/${wxWINUI3_NUPKG_STAMP_NAME}")
    set(manifest "${package_dir}/${wxWINUI3_NUPKG_MANIFEST_NAME}")
    if(NOT EXISTS "${stamp}" OR NOT EXISTS "${manifest}")
        set(valid FALSE)
    endif()

    if(valid)
        file(READ "${stamp}" actual_stamp)
        file(SHA256 "${manifest}" actual_manifest_hash)
        string(CONCAT expected_stamp
            "contract=${wxWINUI3_NUPKG_INTEGRITY_CONTRACT}\n"
            "archive=${expected_archive_sha256}\n"
            "manifest=${actual_manifest_hash}\n")
        if(NOT actual_stamp STREQUAL expected_stamp)
            set(valid FALSE)
        endif()
    endif()

    set(recorded_files)
    if(valid)
        file(STRINGS "${manifest}" manifest_lines)
        if(NOT manifest_lines)
            set(valid FALSE)
        endif()
        foreach(line IN LISTS manifest_lines)
            if(NOT line MATCHES "^([0-9a-f]+)\\|([0-9]+)\\|(.+)$")
                set(valid FALSE)
                break()
            endif()
            set(recorded_hash "${CMAKE_MATCH_1}")
            set(recorded_size "${CMAKE_MATCH_2}")
            set(relative_path "${CMAKE_MATCH_3}")
            string(LENGTH "${recorded_hash}" hash_length)
            string(FIND "${relative_path}" "\\" backslash_index)
            if(NOT hash_length EQUAL 64 OR
                    IS_ABSOLUTE "${relative_path}" OR
                    relative_path MATCHES "(^|/)\\.\\.(/|$)" OR
                    NOT backslash_index EQUAL -1 OR
                    relative_path MATCHES "[\r\n|]")
                set(valid FALSE)
                break()
            endif()

            set(file_path "${package_dir}/${relative_path}")
            if(NOT EXISTS "${file_path}" OR IS_DIRECTORY "${file_path}")
                set(valid FALSE)
                break()
            endif()
            file(SIZE "${file_path}" actual_size)
            file(SHA256 "${file_path}" actual_hash)
            if(NOT actual_size EQUAL recorded_size OR
                    NOT actual_hash STREQUAL recorded_hash)
                set(valid FALSE)
                break()
            endif()
            list(APPEND recorded_files "${relative_path}")
        endforeach()
    endif()

    if(valid)
        file(GLOB_RECURSE actual_files
            RELATIVE "${package_dir}"
            "${package_dir}/*")
        list(REMOVE_ITEM actual_files
            "${wxWINUI3_NUPKG_STAMP_NAME}"
            "${wxWINUI3_NUPKG_MANIFEST_NAME}")
        list(SORT actual_files)
        list(SORT recorded_files)
        if(NOT actual_files STREQUAL recorded_files)
            set(valid FALSE)
        endif()
    endif()

    set(${out_var} "${valid}" PARENT_SCOPE)
endfunction()

# Select one single Windows 10 SDK version under kits_dir and return it in
# out_version: the numerically highest version for which every needed
# artefact exists (Windows.winmd, the C++/WinRT headers, MakePri and — unless
# need_cppwinrt is FALSE because the user provided their own tool —
# cppwinrt.exe).  Selecting the artefacts independently could silently mix
# several half-installed SDKs; comparing numerically matters because a
# lexical sort would order 10.0.9600.0 after 10.0.26100.0.
function(wx_winui3_select_windows_sdk out_version kits_dir need_cppwinrt
         need_makepri)
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
        if(need_makepri AND
                NOT EXISTS
                    "${kits_dir}/bin/${candidate_version}/x64/makepri.exe")
            list(APPEND missing "makepri.exe")
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
            "(Windows.winmd, C++/WinRT headers, makepri.exe and cppwinrt.exe "
            "of one same "
            "version) under ${kits_dir}. Considered: ${rejected_versions}")
    endif()
    set(${out_version} "${selected_version}" PARENT_SCOPE)
endfunction()

function(wx_winui3_find_nuget_package out_var package version marker expected_sha256)
    wx_winui3_validate_package_token("${package}" "NuGet package name")
    wx_winui3_validate_package_version("${version}"
        "NuGet package version for ${package}")
    if(IS_ABSOLUTE "${marker}" OR
            "${marker}" MATCHES "(^|[/\\\\])\\.\\.([/\\\\]|$)")
        message(FATAL_ERROR
            "wxWinUI: unsafe marker path \"${marker}\" for ${package}")
    endif()

    string(REGEX REPLACE "[^A-Za-z0-9]" "_" package_var "${package}")
    string(TOUPPER "${package_var}" package_var)
    set(cache_var "wxWINUI3_${package_var}_PACKAGE_DIR")
    set(auto_cache_var "${cache_var}_WX_AUTO")

    string(TOLOWER "${expected_sha256}" expected_sha256)
    string(LENGTH "${expected_sha256}" expected_sha256_len)
    if(NOT expected_sha256_len EQUAL 64 OR
            NOT expected_sha256 MATCHES "^[0-9a-f]+$")
        message(FATAL_ERROR
            "wxWinUI: no valid pinned SHA-256 for ${package} ${version} "
            "(got \"${expected_sha256}\"). When changing the pinned "
            "package version, update the matching wxWINUI3_*_SHA256 value "
            "in build/cmake/winui.cmake.")
    endif()

    get_filename_component(package_root_abs
        "${wxWINUI3_PACKAGE_ROOT}" ABSOLUTE BASE_DIR "${CMAKE_BINARY_DIR}")
    file(TO_CMAKE_PATH "${package_root_abs}" package_root_norm)
    # Keep different same-version package bytes side by side. A 128-bit hash
    # prefix keeps Windows paths practical; the full hash is still validated
    # and stored in the extraction stamp.
    string(SUBSTRING "${expected_sha256}" 0 32 package_hash_key)
    set(managed_leaf "${package}.${version}-${package_hash_key}")
    set(vendored_dir "${wxSOURCE_DIR}/3rdparty/winui/${package}.${version}")
    set(default_dir "${package_root_norm}/${managed_leaf}")
    get_filename_component(default_parent "${default_dir}" DIRECTORY)
    file(TO_CMAKE_PATH "${default_parent}" default_parent)
    if(NOT default_parent STREQUAL package_root_norm)
        message(FATAL_ERROR
            "wxWinUI internal error: managed package path escaped its root: "
            "${default_dir}")
    endif()
    # Written inside a cache extraction so its origin archive stays provable.
    set(stamp_name "${wxWINUI3_NUPKG_STAMP_NAME}")

    # Only the extraction cache managed by this file (directories directly
    # under wxWINUI3_PACKAGE_ROOT) may ever be invalidated or deleted below.
    # The vendored tree and any directory the user pointed the cache
    # variable at are trusted as-is on the strength of the marker file.
    file(TO_CMAKE_PATH "${default_dir}" default_dir_norm)

    # Multiple build trees may intentionally share the package cache. Serialize
    # validation/download/extraction for this exact package pin so fixed
    # .download/.extracting names can never race.
    file(MAKE_DIRECTORY "${package_root_norm}")
    string(SHA256 package_lock_key
        "${package}|${version}|${expected_sha256}")
    file(LOCK "${package_root_norm}/.wxwinui-${package_lock_key}.lock"
        GUARD FUNCTION TIMEOUT 120 RESULT_VARIABLE package_lock_result)
    if(NOT package_lock_result EQUAL 0)
        message(FATAL_ERROR
            "wxWinUI: could not lock the NuGet cache for ${package} "
            "${version}: ${package_lock_result}")
    endif()

    # Track whether this value was selected by us or supplied by the user.
    # Otherwise changing wxWINUI3_PACKAGE_ROOT in an existing build would
    # silently retain the automatic extraction under the old root and mistake
    # it for a trusted user override. The stamp check migrates older caches
    # created before the internal provenance entry existed.
    set(cache_value_is_auto FALSE)
    set(cache_value_is_user FALSE)
    if(DEFINED ${cache_var} AND NOT "${${cache_var}}" STREQUAL "")
        get_filename_component(cached_dir_abs "${${cache_var}}" ABSOLUTE
            BASE_DIR "${CMAKE_BINARY_DIR}")
        file(TO_CMAKE_PATH "${cached_dir_abs}" cached_dir_norm)
        get_filename_component(cached_parent "${cached_dir_norm}" DIRECTORY)
        get_filename_component(cached_dir_name "${cached_dir_norm}" NAME)

        if(DEFINED ${auto_cache_var} AND
                "${${cache_var}}" STREQUAL "${${auto_cache_var}}")
            set(cache_value_is_auto TRUE)
        elseif(EXISTS "${cached_dir_norm}/${stamp_name}")
            set(cache_value_is_auto TRUE)
        elseif(cached_parent STREQUAL package_root_norm)
            # Migration path for pre-stamp managed extractions.
            set(cache_value_is_auto TRUE)
        else()
            set(cache_value_is_user TRUE)
        endif()

        if(cache_value_is_auto AND
                NOT cached_dir_norm STREQUAL default_dir_norm AND
                NOT cached_dir_norm STREQUAL vendored_dir)
            unset(${cache_var} CACHE)
            unset(${cache_var})
            set(cache_value_is_auto FALSE)
        elseif(NOT EXISTS "${${cache_var}}/${marker}")
            unset(${cache_var} CACHE)
            unset(${cache_var})
            set(cache_value_is_auto FALSE)
            set(cache_value_is_user FALSE)
        endif()
    endif()

    # The unsets above clear both the cache entry and the plain variable, so
    # invalidation also works in script mode, where find_path may store a
    # normal variable.
    find_path(${cache_var}
        NAMES "${marker}"
        PATHS
            "${vendored_dir}"
            "${default_dir}"
        NO_DEFAULT_PATH
        )

    # A managed extraction must carry an archive-bound manifest of every file.
    # This catches post-extraction corruption (not just a changed stamp) before
    # any header, binary, PRI, asset, licence or nuspec is consumed.
    if(${cache_var})
        file(TO_CMAKE_PATH "${${cache_var}}" cached_dir_norm)
        if(cached_dir_norm STREQUAL default_dir_norm)
            wx_winui3_validate_nupkg_integrity(
                cached_extraction_valid
                "${default_dir}"
                "${expected_sha256}")
            if(NOT cached_extraction_valid)
                message(STATUS
                    "wxWinUI: ${package} ${version} extraction integrity "
                    "failed; rebuilding it from the pinned archive")
                file(REMOVE_RECURSE "${default_dir}")
                unset(${cache_var} CACHE)
                unset(${cache_var})
                set(cache_value_is_auto FALSE)
            endif()
        endif()
    endif()

    if(NOT ${cache_var})
        set(download_file "${package_root_norm}/${managed_leaf}.nupkg")

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
        wx_winui3_write_nupkg_integrity(
            "${extract_dir}" "${expected_sha256}")
        file(RENAME "${extract_dir}" "${default_dir}")

        set(${cache_var} "${default_dir}" CACHE PATH "${package} NuGet package path" FORCE)
        set(${auto_cache_var} "${default_dir}" CACHE INTERNAL
            "Last package path selected automatically by wxWinUI" FORCE)
    elseif(NOT cache_value_is_user)
        # find_path selected either the vendored package or the managed
        # content-addressed extraction.
        set(${auto_cache_var} "${${cache_var}}" CACHE INTERNAL
            "Last package path selected automatically by wxWinUI" FORCE)
    endif()

    set(${out_var} "${${cache_var}}" PARENT_SCOPE)
endfunction()

function(wx_winui3_validate_redistributable_package package_dir package_name)
    set(license_file "${package_dir}/license.txt")
    if(NOT EXISTS "${license_file}")
        message(FATAL_ERROR
            "wxWinUI: ${package_name} has no license.txt; refusing to use "
            "an unauditable Windows App SDK package.")
    endif()

    file(READ "${license_file}" license_text)
    string(TOUPPER "${license_text}" license_text_upper)
    if(license_text_upper MATCHES "ENGINEERING[ \r\n\t]+PREVIEW" OR
            license_text_upper MATCHES
                "YOU MAY NOT USE THE SOFTWARE IN A LIVE OPERATING ENVIRONMENT")
        message(FATAL_ERROR
            "wxWinUI: ${package_name} is governed by the Windows App SDK "
            "Engineering Preview licence. It cannot be used in a live "
            "environment or redistributed. Select a stable package release.")
    endif()

    string(FIND "${license_text}"
        "Any files that are binplaced with your application by the WindowsAppSDK NuGet package are, by definition, permitted to be redistributed."
        distribution_grant_pos)
    if(distribution_grant_pos EQUAL -1)
        message(FATAL_ERROR
            "wxWinUI: ${package_name} license.txt does not contain the "
            "expected Windows App SDK NuGet binplacement redistribution "
            "grant. Refusing to assemble a distributable runtime payload.")
    endif()

    message(STATUS
        "wxWinUI: ${package_name} uses the stable Windows App SDK licence "
        "with the NuGet binplacement redistribution grant")
endfunction()

function(wx_winui3_validate_webview2_redistribution package_dir package_name)
    set(license_file "${package_dir}/LICENSE.txt")
    set(notice_file "${package_dir}/NOTICE.txt")
    foreach(required_file IN ITEMS "${license_file}" "${notice_file}")
        if(NOT EXISTS "${required_file}")
            message(FATAL_ERROR
                "wxWinUI: ${package_name} is missing ${required_file}; "
                "refusing to assemble an incomplete redistribution payload.")
        endif()
    endforeach()

    # This is a deterministic package-contract check, not a legal opinion.
    # Binary redistribution under the package's BSD licence requires the
    # copyright, conditions and disclaimer to travel in documentation or
    # other materials, so deploy the unmodified licence and third-party notice
    # beside every executable that receives the native WebView2 component.
    file(READ "${license_file}" license_text)
    string(FIND "${license_text}"
        "Redistribution and use in source and binary forms"
        redistribution_marker)
    string(FIND "${license_text}"
        "Redistributions in binary form must reproduce"
        binary_notice_marker)
    if(redistribution_marker EQUAL -1 OR binary_notice_marker EQUAL -1)
        message(FATAL_ERROR
            "wxWinUI: ${package_name} has an unrecognized redistribution "
            "licence; audit it before changing the runtime payload.")
    endif()

    message(STATUS
        "wxWinUI: ${package_name} licence and third-party notice will be "
        "deployed with the native runtime component")
endfunction()

function(wx_winui3_read_nuspec out_var package_dir package_name
         expected_version)
    file(GLOB nuspec_files "${package_dir}/*.nuspec")
    list(LENGTH nuspec_files nuspec_count)
    if(NOT nuspec_count EQUAL 1)
        message(FATAL_ERROR
            "wxWinUI: expected exactly one .nuspec for ${package_name} "
            "${expected_version} under ${package_dir}, found ${nuspec_count}.")
    endif()

    list(GET nuspec_files 0 nuspec_file)
    file(READ "${nuspec_file}" nuspec_text)
    string(REGEX MATCH
        "<id>[ \r\n\t]*([^< \r\n\t]+)[ \r\n\t]*</id>"
        nuspec_id_match "${nuspec_text}")
    if(NOT nuspec_id_match)
        message(FATAL_ERROR
            "wxWinUI: ${nuspec_file} has no readable package id.")
    endif()
    set(nuspec_id "${CMAKE_MATCH_1}")
    if(NOT nuspec_id STREQUAL package_name)
        message(FATAL_ERROR
            "wxWinUI: package identity mismatch: expected ${package_name}, "
            "nuspec declares ${nuspec_id}.")
    endif()

    string(REGEX MATCH
        "<version>[ \r\n\t]*([^< \r\n\t]+)[ \r\n\t]*</version>"
        nuspec_version_match "${nuspec_text}")
    if(NOT nuspec_version_match)
        message(FATAL_ERROR
            "wxWinUI: ${nuspec_file} has no readable package version.")
    endif()
    set(nuspec_version "${CMAKE_MATCH_1}")
    if(NOT nuspec_version STREQUAL expected_version)
        message(FATAL_ERROR
            "wxWinUI: package identity mismatch for ${package_name}: "
            "configured ${expected_version}, nuspec declares "
            "${nuspec_version}.")
    endif()

    set(${out_var} "${nuspec_text}" PARENT_SCOPE)
endfunction()

function(wx_winui3_require_nuspec_dependency nuspec_text owner dependency
         resolved_version mode)
    string(REGEX MATCH
        "<dependency[^>]*id=\"${dependency}\"[^>]*version=\"([^\"]+)\""
        dependency_match "${nuspec_text}")
    if(NOT dependency_match)
        message(FATAL_ERROR
            "wxWinUI: ${owner} does not declare its required ${dependency} "
            "dependency; refusing an unverified mixed package set.")
    endif()
    set(required_version "${CMAKE_MATCH_1}")

    if(mode STREQUAL "EXACT_COMPONENT")
        # Windows App SDK component packages are generated and serviced as one
        # cohort. Even when a bare NuGet dependency is technically a minimum,
        # accepting a hand-mixed cohort is not a supported runtime baseline.
        if(NOT required_version STREQUAL resolved_version)
            message(FATAL_ERROR
                "wxWinUI: incompatible Windows App SDK package set: ${owner} "
                "requires ${dependency} ${required_version}, but "
                "${resolved_version} is configured.")
        endif()
    elseif(mode STREQUAL "MINIMUM")
        if(NOT required_version MATCHES
                "^[0-9]+\\.[0-9]+\\.[0-9]+(\\.[0-9]+)?$")
            message(FATAL_ERROR
                "wxWinUI: unsupported NuGet version range "
                "\"${required_version}\" for ${owner} -> ${dependency}; "
                "update the compatibility validator before repinning.")
        endif()
        if(resolved_version VERSION_LESS required_version)
            message(FATAL_ERROR
                "wxWinUI: ${owner} requires ${dependency} >= "
                "${required_version}, but ${resolved_version} is configured.")
        endif()
    else()
        message(FATAL_ERROR
            "wxWinUI internal error: unknown dependency validation mode "
            "${mode}")
    endif()
endfunction()

function(wx_winui3_validate_package_set)
    wx_winui3_read_nuspec(runtime_nuspec
        "${wxWINUI3_RUNTIME_PACKAGE_DIR}"
        "Microsoft.WindowsAppSDK.Runtime"
        "${wxWINUI3_WINDOWSAPPSDK_VERSION}")
    wx_winui3_read_nuspec(winui_nuspec
        "${wxWINUI3_WINUI_PACKAGE_DIR}"
        "Microsoft.WindowsAppSDK.WinUI"
        "${wxWINUI3_WINUI_VERSION}")
    wx_winui3_read_nuspec(foundation_nuspec
        "${wxWINUI3_FOUNDATION_PACKAGE_DIR}"
        "Microsoft.WindowsAppSDK.Foundation"
        "${wxWINUI3_FOUNDATION_VERSION}")
    wx_winui3_read_nuspec(interactive_nuspec
        "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}"
        "Microsoft.WindowsAppSDK.InteractiveExperiences"
        "${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}")
    wx_winui3_read_nuspec(base_nuspec
        "${wxWINUI3_BASE_PACKAGE_DIR}"
        "Microsoft.WindowsAppSDK.Base"
        "${wxWINUI3_BASE_VERSION}")
    wx_winui3_read_nuspec(webview_nuspec
        "${wxWINUI3_WEBVIEW2_PACKAGE_DIR}"
        "Microsoft.Web.WebView2"
        "${wxWINUI3_WEBVIEW2_VERSION}")

    wx_winui3_require_nuspec_dependency("${runtime_nuspec}"
        "Microsoft.WindowsAppSDK.Runtime"
        "Microsoft.WindowsAppSDK.Base"
        "${wxWINUI3_BASE_VERSION}" EXACT_COMPONENT)
    wx_winui3_require_nuspec_dependency("${winui_nuspec}"
        "Microsoft.WindowsAppSDK.WinUI"
        "Microsoft.WindowsAppSDK.Base"
        "${wxWINUI3_BASE_VERSION}" EXACT_COMPONENT)
    wx_winui3_require_nuspec_dependency("${winui_nuspec}"
        "Microsoft.WindowsAppSDK.WinUI"
        "Microsoft.WindowsAppSDK.Foundation"
        "${wxWINUI3_FOUNDATION_VERSION}" EXACT_COMPONENT)
    wx_winui3_require_nuspec_dependency("${winui_nuspec}"
        "Microsoft.WindowsAppSDK.WinUI"
        "Microsoft.WindowsAppSDK.InteractiveExperiences"
        "${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}" EXACT_COMPONENT)
    wx_winui3_require_nuspec_dependency("${foundation_nuspec}"
        "Microsoft.WindowsAppSDK.Foundation"
        "Microsoft.WindowsAppSDK.Base"
        "${wxWINUI3_BASE_VERSION}" EXACT_COMPONENT)
    wx_winui3_require_nuspec_dependency("${foundation_nuspec}"
        "Microsoft.WindowsAppSDK.Foundation"
        "Microsoft.WindowsAppSDK.InteractiveExperiences"
        "${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}" EXACT_COMPONENT)
    wx_winui3_require_nuspec_dependency("${interactive_nuspec}"
        "Microsoft.WindowsAppSDK.InteractiveExperiences"
        "Microsoft.WindowsAppSDK.Base"
        "${wxWINUI3_BASE_VERSION}" EXACT_COMPONENT)
    wx_winui3_require_nuspec_dependency("${winui_nuspec}"
        "Microsoft.WindowsAppSDK.WinUI"
        "Microsoft.Web.WebView2"
        "${wxWINUI3_WEBVIEW2_VERSION}" MINIMUM)

    # The Runtime package is the authority for the exact component cohort.
    # Its MSBuild target contains versions generated together with that
    # runtime; validating only transitive nuspec dependencies would still
    # allow two independently coherent, but mutually incompatible cohorts.
    set(component_reference_target
        "${wxWINUI3_RUNTIME_PACKAGE_DIR}/build/Microsoft.WindowsAppSDK.ComponentReference.targets")
    if(NOT EXISTS "${component_reference_target}")
        message(FATAL_ERROR
            "wxWinUI: Runtime package has no component compatibility target: "
            "${component_reference_target}")
    endif()
    file(READ "${component_reference_target}" component_reference_text)
    string(FIND "${component_reference_text}"
        "This is populated during the generation"
        generated_versions_marker)
    if(generated_versions_marker EQUAL -1)
        message(FATAL_ERROR
            "wxWinUI: Runtime compatibility target has an unknown format: "
            "${component_reference_target}")
    endif()
    string(SUBSTRING "${component_reference_text}"
        ${generated_versions_marker} -1 generated_component_versions)
    foreach(component IN ITEMS
            "Microsoft.WindowsAppSDK.Foundation"
            "Microsoft.WindowsAppSDK.InteractiveExperiences"
            "Microsoft.WindowsAppSDK.WinUI")
        if(component STREQUAL "Microsoft.WindowsAppSDK.Foundation")
            set(resolved_component_version "${wxWINUI3_FOUNDATION_VERSION}")
        elseif(component STREQUAL
                "Microsoft.WindowsAppSDK.InteractiveExperiences")
            set(resolved_component_version
                "${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}")
        else()
            set(resolved_component_version "${wxWINUI3_WINUI_VERSION}")
        endif()

        string(FIND "${generated_component_versions}"
            "<_VersionMismatch Include=\"${component}\""
            component_entry_pos)
        if(component_entry_pos EQUAL -1)
            message(FATAL_ERROR
                "wxWinUI: Runtime compatibility target does not expose an "
                "expected version for ${component}.")
        endif()
        string(SUBSTRING "${generated_component_versions}"
            ${component_entry_pos} -1 component_entry_tail)
        string(FIND "${component_entry_tail}" "</_VersionMismatch>"
            component_entry_end)
        if(component_entry_end EQUAL -1)
            message(FATAL_ERROR
                "wxWinUI: Runtime compatibility entry for ${component} is "
                "not terminated.")
        endif()
        string(SUBSTRING "${component_entry_tail}"
            0 ${component_entry_end} component_entry)
        string(REGEX MATCH "<ExpectedVersion>([^<]+)</ExpectedVersion>"
            component_version_match "${component_entry}")
        if(NOT component_version_match)
            message(FATAL_ERROR
                "wxWinUI: Runtime compatibility entry for ${component} has "
                "no readable ExpectedVersion.")
        endif()
        set(runtime_expected_version "${CMAKE_MATCH_1}")
        if(NOT resolved_component_version STREQUAL runtime_expected_version)
            message(FATAL_ERROR
                "wxWinUI: Microsoft.WindowsAppSDK.Runtime "
                "${wxWINUI3_WINDOWSAPPSDK_VERSION} requires ${component} "
                "${runtime_expected_version}, but "
                "${resolved_component_version} is configured.")
        endif()
    endforeach()

    message(STATUS
        "wxWinUI: validated the cross-package Windows App SDK dependency set")
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

function(wx_winui3_validate_selftest_work_dir out_var work_dir safe_root
         expected_prefix)
    if("${work_dir}" STREQUAL "" OR "${safe_root}" STREQUAL "")
        message(FATAL_ERROR
            "wxWinUI self-test requires explicit WORK_DIR and SAFE_ROOT")
    endif()
    get_filename_component(work_abs "${work_dir}" ABSOLUTE)
    get_filename_component(safe_abs "${safe_root}" ABSOLUTE)
    file(TO_CMAKE_PATH "${work_abs}" work_abs)
    file(TO_CMAKE_PATH "${safe_abs}" safe_abs)
    file(RELATIVE_PATH work_relative "${safe_abs}" "${work_abs}")
    if(IS_ABSOLUTE "${work_relative}" OR
            work_relative MATCHES "^\\.\\.(/|$)" OR
            work_relative STREQUAL "." OR
            NOT (work_relative STREQUAL expected_prefix OR
                 work_relative MATCHES "^${expected_prefix}/"))
        message(FATAL_ERROR
            "wxWinUI self-test refuses unsafe WORK_DIR \"${work_abs}\". "
            "It must be ${expected_prefix} (or a child) below "
            "SAFE_ROOT \"${safe_abs}\".")
    endif()
    set(${out_var} "${work_abs}" PARENT_SCOPE)
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
    "${wxWINUI3_WINDOWS_KITS_DIR}" ${wxWINUI3_NEED_SDK_CPPWINRT}
    ${wxWINUI3_DEPLOY_RESOURCES_PRI})
if(wxWINUI3_WINDOWS_SDK_VERSION VERSION_LESS "10.0.19041.0")
    message(FATAL_ERROR
        "wxWinUI requires Windows 10 SDK 10.0.19041.0 or newer, found ${wxWINUI3_WINDOWS_SDK_VERSION}")
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
if(wxWINUI3_DEPLOY_RESOURCES_PRI)
    set(wxWINUI3_MAKEPRI_TOOL
        "${wxWINUI3_WINDOWS_KITS_DIR}/bin/${wxWINUI3_WINDOWS_SDK_VERSION}/x64/makepri.exe")
endif()

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

wx_winui3_validate_package_set()

wx_winui3_validate_redistributable_package(
    "${wxWINUI3_RUNTIME_PACKAGE_DIR}"
    "Microsoft.WindowsAppSDK.Runtime ${wxWINUI3_WINDOWSAPPSDK_VERSION}")
wx_winui3_validate_redistributable_package(
    "${wxWINUI3_WINUI_PACKAGE_DIR}"
    "Microsoft.WindowsAppSDK.WinUI ${wxWINUI3_WINUI_VERSION}")
wx_winui3_validate_redistributable_package(
    "${wxWINUI3_FOUNDATION_PACKAGE_DIR}"
    "Microsoft.WindowsAppSDK.Foundation ${wxWINUI3_FOUNDATION_VERSION}")
wx_winui3_validate_redistributable_package(
    "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}"
    "Microsoft.WindowsAppSDK.InteractiveExperiences ${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}")
wx_winui3_validate_redistributable_package(
    "${wxWINUI3_BASE_PACKAGE_DIR}"
    "Microsoft.WindowsAppSDK.Base ${wxWINUI3_BASE_VERSION}")
wx_winui3_validate_webview2_redistribution(
    "${wxWINUI3_WEBVIEW2_PACKAGE_DIR}"
    "Microsoft.Web.WebView2 ${wxWINUI3_WEBVIEW2_VERSION}")

wx_winui3_glob_winmd(wxWINUI3_FOUNDATION_WINMD
    "${wxWINUI3_FOUNDATION_PACKAGE_DIR}/metadata/*.winmd"
    "Windows App SDK Foundation metadata")
wx_winui3_glob_winmd(wxWINUI3_INTERACTIVE_WINMD
    "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}/metadata/10.0.18362.0/*.winmd"
    "Windows App SDK Interactive Experiences metadata")

set(wxWINUI3_RUNTIME_METADATA_FILES
    "${wxWINUI3_WINUI_PACKAGE_DIR}/metadata/Microsoft.UI.Xaml.winmd"
    "${wxWINUI3_WINUI_PACKAGE_DIR}/metadata/Microsoft.UI.Text.winmd"
    ${wxWINUI3_FOUNDATION_WINMD}
    ${wxWINUI3_INTERACTIVE_WINMD}
    )
set(wxWINUI3_CPPWINRT_INPUTS
    ${wxWINUI3_RUNTIME_METADATA_FILES}
    "${wxWINUI3_WEBVIEW2_PACKAGE_DIR}/lib/Microsoft.Web.WebView2.Core.winmd"
    )

if(NOT EXISTS "${wxWINUI3_CPPWINRT_TOOL}")
    message(FATAL_ERROR
        "wxWinUI C++/WinRT generator not found: ${wxWINUI3_CPPWINRT_TOOL}")
endif()
include("${wxSOURCE_DIR}/build/cmake/winui-cppwinrt.cmake")
wx_winui3_compute_cppwinrt_fingerprint(wxWINUI3_CPPWINRT_FINGERPRINT
    INPUTS ${wxWINUI3_CPPWINRT_INPUTS}
    WINDOWS_WINMD "${wxWINUI3_WINDOWS_WINMD}"
    TOOL "${wxWINUI3_CPPWINRT_TOOL}")

# The generated directory itself is content-addressed. A repin A -> B -> A
# therefore never lets a stale stamp for A bless headers generated from B.
# The always-run worker verifies a manifest covering every generated file and
# regenerates transactionally if any header is missing, injected or corrupt.
set(wxWINUI3_CPPWINRT_PROJECTION_DIR
    "${wxWINUI3_CPPWINRT_OUTPUT_DIR}/${wxWINUI3_CPPWINRT_FINGERPRINT}")
set(wxWINUI3_CPPWINRT_REQUIRED_RELATIVE "winrt/Microsoft.UI.Xaml.h")
set(wxWINUI3_CPPWINRT_REQUIRED_HEADER
    "${wxWINUI3_CPPWINRT_PROJECTION_DIR}/${wxWINUI3_CPPWINRT_REQUIRED_RELATIVE}")
set(wxWINUI3_CPPWINRT_INPUT_LIST
    "${wxBINARY_DIR}/winui/cppwinrt-driver/inputs-${wxWINUI3_CPPWINRT_FINGERPRINT}.txt")
file(MAKE_DIRECTORY "${wxWINUI3_CPPWINRT_OUTPUT_DIR}")
file(MAKE_DIRECTORY "${wxBINARY_DIR}/winui/cppwinrt-driver")
file(WRITE "${wxWINUI3_CPPWINRT_INPUT_LIST}" "")
foreach(wxWINUI3_CPPWINRT_INPUT IN LISTS wxWINUI3_CPPWINRT_INPUTS)
    if(wxWINUI3_CPPWINRT_INPUT MATCHES "[\r\n;]")
        message(FATAL_ERROR
            "wxWinUI C++/WinRT metadata path contains an unsupported "
            "separator: ${wxWINUI3_CPPWINRT_INPUT}")
    endif()
    file(APPEND "${wxWINUI3_CPPWINRT_INPUT_LIST}"
        "${wxWINUI3_CPPWINRT_INPUT}\n")
endforeach()
add_custom_target(wxwinui_cppwinrt
    COMMAND "${CMAKE_COMMAND}"
        "-DWX_CPPWINRT_OUTPUT_ROOT=${wxWINUI3_CPPWINRT_OUTPUT_DIR}"
        "-DWX_CPPWINRT_FINGERPRINT=${wxWINUI3_CPPWINRT_FINGERPRINT}"
        "-DWX_CPPWINRT_INPUT_LIST=${wxWINUI3_CPPWINRT_INPUT_LIST}"
        "-DWX_CPPWINRT_WINDOWS_WINMD=${wxWINUI3_WINDOWS_WINMD}"
        "-DWX_CPPWINRT_TOOL=${wxWINUI3_CPPWINRT_TOOL}"
        "-DWX_CPPWINRT_REQUIRED_RELATIVE=${wxWINUI3_CPPWINRT_REQUIRED_RELATIVE}"
        -P "${wxSOURCE_DIR}/build/cmake/winui-cppwinrt.cmake"
    BYPRODUCTS
        "${wxWINUI3_CPPWINRT_REQUIRED_HEADER}"
        "${wxWINUI3_CPPWINRT_PROJECTION_DIR}/wxwinui-projection-manifest.txt"
    DEPENDS
        ${wxWINUI3_CPPWINRT_INPUTS}
        "${wxWINUI3_WINDOWS_WINMD}"
        "${wxWINUI3_CPPWINRT_TOOL}"
        "${wxSOURCE_DIR}/build/cmake/winui-cppwinrt.cmake"
    COMMENT "Verifying the wxWinUI C++/WinRT projection"
    VERBATIM)

if(wxPLATFORM_ARCH)
    set(wxWINUI3_ARCH "${wxPLATFORM_ARCH}")
else()
    set(wxWINUI3_ARCH "x86")
endif()
if(wxWINUI3_ARCH STREQUAL "win32")
    set(wxWINUI3_ARCH "x86")
endif()
string(TOLOWER "${wxWINUI3_ARCH}" wxWINUI3_ARCH)
if(NOT wxWINUI3_ARCH MATCHES "^(x86|x64|arm64)$")
    message(FATAL_ERROR
        "wxWinUI: unsupported or unsafe target architecture "
        "\"${wxWINUI3_ARCH}\" (expected x86, x64 or arm64).")
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
    # Stable Windows App SDK 1.x component packages use a win10- prefix for
    # this import-library directory, while the 2.x engineering packages use
    # the bare architecture. Accept both package layouts without weakening
    # the exact package/hash pin above.
    set(wxWINUI3_DISPATCHING_LIB
        "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}/lib/native/win10-${wxWINUI3_ARCH}/Microsoft.UI.Dispatching.lib")
endif()
if(NOT EXISTS "${wxWINUI3_DISPATCHING_LIB}")
    message(FATAL_ERROR "wxWinUI dispatching library not found: ${wxWINUI3_DISPATCHING_LIB}")
endif()

# Refer to the Windows App SDK import libraries through stable target names.
# Shared wxWidgets libraries consume these targets privately. Static exports
# retain them in their link interface, and the installed wxWinUI package
# recreates the same targets with locations relative to its own config file.
if(NOT TARGET wx::winui3_bootstrap)
    add_library(wx::winui3_bootstrap UNKNOWN IMPORTED GLOBAL)
endif()
set_target_properties(wx::winui3_bootstrap PROPERTIES
    IMPORTED_LOCATION "${wxWINUI3_BOOTSTRAP_LIB}")
if(NOT TARGET wx::winui3_dispatching)
    add_library(wx::winui3_dispatching UNKNOWN IMPORTED GLOBAL)
endif()
set_target_properties(wx::winui3_dispatching PROPERTIES
    IMPORTED_LOCATION "${wxWINUI3_DISPATCHING_LIB}")

list(APPEND wxTOOLKIT_INCLUDE_DIRS
    "${wxWINUI3_CPPWINRT_PROJECTION_DIR}"
    "${wxWINUI3_CPPWINRT_INCLUDE_DIR}"
    "${wxWINUI3_WINUI_PACKAGE_DIR}/include"
    "${wxWINUI3_FOUNDATION_PACKAGE_DIR}/include"
    "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}/include"
    "${wxWINUI3_RUNTIME_PACKAGE_DIR}/include"
    )
list(APPEND wxTOOLKIT_LIBRARIES
    wx::winui3_bootstrap
    wx::winui3_dispatching
    windowsapp
    runtimeobject
    dwmapi
    uiautomationcore
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
# The stable WindowsAppSDK NuGet component packages explicitly binplace their
# component PRIs and theme assets for framework-dependent deployments.  The
# Windows SDK PRI indexer merges those component PRIs into an application map
# named "Application", which is exactly what an unpackaged process resolves
# for ms-appx:///.  This follows the documented unpackaged MakePri workflow
# and does not extract, modify or redistribute the framework package MSIX.
# build/cmake/winui-priconfig.xml deliberately emits one self-contained PRI,
# because unpackaged applications have no MSIX deployment service to install
# language/scale resource packages.
#
# Every WindowsAppSDK package used here is hash-pinned and its license.txt is
# checked above for the stable NuGet binplacement redistribution grant.  An
# Engineering Preview package is rejected at configure time.

set(wxWINUI3_COMPONENT_PRI_FILES
    "${wxWINUI3_WINUI_PACKAGE_DIR}/runtimes-framework/win-${wxWINUI3_ARCH}/native/Microsoft.UI.Xaml.Controls.pri"
    "${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}/runtimes-framework/win-${wxWINUI3_ARCH}/native/Microsoft.UI.pri"
    "${wxWINUI3_FOUNDATION_PACKAGE_DIR}/runtimes-framework/win-${wxWINUI3_ARCH}/native/Microsoft.WindowsAppRuntime.pri"
    )
set(wxWINUI3_THEME_ASSET_ENTRIES
    Microsoft.UI.Xaml/Assets/NoiseAsset_256x256_PNG.png
    Microsoft.UI.Xaml/Assets/map.html
    )
set(wxWINUI3_PRI_CONFIG "${wxSOURCE_DIR}/build/cmake/winui-priconfig.xml")

foreach(wxWINUI3_THEME_ASSET IN LISTS wxWINUI3_THEME_ASSET_ENTRIES)
    if(NOT EXISTS
            "${wxWINUI3_WINUI_PACKAGE_DIR}/runtimes-framework/win-${wxWINUI3_ARCH}/native/${wxWINUI3_THEME_ASSET}")
        message(FATAL_ERROR
            "wxWinUI: theme asset for win-${wxWINUI3_ARCH} is missing: "
            "${wxWINUI3_THEME_ASSET}")
    endif()
endforeach()

if(wxWINUI3_DEPLOY_RESOURCES_PRI)
    foreach(wxWINUI3_COMPONENT_PRI IN LISTS wxWINUI3_COMPONENT_PRI_FILES)
        if(NOT EXISTS "${wxWINUI3_COMPONENT_PRI}")
            message(FATAL_ERROR
                "wxWinUI: component PRI for win-${wxWINUI3_ARCH} is missing: "
                "${wxWINUI3_COMPONENT_PRI}")
        endif()
    endforeach()
    if(NOT EXISTS "${wxWINUI3_PRI_CONFIG}")
        message(FATAL_ERROR
            "wxWinUI: MakePri configuration is missing: "
            "${wxWINUI3_PRI_CONFIG}")
    endif()

    # Fingerprint the content of every input and of MakePri itself. The stage
    # directory is content-addressed, so a repin never destroys the last valid
    # payload while a replacement is being generated.
    # Bump the contract version whenever the MakePri invocation or manifest
    # semantics change, even if all file inputs stay byte-identical.
    set(wxWINUI3_PAYLOAD_FINGERPRINT_INPUT "contract=wx-pri-v1\n")
    foreach(wxWINUI3_COMPONENT_PRI IN LISTS wxWINUI3_COMPONENT_PRI_FILES)
        file(SHA256 "${wxWINUI3_COMPONENT_PRI}"
            wxWINUI3_COMPONENT_PRI_HASH)
        get_filename_component(wxWINUI3_COMPONENT_PRI_NAME
            "${wxWINUI3_COMPONENT_PRI}" NAME)
        string(APPEND wxWINUI3_PAYLOAD_FINGERPRINT_INPUT
            "${wxWINUI3_COMPONENT_PRI_NAME}="
            "${wxWINUI3_COMPONENT_PRI_HASH}\n")
    endforeach()
    file(SHA256 "${wxWINUI3_PRI_CONFIG}" wxWINUI3_PRI_CONFIG_HASH)
    file(SHA256 "${wxWINUI3_MAKEPRI_TOOL}" wxWINUI3_MAKEPRI_HASH)
    string(APPEND wxWINUI3_PAYLOAD_FINGERPRINT_INPUT
        "config=${wxWINUI3_PRI_CONFIG_HASH}\n"
        "makepri=${wxWINUI3_MAKEPRI_HASH}\n"
        "sdk=${wxWINUI3_WINDOWS_SDK_VERSION}\n")
    string(SHA256 wxWINUI3_PAYLOAD_SOURCE_HASH
        "${wxWINUI3_PAYLOAD_FINGERPRINT_INPUT}")

    function(wx_winui3_prepare_runtime_pri out_var)
        set(stage_parent
            "${wxBINARY_DIR}/winui/runtime-payload/${wxWINUI3_WINDOWSAPPSDK_VERSION}/win-${wxWINUI3_ARCH}")
        set(stage_dir
            "${stage_parent}/${wxWINUI3_PAYLOAD_SOURCE_HASH}")
        set(stage_tmp "${stage_dir}.generating")
        set(manifest_name "wxwinui-pri-manifest.txt")

        file(MAKE_DIRECTORY "${stage_parent}")
        file(LOCK "${stage_parent}/.wxwinui-pri-stage.lock"
            GUARD FUNCTION TIMEOUT 120 RESULT_VARIABLE stage_lock_result)
        if(NOT stage_lock_result EQUAL 0)
            message(FATAL_ERROR
                "wxWinUI: could not lock the PRI stage: "
                "${stage_lock_result}")
        endif()

        set(stage_complete FALSE)
        if(EXISTS "${stage_dir}/resources.pri" AND
                EXISTS "${stage_dir}/${manifest_name}")
            file(SHA256 "${stage_dir}/resources.pri" staged_pri_hash)
            file(SIZE "${stage_dir}/resources.pri" staged_pri_size)
            set(actual_manifest
                "source=${wxWINUI3_PAYLOAD_SOURCE_HASH}\n"
                "makepri=${wxWINUI3_MAKEPRI_HASH}\n"
                "resources.pri=${staged_pri_hash}\n"
                "resources.pri.size=${staged_pri_size}\n")
            file(READ "${stage_dir}/${manifest_name}" stored_manifest)
            if(stored_manifest STREQUAL actual_manifest AND
                    staged_pri_size GREATER_EQUAL 1024)
                set(stage_complete TRUE)
            endif()
        endif()

        if(NOT stage_complete)
            # Generate beside the current stage. Only after every check and
            # manifest write succeeds do we replace an incomplete/corrupt
            # same-fingerprint directory.
            file(REMOVE_RECURSE "${stage_tmp}")
            set(pri_input_dir "${stage_tmp}/pri-input")
            file(MAKE_DIRECTORY "${pri_input_dir}")
            foreach(component_pri IN LISTS wxWINUI3_COMPONENT_PRI_FILES)
                execute_process(
                    COMMAND "${CMAKE_COMMAND}" -E copy
                        "${component_pri}" "${pri_input_dir}"
                    RESULT_VARIABLE pri_copy_result)
                if(NOT pri_copy_result EQUAL 0)
                    file(REMOVE_RECURSE "${stage_tmp}")
                    message(FATAL_ERROR
                        "wxWinUI: failed to stage component PRI "
                        "${component_pri}")
                endif()
            endforeach()

            execute_process(
                COMMAND "${wxWINUI3_MAKEPRI_TOOL}" new
                    /pr "${pri_input_dir}"
                    /cf "${wxWINUI3_PRI_CONFIG}"
                    /of "${stage_tmp}/resources.pri"
                    /o
                RESULT_VARIABLE pri_generate_result
                OUTPUT_VARIABLE pri_generate_stdout
                ERROR_VARIABLE pri_generate_stderr)
            if(NOT pri_generate_result EQUAL 0 OR
                    NOT EXISTS "${stage_tmp}/resources.pri")
                file(REMOVE_RECURSE "${stage_tmp}")
                message(FATAL_ERROR
                    "wxWinUI: MakePri failed to generate the unpackaged "
                    "application resource index (exit "
                    "${pri_generate_result}).\n${pri_generate_stdout}\n"
                    "${pri_generate_stderr}")
            endif()

            file(SIZE "${stage_tmp}/resources.pri" generated_pri_size)
            if(generated_pri_size LESS 1024)
                file(REMOVE_RECURSE "${stage_tmp}")
                message(FATAL_ERROR
                    "wxWinUI: MakePri produced an empty/incomplete "
                    "application resource index "
                    "(${generated_pri_size} bytes)")
            endif()
            file(SHA256 "${stage_tmp}/resources.pri" generated_pri_hash)
            file(REMOVE_RECURSE "${pri_input_dir}")
            file(WRITE "${stage_tmp}/${manifest_name}"
                "source=${wxWINUI3_PAYLOAD_SOURCE_HASH}\n"
                "makepri=${wxWINUI3_MAKEPRI_HASH}\n"
                "resources.pri=${generated_pri_hash}\n"
                "resources.pri.size=${generated_pri_size}\n")

            # The previous directory is either absent or failed its manifest
            # verification. Keep it until the replacement is fully ready.
            file(REMOVE_RECURSE "${stage_dir}")
            file(RENAME "${stage_tmp}" "${stage_dir}")
        endif()

        set(${out_var} "${stage_dir}" PARENT_SCOPE)
    endfunction()

    wx_winui3_prepare_runtime_pri(wxWINUI3_PAYLOAD_STAGE_DIR)
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
    list(FIND wxWINUI3_RUNTIME_PAYLOAD_DST "${dst}" existing_dst_index)
    if(NOT existing_dst_index EQUAL -1)
        list(GET wxWINUI3_RUNTIME_PAYLOAD_SRC
            ${existing_dst_index} existing_src)
        get_filename_component(src_abs "${src}" ABSOLUTE)
        get_filename_component(existing_src_abs "${existing_src}" ABSOLUTE)
        file(TO_CMAKE_PATH "${src_abs}" src_abs)
        file(TO_CMAKE_PATH "${existing_src_abs}" existing_src_abs)
        if(NOT src_abs STREQUAL existing_src_abs)
            message(FATAL_ERROR
                "wxWinUI runtime payload collision for \"${dst}\":\n"
                "  ${existing_src}\n"
                "  ${src}")
        endif()
        return()
    endif()
    # The generated script batches copy_if_different calls by destination
    # directory; CMake's multi-source form preserves each basename. Reject a
    # misleading rename request instead of silently deploying another name.
    get_filename_component(src_name "${src}" NAME)
    get_filename_component(dst_name "${dst}" NAME)
    if(NOT src_name STREQUAL dst_name)
        message(FATAL_ERROR
            "wxWinUI runtime payload cannot rename ${src_name} to "
            "${dst_name}; choose a destination directory and preserve the "
            "source basename.")
    endif()

    list(APPEND wxWINUI3_RUNTIME_PAYLOAD_SRC "${src}")
    list(APPEND wxWINUI3_RUNTIME_PAYLOAD_DST "${dst}")
    set(wxWINUI3_RUNTIME_PAYLOAD_SRC "${wxWINUI3_RUNTIME_PAYLOAD_SRC}" PARENT_SCOPE)
    set(wxWINUI3_RUNTIME_PAYLOAD_DST "${wxWINUI3_RUNTIME_PAYLOAD_DST}" PARENT_SCOPE)
endfunction()

wx_winui3_add_payload_file(
    "${wxWINUI3_BOOTSTRAP_DLL}"
    "Microsoft.WindowsAppRuntime.Bootstrap.dll")
wx_winui3_add_payload_file(
    "${wxWINUI3_WEBVIEW2_PACKAGE_DIR}/runtimes/win-${wxWINUI3_ARCH}/native_uap/Microsoft.Web.WebView2.Core.dll"
    "Microsoft.Web.WebView2.Core.dll")
wx_winui3_add_payload_file(
    "${wxWINUI3_WEBVIEW2_PACKAGE_DIR}/build/native/${wxWINUI3_ARCH}/WebView2Loader.dll"
    "WebView2Loader.dll")
wx_winui3_add_payload_file(
    "${wxWINUI3_WEBVIEW2_PACKAGE_DIR}/LICENSE.txt"
    "licenses/Microsoft.Web.WebView2/LICENSE.txt")
wx_winui3_add_payload_file(
    "${wxWINUI3_WEBVIEW2_PACKAGE_DIR}/NOTICE.txt"
    "licenses/Microsoft.Web.WebView2/NOTICE.txt")

if(wxWINUI3_DEPLOY_RESOURCES_PRI)
    wx_winui3_add_payload_file(
        "${wxWINUI3_PAYLOAD_STAGE_DIR}/resources.pri"
        "resources.pri")
endif()
foreach(wxWINUI3_THEME_ASSET IN LISTS wxWINUI3_THEME_ASSET_ENTRIES)
    wx_winui3_add_payload_file(
        "${wxWINUI3_WINUI_PACKAGE_DIR}/runtimes-framework/win-${wxWINUI3_ARCH}/native/${wxWINUI3_THEME_ASSET}"
        "${wxWINUI3_THEME_ASSET}")
endforeach()
foreach(wxWINUI3_METADATA_FILE IN LISTS wxWINUI3_RUNTIME_METADATA_FILES)
    get_filename_component(wxWINUI3_METADATA_NAME
        "${wxWINUI3_METADATA_FILE}" NAME)
    wx_winui3_add_payload_file(
        "${wxWINUI3_METADATA_FILE}"
        "${wxWINUI3_METADATA_NAME}")
endforeach()

# Deterministic inventory of the deployed payload in the configure log.
message(STATUS "wxWinUI: unpackaged runtime payload (win-${wxWINUI3_ARCH}):")
list(LENGTH wxWINUI3_RUNTIME_PAYLOAD_SRC wxWINUI3_RUNTIME_PAYLOAD_COUNT)
math(EXPR wxWINUI3_RUNTIME_PAYLOAD_LAST "${wxWINUI3_RUNTIME_PAYLOAD_COUNT} - 1")
foreach(wxWINUI3_PAYLOAD_INDEX RANGE ${wxWINUI3_RUNTIME_PAYLOAD_LAST})
    list(GET wxWINUI3_RUNTIME_PAYLOAD_SRC ${wxWINUI3_PAYLOAD_INDEX} wxWINUI3_PAYLOAD_SRC)
    list(GET wxWINUI3_RUNTIME_PAYLOAD_DST ${wxWINUI3_PAYLOAD_INDEX} wxWINUI3_PAYLOAD_DST)
    message(STATUS "  ${wxWINUI3_PAYLOAD_DST} <= ${wxWINUI3_PAYLOAD_SRC}")
endforeach()

# Generate the deployment script once; every WinUI executable depends on an
# always-run deployment target against its own output directory. This restores
# a removed/stale payload even when the linker itself is already up to date.
# copy_if_different keeps the work cheap and safe for targets sharing output.
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
else()
    # Opting out transfers ownership of resources.pri to the application.
    # Remove only a sidecar whose content still exactly matches our ownership
    # tag; an application-provided/replaced PRI is never deleted.
    string(APPEND wxWINUI3_DEPLOY_SCRIPT_CONTENT
"set(wx_winui_pri_dst \"\${WX_WINUI_DEPLOY_DIR}/resources.pri\")
set(wx_winui_pri_tag \"\${WX_WINUI_DEPLOY_DIR}/resources.pri.wxwinui\")
if(EXISTS \"\${wx_winui_pri_tag}\")
    set(wx_winui_remove_owned_pri FALSE)
    if(EXISTS \"\${wx_winui_pri_dst}\")
        file(READ \"\${wx_winui_pri_tag}\" wx_winui_pri_tag_hash)
        string(STRIP \"\${wx_winui_pri_tag_hash}\" wx_winui_pri_tag_hash)
        file(SHA256 \"\${wx_winui_pri_dst}\" wx_winui_pri_dst_hash)
        if(wx_winui_pri_tag_hash STREQUAL wx_winui_pri_dst_hash)
            set(wx_winui_remove_owned_pri TRUE)
        endif()
    endif()
    if(wx_winui_remove_owned_pri)
        file(REMOVE \"\${wx_winui_pri_dst}\")
    endif()
    file(REMOVE \"\${wx_winui_pri_tag}\")
endif()
")
endif()
file(WRITE "${wxWINUI3_DEPLOY_SCRIPT}" "${wxWINUI3_DEPLOY_SCRIPT_CONTENT}")

# Build a second copy of the deployment script for installed consumers. The
# installed payload mirrors wxWINUI3_RUNTIME_PAYLOAD_DST below the script's
# own directory, so replacing every build/package source with that relative
# location produces a relocatable script without duplicating the deployment
# and resources.pri ownership logic above.
set(wxWINUI3_INSTALL_DEPLOY_SCRIPT_CONTENT
    "${wxWINUI3_DEPLOY_SCRIPT_CONTENT}")
foreach(wxWINUI3_PAYLOAD_INDEX RANGE ${wxWINUI3_RUNTIME_PAYLOAD_LAST})
    list(GET wxWINUI3_RUNTIME_PAYLOAD_SRC
        ${wxWINUI3_PAYLOAD_INDEX} wxWINUI3_INSTALL_PAYLOAD_SRC)
    list(GET wxWINUI3_RUNTIME_PAYLOAD_DST
        ${wxWINUI3_PAYLOAD_INDEX} wxWINUI3_INSTALL_PAYLOAD_DST)
    set(wxWINUI3_INSTALL_PAYLOAD_LOCATION
        "\${CMAKE_CURRENT_LIST_DIR}/runtime/${wxWINUI3_INSTALL_PAYLOAD_DST}")
    string(REPLACE
        "${wxWINUI3_INSTALL_PAYLOAD_SRC}"
        "${wxWINUI3_INSTALL_PAYLOAD_LOCATION}"
        wxWINUI3_INSTALL_DEPLOY_SCRIPT_CONTENT
        "${wxWINUI3_INSTALL_DEPLOY_SCRIPT_CONTENT}")
endforeach()

# This is a configure-time invariant, not just a packaging test: none of the
# NuGet cache or generated PRI sources may survive in an installed CMake file.
foreach(wxWINUI3_INSTALL_PAYLOAD_SRC IN LISTS wxWINUI3_RUNTIME_PAYLOAD_SRC)
    string(FIND "${wxWINUI3_INSTALL_DEPLOY_SCRIPT_CONTENT}"
        "${wxWINUI3_INSTALL_PAYLOAD_SRC}" wxWINUI3_INSTALL_SOURCE_INDEX)
    if(NOT wxWINUI3_INSTALL_SOURCE_INDEX EQUAL -1)
        message(FATAL_ERROR
            "wxWinUI internal error: installed deployment script retained "
            "build/package path ${wxWINUI3_INSTALL_PAYLOAD_SRC}")
    endif()
endforeach()
# The source-tree option has already been resolved by the time this installed
# script is generated. Do not tell consumers that setting a cache variable in
# their own project can change the immutable package payload.
string(REPLACE
    "set wxWINUI3_DEPLOY_RESOURCES_PRI=OFF to silence this"
    "rebuild and reinstall wxWidgets with wxWINUI3_DEPLOY_RESOURCES_PRI=OFF to omit it; setting this variable in a consuming project has no effect"
    wxWINUI3_INSTALL_DEPLOY_SCRIPT_CONTENT
    "${wxWINUI3_INSTALL_DEPLOY_SCRIPT_CONTENT}")
set(wxWINUI3_INSTALL_DEPLOY_SCRIPT
    "${wxBINARY_DIR}/winui/install/wxWinUIDeployRuntime.cmake")
get_filename_component(wxWINUI3_INSTALL_DEPLOY_SCRIPT_DIR
    "${wxWINUI3_INSTALL_DEPLOY_SCRIPT}" DIRECTORY)
file(MAKE_DIRECTORY "${wxWINUI3_INSTALL_DEPLOY_SCRIPT_DIR}")
file(WRITE "${wxWINUI3_INSTALL_DEPLOY_SCRIPT}"
    "${wxWINUI3_INSTALL_DEPLOY_SCRIPT_CONTENT}")

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

    set(deploy_target "${target_name}_winui_runtime_deploy")
    add_custom_target(${deploy_target}
        COMMAND "${CMAKE_COMMAND}"
            -DWX_WINUI_DEPLOY_DIR=$<TARGET_FILE_DIR:${target_name}>
            -P "${wxWINUI3_DEPLOY_SCRIPT}"
        COMMENT "Deploying the WinUI runtime payload for ${target_name}"
        VERBATIM
        )
    set_property(TARGET ${deploy_target} PROPERTY FOLDER "Build Infrastructure")
    add_dependencies(${target_name} ${deploy_target})
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
    # Focus-sensitive integration tests cannot safely run on the user's input
    # desktop: SetFocus() may activate their top-level window. This standalone
    # launcher creates a private desktop without ever switching to it, assigns
    # the child to a kill-on-close job and forwards its console output.
    add_executable(wx_winui_desktop_test_runner
        "${wxSOURCE_DIR}/tests/winui/desktop-test-runner.cpp")
    target_link_libraries(wx_winui_desktop_test_runner PRIVATE
        user32 ole32 oleaut32 uiautomationcore)
    set_target_properties(wx_winui_desktop_test_runner PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        FOLDER "Tests"
        RUNTIME_OUTPUT_DIRECTORY
            "${wxBINARY_DIR}/winui/test-tools/$<CONFIG>$<1:/>"
        )

    add_executable(wx_winui_runtime_smoke
        "${wxSOURCE_DIR}/tests/winui/runtime-smoke.cpp")
    add_dependencies(wx_winui_runtime_smoke wxwinui_cppwinrt)
    target_include_directories(wx_winui_runtime_smoke PRIVATE
        "${wxWINUI3_CPPWINRT_PROJECTION_DIR}"
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
        ole32
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
    set_tests_properties(wx_winui_runtime_smoke PROPERTIES TIMEOUT 30)
    add_test(NAME wx_winui_ole_drop_probe
        COMMAND wx_winui_runtime_smoke --ole-drop-probe
        WORKING_DIRECTORY "${wxBINARY_DIR}/winui/runtime-smoke")
    set_tests_properties(wx_winui_ole_drop_probe PROPERTIES TIMEOUT 60)

    # Offline self-tests of the configure-time machinery above (they load
    # this file with wxWINUI3_SELFTEST_FUNCTIONS_ONLY).
    add_test(NAME wx_winui_selftest_sdk_selection
        COMMAND "${CMAKE_COMMAND}"
            -DWINUI_CMAKE=${wxSOURCE_DIR}/build/cmake/winui.cmake
            -DSAFE_ROOT=${wxBINARY_DIR}/winui/selftest
            -DWORK_DIR=${wxBINARY_DIR}/winui/selftest/sdk-selection
            -P "${wxSOURCE_DIR}/tests/winui/selftest/sdk-selection.cmake")
    add_test(NAME wx_winui_selftest_nupkg_cache
        COMMAND "${CMAKE_COMMAND}"
            -DWINUI_CMAKE=${wxSOURCE_DIR}/build/cmake/winui.cmake
            -DSAFE_ROOT=${wxBINARY_DIR}/winui/selftest
            -DWORK_DIR=${wxBINARY_DIR}/winui/selftest/nupkg-cache
            -P "${wxSOURCE_DIR}/tests/winui/selftest/nupkg-cache.cmake")
    add_test(NAME wx_winui_selftest_package_license
        COMMAND "${CMAKE_COMMAND}"
            -DWINUI_CMAKE=${wxSOURCE_DIR}/build/cmake/winui.cmake
            -DSAFE_ROOT=${wxBINARY_DIR}/winui/selftest
            -DWORK_DIR=${wxBINARY_DIR}/winui/selftest/package-license
            -P "${wxSOURCE_DIR}/tests/winui/selftest/package-license.cmake")
    add_test(NAME wx_winui_selftest_package_set
        COMMAND "${CMAKE_COMMAND}"
            -DWINUI_CMAKE=${wxSOURCE_DIR}/build/cmake/winui.cmake
            -DSAFE_ROOT=${wxBINARY_DIR}/winui/selftest
            -DWORK_DIR=${wxBINARY_DIR}/winui/selftest/package-set
            -P "${wxSOURCE_DIR}/tests/winui/selftest/package-set.cmake")
    add_test(NAME wx_winui_selftest_cppwinrt_cache
        COMMAND "${CMAKE_COMMAND}"
            -DWINUI_CMAKE=${wxSOURCE_DIR}/build/cmake/winui.cmake
            -DCPPWINRT_WORKER=${wxSOURCE_DIR}/build/cmake/winui-cppwinrt.cmake
            -DFAKE_GENERATOR=${wxSOURCE_DIR}/tests/winui/selftest/fake-cppwinrt-generator.cmake
            -DSAFE_ROOT=${wxBINARY_DIR}/winui/selftest
            -DWORK_DIR=${wxBINARY_DIR}/winui/selftest/cppwinrt-cache
            -P "${wxSOURCE_DIR}/tests/winui/selftest/cppwinrt-cache.cmake")
    add_test(NAME wx_winui_selftest_toolkit_requires_winui3
        COMMAND "${CMAKE_COMMAND}"
            -DWINUI_CMAKE=${wxSOURCE_DIR}/build/cmake/winui.cmake
            -DSOURCE_DIR=${wxSOURCE_DIR}
            -DSAFE_ROOT=${wxBINARY_DIR}/winui/selftest
            -DWORK_DIR=${wxBINARY_DIR}/winui/selftest/toolkit-requires-winui3
            -P "${wxSOURCE_DIR}/tests/winui/selftest/toolkit-requires-winui3.cmake")
    set_tests_properties(
        wx_winui_selftest_toolkit_requires_winui3
        PROPERTIES TIMEOUT 75)

    # This test is registered from the top-level directory because samples
    # are configured before enable_testing() in build/cmake/main.cmake.
    # winuispike is an IMPORTANT sample, so it exists whenever samples are
    # enabled; the generator expression is resolved after all targets exist.
    if(wxBUILD_SAMPLES)
        add_test(NAME wx_winui_selftest_diagnostics_default_off
            COMMAND "${CMAKE_COMMAND}"
                "-DWINUISPIKE_EXE=$<TARGET_FILE:winuispike>"
                "-DTEST_ROOT=${wxBINARY_DIR}/winui/selftest"
                -P
                "${wxSOURCE_DIR}/tests/winui/selftest/diagnostics-default-off.cmake")
        set_tests_properties(
            wx_winui_selftest_diagnostics_default_off
            PROPERTIES TIMEOUT 30)
    endif()
endif()
