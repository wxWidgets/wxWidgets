# Microsoft Developer Studio Project File - Name="dll_my_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=my_dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dll_my_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dll_my_dll.mak" CFG="my_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "my_dll - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "my_dll - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "my_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "my_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "my_dll - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\my_dll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\my_dll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Zi /Fdvc_mswudll\my_dll.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\..\lib\vc_dll\mswu" /I ".\..\..\include" /W4 /I "." /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "MY_DLL_BUILDING" /c
# ADD CPP /nologo /FD /MD /Zi /Fdvc_mswudll\my_dll.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\..\lib\vc_dll\mswu" /I ".\..\..\include" /W4 /I "." /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "MY_DLL_BUILDING" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "MY_DLL_BUILDING" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "MY_DLL_BUILDING" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswu" /i ".\..\..\include" /d "WXUSINGDLL" /d "MY_DLL_BUILDING" /i .
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswu" /i ".\..\..\include" /d "WXUSINGDLL" /d "MY_DLL_BUILDING" /i .
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw30u_core.lib wxbase30u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswudll\my_dll.dll" /implib:"vc_mswudll\my_dll.lib" /debug /pdb:"vc_mswudll\my_dll.pdb" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw30u_core.lib wxbase30u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswudll\my_dll.dll" /implib:"vc_mswudll\my_dll.lib" /debug /pdb:"vc_mswudll\my_dll.pdb" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "my_dll - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\my_dll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\my_dll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\my_dll.pdb /Od /Gm /GR /EHsc /I ".\..\..\lib\vc_dll\mswud" /I ".\..\..\include" /W4 /I "." /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "MY_DLL_BUILDING" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\my_dll.pdb /Od /Gm /GR /EHsc /I ".\..\..\lib\vc_dll\mswud" /I ".\..\..\include" /W4 /I "." /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "MY_DLL_BUILDING" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "MY_DLL_BUILDING" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "MY_DLL_BUILDING" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswud" /i ".\..\..\include" /d "WXUSINGDLL" /d "MY_DLL_BUILDING" /i .
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswud" /i ".\..\..\include" /d "WXUSINGDLL" /d "MY_DLL_BUILDING" /i .
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw30ud_core.lib wxbase30ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswuddll\my_dll.dll" /implib:"vc_mswuddll\my_dll.lib" /debug /pdb:"vc_mswuddll\my_dll.pdb" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw30ud_core.lib wxbase30ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswuddll\my_dll.dll" /implib:"vc_mswuddll\my_dll.lib" /debug /pdb:"vc_mswuddll\my_dll.pdb" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "my_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\my_dll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\my_dll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Zi /Fdvc_mswu\my_dll.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\..\lib\vc_lib\mswu" /I ".\..\..\include" /W4 /I "." /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "MY_DLL_BUILDING" /c
# ADD CPP /nologo /FD /MD /Zi /Fdvc_mswu\my_dll.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\..\lib\vc_lib\mswu" /I ".\..\..\include" /W4 /I "." /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "MY_DLL_BUILDING" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "MY_DLL_BUILDING" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "MY_DLL_BUILDING" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswu" /i ".\..\..\include" /d "MY_DLL_BUILDING" /i .
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswu" /i ".\..\..\include" /d "MY_DLL_BUILDING" /i .
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw30u_core.lib wxbase30u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswu\my_dll.dll" /implib:"vc_mswu\my_dll.lib" /debug /pdb:"vc_mswu\my_dll.pdb" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw30u_core.lib wxbase30u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswu\my_dll.dll" /implib:"vc_mswu\my_dll.lib" /debug /pdb:"vc_mswu\my_dll.pdb" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "my_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\my_dll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\my_dll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswud\my_dll.pdb /Od /Gm /GR /EHsc /I ".\..\..\lib\vc_lib\mswud" /I ".\..\..\include" /W4 /I "." /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "MY_DLL_BUILDING" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswud\my_dll.pdb /Od /Gm /GR /EHsc /I ".\..\..\lib\vc_lib\mswud" /I ".\..\..\include" /W4 /I "." /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "MY_DLL_BUILDING" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "MY_DLL_BUILDING" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "MY_DLL_BUILDING" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswud" /i ".\..\..\include" /d "MY_DLL_BUILDING" /i .
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswud" /i ".\..\..\include" /d "MY_DLL_BUILDING" /i .
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw30ud_core.lib wxbase30ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswud\my_dll.dll" /implib:"vc_mswud\my_dll.lib" /debug /pdb:"vc_mswud\my_dll.pdb" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw30ud_core.lib wxbase30ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswud\my_dll.dll" /implib:"vc_mswud\my_dll.lib" /debug /pdb:"vc_mswud\my_dll.pdb" /libpath:".\..\..\lib\vc_lib"

!ENDIF

# Begin Target

# Name "my_dll - Win32 DLL Release"
# Name "my_dll - Win32 DLL Debug"
# Name "my_dll - Win32 Release"
# Name "my_dll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\my_dll.cpp
# End Source File
# End Group
# End Target
# End Project

