# Microsoft Developer Studio Project File - Name="plotlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=plotlib - Win32 LIB wxUniv Release Ansi
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plot_plotlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plot_plotlib.mak" CFG="plotlib - Win32 LIB wxUniv Release Ansi"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plotlib - Win32 LIB wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "plotlib - Win32 LIB Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "plotlib - Win32 LIB wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "plotlib - Win32 LIB Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "plotlib - Win32 LIB Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "plotlib - Win32 LIB wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "plotlib - Win32 LIB wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "plotlib - Win32 LIB Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plotlib - Win32 LIB wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\plotlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\plotlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswuniv" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Fd..\..\src\plot\..\..\..\lib\vc_mswuniv\wxmswuniv25_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswuniv" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Fd..\..\src\plot\..\..\..\lib\vc_mswuniv\wxmswuniv25_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswuniv\wxmswuniv25_plot.lib"
# ADD LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswuniv\wxmswuniv25_plot.lib"

!ELSEIF  "$(CFG)" == "plotlib - Win32 LIB Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\plotlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\plotlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswu" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Fd..\..\src\plot\..\..\..\lib\vc_mswu\wxmsw25u_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswu" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Fd..\..\src\plot\..\..\..\lib\vc_mswu\wxmsw25u_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswu\wxmsw25u_plot.lib"
# ADD LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswu\wxmsw25u_plot.lib"

!ELSEIF  "$(CFG)" == "plotlib - Win32 LIB wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\plotlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\plotlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivd" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswunivd\wxmswuniv25d_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivd" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswunivd\wxmswuniv25d_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswunivd\wxmswuniv25d_plot.lib"
# ADD LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswunivd\wxmswuniv25d_plot.lib"

!ELSEIF  "$(CFG)" == "plotlib - Win32 LIB Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\plotlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\plotlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_msw" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Fd..\..\src\plot\..\..\..\lib\vc_msw\wxmsw25_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_msw" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Fd..\..\src\plot\..\..\..\lib\vc_msw\wxmsw25_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_msw\wxmsw25_plot.lib"
# ADD LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_msw\wxmsw25_plot.lib"

!ELSEIF  "$(CFG)" == "plotlib - Win32 LIB Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\plotlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\plotlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswud" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswud\wxmsw25ud_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswud" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswud\wxmsw25ud_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswud\wxmsw25ud_plot.lib"
# ADD LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswud\wxmsw25ud_plot.lib"

!ELSEIF  "$(CFG)" == "plotlib - Win32 LIB wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\plotlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\plotlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivu" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Fd..\..\src\plot\..\..\..\lib\vc_mswunivu\wxmswuniv25u_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivu" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Fd..\..\src\plot\..\..\..\lib\vc_mswunivu\wxmswuniv25u_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswunivu\wxmswuniv25u_plot.lib"
# ADD LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswunivu\wxmswuniv25u_plot.lib"

!ELSEIF  "$(CFG)" == "plotlib - Win32 LIB wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\plotlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\plotlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivud" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivud" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_plot.lib"
# ADD LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_plot.lib"

!ELSEIF  "$(CFG)" == "plotlib - Win32 LIB Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\plotlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\plotlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswd" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswd\wxmsw25d_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswd" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswd\wxmsw25d_plot.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_plotlib.pch" /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswd\wxmsw25d_plot.lib"
# ADD LIB32 /nologo /out:"..\..\src\plot\..\..\..\lib\vc_mswd\wxmsw25d_plot.lib"

!ENDIF

# Begin Target

# Name "plotlib - Win32 LIB wxUniv Release Ansi"
# Name "plotlib - Win32 LIB Native Release Unicode"
# Name "plotlib - Win32 LIB wxUniv Debug Ansi"
# Name "plotlib - Win32 LIB Native Release Ansi"
# Name "plotlib - Win32 LIB Native Debug Unicode"
# Name "plotlib - Win32 LIB wxUniv Release Unicode"
# Name "plotlib - Win32 LIB wxUniv Debug Unicode"
# Name "plotlib - Win32 LIB Native Debug Ansi"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/plot\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/plot\plot.cpp
# End Source File
# End Group
# End Target
# End Project

