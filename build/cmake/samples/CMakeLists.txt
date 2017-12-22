#############################################################################
# Name:        build/cmake/samples/CMakeLists.txt
# Purpose:     CMake script to build samples
# Author:      Tobias Taschner
# Created:     2016-10-04
# Copyright:   (c) 2016 wxWidgets development team
# Licence:     wxWindows licence
#############################################################################

wx_add_sample(access accesstest.cpp)
wx_add_sample(animate anitest.cpp LIBRARIES adv DATA throbber.gif hourglass.ani)
wx_add_sample(artprov arttest.cpp artbrows.cpp artbrows.h)
wx_add_sample(aui auidemo.cpp LIBRARIES adv aui html NAME auidemo)
wx_add_sample(calendar LIBRARIES adv)
wx_add_sample(caret)
wx_add_sample(clipboard)
wx_add_sample(collpane LIBRARIES adv)
wx_add_sample(combo LIBRARIES adv DATA dropbuth.png dropbutn.png dropbutp.png)
wx_add_sample(config conftest.cpp)
wx_add_sample(console CONSOLE IMPORTANT)
wx_add_sample(dataview IMPORTANT dataview.cpp mymodels.cpp mymodels.h LIBRARIES adv)
wx_add_sample(debugrpt LIBRARIES qa)
wx_add_sample(dialogs dialogs.cpp dialogs.h LIBRARIES adv DATA tips.txt)
wx_add_sample(dialup nettest.cpp)
wx_add_sample(display)
wx_add_sample(dnd dnd.cpp RES dnd.rc DATA wxwin.png)
wx_add_sample(docview docview.cpp doc.cpp view.cpp docview.h doc.h view.h
    RES docview.rc)
wx_add_sample(dragimag dragimag.cpp dragimag.h RES dragimag.rc
    DATA backgrnd.png shape01.png shape02.png shape03.png)
wx_add_sample(drawing DATA pat4.bmp pat35.bmp pat36.bmp image.bmp mask.bmp)
wx_add_sample(erase)
wx_add_sample(event)
wx_add_sample(except)
wx_add_sample(exec)
wx_add_sample(font)
wx_add_sample(fswatcher)
wx_add_sample(grid griddemo.cpp griddemo.h LIBRARIES adv)

wx_list_add_prefix(HELP_DOC_FILES doc/
    aindex.html ClassGraph.class ClassGraphPanel.class ClassLayout.class
    down.gif dxxgifs.tex HIER.html HIERjava.html icon1.gif icon2.gif
    index.html logo.gif NavigatorButton.class USE_HELP.html wx204.htm
    wx34.htm wxExtHelpController.html wxhelp.map wx.htm
    )
wx_add_sample(help demo.cpp doc.h LIBRARIES html adv
    DATA
    back.gif bullet.bmp contents.gif cshelp.txt doc.chm doc.cnt doc.hhc
    doc.hhk doc.hhp doc.hlp doc.hpj doc.zip forward.gif up.gif
    ${HELP_DOC_FILES}
    NAME helpdemo
    )
wx_add_sample(htlbox LIBRARIES html)
include(html.cmake)
wx_add_sample(image image.cpp canvas.cpp canvas.h RES image.rc
    DATA horse.png horse.jpg horse.bmp horse.gif horse.pcx horse.pnm
    horse_ag.pnm horse_rg.pnm horse.tif horse.tga horse.xpm horse.cur
    horse.ico horse3.ani smile.xbm toucan.png cmyk.jpg cursor.png)
foreach(lang ar bg cs de fr it ka pl ru sv ja ja_JP.EUC-JP)
    list(APPEND INTERNAT_DATA_FILES ${lang}/internat.po ${lang}/internat.mo)
endforeach()
wx_add_sample(internat DATA ${INTERNAT_DATA_FILES})
# IPC samples
set(wxSAMPLE_FOLDER ipc)
wx_add_sample(ipc client.cpp client.h connection.h ipcsetup.h NAME ipcclient LIBRARIES net)
wx_add_sample(ipc server.cpp server.h connection.h ipcsetup.h NAME ipcserver LIBRARIES net)
wx_add_sample(ipc CONSOLE baseclient.cpp connection.h ipcsetup.h NAME baseipcclient LIBRARIES net)
wx_add_sample(ipc CONSOLE baseserver.cpp connection.h ipcsetup.h NAME baseipcserver LIBRARIES net)
set(wxSAMPLE_FOLDER)
wx_add_sample(joytest joytest.cpp joytest.h DATA buttonpress.wav LIBRARIES adv)
wx_add_sample(keyboard)
wx_add_sample(layout layout.cpp layout.h)
wx_add_sample(listctrl listtest.cpp listtest.h RES listtest.rc)
wx_add_sample(mdi mdi.cpp mdi.h RES mdi.rc)
wx_add_sample(mediaplayer LIBRARIES media DEPENDS wxUSE_MEDIACTRL)
wx_add_sample(memcheck)
wx_add_sample(menu)
wx_add_sample(minimal IMPORTANT)
wx_add_sample(notebook notebook.cpp notebook.h LIBRARIES aui adv)
if(wxUSE_OPENGL)
    set(wxSAMPLE_SUBDIR opengl/)
    set(wxSAMPLE_FOLDER OpenGL)
    wx_add_sample(cube cube.cpp cube.h LIBRARIES gl)
    wx_add_sample(isosurf isosurf.cpp isosurf.h LIBRARIES gl DATA isosurf.dat.gz)
    wx_add_sample(penguin
        penguin.cpp dxfrenderer.cpp trackball.c
        dxfrenderer.h penguin.h trackball.h
        LIBRARIES gl
        DATA penguin.dxf.gz)
    wx_add_sample(pyramid
        pyramid.cpp oglstuff.cpp mathstuff.cpp oglpfuncs.cpp
        pyramid.h oglstuff.h mathstuff.h oglpfuncs.h
        LIBRARIES gl)
    set(wxSAMPLE_SUBDIR)
    set(wxSAMPLE_FOLDER)
endif()
wx_add_sample(ownerdrw RES ownerdrw.rc DATA sound.png nosound.png DEPENDS wxUSE_OWNER_DRAWN)
wx_add_sample(popup)
wx_add_sample(power)
wx_add_sample(preferences)
wx_add_sample(printing printing.cpp printing.h)
wx_add_sample(propgrid propgrid.cpp propgrid_minimal.cpp sampleprops.cpp
    tests.cpp sampleprops.h propgrid.h LIBRARIES adv propgrid NAME propgriddemo)
wx_add_sample(render FOLDER render)
wx_add_sample(render DLL renddll.cpp NAME renddll FOLDER render)
wx_add_sample(ribbon ribbondemo.cpp LIBRARIES ribbon adv NAME ribbondemo)
wx_add_sample(richtext LIBRARIES richtext adv html xml NAME richtextdemo)
wx_add_sample(sashtest sashtest.cpp sashtest.h LIBRARIES adv)
wx_add_sample(scroll)
wx_add_sample(secretstore CONSOLE DEPENDS wxUSE_SECRETSTORE)
wx_add_sample(shaped DATA star.png)
wx_add_sample(sockets client.cpp NAME client LIBRARIES net FOLDER sockets)
wx_add_sample(sockets server.cpp NAME server LIBRARIES net FOLDER sockets)
wx_add_sample(sockets CONSOLE baseclient.cpp NAME baseclient LIBRARIES net FOLDER sockets)
wx_add_sample(sockets CONSOLE baseserver.cpp NAME baseserver LIBRARIES net FOLDER sockets)
wx_add_sample(sound RES sound.rc DATA 9000g.wav cuckoo.wav doggrowl.wav tinkalink2.wav LIBRARIES adv)
wx_add_sample(splash DATA splash.png press.mpg LIBRARIES adv)
if(TARGET splash AND wxUSE_MEDIACTRL)
    target_link_libraries(splash media)
endif()
wx_add_sample(splitter)
wx_add_sample(statbar)
wx_add_sample(stc stctest.cpp edit.cpp prefs.cpp edit.h defsext.h prefs.h
    DATA stctest.cpp NAME stctest LIBRARIES stc)
wx_add_sample(svg svgtest.cpp)
wx_add_sample(taborder)
wx_add_sample(taskbar tbtest.cpp tbtest.h LIBRARIES adv DEPENDS wxUSE_TASKBARICON)
wx_add_sample(text)
wx_add_sample(thread)
wx_add_sample(toolbar RES toolbar.rc)
wx_add_sample(treectrl treetest.cpp treetest.h)
wx_add_sample(treelist LIBRARIES adv)
wx_add_sample(typetest typetest.cpp typetest.h)
wx_add_sample(uiaction)
wx_add_sample(validate)
wx_add_sample(vscroll vstest.cpp)
wx_add_sample(webview LIBRARIES webview stc adv NAME webviewsample)
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
wx_add_sample(widgets IMPORTANT ${SAMPLE_WIDGETS_SRC} LIBRARIES adv)
wx_add_sample(wizard LIBRARIES adv)
wx_add_sample(wrapsizer)

wx_list_add_prefix(XRC_RC_FILES rc/
    aui.xpm aui.xrc
    artprov.xpm artprov.xrc basicdlg.xpm
    basicdlg.xrc controls.xpm controls.xrc custclas.xpm custclas.xrc
    derivdlg.xpm derivdlg.xrc fileopen.gif filesave.gif frame.xrc
    fuzzy.gif menu.xrc platform.xpm platform.xrc quotes.gif
    resource.xrc toolbar.xrc uncenter.xpm
    objref.xrc objrefdlg.xpm
    uncenter.xrc update.gif
    variable.xpm variable.xrc
    variants.xpm variants.xrc
    throbber.gif stop.xpm
    wxbanner.gif
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
    ${XRC_RC_FILES}
    LIBRARIES aui ribbon xrc html adv
    NAME xrcdemo
    )
wx_add_sample(xti xti.cpp classlist.cpp codereadercallback.cpp LIBRARIES xml
    DEPENDS wxUSE_EXTENDED_RTTI)

if(WIN32)
    # Windows only samples

    # DLL Sample
    wx_add_sample(dll DLL my_dll.cpp NAME my_dll FOLDER dll
        DEFINITIONS MY_DLL_BUILDING)
    if(NOT wxBUILD_SHARED)
        # this test only makes sense with statically built wx, otherwise
        # the same copy of wx would be used
        wx_add_sample(dll wx_exe.cpp NAME wx_exe FOLDER dll LIBRARIES my_dll)
    endif()

    wx_add_sample(dll sdk_exe.cpp NAME sdk_exe FOLDER dll LIBRARIES my_dll)
    wx_add_sample(flash)
    #TODO: renable when sample is fixed
    #wx_add_sample(mfc mfctest.cpp mfctest.h resource.h stdafx.h RES mfctest.rc)
    wx_add_sample(nativdlg nativdlg.cpp nativdlg.h resource.h RES nativdlg.rc)
    wx_add_sample(oleauto)
    wx_add_sample(regtest RES regtest.rc)
    wx_add_sample(taskbarbutton LIBRARIES adv DEPENDS wxUSE_TASKBARBUTTON)
endif()
