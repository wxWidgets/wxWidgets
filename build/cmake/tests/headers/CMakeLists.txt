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
    allheaders_utf8.cpp

    testprec.h
    testableframe.h
    allheaders.h
    )

# This file defines symbols affecting the wx headers before including them, so
# it can't use precompiled headers.
set_source_files_properties(${wxSOURCE_DIR}/tests/allheaders_utf8.cpp
    PROPERTIES
        SKIP_PRECOMPILE_HEADERS ON
        COTIRE_EXCLUDED ON
    )

wx_add_test(test_headers CONSOLE_GUI ${TEST_SRC})
if(wxUSE_SOCKETS)
    wx_exe_link_libraries(test_headers wxnet)
endif()
