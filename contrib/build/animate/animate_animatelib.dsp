# Microsoft Developer Studio Project File - Name="animatelib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=animatelib - Win32 LIB wxUniv Release Ansi
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "animate_animatelib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "animate_animatelib.mak" CFG="animatelib - Win32 LIB wxUniv Release Ansi"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "animatelib - Win32 LIB wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "animatelib - Win32 LIB Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "animatelib - Win32 LIB wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "animatelib - Win32 LIB Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "animatelib - Win32 LIB Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "animatelib - Win32 LIB wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "animatelib - Win32 LIB wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "animatelib - Win32 LIB Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "animatelib - Win32 LIB wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswuniv\"
# PROP BASE Intermediate_Dir "vc_mswuniv\animatelib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswuniv\"
# PROP Intermediate_Dir "vc_mswuniv\animatelib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswuniv" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Fd..\..\src\animate\..\..\..\lib\vc_mswuniv\wxmswuniv25_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswuniv" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Fd..\..\src\animate\..\..\..\lib\vc_mswuniv\wxmswuniv25_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswuniv\wxmswuniv25_animate.lib"
# ADD LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswuniv\wxmswuniv25_animate.lib"

!ELSEIF  "$(CFG)" == "animatelib - Win32 LIB Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswu\"
# PROP BASE Intermediate_Dir "vc_mswu\animatelib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswu\"
# PROP Intermediate_Dir "vc_mswu\animatelib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswu" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Fd..\..\src\animate\..\..\..\lib\vc_mswu\wxmsw25u_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswu" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Fd..\..\src\animate\..\..\..\lib\vc_mswu\wxmsw25u_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswu\wxmsw25u_animate.lib"
# ADD LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswu\wxmsw25u_animate.lib"

!ELSEIF  "$(CFG)" == "animatelib - Win32 LIB wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivd\"
# PROP BASE Intermediate_Dir "vc_mswunivd\animatelib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivd\"
# PROP Intermediate_Dir "vc_mswunivd\animatelib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivd" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswunivd\wxmswuniv25d_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivd" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswunivd\wxmswuniv25d_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswunivd\wxmswuniv25d_animate.lib"
# ADD LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswunivd\wxmswuniv25d_animate.lib"

!ELSEIF  "$(CFG)" == "animatelib - Win32 LIB Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_msw\"
# PROP BASE Intermediate_Dir "vc_msw\animatelib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_msw\"
# PROP Intermediate_Dir "vc_msw\animatelib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_msw" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Fd..\..\src\animate\..\..\..\lib\vc_msw\wxmsw25_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_msw" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Fd..\..\src\animate\..\..\..\lib\vc_msw\wxmsw25_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_msw\wxmsw25_animate.lib"
# ADD LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_msw\wxmsw25_animate.lib"

!ELSEIF  "$(CFG)" == "animatelib - Win32 LIB Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswud\"
# PROP BASE Intermediate_Dir "vc_mswud\animatelib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswud\"
# PROP Intermediate_Dir "vc_mswud\animatelib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswud" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswud\wxmsw25ud_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswud" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswud\wxmsw25ud_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswud\wxmsw25ud_animate.lib"
# ADD LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswud\wxmsw25ud_animate.lib"

!ELSEIF  "$(CFG)" == "animatelib - Win32 LIB wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivu\"
# PROP BASE Intermediate_Dir "vc_mswunivu\animatelib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivu\"
# PROP Intermediate_Dir "vc_mswunivu\animatelib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivu" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Fd..\..\src\animate\..\..\..\lib\vc_mswunivu\wxmswuniv25u_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivu" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Fd..\..\src\animate\..\..\..\lib\vc_mswunivu\wxmswuniv25u_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswunivu\wxmswuniv25u_animate.lib"
# ADD LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswunivu\wxmswuniv25u_animate.lib"

!ELSEIF  "$(CFG)" == "animatelib - Win32 LIB wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivud\"
# PROP BASE Intermediate_Dir "vc_mswunivud\animatelib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivud\"
# PROP Intermediate_Dir "vc_mswunivud\animatelib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivud" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivud" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_animate.lib"
# ADD LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_animate.lib"

!ELSEIF  "$(CFG)" == "animatelib - Win32 LIB Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswd\"
# PROP BASE Intermediate_Dir "vc_mswd\animatelib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswd\"
# PROP Intermediate_Dir "vc_mswd\animatelib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswd" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswd\wxmsw25d_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswd" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswd\wxmsw25d_animate.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_animatelib.pch" /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswd\wxmsw25d_animate.lib"
# ADD LIB32 /nologo /out:"..\..\src\animate\..\..\..\lib\vc_mswd\wxmsw25d_animate.lib"

!ENDIF

# Begin Target

# Name "animatelib - Win32 LIB wxUniv Release Ansi"
# Name "animatelib - Win32 LIB Native Release Unicode"
# Name "animatelib - Win32 LIB wxUniv Debug Ansi"
# Name "animatelib - Win32 LIB Native Release Ansi"
# Name "animatelib - Win32 LIB Native Debug Unicode"
# Name "animatelib - Win32 LIB wxUniv Release Unicode"
# Name "animatelib - Win32 LIB wxUniv Debug Unicode"
# Name "animatelib - Win32 LIB Native Debug Ansi"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/animate\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/animate\animate.cpp
# End Source File
# End Group
# End Target
# End Project

