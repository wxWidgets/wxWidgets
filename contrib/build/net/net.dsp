# Microsoft Developer Studio Project File - Name="net" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=netutils - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "net.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "net.mak" CFG="netutils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "netutils - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutils - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutils - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutils - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutils - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutils - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutils - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutils - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutils - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "netutils - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "netutils - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "netutils - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "netutils - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "netutils - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "netutils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "netutils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "netutils - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswunivu" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272u_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswunivu" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272u_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\src\net\..\..\..\lib\vc_dll\mswunivu" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmswuniv272u_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\src\net\..\..\..\lib\vc_dll\mswunivu" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmswuniv272u_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27u_core.lib wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272u_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv27u_netutils.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27u_core.lib wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272u_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv27u_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswunivud" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272ud_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswunivud" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272ud_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\net\..\..\..\lib\vc_dll\mswunivud" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmswuniv272ud_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\net\..\..\..\lib\vc_dll\mswunivud" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmswuniv272ud_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27ud_core.lib wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272ud_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv27ud_netutils.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27ud_core.lib wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272ud_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv27ud_netutils.lib" /debug

!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivdll\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswuniv" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswuniv" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\net\..\..\..\lib\vc_dll\mswuniv" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmswuniv272_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\net\..\..\..\lib\vc_dll\mswuniv" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmswuniv272_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27_core.lib wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv27_netutils.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27_core.lib wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv27_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivddll\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswunivd" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272d_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswunivd" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272d_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\net\..\..\..\lib\vc_dll\mswunivd" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmswuniv272d_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\net\..\..\..\lib\vc_dll\mswunivd" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmswuniv272d_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27d_core.lib wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272d_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv27d_netutils.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv27d_core.lib wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv272d_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmswuniv27d_netutils.lib" /debug

!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswu" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmsw272u_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswu" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmsw272u_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\src\net\..\..\..\lib\vc_dll\mswu" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmsw272u_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\src\net\..\..\..\lib\vc_dll\mswu" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmsw272u_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27u_core.lib wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw272u_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw27u_netutils.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27u_core.lib wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw272u_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw27u_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswud" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmsw272ud_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswud" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmsw272ud_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\net\..\..\..\lib\vc_dll\mswud" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmsw272ud_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\net\..\..\..\lib\vc_dll\mswud" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmsw272ud_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27ud_core.lib wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw272ud_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw27ud_netutils.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27ud_core.lib wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw272ud_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw27ud_netutils.lib" /debug

!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswdll\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswdll\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\msw" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmsw272_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\msw" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmsw272_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i "..\..\src\net\..\..\..\lib\vc_dll\msw" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmsw272_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x409 /d "__WXMSW__" /i "..\..\src\net\..\..\..\lib\vc_dll\msw" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmsw272_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27_core.lib wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw272_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw27_netutils.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27_core.lib wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw272_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw27_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswddll\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswddll\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswd" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmsw272d_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_dll\mswd" /I "..\..\src\net\..\..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_dll\wxmsw272d_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\net\..\..\..\lib\vc_dll\mswd" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmsw272d_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\net\..\..\..\lib\vc_dll\mswd" /i "..\..\src\net\..\..\..\include" /d WXDLLNAME=wxmsw272d_netutils_vc_custom /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27d_core.lib wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw272d_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw27d_netutils.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw27d_core.lib wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw272d_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_dll" /implib:"..\..\src\net\..\..\..\lib\vc_dll\wxmsw27d_netutils.lib" /debug

!ELSEIF  "$(CFG)" == "netutils - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswunivu" /I "..\..\src\net\..\..\..\include" /W4 /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27u_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswunivu" /I "..\..\src\net\..\..\..\include" /W4 /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27u_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27u_netutils.lib"
# ADD LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27u_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswunivud" /I "..\..\src\net\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27ud_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswunivud" /I "..\..\src\net\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27ud_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27ud_netutils.lib"
# ADD LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27ud_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswuniv\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswuniv\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswuniv" /I "..\..\src\net\..\..\..\include" /W4 /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswuniv" /I "..\..\src\net\..\..\..\include" /W4 /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27_netutils.lib"
# ADD LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivd\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivd\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswunivd" /I "..\..\src\net\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27d_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswunivd" /I "..\..\src\net\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27d_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27d_netutils.lib"
# ADD LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmswuniv27d_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswu" /I "..\..\src\net\..\..\..\include" /W4 /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmsw27u_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswu" /I "..\..\src\net\..\..\..\include" /W4 /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmsw27u_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmsw27u_netutils.lib"
# ADD LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmsw27u_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswud" /I "..\..\src\net\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmsw27ud_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswud" /I "..\..\src\net\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmsw27ud_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmsw27ud_netutils.lib"
# ADD LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmsw27ud_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_msw\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_msw\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\msw" /I "..\..\src\net\..\..\..\include" /W4 /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmsw27_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\msw" /I "..\..\src\net\..\..\..\include" /W4 /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmsw27_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmsw27_netutils.lib"
# ADD LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmsw27_netutils.lib"

!ELSEIF  "$(CFG)" == "netutils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswd\netutils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswd\netutils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswd" /I "..\..\src\net\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmsw27d_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\net\..\..\..\lib\vc_lib\mswd" /I "..\..\src\net\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_lib\wxmsw27d_netutils.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_netutilslib.pch" /I "..\..\src\net\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmsw27d_netutils.lib"
# ADD LIB32 /nologo /out:"..\..\src\net\..\..\..\lib\vc_lib\wxmsw27d_netutils.lib"

!ENDIF

# Begin Target

# Name "netutils - Win32 DLL Universal Unicode Release"
# Name "netutils - Win32 DLL Universal Unicode Debug"
# Name "netutils - Win32 DLL Universal Release"
# Name "netutils - Win32 DLL Universal Debug"
# Name "netutils - Win32 DLL Unicode Release"
# Name "netutils - Win32 DLL Unicode Debug"
# Name "netutils - Win32 DLL Release"
# Name "netutils - Win32 DLL Debug"
# Name "netutils - Win32 Universal Unicode Release"
# Name "netutils - Win32 Universal Unicode Debug"
# Name "netutils - Win32 Universal Release"
# Name "netutils - Win32 Universal Debug"
# Name "netutils - Win32 Unicode Release"
# Name "netutils - Win32 Unicode Debug"
# Name "netutils - Win32 Release"
# Name "netutils - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/net\..\..\..\src\common\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/net\email.cpp
# End Source File
# Begin Source File

SOURCE=../../src/net\smapi.cpp
# End Source File
# Begin Source File

SOURCE=../../src/net\..\..\..\src\msw\version.rc

!IF  "$(CFG)" == "netutils - Win32 DLL Universal Unicode Release"


!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Universal Unicode Debug"


!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Universal Release"


!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Universal Debug"


!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Unicode Release"


!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Unicode Debug"


!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "netutils - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "netutils - Win32 Universal Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "netutils - Win32 Universal Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "netutils - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "netutils - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "netutils - Win32 Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "netutils - Win32 Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "netutils - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "netutils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=../../src/net\web.cpp
# End Source File
# End Group
# End Target
# End Project

