#############################################################################
# Name:        build/cmake/tests/gui/CMakeLists.txt
# Purpose:     CMake file for gui test
# Author:      Tobias Taschner
# Created:     2016-10-31
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

set(TEST_GUI_SRC
    asserthelper.cpp
    test.cpp
    testableframe.cpp
    geometry/rect.cpp
    geometry/size.cpp
    geometry/point.cpp
    geometry/region.cpp
    graphics/bitmap.cpp
    graphics/colour.cpp
    graphics/ellipsization.cpp
    graphics/measuring.cpp
    graphics/affinematrix.cpp
    graphics/boundingbox.cpp
    graphics/clippingbox.cpp
    graphics/graphmatrix.cpp
    graphics/graphpath.cpp
    config/config.cpp
    controls/bitmapcomboboxtest.cpp
    controls/bitmaptogglebuttontest.cpp
    controls/bookctrlbasetest.cpp
    controls/buttontest.cpp
    controls/checkboxtest.cpp
    controls/checklistboxtest.cpp
    controls/choicebooktest.cpp
    controls/choicetest.cpp
    controls/comboboxtest.cpp
    controls/dataviewctrltest.cpp
    controls/datepickerctrltest.cpp
    controls/frametest.cpp
    controls/gaugetest.cpp
    controls/gridtest.cpp
    controls/headerctrltest.cpp
    controls/htmllboxtest.cpp
    controls/hyperlinkctrltest.cpp
    controls/itemcontainertest.cpp
    controls/label.cpp
    controls/listbasetest.cpp
    controls/listbooktest.cpp
    controls/listboxtest.cpp
    controls/listctrltest.cpp
    controls/listviewtest.cpp
    controls/markuptest.cpp
    controls/notebooktest.cpp
    controls/ownerdrawncomboboxtest.cpp
    controls/pickerbasetest.cpp
    controls/pickertest.cpp
    controls/radioboxtest.cpp
    controls/radiobuttontest.cpp
    controls/rearrangelisttest.cpp
    controls/richtextctrltest.cpp
    controls/searchctrltest.cpp
    controls/simplebooktest.cpp
    controls/slidertest.cpp
    controls/spinctrldbltest.cpp
    controls/spinctrltest.cpp
    controls/styledtextctrltest.cpp
    controls/textctrltest.cpp
    controls/textentrytest.cpp
    controls/togglebuttontest.cpp
    controls/toolbooktest.cpp
    controls/treebooktest.cpp
    controls/treectrltest.cpp
    controls/treelistctrltest.cpp
    controls/virtlistctrltest.cpp
    controls/webtest.cpp
    controls/windowtest.cpp
    controls/dialogtest.cpp
    events/clone.cpp
    # Duplicate this file here to test GUI event loops too.
    events/evtlooptest.cpp
    events/propagation.cpp
    events/keyboard.cpp
    # And duplicate this one too as wxExecute behaves differently in
    # console and GUI applications.
    exec/exec.cpp
    font/fonttest.cpp
    image/image.cpp
    image/rawbmp.cpp
    html/htmlparser.cpp
    html/htmlwindow.cpp
    html/htmprint.cpp
    menu/accelentry.cpp
    menu/menu.cpp
    misc/guifuncs.cpp
    misc/selstoretest.cpp
    misc/garbage.cpp
    misc/safearrayconverttest.cpp
    misc/settings.cpp
    # This one is intentionally duplicated here (it is also part of
    # non-GUI test) as sockets behave differently in console and GUI
    # applications.
    net/socket.cpp
    persistence/tlw.cpp
    persistence/dataview.cpp
    sizers/boxsizer.cpp
    sizers/gridsizer.cpp
    sizers/wrapsizer.cpp
    toplevel/toplevel.cpp
    validators/valnum.cpp
    window/clientsize.cpp
    window/setsize.cpp
    xml/xrctest.cpp

    testprec.h
    testableframe.h
    asserthelper.h
    testdate.h
    testfile.h
    testimage.h
    controls/bookctrlbasetest.h
    controls/itemcontainertest.h
    controls/listbasetest.h
    controls/pickerbasetest.h
    controls/textentrytest.h
    persistence/testpersistence.h
    )

set(TEST_GUI_DATA
    horse.ani
    horse.bmp
    horse.cur
    horse.gif
    horse.ico
    horse.jpg
    horse.pcx
    horse.png
    horse.pnm
    horse.tga
    horse.tif
    horse.xpm
    image/horse_grey.bmp
    image/horse_grey_flipped.bmp
    image/horse_rle4.bmp
    image/horse_rle4_flipped.bmp
    image/horse_rle8.bmp
    image/horse_rle8_flipped.bmp
    image/cross_bicubic_256x256.png
    image/cross_bilinear_256x256.png
    image/cross_box_average_256x256.png
    image/cross_nearest_neighb_256x256.png
    image/horse_bicubic_50x50.png
    image/horse_bicubic_100x100.png
    image/horse_bicubic_150x150.png
    image/horse_bicubic_300x300.png
    image/horse_bilinear_50x50.png
    image/horse_bilinear_100x100.png
    image/horse_bilinear_150x150.png
    image/horse_bilinear_300x300.png
    image/horse_box_average_50x50.png
    image/horse_box_average_100x100.png
    image/horse_box_average_150x150.png
    image/horse_box_average_300x300.png
    intl/ja/internat.mo
    intl/ja/internat.po
    )

wx_add_test(test_gui ${TEST_GUI_SRC}
    DATA ${TEST_GUI_DATA}
    RES ../samples/sample.rc
    )
wx_exe_link_libraries(test_gui wxcore)
if(wxUSE_RICHTEXT)
    wx_exe_link_libraries(test_gui wxrichtext)
endif()
if(wxUSE_STC)
    wx_exe_link_libraries(test_gui wxstc)
endif()
if(wxUSE_MEDIACTRL)
    wx_exe_link_libraries(test_gui wxmedia)
endif()
if(wxUSE_XRC)
    wx_exe_link_libraries(test_gui wxxrc)
endif()
if(wxUSE_XML)
    wx_exe_link_libraries(test_gui wxxml)
endif()
if(wxUSE_HTML)
    wx_exe_link_libraries(test_gui wxhtml)
endif()
if(wxUSE_SOCKETS)
    wx_exe_link_libraries(test_gui wxnet)
endif()
if(wxUSE_WEBVIEW)
    wx_exe_link_libraries(test_gui wxwebview)
endif()
wx_test_enable_precomp(test_gui)
