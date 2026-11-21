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
    graphics/bmpbundle.cpp
    graphics/colour.cpp
    graphics/ellipsization.cpp
    graphics/measuring.cpp
    graphics/affinematrix.cpp
    graphics/boundingbox.cpp
    graphics/clipper.cpp
    graphics/clippingbox.cpp
    graphics/svgattributes.cpp
    graphics/coords.cpp
    graphics/graphbitmap.cpp
    graphics/graphmatrix.cpp
    graphics/graphpath.cpp
    graphics/imagelist.cpp
    config/config.cpp
    controls/auitest.cpp
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
    controls/editlboxtest.cpp
    controls/filesystemctrltest.cpp
    controls/frametest.cpp
    controls/gaugetest.cpp
    controls/gridtest.cpp
    controls/headerctrltest.cpp
    controls/htmllboxtest.cpp
    controls/hyperlinkctrltest.cpp
    controls/itemcontainertest.cpp
    controls/infobar.cpp
    controls/label.cpp
    controls/mswoledropsession.cpp
    controls/winuiactivex.cpp
    controls/winuiolebroker.cpp
    controls/winuihostlifecycle.cpp
    controls/winuiadvancedharness.cpp
    controls/winuinativewindow.cpp
    controls/winuimdi.cpp
    controls/winuiglcanvas.cpp
    controls/winuiwebview.cpp
    controls/winuimedia.cpp
    controls/winuihtml.cpp
    controls/winuirichtext.cpp
    controls/winuiaui.cpp
    controls/winuiauitabview.cpp
    controls/winuistc.cpp
    controls/winuiribbon.cpp
    controls/winuixamlmetadata.cpp
    controls/winuicompositefocus.cpp
    controls/winuicompositegeometry.cpp
    controls/winuicompositedirect.cpp
    controls/winuicompositepopup.cpp
    controls/winuiflyoutretirement.cpp
    controls/winuibridgepaint.cpp
    controls/winuicompositeretirement.cpp
    controls/winuiinputrouter.cpp
    controls/winuirootinput.cpp
    controls/winuiinputstate.cpp
    controls/winuiaccessibility.cpp
    controls/winuiappearance.cpp
    controls/winuichromefeedback.cpp
    controls/winuitoolbar.cpp
    controls/winuitoolbook.cpp
    controls/winuifilebrowsers.cpp
    controls/winuiradioboxtest.cpp
    controls/winuishowcaselayout.cpp
    controls/winuitreetheme.cpp
    controls/winuicombomodel.cpp
    controls/winuiitemmodel.cpp
    controls/winuidataview.cpp
    controls/winuidataviewcolumns.cpp
    controls/winuidatavieweditors.cpp
    controls/winuidataviewlist.cpp
    controls/winuigrid.cpp
    controls/winuigridautomation.cpp
    controls/winuilistctrl.cpp
    controls/winuiheaderctrl.cpp
    controls/winuipropgrid.cpp
    controls/winuipropgrideditors.cpp
    controls/winuipropgridpreview.cpp
    controls/winuiwindowscrollbar.cpp
    controls/winuiwindowscrollbarindicator.cpp
    controls/winuilistmodel.cpp
    controls/winuirangedate.cpp
    controls/winuirangeevents.cpp
    controls/winuispinmodel.cpp
    controls/winuitransientstate.cpp
    controls/winuitransientwindows.cpp
    controls/winuiauxwindows.cpp
    controls/winuigenericfeedback.cpp
    controls/winuinotification.cpp
    controls/winuitaskbaricon.cpp
    controls/winuitaskbarbutton.cpp
    controls/winuititlebar.cpp
    controls/winuitextpeer.cpp
    controls/listbasetest.cpp
    controls/listbooktest.cpp
    controls/listboxtest.cpp
    controls/listctrltest.cpp
    controls/listviewtest.cpp
    controls/markuptest.cpp
    controls/notebooktest.cpp
    controls/winuinotebook.cpp
    controls/winuitreemodel.cpp
    controls/ownerdrawncomboboxtest.cpp
    controls/pickerbasetest.cpp
    controls/pickertest.cpp
    controls/winuipickercontracts.cpp
    controls/winuiwizardpropsheet.cpp
    controls/propgridtest.cpp
    controls/radioboxtest.cpp
    controls/radiobuttontest.cpp
    controls/rearrangelisttest.cpp
    controls/richtextctrltest.cpp
    controls/searchctrltest.cpp
    controls/simplebooktest.cpp
    controls/slidertest.cpp
    controls/spinctrldbltest.cpp
    controls/spinctrltest.cpp
    controls/statbmptest.cpp
    controls/styledtextctrltest.cpp
    controls/textctrltest.cpp
    controls/textentrytest.cpp
    controls/togglebuttontest.cpp
    controls/toolbooktest.cpp
    controls/treebooktest.cpp
    controls/treectrltest.cpp
    controls/treelistctrltest.cpp
    controls/vlboxtest.cpp
    controls/virtlistctrltest.cpp
    controls/webtest.cpp
    controls/windowtest.cpp
    controls/dialogtest.cpp
    controls/winuiframeclassdialog.cpp
    controls/winuidialogcontracts.cpp
    controls/winuiprinting.cpp
    controls/winuidialogsession.cpp
    controls/winuirichtooltip.cpp
    docview/updateallviews.cpp
    events/clone.cpp
    events/enterleave.cpp
    # Duplicate this file here to test GUI event loops too.
    events/evtlooptest.cpp
    events/propagation.cpp
    events/keyboard.cpp
    events/winuikeyboard.cpp
    # And duplicate this one too as wxExecute behaves differently in
    # console and GUI applications.
    exec/exec.cpp
    font/fonttest.cpp
    image/image.cpp
    image/imagwebp.cpp
    image/rawbmp.cpp
    html/helpdata.cpp
    html/htmlparser.cpp
    html/htmlwindow.cpp
    html/htmprint.cpp
    menu/accelentry.cpp
    menu/menu.cpp
    menu/winuimenubar.cpp
    misc/guifuncs.cpp
    misc/selstoretest.cpp
    misc/garbage.cpp
    misc/safearrayconverttest.cpp
    misc/settings.cpp
    misc/textwrap.cpp
    # This one is intentionally duplicated here (it is also part of
    # non-GUI test) as sockets behave differently in console and GUI
    # applications.
    net/socket.cpp
    persistence/tlw.cpp
    persistence/dataview.cpp
    rowheightcache/rowheightcachetest.cpp
    sizers/boxsizer.cpp
    sizers/gridsizer.cpp
    sizers/wrapsizer.cpp
    toplevel/toplevel.cpp
    validators/valnum.cpp
    validators/valtext.cpp
    window/clientsize.cpp
    window/setsize.cpp
    xml/xrctest.cpp

    testprec.h
    testableframe.h
    asserthelper.h
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
    horse.svg
    horse.tga
    horse.tif
    horse.xpm
    horse.webp
    image/bitfields.bmp
    image/bitfields-alpha.bmp
    image/8bpp-colorsused-large.bmp
    image/8bpp-colorsused-negative.bmp
    image/32bpp_rgb.bmp
    image/32bpp_rgb.ico
    image/32bpp_rgb_a0.ico
    image/badrle4.bmp
    image/rgb16-3103.bmp
    image/rgb32-7187.bmp
    image/rgb32bf.bmp
    image/rgba32.bmp
    image/rle4-delta-320x240.bmp
    image/rle8-delta-320x240-expected.bmp
    image/rle8-delta-320x240.bmp
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
    image/paste_input_background.png
    image/paste_input_black.png
    image/paste_input_overlay_transparent_border_opaque_square.png
    image/paste_input_overlay_transparent_border_semitransparent_circle.png
    image/paste_input_overlay_transparent_border_semitransparent_square.png
    image/paste_result_background_plus_circle_plus_square.png
    image/paste_result_background_plus_overlay_transparent_border_opaque_square.png
    image/paste_result_background_plus_overlay_transparent_border_semitransparent_square.png
    image/paste_result_no_background_square_over_circle.png
    image/wx.png
    image/toucan.png
    image/toucan_hue_0.538.png
    image/toucan_sat_-0.41.png
    image/toucan_bright_-0.259.png
    image/toucan_hsv_0.538_-0.41_-0.259.png
    image/toucan_light_46.png
    image/toucan_dis_240.png
    image/toucan_grey.png
    image/toucan_mono_255_255_255.png
    image/width-times-height-overflow.bmp
    image/width_height_32_bit_overflow.pgm
    image/bad_truncated.gif
    intl/ja/internat.mo
    intl/ja/internat.po
    )

set(TEST_GUI_RES_BUNDLE
    horse.png
    ../include/wx/msw/bullseye.cur
    )

# On macOS create an .app bundle so the resources can be added.
# On other platforms, just use a console app with gui libraries.
set(APP_TYPE)
if(NOT APPLE)
    set(APP_TYPE CONSOLE_GUI)
endif()

if(wxWINUI3_EXPERIMENTAL_RIBBON_TOOLKIT)
    list(APPEND TEST_GUI_SRC
        controls/winuiribbontoolkit.cpp
        controls/winuiribbontoolbar.cpp
        controls/winuiribbongallery.cpp
        controls/winuiribbonflyout.cpp
        controls/winuiribbonexpansion.cpp)
endif()

wx_add_test(test_gui ${APP_TYPE} ${TEST_GUI_SRC}
    DATA ${TEST_GUI_DATA}
    RES_BUNDLE ${TEST_GUI_RES_BUNDLE}
    )
if(TARGET test_gui AND wxWINUI3_EXPERIMENTAL_RIBBON_TOOLKIT)
    wx_winui3_configure_toolkit_target(test_gui)
    target_compile_definitions(test_gui PRIVATE WXWINUI_RIBBON_TOOLKIT=1)
endif()
if(TARGET test_gui AND TARGET wx_winui_test_support)
    target_link_libraries(test_gui PRIVATE wx_winui_test_support)
endif()
if(TARGET test_gui AND WXMSW AND wxUSE_TASKBARBUTTON AND
        wxBUILD_TESTS STREQUAL "ALL")
    target_include_directories(test_gui PRIVATE
        "${wxSOURCE_DIR}/tests/winui/test-support")
    target_compile_definitions(test_gui PRIVATE
        WXMSW_TASKBARBUTTON_TEST_SUPPORT=1)
endif()
if(TARGET test_gui AND wxUSE_AUI AND wxBUILD_TESTS STREQUAL "ALL")
    target_include_directories(test_gui PRIVATE
        "${wxSOURCE_DIR}/tests/winui/test-support")
    target_compile_definitions(test_gui PRIVATE
        WXAUI_NOTEBOOK_TEST_SUPPORT=1)
endif()
if(WXWINUI AND wxUSE_WINUI3 AND TARGET test_gui AND
        TARGET wx_winui_desktop_test_runner)
    add_test(NAME wx_winui_supported_beta
        COMMAND
            $<TARGET_FILE:wx_winui_desktop_test_runner>
            --timeout-ms=240000
            --
            $<TARGET_FILE:test_gui>
            "[winui-v0-supported]~[.]~[physical]"
        WORKING_DIRECTORY
            "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
    set_tests_properties(wx_winui_supported_beta PROPERTIES
        LABELS "winui-supported-beta"
        ENVIRONMENT "WX_UI_TESTS=0"
        RUN_SERIAL TRUE
        TIMEOUT 300)
    foreach(contract IN ITEMS component-metadata opaque-gdi composite flyout-retirement input-native range-date-native)
        string(REPLACE "-" "_" contract_name "${contract}")
        if(contract STREQUAL "component-metadata")
            set(contract_filter "[winui-component-metadata]~[.]~[physical]")
        elseif(contract STREQUAL "composite")
            set(contract_filter "[winui-composite]~[.]~[physical],[winui-composite-retirement]~[.]~[physical]")
        elseif(contract STREQUAL "flyout-retirement")
            set(contract_filter "[winui-flyout-retirement]~[.]~[physical],[winui-popup-topology]~[.]~[physical],[winui-popup-extension]~[.]~[physical],[autocomplete]~[.]~[physical]")
        elseif(contract STREQUAL "input-native")
            set(contract_filter "[winui-root-input]~[.]~[physical],[winui-native-resize]~[.]~[physical],[winui-dialog-contract]~[.]~[physical],[winui-radiobox]~[.]~[physical],[winui-combo-custom-popup]~[.]~[physical]")
        elseif(contract STREQUAL "range-date-native")
            set(contract_filter "[winui-range-date]~[.]~[physical],[winui-showcase-layout]~[.]~[physical]")
        else()
            set(contract_filter "[opaque-gdi]~[.]~[physical]")
        endif()
        add_test(NAME wx_winui_${contract_name}
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=240000
                --
                $<TARGET_FILE:test_gui>
                "${contract_filter}"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_${contract_name} PROPERTIES
            LABELS "winui-${contract}"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 300)
    endforeach()
    if(wxUSE_DATAVIEWCTRL)
        add_test(NAME wx_winui_dataview_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=240000
                --
                $<TARGET_FILE:test_gui>
                "[winui-dataview]~[.]~[physical],[winui-dataview-list]~[.]~[physical],[wxDataViewCtrl]~[.]~[physical],[treelistctrl]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_dataview_native PROPERTIES
            LABELS "winui-native-dataview"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 300)
    endif()
    if(wxUSE_RIBBON)
        add_test(NAME wx_winui_ribbon_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=240000
                --
                $<TARGET_FILE:test_gui>
                "[winui-ribbon][native-buttonbar]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_ribbon_native PROPERTIES
            LABELS "winui-native-ribbon"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 300)
        if(wxWINUI3_EXPERIMENTAL_RIBBON_TOOLKIT)
            add_test(NAME wx_winui_ribbon_toolkit
                COMMAND
                    $<TARGET_FILE:wx_winui_desktop_test_runner>
                    --timeout-ms=240000
                    --
                    $<TARGET_FILE:test_gui>
                    "[winui-ribbon-toolkit]~[.]~[physical]"
                WORKING_DIRECTORY
                    "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
            set_tests_properties(wx_winui_ribbon_toolkit PROPERTIES
                LABELS "winui-experimental-ribbon-toolkit"
                ENVIRONMENT "WX_UI_TESTS=0"
                RUN_SERIAL TRUE
                TIMEOUT 300)
        endif()
    endif()
    if(wxUSE_BUTTON)
        add_test(NAME wx_winui_window_scrollbar_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=240000
                --
                $<TARGET_FILE:test_gui>
                "[winui-window-scrollbar]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_window_scrollbar_native PROPERTIES
            LABELS "winui-native-window-scrollbar"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 300)
    endif()
    if(wxUSE_HEADERCTRL)
        add_test(NAME wx_winui_headerctrl_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=240000
                --
                $<TARGET_FILE:test_gui>
                "[winui-headerctrl]~[.]~[physical],[headerctrl]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_headerctrl_native PROPERTIES
            LABELS "winui-native-headerctrl"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 300)
    endif()
    if(wxUSE_AUI)
        add_test(NAME wx_winui_aui_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=240000
                --
                $<TARGET_FILE:test_gui>
                "[winui-aui-native]~[.]~[physical],[WinUIAUI]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_aui_native PROPERTIES
            LABELS "winui-native-aui"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 300)
    endif()
    if(wxUSE_TOOLBOOK)
        add_test(NAME wx_winui_toolbook_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=120000
                --
                $<TARGET_FILE:test_gui>
                "[winui-toolbook]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_toolbook_native PROPERTIES
            LABELS "winui-native-toolbook"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 150)
    endif()
    if(wxUSE_FILECTRL AND (wxUSE_FILEDLG OR wxUSE_DIRDLG) AND wxUSE_TREECTRL AND wxUSE_LISTCTRL)
        add_test(NAME wx_winui_file_browsers_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=120000
                --
                $<TARGET_FILE:test_gui>
                "[winui-file-browsers]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_file_browsers_native PROPERTIES
            LABELS "winui-native-file-browsers"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 150)
    endif()
    if(wxUSE_TREECTRL)
        add_test(NAME wx_winui_tree_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=240000
                --
                $<TARGET_FILE:test_gui>
                "[winui-treemodel]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_tree_native PROPERTIES
            LABELS "winui-native-tree"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 300)
    endif()
    if(wxUSE_LISTCTRL)
        add_test(NAME wx_winui_listctrl_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=240000
                --
                $<TARGET_FILE:test_gui>
                "[winui-listctrl]~[.]~[physical],[listctrl]~[.]~[physical],[editablelist]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_listctrl_native PROPERTIES
            LABELS "winui-native-listctrl"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 300)
    endif()
    if(wxUSE_PROPGRID)
        add_test(NAME wx_winui_propgrid_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=240000
                --
                $<TARGET_FILE:test_gui>
                "[winui-propgrid]~[.]~[physical],[propgrid]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_propgrid_native PROPERTIES
            LABELS "winui-native-propgrid"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 300)
    endif()
    if(wxUSE_GRID)
        add_test(NAME wx_winui_grid_native
            COMMAND
                $<TARGET_FILE:wx_winui_desktop_test_runner>
                --timeout-ms=240000
                --
                $<TARGET_FILE:test_gui>
                "[winui-grid]~[.]~[physical]"
            WORKING_DIRECTORY
                "${wxOUTPUT_DIR}/${wxPLATFORM_LIB_DIR}")
        set_tests_properties(wx_winui_grid_native PROPERTIES
            LABELS "winui-native-grid"
            ENVIRONMENT "WX_UI_TESTS=0"
            RUN_SERIAL TRUE
            TIMEOUT 300)
    endif()
endif()
if(WXWINUI AND wxUSE_WINUI3 AND MSVC)
    # These are the TUs pulling in the C++/WinRT projection: scope /bigobj
    # to them instead of the whole target. wx_add_test() registers all test
    # sources using absolute paths, so use the same identity here: CMake source
    # properties don't match a later relative spelling of the same file.
    wx_list_add_prefix(TEST_GUI_BIGOBJ_SRC
        "${wxSOURCE_DIR}/tests/"
        controls/winuihostlifecycle.cpp
        controls/winuicompositefocus.cpp
        controls/winuicompositegeometry.cpp
        controls/winuicompositedirect.cpp
        controls/winuicompositepopup.cpp
        controls/winuiflyoutretirement.cpp
        controls/winuidatavieweditors.cpp
        controls/winuidataviewcolumns.cpp
        controls/winuigrid.cpp
        controls/winuigridautomation.cpp
        controls/winuilistctrl.cpp
        controls/winuiheaderctrl.cpp
        controls/winuipropgrid.cpp
        controls/winuipropgrideditors.cpp
        controls/winuipropgridpreview.cpp
        controls/winuitoolbook.cpp
        controls/winuifilebrowsers.cpp
        controls/winuiradioboxtest.cpp
        controls/winuishowcaselayout.cpp
        controls/winuitreetheme.cpp
        controls/winuiwindowscrollbar.cpp
        controls/winuiwindowscrollbarindicator.cpp
        controls/winuibridgepaint.cpp
        controls/winuicompositeretirement.cpp
        controls/winuiadvancedharness.cpp
        controls/winuinativewindow.cpp
        controls/winuimdi.cpp
        controls/winuiglcanvas.cpp
        controls/winuiwebview.cpp
        controls/winuimedia.cpp
        controls/winuihtml.cpp
        controls/winuirichtext.cpp
        controls/winuiaui.cpp
        controls/winuiauitabview.cpp
        controls/winuistc.cpp
        controls/winuiribbon.cpp
        controls/winuiribbontoolkit.cpp
        controls/winuiribbontoolbar.cpp
        controls/winuiribbongallery.cpp
        controls/winuiribbonflyout.cpp
        controls/winuiribbonexpansion.cpp
        controls/winuiaccessibility.cpp
        controls/winuiolebroker.cpp
        controls/winuirootinput.cpp
        controls/winuitransientwindows.cpp
        )
    set_source_files_properties(${TEST_GUI_BIGOBJ_SRC}
        PROPERTIES COMPILE_OPTIONS "/bigobj")
endif()
if(WXWINUI AND wxUSE_WINUI3 AND wxUSE_WEBVIEW AND wxUSE_WEBVIEW_EDGE)
    # The XAML WebView2 interop ABI must come from the same pinned package as
    # the generated Microsoft.Web.WebView2.Core projection. Put it before the
    # older SDK include path published transitively by wxwebview.
    target_include_directories(test_gui BEFORE PRIVATE
        "${wxWINUI3_WEBVIEW2_PACKAGE_DIR}/build/native/include-winrt"
        "${wxWINUI3_WEBVIEW2_PACKAGE_DIR}/build/native/include"
        )
endif()
if(wxUSE_AUI)
    wx_exe_link_libraries(test_gui wxaui)
endif()
if(wxUSE_RIBBON)
    wx_exe_link_libraries(test_gui wxribbon)
endif()
if(wxUSE_OPENGL)
    wx_exe_link_libraries(test_gui wxgl)
endif()
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
if(wxUSE_PROPGRID)
    wx_exe_link_libraries(test_gui wxpropgrid)
endif()
if(TARGET test_gui AND wxUSE_PROPGRID AND
        wxBUILD_TESTS STREQUAL "ALL")
    target_include_directories(test_gui PRIVATE
        "${wxSOURCE_DIR}/tests/winui/test-support")
    target_compile_definitions(test_gui PRIVATE
        WXPROPGRID_TEST_SUPPORT=1)
endif()
