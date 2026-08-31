# Self-test for wx_winui3_find_nuget_package() in build/cmake/winui.cmake.
#
# Usage:
#   cmake -DWINUI_CMAKE=<repo>/build/cmake/winui.cmake
#         -DWORK_DIR=<scratch-dir> -P nupkg-cache.cmake
#
# Uses tiny synthetic archives (no network) to check that: an intact cached
# archive is validated, extracted atomically and stamped with its hash; a
# corrupted cached archive fails the configure BEFORE any extraction; a
# post-extraction corruption is detected and repaired from the pinned archive;
# same-version repins coexist; and a changed pinned version never reuses the
# old version's directory.
#
# The corrupted-archive case runs in a sub-process (this same file with
# -DCASE=corrupt) because the expected outcome is a FATAL_ERROR.

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
    WORK_DIR "${WORK_DIR}" "${SAFE_ROOT}" "nupkg-cache")

set(TEST_PACKAGE "Wx.SelfTest.Fake")
set(TEST_MARKER "marker/probe.txt")

# Build a minimal zip-format nupkg whose marker file holds ${content}.
function(make_nupkg out_path staging_dir content)
    file(REMOVE_RECURSE "${staging_dir}")
    file(WRITE "${staging_dir}/marker/probe.txt" "${content}")
    file(REMOVE "${out_path}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E tar cf "${out_path}" --format=zip -- marker
        WORKING_DIRECTORY "${staging_dir}"
        RESULT_VARIABLE tar_result
        )
    if(NOT tar_result EQUAL 0 OR NOT EXISTS "${out_path}")
        message(FATAL_ERROR "could not build the synthetic archive ${out_path}")
    endif()
endfunction()

function(make_managed_nupkg out_path out_sha package_root version
         staging_dir content)
    set(temp_archive
        "${package_root}/${TEST_PACKAGE}.${version}.building.nupkg")
    make_nupkg("${temp_archive}" "${staging_dir}" "${content}")
    file(SHA256 "${temp_archive}" archive_sha)
    string(SUBSTRING "${archive_sha}" 0 32 archive_key)
    set(final_archive
        "${package_root}/${TEST_PACKAGE}.${version}-${archive_key}.nupkg")
    file(RENAME "${temp_archive}" "${final_archive}")
    set(${out_path} "${final_archive}" PARENT_SCOPE)
    set(${out_sha} "${archive_sha}" PARENT_SCOPE)
endfunction()

# ---- inner mode: expect the corrupted cached archive to be rejected ------
if(DEFINED CASE AND CASE STREQUAL "corrupt")
    # WORK_DIR was prepared by the driver: the cached nupkg is corrupted and
    # EXPECTED_SHA is the hash of the archive as it should have been.
    set(wxWINUI3_PACKAGE_ROOT "${WORK_DIR}/packages")
    set(wxSOURCE_DIR "${WORK_DIR}/no-such-source")
    wx_winui3_find_nuget_package(OUT_DIR
        "${TEST_PACKAGE}" "1.0.0" "${TEST_MARKER}" "${EXPECTED_SHA}")
    # Reaching this line means the corrupted archive was accepted.
    message(STATUS "CORRUPT-ACCEPTED")
    return()
endif()

# ---- driver ---------------------------------------------------------------
file(REMOVE_RECURSE "${WORK_DIR}")
set(wxSOURCE_DIR "${WORK_DIR}/no-such-source")

# --- case 1: intact archive is validated, extracted and stamped -----------
set(wxWINUI3_PACKAGE_ROOT "${WORK_DIR}/packages")
file(MAKE_DIRECTORY "${wxWINUI3_PACKAGE_ROOT}")
make_managed_nupkg(nupkg_v1 sha_v1 "${wxWINUI3_PACKAGE_ROOT}"
    "1.0.0" "${WORK_DIR}/staging" "content-v1")

wx_winui3_find_nuget_package(DIR_V1
    "${TEST_PACKAGE}" "1.0.0" "${TEST_MARKER}" "${sha_v1}")
if(NOT DIR_V1 MATCHES
        "${TEST_PACKAGE}\\.1\\.0\\.0-[0-9a-f]+$" OR
        NOT EXISTS "${DIR_V1}/${TEST_MARKER}")
    message(FATAL_ERROR "case 1: intact archive was not extracted (got ${DIR_V1})")
endif()
file(READ "${DIR_V1}/marker/probe.txt" probe_v1)
if(NOT probe_v1 STREQUAL "content-v1")
    message(FATAL_ERROR "case 1: extracted content mismatch")
endif()
wx_winui3_validate_nupkg_integrity(integrity_v1 "${DIR_V1}" "${sha_v1}")
if(NOT integrity_v1)
    message(FATAL_ERROR
        "case 1: extraction does not have a valid archive-bound manifest")
endif()
message(STATUS "case 1 OK: intact archive validated, extracted and stamped")

# --- case 1b: post-extraction corruption is detected and repaired ----------
file(WRITE "${DIR_V1}/marker/probe.txt" "TAMPERED-AFTER-EXTRACTION")
wx_winui3_find_nuget_package(DIR_V1_REPAIRED
    "${TEST_PACKAGE}" "1.0.0" "${TEST_MARKER}" "${sha_v1}")
file(READ "${DIR_V1_REPAIRED}/marker/probe.txt" probe_v1_repaired)
wx_winui3_validate_nupkg_integrity(
    integrity_v1_repaired "${DIR_V1_REPAIRED}" "${sha_v1}")
if(NOT probe_v1_repaired STREQUAL "content-v1" OR
        NOT integrity_v1_repaired)
    message(FATAL_ERROR
        "case 1b: corrupted extraction was not rebuilt from the archive")
endif()
message(STATUS
    "case 1b OK: post-extraction corruption detected and repaired")

# --- case 2: corrupted cached archive fails before extraction -------------
set(work_corrupt "${WORK_DIR}/corrupt")
set(corrupt_root "${work_corrupt}/packages")
file(MAKE_DIRECTORY "${corrupt_root}")
make_managed_nupkg(nupkg_corrupt sha_pristine "${corrupt_root}"
    "1.0.0" "${work_corrupt}/staging" "content-corrupt")
file(APPEND "${nupkg_corrupt}" "CORRUPTION")

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        -DCASE=corrupt
        -DWINUI_CMAKE=${WINUI_CMAKE}
        -DWORK_DIR=${work_corrupt}
        -DSAFE_ROOT=${SAFE_ROOT}
        -DEXPECTED_SHA=${sha_pristine}
        -P "${CMAKE_CURRENT_LIST_FILE}"
    RESULT_VARIABLE corrupt_result
    OUTPUT_VARIABLE corrupt_stdout
    ERROR_VARIABLE corrupt_stderr
    )
if(corrupt_result EQUAL 0 OR corrupt_stdout MATCHES "CORRUPT-ACCEPTED")
    message(FATAL_ERROR "case 2: the corrupted cached archive was accepted")
endif()
if(NOT corrupt_stderr MATCHES "has SHA-256")
    message(FATAL_ERROR "case 2: unexpected failure message: ${corrupt_stderr}")
endif()
string(SUBSTRING "${sha_pristine}" 0 32 corrupt_key)
if(EXISTS
        "${corrupt_root}/${TEST_PACKAGE}.1.0.0-${corrupt_key}")
    message(FATAL_ERROR "case 2: the corrupted archive was extracted anyway")
endif()
message(STATUS "case 2 OK: corrupted cache rejected before extraction")

# --- case 3: pinned version change never reuses the old directory ---------
# The cache variable still points at the 1.0.0 extraction from case 1.
make_managed_nupkg(nupkg_v2 sha_v2 "${wxWINUI3_PACKAGE_ROOT}"
    "2.0.0" "${WORK_DIR}/staging" "content-v2")

wx_winui3_find_nuget_package(DIR_V2
    "${TEST_PACKAGE}" "2.0.0" "${TEST_MARKER}" "${sha_v2}")
if(NOT DIR_V2 MATCHES
        "${TEST_PACKAGE}\\.2\\.0\\.0-[0-9a-f]+$")
    message(FATAL_ERROR "case 3: version change resolved to ${DIR_V2}")
endif()
file(READ "${DIR_V2}/marker/probe.txt" probe_v2)
if(NOT probe_v2 STREQUAL "content-v2")
    message(FATAL_ERROR "case 3: got stale content after a version change")
endif()
message(STATUS "case 3 OK: version change resolves to the new extraction")

# --- case 4: same-version respin coexists with the old extraction ---------
# A new hash for 1.0.0 gets its own archive/extraction. The old directory must
# remain intact so another build tree already using it keeps stable inputs.
make_managed_nupkg(nupkg_v1_respun sha_v1_respun
    "${wxWINUI3_PACKAGE_ROOT}" "1.0.0"
    "${WORK_DIR}/staging" "content-v1-respun")
if(sha_v1_respun STREQUAL "${sha_v1}")
    message(FATAL_ERROR "case 4: test archives unexpectedly hash-identical")
endif()

wx_winui3_find_nuget_package(DIR_V1B
    "${TEST_PACKAGE}" "1.0.0" "${TEST_MARKER}" "${sha_v1_respun}")
file(READ "${DIR_V1B}/marker/probe.txt" probe_v1b)
if(NOT probe_v1b STREQUAL "content-v1-respun")
    message(FATAL_ERROR "case 4: respun extraction has stale content")
endif()
wx_winui3_validate_nupkg_integrity(
    integrity_v1b "${DIR_V1B}" "${sha_v1_respun}")
if(NOT integrity_v1b)
    message(FATAL_ERROR "case 4: respun extraction manifest is wrong")
endif()
if(NOT EXISTS "${DIR_V1}/marker/probe.txt")
    message(FATAL_ERROR "case 4: old same-version extraction was removed")
endif()
file(READ "${DIR_V1}/marker/probe.txt" probe_v1_still)
if(NOT probe_v1_still STREQUAL "content-v1")
    message(FATAL_ERROR "case 4: old same-version extraction was mutated")
endif()
message(STATUS "case 4 OK: same-version hashes coexist without mutation")

# --- case 5: changing the managed root never reuses the old root -----------
set(package_root_b "${WORK_DIR}/packages-b")
file(MAKE_DIRECTORY "${package_root_b}")
file(COPY "${nupkg_v1_respun}" DESTINATION "${package_root_b}")
set(wxWINUI3_PACKAGE_ROOT "${package_root_b}")

wx_winui3_find_nuget_package(DIR_ROOT_B
    "${TEST_PACKAGE}" "1.0.0" "${TEST_MARKER}" "${sha_v1_respun}")
file(TO_CMAKE_PATH "${DIR_ROOT_B}" dir_root_b_norm)
file(TO_CMAKE_PATH "${package_root_b}" package_root_b_norm)
if(NOT dir_root_b_norm MATCHES "^${package_root_b_norm}/" OR
        DIR_ROOT_B STREQUAL DIR_V1B)
    message(FATAL_ERROR
        "case 5: changing PACKAGE_ROOT kept the old automatic path "
        "${DIR_ROOT_B}")
endif()
if(NOT EXISTS "${DIR_V1B}/marker/probe.txt")
    message(FATAL_ERROR
        "case 5: changing PACKAGE_ROOT removed the old extraction")
endif()
message(STATUS
    "case 5 OK: managed root change resolves below the new root and "
    "preserves the old cache")

# --- case 6: a user-provided directory is trusted and never deleted -------
# Pointing the cache variable outside the managed package root simulates
# -DwxWINUI3_*_PACKAGE_DIR=<somewhere>: it has no stamp, and it must be
# accepted on the strength of the marker alone, never invalidated or
# removed (and no download may be attempted).
set(user_dir "${WORK_DIR}/user-provided/${TEST_PACKAGE}.3.0.0")
file(WRITE "${user_dir}/marker/probe.txt" "user-content")
set(wxWINUI3_WX_SELFTEST_FAKE_PACKAGE_DIR "${user_dir}")

wx_winui3_find_nuget_package(DIR_USER
    "${TEST_PACKAGE}" "3.0.0" "${TEST_MARKER}" "${sha_v1}")
file(TO_CMAKE_PATH "${user_dir}" user_dir_norm)
file(TO_CMAKE_PATH "${DIR_USER}" dir_user_norm)
if(NOT dir_user_norm STREQUAL user_dir_norm)
    message(FATAL_ERROR "case 6: user-provided directory was not used (got ${DIR_USER})")
endif()
if(NOT EXISTS "${user_dir}/marker/probe.txt")
    message(FATAL_ERROR "case 6: user-provided directory was modified or deleted")
endif()
file(READ "${user_dir}/marker/probe.txt" probe_user)
if(NOT probe_user STREQUAL "user-content")
    message(FATAL_ERROR "case 6: user-provided content was altered")
endif()
message(STATUS "case 6 OK: user-provided directory trusted and untouched")

message(STATUS "PASS: wx_winui3_find_nuget_package self-test")
