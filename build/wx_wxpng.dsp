# Microsoft Developer Studio Project File - Name="wxpng" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxpng - Win32 LIB wxUniv Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxpng.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxpng.mak" CFG="wxpng - Win32 LIB wxUniv Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxpng - Win32 LIB wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 LIB Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 LIB Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 LIB wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 LIB Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 LIB wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 LIB Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 LIB wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxpng - Win32 LIB wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivud\wxpngd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivud\wxpngd.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswdll\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswdll\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswdll\wxpng.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswdll\wxpng.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivuddll\wxpngd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivuddll\wxpngd.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivudll\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivudll\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivudll\wxpng.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivudll\wxpng.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivdll\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivdll\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivdll\wxpng.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivdll\wxpng.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 LIB Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswud\wxpngd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswud\wxpngd.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivddll\wxpngd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivddll\wxpngd.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 LIB Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswd\wxpngd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswd\wxpngd.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswudll\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswudll\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswudll\wxpng.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswudll\wxpng.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 LIB wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswuniv\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswuniv\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswuniv\wxpng.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswuniv\wxpng.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 LIB Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswu\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswu\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswu\wxpng.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswu\wxpng.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 LIB wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivd\wxpngd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivd\wxpngd.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswuddll\wxpngd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswuddll\wxpngd.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxpngd.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswddll\wxpngd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswddll\wxpngd.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 LIB Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_msw\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_msw\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_msw\wxpng.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_msw\wxpng.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 LIB wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivu\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivu\wxpng.pdb /I "..\src\zlib" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivu\wxpng.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivu\wxpng.lib"

!ENDIF

# Begin Target

# Name "wxpng - Win32 LIB wxUniv Debug Unicode"
# Name "wxpng - Win32 DLL Native Release Ansi"
# Name "wxpng - Win32 DLL wxUniv Debug Unicode"
# Name "wxpng - Win32 DLL wxUniv Release Unicode"
# Name "wxpng - Win32 DLL wxUniv Release Ansi"
# Name "wxpng - Win32 LIB Native Debug Unicode"
# Name "wxpng - Win32 DLL wxUniv Debug Ansi"
# Name "wxpng - Win32 LIB Native Debug Ansi"
# Name "wxpng - Win32 DLL Native Release Unicode"
# Name "wxpng - Win32 LIB wxUniv Release Ansi"
# Name "wxpng - Win32 LIB Native Release Unicode"
# Name "wxpng - Win32 LIB wxUniv Debug Ansi"
# Name "wxpng - Win32 DLL Native Debug Unicode"
# Name "wxpng - Win32 DLL Native Debug Ansi"
# Name "wxpng - Win32 LIB Native Release Ansi"
# Name "wxpng - Win32 LIB wxUniv Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\png\png.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngerror.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pnggccrd.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngget.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngmem.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngpread.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngread.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngrio.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngrtran.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngrutil.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngset.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngtrans.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngvcrd.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngwio.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngwrite.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngwtran.c
# End Source File
# Begin Source File

SOURCE=..\src\png\pngwutil.c
# End Source File
# End Group
# End Target
# End Project

