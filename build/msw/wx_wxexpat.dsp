# Microsoft Developer Studio Project File - Name="wxexpat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxexpat - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxexpat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxexpat.mak" CFG="wxexpat - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxexpat - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxexpat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswd\wxexpatd.pdb /I "..\..\lib\vc_mswd" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswd\wxexpatd.pdb /I "..\..\lib\vc_mswd" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswd\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswd\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_msw\wxexpat.pdb /I "..\..\lib\vc_msw" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_msw\wxexpat.pdb /I "..\..\lib\vc_msw" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_msw\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_msw\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswud\wxexpatd.pdb /I "..\..\lib\vc_mswud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswud\wxexpatd.pdb /I "..\..\lib\vc_mswud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswud\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswud\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswu\wxexpat.pdb /I "..\..\lib\vc_mswu" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswu\wxexpat.pdb /I "..\..\lib\vc_mswu" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswu\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswu\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivd\wxexpatd.pdb /I "..\..\lib\vc_mswunivd" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivd\wxexpatd.pdb /I "..\..\lib\vc_mswunivd" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivd\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivd\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswuniv\wxexpat.pdb /I "..\..\lib\vc_mswuniv" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswuniv\wxexpat.pdb /I "..\..\lib\vc_mswuniv" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswuniv\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswuniv\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivud\wxexpatd.pdb /I "..\..\lib\vc_mswunivud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivud\wxexpatd.pdb /I "..\..\lib\vc_mswunivud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivud\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivud\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivu\wxexpat.pdb /I "..\..\lib\vc_mswunivu" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivu\wxexpat.pdb /I "..\..\lib\vc_mswunivu" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivu\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivu\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxexpatd.pdb /I "..\..\lib\vc_mswddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxexpatd.pdb /I "..\..\lib\vc_mswddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswddll\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswddll\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswdll\wxexpat.pdb /I "..\..\lib\vc_mswdll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswdll\wxexpat.pdb /I "..\..\lib\vc_mswdll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswdll\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswdll\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxexpatd.pdb /I "..\..\lib\vc_mswuddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxexpatd.pdb /I "..\..\lib\vc_mswuddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswuddll\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswuddll\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswudll\wxexpat.pdb /I "..\..\lib\vc_mswudll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswudll\wxexpat.pdb /I "..\..\lib\vc_mswudll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswudll\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswudll\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxexpatd.pdb /I "..\..\lib\vc_mswunivddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxexpatd.pdb /I "..\..\lib\vc_mswunivddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivddll\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivddll\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivdll\wxexpat.pdb /I "..\..\lib\vc_mswunivdll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivdll\wxexpat.pdb /I "..\..\lib\vc_mswunivdll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivdll\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivdll\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxexpatd.pdb /I "..\..\lib\vc_mswunivuddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxexpatd.pdb /I "..\..\lib\vc_mswunivuddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivuddll\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivuddll\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivudll\wxexpat.pdb /I "..\..\lib\vc_mswunivudll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivudll\wxexpat.pdb /I "..\..\lib\vc_mswunivudll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivudll\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivudll\wxexpat.lib"

!ENDIF

# Begin Target

# Name "wxexpat - Win32 Debug"
# Name "wxexpat - Win32 Release"
# Name "wxexpat - Win32 Unicode Debug"
# Name "wxexpat - Win32 Unicode Release"
# Name "wxexpat - Win32 Universal Debug"
# Name "wxexpat - Win32 Universal Release"
# Name "wxexpat - Win32 Universal Unicode Debug"
# Name "wxexpat - Win32 Universal Unicode Release"
# Name "wxexpat - Win32 DLL Debug"
# Name "wxexpat - Win32 DLL Release"
# Name "wxexpat - Win32 DLL Unicode Debug"
# Name "wxexpat - Win32 DLL Unicode Release"
# Name "wxexpat - Win32 DLL Universal Debug"
# Name "wxexpat - Win32 DLL Universal Release"
# Name "wxexpat - Win32 DLL Universal Unicode Debug"
# Name "wxexpat - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\expat\lib\xmlparse.c
# End Source File
# Begin Source File

SOURCE=..\..\src\expat\lib\xmlrole.c
# End Source File
# Begin Source File

SOURCE=..\..\src\expat\lib\xmltok.c
# End Source File
# End Group
# End Target
# End Project

