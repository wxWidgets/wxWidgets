# Microsoft Developer Studio Project File - Name="ogllib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ogllib - Win32 Static Native Release Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ogl_ogllib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ogl_ogllib.mak" CFG="ogllib - Win32 Static Native Release Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ogllib - Win32 Static Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "ogllib - Win32 Static wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "ogllib - Win32 Static Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "ogllib - Win32 Static Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "ogllib - Win32 Static wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "ogllib - Win32 Static wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "ogllib - Win32 Static Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "ogllib - Win32 Static wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ogllib - Win32 Static Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswu\"
# PROP BASE Intermediate_Dir "vc_mswu\ogllib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswu\"
# PROP Intermediate_Dir "vc_mswu\ogllib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswu" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswu\wxmsw25u_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswu" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswu\wxmsw25u_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswu\wxmsw25u_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswu\wxmsw25u_ogl.lib"

!ELSEIF  "$(CFG)" == "ogllib - Win32 Static wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivu\"
# PROP BASE Intermediate_Dir "vc_mswunivu\ogllib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivu\"
# PROP Intermediate_Dir "vc_mswunivu\ogllib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivu" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivu" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivu\wxmswuniv25u_ogl.lib"

!ELSEIF  "$(CFG)" == "ogllib - Win32 Static Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_msw\"
# PROP BASE Intermediate_Dir "vc_msw\ogllib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_msw\"
# PROP Intermediate_Dir "vc_msw\ogllib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_msw" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_msw\wxmsw25_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_msw" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_msw\wxmsw25_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_msw\wxmsw25_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_msw\wxmsw25_ogl.lib"

!ELSEIF  "$(CFG)" == "ogllib - Win32 Static Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswud\"
# PROP BASE Intermediate_Dir "vc_mswud\ogllib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswud\"
# PROP Intermediate_Dir "vc_mswud\ogllib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswud" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswud\wxmsw25ud_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswud" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswud\wxmsw25ud_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswud\wxmsw25ud_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswud\wxmsw25ud_ogl.lib"

!ELSEIF  "$(CFG)" == "ogllib - Win32 Static wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivd\"
# PROP BASE Intermediate_Dir "vc_mswunivd\ogllib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivd\"
# PROP Intermediate_Dir "vc_mswunivd\ogllib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivd" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivd" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivd\wxmswuniv25d_ogl.lib"

!ELSEIF  "$(CFG)" == "ogllib - Win32 Static wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswuniv\"
# PROP BASE Intermediate_Dir "vc_mswuniv\ogllib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswuniv\"
# PROP Intermediate_Dir "vc_mswuniv\ogllib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswuniv" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswuniv\wxmswuniv25_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswuniv" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Fd..\..\src\ogl\..\..\..\lib\vc_mswuniv\wxmswuniv25_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswuniv\wxmswuniv25_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswuniv\wxmswuniv25_ogl.lib"

!ELSEIF  "$(CFG)" == "ogllib - Win32 Static Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswd\"
# PROP BASE Intermediate_Dir "vc_mswd\ogllib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswd\"
# PROP Intermediate_Dir "vc_mswd\ogllib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswd" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswd\wxmsw25d_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswd" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswd\wxmsw25d_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswd\wxmsw25d_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswd\wxmsw25d_ogl.lib"

!ELSEIF  "$(CFG)" == "ogllib - Win32 Static wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivud\"
# PROP BASE Intermediate_Dir "vc_mswunivud\ogllib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivud\"
# PROP Intermediate_Dir "vc_mswunivud\ogllib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivud" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivud" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_ogl.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_ogllib.pch" /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_ogl.lib"
# ADD LIB32 /nologo /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_ogl.lib"

!ENDIF

# Begin Target

# Name "ogllib - Win32 Static Native Release Unicode"
# Name "ogllib - Win32 Static wxUniv Release Unicode"
# Name "ogllib - Win32 Static Native Release Ansi"
# Name "ogllib - Win32 Static Native Debug Unicode"
# Name "ogllib - Win32 Static wxUniv Debug Ansi"
# Name "ogllib - Win32 Static wxUniv Release Ansi"
# Name "ogllib - Win32 Static Native Debug Ansi"
# Name "ogllib - Win32 Static wxUniv Debug Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/ogl\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
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

