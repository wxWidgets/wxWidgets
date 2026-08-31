# Prove that inherited diagnostic settings cannot accidentally enable the
# winuispike probes. The output-directory variable is deliberately present:
# without an activation flag it must remain inert and the directory must never
# be created. Before taking this disabled exit, the executable also runs its
# pure matrix for the four accepted activation tokens and representative false
# and misspelled values.

if(NOT DEFINED WINUISPIKE_EXE OR WINUISPIKE_EXE STREQUAL "")
    message(FATAL_ERROR "WINUISPIKE_EXE is required")
endif()
if(NOT EXISTS "${WINUISPIKE_EXE}")
    message(FATAL_ERROR "winuispike executable not found: ${WINUISPIKE_EXE}")
endif()
if(NOT DEFINED TEST_ROOT OR TEST_ROOT STREQUAL "")
    message(FATAL_ERROR "TEST_ROOT is required")
endif()

file(MAKE_DIRECTORY "${TEST_ROOT}")
string(TIMESTAMP timestamp "%Y%m%d-%H%M%S" UTC)
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef nonce)
set(probe_dir
    "${TEST_ROOT}/diagnostics-default-off-${timestamp}-${nonce}")
if(EXISTS "${probe_dir}")
    message(FATAL_ERROR "fresh probe path already exists: ${probe_dir}")
endif()

get_filename_component(executable_dir "${WINUISPIKE_EXE}" DIRECTORY)
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        --unset=WX_WINUI_DIAGNOSTICS
        --unset=WX_WINUI_SPIKE_DIAGNOSTICS
        --unset=WX_WINUI_INPUT_LOG
        "WX_WINUI_DIAGNOSTICS_DIR=${probe_dir}"
        --
        "${WINUISPIKE_EXE}" --diagnostics-self-test
    WORKING_DIRECTORY "${executable_dir}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE stdout
    ERROR_VARIABLE stderr
    TIMEOUT 15
    ENCODING UTF-8
)

if(NOT "${result}" STREQUAL "20")
    message(FATAL_ERROR
        "default-off probe returned '${result}', expected 20\n"
        "stdout:\n${stdout}\n"
        "stderr:\n${stderr}")
endif()
if(EXISTS "${probe_dir}")
    message(FATAL_ERROR
        "default-off probe created its diagnostics directory: ${probe_dir}")
endif()

message(STATUS
    "winuispike diagnostics remained disabled (expected exit 20)")
