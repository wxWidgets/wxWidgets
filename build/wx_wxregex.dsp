# Microsoft Developer Studio Project File - Name="wxregex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxregex - Win32 LIB wxUniv Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxregex.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxregex.mak" CFG="wxregex - Win32 LIB wxUniv Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxregex - Win32 LIB wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 LIB Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 LIB Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 LIB wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 LIB Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 LIB wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 LIB Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 LIB wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxregex - Win32 LIB wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivud\"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivud\"
# PROP Intermediate_Dir "vc_mswunivud\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivud\wxregexd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivud\wxregexd.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswdll\"
# PROP BASE Intermediate_Dir "vc_mswdll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswdll\"
# PROP Intermediate_Dir "vc_mswdll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswdll\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswdll\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswdll\wxregex.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswdll\wxregex.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivuddll\"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivuddll\"
# PROP Intermediate_Dir "vc_mswunivuddll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivuddll\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivuddll\wxregexd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivuddll\wxregexd.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivudll\"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivudll\"
# PROP Intermediate_Dir "vc_mswunivudll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivudll\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivudll\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivudll\wxregex.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivudll\wxregex.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivdll\"
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivdll\"
# PROP Intermediate_Dir "vc_mswunivdll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivdll\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivdll\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivdll\wxregex.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivdll\wxregex.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 LIB Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswud\"
# PROP BASE Intermediate_Dir "vc_mswud\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswud\"
# PROP Intermediate_Dir "vc_mswud\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswud\wxregexd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswud\wxregexd.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivddll\"
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivddll\"
# PROP Intermediate_Dir "vc_mswunivddll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivddll\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivddll\wxregexd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivddll\wxregexd.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 LIB Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswd\"
# PROP BASE Intermediate_Dir "vc_mswd\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswd\"
# PROP Intermediate_Dir "vc_mswd\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswd\wxregexd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswd\wxregexd.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswudll\"
# PROP BASE Intermediate_Dir "vc_mswudll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswudll\"
# PROP Intermediate_Dir "vc_mswudll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswudll\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswudll\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswudll\wxregex.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswudll\wxregex.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 LIB wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswuniv\"
# PROP BASE Intermediate_Dir "vc_mswuniv\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswuniv\"
# PROP Intermediate_Dir "vc_mswuniv\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswuniv\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswuniv\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswuniv\wxregex.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswuniv\wxregex.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 LIB Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswu\"
# PROP BASE Intermediate_Dir "vc_mswu\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswu\"
# PROP Intermediate_Dir "vc_mswu\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswu\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswu\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswu\wxregex.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswu\wxregex.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 LIB wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivd\"
# PROP BASE Intermediate_Dir "vc_mswunivd\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivd\"
# PROP Intermediate_Dir "vc_mswunivd\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivd\wxregexd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivd\wxregexd.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswuddll\"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswuddll\"
# PROP Intermediate_Dir "vc_mswuddll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswuddll\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswuddll\wxregexd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswuddll\wxregexd.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswddll\"
# PROP BASE Intermediate_Dir "vc_mswddll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswddll\"
# PROP Intermediate_Dir "vc_mswddll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\lib\vc_mswddll\wxregexd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswddll\wxregexd.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswddll\wxregexd.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 LIB Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_msw\"
# PROP BASE Intermediate_Dir "vc_msw\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_msw\"
# PROP Intermediate_Dir "vc_msw\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_msw\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_msw\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_msw\wxregex.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_msw\wxregex.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 LIB wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivu\"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivu\"
# PROP Intermediate_Dir "vc_mswunivu\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivu\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\lib\vc_mswunivu\wxregex.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivu\wxregex.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivu\wxregex.lib"

!ENDIF

# Begin Target

# Name "wxregex - Win32 LIB wxUniv Debug Unicode"
# Name "wxregex - Win32 DLL Native Release Ansi"
# Name "wxregex - Win32 DLL wxUniv Debug Unicode"
# Name "wxregex - Win32 DLL wxUniv Release Unicode"
# Name "wxregex - Win32 DLL wxUniv Release Ansi"
# Name "wxregex - Win32 LIB Native Debug Unicode"
# Name "wxregex - Win32 DLL wxUniv Debug Ansi"
# Name "wxregex - Win32 LIB Native Debug Ansi"
# Name "wxregex - Win32 DLL Native Release Unicode"
# Name "wxregex - Win32 LIB wxUniv Release Ansi"
# Name "wxregex - Win32 LIB Native Release Unicode"
# Name "wxregex - Win32 LIB wxUniv Debug Ansi"
# Name "wxregex - Win32 DLL Native Debug Unicode"
# Name "wxregex - Win32 DLL Native Debug Ansi"
# Name "wxregex - Win32 LIB Native Release Ansi"
# Name "wxregex - Win32 LIB wxUniv Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\regex\regcomp.c
# End Source File
# Begin Source File

SOURCE=..\src\regex\regerror.c
# End Source File
# Begin Source File

SOURCE=..\src\regex\regexec.c
# End Source File
# Begin Source File

SOURCE=..\src\regex\regfree.c
# End Source File
# End Group
# End Target
# End Project

