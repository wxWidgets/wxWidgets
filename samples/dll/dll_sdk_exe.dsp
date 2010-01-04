# Microsoft Developer Studio Project File - Name="dll_sdk_exe" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=sdk_exe - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dll_sdk_exe.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dll_sdk_exe.mak" CFG="sdk_exe - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sdk_exe - Win32 DLL Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "sdk_exe - Win32 DLL Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "sdk_exe - Win32 DLL Release" (based on "Win32 (x86) Application")
!MESSAGE "sdk_exe - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "sdk_exe - Win32 Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "sdk_exe - Win32 Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "sdk_exe - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "sdk_exe - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sdk_exe - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\sdk_exe"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\sdk_exe"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /Zi /Fdvc_mswunivudll\sdk_exe.pdb /opt:ref /opt:icf /O2 /GR /EHsc /D "WIN32" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /Zi /Fdvc_mswunivudll\sdk_exe.pdb /opt:ref /opt:icf /O2 /GR /EHsc /D "WIN32" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d _WINDOWS
# ADD RSC /l 0x409 /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vc_mswunivudll\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswunivudll\sdk_exe.exe" /debug /pdb:"vc_mswunivudll\sdk_exe.pdb" /subsystem:windows
# ADD LINK32 vc_mswunivudll\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswunivudll\sdk_exe.exe" /debug /pdb:"vc_mswunivudll\sdk_exe.pdb" /subsystem:windows

!ELSEIF  "$(CFG)" == "sdk_exe - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\sdk_exe"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\sdk_exe"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Zi /Fdvc_mswunivuddll\sdk_exe.pdb /Od /Gm /GR /EHsc /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /W1 /Zi /Fdvc_mswunivuddll\sdk_exe.pdb /Od /Gm /GR /EHsc /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d _WINDOWS
# ADD RSC /l 0x409 /d "_DEBUG" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vc_mswunivuddll\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\sdk_exe.exe" /debug /pdb:"vc_mswunivuddll\sdk_exe.pdb" /subsystem:windows
# ADD LINK32 vc_mswunivuddll\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\sdk_exe.exe" /debug /pdb:"vc_mswunivuddll\sdk_exe.pdb" /subsystem:windows

!ELSEIF  "$(CFG)" == "sdk_exe - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\sdk_exe"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\sdk_exe"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /Zi /Fdvc_mswudll\sdk_exe.pdb /opt:ref /opt:icf /O2 /GR /EHsc /D "WIN32" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /Zi /Fdvc_mswudll\sdk_exe.pdb /opt:ref /opt:icf /O2 /GR /EHsc /D "WIN32" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d _WINDOWS
# ADD RSC /l 0x409 /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vc_mswudll\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswudll\sdk_exe.exe" /debug /pdb:"vc_mswudll\sdk_exe.pdb" /subsystem:windows
# ADD LINK32 vc_mswudll\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswudll\sdk_exe.exe" /debug /pdb:"vc_mswudll\sdk_exe.pdb" /subsystem:windows

!ELSEIF  "$(CFG)" == "sdk_exe - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\sdk_exe"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\sdk_exe"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Zi /Fdvc_mswuddll\sdk_exe.pdb /Od /Gm /GR /EHsc /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /W1 /Zi /Fdvc_mswuddll\sdk_exe.pdb /Od /Gm /GR /EHsc /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d _WINDOWS
# ADD RSC /l 0x409 /d "_DEBUG" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vc_mswuddll\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswuddll\sdk_exe.exe" /debug /pdb:"vc_mswuddll\sdk_exe.pdb" /subsystem:windows
# ADD LINK32 vc_mswuddll\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswuddll\sdk_exe.exe" /debug /pdb:"vc_mswuddll\sdk_exe.pdb" /subsystem:windows

!ELSEIF  "$(CFG)" == "sdk_exe - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\sdk_exe"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\sdk_exe"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /Zi /Fdvc_mswunivu\sdk_exe.pdb /opt:ref /opt:icf /O2 /GR /EHsc /D "WIN32" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /Zi /Fdvc_mswunivu\sdk_exe.pdb /opt:ref /opt:icf /O2 /GR /EHsc /D "WIN32" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d _WINDOWS
# ADD RSC /l 0x409 /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vc_mswunivu\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswunivu\sdk_exe.exe" /debug /pdb:"vc_mswunivu\sdk_exe.pdb" /subsystem:windows
# ADD LINK32 vc_mswunivu\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswunivu\sdk_exe.exe" /debug /pdb:"vc_mswunivu\sdk_exe.pdb" /subsystem:windows

!ELSEIF  "$(CFG)" == "sdk_exe - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\sdk_exe"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\sdk_exe"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Zi /Fdvc_mswunivud\sdk_exe.pdb /Od /Gm /GR /EHsc /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /W1 /Zi /Fdvc_mswunivud\sdk_exe.pdb /Od /Gm /GR /EHsc /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d _WINDOWS
# ADD RSC /l 0x409 /d "_DEBUG" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vc_mswunivud\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswunivud\sdk_exe.exe" /debug /pdb:"vc_mswunivud\sdk_exe.pdb" /subsystem:windows
# ADD LINK32 vc_mswunivud\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswunivud\sdk_exe.exe" /debug /pdb:"vc_mswunivud\sdk_exe.pdb" /subsystem:windows

!ELSEIF  "$(CFG)" == "sdk_exe - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\sdk_exe"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\sdk_exe"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /Zi /Fdvc_mswu\sdk_exe.pdb /opt:ref /opt:icf /O2 /GR /EHsc /D "WIN32" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /Zi /Fdvc_mswu\sdk_exe.pdb /opt:ref /opt:icf /O2 /GR /EHsc /D "WIN32" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d _WINDOWS
# ADD RSC /l 0x409 /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vc_mswu\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswu\sdk_exe.exe" /debug /pdb:"vc_mswu\sdk_exe.pdb" /subsystem:windows
# ADD LINK32 vc_mswu\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswu\sdk_exe.exe" /debug /pdb:"vc_mswu\sdk_exe.pdb" /subsystem:windows

!ELSEIF  "$(CFG)" == "sdk_exe - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\sdk_exe"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\sdk_exe"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Zi /Fdvc_mswud\sdk_exe.pdb /Od /Gm /GR /EHsc /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /W1 /Zi /Fdvc_mswud\sdk_exe.pdb /Od /Gm /GR /EHsc /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d _WINDOWS
# ADD RSC /l 0x409 /d "_DEBUG" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vc_mswud\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswud\sdk_exe.exe" /debug /pdb:"vc_mswud\sdk_exe.pdb" /subsystem:windows
# ADD LINK32 vc_mswud\my_dll.lib user32.lib /nologo /machine:i386 /out:"vc_mswud\sdk_exe.exe" /debug /pdb:"vc_mswud\sdk_exe.pdb" /subsystem:windows

!ENDIF

# Begin Target

# Name "sdk_exe - Win32 DLL Universal Release"
# Name "sdk_exe - Win32 DLL Universal Debug"
# Name "sdk_exe - Win32 DLL Release"
# Name "sdk_exe - Win32 DLL Debug"
# Name "sdk_exe - Win32 Universal Release"
# Name "sdk_exe - Win32 Universal Debug"
# Name "sdk_exe - Win32 Release"
# Name "sdk_exe - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sdk_exe.cpp
# End Source File
# End Group
# End Target
# End Project

