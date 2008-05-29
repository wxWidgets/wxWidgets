# Microsoft Developer Studio Project File - Name="wx_wxexpat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxexpat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxexpat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxexpat.mak" CFG="wxexpat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxexpat - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxexpat - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxexpat - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxexpat.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxexpat.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxexpatd.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxexpatd.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivdll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxexpat.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxexpat.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivddll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxexpatd.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxexpatd.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxexpat.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxexpat.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxexpatd.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxexpatd.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswdll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswdll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxexpat.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxexpat.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswddll\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswddll\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxexpatd.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxexpatd.pdb /I "..\..\lib\vc_dll" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxexpat.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxexpat.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxexpatd.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxexpatd.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswuniv\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswuniv\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxexpat.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxexpat.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivd\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivd\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxexpatd.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxexpatd.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxexpat.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxexpat.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxexpatd.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxexpatd.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpatd.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_msw\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_msw\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxexpat.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxexpat.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpat.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpat.lib"

!ELSEIF  "$(CFG)" == "wxexpat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswd\wxexpat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswd\wxexpat"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxexpatd.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxexpatd.pdb /I "..\..\lib\vc_lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "COMPILED_FROM_DSP" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpatd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxexpatd.lib"

!ENDIF

# Begin Target

# Name "wxexpat - Win32 DLL Universal Unicode Release"
# Name "wxexpat - Win32 DLL Universal Unicode Debug"
# Name "wxexpat - Win32 DLL Universal Release"
# Name "wxexpat - Win32 DLL Universal Debug"
# Name "wxexpat - Win32 DLL Unicode Release"
# Name "wxexpat - Win32 DLL Unicode Debug"
# Name "wxexpat - Win32 DLL Release"
# Name "wxexpat - Win32 DLL Debug"
# Name "wxexpat - Win32 Universal Unicode Release"
# Name "wxexpat - Win32 Universal Unicode Debug"
# Name "wxexpat - Win32 Universal Release"
# Name "wxexpat - Win32 Universal Debug"
# Name "wxexpat - Win32 Unicode Release"
# Name "wxexpat - Win32 Unicode Debug"
# Name "wxexpat - Win32 Release"
# Name "wxexpat - Win32 Debug"
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

