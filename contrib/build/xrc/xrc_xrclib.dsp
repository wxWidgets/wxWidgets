# Microsoft Developer Studio Project File - Name="xrclib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=xrclib - Win32 LIB wxUniv Release Ansi
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xrc_xrclib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xrc_xrclib.mak" CFG="xrclib - Win32 LIB wxUniv Release Ansi"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xrclib - Win32 LIB wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "xrclib - Win32 LIB Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "xrclib - Win32 LIB wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "xrclib - Win32 LIB Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "xrclib - Win32 LIB Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "xrclib - Win32 LIB wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "xrclib - Win32 LIB wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "xrclib - Win32 LIB Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xrclib - Win32 LIB wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\xrclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\xrclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswuniv" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Fd..\..\src\xrc\..\..\..\lib\vc_mswuniv\wxmswuniv25_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswuniv" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Fd..\..\src\xrc\..\..\..\lib\vc_mswuniv\wxmswuniv25_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswuniv\wxmswuniv25_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswuniv\wxmswuniv25_xrc.lib"

!ELSEIF  "$(CFG)" == "xrclib - Win32 LIB Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\xrclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\xrclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswu" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Fd..\..\src\xrc\..\..\..\lib\vc_mswu\wxmsw25u_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswu" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Fd..\..\src\xrc\..\..\..\lib\vc_mswu\wxmsw25u_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswu\wxmsw25u_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswu\wxmsw25u_xrc.lib"

!ELSEIF  "$(CFG)" == "xrclib - Win32 LIB wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\xrclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\xrclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivd" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivd" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_xrc.lib"

!ELSEIF  "$(CFG)" == "xrclib - Win32 LIB Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\xrclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\xrclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_msw" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Fd..\..\src\xrc\..\..\..\lib\vc_msw\wxmsw25_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_msw" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Fd..\..\src\xrc\..\..\..\lib\vc_msw\wxmsw25_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_msw\wxmsw25_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_msw\wxmsw25_xrc.lib"

!ELSEIF  "$(CFG)" == "xrclib - Win32 LIB Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\xrclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\xrclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswud" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswud\wxmsw25ud_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswud" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswud\wxmsw25ud_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswud\wxmsw25ud_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswud\wxmsw25ud_xrc.lib"

!ELSEIF  "$(CFG)" == "xrclib - Win32 LIB wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\xrclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\xrclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivu" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivu" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_xrc.lib"

!ELSEIF  "$(CFG)" == "xrclib - Win32 LIB wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\xrclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\xrclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivud" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswunivud" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_xrc.lib"

!ELSEIF  "$(CFG)" == "xrclib - Win32 LIB Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\xrclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\xrc\..\..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\xrclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswd" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswd\wxmsw25d_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\xrc\..\..\..\include" /I "..\..\src\xrc\..\..\..\lib\vc_mswd" /I "..\..\src\xrc\..\..\..\src\tiff" /I "..\..\src\xrc\..\..\..\src\jpeg" /I "..\..\src\xrc\..\..\..\src\png" /I "..\..\src\xrc\..\..\..\src\zlib" /I "..\..\src\xrc\..\..\..\src\regex" /I "..\..\src\xrc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\xrc\..\..\..\lib\vc_mswd\wxmsw25d_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_xrclib.pch" /I "..\..\src\xrc\..\..\include" /I "..\..\src\xrc\expat\xmlparse" /I "..\..\src\xrc\expat\xmltok" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswd\wxmsw25d_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\src\xrc\..\..\..\lib\vc_mswd\wxmsw25d_xrc.lib"

!ENDIF

# Begin Target

# Name "xrclib - Win32 LIB wxUniv Release Ansi"
# Name "xrclib - Win32 LIB Native Release Unicode"
# Name "xrclib - Win32 LIB wxUniv Debug Ansi"
# Name "xrclib - Win32 LIB Native Release Ansi"
# Name "xrclib - Win32 LIB Native Debug Unicode"
# Name "xrclib - Win32 LIB wxUniv Release Unicode"
# Name "xrclib - Win32 LIB wxUniv Debug Unicode"
# Name "xrclib - Win32 LIB Native Debug Ansi"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/xrc\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_bmp.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_bmpbt.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_bttn.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_cald.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_chckb.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_chckl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_choic.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_combo.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_dlg.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_frame.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_gauge.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_gdctl.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_html.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_listb.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_listc.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_menu.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_notbk.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_panel.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_radbt.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_radbx.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_scrol.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_scwin.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_sizer.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_slidr.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_spin.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_split.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_stbmp.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_stbox.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_stlin.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_sttxt.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_text.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_toolb.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_tree.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_unkwn.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xh_wizrd.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xmlres.cpp
# End Source File
# Begin Source File

SOURCE=../../src/xrc\xmlrsall.cpp
# End Source File
# End Group
# End Target
# End Project

