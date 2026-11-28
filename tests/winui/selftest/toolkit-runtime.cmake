# Script-only tests of the opt-in Toolkit runtime boundary. No configure,
# compiler, MakePri, C++/WinRT generator, download or executable is invoked.
# Actual target wiring/deployment remains the integrating build's smoke test.

cmake_minimum_required(VERSION 3.21)
if(NOT DEFINED WINUI_CMAKE OR NOT DEFINED TOOLKIT_RUNTIME_CMAKE OR
        NOT DEFINED WORK_DIR OR NOT DEFINED SAFE_ROOT)
    message(FATAL_ERROR
        "pass WINUI_CMAKE, TOOLKIT_RUNTIME_CMAKE, WORK_DIR and SAFE_ROOT")
endif()
set(WXWINUI 1)
set(wxUSE_WINUI3 1)
set(wxWINUI3_SELFTEST_FUNCTIONS_ONLY 1)
include("${WINUI_CMAKE}")
wx_winui3_validate_selftest_work_dir(
    WORK_DIR "${WORK_DIR}" "${SAFE_ROOT}" "toolkit-runtime")
set(wxBINARY_DIR "${WORK_DIR}/not-a-build")
set(wxWINUI3_PACKAGE_ROOT "${WORK_DIR}/packages")
set(wxWINUI3_CPPWINRT_INPUTS "unchanged-default-inputs")
set(wxWINUI3_RUNTIME_PAYLOAD_SRC "unchanged-default-payload")
set(wxWINUI3_DEPLOY_SCRIPT "unchanged-default-script")
include("${TOOLKIT_RUNTIME_CMAKE}")
include("${TOOLKIT_RUNTIME_CMAKE}")
if(NOT wxWINUI3_CPPWINRT_INPUTS STREQUAL "unchanged-default-inputs" OR
        NOT wxWINUI3_RUNTIME_PAYLOAD_SRC STREQUAL "unchanged-default-payload" OR
        NOT wxWINUI3_DEPLOY_SCRIPT STREQUAL "unchanged-default-script" OR
        EXISTS "${wxBINARY_DIR}")
    message(FATAL_ERROR "including the runtime helper changed the default build")
endif()

# Supply only the capability checks needed to reach preparation guards.
# Reaching this stub would be an unintended MakePri/configuration side effect.
function(wx_winui3_prepare_runtime_pri out_var)
    message(FATAL_ERROR "UNEXPECTED-PRI-PREPARATION")
endfunction()
set(WIN32 TRUE)
set(MSVC TRUE)
set(wxWINUI3_ARCH x64)
set(wxWINUI3_DEPLOY_RESOURCES_PRI ON)
set(wxWINUI3_DEPLOY_SCRIPT_CONTENT "nonempty-existing-runtime-driver")
set(toolkit_nuspec
    "<license type=\"expression\">MIT</license><requireLicenseAcceptance>false</requireLicenseAcceptance>")
set(win2d_nuspec
    "<requireLicenseAcceptance>true</requireLicenseAcceptance><licenseUrl>http://www.microsoft.com/web/webpi/eula/eula_win2d_10012014.htm</licenseUrl>")

if(DEFINED CASE)
    if(CASE STREQUAL "unknown-argument")
        wx_winui3_prepare_toolkit_runtime(ALLOW_ANYTHING)
    elseif(CASE STREQUAL "missing-option-value")
        wx_winui3_prepare_toolkit_runtime(TOOLKIT_LICENSE_FILE)
    elseif(CASE STREQUAL "missing-experimental-opt-in")
        wx_winui3_prepare_toolkit_runtime()
    elseif(CASE STREQUAL "unsupported-rid")
        set(wxWINUI3_ARCH arm64ec)
        wx_winui3_prepare_toolkit_runtime(ACCEPT_EXPERIMENTAL_PACKAGES)
    elseif(CASE STREQUAL "disabled-winui")
        set(wxUSE_WINUI3 OFF)
        wx_winui3_prepare_toolkit_runtime(ACCEPT_EXPERIMENTAL_PACKAGES)
    elseif(CASE STREQUAL "disabled-pri")
        set(wxWINUI3_DEPLOY_RESOURCES_PRI OFF)
        wx_winui3_prepare_toolkit_runtime(ACCEPT_EXPERIMENTAL_PACKAGES)
    elseif(CASE STREQUAL "toolkit-license-changed")
        string(REPLACE "MIT" "GPL-3.0-only" toolkit_nuspec "${toolkit_nuspec}")
        wx_winui3_toolkit_check_nuspec_license(
            "XamlToolkit.Labs.WinUI.Native" "${toolkit_nuspec}" TRUE)
    elseif(CASE STREQUAL "win2d-license-not-accepted")
        wx_winui3_toolkit_check_nuspec_license(
            "Microsoft.Graphics.Win2D" "${win2d_nuspec}" FALSE)
    elseif(CASE STREQUAL "win2d-license-changed")
        string(REPLACE "10012014" "00000000" win2d_nuspec "${win2d_nuspec}")
        wx_winui3_toolkit_check_nuspec_license(
            "Microsoft.Graphics.Win2D" "${win2d_nuspec}" TRUE)
    elseif(CASE STREQUAL "unknown-package-license")
        wx_winui3_toolkit_check_nuspec_license(
            "Unreviewed.Native" "${toolkit_nuspec}" TRUE)
    elseif(CASE MATCHES "^cache-")
        if(CASE STREQUAL "cache-missing")
            set(TEST_VERSION "9.0.0")
        else()
            set(TEST_VERSION "1.0.0")
        endif()
        wx_winui3_toolkit_cached_package(result
            "Wx.SelfTest.Toolkit" "${TEST_VERSION}" "${EXPECTED_SHA}")
    elseif(CASE STREQUAL "notice-hash-mismatch")
        wx_winui3_toolkit_check_license(result "${WORK_DIR}/notice.txt"
            "0000000000000000000000000000000000000000000000000000000000000000")
    elseif(CASE STREQUAL "notice-incomplete-option")
        wx_winui3_prepare_toolkit_runtime(ACCEPT_EXPERIMENTAL_PACKAGES
            TOOLKIT_LICENSE_FILE "${WORK_DIR}/notice.txt")
    elseif(CASE STREQUAL "unsafe-path")
        wx_winui3_toolkit_checked_path(result "${WORK_DIR}/bad;driver.cmake")
    elseif(CASE STREQUAL "conflicting-preparation")
        set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PREPARED TRUE)
        set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_KEY "a-different-preparation")
        wx_winui3_prepare_toolkit_runtime(ACCEPT_EXPERIMENTAL_PACKAGES
            ACCEPT_WIN2D_PACKAGE_LICENSE)
    elseif(CASE STREQUAL "missing-compile-target")
        wx_winui3_configure_toolkit_target(nonexistent)
    elseif(CASE STREQUAL "missing-deploy-target")
        wx_winui3_deploy_toolkit_runtime(nonexistent)
    else()
        message(FATAL_ERROR "unknown negative case ${CASE}")
    endif()
    message(FATAL_ERROR "NEGATIVE-CASE-ACCEPTED")
endif()

function(expect_failure case_name expected_diagnostic)
    execute_process(COMMAND "${CMAKE_COMMAND}"
        "-DCASE=${case_name}"
        "-DWINUI_CMAKE=${WINUI_CMAKE}"
        "-DTOOLKIT_RUNTIME_CMAKE=${TOOLKIT_RUNTIME_CMAKE}"
        "-DWORK_DIR=${WORK_DIR}" "-DSAFE_ROOT=${SAFE_ROOT}"
        "-DEXPECTED_SHA=${fixture_hash}"
        -P "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE result OUTPUT_VARIABLE stdout ERROR_VARIABLE stderr
        TIMEOUT 30)
    set(output "${stdout}\n${stderr}")
    if(result EQUAL 0 OR output MATCHES "NEGATIVE-CASE-ACCEPTED|UNEXPECTED-PRI-PREPARATION" OR
            NOT output MATCHES "${expected_diagnostic}")
        message(FATAL_ERROR "${case_name}: unexpected result ${result}\n${output}")
    endif()
    message(STATUS "case ${case_name} OK")
endfunction()

# The only recursive delete is this dedicated, validated self-test directory.
file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${wxWINUI3_PACKAGE_ROOT}")
file(WRITE "${WORK_DIR}/notice.txt" "Synthetic test notice, not a dependency license.\n")
file(SHA256 "${WORK_DIR}/notice.txt" notice_hash)
wx_winui3_toolkit_check_license(notice "${WORK_DIR}/notice.txt" "${notice_hash}")
if(NOT notice STREQUAL "${WORK_DIR}/notice.txt")
    message(FATAL_ERROR "optional notice validation returned the wrong path")
endif()
wx_winui3_toolkit_check_nuspec_license(
    "XamlToolkit.Labs.WinUI.Native" "${toolkit_nuspec}" FALSE)
wx_winui3_toolkit_check_nuspec_license(
    "XamlToolkit.WinUI.Controls.Native" "${toolkit_nuspec}" FALSE)
wx_winui3_toolkit_check_nuspec_license(
    "Microsoft.Graphics.Win2D" "${win2d_nuspec}" TRUE)
message(STATUS "case factual-license-policies OK")

# No archive extraction is performed by the runtime helper. Tiny synthetic
# bytes suffice to exercise its existing extraction-manifest/archive checks.
set(fixture_archive "Synthetic cache archive, never extracted.")
string(SHA256 fixture_hash "${fixture_archive}")
string(SUBSTRING "${fixture_hash}" 0 32 fixture_key)
set(fixture_dir "${wxWINUI3_PACKAGE_ROOT}/Wx.SelfTest.Toolkit.1.0.0-${fixture_key}")
file(WRITE "${fixture_dir}/marker.txt" "original extracted bytes\n")
file(WRITE "${fixture_dir}.nupkg" "${fixture_archive}")
wx_winui3_write_nupkg_integrity("${fixture_dir}" "${fixture_hash}")
file(SHA256 "${fixture_dir}/${wxWINUI3_NUPKG_MANIFEST_NAME}" manifest_before)
wx_winui3_toolkit_cached_package(cached "Wx.SelfTest.Toolkit" "1.0.0" "${fixture_hash}")
wx_winui3_toolkit_cached_package(cached_again "Wx.SelfTest.Toolkit" "1.0.0" "${fixture_hash}")
file(SHA256 "${fixture_dir}/${wxWINUI3_NUPKG_MANIFEST_NAME}" manifest_after)
if(NOT cached STREQUAL fixture_dir OR NOT cached_again STREQUAL fixture_dir OR
        NOT manifest_before STREQUAL manifest_after)
    message(FATAL_ERROR "cache-only resolution mutated or selected the wrong extraction")
endif()
message(STATUS "case cache-resolution-idempotent OK")

expect_failure(unknown-argument "invalid preparation arguments")
expect_failure(missing-option-value "invalid preparation arguments")
expect_failure(missing-experimental-opt-in "experimental-package acceptance")
expect_failure(unsupported-rid "initialize the MSVC WinUI runtime")
expect_failure(disabled-winui "initialize the MSVC WinUI runtime")
expect_failure(disabled-pri "initialize the MSVC WinUI runtime")
expect_failure(toolkit-license-changed "unexpected license declaration")
expect_failure(win2d-license-not-accepted "ACCEPT_WIN2D_PACKAGE_LICENSE")
expect_failure(win2d-license-changed "unexpected Win2D package licensing metadata")
expect_failure(unknown-package-license "unrecognized package licensing policy")
expect_failure(cache-missing "exact verified package is absent")
file(WRITE "${fixture_dir}/marker.txt" "corrupt extracted bytes\n")
expect_failure(cache-extraction-corrupt "package extraction integrity failed")
file(READ "${fixture_dir}/marker.txt" still_corrupt)
if(NOT still_corrupt STREQUAL "corrupt extracted bytes\n")
    message(FATAL_ERROR "runtime cache validation unexpectedly repaired user data")
endif()
file(WRITE "${fixture_dir}/marker.txt" "original extracted bytes\n")
file(WRITE "${fixture_dir}.nupkg" "corrupt archive\n")
expect_failure(cache-archive-corrupt "archive hash mismatch")
expect_failure(notice-hash-mismatch "license/notice hash mismatch")
expect_failure(notice-incomplete-option "missing license file or SHA256")
expect_failure(unsafe-path "unsafe generated-script path")
expect_failure(conflicting-preparation "conflicting runtime preparations")
expect_failure(missing-compile-target "configure_target requires an existing target")
expect_failure(missing-deploy-target "deploy_runtime requires an executable target")

# A second use of an already-prepared context must not re-enter preparation,
# regenerate a PRI or alter the ordinary payload. This does not claim to test
# first-time target creation, which requires the real integrating CMake build.
set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PREPARED TRUE)
set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PAYLOAD_SRC "a;b")
set_property(GLOBAL PROPERTY wxWINUI3_TOOLKIT_RUNTIME_PAYLOAD_DST "c;d")
wx_winui3_prepare_toolkit_runtime()
wx_winui3_prepare_toolkit_runtime()
wx_winui3_get_toolkit_runtime_payload(sources destinations)
if(NOT sources STREQUAL "a;b" OR NOT destinations STREQUAL "c;d" OR
        NOT wxWINUI3_RUNTIME_PAYLOAD_SRC STREQUAL "unchanged-default-payload" OR
        EXISTS "${wxBINARY_DIR}")
    message(FATAL_ERROR "reusing a prepared context changed or regenerated the runtime")
endif()
message(STATUS "case prepared-context-idempotent OK")
message(STATUS "PASS: Toolkit runtime script guards, cache and idempotence (no build/download)")
