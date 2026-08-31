cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED WX_WINUI_TEST_EXE OR
        NOT EXISTS "${WX_WINUI_TEST_EXE}")
    message(FATAL_ERROR
        "WX_WINUI_TEST_EXE must name the built consumer executable")
endif()
if(NOT DEFINED WX_WINUI_WX_DLL_DIR OR
        NOT IS_DIRECTORY "${WX_WINUI_WX_DLL_DIR}")
    message(FATAL_ERROR
        "WX_WINUI_WX_DLL_DIR must name the installed wx library directory")
endif()

# wx_winui3_deploy_runtime() deploys the Windows App SDK payload, not the
# wxWidgets DLLs themselves. Keep this fixture non-invasive: make the installed
# wx DLL directory visible to the Windows loader for this smoke process only.
set(ENV{PATH} "${WX_WINUI_WX_DLL_DIR};$ENV{PATH}")
get_filename_component(wx_winui_test_dir "${WX_WINUI_TEST_EXE}" DIRECTORY)
get_filename_component(wx_winui_test_name "${WX_WINUI_TEST_EXE}" NAME)

set(wx_winui_dump_dirs)
if(NOT "$ENV{WX_WINUI_DUMP_DIR}" STREQUAL "")
    list(APPEND wx_winui_dump_dirs "$ENV{WX_WINUI_DUMP_DIR}")
endif()
if(NOT "$ENV{LOCALAPPDATA}" STREQUAL "")
    list(APPEND wx_winui_dump_dirs "$ENV{LOCALAPPDATA}/CrashDumps")
endif()
if(WIN32)
    # LocalDumps is machine policy. An executable-specific key overrides the
    # global one; query in that order and use only the first configured folder.
    set(wx_winui_wer_keys
        "HKLM\\Software\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\\${wx_winui_test_name}"
        "HKLM\\Software\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps")
    foreach(wx_winui_wer_key IN LISTS wx_winui_wer_keys)
        execute_process(
            COMMAND reg.exe query "${wx_winui_wer_key}" /v DumpFolder
            RESULT_VARIABLE wx_winui_dump_folder_result
            OUTPUT_VARIABLE wx_winui_dump_folder_output
            ERROR_QUIET
            TIMEOUT 10)
        if(NOT "${wx_winui_dump_folder_result}" MATCHES "^-?[0-9]+$")
            message(FATAL_ERROR
                "Querying WER DumpFolder timed out or failed to launch for "
                "${wx_winui_wer_key}: ${wx_winui_dump_folder_result}")
        endif()
        if("${wx_winui_dump_folder_result}" STREQUAL "0")
            if(NOT wx_winui_dump_folder_output MATCHES
                    "DumpFolder[ \t]+REG_(EXPAND_)?SZ[ \t]+([^\r\n]+)")
                message(FATAL_ERROR
                    "Cannot parse WER DumpFolder from ${wx_winui_wer_key}:\n"
                    "${wx_winui_dump_folder_output}")
            endif()
            string(STRIP "${CMAKE_MATCH_2}" wx_winui_dump_folder)
            set(ENV{WX_WINUI_WER_DUMP_FOLDER}
                "${wx_winui_dump_folder}")
            execute_process(
                COMMAND powershell.exe -NoProfile -NonInteractive -Command
                    "[Environment]::ExpandEnvironmentVariables($env:WX_WINUI_WER_DUMP_FOLDER)"
                RESULT_VARIABLE wx_winui_expand_result
                OUTPUT_VARIABLE wx_winui_expanded_dump_folder
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE
                TIMEOUT 10)
            unset(ENV{WX_WINUI_WER_DUMP_FOLDER})
            if(NOT "${wx_winui_expand_result}" STREQUAL "0" OR
                    wx_winui_expanded_dump_folder STREQUAL "")
                message(FATAL_ERROR
                    "Cannot expand WER DumpFolder '${wx_winui_dump_folder}' "
                    "from ${wx_winui_wer_key}")
            endif()
            list(APPEND wx_winui_dump_dirs
                "${wx_winui_expanded_dump_folder}")
            break()
        endif()
    endforeach()
endif()
list(REMOVE_DUPLICATES wx_winui_dump_dirs)
function(wx_winui_collect_dump_fingerprints result)
    set(fingerprints)
    foreach(wx_winui_dump_dir IN LISTS wx_winui_dump_dirs)
        if(IS_DIRECTORY "${wx_winui_dump_dir}")
            file(GLOB dumps LIST_DIRECTORIES false
                "${wx_winui_dump_dir}/${wx_winui_test_name}*.dmp")
            foreach(dump IN LISTS dumps)
                file(SIZE "${dump}" dump_size)
                file(SHA256 "${dump}" dump_sha256)
                list(APPEND fingerprints
                    "${dump} | ${dump_size} bytes | SHA256=${dump_sha256}")
            endforeach()
        endif()
    endforeach()
    set(${result} "${fingerprints}" PARENT_SCOPE)
endfunction()

wx_winui_collect_dump_fingerprints(wx_winui_dumps_before)

execute_process(
    COMMAND "${WX_WINUI_TEST_EXE}"
    WORKING_DIRECTORY "${wx_winui_test_dir}"
    RESULT_VARIABLE wx_winui_test_result
    OUTPUT_VARIABLE wx_winui_test_output
    ERROR_VARIABLE wx_winui_test_error
    TIMEOUT 120)

execute_process(
    COMMAND "${WX_WINUI_TEST_EXE}" --mta-reject
    WORKING_DIRECTORY "${wx_winui_test_dir}"
    RESULT_VARIABLE wx_winui_mta_result
    OUTPUT_VARIABLE wx_winui_mta_output
    ERROR_VARIABLE wx_winui_mta_error
    TIMEOUT 120)

set(wx_winui_policy_failed false)
set(wx_winui_policy_report "")
set(wx_winui_mta_marker
    "wx_winui_mta_reject: initializer_rejected=1 runtime_rejected=1 mta_preserved=1 recovered=1 com_balanced=1 result=0")
string(FIND "${wx_winui_mta_output}"
    "${wx_winui_mta_marker}" wx_winui_mta_marker_offset)
if(wx_winui_mta_marker_offset EQUAL -1)
    set(wx_winui_policy_failed true)
endif()

# A failure after acquiring COM but before bootstrapping XAML must be clean,
# unlike the permanent quarantines below. Recovery is tested in this same
# child only after proving the one-shot guard was consumed and COM balanced.
execute_process(
    COMMAND "${WX_WINUI_TEST_EXE}" --runtime-fault before-bootstrap
    WORKING_DIRECTORY "${wx_winui_test_dir}"
    RESULT_VARIABLE wx_winui_clean_result
    OUTPUT_VARIABLE wx_winui_clean_output
    ERROR_VARIABLE wx_winui_clean_error
    TIMEOUT 120)
set(wx_winui_clean_marker
    "wx_winui_runtime_clean_fault: name=before-bootstrap initializer_rejected=1 env_consumed=1 com_balanced=1 recovered=1 result=0")
string(FIND "${wx_winui_clean_output}"
    "${wx_winui_clean_marker}" wx_winui_clean_marker_offset)
if(NOT "${wx_winui_clean_result}" STREQUAL "0" OR
        wx_winui_clean_marker_offset EQUAL -1)
    set(wx_winui_policy_failed true)
endif()
string(APPEND wx_winui_policy_report
    "\n[before-bootstrap] result=${wx_winui_clean_result}\n"
    "stdout:\n${wx_winui_clean_output}\n"
    "stderr:\n${wx_winui_clean_error}\n")

# Renderer installation is process-wide, not per runtime epoch. Separate
# children distinguish a custom renderer installed before the first Get()
# and wxInitializer from the normal Fluent default. Both then replace the
# live renderer, explicitly restore the native default and check ownership.
foreach(wx_winui_renderer_mode IN ITEMS custom default)
    execute_process(
        COMMAND "${WX_WINUI_TEST_EXE}"
            "--renderer-${wx_winui_renderer_mode}"
        WORKING_DIRECTORY "${wx_winui_test_dir}"
        RESULT_VARIABLE wx_winui_renderer_result
        OUTPUT_VARIABLE wx_winui_renderer_output
        ERROR_VARIABLE wx_winui_renderer_error
        TIMEOUT 120)
    if(wx_winui_renderer_mode STREQUAL "custom")
        set(wx_winui_renderer_initial_destroyed 1)
    else()
        set(wx_winui_renderer_initial_destroyed 0)
    endif()
    set(wx_winui_renderer_marker
        "wx_winui_renderer: mode=${wx_winui_renderer_mode} epochs=50 live_preserved=1 default_preserved=1 initial_destroyed=${wx_winui_renderer_initial_destroyed} replacement_destroyed=1 com_balanced=1 result=0")
    string(FIND "${wx_winui_renderer_output}"
        "${wx_winui_renderer_marker}" wx_winui_renderer_marker_offset)
    if(NOT "${wx_winui_renderer_result}" STREQUAL "0" OR
            wx_winui_renderer_marker_offset EQUAL -1)
        set(wx_winui_policy_failed true)
    endif()
    string(APPEND wx_winui_policy_report
        "\n[renderer-${wx_winui_renderer_mode}] result=${wx_winui_renderer_result}\n"
        "stdout:\n${wx_winui_renderer_output}\n"
        "stderr:\n${wx_winui_renderer_error}\n")
endforeach()

# Faults which intentionally retain an entire runtime epoch must never share a
# process: Quarantined is permanent by design. Each child proves that the
# required module failed startup, the one-shot guard was consumed, later
# initialization is rejected, the retained apartment survives wx module
# teardown and the process exits without an AV.
set(wx_winui_runtime_faults
    after-application
    after-xaml-manager
    reject-xaml-hook
    reject-required-queue-hook
    throw-shutdown-queue
    suppress-framework-completed)
set(wx_winui_fault_failed false)
set(wx_winui_fault_report "")
foreach(wx_winui_runtime_fault IN LISTS wx_winui_runtime_faults)
    execute_process(
        COMMAND "${WX_WINUI_TEST_EXE}"
            --runtime-fault "${wx_winui_runtime_fault}"
        WORKING_DIRECTORY "${wx_winui_test_dir}"
        RESULT_VARIABLE wx_winui_fault_result
        OUTPUT_VARIABLE wx_winui_fault_output
        ERROR_VARIABLE wx_winui_fault_error
        TIMEOUT 120)

    set(wx_winui_fault_marker
        "wx_winui_runtime_fault: name=${wx_winui_runtime_fault} initializer_rejected=1 env_consumed=1 init_rejected=1 sta_retained=1 result=0")
    string(FIND "${wx_winui_fault_output}"
        "${wx_winui_fault_marker}" wx_winui_fault_marker_offset)
    if(NOT "${wx_winui_fault_result}" STREQUAL "0" OR
            wx_winui_fault_marker_offset EQUAL -1)
        set(wx_winui_fault_failed true)
    endif()
    string(APPEND wx_winui_fault_report
        "\n[${wx_winui_runtime_fault}] result=${wx_winui_fault_result}\n"
        "stdout:\n${wx_winui_fault_output}\n"
        "stderr:\n${wx_winui_fault_error}\n")
endforeach()

# LocalDumps is optional, but when it is enabled no new WER artifact may be
# produced by a test which otherwise returns zero. The executable also has a
# vectored observer so cleanup AVs are detected on machines without LocalDumps.
set(wx_winui_new_dumps)
if(wx_winui_dump_dirs)
    # WER writes are asynchronous to process termination. Poll for a bounded
    # interval instead of sampling once: a late WerFault process must not make
    # an otherwise-zero consumer run a false pass.
    foreach(wx_winui_dump_poll RANGE 1 10)
        execute_process(COMMAND "${CMAKE_COMMAND}" -E sleep 1)
        wx_winui_collect_dump_fingerprints(wx_winui_dumps_after)
        set(wx_winui_new_dumps ${wx_winui_dumps_after})
        foreach(wx_winui_old_dump IN LISTS wx_winui_dumps_before)
            list(REMOVE_ITEM wx_winui_new_dumps "${wx_winui_old_dump}")
        endforeach()
        if(wx_winui_new_dumps)
            break()
        endif()
    endforeach()
endif()

if(wx_winui_new_dumps)
    string(REPLACE ";" "\n  " wx_winui_dump_report
        "${wx_winui_new_dumps}")
    message(FATAL_ERROR
        "Installed wxWinUI consumer generated WER crash dumps:\n"
        "  ${wx_winui_dump_report}\n"
        "STA/cycles stdout:\n${wx_winui_test_output}\n"
        "STA/cycles stderr:\n${wx_winui_test_error}\n"
        "MTA-rejection stdout:\n${wx_winui_mta_output}\n"
        "MTA-rejection stderr:\n${wx_winui_mta_error}\n"
        "Runtime policy subprocesses:${wx_winui_policy_report}"
        "Runtime fault subprocesses:${wx_winui_fault_report}")
endif()

if(NOT "${wx_winui_test_result}" STREQUAL "0" OR
        NOT "${wx_winui_mta_result}" STREQUAL "0" OR
        wx_winui_policy_failed OR
        wx_winui_fault_failed)
    message(FATAL_ERROR
        "Installed wxWinUI consumer failed: STA/cycles="
        "${wx_winui_test_result}, MTA-rejection=${wx_winui_mta_result}\n"
        "STA/cycles stdout:\n${wx_winui_test_output}\n"
        "STA/cycles stderr:\n${wx_winui_test_error}\n"
        "MTA-rejection stdout:\n${wx_winui_mta_output}\n"
        "MTA-rejection stderr:\n${wx_winui_mta_error}\n"
        "Runtime policy subprocesses:${wx_winui_policy_report}"
        "Runtime fault subprocesses:${wx_winui_fault_report}")
endif()

string(STRIP "${wx_winui_test_output}" wx_winui_test_output)
string(STRIP "${wx_winui_mta_output}" wx_winui_mta_output)
if(NOT wx_winui_test_output STREQUAL "" OR
        NOT wx_winui_mta_output STREQUAL "")
    message(STATUS
        "STA/cycles:\n${wx_winui_test_output}\n"
        "MTA rejection:\n${wx_winui_mta_output}\n"
        "Runtime policy subprocesses:${wx_winui_policy_report}"
        "Runtime fault subprocesses:${wx_winui_fault_report}")
endif()
