# Microsoft Developer Studio Project File - Name="gizmos_gizmos_xrc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gizmos_xrc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gizmos_gizmos_xrc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gizmos_gizmos_xrc.mak" CFG="gizmos_xrc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gizmos_xrc - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos_xrc - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos_xrc - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos_xrc - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos_xrc - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos_xrc - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos_xrc - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos_xrc - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos_xrc - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos_xrc - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos_xrc - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos_xrc - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos_xrc - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos_xrc - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos_xrc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos_xrc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gizmos_xrc - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivu" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_gizmos_xrcdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28u_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivu" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_gizmos_xrcdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28u_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivu" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv28u_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivu" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv28u_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28u_gizmos.lib wxmswuniv28u_xrc.lib wxbase28u_xml.lib wxmswuniv28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28u_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28u_gizmos_xrc.lib" /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28u_gizmos_xrc_vc_custom.pdb"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28u_gizmos.lib wxmswuniv28u_xrc.lib wxbase28u_xml.lib wxmswuniv28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28u_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28u_gizmos_xrc.lib" /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28u_gizmos_xrc_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivud" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_gizmos_xrcdll.pch" /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28ud_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivud" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_gizmos_xrcdll.pch" /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28ud_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivud" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv28ud_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivud" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv28ud_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28ud_gizmos.lib wxmswuniv28ud_xrc.lib wxbase28ud_xml.lib wxmswuniv28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28ud_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28ud_gizmos_xrc.lib" /debug /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28ud_gizmos_xrc_vc_custom.pdb"
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28ud_gizmos.lib wxmswuniv28ud_xrc.lib wxbase28ud_xml.lib wxmswuniv28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28ud_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28ud_gizmos_xrc.lib" /debug /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28ud_gizmos_xrc_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivdll\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswuniv" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_gizmos_xrcdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswuniv" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_gizmos_xrcdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswuniv" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv28_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswuniv" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv28_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28_gizmos.lib wxmswuniv28_xrc.lib wxbase28_xml.lib wxmswuniv28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28_gizmos_xrc.lib" /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28_gizmos_xrc_vc_custom.pdb"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28_gizmos.lib wxmswuniv28_xrc.lib wxbase28_xml.lib wxmswuniv28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28_gizmos_xrc.lib" /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28_gizmos_xrc_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivddll\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivd" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_gizmos_xrcdll.pch" /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28d_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivd" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_gizmos_xrcdll.pch" /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28d_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivd" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv28d_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswunivd" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv28d_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28d_gizmos.lib wxmswuniv28d_xrc.lib wxbase28d_xml.lib wxmswuniv28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28d_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28d_gizmos_xrc.lib" /debug /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28d_gizmos_xrc_vc_custom.pdb"
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28d_gizmos.lib wxmswuniv28d_xrc.lib wxbase28d_xml.lib wxmswuniv28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28d_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28d_gizmos_xrc.lib" /debug /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmswuniv28d_gizmos_xrc_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswu" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_gizmos_xrcdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28u_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswu" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_gizmos_xrcdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28u_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswu" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw28u_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswu" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw28u_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28u_gizmos.lib wxmsw28u_xrc.lib wxbase28u_xml.lib wxmsw28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28u_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28u_gizmos_xrc.lib" /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28u_gizmos_xrc_vc_custom.pdb"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28u_gizmos.lib wxmsw28u_xrc.lib wxbase28u_xml.lib wxmsw28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28u_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28u_gizmos_xrc.lib" /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28u_gizmos_xrc_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswud" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_gizmos_xrcdll.pch" /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28ud_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswud" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_gizmos_xrcdll.pch" /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28ud_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswud" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw28ud_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswud" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw28ud_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28ud_gizmos.lib wxmsw28ud_xrc.lib wxbase28ud_xml.lib wxmsw28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28ud_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28ud_gizmos_xrc.lib" /debug /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28ud_gizmos_xrc_vc_custom.pdb"
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28ud_gizmos.lib wxmsw28ud_xrc.lib wxbase28ud_xml.lib wxmsw28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28ud_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28ud_gizmos_xrc.lib" /debug /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28ud_gizmos_xrc_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswdll\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswdll\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\msw" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_gizmos_xrcdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\msw" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_gizmos_xrcdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\msw" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw28_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\msw" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw28_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28_gizmos.lib wxmsw28_xrc.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28_gizmos_xrc.lib" /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28_gizmos_xrc_vc_custom.pdb"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28_gizmos.lib wxmsw28_xrc.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28_gizmos_xrc.lib" /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28_gizmos_xrc_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswddll\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswddll\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswd" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_gizmos_xrcdll.pch" /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28d_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_dll\mswd" /I "..\..\src\gizmos\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_gizmos_xrcdll.pch" /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28d_gizmos_xrc_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswd" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw28d_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\gizmos\..\..\..\lib\vc_dll\mswd" /i "..\..\src\gizmos\..\..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw28d_gizmos_xrc_vc_custom /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28d_gizmos.lib wxmsw28d_xrc.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28d_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28d_gizmos_xrc.lib" /debug /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28d_gizmos_xrc_vc_custom.pdb"
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28d_gizmos.lib wxmsw28d_xrc.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28d_gizmos_xrc_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_dll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28d_gizmos_xrc.lib" /debug /pdb:"..\..\src\gizmos\..\..\..\lib\vc_dll\wxmsw28d_gizmos_xrc_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswunivu" /I "..\..\src\gizmos\..\..\..\include" /W4 /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28u_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswunivu" /I "..\..\src\gizmos\..\..\..\include" /W4 /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28u_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28u_gizmos_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28u_gizmos_xrc.lib"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswunivud" /I "..\..\src\gizmos\..\..\..\include" /W4 /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28ud_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswunivud" /I "..\..\src\gizmos\..\..\..\include" /W4 /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28ud_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28ud_gizmos_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28ud_gizmos_xrc.lib"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswuniv\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswuniv\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswuniv" /I "..\..\src\gizmos\..\..\..\include" /W4 /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswuniv" /I "..\..\src\gizmos\..\..\..\include" /W4 /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28_gizmos_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28_gizmos_xrc.lib"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivd\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivd\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswunivd" /I "..\..\src\gizmos\..\..\..\include" /W4 /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28d_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswunivd" /I "..\..\src\gizmos\..\..\..\include" /W4 /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28d_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28d_gizmos_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmswuniv28d_gizmos_xrc.lib"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswu" /I "..\..\src\gizmos\..\..\..\include" /W4 /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28u_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswu" /I "..\..\src\gizmos\..\..\..\include" /W4 /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28u_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28u_gizmos_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28u_gizmos_xrc.lib"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswud" /I "..\..\src\gizmos\..\..\..\include" /W4 /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28ud_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswud" /I "..\..\src\gizmos\..\..\..\include" /W4 /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28ud_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28ud_gizmos_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28ud_gizmos_xrc.lib"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_msw\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_msw\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\msw" /I "..\..\src\gizmos\..\..\..\include" /W4 /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\msw" /I "..\..\src\gizmos\..\..\..\include" /W4 /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28_gizmos_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28_gizmos_xrc.lib"

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswd\gizmos_xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswd\gizmos_xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswd" /I "..\..\src\gizmos\..\..\..\include" /W4 /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28d_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\src\gizmos\..\..\..\lib\vc_lib\mswd" /I "..\..\src\gizmos\..\..\..\include" /W4 /Zi /Fd..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28d_gizmos_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_gizmos_xrclib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28d_gizmos_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_lib\wxmsw28d_gizmos_xrc.lib"

!ENDIF

# Begin Target

# Name "gizmos_xrc - Win32 DLL Universal Unicode Release"
# Name "gizmos_xrc - Win32 DLL Universal Unicode Debug"
# Name "gizmos_xrc - Win32 DLL Universal Release"
# Name "gizmos_xrc - Win32 DLL Universal Debug"
# Name "gizmos_xrc - Win32 DLL Unicode Release"
# Name "gizmos_xrc - Win32 DLL Unicode Debug"
# Name "gizmos_xrc - Win32 DLL Release"
# Name "gizmos_xrc - Win32 DLL Debug"
# Name "gizmos_xrc - Win32 Universal Unicode Release"
# Name "gizmos_xrc - Win32 Universal Unicode Debug"
# Name "gizmos_xrc - Win32 Universal Release"
# Name "gizmos_xrc - Win32 Universal Debug"
# Name "gizmos_xrc - Win32 Unicode Release"
# Name "gizmos_xrc - Win32 Unicode Debug"
# Name "gizmos_xrc - Win32 Release"
# Name "gizmos_xrc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\gizmos\..\..\..\src\common\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=..\..\src\gizmos\..\..\..\src\msw\version.rc

!IF  "$(CFG)" == "gizmos_xrc - Win32 DLL Universal Unicode Release"


!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Universal Unicode Debug"


!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Universal Release"


!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Universal Debug"


!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Unicode Release"


!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Unicode Debug"


!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Universal Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Universal Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "gizmos_xrc - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\src\gizmos\xh_statpict.cpp
# End Source File
# End Group
# End Target
# End Project

