# Microsoft Developer Studio Project File - Name="wxjpeg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxjpeg - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxjpeg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxjpeg.mak" CFG="wxjpeg - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxjpeg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxjpeg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswd\wxjpegd.pdb /I "..\..\lib\vc_mswd" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswd\wxjpegd.pdb /I "..\..\lib\vc_mswd" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswd\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswd\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_msw\wxjpeg.pdb /I "..\..\lib\vc_msw" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_msw\wxjpeg.pdb /I "..\..\lib\vc_msw" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_msw\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_msw\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswud\wxjpegd.pdb /I "..\..\lib\vc_mswud" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswud\wxjpegd.pdb /I "..\..\lib\vc_mswud" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswud\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswud\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswu\wxjpeg.pdb /I "..\..\lib\vc_mswu" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswu\wxjpeg.pdb /I "..\..\lib\vc_mswu" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswu\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswu\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivd\wxjpegd.pdb /I "..\..\lib\vc_mswunivd" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivd\wxjpegd.pdb /I "..\..\lib\vc_mswunivd" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivd\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivd\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswuniv\wxjpeg.pdb /I "..\..\lib\vc_mswuniv" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswuniv\wxjpeg.pdb /I "..\..\lib\vc_mswuniv" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswuniv\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswuniv\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivud\wxjpegd.pdb /I "..\..\lib\vc_mswunivud" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivud\wxjpegd.pdb /I "..\..\lib\vc_mswunivud" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivud\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivud\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivu\wxjpeg.pdb /I "..\..\lib\vc_mswunivu" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivu\wxjpeg.pdb /I "..\..\lib\vc_mswunivu" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivu\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivu\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxjpegd.pdb /I "..\..\lib\vc_mswddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxjpegd.pdb /I "..\..\lib\vc_mswddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswddll\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswddll\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswdll\wxjpeg.pdb /I "..\..\lib\vc_mswdll" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswdll\wxjpeg.pdb /I "..\..\lib\vc_mswdll" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswdll\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswdll\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxjpegd.pdb /I "..\..\lib\vc_mswuddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxjpegd.pdb /I "..\..\lib\vc_mswuddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswuddll\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswuddll\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswudll\wxjpeg.pdb /I "..\..\lib\vc_mswudll" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswudll\wxjpeg.pdb /I "..\..\lib\vc_mswudll" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswudll\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswudll\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxjpegd.pdb /I "..\..\lib\vc_mswunivddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxjpegd.pdb /I "..\..\lib\vc_mswunivddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivddll\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivddll\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivdll\wxjpeg.pdb /I "..\..\lib\vc_mswunivdll" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivdll\wxjpeg.pdb /I "..\..\lib\vc_mswunivdll" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivdll\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivdll\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxjpegd.pdb /I "..\..\lib\vc_mswunivuddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxjpegd.pdb /I "..\..\lib\vc_mswunivuddll" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivuddll\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivuddll\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivudll\wxjpeg.pdb /I "..\..\lib\vc_mswunivudll" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivudll\wxjpeg.pdb /I "..\..\lib\vc_mswunivudll" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivudll\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivudll\wxjpeg.lib"

!ENDIF

# Begin Target

# Name "wxjpeg - Win32 Debug"
# Name "wxjpeg - Win32 Release"
# Name "wxjpeg - Win32 Unicode Debug"
# Name "wxjpeg - Win32 Unicode Release"
# Name "wxjpeg - Win32 Universal Debug"
# Name "wxjpeg - Win32 Universal Release"
# Name "wxjpeg - Win32 Universal Unicode Debug"
# Name "wxjpeg - Win32 Universal Unicode Release"
# Name "wxjpeg - Win32 DLL Debug"
# Name "wxjpeg - Win32 DLL Release"
# Name "wxjpeg - Win32 DLL Unicode Debug"
# Name "wxjpeg - Win32 DLL Unicode Release"
# Name "wxjpeg - Win32 DLL Universal Debug"
# Name "wxjpeg - Win32 DLL Universal Release"
# Name "wxjpeg - Win32 DLL Universal Unicode Debug"
# Name "wxjpeg - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\jpeg\jcapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jccoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jccolor.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jchuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcinit.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcparam.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcprepct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jcsample.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jctrans.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdatadst.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdatasrc.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdinput.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdmerge.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdsample.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jdtrans.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jerror.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jfdctint.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jidctflt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jidctint.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jidctred.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jmemnobs.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jquant1.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jquant2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\jpeg\jutils.c
# End Source File
# End Group
# End Target
# End Project

