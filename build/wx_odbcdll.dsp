# Microsoft Developer Studio Project File - Name="odbcdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=odbcdll - Win32 DLL Native Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_odbcdll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_odbcdll.mak" CFG="odbcdll - Win32 DLL Native Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "odbcdll - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbcdll - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbcdll - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbcdll - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbcdll - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbcdll - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbcdll - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbcdll - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "odbcdll - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswuddll\"
# PROP BASE Intermediate_Dir "vc_mswuddll\odbcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswuddll\"
# PROP Intermediate_Dir "vc_mswuddll\odbcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxmsw250ud_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxmsw250ud_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswuddll\wxmsw25ud_adv.lib ..\lib\vc_mswuddll\wxmsw25ud_core.lib ..\lib\vc_mswuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswuddll\wxmsw250ud_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswuddll" /implib:"..\lib\vc_mswuddll\wxmsw25ud_odbc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswuddll\wxmsw25ud_adv.lib ..\lib\vc_mswuddll\wxmsw25ud_core.lib ..\lib\vc_mswuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswuddll\wxmsw250ud_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswuddll" /implib:"..\lib\vc_mswuddll\wxmsw25ud_odbc.lib" /debug

!ELSEIF  "$(CFG)" == "odbcdll - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswdll\"
# PROP BASE Intermediate_Dir "vc_mswdll\odbcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswdll\"
# PROP Intermediate_Dir "vc_mswdll\odbcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_odbcdll.pch" /Fd..\lib\vc_mswdll\wxmsw250_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_odbcdll.pch" /Fd..\lib\vc_mswdll\wxmsw250_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\include" /i "..\lib\vc_mswdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\include" /i "..\lib\vc_mswdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswdll\wxmsw25_adv.lib ..\lib\vc_mswdll\wxmsw25_core.lib ..\lib\vc_mswdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswdll\wxmsw250_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswdll" /implib:"..\lib\vc_mswdll\wxmsw25_odbc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswdll\wxmsw25_adv.lib ..\lib\vc_mswdll\wxmsw25_core.lib ..\lib\vc_mswdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswdll\wxmsw250_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswdll" /implib:"..\lib\vc_mswdll\wxmsw25_odbc.lib"

!ELSEIF  "$(CFG)" == "odbcdll - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivuddll\"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\odbcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivuddll\"
# PROP Intermediate_Dir "vc_mswunivuddll\odbcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxmswuniv250ud_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivuddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxmswuniv250ud_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivuddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivuddll\wxmswuniv25ud_adv.lib ..\lib\vc_mswunivuddll\wxmswuniv25ud_core.lib ..\lib\vc_mswunivuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivuddll\wxmswuniv250ud_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswunivuddll" /implib:"..\lib\vc_mswunivuddll\wxmswuniv25ud_odbc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivuddll\wxmswuniv25ud_adv.lib ..\lib\vc_mswunivuddll\wxmswuniv25ud_core.lib ..\lib\vc_mswunivuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivuddll\wxmswuniv250ud_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswunivuddll" /implib:"..\lib\vc_mswunivuddll\wxmswuniv25ud_odbc.lib" /debug

!ELSEIF  "$(CFG)" == "odbcdll - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivudll\"
# PROP BASE Intermediate_Dir "vc_mswunivudll\odbcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivudll\"
# PROP Intermediate_Dir "vc_mswunivudll\odbcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswunivudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_odbcdll.pch" /Fd..\lib\vc_mswunivudll\wxmswuniv250u_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswunivudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_odbcdll.pch" /Fd..\lib\vc_mswunivudll\wxmswuniv250u_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswunivudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivudll\wxmswuniv25u_adv.lib ..\lib\vc_mswunivudll\wxmswuniv25u_core.lib ..\lib\vc_mswunivudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivudll\wxmswuniv250u_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswunivudll" /implib:"..\lib\vc_mswunivudll\wxmswuniv25u_odbc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivudll\wxmswuniv25u_adv.lib ..\lib\vc_mswunivudll\wxmswuniv25u_core.lib ..\lib\vc_mswunivudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivudll\wxmswuniv250u_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswunivudll" /implib:"..\lib\vc_mswunivudll\wxmswuniv25u_odbc.lib"

!ELSEIF  "$(CFG)" == "odbcdll - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswddll\"
# PROP BASE Intermediate_Dir "vc_mswddll\odbcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswddll\"
# PROP Intermediate_Dir "vc_mswddll\odbcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxmsw250d_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxmsw250d_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswddll\wxmsw25d_adv.lib ..\lib\vc_mswddll\wxmsw25d_core.lib ..\lib\vc_mswddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswddll\wxmsw250d_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswddll" /implib:"..\lib\vc_mswddll\wxmsw25d_odbc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswddll\wxmsw25d_adv.lib ..\lib\vc_mswddll\wxmsw25d_core.lib ..\lib\vc_mswddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswddll\wxmsw250d_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswddll" /implib:"..\lib\vc_mswddll\wxmsw25d_odbc.lib" /debug

!ELSEIF  "$(CFG)" == "odbcdll - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivdll\"
# PROP BASE Intermediate_Dir "vc_mswunivdll\odbcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivdll\"
# PROP Intermediate_Dir "vc_mswunivdll\odbcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswunivdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_odbcdll.pch" /Fd..\lib\vc_mswunivdll\wxmswuniv250_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswunivdll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_odbcdll.pch" /Fd..\lib\vc_mswunivdll\wxmswuniv250_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\include" /i "..\lib\vc_mswunivdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\include" /i "..\lib\vc_mswunivdll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivdll\wxmswuniv25_adv.lib ..\lib\vc_mswunivdll\wxmswuniv25_core.lib ..\lib\vc_mswunivdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivdll\wxmswuniv250_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswunivdll" /implib:"..\lib\vc_mswunivdll\wxmswuniv25_odbc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivdll\wxmswuniv25_adv.lib ..\lib\vc_mswunivdll\wxmswuniv25_core.lib ..\lib\vc_mswunivdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivdll\wxmswuniv250_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswunivdll" /implib:"..\lib\vc_mswunivdll\wxmswuniv25_odbc.lib"

!ELSEIF  "$(CFG)" == "odbcdll - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivddll\"
# PROP BASE Intermediate_Dir "vc_mswunivddll\odbcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivddll\"
# PROP Intermediate_Dir "vc_mswunivddll\odbcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxmswuniv250d_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivddll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxmswuniv250d_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswunivddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\include" /i "..\lib\vc_mswunivddll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivddll\wxmswuniv25d_adv.lib ..\lib\vc_mswunivddll\wxmswuniv25d_core.lib ..\lib\vc_mswunivddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivddll\wxmswuniv250d_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswunivddll" /implib:"..\lib\vc_mswunivddll\wxmswuniv25d_odbc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswunivddll\wxmswuniv25d_adv.lib ..\lib\vc_mswunivddll\wxmswuniv25d_core.lib ..\lib\vc_mswunivddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswunivddll\wxmswuniv250d_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswunivddll" /implib:"..\lib\vc_mswunivddll\wxmswuniv25d_odbc.lib" /debug

!ELSEIF  "$(CFG)" == "odbcdll - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswudll\"
# PROP BASE Intermediate_Dir "vc_mswudll\odbcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswudll\"
# PROP Intermediate_Dir "vc_mswudll\odbcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_odbcdll.pch" /Fd..\lib\vc_mswudll\wxmsw250u_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswudll" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_odbcdll.pch" /Fd..\lib\vc_mswudll\wxmsw250u_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\include" /i "..\lib\vc_mswudll" /i "..\src\tiff" /i "..\src\jpeg" /i "..\src\png" /i "..\src\zlib" /i "..\src\regex" /i "..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswudll\wxmsw25u_adv.lib ..\lib\vc_mswudll\wxmsw25u_core.lib ..\lib\vc_mswudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswudll\wxmsw250u_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswudll" /implib:"..\lib\vc_mswudll\wxmsw25u_odbc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\lib\vc_mswudll\wxmsw25u_adv.lib ..\lib\vc_mswudll\wxmsw25u_core.lib ..\lib\vc_mswudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\lib\vc_mswudll\wxmsw250u_odbc_vc_custom.dll" /libpath:"..\lib\vc_mswudll" /implib:"..\lib\vc_mswudll\wxmsw25u_odbc.lib"

!ENDIF

# Begin Target

# Name "odbcdll - Win32 DLL Native Debug Unicode"
# Name "odbcdll - Win32 DLL Native Release Ansi"
# Name "odbcdll - Win32 DLL wxUniv Debug Unicode"
# Name "odbcdll - Win32 DLL wxUniv Release Unicode"
# Name "odbcdll - Win32 DLL Native Debug Ansi"
# Name "odbcdll - Win32 DLL wxUniv Release Ansi"
# Name "odbcdll - Win32 DLL wxUniv Debug Ansi"
# Name "odbcdll - Win32 DLL Native Release Unicode"
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\db.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbgrid.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbkeyg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbtable.h
# End Source File
# End Group
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\common\db.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dbgrid.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dbtable.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# End Group
# End Target
# End Project

