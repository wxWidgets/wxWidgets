# Microsoft Developer Studio Project File - Name="coredll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=coredll - Win32 DLL Native Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_coredll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_coredll.mak" CFG="coredll - Win32 DLL Native Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "coredll - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "coredll - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "coredll - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "coredll - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "coredll - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "coredll - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "coredll - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "coredll - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswuddll\"
# PROP BASE Intermediate_Dir "vc_mswuddll\coredll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswuddll\"
# PROP Intermediate_Dir "vc_mswuddll\coredll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_coredll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxmsw250ud_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_coredll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxmsw250ud_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswuddll\wxmsw250ud_core_vc_custom.dll" /libpath:"..\lib\vc_mswuddll" /implib:"..\lib\vc_mswuddll\wxmsw25ud_core.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswuddll\wxmsw250ud_core_vc_custom.dll" /libpath:"..\lib\vc_mswuddll" /implib:"..\lib\vc_mswuddll\wxmsw25ud_core.lib" /debug

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswdll\"
# PROP BASE Intermediate_Dir "vc_mswdll\coredll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswdll\"
# PROP Intermediate_Dir "vc_mswdll\coredll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_coredll.pch" /Fd..\lib\vc_mswdll\wxmsw250_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_coredll.pch" /Fd..\lib\vc_mswdll\wxmsw250_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\include" /i "..\lib\vc_mswdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\include" /i "..\lib\vc_mswdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswdll\wxmsw250_core_vc_custom.dll" /libpath:"..\lib\vc_mswdll" /implib:"..\lib\vc_mswdll\wxmsw25_core.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswdll\wxmsw250_core_vc_custom.dll" /libpath:"..\lib\vc_mswdll" /implib:"..\lib\vc_mswdll\wxmsw25_core.lib"

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivuddll\"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\coredll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivuddll\"
# PROP Intermediate_Dir "vc_mswunivuddll\coredll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_coredll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxmswuniv250ud_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_coredll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxmswuniv250ud_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivuddll\wxmswuniv250ud_core_vc_custom.dll" /libpath:"..\lib\vc_mswunivuddll" /implib:"..\lib\vc_mswunivuddll\wxmswuniv25ud_core.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivuddll\wxmswuniv250ud_core_vc_custom.dll" /libpath:"..\lib\vc_mswunivuddll" /implib:"..\lib\vc_mswunivuddll\wxmswuniv25ud_core.lib" /debug

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivudll\"
# PROP BASE Intermediate_Dir "vc_mswunivudll\coredll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivudll\"
# PROP Intermediate_Dir "vc_mswunivudll\coredll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswunivudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_coredll.pch" /Fd..\lib\vc_mswunivudll\wxmswuniv250u_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswunivudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_coredll.pch" /Fd..\lib\vc_mswunivudll\wxmswuniv250u_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivudll\wxmswuniv250u_core_vc_custom.dll" /libpath:"..\lib\vc_mswunivudll" /implib:"..\lib\vc_mswunivudll\wxmswuniv25u_core.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivudll\wxmswuniv250u_core_vc_custom.dll" /libpath:"..\lib\vc_mswunivudll" /implib:"..\lib\vc_mswunivudll\wxmswuniv25u_core.lib"

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswddll\"
# PROP BASE Intermediate_Dir "vc_mswddll\coredll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswddll\"
# PROP Intermediate_Dir "vc_mswddll\coredll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_coredll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxmsw250d_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_coredll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxmsw250d_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswddll\wxmsw250d_core_vc_custom.dll" /libpath:"..\lib\vc_mswddll" /implib:"..\lib\vc_mswddll\wxmsw25d_core.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswddll\wxmsw250d_core_vc_custom.dll" /libpath:"..\lib\vc_mswddll" /implib:"..\lib\vc_mswddll\wxmsw25d_core.lib" /debug

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivdll\"
# PROP BASE Intermediate_Dir "vc_mswunivdll\coredll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivdll\"
# PROP Intermediate_Dir "vc_mswunivdll\coredll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswunivdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_coredll.pch" /Fd..\lib\vc_mswunivdll\wxmswuniv250_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswunivdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_coredll.pch" /Fd..\lib\vc_mswunivdll\wxmswuniv250_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\include" /i "..\lib\vc_mswunivdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\include" /i "..\lib\vc_mswunivdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivdll\wxmswuniv250_core_vc_custom.dll" /libpath:"..\lib\vc_mswunivdll" /implib:"..\lib\vc_mswunivdll\wxmswuniv25_core.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivdll\wxmswuniv250_core_vc_custom.dll" /libpath:"..\lib\vc_mswunivdll" /implib:"..\lib\vc_mswunivdll\wxmswuniv25_core.lib"

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivddll\"
# PROP BASE Intermediate_Dir "vc_mswunivddll\coredll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivddll\"
# PROP Intermediate_Dir "vc_mswunivddll\coredll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_coredll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxmswuniv250d_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_coredll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxmswuniv250d_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswunivddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswunivddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivddll\wxmswuniv250d_core_vc_custom.dll" /libpath:"..\lib\vc_mswunivddll" /implib:"..\lib\vc_mswunivddll\wxmswuniv25d_core.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivddll\wxmswuniv250d_core_vc_custom.dll" /libpath:"..\lib\vc_mswunivddll" /implib:"..\lib\vc_mswunivddll\wxmswuniv25d_core.lib" /debug

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswudll\"
# PROP BASE Intermediate_Dir "vc_mswudll\coredll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswudll\"
# PROP Intermediate_Dir "vc_mswudll\coredll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_coredll.pch" /Fd..\lib\vc_mswudll\wxmsw250u_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_coredll.pch" /Fd..\lib\vc_mswudll\wxmsw250u_core_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_CORE" /D wxUSE_BASE=0 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d "WXMAKINGDLL_CORE" /d wxUSE_BASE=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswudll\wxmsw250u_core_vc_custom.dll" /libpath:"..\lib\vc_mswudll" /implib:"..\lib\vc_mswudll\wxmsw25u_core.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswudll\wxmsw250u_core_vc_custom.dll" /libpath:"..\lib\vc_mswudll" /implib:"..\lib\vc_mswudll\wxmsw25u_core.lib"

!ENDIF

# Begin Target

# Name "coredll - Win32 DLL Native Debug Unicode"
# Name "coredll - Win32 DLL Native Release Ansi"
# Name "coredll - Win32 DLL wxUniv Debug Unicode"
# Name "coredll - Win32 DLL wxUniv Release Unicode"
# Name "coredll - Win32 DLL Native Debug Ansi"
# Name "coredll - Win32 DLL wxUniv Release Ansi"
# Name "coredll - Win32 DLL wxUniv Debug Ansi"
# Name "coredll - Win32 DLL Native Release Unicode"
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\accel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\access.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\afterstd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\artprov.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\brush.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\busyinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\button.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\caret.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\checklst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\choicdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\choice.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\clipbrd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cmdproc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cmndata.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\colordlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\colour.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\combobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\control.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cshelp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ctrlsub.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cursor.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dataobj.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcclient.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcmemory.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcmirror.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcprint.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcps.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcscreen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dialog.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dialup.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dirctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dirdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\display.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dnd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\docmdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\docview.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dragimag.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\effects.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\evtloop.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fdrepdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fmappriv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\font.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontenum.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontutil.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\frame.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gauge.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gdicmn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gdiobj.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\accel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\choicdgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\colrdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dcpsg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dirctrlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dirdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dragimgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\fdrepdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\filedlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\fontdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\imaglist.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\listctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\logg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\mdig.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\msgdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\notebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\panelg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\prntdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\progdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\scrolwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\spinctlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\splitter.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\statusbr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\tabg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\textdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\timer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\treectlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\geometry.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gifdecod.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\glcanvas.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\help.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\helpbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\helphtml.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\helpwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\icon.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\iconbndl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\image.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imaggif.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagiff.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagjpeg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imaglist.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagpcx.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagpng.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagpnm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagtiff.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagxpm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\layout.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\listbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\listbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\listctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\matrix.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\menu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\metafile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\minifram.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msgdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\accel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\app.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\brush.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\button.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\caret.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\checklst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\choice.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\clipbrd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\colordlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\colour.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\combobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\control.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\cursor.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcclient.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcmemory.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcprint.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcscreen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dialog.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dib.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dirdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dragimag.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\enhmeta.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\fdrepdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\font.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\fontdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\frame.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gauge95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gdiimage.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gdiobj.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gnuwin32\winresrc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\helpbest.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\helpchm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\helpwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\icon.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\imaglist.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\iniconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\listbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\listctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\mdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\menu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\metafile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\minifram.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\missing.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\msgdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\mslu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\msvcrt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\notebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\access.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\automtn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\dataform.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\dataobj.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\dataobj2.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\dropsrc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\droptgt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\oleutils.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\uuid.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\palette.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\pen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\printdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\printwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\private.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\regconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\region.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\registry.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\setup0.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\slider95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\slidrmsw.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\spinbutt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\spinctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statbr95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\stattext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tabctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tbar95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tbarmsw.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\timer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tooltip.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\toplevel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\treectrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\window.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\notebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ownerdrw.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\palette.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\panel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\paper.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\pen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\popupwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\print.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\printdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\prntbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\progdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ptr_scpd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\quantize.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\rawbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\region.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\renderer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\scopeguard.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\scrolwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\selstore.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\settings.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sizer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\slider.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\spinbutt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\spinctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\splitter.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stack.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stattext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statusbr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tab.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tabctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tbarbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tbarsmpl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\timer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tipwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\toolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tooltip.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\toplevel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\treebase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\treectrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\app.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\button.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\checklst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\choice.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\colschem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\combobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\control.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\dialog.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\frame.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\gauge.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\inpcons.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\inphand.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\listbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\menu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\notebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\renderer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\scrarrow.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\scrthumb.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\scrtimer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\slider.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\spinbutt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\statbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\statline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\stattext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\statusbr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\theme.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\toolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\toplevel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\univ\window.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\valgen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\validate.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\valtext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\vlbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\vms_x_fix.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\vscroll.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\window.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xpmdecod.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xpmhand.h
# End Source File
# End Group
# Begin Group "Generic Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\generic\accel.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\busyinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\choicdgg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\colrdlgg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\dcpsg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\dirctrlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\dirdlgg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\dragimgg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\fdrepdlg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\filedlgg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\fontdlgg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\imaglist.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\listctrl.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\logg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\mdig.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\msgdlgg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\numdlgg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\panelg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\prntdlgg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\progdlgg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\renderg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\scrlwing.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\selstore.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\spinctlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\splitter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\statusbr.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\tabg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\generic\tbarsmpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\textdlgg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\tipwin.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\treectlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\vlbox.cpp
# End Source File
# Begin Source File

SOURCE=..\src\generic\vscroll.cpp
# End Source File
# End Group
# Begin Group "wxUniv Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\univ\bmpbuttn.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\button.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\checkbox.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\checklst.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\choice.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\colschem.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\combobox.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\control.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\ctrlrend.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\dialog.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\framuniv.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\gauge.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\inpcons.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\inphand.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\listbox.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\menu.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\notebook.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\radiobox.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\radiobut.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\scrarrow.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\scrolbar.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\scrthumb.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\slider.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\spinbutt.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\statbmp.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\statbox.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\statline.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\stattext.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\statusbr.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\textctrl.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\theme.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\themes\gtk.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\themes\metal.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\themes\win32.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\toolbar.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\topluniv.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\univ\winuniv.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\common\accesscmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\appcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\artprov.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\artstd.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\bmpbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\choiccmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\clipcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\cmdproc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\cmndata.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\containr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\cshelp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\ctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\ctrlsub.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\datacmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dcbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dndcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dobjcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\docmdi.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\docview.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dpycmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dseldlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\effects.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\event.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\fddlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\fldlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\fontcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\fontmap.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\framecmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\fs_mem.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\gaugecmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\gdicmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\geometry.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\gifdecod.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\helpbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\iconbndl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imagall.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imagbmp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\image.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imagfill.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imaggif.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imagiff.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imagjpeg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imagpcx.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imagpng.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imagpnm.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imagtiff.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\imagxpm.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\layout.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\lboxcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\menucmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\msgout.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\nbkbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\paper.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\popupcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\prntbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\quantize.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\radiocmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\rendcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\rgncmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\settcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\sizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\statbar.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\tbarbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\textcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\timercmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\toplvcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\treebase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\utilscmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\valgen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\validate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\valtext.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\wincmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\xpmdecod.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\msw\accel.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\app.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\bmpbuttn.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\brush.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\button.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\caret.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\checkbox.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\checklst.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\choice.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\clipbrd.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\colordlg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\colour.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\combobox.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\control.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\cursor.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\data.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dcclient.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dcmemory.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dcprint.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dcscreen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dialog.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\dialup.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dib.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dirdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\display.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dragimag.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=..\src\msw\enhmeta.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\evtloop.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\fdrepdlg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\filedlg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\font.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\fontdlg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\fontenum.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\fontutil.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\frame.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\gauge95.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\gdiimage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\gdiobj.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\helpchm.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\helpwin.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\icon.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\imaglist.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\iniconf.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\listbox.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\listctrl.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\main.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\mdi.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\menu.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\menuitem.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\metafile.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\minifram.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\msgdlg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\mslu.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\nativdlg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\notebook.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\ole\access.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\ole\automtn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\ole\dataobj.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\ole\dropsrc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\ole\droptgt.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\ole\oleutils.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\ole\uuid.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\ownerdrw.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\palette.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\pen.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\penwin.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\popupwin.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\printdlg.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\printwin.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\radiobox.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\radiobut.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\region.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\renderer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\scrolbar.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\slider95.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\spinbutt.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\spinctrl.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\statbmp.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\statbox.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\statbr95.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\statline.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\stattext.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\tabctrl.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\tbar95.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\textctrl.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\tglbtn.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\timer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\tooltip.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\toplevel.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\treectrl.cpp

!IF  "$(CFG)" == "coredll - Win32 DLL Native Debug Unicode"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Unicode"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Debug Ansi"


!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Release Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL wxUniv Debug Ansi"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "coredll - Win32 DLL Native Release Unicode"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\src\msw\utilsgui.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\uxtheme.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\volume.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\window.cpp
# End Source File
# End Group
# End Target
# End Project

