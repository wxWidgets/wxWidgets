# Microsoft Developer Studio Project File - Name="wx_base" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=base - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_base.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_base.mak" CFG="base - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "base - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "base - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "base - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "base - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "base - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "base - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "base - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "base - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "base - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "base - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_basedll.pch" /Fd..\..\lib\vc_dll\wxbase271u_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_basedll.pch" /Fd..\..\lib\vc_dll\wxbase271u_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivu" /i "..\..\include" /d WXDLLNAME=wxbase271u_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivu" /i "..\..\include" /d WXDLLNAME=wxbase271u_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271u_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27u.lib"
# ADD LINK32 wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271u_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27u.lib"

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxbase271ud_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxbase271ud_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivud" /i "..\..\include" /d WXDLLNAME=wxbase271ud_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivud" /i "..\..\include" /d WXDLLNAME=wxbase271ud_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271ud_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27ud.lib" /debug
# ADD LINK32 wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271ud_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27ud.lib" /debug

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivdll\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswuniv" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_basedll.pch" /Fd..\..\lib\vc_dll\wxbase271_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswuniv" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_basedll.pch" /Fd..\..\lib\vc_dll\wxbase271_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\lib\vc_dll\mswuniv" /i "..\..\include" /d WXDLLNAME=wxbase271_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\lib\vc_dll\mswuniv" /i "..\..\include" /d WXDLLNAME=wxbase271_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27.lib"
# ADD LINK32 wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27.lib"

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivddll\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxbase271d_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxbase271d_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswunivd" /i "..\..\include" /d WXDLLNAME=wxbase271d_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswunivd" /i "..\..\include" /d WXDLLNAME=wxbase271d_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271d_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27d.lib" /debug
# ADD LINK32 wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271d_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27d.lib" /debug

!ELSEIF  "$(CFG)" == "base - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_basedll.pch" /Fd..\..\lib\vc_dll\wxbase271u_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_basedll.pch" /Fd..\..\lib\vc_dll\wxbase271u_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d WXDLLNAME=wxbase271u_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d WXDLLNAME=wxbase271u_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271u_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27u.lib"
# ADD LINK32 wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271u_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27u.lib"

!ELSEIF  "$(CFG)" == "base - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxbase271ud_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxbase271ud_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d WXDLLNAME=wxbase271ud_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d WXDLLNAME=wxbase271ud_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271ud_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27ud.lib" /debug
# ADD LINK32 wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271ud_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27ud.lib" /debug

!ELSEIF  "$(CFG)" == "base - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswdll\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswdll\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\msw" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_basedll.pch" /Fd..\..\lib\vc_dll\wxbase271_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\msw" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_basedll.pch" /Fd..\..\lib\vc_dll\wxbase271_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i "..\..\lib\vc_dll\msw" /i "..\..\include" /d WXDLLNAME=wxbase271_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x409 /d "__WXMSW__" /i "..\..\lib\vc_dll\msw" /i "..\..\include" /d WXDLLNAME=wxbase271_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27.lib"
# ADD LINK32 wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27.lib"

!ELSEIF  "$(CFG)" == "base - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswddll\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswddll\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxbase271d_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxbase271d_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswd" /i "..\..\include" /d WXDLLNAME=wxbase271d_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswd" /i "..\..\include" /d WXDLLNAME=wxbase271d_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271d_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27d.lib" /debug
# ADD LINK32 wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxbase271d_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxbase27d.lib" /debug

!ELSEIF  "$(CFG)" == "base - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxbase27u.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxbase27u.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27u.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27u.lib"

!ELSEIF  "$(CFG)" == "base - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxbase27ud.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxbase27ud.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27ud.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27ud.lib"

!ELSEIF  "$(CFG)" == "base - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswuniv\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswuniv\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswuniv" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxbase27.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswuniv" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxbase27.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27.lib"

!ELSEIF  "$(CFG)" == "base - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivd\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivd\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxbase27d.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxbase27d.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27d.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27d.lib"

!ELSEIF  "$(CFG)" == "base - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxbase27u.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxbase27u.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27u.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27u.lib"

!ELSEIF  "$(CFG)" == "base - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxbase27ud.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxbase27ud.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27ud.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27ud.lib"

!ELSEIF  "$(CFG)" == "base - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_msw\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_msw\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\msw" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxbase27.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\msw" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxbase27.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27.lib"

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswd\base"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswd\base"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxbase27d.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxbase27d.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_baselib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D wxUSE_BASE=1 /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27d.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxbase27d.lib"

!ENDIF

# Begin Target

# Name "base - Win32 DLL Universal Unicode Release"
# Name "base - Win32 DLL Universal Unicode Debug"
# Name "base - Win32 DLL Universal Release"
# Name "base - Win32 DLL Universal Debug"
# Name "base - Win32 DLL Unicode Release"
# Name "base - Win32 DLL Unicode Debug"
# Name "base - Win32 DLL Release"
# Name "base - Win32 DLL Debug"
# Name "base - Win32 Universal Unicode Release"
# Name "base - Win32 Universal Unicode Debug"
# Name "base - Win32 Universal Release"
# Name "base - Win32 Universal Debug"
# Name "base - Win32 Unicode Release"
# Name "base - Win32 Unicode Debug"
# Name "base - Win32 Release"
# Name "base - Win32 Debug"
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\common\appbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\archive.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\clntdata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\cmdline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\config.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\convauto.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\datetime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\datstrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dircmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dynarray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dynlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dynload.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\encconv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\event.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\extended.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\src\common\ffile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fileback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fileconf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\filefn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\filename.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\filesys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\filtall.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\filtfind.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fmapbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fs_mem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\fs_zip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\hash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\hashmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\init.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\intl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\ipcbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\list.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\longlong.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\memory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\mimecmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\module.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\msgout.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\mstream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\object.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\platinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\powercmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\process.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\regex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\sstream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\stdpbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\stopwatch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\strconv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\stream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\string.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\sysopt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\textbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\textfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\tokenzr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\txtstrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\uri.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\utilscmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\variant.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\wfstream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\wxchar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\xti.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\xtistrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\zipstrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\zstream.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\msw\basemsw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\crashrpt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dde.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\debughlp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dlmsw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\mimetype.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\mslu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\power.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\regconf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\registry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\snglinst.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\stackwalk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\stdpaths.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\utilsexc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\version.rc

!IF  "$(CFG)" == "base - Win32 DLL Universal Unicode Release"


!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Unicode Debug"


!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Release"


!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Debug"


!ELSEIF  "$(CFG)" == "base - Win32 DLL Unicode Release"


!ELSEIF  "$(CFG)" == "base - Win32 DLL Unicode Debug"


!ELSEIF  "$(CFG)" == "base - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "base - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "base - Win32 Universal Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "base - Win32 Universal Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "base - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "base - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "base - Win32 Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "base - Win32 Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "base - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\src\msw\volume.cpp
# End Source File
# End Group
# Begin Group "Setup Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\setup.h

!IF  "$(CFG)" == "base - Win32 DLL Universal Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\msw\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_dll\msw\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\msw\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswd\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswd\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\msw\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_lib\msw\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\msw\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswd\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswd\wx\setup.h

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\univ\setup.h

!IF  "$(CFG)" == "base - Win32 DLL Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswuniv\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswuniv\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswuniv\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivd\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivd\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswuniv\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswuniv\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswuniv\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivd\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivd\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

# Begin Custom Build - 

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "MSW Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\apptbase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\apptrait.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\chkconf.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\crashrpt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\dde.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\debughlp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\gccpriv.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\genrcdefs.h

!IF  "$(CFG)" == "base - Win32 DLL Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswuniv\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswuniv\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswuniv\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivd\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswunivd\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivd\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\msw\wx\msw\rcdefs.h

"..\..\lib\vc_dll\msw\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\msw\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswd\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswd\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswd\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswuniv\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswuniv\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswuniv\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivd\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswunivd\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivd\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\msw\wx\msw\rcdefs.h

"..\..\lib\vc_lib\msw\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\msw\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswd\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswd\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswd\wx\msw\rcdefs.h"

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\libraries.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\mslu.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\private.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\regconf.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\registry.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\seh.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\stackwalk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\stdpaths.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\winundef.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\wrapcctl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\wrapcdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\wrapwin.h
# End Source File
# End Group
# Begin Group "wxHTML Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\html\forcelnk.h
# End Source File
# End Group
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\afterstd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\app.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\apptrait.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\archive.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\arrstr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\beforestd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\buffer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\build.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\chkconf.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\clntdata.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\cmdline.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\confbase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\config.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\containr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\convauto.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\cpp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\datetime.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\datstrm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dde.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\defs.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dir.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dlimpexp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dynarray.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dynlib.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dynload.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\encconv.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\event.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\except.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\features.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ffile.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\file.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fileback.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fileconf.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\filefn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\filename.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\filesys.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fontenc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fontmap.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fs_mem.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fs_zip.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\hash.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\hashmap.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\hashset.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\iconloc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\init.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\intl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\iosfwrap.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ioswrap.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ipc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ipcbase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\isql.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\isqlext.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\link.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\list.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\log.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\longlong.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\math.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\memconf.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\memory.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\memtext.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\module.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msgout.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\mstream.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\object.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\platinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\power.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\process.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ptr_scpd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\recguard.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\regex.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\scopeguard.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\snglinst.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\sstream.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\stack.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\stackwalk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\stdpaths.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\stockitem.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\stopwatch.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\strconv.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\stream.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\string.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\sysopt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\textbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\textfile.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\tokenzr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\txtstrm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\types.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\uri.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\utils.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\variant.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\vector.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\version.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\volume.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wfstream.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wx.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wxchar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wxprec.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xti.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xtistrm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\zipstrm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\zstream.h
# End Source File
# End Group
# End Target
# End Project

