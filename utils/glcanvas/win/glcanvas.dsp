# Microsoft Developer Studio Project File - Name="glcanvas" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=glcanvas - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "glcanvas.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "glcanvas.mak" CFG="glcanvas - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "glcanvas - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "glcanvas - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "glcanvas - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\include\wx\msw" /I "..\..\..\include\wx\generic" /I "..\..\..\include\wx\other" /I "..\..\..\src\png" /I "..\..\..\src\zlib" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D "__WINDOWS__" /D "STRICT" /Fp"..\..\..\Release\wxWin.pch" /YX"wx\wxprec.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "glcanvas - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Gf /Gy /I "..\..\..\include" /I "..\..\..\include\wx\msw" /I "..\..\..\include\wx\generic" /I "..\..\..\include\wx\other" /I "..\..\..\src\png" /I "..\..\..\src\zlib" /D "__WXDEBUG__" /D WXDEBUG=1 /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D "__WINDOWS__" /D "STRICT" /Fp"..\..\..\Debug\wxWin.pch" /YX"wx\wxprec.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "glcanvas - Win32 Release"
# Name "glcanvas - Win32 Debug"
# Begin Source File

SOURCE=.\glcanvas.cpp
# End Source File
# Begin Source File

SOURCE=.\glcanvas.h
# End Source File
# End Target
# End Project
