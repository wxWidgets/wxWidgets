# Microsoft Developer Studio Project File - Name="wxBaseDll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=wxBaseDll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxBaseDll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxBaseDll.mak" CFG="wxBaseDll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxBaseDll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxBaseDll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxBaseDll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BaseDllRelease"
# PROP Intermediate_Dir "BaseDllRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WXBASEDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W4 /O1 /I "$(wx)\include" /I "$(wx)\src\zlib" /D "NDEBUG" /D wxUSE_GUI=0 /D "__WIN95__" /D "WIN32" /D "_WIN32" /D WINVER=0x400 /D "__WINDOWS__" /D "__WIN32__" /D "__WXMSW__" /D "_MT" /D "WXMAKINGDLL" /Yu"wx/wxprec.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(wx)\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"BaseDllRelease/wxbase221.dll"
# ADD LINK32 kernel32.lib user32.lib advapi32.lib wsock32.lib /nologo /version:2.2 /dll /machine:I386 /out:"BaseDllRelease/wxbase221.dll"

!ELSEIF  "$(CFG)" == "wxBaseDll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BaseDllDebug"
# PROP Intermediate_Dir "BaseDllDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WXBASEDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /Zi /Od /I "$(wx)\include" /I "$(wx)\src\zlib" /D "_DEBUG" /D wxUSE_GUI=0 /D "__WIN95__" /D "WIN32" /D "_WIN32" /D WINVER=0x400 /D "__WINDOWS__" /D "__WIN32__" /D "__WXMSW__" /D "_MT" /D "WXMAKINGDLL" /D "__WXDEBUG__" /Yu"wx/wxprec.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "$(wx)\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /out:"BaseDllDebug/wxbase221d.dll"
# ADD LINK32 kernel32.lib user32.lib advapi32.lib wsock32.lib /nologo /version:2.2 /dll /debug /machine:I386 /pdbtype:sept /out:"BaseDllDebug/wxbase221d.dll"

!ENDIF 

# Begin Target

# Name "wxBaseDll - Win32 Release"
# Name "wxBaseDll - Win32 Debug"
# Begin Source File

SOURCE=.\src\common\base.rc
# End Source File
# Begin Source File

SOURCE=.\src\msw\dummydll.cpp
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=.\src\common\appcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\cmdline.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\config.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\datetime.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\datstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\dynarray.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\dynlib.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\encconv.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\event.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\ffile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\file.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\fileconf.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\filefn.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\filesys.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\fontmap.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\fs_inet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\fs_mem.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\fs_zip.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\ftp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\hash.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\http.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\intl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\ipcbase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\list.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\log.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\longlong.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\mimecmn.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\module.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\mstream.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\object.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\objstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\process.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\protocol.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\sckaddr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\sckfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\sckipc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\sckstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\serbase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\strconv.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\stream.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\string.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\textfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\timercmn.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\tokenzr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\txtstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\url.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\utilscmn.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\variant.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\wfstream.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\wxchar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\zipstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\zstream.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\init.cpp
# End Source File

# Begin Source File

SOURCE=.\src\msw\dde.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\dir.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\mimetype.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\regconf.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\thread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\utilsexc.cpp
# End Source File

# Begin Source File

SOURCE=.\src\common\extended.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\common\unzip.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File

# Begin Source File

SOURCE=.\src\msw\gsocket.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\src\msw\gsockmsw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File

# End Target
# End Project
