# Microsoft Developer Studio Project File - Name="wx_stc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=stc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_stc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_stc.mak" CFG="stc - Win32 Debug"
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
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_stcdll.pch" /Fd..\..\lib\vc_dll\wxmswuniv290u_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_stcdll.pch" /Fd..\..\lib\vc_dll\wxmswuniv290u_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivu" /i "..\..\include" /d WXDLLNAME=wxmswuniv290u_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivu" /i "..\..\include" /d WXDLLNAME=wxmswuniv290u_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintilla.lib ..\..\lib\vc_dll\wxmswuniv29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv290u_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29u_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintilla.lib ..\..\lib\vc_dll\wxmswuniv29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv290u_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_stcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmswuniv290ud_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_stcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmswuniv290ud_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivud" /i "..\..\include" /d WXDLLNAME=wxmswuniv290ud_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivud" /i "..\..\include" /d WXDLLNAME=wxmswuniv290ud_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintillad.lib ..\..\lib\vc_dll\wxmswuniv29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv290ud_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29ud_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintillad.lib ..\..\lib\vc_dll\wxmswuniv29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv290ud_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29ud_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivdll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswuniv" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_stcdll.pch" /Fd..\..\lib\vc_dll\wxmswuniv290_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswuniv" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_stcdll.pch" /Fd..\..\lib\vc_dll\wxmswuniv290_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\lib\vc_dll\mswuniv" /i "..\..\include" /d WXDLLNAME=wxmswuniv290_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\lib\vc_dll\mswuniv" /i "..\..\include" /d WXDLLNAME=wxmswuniv290_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintilla.lib ..\..\lib\vc_dll\wxmswuniv29_core.lib ..\..\lib\vc_dll\wxbase29.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv290_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintilla.lib ..\..\lib\vc_dll\wxmswuniv29_core.lib ..\..\lib\vc_dll\wxbase29.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv290_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_stcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmswuniv290d_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_stcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmswuniv290d_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswunivd" /i "..\..\include" /d WXDLLNAME=wxmswuniv290d_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswunivd" /i "..\..\include" /d WXDLLNAME=wxmswuniv290d_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintillad.lib ..\..\lib\vc_dll\wxmswuniv29d_core.lib ..\..\lib\vc_dll\wxbase29d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv290d_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29d_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintillad.lib ..\..\lib\vc_dll\wxmswuniv29d_core.lib ..\..\lib\vc_dll\wxbase29d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv290d_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29d_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_stcdll.pch" /Fd..\..\lib\vc_dll\wxmsw290u_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_stcdll.pch" /Fd..\..\lib\vc_dll\wxmsw290u_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d WXDLLNAME=wxmsw290u_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d WXDLLNAME=wxmsw290u_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintilla.lib ..\..\lib\vc_dll\wxmsw29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw290u_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29u_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintilla.lib ..\..\lib\vc_dll\wxmsw29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw290u_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_stcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmsw290ud_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_stcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmsw290ud_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d WXDLLNAME=wxmsw290ud_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d WXDLLNAME=wxmsw290ud_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintillad.lib ..\..\lib\vc_dll\wxmsw29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw290ud_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29ud_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintillad.lib ..\..\lib\vc_dll\wxmsw29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw290ud_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29ud_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswdll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswdll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\msw" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_stcdll.pch" /Fd..\..\lib\vc_dll\wxmsw290_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\msw" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_stcdll.pch" /Fd..\..\lib\vc_dll\wxmsw290_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i "..\..\lib\vc_dll\msw" /i "..\..\include" /d WXDLLNAME=wxmsw290_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "__WXMSW__" /i "..\..\lib\vc_dll\msw" /i "..\..\include" /d WXDLLNAME=wxmsw290_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintilla.lib ..\..\lib\vc_dll\wxmsw29_core.lib ..\..\lib\vc_dll\wxbase29.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw290_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintilla.lib ..\..\lib\vc_dll\wxmsw29_core.lib ..\..\lib\vc_dll\wxbase29.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw290_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_stcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmsw290d_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_stcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmsw290d_stc_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswd" /i "..\..\include" /d WXDLLNAME=wxmsw290d_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswd" /i "..\..\include" /d WXDLLNAME=wxmsw290d_stc_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintillad.lib ..\..\lib\vc_dll\wxmsw29d_core.lib ..\..\lib\vc_dll\wxbase29d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw290d_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29d_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxscintillad.lib ..\..\lib\vc_dll\wxmsw29d_core.lib ..\..\lib\vc_dll\wxbase29d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw290d_stc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29d_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmswuniv29u_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmswuniv29u_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29u_stc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmswuniv29ud_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmswuniv29ud_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29ud_stc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29ud_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswuniv\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswuniv\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswuniv" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmswuniv29_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswuniv" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmswuniv29_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29_stc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivd\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivd\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmswuniv29d_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmswuniv29d_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29d_stc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29d_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmsw29u_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmsw29u_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29u_stc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmsw29ud_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmsw29ud_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29ud_stc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29ud_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_msw\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_msw\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\msw" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmsw29_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\msw" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmsw29_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29_stc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswd\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswd\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmsw29d_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmsw29d_stc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_stclib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29d_stc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29d_stc.lib"

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
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\common\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\msw\version.rc

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
# Begin Group "Setup Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\univ\setup.h

!IF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswuniv\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswuniv\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswuniv\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivd\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivd\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswuniv\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswuniv\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswuniv\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivd\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivd\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Debug"

# Begin Custom Build - 

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\setup.h

!IF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\msw\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\msw\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\msw\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswd\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswd\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\msw\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\msw\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\msw\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswd\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswd\wx\setup.h

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "MSW Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\genrcdefs.h

!IF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswuniv\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswuniv\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswuniv\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswuniv\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivd\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivd\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivd\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivd\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\msw\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\msw\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\msw\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\msw\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswd\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswd\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswd\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswd\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswuniv\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswuniv\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswuniv\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswuniv\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivd\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivd\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivd\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivd\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\msw\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\msw\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\msw\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\msw\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "stc - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswd\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswd\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswd\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswd\wx\msw\rcdefs.h"

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\stc\stc.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\stc\PlatWX.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\ScintillaWX.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\stc.cpp
# End Source File
# End Group
# End Target
# End Project

