# Self-test for the Windows App SDK cross-package compatibility guard.

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
    WORK_DIR "${WORK_DIR}" "${SAFE_ROOT}" "package-set")

set(wxWINUI3_WINDOWSAPPSDK_VERSION "1.8.100")
set(wxWINUI3_WINUI_VERSION "1.8.104")
set(wxWINUI3_FOUNDATION_VERSION "1.8.103")
set(wxWINUI3_INTERACTIVEEXPERIENCES_VERSION "1.8.102")
set(wxWINUI3_BASE_VERSION "1.8.101")
set(wxWINUI3_WEBVIEW2_VERSION "1.0.4000.0")

set(wxWINUI3_RUNTIME_PACKAGE_DIR "${WORK_DIR}/Runtime")
set(wxWINUI3_WINUI_PACKAGE_DIR "${WORK_DIR}/WinUI")
set(wxWINUI3_FOUNDATION_PACKAGE_DIR "${WORK_DIR}/Foundation")
set(wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR
    "${WORK_DIR}/Interactive")
set(wxWINUI3_BASE_PACKAGE_DIR "${WORK_DIR}/Base")
set(wxWINUI3_WEBVIEW2_PACKAGE_DIR "${WORK_DIR}/WebView2")

function(write_nuspec directory package_name package_version dependencies)
    set(dependency_xml "${dependencies}")
    foreach(extra_dependency IN LISTS ARGN)
        string(APPEND dependency_xml "${extra_dependency}")
    endforeach()
    file(MAKE_DIRECTORY "${directory}")
    file(WRITE "${directory}/${package_name}.nuspec"
        "<?xml version=\"1.0\"?>\n"
        "<package><metadata><id>${package_name}</id>"
        "<version>${package_version}</version>"
        "<dependencies>${dependency_xml}</dependencies>"
        "</metadata></package>\n")
endfunction()

function(write_runtime_compatibility_target foundation_version)
    file(MAKE_DIRECTORY "${wxWINUI3_RUNTIME_PACKAGE_DIR}/build")
    file(WRITE
        "${wxWINUI3_RUNTIME_PACKAGE_DIR}/build/Microsoft.WindowsAppSDK.ComponentReference.targets"
        "<Project>\n"
        "<!-- This is populated during the generation of the nuget. -->\n"
        "<_VersionMismatch Include=\"Microsoft.WindowsAppSDK.Foundation\">\n"
        "<ExpectedVersion>${foundation_version}</ExpectedVersion>\n"
        "</_VersionMismatch>\n"
        "<_VersionMismatch Include=\"Microsoft.WindowsAppSDK.InteractiveExperiences\">\n"
        "<ExpectedVersion>${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}</ExpectedVersion>\n"
        "</_VersionMismatch>\n"
        "<_VersionMismatch Include=\"Microsoft.WindowsAppSDK.WinUI\">\n"
        "<ExpectedVersion>${wxWINUI3_WINUI_VERSION}</ExpectedVersion>\n"
        "</_VersionMismatch>\n"
        "</Project>\n")
endfunction()

if(NOT DEFINED CASE)
    file(REMOVE_RECURSE "${WORK_DIR}")

    write_nuspec("${wxWINUI3_RUNTIME_PACKAGE_DIR}"
        "Microsoft.WindowsAppSDK.Runtime"
        "${wxWINUI3_WINDOWSAPPSDK_VERSION}"
        "<dependency id=\"Microsoft.WindowsAppSDK.Base\" version=\"${wxWINUI3_BASE_VERSION}\"/>")
    write_nuspec("${wxWINUI3_WINUI_PACKAGE_DIR}"
        "Microsoft.WindowsAppSDK.WinUI"
        "${wxWINUI3_WINUI_VERSION}"
        "<dependency id=\"Microsoft.WindowsAppSDK.Base\" version=\"${wxWINUI3_BASE_VERSION}\"/>"
        "<dependency id=\"Microsoft.WindowsAppSDK.Foundation\" version=\"${wxWINUI3_FOUNDATION_VERSION}\"/>"
        "<dependency id=\"Microsoft.WindowsAppSDK.InteractiveExperiences\" version=\"${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}\"/>"
        "<dependency id=\"Microsoft.Web.WebView2\" version=\"1.0.3000.0\"/>")
    write_nuspec("${wxWINUI3_FOUNDATION_PACKAGE_DIR}"
        "Microsoft.WindowsAppSDK.Foundation"
        "${wxWINUI3_FOUNDATION_VERSION}"
        "<dependency id=\"Microsoft.WindowsAppSDK.Base\" version=\"${wxWINUI3_BASE_VERSION}\"/>"
        "<dependency id=\"Microsoft.WindowsAppSDK.InteractiveExperiences\" version=\"${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}\"/>")
    write_nuspec("${wxWINUI3_INTERACTIVEEXPERIENCES_PACKAGE_DIR}"
        "Microsoft.WindowsAppSDK.InteractiveExperiences"
        "${wxWINUI3_INTERACTIVEEXPERIENCES_VERSION}"
        "<dependency id=\"Microsoft.WindowsAppSDK.Base\" version=\"${wxWINUI3_BASE_VERSION}\"/>")
    write_nuspec("${wxWINUI3_BASE_PACKAGE_DIR}"
        "Microsoft.WindowsAppSDK.Base" "${wxWINUI3_BASE_VERSION}" "")
    write_nuspec("${wxWINUI3_WEBVIEW2_PACKAGE_DIR}"
        "Microsoft.Web.WebView2" "${wxWINUI3_WEBVIEW2_VERSION}" "")
    write_runtime_compatibility_target("${wxWINUI3_FOUNDATION_VERSION}")

    wx_winui3_validate_package_set()
    message(STATUS "case compatible OK")

    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DCASE=runtime-mismatch
            -DWINUI_CMAKE=${WINUI_CMAKE}
            -DSAFE_ROOT=${SAFE_ROOT}
            -DWORK_DIR=${WORK_DIR}
            -P "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE mismatch_result
        OUTPUT_VARIABLE mismatch_stdout
        ERROR_VARIABLE mismatch_stderr)
    if(mismatch_result EQUAL 0 OR
            mismatch_stdout MATCHES "MISMATCH-ACCEPTED")
        message(FATAL_ERROR
            "case runtime-mismatch: incompatible cohort was accepted")
    endif()
    if(NOT mismatch_stderr MATCHES
            "requires[ \r\n]+Microsoft.WindowsAppSDK.Foundation")
        message(FATAL_ERROR
            "case runtime-mismatch: unexpected diagnostic: "
            "${mismatch_stderr}")
    endif()
    message(STATUS "case runtime-mismatch OK")
    write_runtime_compatibility_target("${wxWINUI3_FOUNDATION_VERSION}")

    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DCASE=malformed-runtime-entry
            -DWINUI_CMAKE=${WINUI_CMAKE}
            -DSAFE_ROOT=${SAFE_ROOT}
            -DWORK_DIR=${WORK_DIR}
            -P "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE malformed_result
        OUTPUT_VARIABLE malformed_stdout
        ERROR_VARIABLE malformed_stderr)
    if(malformed_result EQUAL 0 OR
            malformed_stdout MATCHES "MALFORMED-ACCEPTED")
        message(FATAL_ERROR
            "case malformed-runtime-entry: next entry's version was accepted")
    endif()
    if(NOT malformed_stderr MATCHES
            "Foundation[ \r\n]+has no readable ExpectedVersion")
        message(FATAL_ERROR
            "case malformed-runtime-entry: unexpected diagnostic: "
            "${malformed_stderr}")
    endif()
    message(STATUS "case malformed-runtime-entry OK")
    write_runtime_compatibility_target("${wxWINUI3_FOUNDATION_VERSION}")

    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -DCASE=nuspec-id-mismatch
            -DWINUI_CMAKE=${WINUI_CMAKE}
            -DSAFE_ROOT=${SAFE_ROOT}
            -DWORK_DIR=${WORK_DIR}
            -P "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE identity_result
        OUTPUT_VARIABLE identity_stdout
        ERROR_VARIABLE identity_stderr)
    if(identity_result EQUAL 0 OR
            identity_stdout MATCHES "IDENTITY-ACCEPTED")
        message(FATAL_ERROR
            "case nuspec-id-mismatch: wrong package id was accepted")
    endif()
    if(NOT identity_stderr MATCHES "package identity mismatch")
        message(FATAL_ERROR
            "case nuspec-id-mismatch: unexpected diagnostic: "
            "${identity_stderr}")
    endif()
    message(STATUS "case nuspec-id-mismatch OK")
    message(STATUS "PASS: Windows App SDK package-set self-test")
    return()
endif()

if(CASE STREQUAL "runtime-mismatch")
    write_runtime_compatibility_target("1.8.999")
    wx_winui3_validate_package_set()
    message(STATUS "MISMATCH-ACCEPTED")
    return()
endif()

if(CASE STREQUAL "malformed-runtime-entry")
    set(target
        "${wxWINUI3_RUNTIME_PACKAGE_DIR}/build/Microsoft.WindowsAppSDK.ComponentReference.targets")
    file(READ "${target}" target_text)
    string(REPLACE
        "<ExpectedVersion>${wxWINUI3_FOUNDATION_VERSION}</ExpectedVersion>"
        "<MalformedVersion>${wxWINUI3_FOUNDATION_VERSION}</MalformedVersion>"
        target_text "${target_text}")
    file(WRITE "${target}" "${target_text}")
    wx_winui3_validate_package_set()
    message(STATUS "MALFORMED-ACCEPTED")
    return()
endif()

if(CASE STREQUAL "nuspec-id-mismatch")
    set(nuspec
        "${wxWINUI3_BASE_PACKAGE_DIR}/Microsoft.WindowsAppSDK.Base.nuspec")
    file(READ "${nuspec}" nuspec_text)
    string(REPLACE
        "<id>Microsoft.WindowsAppSDK.Base</id>"
        "<id>Microsoft.WindowsAppSDK.Wrong</id>"
        nuspec_text "${nuspec_text}")
    file(WRITE "${nuspec}" "${nuspec_text}")
    wx_winui3_validate_package_set()
    message(STATUS "IDENTITY-ACCEPTED")
    return()
endif()

message(FATAL_ERROR "unknown CASE ${CASE}")
