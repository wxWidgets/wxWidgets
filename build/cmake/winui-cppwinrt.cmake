#############################################################################
# Name:        build/cmake/winui-cppwinrt.cmake
# Purpose:     Transactional, content-addressed C++/WinRT projections
# Author:      wxWidgets development team
# Created:     2026-07-25
# Copyright:   (c) wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# Keep the fingerprint computation in one place: winui.cmake uses it while
# configuring the include directory and this file recomputes it immediately
# before every build. This prevents modified/corrupt metadata from being
# projected under the address of the original inputs.
function(wx_winui3_compute_cppwinrt_fingerprint out_var)
    cmake_parse_arguments(ARG ""
        "WINDOWS_WINMD;TOOL;AUXILIARY"
        "INPUTS"
        ${ARGN})

    if(NOT ARG_INPUTS OR
            NOT EXISTS "${ARG_WINDOWS_WINMD}" OR
            NOT EXISTS "${ARG_TOOL}")
        message(FATAL_ERROR
            "wxWinUI C++/WinRT fingerprint: missing metadata, Windows.winmd "
            "or generator tool.")
    endif()

    # This version covers the invocation contract and manifest semantics, not
    # just the binaries and metadata. Bump it whenever those rules change so
    # an older, otherwise valid cache cannot be reused under a new contract.
    set(fingerprint_input "contract=wx-cppwinrt-v1\n")
    foreach(input IN LISTS ARG_INPUTS)
        if(NOT EXISTS "${input}")
            message(FATAL_ERROR
                "wxWinUI C++/WinRT metadata input does not exist: ${input}")
        endif()
        file(SHA256 "${input}" input_hash)
        get_filename_component(input_name "${input}" NAME)
        string(APPEND fingerprint_input "${input_name}=${input_hash}\n")
    endforeach()

    file(SHA256 "${ARG_WINDOWS_WINMD}" windows_winmd_hash)
    file(SHA256 "${ARG_TOOL}" tool_hash)
    string(APPEND fingerprint_input
        "Windows.winmd=${windows_winmd_hash}\n"
        "cppwinrt.exe=${tool_hash}\n")

    if(ARG_AUXILIARY)
        if(NOT EXISTS "${ARG_AUXILIARY}")
            message(FATAL_ERROR
                "wxWinUI C++/WinRT auxiliary generator does not exist: "
                "${ARG_AUXILIARY}")
        endif()
        file(SHA256 "${ARG_AUXILIARY}" auxiliary_hash)
        string(APPEND fingerprint_input
            "auxiliary-generator=${auxiliary_hash}\n")
    endif()

    string(SHA256 fingerprint "${fingerprint_input}")
    set(${out_var} "${fingerprint}" PARENT_SCOPE)
endfunction()

function(wx_winui3_validate_cppwinrt_manifest out_var projection_dir
         expected_fingerprint required_relative)
    set(valid TRUE)
    set(manifest "${projection_dir}/wxwinui-projection-manifest.txt")

    if(NOT EXISTS "${manifest}")
        set(valid FALSE)
    else()
        file(STRINGS "${manifest}" manifest_lines)
        list(LENGTH manifest_lines manifest_line_count)
        if(manifest_line_count LESS 2)
            set(valid FALSE)
        else()
            list(GET manifest_lines 0 fingerprint_line)
            if(NOT fingerprint_line STREQUAL
                    "fingerprint=${expected_fingerprint}")
                set(valid FALSE)
            endif()
        endif()
    endif()

    set(recorded_files)
    set(required_seen FALSE)
    if(valid)
        list(REMOVE_AT manifest_lines 0)
        foreach(line IN LISTS manifest_lines)
            if(NOT line MATCHES "^([0-9a-f]+)\\|([0-9]+)\\|(.+)$")
                set(valid FALSE)
                break()
            endif()

            set(recorded_hash "${CMAKE_MATCH_1}")
            set(recorded_size "${CMAKE_MATCH_2}")
            set(relative_path "${CMAKE_MATCH_3}")
            string(FIND "${relative_path}" "\\" backslash_index)
            string(LENGTH "${recorded_hash}" recorded_hash_length)
            if(NOT recorded_hash_length EQUAL 64 OR
                    IS_ABSOLUTE "${relative_path}" OR
                    relative_path MATCHES "(^|/)\\.\\.(/|$)" OR
                    NOT backslash_index EQUAL -1 OR
                    relative_path MATCHES "[\r\n|]")
                set(valid FALSE)
                break()
            endif()

            set(file_path "${projection_dir}/${relative_path}")
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
            if(relative_path STREQUAL "${required_relative}")
                set(required_seen TRUE)
            endif()
        endforeach()
    endif()

    # A manifest also records the complete file set, not just one sentinel
    # header. Missing, injected or corrupted secondary projection headers are
    # therefore caught before any consumer is compiled.
    if(valid)
        file(GLOB_RECURSE actual_files
            RELATIVE "${projection_dir}"
            "${projection_dir}/*")
        list(REMOVE_ITEM actual_files "wxwinui-projection-manifest.txt")
        list(SORT actual_files)
        list(SORT recorded_files)
        if(NOT required_seen OR NOT actual_files STREQUAL recorded_files)
            set(valid FALSE)
        endif()
    endif()

    set(${out_var} "${valid}" PARENT_SCOPE)
endfunction()

function(wx_winui3_write_cppwinrt_manifest projection_dir fingerprint
         required_relative)
    file(GLOB_RECURSE generated_files
        RELATIVE "${projection_dir}"
        "${projection_dir}/*")
    list(REMOVE_ITEM generated_files "wxwinui-projection-manifest.txt")
    list(SORT generated_files)

    if(NOT EXISTS "${projection_dir}/${required_relative}")
        message(FATAL_ERROR
            "wxWinUI C++/WinRT generation did not produce "
            "${required_relative}.")
    endif()

    set(manifest "${projection_dir}/wxwinui-projection-manifest.txt")
    file(WRITE "${manifest}" "fingerprint=${fingerprint}\n")
    foreach(relative_path IN LISTS generated_files)
        set(file_path "${projection_dir}/${relative_path}")
        if(IS_DIRECTORY "${file_path}" OR relative_path MATCHES "[\r\n|]")
            message(FATAL_ERROR
                "wxWinUI C++/WinRT produced an unsafe manifest path: "
                "${relative_path}")
        endif()
        file(SHA256 "${file_path}" file_hash)
        file(SIZE "${file_path}" file_size)
        file(APPEND "${manifest}"
            "${file_hash}|${file_size}|${relative_path}\n")
    endforeach()
endfunction()

function(wx_winui3_ensure_cppwinrt_projection)
    foreach(required_var IN ITEMS
            WX_CPPWINRT_OUTPUT_ROOT
            WX_CPPWINRT_FINGERPRINT
            WX_CPPWINRT_INPUT_LIST
            WX_CPPWINRT_WINDOWS_WINMD
            WX_CPPWINRT_TOOL
            WX_CPPWINRT_REQUIRED_RELATIVE)
        if(NOT DEFINED ${required_var} OR "${${required_var}}" STREQUAL "")
            message(FATAL_ERROR
                "wxWinUI C++/WinRT worker: ${required_var} is required.")
        endif()
    endforeach()

    string(LENGTH "${WX_CPPWINRT_FINGERPRINT}" fingerprint_length)
    if(NOT fingerprint_length EQUAL 64 OR
            NOT WX_CPPWINRT_FINGERPRINT MATCHES "^[0-9a-f]+$")
        message(FATAL_ERROR
            "wxWinUI C++/WinRT worker: unsafe fingerprint "
            "\"${WX_CPPWINRT_FINGERPRINT}\".")
    endif()
    if(NOT EXISTS "${WX_CPPWINRT_INPUT_LIST}")
        message(FATAL_ERROR
            "wxWinUI C++/WinRT worker: input list does not exist: "
            "${WX_CPPWINRT_INPUT_LIST}")
    endif()

    file(STRINGS "${WX_CPPWINRT_INPUT_LIST}" cppwinrt_inputs)
    if(NOT cppwinrt_inputs)
        message(FATAL_ERROR
            "wxWinUI C++/WinRT worker: metadata input list is empty.")
    endif()

    set(auxiliary_arg)
    if(DEFINED WX_CPPWINRT_TEST_GENERATOR AND
            NOT "${WX_CPPWINRT_TEST_GENERATOR}" STREQUAL "")
        set(auxiliary_arg AUXILIARY "${WX_CPPWINRT_TEST_GENERATOR}")
    endif()
    wx_winui3_compute_cppwinrt_fingerprint(actual_fingerprint
        INPUTS ${cppwinrt_inputs}
        WINDOWS_WINMD "${WX_CPPWINRT_WINDOWS_WINMD}"
        TOOL "${WX_CPPWINRT_TOOL}"
        ${auxiliary_arg})
    if(NOT actual_fingerprint STREQUAL WX_CPPWINRT_FINGERPRINT)
        message(FATAL_ERROR
            "wxWinUI C++/WinRT inputs changed after configuration "
            "(expected ${WX_CPPWINRT_FINGERPRINT}, got "
            "${actual_fingerprint}). Re-run CMake before building.")
    endif()

    get_filename_component(output_root
        "${WX_CPPWINRT_OUTPUT_ROOT}" ABSOLUTE)
    file(MAKE_DIRECTORY "${output_root}")
    set(projection_dir
        "${output_root}/${WX_CPPWINRT_FINGERPRINT}")
    set(stage_dir
        "${output_root}/.${WX_CPPWINRT_FINGERPRINT}.generating")
    set(invalid_dir
        "${output_root}/.${WX_CPPWINRT_FINGERPRINT}.invalid")

    file(LOCK "${output_root}/.wxwinui-cppwinrt.lock"
        GUARD FUNCTION TIMEOUT 300)

    wx_winui3_validate_cppwinrt_manifest(projection_valid
        "${projection_dir}"
        "${WX_CPPWINRT_FINGERPRINT}"
        "${WX_CPPWINRT_REQUIRED_RELATIVE}")
    if(projection_valid)
        message(STATUS
            "wxWinUI C++/WinRT projection verified: ${projection_dir}")
        return()
    endif()

    # The only recursively removed paths are fixed children of OUTPUT_ROOT
    # built from a validated SHA-256 fingerprint.
    file(REMOVE_RECURSE "${stage_dir}" "${invalid_dir}")
    file(MAKE_DIRECTORY "${stage_dir}")

    if(DEFINED WX_CPPWINRT_TEST_GENERATOR AND
            NOT "${WX_CPPWINRT_TEST_GENERATOR}" STREQUAL "")
        execute_process(
            COMMAND "${CMAKE_COMMAND}"
                "-DOUTPUT_DIR=${stage_dir}"
                "-DINPUT_LIST=${WX_CPPWINRT_INPUT_LIST}"
                -P "${WX_CPPWINRT_TEST_GENERATOR}"
            RESULT_VARIABLE generator_result
            OUTPUT_VARIABLE generator_stdout
            ERROR_VARIABLE generator_stderr)
    else()
        execute_process(
            COMMAND "${WX_CPPWINRT_TOOL}"
                -in ${cppwinrt_inputs}
                -ref "${WX_CPPWINRT_WINDOWS_WINMD}"
                -out "${stage_dir}"
            RESULT_VARIABLE generator_result
            OUTPUT_VARIABLE generator_stdout
            ERROR_VARIABLE generator_stderr)
    endif()
    if(NOT generator_result EQUAL 0)
        file(REMOVE_RECURSE "${stage_dir}")
        message(FATAL_ERROR
            "wxWinUI C++/WinRT generator failed (${generator_result}).\n"
            "${generator_stdout}\n${generator_stderr}")
    endif()

    wx_winui3_write_cppwinrt_manifest(
        "${stage_dir}"
        "${WX_CPPWINRT_FINGERPRINT}"
        "${WX_CPPWINRT_REQUIRED_RELATIVE}")
    wx_winui3_validate_cppwinrt_manifest(stage_valid
        "${stage_dir}"
        "${WX_CPPWINRT_FINGERPRINT}"
        "${WX_CPPWINRT_REQUIRED_RELATIVE}")
    if(NOT stage_valid)
        file(REMOVE_RECURSE "${stage_dir}")
        message(FATAL_ERROR
            "wxWinUI C++/WinRT generated projection failed its manifest "
            "validation.")
    endif()

    # Preserve an invalid same-address directory until the new, fully checked
    # generation exists. The lock serializes all build/configuration users of
    # this cache root.
    if(EXISTS "${projection_dir}")
        file(RENAME "${projection_dir}" "${invalid_dir}")
    endif()
    file(RENAME "${stage_dir}" "${projection_dir}")
    file(REMOVE_RECURSE "${invalid_dir}")

    wx_winui3_validate_cppwinrt_manifest(installed_valid
        "${projection_dir}"
        "${WX_CPPWINRT_FINGERPRINT}"
        "${WX_CPPWINRT_REQUIRED_RELATIVE}")
    if(NOT installed_valid)
        message(FATAL_ERROR
            "wxWinUI C++/WinRT installed projection failed its manifest "
            "validation: ${projection_dir}")
    endif()
    message(STATUS
        "wxWinUI C++/WinRT projection generated: ${projection_dir}")
endfunction()

if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
    wx_winui3_ensure_cppwinrt_projection()
endif()
