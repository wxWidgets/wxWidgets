# Microsoft Developer Studio Project File - Name="wx_wxjpeg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxjpeg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxjpeg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxjpeg.mak" CFG="wxjpeg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxjpeg - Win32 DLL Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxjpeg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxjpeg - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxjpeg.pdb /opt:ref /opt:icf /I "..\..\lib\vc_dll\mswunivu" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxjpeg.pdb /opt:ref /opt:icf /I "..\..\lib\vc_dll\mswunivu" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxjpegd.pdb /I "..\..\lib\vc_dll\mswunivud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxjpegd.pdb /I "..\..\lib\vc_dll\mswunivud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxjpeg.pdb /opt:ref /opt:icf /I "..\..\lib\vc_dll\mswu" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxjpeg.pdb /opt:ref /opt:icf /I "..\..\lib\vc_dll\mswu" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxjpegd.pdb /I "..\..\lib\vc_dll\mswud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxjpegd.pdb /I "..\..\lib\vc_dll\mswud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxjpeg.pdb /opt:ref /opt:icf /I "..\..\lib\vc_lib\mswunivu" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxjpeg.pdb /opt:ref /opt:icf /I "..\..\lib\vc_lib\mswunivu" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxjpegd.pdb /I "..\..\lib\vc_lib\mswunivud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxjpegd.pdb /I "..\..\lib\vc_lib\mswunivud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxjpegd.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxjpeg.pdb /opt:ref /opt:icf /I "..\..\lib\vc_lib\mswu" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxjpeg.pdb /opt:ref /opt:icf /I "..\..\lib\vc_lib\mswu" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxjpeg.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxjpeg.lib"

!ELSEIF  "$(CFG)" == "wxjpeg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\wxjpeg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\wxjpeg"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxjpegd.pdb /I "..\..\lib\vc_lib\mswud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxjpegd.pdb /I "..\..\lib\vc_lib\mswud" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxjpegd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxjpegd.lib"

!ENDIF

# Begin Target

# Name "wxjpeg - Win32 DLL Universal Release"
# Name "wxjpeg - Win32 DLL Universal Debug"
# Name "wxjpeg - Win32 DLL Release"
# Name "wxjpeg - Win32 DLL Debug"
# Name "wxjpeg - Win32 Universal Release"
# Name "wxjpeg - Win32 Universal Debug"
# Name "wxjpeg - Win32 Release"
# Name "wxjpeg - Win32 Debug"
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

