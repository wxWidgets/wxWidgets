# Microsoft Developer Studio Project File - Name="wxvc_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=wxvc_dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxvc_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxvc_dll.mak" CFG="wxvc_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxvc_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxvc_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wxvc_dll"
# PROP BASE Intermediate_Dir "wxvc_dll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "msw/ReleaseDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /Ob2 /I "../include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXMAKINGDLL=1 /Yu"wx/wxprec.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib rpcrt4.lib winmm.lib opengl32.lib glu32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../lib/wxdll.dll"

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wxvc_dl0"
# PROP BASE Intermediate_Dir "wxvc_dl0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "msw/DebugDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXMAKINGDLL=1 /Yu"wx/wxprec.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib rpcrt4.lib winmm.lib opengl32.lib glu32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../lib/wxdlld.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "wxvc_dll - Win32 Release"
# Name "wxvc_dll - Win32 Debug"
# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common\appcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\choiccmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\clipcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmndata.cpp
# End Source File
# Begin Source File

SOURCE=.\common\config.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ctrlsub.cpp
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

SOURCE=.\common\dbtable.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dcbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dlgcmn.cpp
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

SOURCE=.\common\dosyacc.c
# ADD CPP /D "USE_DEFINE" /D "YY_USE_PROTOS" /D "IDE_INVOKED"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\common\dynarray.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dynlib.cpp
# End Source File
# Begin Source File

SOURCE=.\common\encconv.cpp
# End Source File
# Begin Source File

SOURCE=.\common\event.cpp
# End Source File
# Begin Source File

SOURCE=.\common\extended.c
# SUBTRACT CPP /YX /Yc /Yu
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

SOURCE=.\common\filesys.cpp
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
# ADD CPP /I "zlib"
# End Source File
# Begin Source File

SOURCE=.\common\ftp.cpp
# End Source File
# Begin Source File

SOURCE=.\common\gdicmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\gifdecod.cpp
# End Source File
# Begin Source File

SOURCE=.\common\hash.cpp
# End Source File
# Begin Source File

SOURCE=.\common\helpbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\http.cpp
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

SOURCE=.\common\imaggif.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagjpeg.cpp
# ADD CPP /I "../src/jpeg"
# End Source File
# Begin Source File

SOURCE=.\common\imagpcx.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagpng.cpp
# ADD CPP /I "png" /I "zlib"
# End Source File
# Begin Source File

SOURCE=.\common\imagpnm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagtiff.cpp
# ADD CPP /I "tiff" /I "zlib"
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

SOURCE=.\common\mstream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\object.cpp
# End Source File
# Begin Source File

SOURCE=.\common\objstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\odbc.cpp
# End Source File
# Begin Source File

SOURCE=.\common\paper.cpp
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

SOURCE=.\common\resourc2.cpp
# End Source File
# Begin Source File

SOURCE=.\common\resource.cpp
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

SOURCE=.\common\serbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sizer.cpp
# End Source File
# Begin Source File

SOURCE=.\common\socket.cpp
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

SOURCE=.\common\tbarbase.cpp
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

SOURCE=.\common\txtstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\unzip.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
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

SOURCE=.\common\wxexpr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\zipstrm.cpp
# ADD CPP /I "zlib"
# End Source File
# Begin Source File

SOURCE=.\common\zstream.cpp
# End Source File
# End Group
# Begin Group "Generic Files"

# PROP Default_Filter ""
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

SOURCE=.\generic\dragimgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\grid.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\gridsel.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\helpext.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\helphtml.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\helpwxht.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\laywin.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\logg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\numdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\panelg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\progdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\prop.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\propform.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\proplist.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\sashwin.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\scrolwin.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\statusbr.cpp
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

SOURCE=.\generic\treelay.cpp
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

SOURCE=.\html\m_meta.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_pre.cpp
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

SOURCE=.\msw\accel.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\app.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\bmpbuttn.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\brush.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\button.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\caret.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\checkbox.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\checklst.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\choice.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\clipbrd.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\colordlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\colour.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\combobox.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\control.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\curico.cpp
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

SOURCE=.\msw\dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dialup.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dib.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dibutils.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dir.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dirdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dragimag.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dummy.cpp
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=.\msw\enhmeta.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\filedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\font.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\fontdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\fontenum.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\fontutil.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\frame.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\gauge95.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\gaugemsw.cpp
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

SOURCE=.\msw\gsocket.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\msw\gsockmsw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\msw\helpwin.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\icon.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\imaglist.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\iniconf.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\joystick.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\listctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\main.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\mdi.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\menuitem.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\metafile.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\mimetype.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\minifram.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\msgdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\nativdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\notebook.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ownerdrw.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\palette.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\pen.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\penwin.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\printdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\printwin.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\radiobox.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\radiobut.cpp
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

SOURCE=.\msw\scrolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\settings.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\slider95.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\slidrmsw.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\spinbutt.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\spinctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\statbmp.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\statbox.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\statbr95.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\statline.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\stattext.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\tabctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\taskbar.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\tbar95.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\textctrl.cpp
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

SOURCE=.\msw\treectrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\utilsexc.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\wave.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\window.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\xpmhand.cpp
# ADD CPP /I "../src/xpm"
# End Source File
# End Group
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
# Begin Group "PNG Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\png\png.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngerror.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngget.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngmem.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngpread.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngread.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngrio.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngrtran.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngrutil.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngset.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngtest.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngtrans.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngwio.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngwrite.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngwtran.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\png\pngwutil.c
# ADD CPP /I "zlib"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "XPM Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xpm\attrib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crbuffri.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crdatfri.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\create.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crifrbuf.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crifrdat.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\dataxpm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\hashtab.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\imagexpm.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\info.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\misc.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\parse.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\rdftodat.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\rdftoi.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\rgb.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\scan.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\simx.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\wrffrdat.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\wrffri.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Zlib Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\zlib\adler32.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\compress.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\crc32.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\deflate.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\gzio.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\infblock.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\infcodes.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\inffast.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\inflate.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\inftrees.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\infutil.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\trees.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\uncompr.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zlib\zutil.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Setup"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\msw\setup.h
# End Source File
# End Group
# Begin Group "JPEG Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\jpeg\jcapimin.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcapistd.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jccoefct.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jccolor.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcdctmgr.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jchuff.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcinit.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcmainct.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcmarker.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcmaster.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcomapi.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcparam.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcphuff.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcprepct.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jcsample.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jctrans.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdapimin.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdapistd.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdatadst.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdatasrc.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdcoefct.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdcolor.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jddctmgr.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdhuff.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdinput.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmainct.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmarker.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmaster.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdmerge.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdphuff.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdpostct.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdsample.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jdtrans.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jerror.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jfdctflt.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jfdctfst.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jfdctint.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctflt.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctfst.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctint.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jidctred.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jmemmgr.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jmemnobs.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jquant1.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jquant2.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jpeg\jutils.c

!IF  "$(CFG)" == "wxvc_dll - Win32 Release"

# ADD CPP /I "..\include" /I ".."
# SUBTRACT CPP /I "../include" /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxvc_dll - Win32 Debug"

# ADD CPP /I ".."
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "TIFF files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\tiff\tif_aux.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_close.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_codec.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_compress.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_dir.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_dirinfo.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_dirread.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_dirwrite.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_dumpmode.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_error.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_fax3.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_fax3sm.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_flush.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_getimage.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_jpeg.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_luv.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_lzw.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_next.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_open.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_packbits.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_pixarlog.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_predict.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_print.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_read.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_strip.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_swab.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_thunder.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_tile.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_version.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_warning.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_win32.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_write.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\tiff\tif_zip.c
# ADD CPP /I "tiff"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Target
# End Project
