# Microsoft Developer Studio Project File - Name="console" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=console - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "console.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "console.mak" CFG="console - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "console - Win32 Release Unicode DLL" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Debug Unicode DLL" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Debug Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Release DLL" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Debug DLL" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF "$(CFG)" == "console - Win32 Release Unicode DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BaseReleaseUnicodeDll"
# PROP BASE Intermediate_Dir "BaseReleaseUnicodeDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BaseReleaseUnicodeDll"
# PROP Intermediate_Dir "BaseReleaseUnicodeDll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /c
# ADD CPP /nologo /W4 /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D WIN32 /D WINVER=0x400 /D _MT /D wxUSE_GUI=0 /YX /FD /c /MD  /D WXUSINGDLL  /D _UNICODE /D UNICODE  /I..\..\lib\basedllu 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /I "../../include"
# ADD RSC /l 0x409 /d "NDEBUG" /I "../../include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib wsock32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib /nologo /subsystem:console /machine:I386 ..\..\lib\wxbase233u.lib

!IF "$(CFG)" == "console - Win32 Debug Unicode DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "BaseDebugUnicodeDll"
# PROP BASE Intermediate_Dir "BaseDebugUnicodeDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BaseDebugUnicodeDll"
# PROP Intermediate_Dir "BaseDebugUnicodeDll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /c
# ADD CPP /nologo /W4 /Zi /Od /I "../../include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D WIN32 /D WINVER=0x400 /D _MT /D wxUSE_GUI=0 /YX /FD /c /MDd /D "__WXDEBUG__" /D "WXDEBUG=1"  /D WXUSINGDLL  /D _UNICODE /D UNICODE  /I..\..\lib\basedllud 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /I "../../include"
# ADD RSC /l 0x409 /d "_DEBUG" /I "../../include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept ..\..\lib\wxbase233ud.lib

!IF "$(CFG)" == "console - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BaseReleaseUnicode"
# PROP BASE Intermediate_Dir "BaseReleaseUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BaseReleaseUnicode"
# PROP Intermediate_Dir "BaseReleaseUnicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /c
# ADD CPP /nologo /W4 /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D WIN32 /D WINVER=0x400 /D _MT /D wxUSE_GUI=0 /YX /FD /c /MD  /D _UNICODE /D UNICODE  /I..\..\lib\baseu 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /I "../../include"
# ADD RSC /l 0x409 /d "NDEBUG" /I "../../include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib wsock32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib /nologo /subsystem:console /machine:I386 ..\..\lib\zlib.lib ..\..\lib\regex.lib ..\..\lib\wxbaseu.lib

!IF "$(CFG)" == "console - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "BaseDebugUnicode"
# PROP BASE Intermediate_Dir "BaseDebugUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BaseDebugUnicode"
# PROP Intermediate_Dir "BaseDebugUnicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /c
# ADD CPP /nologo /W4 /Zi /Od /I "../../include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D WIN32 /D WINVER=0x400 /D _MT /D wxUSE_GUI=0 /YX /FD /c /MDd /D "__WXDEBUG__" /D "WXDEBUG=1"  /D _UNICODE /D UNICODE  /I..\..\lib\baseud 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /I "../../include"
# ADD RSC /l 0x409 /d "_DEBUG" /I "../../include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept ..\..\lib\zlibd.lib ..\..\lib\regexd.lib ..\..\lib\wxbaseud.lib

!IF "$(CFG)" == "console - Win32 Release DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BaseReleaseDll"
# PROP BASE Intermediate_Dir "BaseReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BaseReleaseDll"
# PROP Intermediate_Dir "BaseReleaseDll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /c
# ADD CPP /nologo /W4 /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D WIN32 /D WINVER=0x400 /D _MT /D wxUSE_GUI=0 /YX /FD /c /MD  /D WXUSINGDLL  /I..\..\lib\basedll 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /I "../../include"
# ADD RSC /l 0x409 /d "NDEBUG" /I "../../include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib wsock32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib /nologo /subsystem:console /machine:I386 ..\..\lib\wxbase233.lib

!IF "$(CFG)" == "console - Win32 Debug DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "BaseDebugDll"
# PROP BASE Intermediate_Dir "BaseDebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BaseDebugDll"
# PROP Intermediate_Dir "BaseDebugDll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /c
# ADD CPP /nologo /W4 /Zi /Od /I "../../include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D WIN32 /D WINVER=0x400 /D _MT /D wxUSE_GUI=0 /YX /FD /c /MDd /D "__WXDEBUG__" /D "WXDEBUG=1"  /D WXUSINGDLL  /I..\..\lib\basedlld 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /I "../../include"
# ADD RSC /l 0x409 /d "_DEBUG" /I "../../include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept ..\..\lib\wxbase233d.lib

!IF "$(CFG)" == "console - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BaseRelease"
# PROP BASE Intermediate_Dir "BaseRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BaseRelease"
# PROP Intermediate_Dir "BaseRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /c
# ADD CPP /nologo /W4 /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D WIN32 /D WINVER=0x400 /D _MT /D wxUSE_GUI=0 /YX /FD /c /MD  /I..\..\lib\base 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /I "../../include"
# ADD RSC /l 0x409 /d "NDEBUG" /I "../../include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib wsock32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib /nologo /subsystem:console /machine:I386 ..\..\lib\zlib.lib ..\..\lib\regex.lib ..\..\lib\wxbase.lib

!IF "$(CFG)" == "console - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "BaseDebug"
# PROP BASE Intermediate_Dir "BaseDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BaseDebug"
# PROP Intermediate_Dir "BaseDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /c
# ADD CPP /nologo /W4 /Zi /Od /I "../../include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D WIN32 /D WINVER=0x400 /D _MT /D wxUSE_GUI=0 /YX /FD /c /MDd /D "__WXDEBUG__" /D "WXDEBUG=1"  /I..\..\lib\based 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /I "../../include"
# ADD RSC /l 0x409 /d "_DEBUG" /I "../../include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib advapi32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept ..\..\lib\zlibd.lib ..\..\lib\regexd.lib ..\..\lib\wxbased.lib

!ENDIF 

# Begin Target

# Name "console - Win32 Release Unicode DLL"
# Name "console - Win32 Debug Unicode DLL"
# Name "console - Win32 Debug Unicode"
# Name "console - Win32 Release DLL"
# Name "console - Win32 Debug DLL"
# Name "console - Win32 Release"
# Name "console - Win32 Debug"
# Begin Source File

SOURCE=.\console.cpp
# End Source File
# End Target
# End Project
