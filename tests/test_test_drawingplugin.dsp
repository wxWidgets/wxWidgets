# Microsoft Developer Studio Project File - Name="test_test_drawingplugin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=test_drawingplugin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "test_test_drawingplugin.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "test_test_drawingplugin.mak" CFG="test_drawingplugin - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "test_drawingplugin - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "test_drawingplugin - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "test_drawingplugin - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "test_drawingplugin - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test_drawingplugin - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\test_drawingplugin"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\test_drawingplugin"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Zi /Fdvc_mswudll\test_drawingplugin.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswu" /I ".\..\include" /W4 /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /c
# ADD CPP /nologo /FD /MD /Zi /Fdvc_mswudll\test_drawingplugin.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswu" /I ".\..\include" /W4 /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\lib\vc_dll\mswu" /i ".\..\include" /d WXUSINGDLL
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\lib\vc_dll\mswu" /i ".\..\include" /d WXUSINGDLL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw31u_core.lib wxbase31u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswudll\test_drawingplugin.dll" /debug /pdb:"vc_mswudll\test_drawingplugin.pdb" /libpath:".\..\lib\vc_dll"
# ADD LINK32 wxmsw31u_core.lib wxbase31u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswudll\test_drawingplugin.dll" /debug /pdb:"vc_mswudll\test_drawingplugin.pdb" /libpath:".\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "test_drawingplugin - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\test_drawingplugin"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\test_drawingplugin"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\test_drawingplugin.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_dll\mswud" /I ".\..\include" /W4 /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\test_drawingplugin.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_dll\mswud" /I ".\..\include" /W4 /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswud" /i ".\..\include" /d WXUSINGDLL
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswud" /i ".\..\include" /d WXUSINGDLL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw31ud_core.lib wxbase31ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswuddll\test_drawingplugin.dll" /debug /pdb:"vc_mswuddll\test_drawingplugin.pdb" /libpath:".\..\lib\vc_dll"
# ADD LINK32 wxmsw31ud_core.lib wxbase31ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswuddll\test_drawingplugin.dll" /debug /pdb:"vc_mswuddll\test_drawingplugin.pdb" /libpath:".\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "test_drawingplugin - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\test_drawingplugin"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\test_drawingplugin"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Zi /Fdvc_mswu\test_drawingplugin.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswu" /I ".\..\include" /W4 /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /Zi /Fdvc_mswu\test_drawingplugin.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswu" /I ".\..\include" /W4 /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\lib\vc_lib\mswu" /i .\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\lib\vc_lib\mswu" /i .\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw31u_core.lib wxbase31u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswu\test_drawingplugin.dll" /debug /pdb:"vc_mswu\test_drawingplugin.pdb" /libpath:".\..\lib\vc_lib"
# ADD LINK32 wxmsw31u_core.lib wxbase31u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswu\test_drawingplugin.dll" /debug /pdb:"vc_mswu\test_drawingplugin.pdb" /libpath:".\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "test_drawingplugin - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\test_drawingplugin"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\test_drawingplugin"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswud\test_drawingplugin.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_lib\mswud" /I ".\..\include" /W4 /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswud\test_drawingplugin.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_lib\mswud" /I ".\..\include" /W4 /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswud" /i .\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswud" /i .\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw31ud_core.lib wxbase31ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswud\test_drawingplugin.dll" /debug /pdb:"vc_mswud\test_drawingplugin.pdb" /libpath:".\..\lib\vc_lib"
# ADD LINK32 wxmsw31ud_core.lib wxbase31ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /dll /machine:i386 /out:"vc_mswud\test_drawingplugin.dll" /debug /pdb:"vc_mswud\test_drawingplugin.pdb" /libpath:".\..\lib\vc_lib"

!ENDIF

# Begin Target

# Name "test_drawingplugin - Win32 DLL Release"
# Name "test_drawingplugin - Win32 DLL Debug"
# Name "test_drawingplugin - Win32 Release"
# Name "test_drawingplugin - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drawing\pluginsample.cpp
# End Source File
# End Group
# End Target
# End Project
