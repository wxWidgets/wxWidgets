# Microsoft Developer Studio Project File - Name="wx_wxzlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxzlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxzlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxzlib.mak" CFG="wxzlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxzlib - Win32 DLL Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxzlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxzlib - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxzlib.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxzlib.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxzlib.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxzlib.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxzlib.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxzlib.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxzlibd.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxzlib.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxzlib.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxzlib.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxzlib.lib"

!ELSEIF  "$(CFG)" == "wxzlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\wxzlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\wxzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxzlibd.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxzlibd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxzlibd.lib"

!ENDIF

# Begin Target

# Name "wxzlib - Win32 DLL Universal Release"
# Name "wxzlib - Win32 DLL Universal Debug"
# Name "wxzlib - Win32 DLL Release"
# Name "wxzlib - Win32 DLL Debug"
# Name "wxzlib - Win32 Universal Release"
# Name "wxzlib - Win32 Universal Debug"
# Name "wxzlib - Win32 Release"
# Name "wxzlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\compress.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\infback.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\trees.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\zlib\zutil.c
# End Source File
# End Group
# End Target
# End Project

