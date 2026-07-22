# Self-test for wx_winui3_select_windows_sdk() in build/cmake/winui.cmake.
#
# Usage:
#   cmake -DWINUI_CMAKE=<repo>/build/cmake/winui.cmake
#         -DWORK_DIR=<scratch-dir> -P sdk-selection.cmake
#
# Builds fake Windows Kits trees and checks that the selection: compares
# versions numerically (10.0.9600.0 sorts lexically AFTER 10.0.26100.0, which
# is the historical trap), only considers versions where every artefact
# exists, and honours the "user provided their own cppwinrt.exe" mode.

if(NOT DEFINED WINUI_CMAKE OR NOT DEFINED WORK_DIR)
    message(FATAL_ERROR "pass -DWINUI_CMAKE=... and -DWORK_DIR=...")
endif()

set(WXWINUI 1)
set(wxUSE_WINUI3 1)
set(wxWINUI3_SELFTEST_FUNCTIONS_ONLY 1)
include("${WINUI_CMAKE}")

# Create one fake SDK version; each artefact is optional so incomplete
# installations can be simulated.
function(make_sdk_version root version with_winmd with_headers with_bin)
    file(MAKE_DIRECTORY "${root}/UnionMetadata/${version}")
    if(with_winmd)
        file(WRITE "${root}/UnionMetadata/${version}/Windows.winmd" "fake")
    endif()
    if(with_headers)
        file(WRITE "${root}/Include/${version}/cppwinrt/winrt/base.h" "fake")
    endif()
    if(with_bin)
        file(WRITE "${root}/bin/${version}/x64/cppwinrt.exe" "fake")
    endif()
endfunction()

# --- case A: three complete versions -> numerically highest wins ----------
set(rootA "${WORK_DIR}/kits-complete")
file(REMOVE_RECURSE "${rootA}")
make_sdk_version("${rootA}" 10.0.9600.0 TRUE TRUE TRUE)
make_sdk_version("${rootA}" 10.0.19041.0 TRUE TRUE TRUE)
make_sdk_version("${rootA}" 10.0.26100.0 TRUE TRUE TRUE)
wx_winui3_select_windows_sdk(pickA "${rootA}" TRUE)
if(NOT pickA STREQUAL "10.0.26100.0")
    message(FATAL_ERROR "case A: picked ${pickA} instead of 10.0.26100.0")
endif()
message(STATUS "case A OK: complete tree picks ${pickA}")

# --- case B: highest version incomplete -> next complete one wins ---------
set(rootB "${WORK_DIR}/kits-incomplete-top")
file(REMOVE_RECURSE "${rootB}")
make_sdk_version("${rootB}" 10.0.9600.0 TRUE TRUE TRUE)
make_sdk_version("${rootB}" 10.0.19041.0 TRUE TRUE TRUE)
make_sdk_version("${rootB}" 10.0.26100.0 FALSE TRUE TRUE)   # Windows.winmd missing
wx_winui3_select_windows_sdk(pickB "${rootB}" TRUE)
if(NOT pickB STREQUAL "10.0.19041.0")
    message(FATAL_ERROR "case B: picked ${pickB} instead of 10.0.19041.0 "
                        "(10.0.26100.0 has no Windows.winmd and must be skipped)")
endif()
message(STATUS "case B OK: incomplete 26100 skipped, picked ${pickB}")

# --- case C: user-provided cppwinrt.exe -> bin/ not required --------------
set(rootC "${WORK_DIR}/kits-no-bin")
file(REMOVE_RECURSE "${rootC}")
make_sdk_version("${rootC}" 10.0.26100.0 TRUE TRUE FALSE)   # no cppwinrt.exe
wx_winui3_select_windows_sdk(pickC "${rootC}" FALSE)
if(NOT pickC STREQUAL "10.0.26100.0")
    message(FATAL_ERROR "case C: picked ${pickC} instead of 10.0.26100.0")
endif()
message(STATUS "case C OK: without the bin requirement picked ${pickC}")

message(STATUS "PASS: wx_winui3_select_windows_sdk self-test")
