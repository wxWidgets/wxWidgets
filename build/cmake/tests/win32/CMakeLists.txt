#############################################################################
# Name:        build/cmake/tests/win32/CMakeLists.txt
# Purpose:     CMake file for win32 test
# Author:      Maarten Bent
# Created:     2025-11-15
# Copyright:   (c) 2025 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

set(TEST_SRC
    testwin32.cpp
)

wx_add_test(test_win32_wide ${TEST_SRC})
if(TARGET test_win32_wide)
    target_compile_definitions(test_win32_wide PRIVATE UNICODE _UNICODE)
endif()

wx_add_test(test_win32_ansi ${TEST_SRC})
if(TARGET test_win32_ansi)
    target_compile_definitions(test_win32_ansi PRIVATE _SBCS)
endif()
