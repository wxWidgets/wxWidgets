# Microsoft Developer Studio Project File - Name="wxtiff" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxtiff - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxtiff.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxtiff.mak" CFG="wxtiff - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxtiff - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 wxUniv Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 wxUniv Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 wxUniv Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 wxUniv Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL wxUniv Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL wxUniv Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL wxUniv Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL wxUniv Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxtiff - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswd\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswd\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswd\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswd\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_msw\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_msw\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_msw\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_msw\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswud\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswud\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswud\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswud\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswu\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswu\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswu\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswu\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 wxUniv Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivd\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivd\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivd\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivd\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 wxUniv Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswuniv\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswuniv\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswuniv\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswuniv\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 wxUniv Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivud\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivud\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivud\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivud\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 wxUniv Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivu\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivu\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivu\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivu\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswddll\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswddll\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswdll\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswdll\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswdll\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswdll\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswuddll\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswuddll\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswudll\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswudll\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswudll\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswudll\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL wxUniv Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivddll\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivddll\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL wxUniv Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivdll\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivdll\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivdll\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivdll\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL wxUniv Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxtiffd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivuddll\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivuddll\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL wxUniv Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivudll\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /Fd..\..\lib\vc_mswunivudll\wxtiff.pdb /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivudll\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivudll\wxtiff.lib"

!ENDIF

# Begin Target

# Name "wxtiff - Win32 Debug"
# Name "wxtiff - Win32 Release"
# Name "wxtiff - Win32 Unicode Debug"
# Name "wxtiff - Win32 Unicode Release"
# Name "wxtiff - Win32 wxUniv Debug"
# Name "wxtiff - Win32 wxUniv Release"
# Name "wxtiff - Win32 wxUniv Unicode Debug"
# Name "wxtiff - Win32 wxUniv Unicode Release"
# Name "wxtiff - Win32 DLL Debug"
# Name "wxtiff - Win32 DLL Release"
# Name "wxtiff - Win32 DLL Unicode Debug"
# Name "wxtiff - Win32 DLL Unicode Release"
# Name "wxtiff - Win32 DLL wxUniv Debug"
# Name "wxtiff - Win32 DLL wxUniv Release"
# Name "wxtiff - Win32 DLL wxUniv Unicode Debug"
# Name "wxtiff - Win32 DLL wxUniv Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\tiff\tif_aux.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_close.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_codec.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_compress.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_dir.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_dirinfo.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_dirread.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_dirwrite.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_dumpmode.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_error.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_fax3.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_fax3sm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_flush.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_getimage.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_jpeg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_luv.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_lzw.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_next.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_open.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_packbits.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_pixarlog.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_predict.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_print.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_read.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_strip.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_swab.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_thunder.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_tile.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_version.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_warning.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_win32.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_write.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\tif_zip.c
# End Source File
# End Group
# End Target
# End Project

