# Microsoft Developer Studio Project File - Name="wx_wxpng" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxpng - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxpng.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxpng.mak" CFG="wxpng - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxpng - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxpng - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxpng - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxpng.pdb /opt:ref /opt:icf /I "..\..\src\zlib" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxpng.pdb /opt:ref /opt:icf /I "..\..\src\zlib" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxpng.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxpng.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxpngd.pdb /I "..\..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxpngd.pdb /I "..\..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxpngd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxpngd.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxpng.pdb /opt:ref /opt:icf /I "..\..\src\zlib" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxpng.pdb /opt:ref /opt:icf /I "..\..\src\zlib" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxpng.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxpng.lib"

!ELSEIF  "$(CFG)" == "wxpng - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\wxpng"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\wxpng"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxpngd.pdb /I "..\..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxpngd.pdb /I "..\..\src\zlib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxpngd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxpngd.lib"

!ENDIF

# Begin Target

# Name "wxpng - Win32 DLL Release"
# Name "wxpng - Win32 DLL Debug"
# Name "wxpng - Win32 Release"
# Name "wxpng - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\png\png.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngerror.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngget.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngmem.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngpread.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngread.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngrio.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngrtran.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngrutil.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngset.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngtrans.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngwio.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngwrite.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngwtran.c
# End Source File
# Begin Source File

SOURCE=..\..\src\png\pngwutil.c
# End Source File
# End Group
# End Target
# End Project

