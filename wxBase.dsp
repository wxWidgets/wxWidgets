# Microsoft Developer Studio Project File - Name="wxBase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxBase - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxBase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxBase.mak" CFG="wxBase - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxBase - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxBase - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxBase - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BaseRelease"
# PROP BASE Intermediate_Dir "BaseRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "BaseRelease"
# PROP Intermediate_Dir "BaseRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MT" /YX /FD /c
# ADD CPP /nologo /MT /W4 /Zi /O2 /I "$(wx)\include" /I "$(wx)\src\zlib" /D "NDEBUG" /D wxUSE_GUI=0 /D WIN95=1 /D "__WIN95__" /D "WIN32" /D "_WIN32" /D WINVER=0x400 /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN32__" /D "_MT" /Yu"wx/wxprec.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wxBase - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "BaseDebug"
# PROP BASE Intermediate_Dir "BaseDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "BaseDebug"
# PROP Intermediate_Dir "BaseDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MT" /YX /FD /c
# ADD CPP /nologo /MTd /W4 /Zi /Od /I "$(wx)\include" /I "$(wx)\src\zlib" /D "_DEBUG" /D DEBUG=1 /D WXDEBUG=1 /D "__WXDEBUG__" /D wxUSE_GUI=0 /D "__WIN95__" /D "WIN32" /D "_WIN32" /D WINVER=0x400 /D "__WINDOWS__" /D "__WIN32__" /D "__WXMSW__" /D "_MT" /Fr /Yu"wx/wxprec.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"lib/wxBase.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "wxBase - Win32 Release"
# Name "wxBase - Win32 Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\msw\dummy.cpp
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


# Begin Source File

SOURCE=.\src\common\y_tab.c

!IF  "$(CFG)" == "wxBase - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "wxBase - Win32 Debug"

# ADD CPP /W1
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Target
# End Project
