#############################################################################
# Name:        build/cmake/tests/base/CMakeLists.txt
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
    sizers/boxsizer.cpp
    sizers/gridsizer.cpp
    sizers/wrapsizer.cpp
    toplevel/toplevel.cpp
    validators/valnum.cpp
    window/clientsize.cpp
    window/setsize.cpp
    xml/xrctest.cpp
    )
wx_add_test(test_gui ${TEST_GUI_SRC})
wx_exe_link_libraries(test_gui core)
if(wxUSE_RICHTEXT)
    wx_exe_link_libraries(test_gui richtext)
endif()
if(wxUSE_MEDIACTRL)
    wx_exe_link_libraries(test_gui media)
endif()
if(wxUSE_XRC)
    wx_exe_link_libraries(test_gui xrc)
endif()
if(wxUSE_XML)
    wx_exe_link_libraries(test_gui xml)
endif()
wx_exe_link_libraries(test_gui adv)
if(wxUSE_HTML)
    wx_exe_link_libraries(test_gui html)
endif()
if(wxUSE_SOCKETS)
    wx_exe_link_libraries(test_gui net)
endif()
if(wxUSE_WEBVIEW)
    wx_exe_link_libraries(test_gui webview)
endif()
wx_test_enable_precomp(test_gui)
