#############################################################################
# Name:        build/cmake/tests/headers/CMakeLists.txt
# Purpose:     CMake file for headers test
# Author:      Maarten Bent
# Created:     2020-07-23
# Copyright:   (c) 2020 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

set(TEST_SRC
    test.cpp
    testableframe.cpp
    allheaders.cpp

    testprec.h
    testableframe.h
    allheaders.h
    )

wx_add_test(test_headers CONSOLE_GUI ${TEST_SRC})
if(wxUSE_SOCKETS)
    wx_exe_link_libraries(test_headers wxnet)
endif()
