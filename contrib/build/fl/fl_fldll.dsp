# Microsoft Developer Studio Project File - Name="fldll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=fldll - Win32 DLL Native Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fl_fldll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fl_fldll.mak" CFG="fldll - Win32 DLL Native Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fldll - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fldll - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fldll - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fldll - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fldll - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fldll - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fldll - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fldll - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fldll - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswuddll\"
# PROP BASE Intermediate_Dir "vc_mswuddll\fldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswuddll\"
# PROP Intermediate_Dir "vc_mswuddll\fldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswuddll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswuddll\wxmsw250ud_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswuddll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswuddll\wxmsw250ud_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswuddll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswuddll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswuddll\wxmsw250ud_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswuddll\wxmsw25ud_fl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswuddll\wxmsw250ud_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswuddll\wxmsw25ud_fl.lib" /debug

!ELSEIF  "$(CFG)" == "fldll - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswdll\"
# PROP BASE Intermediate_Dir "vc_mswdll\fldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswdll\"
# PROP Intermediate_Dir "vc_mswdll\fldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswdll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw250_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswdll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw250_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswdll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswdll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw250_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswdll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw25_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw250_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswdll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw25_fl.lib"

!ELSEIF  "$(CFG)" == "fldll - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivuddll\"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\fldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivuddll\"
# PROP Intermediate_Dir "vc_mswunivuddll\fldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_fl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_fl.lib" /debug

!ELSEIF  "$(CFG)" == "fldll - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivudll\"
# PROP BASE Intermediate_Dir "vc_mswunivudll\fldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivudll\"
# PROP Intermediate_Dir "vc_mswunivudll\fldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivudll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivudll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivudll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivudll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_fl.lib"

!ELSEIF  "$(CFG)" == "fldll - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswddll\"
# PROP BASE Intermediate_Dir "vc_mswddll\fldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswddll\"
# PROP Intermediate_Dir "vc_mswddll\fldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswddll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswddll\wxmsw250d_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswddll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswddll\wxmsw250d_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswddll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswddll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswddll\wxmsw250d_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswddll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswddll\wxmsw25d_fl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswddll\wxmsw250d_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswddll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswddll\wxmsw25d_fl.lib" /debug

!ELSEIF  "$(CFG)" == "fldll - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivdll\"
# PROP BASE Intermediate_Dir "vc_mswunivdll\fldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivdll\"
# PROP Intermediate_Dir "vc_mswunivdll\fldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivdll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivdll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivdll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivdll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv25_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv25_fl.lib"

!ELSEIF  "$(CFG)" == "fldll - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivddll\"
# PROP BASE Intermediate_Dir "vc_mswunivddll\fldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivddll\"
# PROP Intermediate_Dir "vc_mswunivddll\fldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivddll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivddll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_fldll.pch" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivddll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivddll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_fl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_fl.lib" /debug

!ELSEIF  "$(CFG)" == "fldll - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswudll\"
# PROP BASE Intermediate_Dir "vc_mswudll\fldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswudll\"
# PROP Intermediate_Dir "vc_mswudll\fldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswudll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw250u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswudll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw250u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswudll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswudll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw250u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswudll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw25u_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw250u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswudll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw25u_fl.lib"

!ENDIF

# Begin Target

# Name "fldll - Win32 DLL Native Debug Unicode"
# Name "fldll - Win32 DLL Native Release Ansi"
# Name "fldll - Win32 DLL wxUniv Debug Unicode"
# Name "fldll - Win32 DLL wxUniv Release Unicode"
# Name "fldll - Win32 DLL Native Debug Ansi"
# Name "fldll - Win32 DLL wxUniv Release Ansi"
# Name "fldll - Win32 DLL wxUniv Debug Ansi"
# Name "fldll - Win32 DLL Native Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/fl\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
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
# End Group
# End Target
# End Project

