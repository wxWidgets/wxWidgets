# Microsoft Developer Studio Project File - Name="netutilsdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=netutilsdll - Win32 DLL Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "netutils_netutilsdll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "netutils_netutilsdll.mak" CFG="netutilsdll - Win32 DLL Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "netutilsdll - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutilsdll - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutilsdll - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutilsdll - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutilsdll - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutilsdll - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutilsdll - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "netutilsdll - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "netutilsdll - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\netutilsdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\netutilsdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswddll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_mswddll\wxmsw250d_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswddll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_mswddll\wxmsw250d_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswddll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswddll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswddll\wxmsw250d_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswddll" /implib:"..\..\src\net\..\..\..\lib\vc_mswddll\wxmsw25d_netutils.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswddll\wxmsw250d_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswddll" /implib:"..\..\src\net\..\..\..\lib\vc_mswddll\wxmsw25d_netutils.lib" /debug

!ELSEIF  "$(CFG)" == "netutilsdll - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\netutilsdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\netutilsdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswdll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_mswdll\wxmsw250_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswdll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_mswdll\wxmsw250_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswdll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswdll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswdll\wxmsw250_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswdll" /implib:"..\..\src\net\..\..\..\lib\vc_mswdll\wxmsw25_netutils.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswdll\wxmsw250_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswdll" /implib:"..\..\src\net\..\..\..\lib\vc_mswdll\wxmsw25_netutils.lib"

!ELSEIF  "$(CFG)" == "netutilsdll - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\netutilsdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\netutilsdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswuddll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_mswuddll\wxmsw250ud_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswuddll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_mswuddll\wxmsw250ud_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswuddll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswuddll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswuddll\wxmsw250ud_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\net\..\..\..\lib\vc_mswuddll\wxmsw25ud_netutils.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswuddll\wxmsw250ud_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\net\..\..\..\lib\vc_mswuddll\wxmsw25ud_netutils.lib" /debug

!ELSEIF  "$(CFG)" == "netutilsdll - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\netutilsdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\netutilsdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswudll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_mswudll\wxmsw250u_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswudll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_mswudll\wxmsw250u_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswudll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswudll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswudll\wxmsw250u_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswudll" /implib:"..\..\src\net\..\..\..\lib\vc_mswudll\wxmsw25u_netutils.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswudll\wxmsw250u_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswudll" /implib:"..\..\src\net\..\..\..\lib\vc_mswudll\wxmsw25u_netutils.lib"

!ELSEIF  "$(CFG)" == "netutilsdll - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\netutilsdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\netutilsdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswunivddll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswunivddll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswunivddll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswunivddll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\net\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_netutils.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\net\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_netutils.lib" /debug

!ELSEIF  "$(CFG)" == "netutilsdll - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\netutilsdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\netutilsdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswunivdll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_mswunivdll\wxmswuniv250_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswunivdll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_mswunivdll\wxmswuniv250_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswunivdll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswunivdll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswunivdll\wxmswuniv250_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\net\..\..\..\lib\vc_mswunivdll\wxmswuniv25_netutils.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswunivdll\wxmswuniv250_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\net\..\..\..\lib\vc_mswunivdll\wxmswuniv25_netutils.lib"

!ELSEIF  "$(CFG)" == "netutilsdll - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\netutilsdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\netutilsdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_netutilsdll.pch" /Zi /Gm /GZ /Fd..\..\src\net\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\net\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_netutils.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\net\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_netutils.lib" /debug

!ELSEIF  "$(CFG)" == "netutilsdll - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\net\..\..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\netutilsdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\net\..\..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\netutilsdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswunivudll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\net\..\..\..\include" /I "..\..\src\net\..\..\..\lib\vc_mswunivudll" /I "..\..\src\net\..\..\..\src\tiff" /I "..\..\src\net\..\..\..\src\jpeg" /I "..\..\src\net\..\..\..\src\png" /I "..\..\src\net\..\..\..\src\zlib" /I "..\..\src\net\..\..\..\src\regex" /I "..\..\src\net\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_netutilsdll.pch" /Fd..\..\src\net\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_netutils_vc_custom.pdb /I "..\..\src\net\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NETUTILS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswunivudll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\net\..\..\..\include" /i "..\..\src\net\..\..\..\lib\vc_mswunivudll" /i "..\..\src\net\..\..\..\src\tiff" /i "..\..\src\net\..\..\..\src\jpeg" /i "..\..\src\net\..\..\..\src\png" /i "..\..\src\net\..\..\..\src\zlib" /i "..\..\src\net\..\..\..\src\regex" /i "..\..\src\net\..\..\..\src\expat\lib" /i "..\..\src\net\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_NETUTILS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\net\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_netutils.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\net\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_netutils_vc_custom.dll" /libpath:"..\..\src\net\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\net\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_netutils.lib"

!ENDIF

# Begin Target

# Name "netutilsdll - Win32 DLL Debug"
# Name "netutilsdll - Win32 DLL Release"
# Name "netutilsdll - Win32 DLL Unicode Debug"
# Name "netutilsdll - Win32 DLL Unicode Release"
# Name "netutilsdll - Win32 DLL Universal Debug"
# Name "netutilsdll - Win32 DLL Universal Release"
# Name "netutilsdll - Win32 DLL Universal Unicode Debug"
# Name "netutilsdll - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/net\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/net\email.cpp
# End Source File
# Begin Source File

SOURCE=../../src/net\smapi.cpp
# End Source File
# Begin Source File

SOURCE=../../src/net\web.cpp
# End Source File
# End Group
# End Target
# End Project

