# Microsoft Developer Studio Project File - Name="wxUniv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxUniv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxUniv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxUniv.mak" CFG="wxUniv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxUniv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxUniv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxUniv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../UnivRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../UnivRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MT" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/univ" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /D "WIN32" /D "NDEBUG" /D WINVER=0x0400 /D "STRICT" /D "__WXUNIVERSAL__" /Yu"wx/wxprec.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxuniv.lib"

!ELSEIF  "$(CFG)" == "wxUniv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../UnivDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../UnivDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/univd" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /D "WIN32" /D "_DEBUG" /D "__WXDEBUG__" /D WINVER=0x0400 /D "STRICT" /D "__WXUNIVERSAL__" /Yu"wx/wxprec.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxunivd.lib"

!ENDIF 

# Begin Target

# Name "wxUniv - Win32 Release"
# Name "wxUniv - Win32 Debug"
# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common\accesscmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\appcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\artprov.cpp
# End Source File
# Begin Source File

SOURCE=.\common\artstd.cpp
# End Source File
# Begin Source File

SOURCE=.\common\bmpbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\choiccmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\clipcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\clntdata.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmdline.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmdproc.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmndata.cpp
# End Source File
# Begin Source File

SOURCE=.\common\config.cpp
# End Source File
# Begin Source File

SOURCE=.\common\containr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cshelp.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ctrlsub.cpp
# End Source File
# Begin Source File

SOURCE=.\common\datacmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\datetime.cpp
# End Source File
# Begin Source File

SOURCE=.\common\datstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\db.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dbgrid.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dbtable.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dcbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dircmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dndcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dobjcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\docmdi.cpp
# End Source File
# Begin Source File

SOURCE=.\common\docview.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dpycmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dseldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dynarray.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dynlib.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dynload.cpp
# End Source File
# Begin Source File

SOURCE=.\common\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\common\encconv.cpp
# End Source File
# Begin Source File

SOURCE=.\common\event.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fddlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ffile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\file.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fileconf.cpp
# End Source File
# Begin Source File

SOURCE=.\common\filefn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\filename.cpp
# End Source File
# Begin Source File

SOURCE=.\common\filesys.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fldlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fontcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fontmap.cpp
# End Source File
# Begin Source File

SOURCE=.\common\framecmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fs_inet.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fs_mem.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fs_zip.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ftp.cpp
# End Source File
# Begin Source File

SOURCE=.\common\gaugecmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\gdicmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\geometry.cpp
# End Source File
# Begin Source File

SOURCE=.\common\gifdecod.cpp
# End Source File
# Begin Source File

SOURCE=.\common\hash.cpp
# End Source File
# Begin Source File

SOURCE=.\common\hashmap.cpp
# End Source File
# Begin Source File

SOURCE=.\common\helpbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\http.cpp
# End Source File
# Begin Source File

SOURCE=.\common\iconbndl.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagall.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagbmp.cpp
# End Source File
# Begin Source File

SOURCE=.\common\image.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagfill.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imaggif.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagiff.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagjpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagpcx.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagpng.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagpnm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagtiff.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagxpm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\intl.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ipcbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\layout.cpp
# End Source File
# Begin Source File

SOURCE=.\common\lboxcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\list.cpp
# End Source File
# Begin Source File

SOURCE=.\common\log.cpp
# End Source File
# Begin Source File

SOURCE=.\common\longlong.cpp
# End Source File
# Begin Source File

SOURCE=.\common\matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\common\memory.cpp
# End Source File
# Begin Source File

SOURCE=.\common\menucmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mimecmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\module.cpp
# End Source File
# Begin Source File

SOURCE=.\common\msgout.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mstream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\nbkbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\object.cpp
# End Source File
# Begin Source File

SOURCE=.\common\paper.cpp
# End Source File
# Begin Source File

SOURCE=.\common\popupcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\prntbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\process.cpp
# End Source File
# Begin Source File

SOURCE=.\common\protocol.cpp
# End Source File
# Begin Source File

SOURCE=.\common\quantize.cpp
# End Source File
# Begin Source File

SOURCE=.\common\radiocmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\regex.cpp
# End Source File
# Begin Source File

SOURCE=.\common\rgncmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sckaddr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sckfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sckipc.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sckstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\settcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sizer.cpp
# End Source File
# Begin Source File

SOURCE=.\common\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\common\statbar.cpp
# End Source File
# Begin Source File

SOURCE=.\common\strconv.cpp
# End Source File
# Begin Source File

SOURCE=.\common\stream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\string.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sysopt.cpp
# End Source File
# Begin Source File

SOURCE=.\common\taskbarcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\tbarbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\timercmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\tokenzr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\toplvcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\treebase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\txtstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\url.cpp
# End Source File
# Begin Source File

SOURCE=.\common\utilscmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\valgen.cpp
# End Source File
# Begin Source File

SOURCE=.\common\validate.cpp
# End Source File
# Begin Source File

SOURCE=.\common\valtext.cpp
# End Source File
# Begin Source File

SOURCE=.\common\variant.cpp
# End Source File
# Begin Source File

SOURCE=.\common\wfstream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\wincmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\wxchar.cpp
# End Source File
# Begin Source File

SOURCE=.\common\xpmdecod.cpp
# End Source File
# Begin Source File

SOURCE=.\common\zipstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\zstream.cpp
# End Source File

# Begin Source File

SOURCE=.\common\extended.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\common\unzip.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File

# End Group
# Begin Group "Generic Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\generic\accel.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\busyinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\calctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\choicdgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\colrdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\dcbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\dcpsg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\dirctrlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\dragimgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\fdrepdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\filedlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\fontdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\grid.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\gridctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\gridsel.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\helpext.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\imaglist.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\laywin.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\listctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\logg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\mdig.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\msgdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\numdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\panelg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\printps.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\prntdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\progdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\sashwin.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\scrlwing.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\selstore.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\spinctlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\splash.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\tabg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\tbarsmpl.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\textdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\tipdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\tipwin.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\treectlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\wizard.cpp
# End Source File

# End Group
# Begin Group "wxHTML Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\html\helpctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\html\helpdata.cpp
# End Source File
# Begin Source File

SOURCE=.\html\helpfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmlcell.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmlfilt.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmlpars.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmltag.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmlwin.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmprint.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_dflist.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_fonts.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_hline.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_image.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_layout.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_links.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_list.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_pre.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_style.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_tables.cpp
# End Source File
# Begin Source File

SOURCE=.\html\winpars.cpp
# End Source File

# End Group
# Begin Group "MSW Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\msw\dummy.cpp
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=.\msw\app.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\brush.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\caret.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\clipbrd.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\colour.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\data.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dc.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dcclient.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dcmemory.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dcprint.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dcscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dde.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dialup.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dib.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dir.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dirdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\display.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\enhmeta.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\evtloop.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\font.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\fontenum.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\fontutil.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\gdiimage.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\gdiobj.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\glcanvas.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\helpbest.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\helpchm.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\helpwin.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\icon.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\joystick.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\main.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\mimetype.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\minifram.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\palette.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\pen.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\popupwin.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\regconf.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\region.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\settings.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\snglinst.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\taskbar.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\thread.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\tooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\toplevel.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\utilsexc.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\volume.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\wave.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\window.cpp
# End Source File

# Begin Source File

SOURCE=.\msw\gsocket.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\msw\gsockmsw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File

# Begin Group "OLE Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\msw\ole\automtn.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\dataobj.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\dropsrc.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\droptgt.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\oleutils.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\uuid.cpp
# End Source File

# End Group
# End Group
# Begin Group "Universal Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\univ\bmpbuttn.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\button.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\checkbox.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\checklst.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\choice.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\colschem.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\combobox.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\control.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\framuniv.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\gauge.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\inpcons.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\inphand.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\notebook.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\radiobox.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\radiobut.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\scrarrow.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\scrolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\scrthumb.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\slider.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\spinbutt.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\statbmp.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\statbox.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\statline.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\stattext.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\statusbr.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\textctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\theme.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\toolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\topluniv.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\winuniv.cpp
# End Source File

# Begin Group "Theme Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\univ\themes\gtk.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\themes\metal.cpp
# End Source File
# Begin Source File

SOURCE=.\univ\themes\win32.cpp
# End Source File

# End Group
# End Group
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Group "Setup"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\univ\setup.h
!IF  "$(CFG)" == "wxUniv - Win32 Release"
# Begin Custom Build - Creating lib\univ\wx\setup.h from $(InputPath)
InputPath=..\include\wx\univ\setup.h

"../lib/univ/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\univ\wx\setup.h

# End Custom Build
!ELSEIF  "$(CFG)" == "wxUniv - Win32 Debug"
# Begin Custom Build - Creating lib\univd\wx\setup.h from $(InputPath)
InputPath=..\include\wx\univ\setup.h

"../lib/univd/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\univd\wx\setup.h

# End Custom Build
!ENDIF 
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\accel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\access.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\app.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\artprov.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\brush.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\buffer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\build.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\busyinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\button.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\calctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\caret.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\checklst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\chkconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\choicdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\choice.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\clipbrd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\clntdata.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cmdline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cmdproc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cmndata.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\colordlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\colour.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\combobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\confbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\config.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\containr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\control.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cshelp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ctrlsub.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cursor.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dataobj.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\datetime.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\datetime.inl
# End Source File
# Begin Source File

SOURCE=..\include\wx\datstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\db.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbgrid.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbkeyg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbtable.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcbuffer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcclient.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcmemory.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcprint.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcps.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcscreen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dde.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\debug.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\defs.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dialog.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dialup.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dir.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dirctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dirdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\display.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dnd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\docmdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\docview.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dragimag.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dynarray.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dynlib.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dynload.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\effects.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\encconv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\event.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fdrepdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\features.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ffile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\file.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fileconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filefn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filename.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filesys.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\font.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontenc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontenum.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontutil.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\frame.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fs_inet.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fs_mem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fs_zip.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gauge.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gdicmn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gdiobj.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\geometry.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gifdecod.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\glcanvas.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\grid.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gsocket.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\hash.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\hashmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\help.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\helpbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\helpwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\icon.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\iconbndl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\image.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imaggif.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagiff.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagjpeg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imaglist.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagpcx.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagpng.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagpnm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagtiff.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagxpm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\intl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\iosfwrap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ioswrap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ipc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ipcbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\isql.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\isqlext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\joystick.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\layout.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\laywin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\list.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\listbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\listbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\listctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\log.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\longlong.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\math.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\matrix.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\memconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\memory.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\memtext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\menu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\metafile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\minifram.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\module.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msgdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msgout.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mstream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\notebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\object.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ownerdrw.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\palette.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\panel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\paper.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\pen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\platform.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\popupwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\print.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\printdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\prntbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\process.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\progdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\quantize.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\rawbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\regex.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\region.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sashwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sckaddr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sckipc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sckstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\scrolwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\selstore.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\settings.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sizer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\slider.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\snglinst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\socket.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\spinbutt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\spinctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\splash.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\splitter.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stack.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stattext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statusbr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\strconv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\string.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sysopt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tab.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tabctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\taskbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tbarbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tbarsmpl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textbuf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textfile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\thread.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\timer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tipdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tipwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tokenzr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\toolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tooltip.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\toplevel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\treebase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\treectrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\txtstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\types.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\url.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\utils.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\valgen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\validate.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\valtext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\variant.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\vector.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\version.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\volume.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wave.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wfstream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\window.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wizard.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wx.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wx_cw.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wx_cw_cm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wx_cw_d.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxchar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxhtml.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxprec.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xpmdecod.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\zipstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\zstream.h
# End Source File

# End Group
# Begin Group "MSW"

# PROP Default_Filter ""
# End Group
# Begin Group "Generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\generic\accel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\calctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\caret.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\choicdgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\colrdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dcpsg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dirctrlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dirdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dragimgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\fdrepdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\filedlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\fontdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\grid.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\gridctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\gridg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\helpext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\imaglist.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\laywin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\listctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\mdig.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\msgdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\notebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\paletteg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\panelg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\printps.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\prntdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\progdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\sashwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\scrolwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\spinctlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\splash.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\splitter.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\statline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\statusbr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\tabg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\textdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\timer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\treectlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\wizard.h
# End Source File

# End Group
# Begin Group "HTML"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\html\forcelnk.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpdata.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpfrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlcell.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmldefs.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlfilt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlpars.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmltag.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmprint.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\m_templ.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\winpars.h
# End Source File

# End Group
# Begin Group "Universal"

# PROP Default_Filter ""
# End Group
# End Group
# End Target
# End Project
