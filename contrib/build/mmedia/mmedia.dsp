# Microsoft Developer Studio Project File - Name="mmedia" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mmedia - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mmedia.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mmedia.mak" CFG="mmedia - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mmedia - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedia - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedia - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedia - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedia - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedia - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedia - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedia - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedia - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mmedia - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mmedia - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mmedia - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mmedia - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mmedia - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mmedia - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mmedia - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mmedia - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswd" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswd\wxmsw25d_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswd" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswd\wxmsw25d_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswd\wxmsw25d_mmedia.lib"
# ADD LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswd\wxmsw25d_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_msw" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Fd..\..\src\mmedia\..\..\..\lib\vc_msw\wxmsw25_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_msw" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Fd..\..\src\mmedia\..\..\..\lib\vc_msw\wxmsw25_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_msw\wxmsw25_mmedia.lib"
# ADD LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_msw\wxmsw25_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswud" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswud\wxmsw25ud_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswud" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswud\wxmsw25ud_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswud\wxmsw25ud_mmedia.lib"
# ADD LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswud\wxmsw25ud_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswu" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswu\wxmsw25u_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswu" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswu\wxmsw25u_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswu\wxmsw25u_mmedia.lib"
# ADD LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswu\wxmsw25u_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivd" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivd\wxmswuniv25d_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivd" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivd\wxmswuniv25d_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivd\wxmswuniv25d_mmedia.lib"
# ADD LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivd\wxmswuniv25d_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswuniv" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswuniv\wxmswuniv25_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswuniv" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswuniv\wxmswuniv25_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswuniv\wxmswuniv25_mmedia.lib"
# ADD LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswuniv\wxmswuniv25_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivud" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivud" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_mmedia.lib"
# ADD LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivu" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivu\wxmswuniv25u_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivu" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivu\wxmswuniv25u_mmedia.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_mmedialib.pch" /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivu\wxmswuniv25u_mmedia.lib"
# ADD LIB32 /nologo /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivu\wxmswuniv25u_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswddll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_mmediadll.pch" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswddll\wxmsw250d_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswddll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_mmediadll.pch" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswddll\wxmsw250d_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswddll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswddll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswddll\wxmsw250d_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswddll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswddll\wxmsw25d_mmedia.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswddll\wxmsw250d_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswddll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswddll\wxmsw25d_mmedia.lib" /debug

!ELSEIF  "$(CFG)" == "mmedia - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswdll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_mmediadll.pch" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswdll\wxmsw250_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswdll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_mmediadll.pch" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswdll\wxmsw250_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswdll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswdll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswdll\wxmsw250_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswdll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswdll\wxmsw25_mmedia.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswdll\wxmsw250_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswdll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswdll\wxmsw25_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswuddll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_mmediadll.pch" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswuddll\wxmsw250ud_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswuddll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_mmediadll.pch" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswuddll\wxmsw250ud_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswuddll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswuddll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswuddll\wxmsw250ud_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswuddll\wxmsw25ud_mmedia.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswuddll\wxmsw250ud_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswuddll\wxmsw25ud_mmedia.lib" /debug

!ELSEIF  "$(CFG)" == "mmedia - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswudll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_mmediadll.pch" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswudll\wxmsw250u_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswudll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_mmediadll.pch" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswudll\wxmsw250u_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswudll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswudll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswudll\wxmsw250u_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswudll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswudll\wxmsw25u_mmedia.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswudll\wxmsw250u_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswudll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswudll\wxmsw25u_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivddll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_mmediadll.pch" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivddll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_mmediadll.pch" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswunivddll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswunivddll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_mmedia.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_mmedia.lib" /debug

!ELSEIF  "$(CFG)" == "mmedia - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivdll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_mmediadll.pch" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivdll\wxmswuniv250_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivdll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_mmediadll.pch" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivdll\wxmswuniv250_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswunivdll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswunivdll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivdll\wxmswuniv250_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswunivdll\wxmswuniv25_mmedia.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivdll\wxmswuniv250_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswunivdll\wxmswuniv25_mmedia.lib"

!ELSEIF  "$(CFG)" == "mmedia - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_mmediadll.pch" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_mmediadll.pch" /Zi /Gm /GZ /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_mmedia.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_mmedia.lib" /debug

!ELSEIF  "$(CFG)" == "mmedia - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\mmedia"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\mmedia"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivudll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_mmediadll.pch" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\mmedia\..\..\..\include" /I "..\..\src\mmedia\..\..\..\lib\vc_mswunivudll" /I "..\..\src\mmedia\..\..\..\src\tiff" /I "..\..\src\mmedia\..\..\..\src\jpeg" /I "..\..\src\mmedia\..\..\..\src\png" /I "..\..\src\mmedia\..\..\..\src\zlib" /I "..\..\src\mmedia\..\..\..\src\regex" /I "..\..\src\mmedia\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_mmediadll.pch" /Fd..\..\src\mmedia\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_mmedia_vc_custom.pdb /I "..\..\src\mmedia\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_MMEDIA" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswunivudll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\mmedia\..\..\..\include" /i "..\..\src\mmedia\..\..\..\lib\vc_mswunivudll" /i "..\..\src\mmedia\..\..\..\src\tiff" /i "..\..\src\mmedia\..\..\..\src\jpeg" /i "..\..\src\mmedia\..\..\..\src\png" /i "..\..\src\mmedia\..\..\..\src\zlib" /i "..\..\src\mmedia\..\..\..\src\regex" /i "..\..\src\mmedia\..\..\..\src\expat\lib" /i "..\..\src\mmedia\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_MMEDIA
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_mmedia.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\mmedia\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_mmedia_vc_custom.dll" /libpath:"..\..\src\mmedia\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\mmedia\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_mmedia.lib"

!ENDIF

# Begin Target

# Name "mmedia - Win32 Debug"
# Name "mmedia - Win32 Release"
# Name "mmedia - Win32 Unicode Debug"
# Name "mmedia - Win32 Unicode Release"
# Name "mmedia - Win32 Universal Debug"
# Name "mmedia - Win32 Universal Release"
# Name "mmedia - Win32 Universal Unicode Debug"
# Name "mmedia - Win32 Universal Unicode Release"
# Name "mmedia - Win32 DLL Debug"
# Name "mmedia - Win32 DLL Release"
# Name "mmedia - Win32 DLL Unicode Debug"
# Name "mmedia - Win32 DLL Unicode Release"
# Name "mmedia - Win32 DLL Universal Debug"
# Name "mmedia - Win32 DLL Universal Release"
# Name "mmedia - Win32 DLL Universal Unicode Debug"
# Name "mmedia - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/mmedia\cdbase.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\cdwin.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\g711.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\g721.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\g723_24.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\g723_40.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\g72x.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndaiff.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndbase.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndcodec.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndcpcm.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndfile.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndg72x.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndmsad.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndpcm.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndulaw.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndwav.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\sndwin.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\vidbase.cpp
# End Source File
# Begin Source File

SOURCE=../../src/mmedia\vidwin.cpp
# End Source File
# End Group
# End Target
# End Project

