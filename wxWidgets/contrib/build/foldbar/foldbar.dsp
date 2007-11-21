# Microsoft Developer Studio Project File - Name="foldbar" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=foldbar - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "foldbar.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "foldbar.mak" CFG="foldbar - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "foldbar - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foldbar - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foldbar - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foldbar - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foldbar - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foldbar - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foldbar - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foldbar - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "foldbar - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "foldbar - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "foldbar - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "foldbar - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "foldbar - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "foldbar - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "foldbar - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "foldbar - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "foldbar - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivu" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_foldbardll.pch" /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28u_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivu" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_foldbardll.pch" /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28u_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivu" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmswuniv28u_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivu" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmswuniv28u_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28u_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28u_foldbar.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28u_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28u_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivud" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_foldbardll.pch" /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28ud_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivud" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_foldbardll.pch" /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28ud_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivud" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmswuniv28ud_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivud" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmswuniv28ud_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28ud_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28ud_foldbar.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28ud_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28ud_foldbar.lib" /debug

!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivdll\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswuniv" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_foldbardll.pch" /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswuniv" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_foldbardll.pch" /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswuniv" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmswuniv28_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswuniv" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmswuniv28_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28_foldbar.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivddll\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivd" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_foldbardll.pch" /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28d_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivd" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_foldbardll.pch" /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28d_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivd" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmswuniv28d_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswunivd" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmswuniv28d_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28d_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28d_foldbar.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28d_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmswuniv28d_foldbar.lib" /debug

!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswu" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_foldbardll.pch" /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28u_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswu" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_foldbardll.pch" /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28u_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswu" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmsw28u_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswu" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmsw28u_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28u_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28u_foldbar.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28u_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28u_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswud" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_foldbardll.pch" /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28ud_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswud" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_foldbardll.pch" /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28ud_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswud" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmsw28ud_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswud" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmsw28ud_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28ud_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28ud_foldbar.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28ud_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28ud_foldbar.lib" /debug

!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswdll\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswdll\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\msw" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_foldbardll.pch" /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\msw" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_foldbardll.pch" /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\msw" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmsw28_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
# ADD RSC /l 0x409 /d "__WXMSW__" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\msw" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmsw28_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28_foldbar.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswddll\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswddll\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswd" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_foldbardll.pch" /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28d_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_dll\mswd" /I "..\..\src\foldbar\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_foldbardll.pch" /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28d_foldbar_vc_custom.pdb /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FOLDBAR" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswd" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmsw28d_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\foldbar\..\..\..\lib\vc_dll\mswd" /i "..\..\src\foldbar\..\..\..\include" /d WXDLLNAME=wxmsw28d_foldbar_vc_custom /i "..\..\src\foldbar\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FOLDBAR
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28d_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28d_foldbar.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28d_foldbar_vc_custom.dll" /libpath:"..\..\src\foldbar\..\..\..\lib\vc_dll" /implib:"..\..\src\foldbar\..\..\..\lib\vc_dll\wxmsw28d_foldbar.lib" /debug

!ELSEIF  "$(CFG)" == "foldbar - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswunivu" /I "..\..\src\foldbar\..\..\..\include" /W4 /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28u_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswunivu" /I "..\..\src\foldbar\..\..\..\include" /W4 /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28u_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28u_foldbar.lib"
# ADD LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28u_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswunivud" /I "..\..\src\foldbar\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28ud_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswunivud" /I "..\..\src\foldbar\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28ud_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28ud_foldbar.lib"
# ADD LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28ud_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswuniv\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswuniv\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswuniv" /I "..\..\src\foldbar\..\..\..\include" /W4 /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswuniv" /I "..\..\src\foldbar\..\..\..\include" /W4 /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28_foldbar.lib"
# ADD LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivd\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivd\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswunivd" /I "..\..\src\foldbar\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28d_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswunivd" /I "..\..\src\foldbar\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28d_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28d_foldbar.lib"
# ADD LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmswuniv28d_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswu" /I "..\..\src\foldbar\..\..\..\include" /W4 /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28u_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswu" /I "..\..\src\foldbar\..\..\..\include" /W4 /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28u_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28u_foldbar.lib"
# ADD LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28u_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswud" /I "..\..\src\foldbar\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28ud_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswud" /I "..\..\src\foldbar\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28ud_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28ud_foldbar.lib"
# ADD LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28ud_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_msw\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_msw\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\msw" /I "..\..\src\foldbar\..\..\..\include" /W4 /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\msw" /I "..\..\src\foldbar\..\..\..\include" /W4 /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28_foldbar.lib"
# ADD LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28_foldbar.lib"

!ELSEIF  "$(CFG)" == "foldbar - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswd\foldbar"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\foldbar\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswd\foldbar"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswd" /I "..\..\src\foldbar\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28d_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\foldbar\..\..\..\lib\vc_lib\mswd" /I "..\..\src\foldbar\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28d_foldbar.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_foldbarlib.pch" /I "..\..\src\foldbar\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28d_foldbar.lib"
# ADD LIB32 /nologo /out:"..\..\src\foldbar\..\..\..\lib\vc_lib\wxmsw28d_foldbar.lib"

!ENDIF

# Begin Target

# Name "foldbar - Win32 DLL Universal Unicode Release"
# Name "foldbar - Win32 DLL Universal Unicode Debug"
# Name "foldbar - Win32 DLL Universal Release"
# Name "foldbar - Win32 DLL Universal Debug"
# Name "foldbar - Win32 DLL Unicode Release"
# Name "foldbar - Win32 DLL Unicode Debug"
# Name "foldbar - Win32 DLL Release"
# Name "foldbar - Win32 DLL Debug"
# Name "foldbar - Win32 Universal Unicode Release"
# Name "foldbar - Win32 Universal Unicode Debug"
# Name "foldbar - Win32 Universal Release"
# Name "foldbar - Win32 Universal Debug"
# Name "foldbar - Win32 Unicode Release"
# Name "foldbar - Win32 Unicode Debug"
# Name "foldbar - Win32 Release"
# Name "foldbar - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/foldbar\captionbar.cpp
# End Source File
# Begin Source File

SOURCE=../../src/foldbar\..\..\..\src\common\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/foldbar\foldpanelbar.cpp
# End Source File
# Begin Source File

SOURCE=../../src/foldbar\foldpanelitem.cpp
# End Source File
# Begin Source File

SOURCE=../../src/foldbar\..\..\..\src\msw\version.rc

!IF  "$(CFG)" == "foldbar - Win32 DLL Universal Unicode Release"


!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Universal Unicode Debug"


!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Universal Release"


!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Universal Debug"


!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Unicode Release"


!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Unicode Debug"


!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "foldbar - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "foldbar - Win32 Universal Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "foldbar - Win32 Universal Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "foldbar - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "foldbar - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "foldbar - Win32 Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "foldbar - Win32 Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "foldbar - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "foldbar - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# End Target
# End Project

