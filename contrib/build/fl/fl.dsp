# Microsoft Developer Studio Project File - Name="fl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=fl - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fl.mak" CFG="fl - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fl - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "fl - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswd" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswd\wxmsw25d_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswd" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswd\wxmsw25d_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswd\wxmsw25d_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswd\wxmsw25d_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_msw" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Fd..\..\src\fl\..\..\..\lib\vc_msw\wxmsw25_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_msw" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Fd..\..\src\fl\..\..\..\lib\vc_msw\wxmsw25_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_msw\wxmsw25_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_msw\wxmsw25_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswud" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswud\wxmsw25ud_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswud" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswud\wxmsw25ud_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswud\wxmsw25ud_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswud\wxmsw25ud_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswu" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Fd..\..\src\fl\..\..\..\lib\vc_mswu\wxmsw25u_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswu" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Fd..\..\src\fl\..\..\..\lib\vc_mswu\wxmsw25u_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswu\wxmsw25u_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswu\wxmsw25u_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivd" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivd" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswuniv" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Fd..\..\src\fl\..\..\..\lib\vc_mswuniv\wxmswuniv25_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswuniv" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Fd..\..\src\fl\..\..\..\lib\vc_mswuniv\wxmswuniv25_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswuniv\wxmswuniv25_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswuniv\wxmswuniv25_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivud" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivud" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\fl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivu" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Fd..\..\src\fl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivu" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Fd..\..\src\fl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_fl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_fllib.pch" /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_fl.lib"
# ADD LIB32 /nologo /out:"..\..\src\fl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\fl"
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

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswdll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw250_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswdll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw250_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswdll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswdll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw250_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswdll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw25_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw250_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswdll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswdll\wxmsw25_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\fl"
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

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswudll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw250u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswudll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw250u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswudll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswudll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw250u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswudll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw25u_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw250u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswudll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswudll\wxmsw25u_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\fl"
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

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivdll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivdll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivdll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivdll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv25_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivdll\wxmswuniv25_fl.lib"

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\fl"
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

!ELSEIF  "$(CFG)" == "fl - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\fl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\fl\..\..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\fl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivudll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\fl\..\..\..\include" /I "..\..\src\fl\..\..\..\lib\vc_mswunivudll" /I "..\..\src\fl\..\..\..\src\tiff" /I "..\..\src\fl\..\..\..\src\jpeg" /I "..\..\src\fl\..\..\..\src\png" /I "..\..\src\fl\..\..\..\src\zlib" /I "..\..\src\fl\..\..\..\src\regex" /I "..\..\src\fl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_fldll.pch" /Fd..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_fl_vc_custom.pdb /I "..\..\src\fl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_FL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivudll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\fl\..\..\..\include" /i "..\..\src\fl\..\..\..\lib\vc_mswunivudll" /i "..\..\src\fl\..\..\..\src\tiff" /i "..\..\src\fl\..\..\..\src\jpeg" /i "..\..\src\fl\..\..\..\src\png" /i "..\..\src\fl\..\..\..\src\zlib" /i "..\..\src\fl\..\..\..\src\regex" /i "..\..\src\fl\..\..\..\src\expat\lib" /i "..\..\src\fl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_FL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_fl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_fl_vc_custom.dll" /libpath:"..\..\src\fl\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\fl\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_fl.lib"

!ENDIF

# Begin Target

# Name "fl - Win32 Debug"
# Name "fl - Win32 Release"
# Name "fl - Win32 Unicode Debug"
# Name "fl - Win32 Unicode Release"
# Name "fl - Win32 Universal Debug"
# Name "fl - Win32 Universal Release"
# Name "fl - Win32 Universal Unicode Debug"
# Name "fl - Win32 Universal Unicode Release"
# Name "fl - Win32 DLL Debug"
# Name "fl - Win32 DLL Release"
# Name "fl - Win32 DLL Unicode Debug"
# Name "fl - Win32 DLL Unicode Release"
# Name "fl - Win32 DLL Universal Debug"
# Name "fl - Win32 DLL Universal Release"
# Name "fl - Win32 DLL Universal Unicode Debug"
# Name "fl - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
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

SOURCE=../../src/fl\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
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

