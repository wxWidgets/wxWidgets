# Prove that the WinUI toolkit cannot be configured with its implementation
# switch disabled. That combination selects WinUI public headers while keeping
# the ordinary wxMSW source list and is therefore not a supported fallback.

if(NOT DEFINED WINUI_CMAKE OR NOT DEFINED SOURCE_DIR OR
        NOT DEFINED WORK_DIR OR NOT DEFINED SAFE_ROOT)
    message(FATAL_ERROR
        "pass -DWINUI_CMAKE=..., -DSOURCE_DIR=..., -DWORK_DIR=... "
        "and -DSAFE_ROOT=...")
endif()

set(WXWINUI 1)
set(wxUSE_WINUI3 1)
set(wxWINUI3_SELFTEST_FUNCTIONS_ONLY 1)
include("${WINUI_CMAKE}")
wx_winui3_validate_selftest_work_dir(
    checked_work_dir "${WORK_DIR}" "${SAFE_ROOT}"
    "toolkit-requires-winui3")

file(REMOVE_RECURSE "${checked_work_dir}")
execute_process(
    COMMAND "${CMAKE_COMMAND}"
        -S "${SOURCE_DIR}"
        -B "${checked_work_dir}"
        -DwxBUILD_TOOLKIT=winui
        -DwxUSE_WINUI3=OFF
        -DwxBUILD_SAMPLES=OFF
        -DwxBUILD_TESTS=OFF
    RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr
    TIMEOUT 60
    ENCODING UTF-8
)

set(output "${stdout}\n${stderr}")
if("${result}" STREQUAL "0")
    message(FATAL_ERROR
        "invalid wxBUILD_TOOLKIT=winui/wxUSE_WINUI3=OFF configuration "
        "unexpectedly succeeded\n${output}")
endif()
if(NOT output MATCHES
        "wxBUILD_TOOLKIT=winui requires wxUSE_WINUI3=ON")
    message(FATAL_ERROR
        "configuration failed for an unexpected reason (exit ${result})\n"
        "${output}")
endif()

file(REMOVE_RECURSE "${checked_work_dir}")
message(STATUS "WinUI toolkit correctly rejected wxUSE_WINUI3=OFF")
