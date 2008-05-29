# Microsoft Developer Studio Project File - Name="wx_wxtiff" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxtiff - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxtiff.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxtiff.mak" CFG="wxtiff - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxtiff - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxtiff - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivdll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivddll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswdll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswdll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_dll\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswddll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswddll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswuniv\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswuniv\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivd\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivd\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_msw\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_msw\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Fd..\..\lib\vc_lib\wxtiff.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswd\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswd\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /D "WIN32" /D "_LIB" /D "_DEBUG" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiffd.lib"

!ENDIF

# Begin Target

# Name "wxtiff - Win32 DLL Universal Unicode Release"
# Name "wxtiff - Win32 DLL Universal Unicode Debug"
# Name "wxtiff - Win32 DLL Universal Release"
# Name "wxtiff - Win32 DLL Universal Debug"
# Name "wxtiff - Win32 DLL Unicode Release"
# Name "wxtiff - Win32 DLL Unicode Debug"
# Name "wxtiff - Win32 DLL Release"
# Name "wxtiff - Win32 DLL Debug"
# Name "wxtiff - Win32 Universal Unicode Release"
# Name "wxtiff - Win32 Universal Unicode Debug"
# Name "wxtiff - Win32 Universal Release"
# Name "wxtiff - Win32 Universal Debug"
# Name "wxtiff - Win32 Unicode Release"
# Name "wxtiff - Win32 Unicode Debug"
# Name "wxtiff - Win32 Release"
# Name "wxtiff - Win32 Debug"
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

SOURCE=..\..\src\tiff\tif_color.c
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

SOURCE=..\..\src\tiff\tif_extension.c
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

