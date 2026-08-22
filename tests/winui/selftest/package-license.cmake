# Self-test for the Windows App SDK package-license guard.
#
# The positive case uses a minimal stable licence carrying the exact NuGet
# binplacement grant. The negative cases run in child CMake processes because
# the expected result is a configure-time FATAL_ERROR.

if(NOT DEFINED WINUI_CMAKE OR NOT DEFINED WORK_DIR OR
        NOT DEFINED SAFE_ROOT)
    message(FATAL_ERROR
        "pass -DWINUI_CMAKE=..., -DWORK_DIR=... and -DSAFE_ROOT=...")
endif()

set(WXWINUI 1)
set(wxUSE_WINUI3 1)
set(wxWINUI3_SELFTEST_FUNCTIONS_ONLY 1)
include("${WINUI_CMAKE}")
wx_winui3_validate_selftest_work_dir(
    WORK_DIR "${WORK_DIR}" "${SAFE_ROOT}" "package-license")

set(distribution_grant
    "Any files that are binplaced with your application by the WindowsAppSDK NuGet package are, by definition, permitted to be redistributed.")

if(DEFINED CASE)
    set(case_dir "${WORK_DIR}/${CASE}")
    if(CASE STREQUAL "preview")
        file(WRITE "${case_dir}/license.txt"
            "MICROSOFT WINDOWS APP SDK ENGINEERING PREVIEW\n"
            "You may not use the software in a live operating environment.\n")
    elseif(CASE STREQUAL "missing-grant")
        file(WRITE "${case_dir}/license.txt"
            "MICROSOFT SOFTWARE LICENSE TERMS\nMICROSOFT WINDOWS APP SDK\n")
    elseif(CASE STREQUAL "missing-file")
        file(MAKE_DIRECTORY "${case_dir}")
    else()
        message(FATAL_ERROR "unknown negative case ${CASE}")
    endif()

    wx_winui3_validate_redistributable_package("${case_dir}" "synthetic ${CASE}")
    message(STATUS "NEGATIVE-CASE-ACCEPTED")
    return()
endif()

file(REMOVE_RECURSE "${WORK_DIR}")
set(stable_dir "${WORK_DIR}/stable")
file(WRITE "${stable_dir}/license.txt"
    "MICROSOFT SOFTWARE LICENSE TERMS\nMICROSOFT WINDOWS APP SDK\n"
    "${distribution_grant}\n")
wx_winui3_validate_redistributable_package("${stable_dir}" "synthetic stable")
message(STATUS "stable package accepted")

foreach(negative_case preview missing-grant missing-file)
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DCASE=${negative_case}
            -DWINUI_CMAKE=${WINUI_CMAKE}
            -DWORK_DIR=${WORK_DIR}
            -DSAFE_ROOT=${SAFE_ROOT}
            -P "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE negative_result
        OUTPUT_VARIABLE negative_stdout
        ERROR_VARIABLE negative_stderr
        )
    if(negative_result EQUAL 0 OR
            negative_stdout MATCHES "NEGATIVE-CASE-ACCEPTED")
        message(FATAL_ERROR "${negative_case}: invalid licence was accepted")
    endif()
    if(negative_case STREQUAL "preview")
        set(expected_diagnostic "Engineering[ \r\n]+Preview")
    elseif(negative_case STREQUAL "missing-grant")
        set(expected_diagnostic "redistribution[ \r\n]+grant")
    else()
        set(expected_diagnostic "has no license.txt")
    endif()
    if(NOT negative_stderr MATCHES "${expected_diagnostic}")
        message(FATAL_ERROR
            "${negative_case}: unexpected diagnostic: ${negative_stderr}")
    endif()
endforeach()

message(STATUS "PASS: Windows App SDK package-license self-test")
