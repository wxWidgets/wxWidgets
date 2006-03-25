# Microsoft Developer Studio Project File - Name="MmediaVC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MmediaVC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MmediaVC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MmediaVC.mak" CFG="MmediaVC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MmediaVC - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MmediaVC - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MmediaVC - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "../../../include" /I "../../include" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\mmedia.lib"

!ELSEIF  "$(CFG)" == "MmediaVC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "../../../include" /I "../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\mmediad.lib"

!ENDIF 

# Begin Target

# Name "MmediaVC - Win32 Release"
# Name "MmediaVC - Win32 Debug"
# Begin Source File

SOURCE=.\cdbase.cpp
# End Source File
# Begin Source File

SOURCE=.\cdwin.cpp
# End Source File
# Begin Source File

SOURCE=.\g711.cpp
# End Source File
# Begin Source File

SOURCE=.\g721.cpp
# End Source File
# Begin Source File

SOURCE=.\g723_24.cpp
# End Source File
# Begin Source File

SOURCE=.\g723_40.cpp
# End Source File
# Begin Source File

SOURCE=.\g72x.cpp
# End Source File
# Begin Source File

SOURCE=.\sndaiff.cpp
# End Source File
# Begin Source File

SOURCE=.\sndbase.cpp
# End Source File
# Begin Source File

SOURCE=.\sndcodec.cpp
# End Source File
# Begin Source File

SOURCE=.\sndcpcm.cpp
# End Source File
# Begin Source File

SOURCE=.\sndfile.cpp
# End Source File
# Begin Source File

SOURCE=.\sndg72x.cpp
# End Source File
# Begin Source File

SOURCE=.\sndpcm.cpp
# End Source File
# Begin Source File

SOURCE=.\sndulaw.cpp
# End Source File
# Begin Source File

SOURCE=.\sndwav.cpp
# End Source File
# Begin Source File

SOURCE=.\sndwin.cpp
# End Source File
# Begin Source File

SOURCE=.\vidbase.cpp
# End Source File
# Begin Source File

SOURCE=.\vidwin.cpp
# End Source File
# End Target
# End Project
