# Microsoft Developer Studio Project File - Name="basedll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=basedll - Win32 DLL Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_basedll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_basedll.mak" CFG="basedll - Win32 DLL Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "basedll - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basedll - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basedll - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basedll - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basedll - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basedll - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basedll - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basedll - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "basedll - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\basedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\basedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxbase250d_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxbase250d_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswddll\wxbase250d_vc_custom.dll" /libpath:"..\..\lib\vc_mswddll" /implib:"..\..\lib\vc_mswddll\wxbase25d.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswddll\wxbase250d_vc_custom.dll" /libpath:"..\..\lib\vc_mswddll" /implib:"..\..\lib\vc_mswddll\wxbase25d.lib" /debug

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\basedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\basedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_basedll.pch" /Fd..\..\lib\vc_mswdll\wxbase250_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_basedll.pch" /Fd..\..\lib\vc_mswdll\wxbase250_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\include" /i "..\..\lib\vc_mswdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\include" /i "..\..\lib\vc_mswdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswdll\wxbase250_vc_custom.dll" /libpath:"..\..\lib\vc_mswdll" /implib:"..\..\lib\vc_mswdll\wxbase25.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswdll\wxbase250_vc_custom.dll" /libpath:"..\..\lib\vc_mswdll" /implib:"..\..\lib\vc_mswdll\wxbase25.lib"

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\basedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\basedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxbase250ud_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxbase250ud_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswuddll\wxbase250ud_vc_custom.dll" /libpath:"..\..\lib\vc_mswuddll" /implib:"..\..\lib\vc_mswuddll\wxbase25ud.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswuddll\wxbase250ud_vc_custom.dll" /libpath:"..\..\lib\vc_mswuddll" /implib:"..\..\lib\vc_mswuddll\wxbase25ud.lib" /debug

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\basedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\basedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_basedll.pch" /Fd..\..\lib\vc_mswudll\wxbase250u_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_basedll.pch" /Fd..\..\lib\vc_mswudll\wxbase250u_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswudll\wxbase250u_vc_custom.dll" /libpath:"..\..\lib\vc_mswudll" /implib:"..\..\lib\vc_mswudll\wxbase25u.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswudll\wxbase250u_vc_custom.dll" /libpath:"..\..\lib\vc_mswudll" /implib:"..\..\lib\vc_mswudll\wxbase25u.lib"

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\basedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\basedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxbase250d_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxbase250d_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswunivddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswunivddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivddll\wxbase250d_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivddll" /implib:"..\..\lib\vc_mswunivddll\wxbase25d.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivddll\wxbase250d_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivddll" /implib:"..\..\lib\vc_mswunivddll\wxbase25d.lib" /debug

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\basedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\basedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_basedll.pch" /Fd..\..\lib\vc_mswunivdll\wxbase250_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_basedll.pch" /Fd..\..\lib\vc_mswunivdll\wxbase250_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\include" /i "..\..\lib\vc_mswunivdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\include" /i "..\..\lib\vc_mswunivdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivdll\wxbase250_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivdll" /implib:"..\..\lib\vc_mswunivdll\wxbase25.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivdll\wxbase250_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivdll" /implib:"..\..\lib\vc_mswunivdll\wxbase25.lib"

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\basedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\basedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxbase250ud_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_basedll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxbase250ud_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivuddll\wxbase250ud_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivuddll" /implib:"..\..\lib\vc_mswunivuddll\wxbase25ud.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivuddll\wxbase250ud_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivuddll" /implib:"..\..\lib\vc_mswunivuddll\wxbase25ud.lib" /debug

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\basedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\basedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_basedll.pch" /Fd..\..\lib\vc_mswunivudll\wxbase250u_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_basedll.pch" /Fd..\..\lib\vc_mswunivudll\wxbase250u_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXMAKINGDLL_BASE" /D wxUSE_BASE=1 /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXMAKINGDLL_BASE" /d wxUSE_BASE=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivudll\wxbase250u_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivudll" /implib:"..\..\lib\vc_mswunivudll\wxbase25u.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivudll\wxbase250u_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivudll" /implib:"..\..\lib\vc_mswunivudll\wxbase25u.lib"

!ENDIF

# Begin Target

# Name "basedll - Win32 DLL Debug"
# Name "basedll - Win32 DLL Release"
# Name "basedll - Win32 DLL Unicode Debug"
# Name "basedll - Win32 DLL Unicode Release"
# Name "basedll - Win32 DLL Universal Debug"
# Name "basedll - Win32 DLL Universal Release"
# Name "basedll - Win32 DLL Universal Unicode Debug"
# Name "basedll - Win32 DLL Universal Unicode Release"
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\common\appbase.cpp
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

SOURCE=..\..\src\common\datetime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\datstrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dircmn.cpp
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

SOURCE=..\..\src\common\process.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\regex.cpp
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

SOURCE=..\..\src\common\unzip.c
# SUBTRACT CPP /YX /Yc /Yu
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

SOURCE=..\..\src\msw\dir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
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

SOURCE=..\..\src\msw\regconf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\registry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\snglinst.cpp
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

SOURCE=..\..\src\msw\volume.cpp
# End Source File
# End Group
# Begin Group "Setup Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\setup.h

!IF  "$(CFG)" == "basedll - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_mswddll\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_mswddll\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_mswddll\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_mswdll\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_mswdll\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_mswdll\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_mswuddll\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_mswuddll\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_mswuddll\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_mswudll\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_mswudll\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_mswudll\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_mswunivddll\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_mswunivddll\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_mswunivddll\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_mswunivdll\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_mswunivdll\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_mswunivdll\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_mswunivuddll\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_mswunivuddll\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_mswunivuddll\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "basedll - Win32 DLL Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_mswunivudll\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_mswunivudll\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_mswunivudll\wx\setup.h

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "MSW Headers"

# PROP Default_Filter ""
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

SOURCE=..\..\include\wx\msw\gccpriv.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\winundef.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\wrapcctl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\wrapwin.h
# End Source File
# End Group
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\app.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\apptrait.h
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

SOURCE=..\..\include\wx\features.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\ffile.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\file.h
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

SOURCE=..\..\include\wx\process.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\regex.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\snglinst.h
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

SOURCE=..\..\include\wx\tokenzr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\txtstrm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\types.h
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

SOURCE=..\..\include\wx\zipstrm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\zstream.h
# End Source File
# End Group
# End Target
# End Project

