# Microsoft Developer Studio Project File - Name="gizmos" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gizmos - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gizmos.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gizmos.mak" CFG="gizmos - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gizmos - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmos - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gizmos - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gizmos - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswd" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswd\wxmsw25d_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswd" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswd\wxmsw25d_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswd\wxmsw25d_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswd\wxmsw25d_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_msw" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_msw\wxmsw25_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_msw" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_msw\wxmsw25_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_msw\wxmsw25_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_msw\wxmsw25_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswud" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswud\wxmsw25ud_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswud" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswud\wxmsw25ud_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswud\wxmsw25ud_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswud\wxmsw25ud_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswu" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswu\wxmsw25u_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswu" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswu\wxmsw25u_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswu\wxmsw25u_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswu\wxmsw25u_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivd" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivd\wxmswuniv25d_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivd" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivd\wxmswuniv25d_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivd\wxmswuniv25d_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivd\wxmswuniv25d_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswuniv" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswuniv\wxmswuniv25_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswuniv" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswuniv\wxmswuniv25_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswuniv\wxmswuniv25_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswuniv\wxmswuniv25_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivud" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivud" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivu" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivu\wxmswuniv25u_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivu" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivu\wxmswuniv25u_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivu\wxmswuniv25u_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivu\wxmswuniv25u_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswddll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_gizmosdll.pch" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswddll\wxmsw250d_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswddll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_gizmosdll.pch" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswddll\wxmsw250d_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswddll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswddll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25d.lib wxmsw25d_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswddll\wxmsw250d_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswddll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswddll\wxmsw25d_gizmos.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25d.lib wxmsw25d_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswddll\wxmsw250d_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswddll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswddll\wxmsw25d_gizmos.lib" /debug

!ELSEIF  "$(CFG)" == "gizmos - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswdll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_gizmosdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswdll\wxmsw250_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswdll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_gizmosdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswdll\wxmsw250_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswdll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswdll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25.lib wxmsw25_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswdll\wxmsw250_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswdll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswdll\wxmsw25_gizmos.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25.lib wxmsw25_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswdll\wxmsw250_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswdll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswdll\wxmsw25_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswuddll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_gizmosdll.pch" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswuddll\wxmsw250ud_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswuddll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_gizmosdll.pch" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswuddll\wxmsw250ud_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswuddll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswuddll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25ud.lib wxmsw25ud_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswuddll\wxmsw250ud_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswuddll\wxmsw25ud_gizmos.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25ud.lib wxmsw25ud_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswuddll\wxmsw250ud_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswuddll\wxmsw25ud_gizmos.lib" /debug

!ELSEIF  "$(CFG)" == "gizmos - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswudll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_gizmosdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswudll\wxmsw250u_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswudll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_gizmosdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswudll\wxmsw250u_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswudll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswudll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25u.lib wxmsw25u_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswudll\wxmsw250u_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswudll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswudll\wxmsw25u_gizmos.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25u.lib wxmsw25u_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswudll\wxmsw250u_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswudll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswudll\wxmsw25u_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivddll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_gizmosdll.pch" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivddll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_gizmosdll.pch" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswunivddll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswunivddll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25d.lib wxmswuniv25d_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_gizmos.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25d.lib wxmswuniv25d_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_gizmos.lib" /debug

!ELSEIF  "$(CFG)" == "gizmos - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivdll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_gizmosdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivdll\wxmswuniv250_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivdll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_gizmosdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivdll\wxmswuniv250_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswunivdll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswunivdll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25.lib wxmswuniv25_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivdll\wxmswuniv250_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswunivdll\wxmswuniv25_gizmos.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25.lib wxmswuniv25_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivdll\wxmswuniv250_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswunivdll\wxmswuniv25_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmos - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_gizmosdll.pch" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_gizmosdll.pch" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25ud.lib wxmswuniv25ud_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_gizmos.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25ud.lib wxmswuniv25ud_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_gizmos.lib" /debug

!ELSEIF  "$(CFG)" == "gizmos - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\gizmos"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\gizmos"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivudll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_gizmosdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivudll" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_gizmosdll.pch" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_gizmos_vc_custom.pdb /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GIZMOS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswunivudll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\gizmos\..\..\..\include" /i "..\..\src\gizmos\..\..\..\lib\vc_mswunivudll" /i "..\..\src\gizmos\..\..\..\src\tiff" /i "..\..\src\gizmos\..\..\..\src\jpeg" /i "..\..\src\gizmos\..\..\..\src\png" /i "..\..\src\gizmos\..\..\..\src\zlib" /i "..\..\src\gizmos\..\..\..\src\regex" /i "..\..\src\gizmos\..\..\..\src\expat\lib" /i "..\..\src\gizmos\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_GIZMOS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25u.lib wxmswuniv25u_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_gizmos.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxbase25u.lib wxmswuniv25u_core.lib /nologo /dll /machine:i386 /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_gizmos_vc_custom.dll" /libpath:"..\..\src\gizmos\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\gizmos\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_gizmos.lib"

!ENDIF

# Begin Target

# Name "gizmos - Win32 Debug"
# Name "gizmos - Win32 Release"
# Name "gizmos - Win32 Unicode Debug"
# Name "gizmos - Win32 Unicode Release"
# Name "gizmos - Win32 Universal Debug"
# Name "gizmos - Win32 Universal Release"
# Name "gizmos - Win32 Universal Unicode Debug"
# Name "gizmos - Win32 Universal Unicode Release"
# Name "gizmos - Win32 DLL Debug"
# Name "gizmos - Win32 DLL Release"
# Name "gizmos - Win32 DLL Unicode Debug"
# Name "gizmos - Win32 DLL Unicode Release"
# Name "gizmos - Win32 DLL Universal Debug"
# Name "gizmos - Win32 DLL Universal Release"
# Name "gizmos - Win32 DLL Universal Unicode Debug"
# Name "gizmos - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/gizmos\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/gizmos\dynamicsash.cpp
# End Source File
# Begin Source File

SOURCE=../../src/gizmos\editlbox.cpp
# End Source File
# Begin Source File

SOURCE=../../src/gizmos\ledctrl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/gizmos\multicell.cpp
# End Source File
# Begin Source File

SOURCE=../../src/gizmos\splittree.cpp
# End Source File
# Begin Source File

SOURCE=../../src/gizmos\statpict.cpp
# End Source File
# End Group
# End Target
# End Project

