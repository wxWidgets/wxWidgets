#############################################################################
# Name:        build/cmake/samples/CMakeLists.txt
# Purpose:     CMake script to build samples
# Author:      Tobias Taschner
# Created:     2016-10-04
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

wx_add_sample(access accesstest.cpp DEPENDS wxUSE_ACCESSIBILITY)
wx_add_sample(animate anitest.cpp anitest.h DATA throbber.gif throbber_2x.gif hourglass.ani DEPENDS wxUSE_ANIMATIONCTRL)
wx_add_sample(archive CONSOLE)
wx_add_sample(artprov arttest.cpp artbrows.cpp artbrows.h)
wx_add_sample(aui auidemo.cpp LIBRARIES wxaui wxhtml NAME auidemo DEPENDS wxUSE_AUI)
wx_add_sample(calendar DEPENDS wxUSE_CALENDARCTRL)
wx_add_sample(caret DEPENDS wxUSE_CARET)
wx_add_sample(collpane DEPENDS wxUSE_COLLPANE)
wx_add_sample(combo DATA dropbuth.svg dropbutn.svg dropbutp.svg three-dots.svg DEPENDS wxUSE_COMBOCTRL)
wx_add_sample(config conftest.cpp DEPENDS wxUSE_CONFIG)
wx_add_sample(console CONSOLE IMPORTANT)
wx_add_sample(dataview IMPORTANT dataview.cpp mymodels.cpp mymodels.h DEPENDS wxUSE_DATAVIEWCTRL)
if(wxUSE_ON_FATAL_EXCEPTION AND (NOT WIN32 OR MSVC))
    wx_add_sample(debugrpt LIBRARIES wxqa DEPENDS wxUSE_DEBUGREPORT)
endif()
set(SAMPLE_DIALOGS_SRC dialogs.cpp dialogs.h)
if(NOT wxBUILD_SHARED AND (WXMSW OR APPLE)) # AND NOT WXUNIV
    wx_list_add_prefix(SAMPLE_DIALOGS_SRC ../../src/generic/ colrdlgg.cpp dirdlgg.cpp filedlgg.cpp)
    if(WXMSW)
        wx_list_add_prefix(SAMPLE_DIALOGS_SRC ../../src/generic/ fontdlgg.cpp)
    endif()
endif()
if(WXGTK2)
    wx_list_add_prefix(SAMPLE_DIALOGS_SRC ../../src/generic/ filedlgg.cpp)
endif()
wx_add_sample(dialogs ${SAMPLE_DIALOGS_SRC} DATA tips.txt)
wx_add_sample(dialup nettest.cpp LIBRARIES wxnet DEPENDS wxUSE_DIALUP_MANAGER)
wx_add_sample(display)
wx_add_sample(dnd dnd.cpp RES dnd.rc DATA wxwin.png DEPENDS wxUSE_DRAG_AND_DROP)
wx_add_sample(docview docview.cpp doc.cpp view.cpp docview.h doc.h view.h
    RES docview.rc PLIST Info.plist.in LIBRARIES wxaui DEPENDS wxUSE_DOC_VIEW_ARCHITECTURE)
wx_add_sample(dragimag dragimag.cpp dragimag.h RES dragimag.rc
    DATA backgrnd.png shape01.png shape02.png shape03.png
    DEPENDS wxUSE_DRAGIMAGE)
wx_add_sample(drawing DATA pat4.bmp pat35.bmp pat36.bmp image.bmp mask.bmp NAME drawingsample)
wx_add_sample(erase)
wx_add_sample(event event.cpp gestures.cpp gestures.h chessboard.cpp chessboard.h)
wx_add_sample(except DEPENDS wxUSE_EXCEPTIONS)
wx_add_sample(exec)
wx_add_sample(font DATA wxprivate.ttf)
wx_add_sample(fswatcher DEPENDS wxUSE_FSWATCHER)
wx_add_sample(grid griddemo.cpp griddemo.h DEPENDS wxUSE_GRID)

wx_list_add_prefix(HELP_DOC_FILES doc/
    aindex.html down.gif dxxgifs.tex HIER.html icon1.gif icon2.gif index.html
    logo.gif wx204.htm wx34.htm wxExtHelpController.html wxhelp.map wx.htm
    )
wx_add_sample(help demo.cpp LIBRARIES wxhtml
    DATA
    back.gif bullet.bmp contents.gif cshelp.txt doc.chm doc.cnt doc.hhc
    doc.hhk doc.hhp doc.hlp doc.hpj doc.zip forward.gif up.gif
    ${HELP_DOC_FILES}
    NAME helpdemo
    DEPENDS wxUSE_HELP
    )
wx_add_sample(htlbox LIBRARIES wxhtml DEPENDS wxUSE_HTML)
if(wxUSE_HTML)
    include(html.cmake)
endif()
wx_add_sample(image image.cpp canvas.cpp canvas.h RES image.rc
    DATA horse.png horse.jpg horse.bmp horse.gif horse.pcx horse.pnm
    horse_ag.pnm horse_rg.pnm horse.tif horse.tga horse.xpm horse.cur
    horse.ico horse3.ani horse.svg smile.xbm toucan.png cmyk.jpg cursor.png
    NAME imagesample DEPENDS wxUSE_IMAGE)
foreach(lang ar bg cs de fr it ka pl ru sv ja ja_JP.EUC-JP)
    list(APPEND INTERNAT_DATA_FILES ${lang}/internat.po ${lang}/internat.mo)
endforeach()
wx_add_sample(internat DATA ${INTERNAT_DATA_FILES} PLIST Info.plist.in DEPENDS wxUSE_INTL)
# IPC samples
set(wxSAMPLE_FOLDER ipc)
wx_add_sample(ipc client.cpp client.h connection.h ipcsetup.h NAME ipcclient LIBRARIES wxnet DEPENDS wxUSE_IPC)
wx_add_sample(ipc server.cpp server.h connection.h ipcsetup.h NAME ipcserver LIBRARIES wxnet DEPENDS wxUSE_IPC)
wx_add_sample(ipc CONSOLE baseclient.cpp connection.h ipcsetup.h NAME baseipcclient LIBRARIES wxnet DEPENDS wxUSE_IPC)
wx_add_sample(ipc CONSOLE baseserver.cpp connection.h ipcsetup.h NAME baseipcserver LIBRARIES wxnet DEPENDS wxUSE_IPC)
set(wxSAMPLE_FOLDER)
wx_add_sample(joytest joytest.cpp joytest.h DATA buttonpress.wav DEPENDS wxUSE_JOYSTICK)
wx_add_sample(keyboard)
wx_add_sample(layout layout.cpp layout.h)
wx_add_sample(listctrl listtest.cpp listtest.h RES listtest.rc DEPENDS wxUSE_LISTCTRL)
wx_add_sample(mdi mdi.cpp mdi.h RES mdi.rc DEPENDS wxUSE_MDI wxUSE_DOC_VIEW_ARCHITECTURE wxUSE_MDI_ARCHITECTURE)
wx_add_sample(mediaplayer LIBRARIES wxmedia DEPENDS wxUSE_MEDIACTRL)
wx_add_sample(menu DEPENDS wxUSE_MENUS wxUSE_MENUBAR)
wx_add_sample(minimal IMPORTANT)
wx_add_sample(notebook notebook.cpp notebook.h LIBRARIES wxaui DEPENDS wxUSE_NOTEBOOK)
if(wxUSE_OPENGL)
    set(wxSAMPLE_SUBDIR opengl/)
    set(wxSAMPLE_FOLDER OpenGL)
    wx_add_sample(cube cube.cpp cube.h LIBRARIES wxgl)
    wx_add_sample(isosurf isosurf.cpp isosurf.h LIBRARIES wxgl DATA isosurf.dat.gz)
    wx_add_sample(penguin
        penguin.cpp dxfrenderer.cpp trackball.c
        dxfrenderer.h penguin.h trackball.h
        LIBRARIES wxgl
        DATA penguin.dxf.gz)
    wx_add_sample(pyramid
        pyramid.cpp oglstuff.cpp mathstuff.cpp oglpfuncs.cpp
        pyramid.h oglstuff.h mathstuff.h oglpfuncs.h
        LIBRARIES wxgl)
    set(wxSAMPLE_SUBDIR)
    set(wxSAMPLE_FOLDER)
endif()
wx_add_sample(ownerdrw RES ownerdrw.rc DATA sound.png nosound.png DEPENDS wxUSE_OWNER_DRAWN)
wx_add_sample(popup DEPENDS wxUSE_POPUPWIN)
wx_add_sample(power)
wx_add_sample(preferences DEPENDS wxUSE_PREFERENCES_EDITOR)
wx_add_sample(printing printing.cpp printing.h DEPENDS wxUSE_PRINTING_ARCHITECTURE)
wx_add_sample(propgrid propgrid.cpp propgrid_minimal.cpp sampleprops.cpp
    sampleprops.h propgrid.h LIBRARIES wxpropgrid wxxrc NAME propgriddemo DEPENDS wxUSE_PROPGRID)
wx_add_sample(render FOLDER render)
wx_add_sample(render DLL renddll.cpp NAME renddll FOLDER render)
wx_add_sample(ribbon ribbondemo.cpp LIBRARIES wxribbon NAME ribbondemo DEPENDS wxUSE_RIBBON)
wx_add_sample(richtext LIBRARIES wxrichtext wxhtml wxxml NAME richtextdemo DEPENDS wxUSE_XML wxUSE_RICHTEXT)
wx_add_sample(sashtest sashtest.cpp sashtest.h RES sashtest.rc DEPENDS wxUSE_SASH)
wx_add_sample(scroll)
wx_add_sample(secretstore CONSOLE DEPENDS wxUSE_SECRETSTORE)
wx_add_sample(shaped DATA star.png)
if(wxUSE_SOCKETS)
    wx_add_sample(sockets client.cpp NAME client LIBRARIES wxnet FOLDER sockets)
    wx_add_sample(sockets server.cpp NAME server LIBRARIES wxnet FOLDER sockets)
    wx_add_sample(sockets CONSOLE baseclient.cpp NAME baseclient LIBRARIES wxnet FOLDER sockets)
    wx_add_sample(sockets CONSOLE baseserver.cpp NAME baseserver LIBRARIES wxnet FOLDER sockets)
endif()
wx_add_sample(sound RES sound.rc DATA 9000g.wav cuckoo.wav doggrowl.wav tinkalink2.wav DEPENDS wxUSE_SOUND)
wx_add_sample(splash DATA splash.png press.mpg DEPENDS wxUSE_SPLASH)
if(TARGET splash AND wxUSE_MEDIACTRL)
    wx_exe_link_libraries(splash wxmedia)
endif()
wx_add_sample(splitter DEPENDS wxUSE_SPLITTER)
wx_add_sample(statbar DEPENDS wxUSE_STATUSBAR)
wx_add_sample(stc stctest.cpp edit.cpp prefs.cpp edit.h defsext.h prefs.h
    DATA stctest.cpp NAME stctest LIBRARIES wxstc DEPENDS wxUSE_STC)
wx_add_sample(svg svgtest.cpp RES svgtest.rc DEPENDS wxUSE_SVG)
wx_add_sample(taborder)
wx_add_sample(taskbar tbtest.cpp tbtest.h DATA info.svg DEPENDS wxUSE_TASKBARICON)
wx_add_sample(text DEPENDS wxUSE_TEXTCTRL)
wx_add_sample(thread DEPENDS wxUSE_THREADS)
wx_add_sample(toolbar RES toolbar.rc DEPENDS wxUSE_TOOLBAR)
wx_add_sample(treectrl treetest.cpp treetest.h DEPENDS wxUSE_TREECTRL)
wx_add_sample(treelist DEPENDS wxUSE_TREELISTCTRL)
wx_add_sample(typetest typetest.cpp typetest.h)
wx_add_sample(uiaction DEPENDS wxUSE_UIACTIONSIMULATOR)
wx_add_sample(validate validate.cpp validate.h DEPENDS wxUSE_VALIDATORS)
wx_add_sample(vscroll vstest.cpp)
if(wxUSE_WEBVIEW)
    include(webview.cmake)
endif()
wx_add_sample(webrequest LIBRARIES wxnet DEPENDS wxUSE_WEBREQUEST)
# widgets Sample
set(SAMPLE_WIDGETS_SRC
    activityindicator.cpp
    bmpcombobox.cpp
    button.cpp
    checkbox.cpp
    choice.cpp
    clrpicker.cpp
    combobox.cpp
    datepick.cpp
    dirctrl.cpp
    dirpicker.cpp
    editlbox.cpp
    filectrl.cpp
    filepicker.cpp
    fontpicker.cpp
    gauge.cpp
    headerctrl.cpp
    hyperlnk.cpp
    itemcontainer.cpp
    listbox.cpp
    native.cpp
    notebook.cpp
    odcombobox.cpp
    radiobox.cpp
    searchctrl.cpp
    slider.cpp
    spinbtn.cpp
    static.cpp
    statbmp.cpp
    textctrl.cpp
    timepick.cpp
    toggle.cpp
    widgets.cpp
    widgets.h
    itemcontainer.h
    )
if(APPLE)
    # The source file using native controls uses Cocoa under OS X, so it must
    # be compiled as Objective C++ which means it must have .mm extension.
    # But this would make it uncompilable under the other platforms and we
    # don't want to have two files with identical contents. Hence this hack:
    # we have native.mm which just includes native.cpp under OS X, while
    # elsewhere we just compile native.cpp directly.
    list(APPEND SAMPLE_WIDGETS_SRC native_wrapper.mm)
endif()
wx_list_add_prefix(WIDGETS_RC_FILES icons/
    activityindicator.xpm bmpbtn.xpm bmpcombobox.xpm button.xpm
    checkbox.xpm choice.xpm choicebk.xpm clrpicker.xpm
    combobox.xpm datepick.xpm dirctrl.xpm dirpicker.xpm
    filepicker.xpm fontpicker.xpm gauge.xpm header.xpm
    hyperlnk.xpm listbook.xpm listbox.xpm native.xpm
    notebook.xpm odcombobox.xpm radiobox.xpm scrolbar.xpm
    slider.xpm spinbtn.xpm statbmp.xpm statbox.xpm
    stattext.xpm text.xpm timepick.xpm toggle.xpm
    )
wx_add_sample(widgets IMPORTANT ${SAMPLE_WIDGETS_SRC}
    DATA ${WIDGETS_RC_FILES} textctrl.cpp ../image/toucan.png:toucan.png
    )
# includes needed by the native widget (like gtk.h)
target_include_directories(widgets PRIVATE ${wxTOOLKIT_INCLUDE_DIRS})
wx_add_sample(wizard DATA wiztest.svg wiztest2.svg DEPENDS wxUSE_WIZARDDLG)
wx_add_sample(wrapsizer)

wx_list_add_prefix(XRC_RC_FILES rc/
    aui.xpm aui.xrc
    artprov.xpm artprov.xrc basicdlg.xpm basicdlg2.xpm
    basicdlg.xrc controls.xpm controls.xrc custclas.xpm custclas2.xpm custclas.xrc
    derivdlg.xpm derivdlg2.xpm derivdlg.xrc fileopen.gif filesave.gif frame.xrc
    fuzzy.gif menu.xrc platform.xpm platform.xrc quotes.gif
    resource.xrc toolbar.xrc uncenter.xpm uncenter2.xpm
    objref.xrc objrefdlg.xpm
    uncenter.xrc update.gif
    variable.xpm variable.xrc
    variants.xpm variants.xrc
    throbber.gif stop.xpm
    stop_2x.xpm wxbanner.gif
    )
wx_add_sample(xrc
    xrcdemo.cpp
    myframe.cpp
    derivdlg.cpp
    custclas.cpp
    objrefdlg.cpp
    derivdlg.h
    xrcdemo.h
    myframe.h
    custclas.h
    objrefdlg.h
    DATA ${XRC_RC_FILES}
    LIBRARIES wxaui wxribbon wxxrc wxhtml
    NAME xrcdemo
    DEPENDS wxUSE_XML wxUSE_XRC
    )
wx_add_sample(xti xti.cpp classlist.cpp codereadercallback.cpp
    classlist.h codereadercallback.h LIBRARIES wxxml
    DEPENDS wxUSE_XML wxUSE_EXTENDED_RTTI)

if(WIN32)
    # Windows only samples

    # DLL Sample
    if(wxUSE_DYNLIB_CLASS)
        wx_add_sample(dll DLL my_dll.cpp my_dll.h NAME my_dll FOLDER dll
            DEFINITIONS MY_DLL_BUILDING)
        if(NOT wxBUILD_SHARED)
            # this test only makes sense with statically built wx, otherwise
            # the same copy of wx would be used
            wx_add_sample(dll wx_exe.cpp my_dll.h NAME wx_exe FOLDER dll)
            wx_link_sample_libraries(wx_exe my_dll)
        endif()

        wx_add_sample(dll sdk_exe.cpp my_dll.h NAME sdk_exe FOLDER dll)
        wx_link_sample_libraries(sdk_exe my_dll)
    endif()

    wx_add_sample(regtest RES regtest.rc DEPENDS wxUSE_REGKEY)
    wx_add_sample(oleauto DEPENDS wxUSE_OLE)

    if(WXMSW)
        wx_add_sample(nativdlg nativdlg.cpp nativdlg.h resource.h RES nativdlg.rc)
        #TODO: reenable when sample is fixed
        #wx_add_sample(mfc mfctest.cpp mfctest.h resource.h stdafx.h RES mfctest.rc)
        wx_add_sample(taskbarbutton DEPENDS wxUSE_TASKBARBUTTON)
    endif()
endif()
