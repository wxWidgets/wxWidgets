# Microsoft Developer Studio Project File - Name="xrcdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xrcdll - Win32 DLL Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xrc_xrcdll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xrc_xrcdll.mak" CFG="xrcdll - Win32 DLL Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xrcdll - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrcdll - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrcdll - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrcdll - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrcdll - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrcdll - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrcdll - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrcdll - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xrcdll - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\xrcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\xrcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswddll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_xrcdll.pch" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswddll\wxmsw250d_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswddll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_xrcdll.pch" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswddll\wxmsw250d_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswddll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswddll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_html.lib wxmsw25d_adv.lib wxmsw25d_core.lib wxbase25d_xml.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswddll\wxmsw250d_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswddll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswddll\wxmsw25d_xrc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_html.lib wxmsw25d_adv.lib wxmsw25d_core.lib wxbase25d_xml.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswddll\wxmsw250d_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswddll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswddll\wxmsw25d_xrc.lib" /debug

!ELSEIF  "$(CFG)" == "xrcdll - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\xrcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\xrcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswdll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_xrcdll.pch" /Fd..\..\src\xrc\..\..\..\lib\vc_mswdll\wxmsw250_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswdll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_xrcdll.pch" /Fd..\..\src\xrc\..\..\..\lib\vc_mswdll\wxmsw250_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswdll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswdll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_html.lib wxmsw25_adv.lib wxmsw25_core.lib wxbase25_xml.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswdll\wxmsw250_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswdll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswdll\wxmsw25_xrc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_html.lib wxmsw25_adv.lib wxmsw25_core.lib wxbase25_xml.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswdll\wxmsw250_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswdll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswdll\wxmsw25_xrc.lib"

!ELSEIF  "$(CFG)" == "xrcdll - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\xrcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\xrcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswuddll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_xrcdll.pch" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswuddll\wxmsw250ud_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswuddll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_xrcdll.pch" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswuddll\wxmsw250ud_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswuddll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswuddll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_html.lib wxmsw25ud_adv.lib wxmsw25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswuddll\wxmsw250ud_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswuddll\wxmsw25ud_xrc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_html.lib wxmsw25ud_adv.lib wxmsw25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswuddll\wxmsw250ud_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswuddll\wxmsw25ud_xrc.lib" /debug

!ELSEIF  "$(CFG)" == "xrcdll - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\xrcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\xrcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswudll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_xrcdll.pch" /Fd..\..\src\xrc\..\..\..\lib\vc_mswudll\wxmsw250u_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswudll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_xrcdll.pch" /Fd..\..\src\xrc\..\..\..\lib\vc_mswudll\wxmsw250u_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswudll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswudll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_html.lib wxmsw25u_adv.lib wxmsw25u_core.lib wxbase25u_xml.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswudll\wxmsw250u_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswudll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswudll\wxmsw25u_xrc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_html.lib wxmsw25u_adv.lib wxmsw25u_core.lib wxbase25u_xml.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswudll\wxmsw250u_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswudll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswudll\wxmsw25u_xrc.lib"

!ELSEIF  "$(CFG)" == "xrcdll - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\xrcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\xrcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivddll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_xrcdll.pch" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivddll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_xrcdll.pch" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswunivddll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswunivddll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_html.lib wxmswuniv25d_adv.lib wxmswuniv25d_core.lib wxbase25d_xml.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_xrc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_html.lib wxmswuniv25d_adv.lib wxmswuniv25d_core.lib wxbase25d_xml.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_xrc.lib" /debug

!ELSEIF  "$(CFG)" == "xrcdll - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\xrcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\xrcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivdll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_xrcdll.pch" /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivdll\wxmswuniv250_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivdll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_xrcdll.pch" /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivdll\wxmswuniv250_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswunivdll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswunivdll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_html.lib wxmswuniv25_adv.lib wxmswuniv25_core.lib wxbase25_xml.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivdll\wxmswuniv250_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswunivdll\wxmswuniv25_xrc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_html.lib wxmswuniv25_adv.lib wxmswuniv25_core.lib wxbase25_xml.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivdll\wxmswuniv250_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswunivdll\wxmswuniv25_xrc.lib"

!ELSEIF  "$(CFG)" == "xrcdll - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\xrcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\xrcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_xrcdll.pch" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_xrcdll.pch" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_html.lib wxmswuniv25ud_adv.lib wxmswuniv25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_xrc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_html.lib wxmswuniv25ud_adv.lib wxmswuniv25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_xrc.lib" /debug

!ELSEIF  "$(CFG)" == "xrcdll - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\xrcdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\xrcdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivudll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_xrcdll.pch" /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivudll" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_xrcdll.pch" /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_xrc_vc_custom.pdb /I "..\..\src\xrc\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswunivudll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\xrc\..\..\..\include" /i "..\..\src\xrc\..\..\..\lib\vc_mswunivudll" /i "..\..\src\xrc\..\..\..\src\tiff" /i "..\..\src\xrc\..\..\..\src\jpeg" /i "..\..\src\xrc\..\..\..\src\png" /i "..\..\src\xrc\..\..\..\src\zlib" /i "..\..\src\xrc\..\..\..\src\regex" /i "..\..\src\xrc\..\..\..\src\expat\lib" /i "..\..\src\xrc\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_html.lib wxmswuniv25u_adv.lib wxmswuniv25u_core.lib wxbase25u_xml.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_xrc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_html.lib wxmswuniv25u_adv.lib wxmswuniv25u_core.lib wxbase25u_xml.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_xrc_vc_custom.dll" /libpath:"..\..\src\xrc\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\xrc\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_xrc.lib"

!ENDIF

# Begin Target

# Name "xrcdll - Win32 DLL Debug"
# Name "xrcdll - Win32 DLL Release"
# Name "xrcdll - Win32 DLL Unicode Debug"
# Name "xrcdll - Win32 DLL Unicode Release"
# Name "xrcdll - Win32 DLL Universal Debug"
# Name "xrcdll - Win32 DLL Universal Release"
# Name "xrcdll - Win32 DLL Universal Unicode Debug"
# Name "xrcdll - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/xrc\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_bmp.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_bmpbt.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_bttn.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_cald.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_chckb.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_chckl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_choic.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_combo.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_dlg.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_frame.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_gauge.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_gdctl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_html.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_listb.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_listc.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_menu.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_notbk.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_panel.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_radbt.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_radbx.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_scrol.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_scwin.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_sizer.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_slidr.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_spin.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_split.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_stbmp.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_stbox.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_stlin.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_sttxt.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_text.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_toolb.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_tree.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_unkwn.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_wizrd.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xmlres.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xmlrsall.cpp
# End Source File
# End Group
# End Target
# End Project

