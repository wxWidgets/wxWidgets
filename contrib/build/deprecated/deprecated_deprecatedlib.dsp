# Microsoft Developer Studio Project File - Name="deprecatedlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=deprecatedlib - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "deprecated_deprecatedlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "deprecated_deprecatedlib.mak" CFG="deprecatedlib - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "deprecatedlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "deprecatedlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "deprecatedlib - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "deprecatedlib - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "deprecatedlib - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "deprecatedlib - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "deprecatedlib - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "deprecatedlib - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "deprecatedlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\deprecatedlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\deprecatedlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswd" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswd\wxmsw25d_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswd" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswd\wxmsw25d_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswd\wxmsw25d_deprecated.lib"
# ADD LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswd\wxmsw25d_deprecated.lib"

!ELSEIF  "$(CFG)" == "deprecatedlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\deprecatedlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\deprecatedlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_msw" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Fd..\..\src\deprecated\..\..\..\lib\vc_msw\wxmsw25_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_msw" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Fd..\..\src\deprecated\..\..\..\lib\vc_msw\wxmsw25_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_msw\wxmsw25_deprecated.lib"
# ADD LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_msw\wxmsw25_deprecated.lib"

!ELSEIF  "$(CFG)" == "deprecatedlib - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\deprecatedlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\deprecatedlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswud" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswud\wxmsw25ud_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswud" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswud\wxmsw25ud_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswud\wxmsw25ud_deprecated.lib"
# ADD LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswud\wxmsw25ud_deprecated.lib"

!ELSEIF  "$(CFG)" == "deprecatedlib - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\deprecatedlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\deprecatedlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswu" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswu\wxmsw25u_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswu" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswu\wxmsw25u_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswu\wxmsw25u_deprecated.lib"
# ADD LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswu\wxmsw25u_deprecated.lib"

!ELSEIF  "$(CFG)" == "deprecatedlib - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\deprecatedlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\deprecatedlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivd" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivd\wxmswuniv25d_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivd" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivd\wxmswuniv25d_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivd\wxmswuniv25d_deprecated.lib"
# ADD LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivd\wxmswuniv25d_deprecated.lib"

!ELSEIF  "$(CFG)" == "deprecatedlib - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\deprecatedlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\deprecatedlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswuniv" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswuniv\wxmswuniv25_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswuniv" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswuniv\wxmswuniv25_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswuniv\wxmswuniv25_deprecated.lib"
# ADD LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswuniv\wxmswuniv25_deprecated.lib"

!ELSEIF  "$(CFG)" == "deprecatedlib - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\deprecatedlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\deprecatedlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivud" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivud" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_deprecated.lib"
# ADD LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_deprecated.lib"

!ELSEIF  "$(CFG)" == "deprecatedlib - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\deprecatedlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\deprecated\..\..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\deprecatedlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivu" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivu\wxmswuniv25u_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\deprecated\..\..\..\include" /I "..\..\src\deprecated\..\..\..\lib\vc_mswunivu" /I "..\..\src\deprecated\..\..\..\src\tiff" /I "..\..\src\deprecated\..\..\..\src\jpeg" /I "..\..\src\deprecated\..\..\..\src\png" /I "..\..\src\deprecated\..\..\..\src\zlib" /I "..\..\src\deprecated\..\..\..\src\regex" /I "..\..\src\deprecated\..\..\..\src\expat\lib" /Fd..\..\src\deprecated\..\..\..\lib\vc_mswunivu\wxmswuniv25u_deprecated.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_deprecatedlib.pch" /I "..\..\src\deprecated\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivu\wxmswuniv25u_deprecated.lib"
# ADD LIB32 /nologo /out:"..\..\src\deprecated\..\..\..\lib\vc_mswunivu\wxmswuniv25u_deprecated.lib"

!ENDIF

# Begin Target

# Name "deprecatedlib - Win32 Debug"
# Name "deprecatedlib - Win32 Release"
# Name "deprecatedlib - Win32 Unicode Debug"
# Name "deprecatedlib - Win32 Unicode Release"
# Name "deprecatedlib - Win32 Universal Debug"
# Name "deprecatedlib - Win32 Universal Release"
# Name "deprecatedlib - Win32 Universal Unicode Debug"
# Name "deprecatedlib - Win32 Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/deprecated\dosyacc.c
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\prop.cpp
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\propform.cpp
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\proplist.cpp
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\resource.cpp
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\treelay.cpp
# End Source File
# Begin Source File

SOURCE=../../src/deprecated\wxexpr.cpp
# End Source File
# End Group
# End Target
# End Project

