# Microsoft Developer Studio Project File - Name="widgets" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=widgets - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "widgets.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "widgets.mak" CFG="widgets - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "widgets - Win32 Release DLL" (based on "Win32 (x86) Application")
!MESSAGE "widgets - Win32 Debug DLL" (based on "Win32 (x86) Application")
!MESSAGE "widgets - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "widgets - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!ELSEIF "$(CFG)" == "widgets - Win32 Release DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "UnivReleaseDll"
# PROP BASE Intermediate_Dir "UnivReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UnivReleaseDll"
# PROP Intermediate_Dir "UnivReleaseDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /YX /FD /c
# ADD CPP /nologo /W4 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D WIN32 /D WINVER=0x400 /D wxUSE_GUI=1 /D "__WXUNIVERSAL__" /YX /FD /c /MD  /D WXUSINGDLL  /I..\..\lib\univdll 
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /I "..\..\include"
# ADD RSC /l 0x409 /d "NDEBUG" /I "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /machine:I386 ..\..\lib\wxuniv233.lib

!ELSEIF "$(CFG)" == "widgets - Win32 Debug DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UnivDebugDll"
# PROP BASE Intermediate_Dir "UnivDebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UnivDebugDll"
# PROP Intermediate_Dir "UnivDebugDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /YX /FD /c
# ADD CPP /nologo /W4 /Zi /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D WIN32 /D WINVER=0x400 /D wxUSE_GUI=1 /D "__WXUNIVERSAL__" /YX /FD /c /MDd /D "__WXDEBUG__" /D "WXDEBUG=1"  /D WXUSINGDLL  /I..\..\lib\univdlld 
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /I "..\..\include"
# ADD RSC /l 0x409 /d "_DEBUG" /I "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept ..\..\lib\wxuniv233d.lib

!ELSEIF "$(CFG)" == "widgets - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "UnivRelease"
# PROP BASE Intermediate_Dir "UnivRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UnivRelease"
# PROP Intermediate_Dir "UnivRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /YX /FD /c
# ADD CPP /nologo /W4 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D WIN32 /D WINVER=0x400 /D wxUSE_GUI=1 /D "__WXUNIVERSAL__" /YX /FD /c /MD  /I..\..\lib\univ 
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /I "..\..\include"
# ADD RSC /l 0x409 /d "NDEBUG" /I "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /machine:I386 ..\..\lib\zlib.lib ..\..\lib\regex.lib ..\..\lib\png.lib ..\..\lib\jpeg.lib ..\..\lib\tiff.lib ..\..\lib\wxuniv.lib

!ELSEIF "$(CFG)" == "widgets - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UnivDebug"
# PROP BASE Intermediate_Dir "UnivDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UnivDebug"
# PROP Intermediate_Dir "UnivDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /YX /FD /c
# ADD CPP /nologo /W4 /Zi /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D WIN32 /D WINVER=0x400 /D wxUSE_GUI=1 /D "__WXUNIVERSAL__" /YX /FD /c /MDd /D "__WXDEBUG__" /D "WXDEBUG=1"  /I..\..\lib\univd 
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /I "..\..\include"
# ADD RSC /l 0x409 /d "_DEBUG" /I "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept ..\..\lib\zlibd.lib ..\..\lib\regexd.lib ..\..\lib\pngd.lib ..\..\lib\jpegd.lib ..\..\lib\tiffd.lib ..\..\lib\wxunivd.lib

!ENDIF 

# Begin Target

# Name "widgets - Win32 Release DLL"
# Name "widgets - Win32 Debug DLL"
# Name "widgets - Win32 Release"
# Name "widgets - Win32 Debug"
# Begin Source File

SOURCE=.\button.cpp
# End Source File
# Begin Source File

SOURCE=.\combobox.cpp
# End Source File
# Begin Source File

SOURCE=.\gauge.cpp
# End Source File
# Begin Source File

SOURCE=.\listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\notebook.cpp
# End Source File
# Begin Source File

SOURCE=.\radiobox.cpp
# End Source File
# Begin Source File

SOURCE=.\slider.cpp
# End Source File
# Begin Source File

SOURCE=.\spinbtn.cpp
# End Source File
# Begin Source File

SOURCE=.\static.cpp
# End Source File
# Begin Source File

SOURCE=.\textctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\widgets.cpp
# End Source File
# Begin Source File

SOURCE=.\widgets.rc
# End Source File
# End Target
# End Project


