# Microsoft Developer Studio Project File - Name="gizmoslib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gizmoslib - Win32 Static Native Release Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gizmos_gizmoslib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gizmos_gizmoslib.mak" CFG="gizmoslib - Win32 Static Native Release Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gizmoslib - Win32 Static Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmoslib - Win32 Static wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmoslib - Win32 Static Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmoslib - Win32 Static Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmoslib - Win32 Static wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmoslib - Win32 Static wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmoslib - Win32 Static Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "gizmoslib - Win32 Static wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gizmoslib - Win32 Static Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswu\"
# PROP BASE Intermediate_Dir "vc_mswu\gizmoslib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswu\"
# PROP Intermediate_Dir "vc_mswu\gizmoslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswu" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswu\wxmsw25u_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswu" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswu\wxmsw25u_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswu\wxmsw25u_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswu\wxmsw25u_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmoslib - Win32 Static wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivu\"
# PROP BASE Intermediate_Dir "vc_mswunivu\gizmoslib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivu\"
# PROP Intermediate_Dir "vc_mswunivu\gizmoslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivu" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivu\wxmswuniv25u_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivu" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivu\wxmswuniv25u_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivu\wxmswuniv25u_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivu\wxmswuniv25u_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmoslib - Win32 Static Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_msw\"
# PROP BASE Intermediate_Dir "vc_msw\gizmoslib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_msw\"
# PROP Intermediate_Dir "vc_msw\gizmoslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_msw" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_msw\wxmsw25_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_msw" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_msw\wxmsw25_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_msw\wxmsw25_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_msw\wxmsw25_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmoslib - Win32 Static Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswud\"
# PROP BASE Intermediate_Dir "vc_mswud\gizmoslib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswud\"
# PROP Intermediate_Dir "vc_mswud\gizmoslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswud" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswud\wxmsw25ud_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswud" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswud\wxmsw25ud_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswud\wxmsw25ud_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswud\wxmsw25ud_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmoslib - Win32 Static wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivd\"
# PROP BASE Intermediate_Dir "vc_mswunivd\gizmoslib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivd\"
# PROP Intermediate_Dir "vc_mswunivd\gizmoslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivd" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivd\wxmswuniv25d_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivd" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivd\wxmswuniv25d_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivd\wxmswuniv25d_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivd\wxmswuniv25d_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmoslib - Win32 Static wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswuniv\"
# PROP BASE Intermediate_Dir "vc_mswuniv\gizmoslib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswuniv\"
# PROP Intermediate_Dir "vc_mswuniv\gizmoslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswuniv" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswuniv\wxmswuniv25_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswuniv" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Fd..\..\src\gizmos\..\..\..\lib\vc_mswuniv\wxmswuniv25_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswuniv\wxmswuniv25_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswuniv\wxmswuniv25_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmoslib - Win32 Static Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswd\"
# PROP BASE Intermediate_Dir "vc_mswd\gizmoslib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswd\"
# PROP Intermediate_Dir "vc_mswd\gizmoslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswd" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswd\wxmsw25d_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswd" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswd\wxmsw25d_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswd\wxmsw25d_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswd\wxmsw25d_gizmos.lib"

!ELSEIF  "$(CFG)" == "gizmoslib - Win32 Static wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivud\"
# PROP BASE Intermediate_Dir "vc_mswunivud\gizmoslib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\gizmos\..\..\..\lib\vc_mswunivud\"
# PROP Intermediate_Dir "vc_mswunivud\gizmoslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivud" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\gizmos\..\..\..\include" /I "..\..\src\gizmos\..\..\..\lib\vc_mswunivud" /I "..\..\src\gizmos\..\..\..\src\tiff" /I "..\..\src\gizmos\..\..\..\src\jpeg" /I "..\..\src\gizmos\..\..\..\src\png" /I "..\..\src\gizmos\..\..\..\src\zlib" /I "..\..\src\gizmos\..\..\..\src\regex" /I "..\..\src\gizmos\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\gizmos\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_gizmos.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_gizmoslib.pch" /I "..\..\src\gizmos\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_gizmos.lib"
# ADD LIB32 /nologo /out:"..\..\src\gizmos\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_gizmos.lib"

!ENDIF

# Begin Target

# Name "gizmoslib - Win32 Static Native Release Unicode"
# Name "gizmoslib - Win32 Static wxUniv Release Unicode"
# Name "gizmoslib - Win32 Static Native Release Ansi"
# Name "gizmoslib - Win32 Static Native Debug Unicode"
# Name "gizmoslib - Win32 Static wxUniv Debug Ansi"
# Name "gizmoslib - Win32 Static wxUniv Release Ansi"
# Name "gizmoslib - Win32 Static Native Debug Ansi"
# Name "gizmoslib - Win32 Static wxUniv Debug Unicode"
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

