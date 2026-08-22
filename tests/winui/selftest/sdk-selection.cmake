# Self-test for wx_winui3_select_windows_sdk() in build/cmake/winui.cmake.
#
# Usage:
#   cmake -DWINUI_CMAKE=<repo>/build/cmake/winui.cmake
#         -DWORK_DIR=<scratch-dir> -P sdk-selection.cmake
#
# Builds fake Windows Kits trees and checks that the selection: compares
# versions numerically (10.0.9600.0 sorts lexically AFTER 10.0.26100.0, which
# is the historical trap), only considers versions where every artefact
# exists (including MakePri), and honours the "user provided their own
# cppwinrt.exe" mode.

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
    WORK_DIR "${WORK_DIR}" "${SAFE_ROOT}" "sdk-selection")

# Create one fake SDK version; each artefact is optional so incomplete
# installations can be simulated.
function(make_sdk_version root version with_winmd with_headers with_cppwinrt
         with_makepri)
    file(MAKE_DIRECTORY "${root}/UnionMetadata/${version}")
    if(with_winmd)
        file(WRITE "${root}/UnionMetadata/${version}/Windows.winmd" "fake")
    endif()
    if(with_headers)
        file(WRITE "${root}/Include/${version}/cppwinrt/winrt/base.h" "fake")
    endif()
    if(with_cppwinrt)
        file(WRITE "${root}/bin/${version}/x64/cppwinrt.exe" "fake")
    endif()
    if(with_makepri)
        file(WRITE "${root}/bin/${version}/x64/makepri.exe" "fake")
    endif()
endfunction()

# --- case A: three complete versions -> numerically highest wins ----------
set(rootA "${WORK_DIR}/kits-complete")
file(REMOVE_RECURSE "${rootA}")
make_sdk_version("${rootA}" 10.0.9600.0 TRUE TRUE TRUE TRUE)
make_sdk_version("${rootA}" 10.0.19041.0 TRUE TRUE TRUE TRUE)
make_sdk_version("${rootA}" 10.0.26100.0 TRUE TRUE TRUE TRUE)
wx_winui3_select_windows_sdk(pickA "${rootA}" TRUE TRUE)
if(NOT pickA STREQUAL "10.0.26100.0")
    message(FATAL_ERROR "case A: picked ${pickA} instead of 10.0.26100.0")
endif()
message(STATUS "case A OK: complete tree picks ${pickA}")

# --- case B: highest version incomplete -> next complete one wins ---------
set(rootB "${WORK_DIR}/kits-incomplete-top")
file(REMOVE_RECURSE "${rootB}")
make_sdk_version("${rootB}" 10.0.9600.0 TRUE TRUE TRUE TRUE)
make_sdk_version("${rootB}" 10.0.19041.0 TRUE TRUE TRUE TRUE)
make_sdk_version("${rootB}" 10.0.26100.0 FALSE TRUE TRUE TRUE) # Windows.winmd missing
wx_winui3_select_windows_sdk(pickB "${rootB}" TRUE TRUE)
if(NOT pickB STREQUAL "10.0.19041.0")
    message(FATAL_ERROR "case B: picked ${pickB} instead of 10.0.19041.0 "
                        "(10.0.26100.0 has no Windows.winmd and must be skipped)")
endif()
message(STATUS "case B OK: incomplete 26100 skipped, picked ${pickB}")

# --- case C: user cppwinrt override still requires MakePri ----------------
set(rootC "${WORK_DIR}/kits-no-bin")
file(REMOVE_RECURSE "${rootC}")
make_sdk_version("${rootC}" 10.0.26100.0 TRUE TRUE FALSE TRUE)
wx_winui3_select_windows_sdk(pickC "${rootC}" FALSE TRUE)
if(NOT pickC STREQUAL "10.0.26100.0")
    message(FATAL_ERROR "case C: picked ${pickC} instead of 10.0.26100.0")
endif()
message(STATUS "case C OK: cppwinrt override still accepts SDK MakePri")

# --- case D: MakePri is mandatory even with a cppwinrt override -----------
set(rootD "${WORK_DIR}/kits-no-makepri")
file(REMOVE_RECURSE "${rootD}")
make_sdk_version("${rootD}" 10.0.19041.0 TRUE TRUE FALSE TRUE)
make_sdk_version("${rootD}" 10.0.26100.0 TRUE TRUE FALSE FALSE)
wx_winui3_select_windows_sdk(pickD "${rootD}" FALSE TRUE)
if(NOT pickD STREQUAL "10.0.19041.0")
    message(FATAL_ERROR "case D: picked ${pickD} instead of 10.0.19041.0 "
                        "(10.0.26100.0 has no MakePri and must be skipped)")
endif()
message(STATUS "case D OK: SDK without MakePri skipped, picked ${pickD}")

# --- case E: application-owned PRI does not require MakePri ---------------
set(rootE "${WORK_DIR}/kits-pri-opt-out")
file(REMOVE_RECURSE "${rootE}")
make_sdk_version("${rootE}" 10.0.26100.0 TRUE TRUE TRUE FALSE)
wx_winui3_select_windows_sdk(pickE "${rootE}" TRUE FALSE)
if(NOT pickE STREQUAL "10.0.26100.0")
    message(FATAL_ERROR
        "case E: PRI opt-out should accept SDK without MakePri, got ${pickE}")
endif()
message(STATUS "case E OK: PRI opt-out does not require MakePri")

message(STATUS "PASS: wx_winui3_select_windows_sdk self-test")
