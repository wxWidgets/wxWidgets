# Microsoft Developer Studio Project File - Name="wx_core" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=core - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_core.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_core.mak" CFG="core - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "core - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "core - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "core - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "core - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "core - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\core"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\core"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_coredll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw30u_core_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_coredll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw30u_core_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw30u_core_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw30u_core_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxbase30u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw30u_core_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw30u_core.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw30u_core_vc_custom.pdb"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxbase30u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw30u_core_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw30u_core.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw30u_core_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "core - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\core"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\core"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_coredll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw30ud_core_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_coredll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw30ud_core_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw30ud_core_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw30ud_core_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxbase30ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw30ud_core_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw30ud_core.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw30ud_core_vc_custom.pdb"
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxbase30ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw30ud_core_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw30ud_core.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw30ud_core_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "core - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\core"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\core"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw30u_core.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_corelib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw30u_core.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_corelib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D wxUSE_BASE=0 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw30u_core.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw30u_core.lib"

!ELSEIF  "$(CFG)" == "core - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\core"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\core"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw30ud_core.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_corelib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw30ud_core.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_corelib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D wxUSE_BASE=0 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw30ud_core.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw30ud_core.lib"

!ENDIF

# Begin Target

# Name "core - Win32 DLL Release"
# Name "core - Win32 DLL Debug"
# Name "core - Win32 Release"
# Name "core - Win32 Debug"
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\common\accelcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\accesscmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\affinematrix2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\anidecod.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\appcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\artprov.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\artstd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\arttango.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\bmpbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\bmpbtncmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\bookctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\btncmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\cairo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\checkboxcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\checklstcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\choiccmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\clipcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\clrpickercmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\cmdproc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\cmndata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\colourcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\colourdata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\combocmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\containr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\cshelp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\ctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\ctrlsub.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dcbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dcbufcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dcgraph.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dcsvg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dirctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dndcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dobjcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\docmdi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\docview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dpycmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dseldlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=..\..\src\common\effects.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\event.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fddlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\filectrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\filehistorycmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\filepickercmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fldlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fontcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fontdata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fontenumcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fontmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fontpickercmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fontutilcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\framecmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fs_mem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\gaugecmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\gbsizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\gdicmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\geometry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\gifdecod.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\graphcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\headercolcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\headerctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\helpbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\iconbndl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagall.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagbmp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagfill.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imaggif.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagiff.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagjpeg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagpcx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagpng.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagpnm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagtga.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagtiff.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\imagxpm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\layout.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\lboxcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\listctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\markupparser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\menucmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\modalhook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\mousemanager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\msgout.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\nbkbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\overlaycmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\ownerdrwcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\panelcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\paper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\persist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\pickerbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\popupcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\preferencescmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\prntbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\quantize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\radiobtncmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\radiocmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\rearrangectrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\rendcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\rgncmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\scrolbarcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\settcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\sizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\slidercmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\spinbtncmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\spinctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\srchcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\statbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\statbmpcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\statboxcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\statlinecmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\stattextcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\stockitem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\tbarbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\textcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\textentrycmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\textmeasurecmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\toplvcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\treebase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\uiactioncmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\utilscmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\valgen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\validate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\valnum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\valtext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\wincmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\windowid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\wrapsizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\xpmdecod.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\msw\accel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\ole\access.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\ole\activex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\anybutton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\app.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\artmsw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\ole\automtn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\bmpbuttn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\brush.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\button.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\caret.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\checkbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\checklst.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\choice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\clipbrd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\colordlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\colour.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\combo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\combobox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\control.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\cursor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\ole\dataobj.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dcclient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dcmemory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dcprint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dcscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dialup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dirdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\display.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dragimag.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\ole\dropsrc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\ole\droptgt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\enhmeta.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\evtloop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\fdrepdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\filedlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\font.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\fontdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\fontenum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\fontutil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\frame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\gauge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\gdiimage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\gdiobj.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\gdiplus.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\graphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\headerctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\helpchm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\helpwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\icon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\imaglist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\iniconf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\listbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\listctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\mdi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\menu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\menuitem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\metafile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\minifram.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\msgdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\mslu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\nativdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\nativewin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\nonownedwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\notebook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\ole\oleutils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\ownerdrw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\palette.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\panel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\pen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\popupwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\printdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\printwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\progdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\radiobox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\radiobut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\region.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\renderer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\richmsgdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\ole\safearray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\scrolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\slider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\spinbutt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\spinctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\statbmp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\statbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\statline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\stattext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\statusbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\textctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\textentry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\textmeasure.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\tglbtn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\toolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\tooltip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\toplevel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\treectrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\uiaction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\utilsgui.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\utilswin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\ole\uuid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\uxtheme.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\version.rc

!IF  "$(CFG)" == "core - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "core - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "core - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "core - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\src\msw\volume.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\window.cpp
# End Source File
# End Group
# Begin Group "Generic Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\generic\busyinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\buttonbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\choicbkg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\choicdgg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\clrpickerg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\collpaneg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\combog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\dcpsg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\dirctrlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\dragimgg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\filectrlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\filepickerg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\fontpickerg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\graphicc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\headerctrlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\infobar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\listbkg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\logg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\markuptext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\msgdlgg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\numdlgg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\preferencesg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\printps.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\prntdlgg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\progdlgg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\renderg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\richmsgdlgg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\scrlwing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\selstore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\spinctlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\splitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\srchctlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\statbmpg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\stattextg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\statusbr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\textdlgg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\tipwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\toolbkg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\treebkg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\treectlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\vlbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\vscroll.cpp
# End Source File
# End Group
# Begin Group "Setup Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\setup.h

!IF  "$(CFG)" == "core - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "core - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "core - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "core - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswud\wx\setup.h

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\univ\setup.h

!IF  "$(CFG)" == "core - Win32 DLL Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "core - Win32 DLL Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "core - Win32 Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "core - Win32 Debug"

# Begin Custom Build - 

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "MSW Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\accel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\access.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\activex.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\anybutton.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\app.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\automtn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\brush.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\button.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\calctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\caret.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\checklst.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\choice.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\clipbrd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\colordlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\colour.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\combo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\combobox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\commandlinkbutton.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\control.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ctrlsub.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\cursor.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\custombgwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\dataform.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\dataobj.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\dataobj2.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\datectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\datetimectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\dc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\dcclient.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\dcmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\dcprint.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\dcscreen.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\dialog.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\dib.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\dirdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\dragimag.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\dropsrc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\droptgt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\enhmeta.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\evtloop.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\fdrepdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\font.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\fontdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\frame.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\gauge.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\gdiimage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\genrcdefs.h

!IF  "$(CFG)" == "core - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "core - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "core - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "core - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\glcanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\headerctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\helpbest.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\helpchm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\helpwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\htmlhelp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\icon.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\imaglist.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\iniconf.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\init.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\joystick.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\listbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\listctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\mdi.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\menu.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\metafile.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\minifram.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\missing.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\msgdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\msvcrt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\nonownedwnd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\notebook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\notifmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\oleutils.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ownerdrw.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\palette.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\panel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\pen.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\popupwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\printdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\printwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\progdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\rcdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\region.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\richmsgdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\safearray.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\slider.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\sound.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\spinbutt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\spinctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\statbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\statline.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\stattext.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\statusbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\taskbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\textentry.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\timectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\toolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\tooltip.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\toplevel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\treectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\ole\uuid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\uxtheme.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\uxthemep.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\webview_ie.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\webview_missing.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\webviewhistoryitem_ie.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\window.h
# End Source File
# End Group
# Begin Group "Generic Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\generic\aboutdlgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\accel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\animate.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\busyinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\buttonbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\calctrlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\choicdgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\clrpickerg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\collpaneg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\colrdlgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\combo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\custombgwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\dataview.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\datectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\dcpsg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\dirctrlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\dragimgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\dvrenderer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\dvrenderers.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\filectrlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\filepickerg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\fontpickerg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\grid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\gridctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\grideditors.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\gridsel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\headerctrlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\helpext.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\infobar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\laywin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\logg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\msgdlgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\notebook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\notifmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\numdlgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\panelg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\printps.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\prntdlgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\progdlgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\propdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\richmsgdlgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\sashwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\scrolwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\spinctlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\splash.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\splitter.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\srchctlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\statbmpg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\stattextg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\textdlgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\timectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\treectlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\wizard.h
# End Source File
# End Group
# Begin Group "wxHTML Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\html\helpctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\helpdata.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\helpdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\helpfrm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\helpwnd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\htmlcell.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\htmldefs.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\htmlfilt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\htmlpars.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\htmlproc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\htmltag.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\htmlwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\htmprint.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\m_templ.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\styleparams.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\html\winpars.h
# End Source File
# End Group
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\aboutdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\accel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\access.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propgrid\advprops.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\affinematrix2d.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\affinematrix2dbase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\anidecod.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\animate.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\animdecod.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\anybutton.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ribbon\art.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ribbon\art_internal.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\artprov.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\aui\aui.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\aui\auibar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\aui\auibook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\bannerwindow.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ribbon\bar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\bookctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\persist\bookctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\brush.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\busyinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\button.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ribbon\buttonbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\calctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\caret.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\checklst.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\choicdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\choice.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\choicebk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\clipbrd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\clrpicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\cmdproc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\cmndata.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\collpane.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\colordlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\colour.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\colourdata.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\combo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\combobox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\commandlinkbutton.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\compositewin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\control.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ribbon\control.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\cshelp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ctrlsub.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\cursor.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\custombgwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dataobj.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dataview.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\datectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dateevt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\datetimectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcclient.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcgraph.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcmirror.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcprint.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcps.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcscreen.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcsvg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\debugrpt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dialog.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dialup.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dirctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dirdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\display.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\display_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dnd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\aui\dockart.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\docmdi.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\docview.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dragimag.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dvrenderers.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\editlbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propgrid\editors.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\effects.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\encinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fdrepdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\filectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\filehistory.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\filepicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\aui\floatpane.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fmappriv.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\font.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fontdata.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fontdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fontenum.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fontpicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fontutil.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\frame.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\aui\framemanager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ribbon\gallery.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\gauge.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\gbsizer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\gdicmn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\gdiobj.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\geometry.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\gifdecod.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\glcanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\graphics.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\grid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\headercol.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\headerctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\help.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\helpbase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\helphtml.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\helpwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\htmllbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\icon.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\iconbndl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imagbmp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\image.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imaggif.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imagiff.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imagjpeg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imaglist.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imagpcx.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imagpng.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imagpnm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imagtga.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imagtiff.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\imagxpm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\infobar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\itemid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\joystick.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\layout.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\laywin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\listbase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\listbook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\listbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\listctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propgrid\manager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\matrix.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\mdi.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\mediactrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\menu.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\metafile.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\minifram.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\modalhook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\mousemanager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msgdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\nativewin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\nonownedwnd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\notebook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\notifmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\numdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\odcombo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\overlay.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ownerdrw.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ribbon\page.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\palette.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\panel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ribbon\panel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\paper.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\pen.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\persist.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\pickerbase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\popupwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\position.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\preferences.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\print.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\printdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\prntbase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\progdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propgrid\property.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propgrid\propgrid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propgrid\propgriddefs.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propgrid\propgridiface.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propgrid\propgridpagestate.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propgrid\props.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\quantize.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\range.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\rawbmp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\rearrangectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\region.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\renderer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richmsgdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextbackgroundpage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextborderspage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextbulletspage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextdialogpage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextfontpage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextformatdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtexthtml.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextimagedlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextindentspage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextliststylepage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextmarginspage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextprint.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextsizepage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextstyledlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextstylepage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextstyles.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextsymboldlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtexttabspage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextuicustomization.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextxml.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtooltip.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\sashwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\scopeguard.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\scrolwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\selstore.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\settings.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\simplebook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\sizer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\slider.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\sound.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\spinbutt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\spinctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\splash.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\persist\splitter.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\splitter.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\srchctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\statbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\statline.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\stattext.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\statusbr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\stc\stc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\aui\tabart.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\aui\tabmdi.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\taskbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\tbarbase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\testing.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\textcompleter.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\textdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\textentry.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\textwrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\timectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\tipdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\tipwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ribbon\toolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\toolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\toolbook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\tooltip.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\persist\toplevel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\toplevel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\treebase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\persist\treebook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\treebook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\treectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\treelist.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\uiaction.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\valgen.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\validate.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\valnum.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\valtext.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\vidmode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\vlbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\vms_x_fix.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\vscroll.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\webview.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\webviewarchivehandler.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\webviewfshandler.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\persist\window.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\window.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\windowid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\windowptr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\withimages.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wizard.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wrapsizer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wupdlock.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wxhtml.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_all.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_animatctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_auinotbk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_bannerwindow.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_bmp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_bmpbt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_bttn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_cald.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_chckb.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_chckl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_choic.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_choicbk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_clrpicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_cmdlinkbn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_collpane.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_combo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_comboctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_datectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_dirpicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_dlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_editlbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_filectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_filepicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_fontpicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_frame.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_gauge.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_gdctl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_grid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_html.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_htmllbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_listb.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_listbk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_listc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_mdi.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_menu.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_notbk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_odcombo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_panel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_propdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_radbt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_radbx.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_ribbon.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_richtext.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_scrol.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_scwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_simplebook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_sizer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_slidr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_spin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_split.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_srchctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_statbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_stbmp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_stbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_stlin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_sttxt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_text.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_timectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_toolb.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_toolbk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_tree.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_treebk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_unkwn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_wizrd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xmlres.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xmlreshandler.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xpmdecod.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xpmhand.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\xrc\xmlreshandler.cpp
# End Source File
# End Group
# End Target
# End Project

