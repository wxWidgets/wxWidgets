# Microsoft Developer Studio Project File - Name="stc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=stc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stc.mak" CFG="stc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stc - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswunivu" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28u_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswunivu" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28u_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswunivu" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmswuniv28u_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswunivu" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmswuniv28u_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28u_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28u_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28u_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswunivud" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28ud_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswunivud" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28ud_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswunivud" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmswuniv28ud_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswunivud" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmswuniv28ud_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28ud_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28ud_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28ud_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28ud_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivdll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswuniv" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswuniv" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswuniv" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmswuniv28_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswuniv" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmswuniv28_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswunivd" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28d_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswunivd" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28d_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswunivd" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmswuniv28d_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswunivd" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmswuniv28d_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28d_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28d_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmswuniv28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28d_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmswuniv28d_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswu" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28u_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswu" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28u_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswu" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmsw28u_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswu" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmsw28u_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28u_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28u_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28u_core.lib wxbase28u.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28u_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswud" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28ud_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswud" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28ud_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswud" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmsw28ud_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswud" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmsw28ud_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28ud_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28ud_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28ud_core.lib wxbase28ud.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28ud_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28ud_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswdll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswdll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\msw" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\msw" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i "..\..\src\stc\..\..\..\lib\vc_dll\msw" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmsw28_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "__WXMSW__" /i "..\..\src\stc\..\..\..\lib\vc_dll\msw" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmsw28_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswd" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28d_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_dll\mswd" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28d_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswd" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmsw28d_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\stc\..\..\..\lib\vc_dll\mswd" /i "..\..\src\stc\..\..\..\include" /d WXDLLNAME=wxmsw28d_stc_vc_custom /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28d_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28d_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib wxmsw28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28d_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_dll" /implib:"..\..\src\stc\..\..\..\lib\vc_dll\wxmsw28d_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswunivu" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswunivu" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28u_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswunivud" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswunivud" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28ud_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28ud_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswuniv\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswuniv\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswuniv" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswuniv" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivd\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivd\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswunivd" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswunivd" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28d_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmswuniv28d_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswu" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswu" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28u_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswud" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswud" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28ud_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28ud_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_msw\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_msw\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\msw" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\msw" /I "..\..\src\stc\..\..\..\include" /W4 /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswd\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswd\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswd" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\src\stc\..\..\..\lib\vc_lib\mswd" /I "..\..\src\stc\..\..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28d_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_lib\wxmsw28d_stc.lib"

!ENDIF

# Begin Target

# Name "stc - Win32 DLL Universal Unicode Release"
# Name "stc - Win32 DLL Universal Unicode Debug"
# Name "stc - Win32 DLL Universal Release"
# Name "stc - Win32 DLL Universal Debug"
# Name "stc - Win32 DLL Unicode Release"
# Name "stc - Win32 DLL Unicode Debug"
# Name "stc - Win32 DLL Release"
# Name "stc - Win32 DLL Debug"
# Name "stc - Win32 Universal Unicode Release"
# Name "stc - Win32 Universal Unicode Debug"
# Name "stc - Win32 Universal Release"
# Name "stc - Win32 Universal Debug"
# Name "stc - Win32 Unicode Release"
# Name "stc - Win32 Unicode Debug"
# Name "stc - Win32 Release"
# Name "stc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/stc\scintilla\src\AutoComplete.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\CallTip.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\CellBuffer.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\CharClassify.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\ContractionState.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\Document.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\DocumentAccessor.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\Editor.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\ExternalLexer.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\Indicator.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\KeyMap.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\KeyWords.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexAPDL.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexAU3.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexAVE.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexAda.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexAsm.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexAsn1.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexBaan.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexBash.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexBasic.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexBullant.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexCLW.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexCPP.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexCSS.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexCaml.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexConf.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexCrontab.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexCsound.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexEScript.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexEiffel.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexErlang.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexFlagship.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexForth.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexFortran.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexGui4Cli.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexHTML.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexHaskell.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexInno.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexKix.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexLisp.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexLout.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexLua.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexMMIXAL.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexMPT.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexMSSQL.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexMatlab.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexMetapost.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexNsis.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexOpal.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexOthers.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexPB.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexPOV.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexPS.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexPascal.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexPerl.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexPython.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexRebol.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexRuby.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexSQL.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexScriptol.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexSmalltalk.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexSpecman.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexSpice.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexTADS3.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexTCL.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexTeX.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexVB.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexVHDL.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexVerilog.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexYAML.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LineMarker.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\PlatWX.cpp
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\PropSet.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\RESearch.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\ScintillaBase.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\ScintillaWX.cpp
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\Style.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\StyleContext.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\UniConversion.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\ViewStyle.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\WindowAccessor.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\XPM.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\stc.cpp
# End Source File
# Begin Source File

SOURCE=../../src/stc\..\..\..\src\msw\version.rc

!IF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Release"


!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Debug"


!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Release"


!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Debug"


!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Release"


!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Debug"


!ELSEIF  "$(CFG)" == "stc - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "stc - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stc - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "stc - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# End Target
# End Project

