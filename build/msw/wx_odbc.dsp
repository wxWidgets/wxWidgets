# Microsoft Developer Studio Project File - Name="odbc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=odbc - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_odbc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_odbc.mak" CFG="odbc - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "odbc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "odbc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "odbc - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "odbc - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "odbc - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "odbc - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "odbc - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "odbc - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "odbc - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbc - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbc - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbc - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbc - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbc - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbc - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "odbc - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "odbc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswd" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswd\wxbase25d_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswd" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswd\wxbase25d_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswd\wxbase25d_odbc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswd\wxbase25d_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_msw" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_msw\wxbase25_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_msw" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_msw\wxbase25_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_msw\wxbase25_odbc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_msw\wxbase25_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswud" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswud\wxbase25ud_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswud" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswud\wxbase25ud_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswud\wxbase25ud_odbc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswud\wxbase25ud_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswu" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswu\wxbase25u_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswu" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswu\wxbase25u_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswu\wxbase25u_odbc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswu\wxbase25u_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivd" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivd\wxbase25d_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivd" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivd\wxbase25d_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivd\wxbase25d_odbc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivd\wxbase25d_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswuniv" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswuniv\wxbase25_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswuniv" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswuniv\wxbase25_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswuniv\wxbase25_odbc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswuniv\wxbase25_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivud" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivud\wxbase25ud_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivud" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivud\wxbase25ud_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivud\wxbase25ud_odbc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivud\wxbase25ud_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivu" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswunivu\wxbase25u_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivu" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswunivu\wxbase25u_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivu\wxbase25u_odbc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivu\wxbase25u_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxbase250d_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxbase250d_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswddll\wxbase250d_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswddll" /implib:"..\..\lib\vc_mswddll\wxbase25d_odbc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswddll\wxbase250d_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswddll" /implib:"..\..\lib\vc_mswddll\wxbase25d_odbc.lib" /debug

!ELSEIF  "$(CFG)" == "odbc - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_odbcdll.pch" /Fd..\..\lib\vc_mswdll\wxbase250_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_odbcdll.pch" /Fd..\..\lib\vc_mswdll\wxbase250_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\include" /i "..\..\lib\vc_mswdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\include" /i "..\..\lib\vc_mswdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswdll\wxbase250_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswdll" /implib:"..\..\lib\vc_mswdll\wxbase25_odbc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswdll\wxbase250_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswdll" /implib:"..\..\lib\vc_mswdll\wxbase25_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxbase250ud_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxbase250ud_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswuddll\wxbase250ud_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswuddll" /implib:"..\..\lib\vc_mswuddll\wxbase25ud_odbc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswuddll\wxbase250ud_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswuddll" /implib:"..\..\lib\vc_mswuddll\wxbase25ud_odbc.lib" /debug

!ELSEIF  "$(CFG)" == "odbc - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_odbcdll.pch" /Fd..\..\lib\vc_mswudll\wxbase250u_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_odbcdll.pch" /Fd..\..\lib\vc_mswudll\wxbase250u_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswudll\wxbase250u_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswudll" /implib:"..\..\lib\vc_mswudll\wxbase25u_odbc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswudll\wxbase250u_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswudll" /implib:"..\..\lib\vc_mswudll\wxbase25u_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxbase250d_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxbase250d_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswunivddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswunivddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswunivddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivddll\wxbase250d_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivddll" /implib:"..\..\lib\vc_mswunivddll\wxbase25d_odbc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswunivddll\wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivddll\wxbase250d_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivddll" /implib:"..\..\lib\vc_mswunivddll\wxbase25d_odbc.lib" /debug

!ELSEIF  "$(CFG)" == "odbc - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_odbcdll.pch" /Fd..\..\lib\vc_mswunivdll\wxbase250_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_odbcdll.pch" /Fd..\..\lib\vc_mswunivdll\wxbase250_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\include" /i "..\..\lib\vc_mswunivdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\include" /i "..\..\lib\vc_mswunivdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswunivdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivdll\wxbase250_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivdll" /implib:"..\..\lib\vc_mswunivdll\wxbase25_odbc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswunivdll\wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivdll\wxbase250_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivdll" /implib:"..\..\lib\vc_mswunivdll\wxbase25_odbc.lib"

!ELSEIF  "$(CFG)" == "odbc - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxbase250ud_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_odbcdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxbase250ud_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswunivuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivuddll\wxbase250ud_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivuddll" /implib:"..\..\lib\vc_mswunivuddll\wxbase25ud_odbc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswunivuddll\wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivuddll\wxbase250ud_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivuddll" /implib:"..\..\lib\vc_mswunivuddll\wxbase25ud_odbc.lib" /debug

!ELSEIF  "$(CFG)" == "odbc - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\odbc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\odbc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_odbcdll.pch" /Fd..\..\lib\vc_mswunivudll\wxbase250u_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_odbcdll.pch" /Fd..\..\lib\vc_mswunivudll\wxbase250u_odbc_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_GUI=0 /D "WXUSINGDLL" /D "WXMAKINGDLL_ODBC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d wxUSE_GUI=0 /d "WXUSINGDLL" /d WXMAKINGDLL_ODBC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswunivudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivudll\wxbase250u_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivudll" /implib:"..\..\lib\vc_mswunivudll\wxbase25u_odbc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib ..\..\lib\vc_mswunivudll\wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivudll\wxbase250u_odbc_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivudll" /implib:"..\..\lib\vc_mswunivudll\wxbase25u_odbc.lib"

!ENDIF

# Begin Target

# Name "odbc - Win32 Debug"
# Name "odbc - Win32 Release"
# Name "odbc - Win32 Unicode Debug"
# Name "odbc - Win32 Unicode Release"
# Name "odbc - Win32 Universal Debug"
# Name "odbc - Win32 Universal Release"
# Name "odbc - Win32 Universal Unicode Debug"
# Name "odbc - Win32 Universal Unicode Release"
# Name "odbc - Win32 DLL Debug"
# Name "odbc - Win32 DLL Release"
# Name "odbc - Win32 DLL Unicode Debug"
# Name "odbc - Win32 DLL Unicode Release"
# Name "odbc - Win32 DLL Universal Debug"
# Name "odbc - Win32 DLL Universal Release"
# Name "odbc - Win32 DLL Universal Unicode Debug"
# Name "odbc - Win32 DLL Universal Unicode Release"
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\common\db.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dbtable.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# End Group
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\db.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dbkeyg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dbtable.h
# End Source File
# End Group
# End Target
# End Project

