# Microsoft Developer Studio Project File - Name="ogl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ogl - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ogl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ogl.mak" CFG="ogl - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ogl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ogl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ogl - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ogl - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ogl - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ogl - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ogl - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ogl - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ogl - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogl - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogl - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogl - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogl - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogl - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogl - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogl - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ogl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswd" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswd\wxmsw25d_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswd" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswd\wxmsw25d_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswd\wxmsw25d_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswd\wxmsw25d_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_msw" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_msw\wxmsw25_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_msw" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_msw\wxmsw25_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_msw\wxmsw25_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_msw\wxmsw25_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswud" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswud\wxmsw25ud_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswud" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswud\wxmsw25ud_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswud\wxmsw25ud_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswud\wxmsw25ud_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswu" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswu\wxmsw25u_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswu" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswu\wxmsw25u_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswu\wxmsw25u_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswu\wxmsw25u_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivd" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivd" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswuniv" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswuniv\wxmswuniv25_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswuniv" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswuniv\wxmswuniv25_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswuniv\wxmswuniv25_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswuniv\wxmswuniv25_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivud" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivud" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivu" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivu" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw250d_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw250d_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw250d_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw25d_ogl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw250d_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw25d_ogl.lib" /debug

!ELSEIF  "$(CFG)" == "ogl - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswdll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw250_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswdll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw250_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswdll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswdll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw250_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswdll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw25_ogl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw250_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswdll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw25_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswuddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw250ud_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswuddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw250ud_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswuddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswuddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw250ud_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw25ud_ogl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw250ud_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw25ud_ogl.lib" /debug

!ELSEIF  "$(CFG)" == "ogl - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswudll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw250u_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswudll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw250u_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswudll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswudll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw250u_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswudll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw25u_ogl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw250u_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswudll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw25u_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_ogl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_ogl.lib" /debug

!ELSEIF  "$(CFG)" == "ogl - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv25_ogl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv25_ogl.lib"

!ELSEIF  "$(CFG)" == "ogl - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_ogl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_ogl.lib" /debug

!ELSEIF  "$(CFG)" == "ogl - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\ogl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\ogl"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_ogl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_ogl.lib"

!ENDIF

# Begin Target

# Name "ogl - Win32 Debug"
# Name "ogl - Win32 Release"
# Name "ogl - Win32 Unicode Debug"
# Name "ogl - Win32 Unicode Release"
# Name "ogl - Win32 Universal Debug"
# Name "ogl - Win32 Universal Release"
# Name "ogl - Win32 Universal Unicode Debug"
# Name "ogl - Win32 Universal Unicode Release"
# Name "ogl - Win32 DLL Debug"
# Name "ogl - Win32 DLL Release"
# Name "ogl - Win32 DLL Unicode Debug"
# Name "ogl - Win32 DLL Unicode Release"
# Name "ogl - Win32 DLL Universal Debug"
# Name "ogl - Win32 DLL Universal Release"
# Name "ogl - Win32 DLL Universal Unicode Debug"
# Name "ogl - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/ogl\basic.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\basic2.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\bmpshape.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\canvas.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\composit.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\constrnt.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\divided.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\drawn.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/ogl\lines.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\mfutils.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\ogldiag.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\oglmisc.cpp
# End Source File
# End Group
# End Target
# End Project

