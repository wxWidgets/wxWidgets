# Microsoft Developer Studio Project File - Name="svg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=svg - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "svg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "svg.mak" CFG="svg - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "svg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "svg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "svg - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "svg - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "svg - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "svg - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "svg - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "svg - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "svg - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svg - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svg - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svg - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svg - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svg - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svg - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svg - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "svg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswd" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswd\wxmsw25d_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswd" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswd\wxmsw25d_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswd\wxmsw25d_svg.lib"
# ADD LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswd\wxmsw25d_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_msw" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Fd..\..\src\svg\..\..\..\lib\vc_msw\wxmsw25_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_msw" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Fd..\..\src\svg\..\..\..\lib\vc_msw\wxmsw25_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_msw\wxmsw25_svg.lib"
# ADD LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_msw\wxmsw25_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswud" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswud\wxmsw25ud_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswud" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswud\wxmsw25ud_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswud\wxmsw25ud_svg.lib"
# ADD LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswud\wxmsw25ud_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswu" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Fd..\..\src\svg\..\..\..\lib\vc_mswu\wxmsw25u_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswu" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Fd..\..\src\svg\..\..\..\lib\vc_mswu\wxmsw25u_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswu\wxmsw25u_svg.lib"
# ADD LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswu\wxmsw25u_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivd" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivd\wxmswuniv25d_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivd" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivd\wxmswuniv25d_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswunivd\wxmswuniv25d_svg.lib"
# ADD LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswunivd\wxmswuniv25d_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswuniv" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Fd..\..\src\svg\..\..\..\lib\vc_mswuniv\wxmswuniv25_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswuniv" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Fd..\..\src\svg\..\..\..\lib\vc_mswuniv\wxmswuniv25_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswuniv\wxmswuniv25_svg.lib"
# ADD LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswuniv\wxmswuniv25_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivud" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivud" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_svg.lib"
# ADD LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivu" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Fd..\..\src\svg\..\..\..\lib\vc_mswunivu\wxmswuniv25u_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivu" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Fd..\..\src\svg\..\..\..\lib\vc_mswunivu\wxmswuniv25u_svg.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_svglib.pch" /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswunivu\wxmswuniv25u_svg.lib"
# ADD LIB32 /nologo /out:"..\..\src\svg\..\..\..\lib\vc_mswunivu\wxmswuniv25u_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw250d_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw250d_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw250d_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw25d_svg.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw250d_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw25d_svg.lib" /debug

!ELSEIF  "$(CFG)" == "svg - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswdll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw250_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswdll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw250_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswdll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswdll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw250_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswdll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw25_svg.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw250_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswdll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw25_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswuddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw250ud_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswuddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw250ud_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswuddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswuddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw250ud_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw25ud_svg.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw250ud_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw25ud_svg.lib" /debug

!ELSEIF  "$(CFG)" == "svg - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswudll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw250u_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswudll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw250u_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswudll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswudll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw250u_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswudll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw25u_svg.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw250u_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswudll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw25u_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_svg.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_svg.lib" /debug

!ELSEIF  "$(CFG)" == "svg - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivdll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv250_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivdll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv250_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivdll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivdll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv250_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv25_svg.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv250_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv25_svg.lib"

!ELSEIF  "$(CFG)" == "svg - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_svg.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_svg.lib" /debug

!ELSEIF  "$(CFG)" == "svg - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\svg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\svg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivudll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivudll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivudll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivudll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_svg.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_svg.lib"

!ENDIF

# Begin Target

# Name "svg - Win32 Debug"
# Name "svg - Win32 Release"
# Name "svg - Win32 Unicode Debug"
# Name "svg - Win32 Unicode Release"
# Name "svg - Win32 Universal Debug"
# Name "svg - Win32 Universal Release"
# Name "svg - Win32 Universal Unicode Debug"
# Name "svg - Win32 Universal Unicode Release"
# Name "svg - Win32 DLL Debug"
# Name "svg - Win32 DLL Release"
# Name "svg - Win32 DLL Unicode Debug"
# Name "svg - Win32 DLL Unicode Release"
# Name "svg - Win32 DLL Universal Debug"
# Name "svg - Win32 DLL Universal Release"
# Name "svg - Win32 DLL Universal Unicode Debug"
# Name "svg - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/svg\dcsvg.cpp
# End Source File
# Begin Source File

SOURCE=../../src/svg\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# End Group
# End Target
# End Project

