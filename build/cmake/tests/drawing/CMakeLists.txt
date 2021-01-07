#############################################################################
# Name:        build/cmake/tests/drawing/CMakeLists.txt
# Purpose:     CMake file for drawing test
# Author:      Tobias Taschner
# Created:     2016-10-31
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

# This test program is targeted to "headless GUI" tests, tests which are
# typically tied to the "core" component but that should run nicely in a
# console only program. This program should be executable from a console
# only Unix session (such as telnet or ssh) although it uses graphics
# contexts, so if you modify this project, please check that it can still
# be ran in such configuration and doesn't require an X server connection.
set(TEST_DRAWING_SRC
    test.cpp
    testableframe.cpp
    drawing/drawing.cpp
    drawing/plugindriver.cpp
    drawing/basictest.cpp
    drawing/fonttest.cpp

    testprec.h
    testableframe.h
    testimage.h
    drawing/drawing.h
    drawing/gcfactory.h
    drawing/plugin.h
    drawing/pluginsample.cpp
    drawing/testimagefile.h
    )

set(TEST_DRAWING_DATA
    drawing/references/image_test_image_cairo-1.8_2_ref.png
    drawing/references/image_test_image_cg-10.5_2_ref.png
    drawing/references/image_test_image_gdiplus-6.1_2_ref.png
    )

wx_add_test(test_drawing ${TEST_DRAWING_SRC}
    DATA ${TEST_DRAWING_DATA}
    )
if(wxUSE_SOCKETS)
    wx_exe_link_libraries(test_drawing wxnet)
endif()
wx_exe_link_libraries(test_drawing wxcore)
wx_test_enable_precomp(test_drawing)

# This is a sample plugin, it simply uses a wxImage based
# wxGraphicsContext. It should render the same as the built-in test. Use
# the WX_TEST_SUITE_GC_DRAWING_PLUGINS variable to specify the location of
# the produced DLL/so to get it loaded and tested. To make your own plugin,
# you can copy this sample and link toward your own implementation of
# wxGraphicsContext interface, building the appropriate DrawingTestGCFactory
# TODO: test_drawingplugin
