# Microsoft Developer Studio Project File - Name="wxzlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxzlib - Win32 LIB wxUniv Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxzlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxzlib.mak" CFG="wxzlib - Win32 LIB wxUniv Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxzlib - Win32 LIB wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 LIB Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 LIB Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 LIB wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 LIB Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 LIB wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 LIB Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 LIB wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxzlib - Win32 LIB wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivud\"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivud\"
# PROP Intermediate_Dir "vc_mswunivud\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivud\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivud\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswdll\"
# PROP BASE Intermediate_Dir "vc_mswdll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswdll\"
# PROP Intermediate_Dir "vc_mswdll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswdll\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswdll\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswdll\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswdll\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivuddll\"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivuddll\"
# PROP Intermediate_Dir "vc_mswunivuddll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivuddll\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivuddll\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivudll\"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivudll\"
# PROP Intermediate_Dir "vc_mswunivudll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivudll\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivudll\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivudll\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivudll\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivdll\"
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivdll\"
# PROP Intermediate_Dir "vc_mswunivdll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivdll\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivdll\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivdll\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivdll\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 LIB Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswud\"
# PROP BASE Intermediate_Dir "vc_mswud\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswud\"
# PROP Intermediate_Dir "vc_mswud\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswud\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswud\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivddll\"
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivddll\"
# PROP Intermediate_Dir "vc_mswunivddll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivddll\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivddll\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 LIB Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswd\"
# PROP BASE Intermediate_Dir "vc_mswd\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswd\"
# PROP Intermediate_Dir "vc_mswd\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswd\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswd\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswudll\"
# PROP BASE Intermediate_Dir "vc_mswudll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswudll\"
# PROP Intermediate_Dir "vc_mswudll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswudll\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswudll\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswudll\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswudll\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 LIB wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswuniv\"
# PROP BASE Intermediate_Dir "vc_mswuniv\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswuniv\"
# PROP Intermediate_Dir "vc_mswuniv\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswuniv\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswuniv\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswuniv\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswuniv\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 LIB Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswu\"
# PROP BASE Intermediate_Dir "vc_mswu\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswu\"
# PROP Intermediate_Dir "vc_mswu\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswu\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswu\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswu\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswu\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 LIB wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivd\"
# PROP BASE Intermediate_Dir "vc_mswunivd\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivd\"
# PROP Intermediate_Dir "vc_mswunivd\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivd\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivd\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswuddll\"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswuddll\"
# PROP Intermediate_Dir "vc_mswuddll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswuddll\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswuddll\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswddll\"
# PROP BASE Intermediate_Dir "vc_mswddll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswddll\"
# PROP Intermediate_Dir "vc_mswddll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswddll\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswddll\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 LIB Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_msw\"
# PROP BASE Intermediate_Dir "vc_msw\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_msw\"
# PROP Intermediate_Dir "vc_msw\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_msw\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_msw\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_msw\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_msw\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 LIB wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivu\"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivu\"
# PROP Intermediate_Dir "vc_mswunivu\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivu\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivu\wxzlib.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivu\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivu\wxzlib.lib"

!ENDIF

# Begin Target

# Name "wxzlib - Win32 LIB wxUniv Debug Unicode"
# Name "wxzlib - Win32 DLL Native Release Ansi"
# Name "wxzlib - Win32 DLL wxUniv Debug Unicode"
# Name "wxzlib - Win32 DLL wxUniv Release Unicode"
# Name "wxzlib - Win32 DLL wxUniv Release Ansi"
# Name "wxzlib - Win32 LIB Native Debug Unicode"
# Name "wxzlib - Win32 DLL wxUniv Debug Ansi"
# Name "wxzlib - Win32 LIB Native Debug Ansi"
# Name "wxzlib - Win32 DLL Native Release Unicode"
# Name "wxzlib - Win32 LIB wxUniv Release Ansi"
# Name "wxzlib - Win32 LIB Native Release Unicode"
# Name "wxzlib - Win32 LIB wxUniv Debug Ansi"
# Name "wxzlib - Win32 DLL Native Debug Unicode"
# Name "wxzlib - Win32 DLL Native Debug Ansi"
# Name "wxzlib - Win32 LIB Native Release Ansi"
# Name "wxzlib - Win32 LIB wxUniv Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\compress.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\infblock.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\infcodes.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\infutil.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\trees.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=..\src\zlib\zutil.c
# End Source File
# End Group
# End Target
# End Project

