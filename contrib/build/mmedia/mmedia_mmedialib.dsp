# Microsoft Developer Studio Project File - Name="mmedialib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mmedialib - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mmedia_mmedialib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mmedia_mmedialib.mak" CFG="mmedialib - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mmedialib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedialib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedialib - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedialib - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedialib - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedialib - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedialib - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mmedialib - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mmedialib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\mmedialib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\mmedialib"
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

!ELSEIF  "$(CFG)" == "mmedialib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\mmedialib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\mmedialib"
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

!ELSEIF  "$(CFG)" == "mmedialib - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\mmedialib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\mmedialib"
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

!ELSEIF  "$(CFG)" == "mmedialib - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\mmedialib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\mmedialib"
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

!ELSEIF  "$(CFG)" == "mmedialib - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\mmedialib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\mmedialib"
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

!ELSEIF  "$(CFG)" == "mmedialib - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\mmedialib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\mmedialib"
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

!ELSEIF  "$(CFG)" == "mmedialib - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\mmedialib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\mmedialib"
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

!ELSEIF  "$(CFG)" == "mmedialib - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\mmedialib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\mmedia\..\..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\mmedialib"
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

!ENDIF

# Begin Target

# Name "mmedialib - Win32 Debug"
# Name "mmedialib - Win32 Release"
# Name "mmedialib - Win32 Unicode Debug"
# Name "mmedialib - Win32 Unicode Release"
# Name "mmedialib - Win32 Universal Debug"
# Name "mmedialib - Win32 Universal Release"
# Name "mmedialib - Win32 Universal Unicode Debug"
# Name "mmedialib - Win32 Universal Unicode Release"
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

