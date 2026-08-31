# Self-test for the transactional, content-addressed C++/WinRT projection
# worker. No SDK or network access is needed.

foreach(required_var IN ITEMS
        WINUI_CMAKE CPPWINRT_WORKER FAKE_GENERATOR WORK_DIR SAFE_ROOT)
    if(NOT DEFINED ${required_var} OR "${${required_var}}" STREQUAL "")
        message(FATAL_ERROR "${required_var} is required")
    endif()
endforeach()

set(WXWINUI 1)
set(wxUSE_WINUI3 1)
set(wxWINUI3_SELFTEST_FUNCTIONS_ONLY 1)
include("${WINUI_CMAKE}")
wx_winui3_validate_selftest_work_dir(
    safe_work_dir "${WORK_DIR}" "${SAFE_ROOT}" "cppwinrt-cache")

file(REMOVE_RECURSE "${safe_work_dir}")
file(MAKE_DIRECTORY
    "${safe_work_dir}/pin-a"
    "${safe_work_dir}/pin-b"
    "${safe_work_dir}/cache")

set(windows_winmd "${safe_work_dir}/Windows.winmd")
set(input_a "${safe_work_dir}/pin-a/Component.winmd")
set(input_b "${safe_work_dir}/pin-b/Component.winmd")
file(WRITE "${windows_winmd}" "synthetic Windows metadata")
file(WRITE "${input_a}" "pin-A")
file(WRITE "${input_b}" "pin-B")
set(tool "${CMAKE_COMMAND}")

include("${CPPWINRT_WORKER}")
wx_winui3_compute_cppwinrt_fingerprint(fingerprint_a
    INPUTS "${input_a}"
    WINDOWS_WINMD "${windows_winmd}"
    TOOL "${tool}"
    AUXILIARY "${FAKE_GENERATOR}")
wx_winui3_compute_cppwinrt_fingerprint(fingerprint_b
    INPUTS "${input_b}"
    WINDOWS_WINMD "${windows_winmd}"
    TOOL "${tool}"
    AUXILIARY "${FAKE_GENERATOR}")
if(fingerprint_a STREQUAL fingerprint_b)
    message(FATAL_ERROR "synthetic A and B fingerprints are identical")
endif()

set(input_list_a "${safe_work_dir}/inputs-a.txt")
set(input_list_b "${safe_work_dir}/inputs-b.txt")
file(WRITE "${input_list_a}" "${input_a}\n")
file(WRITE "${input_list_b}" "${input_b}\n")

function(run_worker fingerprint input_list)
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            "-DWX_CPPWINRT_OUTPUT_ROOT=${safe_work_dir}/cache"
            "-DWX_CPPWINRT_FINGERPRINT=${fingerprint}"
            "-DWX_CPPWINRT_INPUT_LIST=${input_list}"
            "-DWX_CPPWINRT_WINDOWS_WINMD=${windows_winmd}"
            "-DWX_CPPWINRT_TOOL=${tool}"
            "-DWX_CPPWINRT_REQUIRED_RELATIVE=winrt/Microsoft.UI.Xaml.h"
            "-DWX_CPPWINRT_TEST_GENERATOR=${FAKE_GENERATOR}"
            -P "${CPPWINRT_WORKER}"
        RESULT_VARIABLE worker_result
        OUTPUT_VARIABLE worker_stdout
        ERROR_VARIABLE worker_stderr)
    if(NOT worker_result EQUAL 0)
        message(FATAL_ERROR
            "projection worker failed (${worker_result})\n"
            "${worker_stdout}\n${worker_stderr}")
    endif()
endfunction()

# A then B must create two immutable directories.
run_worker("${fingerprint_a}" "${input_list_a}")
set(dir_a "${safe_work_dir}/cache/${fingerprint_a}")
file(READ "${dir_a}/winrt/Microsoft.UI.Xaml.h" a_xaml)
if(NOT a_xaml MATCHES "pin-A")
    message(FATAL_ERROR "projection A has the wrong content")
endif()

run_worker("${fingerprint_b}" "${input_list_b}")
set(dir_b "${safe_work_dir}/cache/${fingerprint_b}")
file(READ "${dir_b}/winrt/Microsoft.UI.Xaml.h" b_xaml)
file(READ "${dir_a}/winrt/Microsoft.UI.Xaml.h" a_after_b)
if(NOT b_xaml MATCHES "pin-B" OR NOT a_after_b STREQUAL a_xaml)
    message(FATAL_ERROR
        "projection B replaced or contaminated the existing A directory")
endif()

# Returning to A must resolve A, not bless B via an old stamp.
run_worker("${fingerprint_a}" "${input_list_a}")
file(READ "${dir_a}/winrt/Microsoft.UI.Xaml.h" a_returned)
if(NOT a_returned STREQUAL a_xaml)
    message(FATAL_ERROR "A -> B -> A did not preserve projection A")
endif()

# A secondary (non-sentinel) header is covered by the manifest. Corrupting it
# must trigger a complete regeneration while leaving B untouched.
file(WRITE "${dir_a}/winrt/Microsoft.UI.Secondary.h" "CORRUPTED")
file(READ "${dir_b}/winrt/Microsoft.UI.Xaml.h" b_before_repair)
run_worker("${fingerprint_a}" "${input_list_a}")
file(READ "${dir_a}/winrt/Microsoft.UI.Secondary.h" a_secondary)
file(READ "${dir_b}/winrt/Microsoft.UI.Xaml.h" b_after_repair)
if(NOT a_secondary MATCHES "pin-A" OR
        NOT b_after_repair STREQUAL b_before_repair)
    message(FATAL_ERROR
        "manifest repair failed or mutated the other content address")
endif()

message(STATUS
    "cppwinrt cache OK: A/B coexist, A -> B -> A is stable, and all-header "
    "manifest repair works")
