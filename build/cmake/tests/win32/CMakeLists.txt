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

wx_add_test(test_win32_unicode ${TEST_SRC} CHARSET UNICODE _UNICODE)

wx_add_test(test_win32_mbcs ${TEST_SRC} CHARSET _MBCS)

wx_add_test(test_win32_sbcs ${TEST_SRC} CHARSET _SBCS)
