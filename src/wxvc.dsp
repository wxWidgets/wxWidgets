# Microsoft Developer Studio Project File - Name="wxvc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxvc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxvc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxvc.mak" CFG="wxvc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxvc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxvc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxvc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "../include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /Yu"wx/wxprec.h" /FD /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\wxvc.lib"

!ELSEIF  "$(CFG)" == "wxvc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /Yu"wx/wxprec.h" /FD /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\wxvc.lib"

!ENDIF 

# Begin Target

# Name "wxvc - Win32 Release"
# Name "wxvc - Win32 Debug"
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

SOURCE=.\common\date.cpp
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

SOURCE=.\common\imagpng.cpp
# ADD CPP /I "png" /I "zlib"
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

SOURCE=.\common\mimetype.cpp
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

SOURCE=.\common\tbarsmpl.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\time.cpp
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

!IF  "$(CFG)" == "wxvc - Win32 Release"

!ELSEIF  "$(CFG)" == "wxvc - Win32 Debug"

# SUBTRACT CPP /X

!ENDIF 

# End Source File
# End Group
# Begin Group "Generic Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\generic\busyinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\choicdgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\gridg.cpp
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

SOURCE=.\generic\textdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\tipdlg.cpp
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

SOURCE=.\msw\dib.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dibutils.cpp
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

SOURCE=.\msw\gsocket.c
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

SOURCE=.\msw\pnghand.cpp

!IF  "$(CFG)" == "wxvc - Win32 Release"

# ADD CPP /I "png" /I "zlib"

!ELSEIF  "$(CFG)" == "wxvc - Win32 Debug"

# ADD CPP /I "zlib" /I "png"

!ENDIF 

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

SOURCE=.\png\png.h
# End Source File
# Begin Source File

SOURCE=.\png\pngconf.h
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
# Begin Group "XPM files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\xpm\crbuffri.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crbuffrp.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crdatfri.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crdatfrp.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\create.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crifrbuf.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crifrdat.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crpfrbuf.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\crpfrdat.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\dataxpm.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\hashtab.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\misc.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\parse.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\rdftodat.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\rdftoi.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\rdftop.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\rgb.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\scan.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\simx.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\wrffrdat.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\wrffri.c
# ADD CPP /D "FOR_MSW"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\xpm\wrffrp.c
# ADD CPP /D "FOR_MSW"
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
# End Target
# End Project
