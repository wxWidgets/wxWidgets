# Microsoft Developer Studio Project File - Name="flVC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=flVC - Win32 Debug Unicode DLL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "flVC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "flVC.mak" CFG="flVC - Win32 Debug Unicode DLL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "flVC - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "flVC - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "flVC - Win32 Debug DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "flVC - Win32 Release DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "flVC - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "flVC - Win32 Release Unicode DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "flVC - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "flVC - Win32 Debug Unicode DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "flVC - Win32 Release"

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
LINK32=link.exe
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "../../../include" /I "../../include" /I "../../../lib/msw" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__HACK_MY_MSDEV40__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\fl.lib"

!ELSEIF  "$(CFG)" == "flVC - Win32 Debug"

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
LINK32=link.exe
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\fld.lib"

!ELSEIF  "$(CFG)" == "flVC - Win32 Debug DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_DLL"
# PROP BASE Intermediate_Dir "Debug_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "Debug_DLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\fld.lib"
# ADD LIB32 /nologo /out:"..\..\..\lib\fld.lib"
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswdlld" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "WXUSINGDLL" /D "WXMAKING_FL_DLL" /YX /FD /c
# SUBTRACT CPP /u
MTL=midl.exe
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib wxmsw24d.lib /nologo /version:1.0 /dll /debug /machine:IX86 /out:"../../../lib/fldlld.dll" /libpath:"../../../lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "flVC - Win32 Release DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "flVC___Win32_Release_DLL"
# PROP BASE Intermediate_Dir "flVC___Win32_Release_DLL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "Release_DLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\fld.lib"
# ADD LIB32 /nologo /out:"..\..\..\lib\fld.lib"
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswdlld" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXUSINGDLL=1 /D "WXBUILD_FL_DLL" /YX /FD /c
# SUBTRACT BASE CPP /u
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "../../../include" /I "../../include" /I "../../../lib/mswdll" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "WXUSINGDLL" /D "WXMAKING_FL_DLL" /YX /FD /c
# SUBTRACT CPP /u
MTL=midl.exe
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib wxmsw24d.lib /nologo /version:1.0 /dll /debug /machine:IX86 /out:"../../../lib/fldlld.dll" /libpath:"../../../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib wxmsw24.lib /nologo /version:1.0 /dll /machine:IX86 /out:"../../../lib/fldll.dll" /libpath:"../../../lib"
# SUBTRACT LINK32 /pdb:none /incremental:no /debug

!ELSEIF  "$(CFG)" == "flVC - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "flVC___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "flVC___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Unicode"
# PROP Intermediate_Dir "Release_Unicode"
# PROP Target_Dir ""
LINK32=link.exe
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "../../../include" /I "../../include" /I "../../../lib/msw" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__HACK_MY_MSDEV40__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "../../../include" /I "../../include" /I "../../../lib/mswu" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__HACK_MY_MSDEV40__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "UNICODE" /D "_UNICODE" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\fl.lib"
# ADD LIB32 /nologo /out:"..\..\..\lib\flu.lib"

!ELSEIF  "$(CFG)" == "flVC - Win32 Release Unicode DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "flVC___Win32_Release_Unicode_DLL"
# PROP BASE Intermediate_Dir "flVC___Win32_Release_Unicode_DLL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release_Unicode_DLL"
# PROP Intermediate_Dir "Release_Unicode_DLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\fld.lib"
# ADD LIB32 /nologo /out:"..\..\..\lib\fld.lib"
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "../../../include" /I "../../include" /I "../../../lib/mswdlld" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "WXUSINGDLL" /D "WXMAKING_FL_DLL" /YX /FD /c
# SUBTRACT BASE CPP /u
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "../../../include" /I "../../include" /I "../../../lib/mswdllu" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "WXUSINGDLL" /D "WXMAKING_FL_DLL" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# SUBTRACT CPP /u
MTL=midl.exe
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib wxmsw24.lib /nologo /version:1.0 /dll /machine:IX86 /out:"../../../lib/fldll.dll" /libpath:"../../../lib"
# SUBTRACT BASE LINK32 /pdb:none /incremental:no /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib wxmsw24u.lib /nologo /version:1.0 /dll /machine:IX86 /out:"../../../lib/fldllu.dll" /libpath:"../../../lib"
# SUBTRACT LINK32 /pdb:none /incremental:no /debug

!ELSEIF  "$(CFG)" == "flVC - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "flVC___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "flVC___Win32_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Target_Dir ""
LINK32=link.exe
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswud" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "UNICODE" /D "_UNICODE" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\fld.lib"
# ADD LIB32 /nologo /out:"..\..\..\lib\flud.lib"

!ELSEIF  "$(CFG)" == "flVC - Win32 Debug Unicode DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "flVC___Win32_Debug_Unicode_DLL"
# PROP BASE Intermediate_Dir "flVC___Win32_Debug_Unicode_DLL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode_DLL"
# PROP Intermediate_Dir "Debug_Unicode_DLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\fld.lib"
# ADD LIB32 /nologo /out:"..\..\..\lib\fld.lib"
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswdlld" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "WXUSINGDLL" /D "WXMAKING_FL_DLL" /YX /FD /c
# SUBTRACT BASE CPP /u
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswdllud" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "WXUSINGDLL" /D "WXMAKING_FL_DLL" /D "UNICODE" /D "_UNICODE" /YX /FD /c
# SUBTRACT CPP /u
MTL=midl.exe
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib wxmsw24d.lib /nologo /version:1.0 /dll /debug /machine:IX86 /out:"../../../lib/fldlld.dll" /libpath:"../../../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib wxmsw24ud.lib /nologo /version:1.0 /dll /debug /machine:IX86 /out:"../../../lib/fldllud.dll" /libpath:"../../../lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "flVC - Win32 Release"
# Name "flVC - Win32 Debug"
# Name "flVC - Win32 Debug DLL"
# Name "flVC - Win32 Release DLL"
# Name "flVC - Win32 Release Unicode"
# Name "flVC - Win32 Release Unicode DLL"
# Name "flVC - Win32 Debug Unicode"
# Name "flVC - Win32 Debug Unicode DLL"
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\fl\antiflickpl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\bardragpl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\barhintspl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\cbcustom.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\controlbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\dynbarhnd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\dyntbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\dyntbarhnd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\fldefs.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\frmview.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\garbagec.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\gcupdatesmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\hintanimpl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\newbmpbtn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\panedrawpl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\rowdragpl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\rowlayoutpl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\toolwnd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\fl\updatesmgr.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\antiflickpl.cpp
# End Source File
# Begin Source File

SOURCE=.\bardragpl.cpp
# End Source File
# Begin Source File

SOURCE=.\barhintspl.cpp
# End Source File
# Begin Source File

SOURCE=.\cbcustom.cpp
# End Source File
# Begin Source File

SOURCE=.\controlbar.cpp
# End Source File
# Begin Source File

SOURCE=.\dyntbar.cpp
# End Source File
# Begin Source File

SOURCE=.\dyntbarhnd.cpp
# End Source File
# Begin Source File

SOURCE=.\garbagec.cpp
# End Source File
# Begin Source File

SOURCE=.\gcupdatesmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\hintanimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\newbmpbtn.cpp
# End Source File
# Begin Source File

SOURCE=.\panedrawpl.cpp
# End Source File
# Begin Source File

SOURCE=.\rowdragpl.cpp
# End Source File
# Begin Source File

SOURCE=.\rowlayoutpl.cpp
# End Source File
# Begin Source File

SOURCE=.\toolwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\updatesmgr.cpp
# End Source File
# End Target
# End Project
