# Microsoft Developer Studio Project File - Name="htmldll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=htmldll - Win32 DLL Native Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_htmldll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_htmldll.mak" CFG="htmldll - Win32 DLL Native Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "htmldll - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "htmldll - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "htmldll - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "htmldll - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "htmldll - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "htmldll - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "htmldll - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "htmldll - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "htmldll - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswuddll\"
# PROP BASE Intermediate_Dir "vc_mswuddll\htmldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswuddll\"
# PROP Intermediate_Dir "vc_mswuddll\htmldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_htmldll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxmsw250ud_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_htmldll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxmsw250ud_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "_DEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "_DEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswuddll\wxmsw25ud_core.lib ..\lib\vc_mswuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswuddll\wxmsw250ud_html_vc_custom.dll" /libpath:"..\lib\vc_mswuddll" /implib:"..\lib\vc_mswuddll\wxmsw25ud_html.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswuddll\wxmsw25ud_core.lib ..\lib\vc_mswuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswuddll\wxmsw250ud_html_vc_custom.dll" /libpath:"..\lib\vc_mswuddll" /implib:"..\lib\vc_mswuddll\wxmsw25ud_html.lib" /debug

!ELSEIF  "$(CFG)" == "htmldll - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswdll\"
# PROP BASE Intermediate_Dir "vc_mswdll\htmldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswdll\"
# PROP Intermediate_Dir "vc_mswdll\htmldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_htmldll.pch" /Fd..\lib\vc_mswdll\wxmsw250_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_htmldll.pch" /Fd..\lib\vc_mswdll\wxmsw250_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\include" /i "..\lib\vc_mswdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "NDEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\include" /i "..\lib\vc_mswdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "NDEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswdll\wxmsw25_core.lib ..\lib\vc_mswdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswdll\wxmsw250_html_vc_custom.dll" /libpath:"..\lib\vc_mswdll" /implib:"..\lib\vc_mswdll\wxmsw25_html.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswdll\wxmsw25_core.lib ..\lib\vc_mswdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswdll\wxmsw250_html_vc_custom.dll" /libpath:"..\lib\vc_mswdll" /implib:"..\lib\vc_mswdll\wxmsw25_html.lib"

!ELSEIF  "$(CFG)" == "htmldll - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivuddll\"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\htmldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivuddll\"
# PROP Intermediate_Dir "vc_mswunivuddll\htmldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_htmldll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxmswuniv250ud_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_htmldll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxmswuniv250ud_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "_DEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "_DEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivuddll\wxmswuniv25ud_core.lib ..\lib\vc_mswunivuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivuddll\wxmswuniv250ud_html_vc_custom.dll" /libpath:"..\lib\vc_mswunivuddll" /implib:"..\lib\vc_mswunivuddll\wxmswuniv25ud_html.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivuddll\wxmswuniv25ud_core.lib ..\lib\vc_mswunivuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivuddll\wxmswuniv250ud_html_vc_custom.dll" /libpath:"..\lib\vc_mswunivuddll" /implib:"..\lib\vc_mswunivuddll\wxmswuniv25ud_html.lib" /debug

!ELSEIF  "$(CFG)" == "htmldll - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivudll\"
# PROP BASE Intermediate_Dir "vc_mswunivudll\htmldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivudll\"
# PROP Intermediate_Dir "vc_mswunivudll\htmldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswunivudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_htmldll.pch" /Fd..\lib\vc_mswunivudll\wxmswuniv250u_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswunivudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_htmldll.pch" /Fd..\lib\vc_mswunivudll\wxmswuniv250u_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "NDEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "NDEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivudll\wxmswuniv25u_core.lib ..\lib\vc_mswunivudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivudll\wxmswuniv250u_html_vc_custom.dll" /libpath:"..\lib\vc_mswunivudll" /implib:"..\lib\vc_mswunivudll\wxmswuniv25u_html.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivudll\wxmswuniv25u_core.lib ..\lib\vc_mswunivudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivudll\wxmswuniv250u_html_vc_custom.dll" /libpath:"..\lib\vc_mswunivudll" /implib:"..\lib\vc_mswunivudll\wxmswuniv25u_html.lib"

!ELSEIF  "$(CFG)" == "htmldll - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswddll\"
# PROP BASE Intermediate_Dir "vc_mswddll\htmldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswddll\"
# PROP Intermediate_Dir "vc_mswddll\htmldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_htmldll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxmsw250d_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXDEBUG__" /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_htmldll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxmsw250d_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXDEBUG__" /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXDEBUG__" /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXDEBUG__" /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "_DEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "_DEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswddll\wxmsw25d_core.lib ..\lib\vc_mswddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswddll\wxmsw250d_html_vc_custom.dll" /libpath:"..\lib\vc_mswddll" /implib:"..\lib\vc_mswddll\wxmsw25d_html.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswddll\wxmsw25d_core.lib ..\lib\vc_mswddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswddll\wxmsw250d_html_vc_custom.dll" /libpath:"..\lib\vc_mswddll" /implib:"..\lib\vc_mswddll\wxmsw25d_html.lib" /debug

!ELSEIF  "$(CFG)" == "htmldll - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivdll\"
# PROP BASE Intermediate_Dir "vc_mswunivdll\htmldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivdll\"
# PROP Intermediate_Dir "vc_mswunivdll\htmldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswunivdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_htmldll.pch" /Fd..\lib\vc_mswunivdll\wxmswuniv250_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswunivdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_htmldll.pch" /Fd..\lib\vc_mswunivdll\wxmswuniv250_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\include" /i "..\lib\vc_mswunivdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "NDEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\include" /i "..\lib\vc_mswunivdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "NDEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivdll\wxmswuniv25_core.lib ..\lib\vc_mswunivdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivdll\wxmswuniv250_html_vc_custom.dll" /libpath:"..\lib\vc_mswunivdll" /implib:"..\lib\vc_mswunivdll\wxmswuniv25_html.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivdll\wxmswuniv25_core.lib ..\lib\vc_mswunivdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivdll\wxmswuniv250_html_vc_custom.dll" /libpath:"..\lib\vc_mswunivdll" /implib:"..\lib\vc_mswunivdll\wxmswuniv25_html.lib"

!ELSEIF  "$(CFG)" == "htmldll - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivddll\"
# PROP BASE Intermediate_Dir "vc_mswunivddll\htmldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivddll\"
# PROP Intermediate_Dir "vc_mswunivddll\htmldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_htmldll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxmswuniv250d_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_htmldll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxmswuniv250d_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_DEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswunivddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "_DEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswunivddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "_DEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivddll\wxmswuniv25d_core.lib ..\lib\vc_mswunivddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivddll\wxmswuniv250d_html_vc_custom.dll" /libpath:"..\lib\vc_mswunivddll" /implib:"..\lib\vc_mswunivddll\wxmswuniv25d_html.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivddll\wxmswuniv25d_core.lib ..\lib\vc_mswunivddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivddll\wxmswuniv250d_html_vc_custom.dll" /libpath:"..\lib\vc_mswunivddll" /implib:"..\lib\vc_mswunivddll\wxmswuniv25d_html.lib" /debug

!ELSEIF  "$(CFG)" == "htmldll - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswudll\"
# PROP BASE Intermediate_Dir "vc_mswudll\htmldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswudll\"
# PROP Intermediate_Dir "vc_mswudll\htmldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_htmldll.pch" /Fd..\lib\vc_mswudll\wxmsw250u_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_htmldll.pch" /Fd..\lib\vc_mswudll\wxmsw250u_html_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "NDEBUG" /D "WXUSINGDLL" /D "WXMAKINGDLL_HTML" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "NDEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "NDEBUG" /d "WXUSINGDLL" /d WXMAKINGDLL_HTML
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswudll\wxmsw25u_core.lib ..\lib\vc_mswudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswudll\wxmsw250u_html_vc_custom.dll" /libpath:"..\lib\vc_mswudll" /implib:"..\lib\vc_mswudll\wxmsw25u_html.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswudll\wxmsw25u_core.lib ..\lib\vc_mswudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswudll\wxmsw250u_html_vc_custom.dll" /libpath:"..\lib\vc_mswudll" /implib:"..\lib\vc_mswudll\wxmsw25u_html.lib"

!ENDIF

# Begin Target

# Name "htmldll - Win32 DLL Native Debug Unicode"
# Name "htmldll - Win32 DLL Native Release Ansi"
# Name "htmldll - Win32 DLL wxUniv Debug Unicode"
# Name "htmldll - Win32 DLL wxUniv Release Unicode"
# Name "htmldll - Win32 DLL Native Debug Ansi"
# Name "htmldll - Win32 DLL wxUniv Release Ansi"
# Name "htmldll - Win32 DLL wxUniv Debug Ansi"
# Name "htmldll - Win32 DLL Native Release Unicode"
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\html\forcelnk.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpdata.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpfrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlcell.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmldefs.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlfilt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlpars.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlproc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmltag.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmprint.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\m_templ.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\winpars.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\htmllbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxhtml.h
# End Source File
# End Group
# Begin Group "wxHTML Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\html\helpctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\helpdata.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\helpfrm.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\htmlcell.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\htmlfilt.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\htmlpars.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\htmltag.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\htmlwin.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\htmprint.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\m_dflist.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\m_fonts.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\m_hline.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\m_image.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\m_layout.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\m_links.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\m_list.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\m_pre.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\m_style.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\m_tables.cpp
# End Source File
# Begin Source File

SOURCE=..\src\html\winpars.cpp
# End Source File
# End Group
# Begin Group "Generic Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\generic\htmllbox.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=..\src\msw\helpbest.cpp
# End Source File
# End Group
# End Target
# End Project

