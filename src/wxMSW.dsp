# Microsoft Developer Studio Project File - Name="wxWindows" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxWindows - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxMSW.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxMSW.mak" CFG="wxWindows - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxWindows - Win32 Release Unicode DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxWindows - Win32 Debug Unicode DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxWindows - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxWindows - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxWindows - Win32 Release DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxWindows - Win32 Debug DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxWindows - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxWindows - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "wxWindows - Win32 Release Unicode DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../ReleaseUnicodeDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../ReleaseUnicodeDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WXWINDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/mswdllu" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /D "_USRDLL" /D "WIN32" /D "NDEBUG" /D WINVER=0x0400 /D "STRICT" /D "WXMAKINGDLL" /D "_UNICODE" /D "UNICODE" /Yu"wx/wxprec.h" /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "../include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\lib\jpeg.lib ..\lib\tiff.lib ..\lib\png.lib ..\lib\regex.lib ..\lib\zlib.lib /nologo /dll /machine:I386 /out:"../lib/wxmsw250u.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib ..\lib\jpeg.lib ..\lib\tiff.lib ..\lib\png.lib ..\lib\regex.lib ..\lib\zlib.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxmsw250u.dll"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug Unicode DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../DebugUnicodeDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../DebugUnicodeDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WXWINDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/mswdllud" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /D "_USRDLL" /D "WIN32" /D "_DEBUG" /D WINVER=0x0400 /D "STRICT" /D "WXMAKINGDLL" /D "_UNICODE" /D "UNICODE" /Yu"wx/wxprec.h" /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\lib\jpegd.lib ..\lib\tiffd.lib ..\lib\pngd.lib ..\lib\regexd.lib ..\lib\zlibd.lib /dll /debug /machine:I386 /out:"../lib/wxmsw250ud.dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib ..\lib\jpegd.lib ..\lib\tiffd.lib ..\lib\pngd.lib ..\lib\regexd.lib ..\lib\zlibd.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxmsw250ud.dll"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../ReleaseUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../ReleaseUnicode"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W4 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/mswu" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /D "WIN32" /D "NDEBUG" /D WINVER=0x0400 /D "STRICT" /D "_UNICODE" /D "UNICODE" /Yu"wx/wxprec.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxmswu.lib"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../DebugUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../DebugUnicode"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/mswud" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /D "WIN32" /D "_DEBUG" /D "__WXDEBUG__" /D WINVER=0x0400 /D "STRICT" /D "_UNICODE" /D "UNICODE" /Yu"wx/wxprec.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxmswud.lib"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../ReleaseDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WXWINDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/mswdll" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /D "_USRDLL" /D "WIN32" /D "NDEBUG" /D WINVER=0x0400 /D "STRICT" /D "WXMAKINGDLL" /Yu"wx/wxprec.h" /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "../include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\lib\jpeg.lib ..\lib\tiff.lib ..\lib\png.lib ..\lib\regex.lib ..\lib\zlib.lib /nologo /dll /machine:I386 /out:"../lib/wxmsw250.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib ..\lib\jpeg.lib ..\lib\tiff.lib ..\lib\png.lib ..\lib\regex.lib ..\lib\zlib.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxmsw250.dll"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../DebugDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WXWINDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/mswdlld" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /D "_USRDLL" /D "WIN32" /D "_DEBUG" /D WINVER=0x0400 /D "STRICT" /D "WXMAKINGDLL" /Yu"wx/wxprec.h" /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\lib\jpegd.lib ..\lib\tiffd.lib ..\lib\pngd.lib ..\lib\regexd.lib ..\lib\zlibd.lib /dll /debug /machine:I386 /out:"../lib/wxmsw250d.dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib ..\lib\jpegd.lib ..\lib\tiffd.lib ..\lib\pngd.lib ..\lib\regexd.lib ..\lib\zlibd.lib ../lib/wxbase250d.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxmsw250d.dll"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../Release"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W4 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/msw" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /D "WIN32" /D "NDEBUG" /D WINVER=0x0400 /D "STRICT" /Yu"wx/wxprec.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../Debug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/mswd" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /D "WIN32" /D "_DEBUG" /D "__WXDEBUG__" /D WINVER=0x0400 /D "STRICT" /Yu"wx/wxprec.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxmswd.lib"

!ENDIF 

# Begin Target

# Name "wxWindows - Win32 Release Unicode DLL"
# Name "wxWindows - Win32 Debug Unicode DLL"
# Name "wxWindows - Win32 Release Unicode"
# Name "wxWindows - Win32 Debug Unicode"
# Name "wxWindows - Win32 Release DLL"
# Name "wxWindows - Win32 Debug DLL"
# Name "wxWindows - Win32 Release"
# Name "wxWindows - Win32 Debug"
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

SOURCE=.\common\choiccmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\clipcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmdproc.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmndata.cpp
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

SOURCE=.\common\dbgrid.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dcbase.cpp
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

SOURCE=.\common\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\common\event.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fddlgcmn.cpp
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

SOURCE=.\common\fs_mem.cpp
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

SOURCE=.\common\helpbase.cpp
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

SOURCE=.\common\layout.cpp
# End Source File
# Begin Source File

SOURCE=.\common\lboxcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\common\menucmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\msgout.cpp
# End Source File
# Begin Source File

SOURCE=.\common\nbkbase.cpp
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

SOURCE=.\common\quantize.cpp
# End Source File
# Begin Source File

SOURCE=.\common\radiocmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\rgncmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\settcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sizer.cpp
# End Source File
# Begin Source File

SOURCE=.\common\statbar.cpp
# End Source File
# Begin Source File

SOURCE=.\common\taskbarcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\tbarbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\timercmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\toplvcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\treebase.cpp
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

SOURCE=.\common\wincmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\xpmdecod.cpp
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

SOURCE=.\generic\grid.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\gridctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\gridsel.cpp
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

SOURCE=.\generic\statusbr.cpp
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
# Begin Group "OLE Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\msw\ole\access.cpp
# End Source File
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

SOURCE=.\msw\dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dialup.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dib.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dirdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\display.cpp
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

SOURCE=.\msw\evtloop.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\fdrepdlg.cpp
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

SOURCE=.\msw\mslu.cpp
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

SOURCE=.\msw\popupwin.cpp
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

SOURCE=.\msw\region.cpp
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

SOURCE=.\msw\tglbtn.cpp
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

SOURCE=.\msw\treectrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\utilsgui.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\uxtheme.cpp
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
# End Group
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Group "Setup"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\msw\setup.h

!IF  "$(CFG)" == "wxWindows - Win32 Release Unicode DLL"

# Begin Custom Build - Creating ..\lib\mswdllu\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswdllu/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswdllu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug Unicode DLL"

# Begin Custom Build - Creating ..\lib\mswdllud\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswdllud/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswdllud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release Unicode"

# Begin Custom Build - Creating ..\lib\mswu\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswu/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug Unicode"

# Begin Custom Build - Creating ..\lib\mswud\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswud/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release DLL"

# Begin Custom Build - Creating ..\lib\mswdll\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswdll/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswdll\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug DLL"

# Begin Custom Build - Creating ..\lib\mswdlld\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswdlld/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswdlld\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release"

# Begin Custom Build - Creating ..\lib\msw\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/msw/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\msw\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug"

# Begin Custom Build - Creating ..\lib\mswd\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswd/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswd\wx\setup.h

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
# Begin Source File

SOURCE=..\include\wx\msw\accel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\access.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\app.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\brush.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\button.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\caret.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\checklst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\choice.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\clipbrd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\colordlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\colour.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\combobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\control.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\cursor.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcclient.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcmemory.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcprint.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcscreen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dde.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dialog.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dib.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dirdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dragimag.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\enhmeta.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\fdrepdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\font.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\fontdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\frame.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gauge95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gccpriv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gdiimage.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gdiobj.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\glcanvas.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\helpbest.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\helpchm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\helpwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\icon.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\imaglist.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\iniconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\joystick.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\listbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\listctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\mdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\menu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\metafile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\minifram.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\missing.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\msgdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\mslu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\msvcrt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\notebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\palette.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\pen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\printdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\printwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\private.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\regconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\region.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\registry.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\slider95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\slidrmsw.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\spinbutt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\spinctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statbr95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\stattext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tabctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\taskbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tbar95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tbarmsw.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\timer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tooltip.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\toplevel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\treectrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\wave.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\window.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\winundef.h
# End Source File
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
# End Group
# End Target
# End Project
