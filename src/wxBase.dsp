# Microsoft Developer Studio Project File - Name="wxBase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

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
!MESSAGE "wxBase - Win32 Release Unicode DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxBase - Win32 Debug Unicode DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxBase - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxBase - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxBase - Win32 Release DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxBase - Win32 Debug DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxBase - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxBase - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxBase - Win32 Release Unicode DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../BaseReleaseUnicodeDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../BaseReleaseUnicodeDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "WXBASEDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/basedllu" /I "../include" /I "./regex" /I "./zlib" /D "NDEBUG" /D wxUSE_GUI=0 /D "WIN32" /D WINVER=0x400 /D "WXMAKINGDLL" /D "_UNICODE" /D "UNICODE" /Yu"wx/wxprec.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "../include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../lib/wxbase250u.dll"
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib ../lib/zlib.lib ../lib/regex.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxbase250u.dll"

!ELSEIF  "$(CFG)" == "wxBase - Win32 Debug Unicode DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../BaseDebugUnicodeDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../BaseDebugUnicodeDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "WXBASEDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/basedllud" /I "../include" /I "./regex" /I "./zlib" /D "_DEBUG" /D wxUSE_GUI=0 /D "WIN32" /D WINVER=0x400 /D "WXMAKINGDLL" /D "_UNICODE" /D "UNICODE" /Yu"wx/wxprec.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /out:"../lib/wxbase250ud.dll"
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib ../lib/zlibd.lib ../lib/regexd.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxbase250ud.dll"
!ELSEIF  "$(CFG)" == "wxBase - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "..\BaseReleaseUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "..\BaseReleaseUnicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/baseu" /I "../include" /I "./regex" /I "./zlib" /D "NDEBUG" /D wxUSE_GUI=0 /D "WIN32" /D WINVER=0x400 /D "_UNICODE" /D "UNICODE" /Yu"wx/wxprec.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxbaseu.lib"

!ELSEIF  "$(CFG)" == "wxBase - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "..\BaseDebugUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "..\BaseDebugUnicode"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/baseud" /I "../include" /I "./regex" /I "./zlib" /D "_DEBUG" /D wxUSE_GUI=0 /D "WIN32" /D "__WXDEBUG__" /D WINVER=0x400 /D "_UNICODE" /D "UNICODE" /Yu"wx/wxprec.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"../lib/wxbase.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxbaseud.lib"

!ELSEIF  "$(CFG)" == "wxBase - Win32 Release DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../BaseReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../BaseReleaseDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "WXBASEDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/basedll" /I "../include" /I "./regex" /I "./zlib" /D "NDEBUG" /D wxUSE_GUI=0 /D "WIN32" /D WINVER=0x400 /D "WXMAKINGDLL" /Yu"wx/wxprec.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "../include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../lib/wxbase250.dll"
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib ../lib/zlib.lib ../lib/regex.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxbase250.dll"

!ELSEIF  "$(CFG)" == "wxBase - Win32 Debug DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../BaseDebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../BaseDebugDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "WXBASEDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/basedlld" /I "../include" /I "./regex" /I "./zlib" /D "_DEBUG" /D wxUSE_GUI=0 /D "WIN32" /D WINVER=0x400 /D "WXMAKINGDLL" /Yu"wx/wxprec.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /out:"../lib/wxbase250d.dll"
# ADD LINK32 kernel32.lib user32.lib advapi32.lib shell32.lib wsock32.lib ../lib/zlibd.lib ../lib/regexd.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxbase250d.dll"

!ELSEIF  "$(CFG)" == "wxBase - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "..\BaseRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "..\BaseRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/base" /I "../include" /I "./regex" /I "./zlib" /D "NDEBUG" /D wxUSE_GUI=0 /D "WIN32" /D WINVER=0x400 /Yu"wx/wxprec.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxbase.lib"

!ELSEIF  "$(CFG)" == "wxBase - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib"
# PROP BASE Intermediate_Dir "..\BaseDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "..\BaseDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/based" /I "../include" /I "./regex" /I "./zlib" /D "_DEBUG" /D wxUSE_GUI=0 /D "WIN32" /D "__WXDEBUG__" /D WINVER=0x400 /Yu"wx/wxprec.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"../lib/wxbase.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxbased.lib"

!ENDIF 

# Begin Target

# Name "wxBase - Win32 Release Unicode DLL"
# Name "wxBase - Win32 Debug Unicode DLL"
# Name "wxBase - Win32 Release Unicode"
# Name "wxBase - Win32 Debug Unicode"
# Name "wxBase - Win32 Release DLL"
# Name "wxBase - Win32 Debug DLL"
# Name "wxBase - Win32 Release"
# Name "wxBase - Win32 Debug"

# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common\appbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\clntdata.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmdline.cpp
# End Source File
# Begin Source File

SOURCE=.\common\config.cpp
# End Source File
# Begin Source File

SOURCE=.\common\datetime.cpp
# End Source File
# Begin Source File

SOURCE=.\common\datstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\db.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dbtable.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dircmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dynarray.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dynlib.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dynload.cpp
# End Source File
# Begin Source File

SOURCE=.\common\encconv.cpp
# End Source File
# Begin Source File

SOURCE=.\common\event.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ffile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\file.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fileconf.cpp
# End Source File
# Begin Source File

SOURCE=.\common\filefn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\filename.cpp
# End Source File
# Begin Source File

SOURCE=.\common\filesys.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fmapbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fs_inet.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fs_mem.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fs_zip.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ftp.cpp
# End Source File
# Begin Source File

SOURCE=.\common\hash.cpp
# End Source File
# Begin Source File

SOURCE=.\common\hashmap.cpp
# End Source File
# Begin Source File

SOURCE=.\common\http.cpp
# End Source File
# Begin Source File

SOURCE=.\common\intl.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ipcbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\list.cpp
# End Source File
# Begin Source File

SOURCE=.\common\log.cpp
# End Source File
# Begin Source File

SOURCE=.\common\longlong.cpp
# End Source File
# Begin Source File

SOURCE=.\common\memory.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mimecmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\module.cpp
# End Source File
# Begin Source File

SOURCE=.\common\msgout.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mstream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\object.cpp
# End Source File
# Begin Source File

SOURCE=.\common\process.cpp
# End Source File
# Begin Source File

SOURCE=.\common\protocol.cpp
# End Source File
# Begin Source File

SOURCE=.\common\regex.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sckaddr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sckfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sckipc.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sckstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\common\stopwatch.cpp
# End Source File
# Begin Source File

SOURCE=.\common\strconv.cpp
# End Source File
# Begin Source File

SOURCE=.\common\stream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\string.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sysopt.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\tokenzr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\txtstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\url.cpp
# End Source File
# Begin Source File

SOURCE=.\common\utilscmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\variant.cpp
# End Source File
# Begin Source File

SOURCE=.\common\wfstream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\wxchar.cpp
# End Source File
# Begin Source File

SOURCE=.\common\zipstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\zstream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\init.cpp
# End Source File

# Begin Source File

SOURCE=.\common\extended.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\common\unzip.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File

# End Group
# Begin Group "MSW Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\msw\dummy.cpp
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=.\msw\basemsw.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dde.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dir.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\main.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\mimetype.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\mslu.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\regconf.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\snglinst.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\thread.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\utilsexc.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\volume.cpp
# End Source File

# Begin Source File

SOURCE=.\msw\gsocket.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\msw\gsockmsw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File

# End Group
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Group "Setup"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\msw\setup.h
!IF  "$(CFG)" == "wxBase - Win32 Release Unicode DLL"
# Begin Custom Build - Copying $(InputPath) to $(TargetDir)
InputPath=..\include\wx\msw\setup.h

"../lib/basedllu/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\basedllu\wx\setup.h

# End Custom Build
!ELSEIF  "$(CFG)" == "wxBase - Win32 Debug Unicode DLL"
# Begin Custom Build - Copying $(InputPath) to $(TargetDir)
InputPath=..\include\wx\msw\setup.h

"../lib/basedllud/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\basedllud\wx\setup.h
# End Custom Build
!ELSEIF  "$(CFG)" == "wxBase - Win32 Release Unicode"
# Begin Custom Build - Copying $(InputPath) to $(TargetDir)
InputPath=..\include\wx\msw\setup.h

"../lib/baseu/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\baseu\wx\setup.h

# End Custom Build
!ELSEIF  "$(CFG)" == "wxBase - Win32 Debug Unicode"
# Begin Custom Build - Copying $(InputPath) to $(TargetDir)
InputPath=..\include\wx\msw\setup.h

"../lib/baseud/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\baseud\wx\setup.h

# End Custom Build
!ELSEIF  "$(CFG)" == "wxBase - Win32 Release DLL"
# Begin Custom Build - Copying $(InputPath) to $(TargetDir)
InputPath=..\include\wx\msw\setup.h

"../lib/basedll/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\basedll\wx\setup.h

# End Custom Build
!ELSEIF  "$(CFG)" == "wxBase - Win32 Debug DLL"
# Begin Custom Build - Copying $(InputPath) to $(TargetDir)
InputPath=..\include\wx\msw\setup.h

"../lib/basedlld/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\basedlld\wx\setup.h
# End Custom Build
!ELSEIF  "$(CFG)" == "wxBase - Win32 Release"
# Begin Custom Build - Copying $(InputPath) to $(TargetDir)
InputPath=..\include\wx\msw\setup.h

"../lib/base/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\base\wx\setup.h

# End Custom Build
!ELSEIF  "$(CFG)" == "wxBase - Win32 Debug"
# Begin Custom Build - Copying $(InputPath) to $(TargetDir)
InputPath=..\include\wx\msw\setup.h

"../lib/based/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\based\wx\setup.h

# End Custom Build
!ENDIF 

# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\app.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\apptrait.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\buffer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\build.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\chkconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\clntdata.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cmdline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\confbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\config.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\datetime.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\datetime.inl
# End Source File
# Begin Source File

SOURCE=..\include\wx\datstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\db.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbkeyg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbtable.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dde.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\debug.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\defs.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dir.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dynarray.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dynlib.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dynload.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\encconv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\event.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\features.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ffile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\file.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fileconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filefn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filename.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filesys.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fmappriv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontenc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fs_inet.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fs_mem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fs_zip.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gsocket.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\hash.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\hashmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\intl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\iosfwrap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ioswrap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ipc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ipcbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\isql.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\isqlext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\list.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\log.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\longlong.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\math.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\memconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\memory.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\memtext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\module.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msgout.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mstream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\object.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\platform.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\process.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\regex.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sckaddr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sckipc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sckstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\snglinst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\socket.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\strconv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\string.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sysopt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textbuf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textfile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\thread.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\timer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tokenzr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\txtstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\url.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\utils.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\variant.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\vector.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\version.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\volume.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wfstream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wx.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxchar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxprec.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\zipstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\zstream.h
# End Source File

# End Group
# Begin Group "MSW"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\msw\apptbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\apptrait.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dde.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gccpriv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\winundef.h
# End Source File

# End Group
# End Group
# End Target
# End Project
