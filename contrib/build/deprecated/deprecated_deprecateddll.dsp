# Microsoft Developer Studio Project File - Name="deprecateddll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=deprecateddll - Win32 DLL Native Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "deprecated_deprecateddll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "deprecated_deprecateddll.mak" CFG="deprecateddll - Win32 DLL Native Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "deprecateddll - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "deprecateddll - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "deprecateddll - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "deprecateddll - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "deprecateddll - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "deprecateddll - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "deprecateddll - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "deprecateddll - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "deprecateddll - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\deprecateddll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\deprecateddll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswuddll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_deprecateddll.pch" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswuddll\wxmsw250ud_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswuddll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_deprecateddll.pch" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswuddll\wxmsw250ud_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswuddll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswuddll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswuddll\wxmsw250ud_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswuddll\wxmsw25ud_deprecated.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswuddll\wxmsw250ud_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswuddll\wxmsw25ud_deprecated.lib" /debug

!ELSEIF  "$(CFG)" == "deprecateddll - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\deprecateddll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\deprecateddll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswdll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_deprecateddll.pch" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswdll\wxmsw250_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswdll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_deprecateddll.pch" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswdll\wxmsw250_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswdll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswdll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswdll\wxmsw250_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswdll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswdll\wxmsw25_deprecated.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswdll\wxmsw250_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswdll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswdll\wxmsw25_deprecated.lib"

!ELSEIF  "$(CFG)" == "deprecateddll - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\deprecateddll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\deprecateddll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_deprecateddll.pch" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_deprecateddll.pch" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_deprecated.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_deprecated.lib" /debug

!ELSEIF  "$(CFG)" == "deprecateddll - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\deprecateddll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\deprecateddll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivudll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_deprecateddll.pch" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivudll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_deprecateddll.pch" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswunivudll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswunivudll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_deprecated.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_deprecated.lib"

!ELSEIF  "$(CFG)" == "deprecateddll - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\deprecateddll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\deprecateddll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswddll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_deprecateddll.pch" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswddll\wxmsw250d_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswddll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_deprecateddll.pch" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswddll\wxmsw250d_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswddll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswddll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswddll\wxmsw250d_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswddll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswddll\wxmsw25d_deprecated.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswddll\wxmsw250d_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswddll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswddll\wxmsw25d_deprecated.lib" /debug

!ELSEIF  "$(CFG)" == "deprecateddll - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\deprecateddll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\deprecateddll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivdll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_deprecateddll.pch" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivdll\wxmswuniv250_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivdll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_deprecateddll.pch" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivdll\wxmswuniv250_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswunivdll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswunivdll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivdll\wxmswuniv250_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswunivdll\wxmswuniv25_deprecated.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivdll\wxmswuniv250_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswunivdll\wxmswuniv25_deprecated.lib"

!ELSEIF  "$(CFG)" == "deprecateddll - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\deprecateddll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\deprecateddll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivddll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_deprecateddll.pch" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivddll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_deprecateddll.pch" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswunivddll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswunivddll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_deprecated.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_deprecated.lib" /debug

!ELSEIF  "$(CFG)" == "deprecateddll - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\deprecateddll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\deprecateddll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswudll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_deprecateddll.pch" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswudll\wxmsw250u_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswudll" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_deprecateddll.pch" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswudll\wxmsw250u_deprecated_vc_custom.pdb /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_DEPRECATED" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswudll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\deprecated\..\..\..\include" /i "..\..\src\deprecated\..\..\..\lib\vc_mswudll" /i "..\..\src\deprecated\..\..\..\src\tiff" /i "..\..\src\deprecated\..\..\..\src\jpeg" /i "..\..\src\deprecated\..\..\..\src\png" /i "..\..\src\deprecated\..\..\..\src\zlib" /i "..\..\src\deprecated\..\..\..\src\regex" /i "..\..\src\deprecated\..\..\..\src\expat\lib" /i "..\..\src\deprecated\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_DEPRECATED
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswudll\wxmsw250u_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswudll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswudll\wxmsw25u_deprecated.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\deprecated\..\..\..\lib\vc_mswudll\wxmsw250u_deprecated_vc_custom.dll" /libpath:"..\..\src\deprecated\..\..\..\lib\vc_mswudll" /implib:"..\..\src\deprecated\..\..\..\lib\vc_mswudll\wxmsw25u_deprecated.lib"

!ENDIF

# Begin Target

# Name "deprecateddll - Win32 DLL Native Debug Unicode"
# Name "deprecateddll - Win32 DLL Native Release Ansi"
# Name "deprecateddll - Win32 DLL wxUniv Debug Unicode"
# Name "deprecateddll - Win32 DLL wxUniv Release Unicode"
# Name "deprecateddll - Win32 DLL Native Debug Ansi"
# Name "deprecateddll - Win32 DLL wxUniv Release Ansi"
# Name "deprecateddll - Win32 DLL wxUniv Debug Ansi"
# Name "deprecateddll - Win32 DLL Native Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/deprecated\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\dosyacc.c
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\prop.cpp
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\propform.cpp
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\proplist.cpp
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\resource.cpp
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\treelay.cpp
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\wxexpr.cpp
# End Source File
# End Group
# End Target
# End Project

