# Test-only projection generator used by cppwinrt-cache.cmake. It deliberately
# emits more than the single sentinel header so the production worker's full
# manifest validation can be exercised without requiring an SDK invocation.

if(NOT DEFINED OUTPUT_DIR OR NOT DEFINED INPUT_LIST)
    message(FATAL_ERROR "OUTPUT_DIR and INPUT_LIST are required")
endif()

file(STRINGS "${INPUT_LIST}" fake_inputs)
list(GET fake_inputs 0 first_input)
file(READ "${first_input}" input_content)

file(MAKE_DIRECTORY "${OUTPUT_DIR}/winrt")
file(WRITE "${OUTPUT_DIR}/winrt/Microsoft.UI.Xaml.h"
    "Xaml projection: ${input_content}\n")
file(WRITE "${OUTPUT_DIR}/winrt/Microsoft.UI.Secondary.h"
    "Secondary projection: ${input_content}\n")
