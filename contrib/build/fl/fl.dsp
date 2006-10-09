# Microsoft Developer Studio Project File - Name="fl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=fl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fl.mak" CFG="fl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fl - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fl - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswunivu" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswunivu" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswunivu" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmswuniv271u_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswunivu" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmswuniv271u_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27u_core.lib wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv27u_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27u_core.lib wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv27u_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswunivud" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271ud_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswunivud" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271ud_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswunivud" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmswuniv271ud_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswunivud" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmswuniv271ud_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27ud_core.lib wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271ud_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv27ud_fl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27ud_core.lib wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271ud_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv27ud_fl.lib" /debug

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivdll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswuniv" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswuniv" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswuniv" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmswuniv271_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswuniv" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmswuniv271_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27_core.lib wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv27_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27_core.lib wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv27_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivddll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswunivd" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271d_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswunivd" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271d_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswunivd" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmswuniv271d_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswunivd" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmswuniv271d_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27d_core.lib wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271d_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv27d_fl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27d_core.lib wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv271d_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmswuniv27d_fl.lib" /debug

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswu" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswu" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswu" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmsw271u_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswu" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmsw271u_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27u_core.lib wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw27u_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27u_core.lib wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw27u_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswud" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271ud_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswud" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271ud_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswud" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmsw271ud_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswud" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmsw271ud_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27ud_core.lib wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271ud_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw27ud_fl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27ud_core.lib wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271ud_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw27ud_fl.lib" /debug

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswdll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswdll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\msw" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\msw" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i "..\..\src\fl\..\..\..\lib\vc_dll\msw" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmsw271_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x409 /d "__WXMSW__" /i "..\..\src\fl\..\..\..\lib\vc_dll\msw" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmsw271_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27_core.lib wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw27_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27_core.lib wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw27_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswddll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswddll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswd" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271d_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_dll\mswd" /I "..\..\src\fl\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271d_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswd" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmsw271d_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\fl\..\..\..\lib\vc_dll\mswd" /i "..\..\src\fl\..\..\..\include" /d WXDLLNAME=wxmsw271d_fl_vc_custom /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27d_core.lib wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271d_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw27d_fl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27d_core.lib wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw271d_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_dll" /implib:"..\..\src\fl\..\..\..\lib\vc_dll\wxmsw27d_fl.lib" /debug

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswunivu" /I "..\..\src\fl\..\..\..\include" /W4 /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27u_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswunivu" /I "..\..\src\fl\..\..\..\include" /W4 /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27u_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27u_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27u_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswunivud" /I "..\..\src\fl\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27ud_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswunivud" /I "..\..\src\fl\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27ud_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27ud_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27ud_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswuniv\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswuniv\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswuniv" /I "..\..\src\fl\..\..\..\include" /W4 /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswuniv" /I "..\..\src\fl\..\..\..\include" /W4 /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivd\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivd\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswunivd" /I "..\..\src\fl\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27d_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswunivd" /I "..\..\src\fl\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27d_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27d_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmswuniv27d_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswu" /I "..\..\src\fl\..\..\..\include" /W4 /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27u_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswu" /I "..\..\src\fl\..\..\..\include" /W4 /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27u_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27u_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27u_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswud" /I "..\..\src\fl\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27ud_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswud" /I "..\..\src\fl\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27ud_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27ud_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27ud_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_msw\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_msw\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\msw" /I "..\..\src\fl\..\..\..\include" /W4 /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\msw" /I "..\..\src\fl\..\..\..\include" /W4 /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswd\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswd\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswd" /I "..\..\src\fl\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27d_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\fl\..\..\..\lib\vc_lib\mswd" /I "..\..\src\fl\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27d_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27d_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_lib\wxmsw27d_fl.lib"

!ENDIF

# Begin Target

# Name "fl - Win32 DLL Universal Unicode Release"
# Name "fl - Win32 DLL Universal Unicode Debug"
# Name "fl - Win32 DLL Universal Release"
# Name "fl - Win32 DLL Universal Debug"
# Name "fl - Win32 DLL Unicode Release"
# Name "fl - Win32 DLL Unicode Debug"
# Name "fl - Win32 DLL Release"
# Name "fl - Win32 DLL Debug"
# Name "fl - Win32 Universal Unicode Release"
# Name "fl - Win32 Universal Unicode Debug"
# Name "fl - Win32 Universal Release"
# Name "fl - Win32 Universal Debug"
# Name "fl - Win32 Unicode Release"
# Name "fl - Win32 Unicode Debug"
# Name "fl - Win32 Release"
# Name "fl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/fl\antiflickpl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\bardragpl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\barhintspl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\cbcustom.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\controlbar.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\..\..\..\src\common\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/fl\dyntbar.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\dyntbarhnd.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\frmview.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\garbagec.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\gcupdatesmgr.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\hintanimpl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\newbmpbtn.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\panedrawpl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\rowdragpl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\rowlayoutpl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\toolwnd.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\updatesmgr.cpp
# End Source File
# Begin Source File

SOURCE=../../src/fl\..\..\..\src\msw\version.rc

!IF  "$(CFG)" == "fl - Win32 DLL Universal Unicode Release"


!ELSEIF  "$(CFG)" == "fl - Win32 DLL Universal Unicode Debug"


!ELSEIF  "$(CFG)" == "fl - Win32 DLL Universal Release"


!ELSEIF  "$(CFG)" == "fl - Win32 DLL Universal Debug"


!ELSEIF  "$(CFG)" == "fl - Win32 DLL Unicode Release"


!ELSEIF  "$(CFG)" == "fl - Win32 DLL Unicode Debug"


!ELSEIF  "$(CFG)" == "fl - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "fl - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "fl - Win32 Universal Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "fl - Win32 Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "fl - Win32 Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "fl - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "fl - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# End Target
# End Project

