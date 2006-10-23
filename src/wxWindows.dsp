# Microsoft Developer Studio Project File - Name="wxWindows" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxWindows - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxWindows.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxWindows.mak" CFG="wxWindows - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxWindows - Win32 Release Unicode DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxWindows - Win32 Debug Unicode DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxWindows - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxWindows - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "wxWindows - Win32 Release DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxWindows - Win32 Debug DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wxWindows - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxWindows - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxWindows - Win32 Release With Debug Info" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "wxWindows - Win32 Release Unicode DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../ReleaseUnicodeDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../ReleaseUnicodeDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WXWINDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/mswdllu" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /I "./expat/lib" /D "_USRDLL" /D "NDEBUG" /D "WXMAKINGDLL" /D "_UNICODE" /D "UNICODE" /D "WIN32" /D WINVER=0x0400 /D "STRICT" /D wxUSE_BASE=1 /Yu"wx/wxprec.h" /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "../include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\lib\jpeg.lib ..\lib\tiff.lib ..\lib\expat.lib ..\lib\png.lib ..\lib\regex.lib ..\lib\zlib.lib /nologo /dll /machine:I386 /out:"../lib/wxmsw250u.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib ..\lib\jpeg.lib ..\lib\tiff.lib ..\lib\expat.lib ..\lib\png.lib ..\lib\regex.lib ..\lib\zlib.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxmsw250u.dll"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug Unicode DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../DebugUnicodeDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../DebugUnicodeDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WXWINDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/mswdllud" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /I "./expat/lib" /D "_USRDLL" /D "_DEBUG" /D "WXMAKINGDLL" /D "_UNICODE" /D "UNICODE" /D "WIN32" /D WINVER=0x0400 /D "STRICT" /D wxUSE_BASE=1 /Yu"wx/wxprec.h" /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\lib\jpegd.lib ..\lib\tiffd.lib ..\lib\expatd.lib ..\lib\pngd.lib ..\lib\regexd.lib ..\lib\zlibd.lib /dll /debug /machine:I386 /out:"../lib/wxmsw250ud.dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib ..\lib\jpegd.lib ..\lib\tiffd.lib ..\lib\expatd.lib ..\lib\pngd.lib ..\lib\regexd.lib ..\lib\zlibd.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxmsw250ud.dll"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../ReleaseUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../ReleaseUnicode"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W4 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/mswu" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /I "./expat/lib" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "WIN32" /D WINVER=0x0400 /D "STRICT" /D wxUSE_BASE=1 /Yu"wx/wxprec.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxmswu.lib"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../DebugUnicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../DebugUnicode"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/mswud" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /I "./expat/lib" /D "_DEBUG" /D "__WXDEBUG__" /D "_UNICODE" /D "UNICODE" /D "WIN32" /D WINVER=0x0400 /D "STRICT" /D wxUSE_BASE=1 /Yu"wx/wxprec.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxmswud.lib"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../ReleaseDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WXWINDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/mswdll" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /I "./expat/lib" /D "_USRDLL" /D "NDEBUG" /D "WXMAKINGDLL" /D "WIN32" /D WINVER=0x0400 /D "STRICT" /D wxUSE_BASE=1 /Yu"wx/wxprec.h" /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "../include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\lib\jpeg.lib ..\lib\tiff.lib ..\lib\expat.lib ..\lib\png.lib ..\lib\regex.lib ..\lib\zlib.lib /nologo /dll /machine:I386 /out:"../lib/wxmsw250.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib ..\lib\jpeg.lib ..\lib\tiff.lib ..\lib\expat.lib ..\lib\png.lib ..\lib\regex.lib ..\lib\zlib.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxmsw250.dll"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../DebugDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WXWINDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/mswdlld" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /I "./expat/lib" /D "_USRDLL" /D "_DEBUG" /D "WXMAKINGDLL" /D "WIN32" /D WINVER=0x0400 /D "STRICT" /D wxUSE_BASE=1 /Yu"wx/wxprec.h" /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\lib\jpegd.lib ..\lib\tiffd.lib ..\lib\expatd.lib ..\lib\pngd.lib ..\lib\regexd.lib ..\lib\zlibd.lib /dll /debug /machine:I386 /out:"../lib/wxmsw250d.dll" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib ..\lib\jpegd.lib ..\lib\tiffd.lib ..\lib\expatd.lib ..\lib\pngd.lib ..\lib\regexd.lib ..\lib\zlibd.lib /nologo /version:2.5 /dll /machine:I386 /out:"../lib/wxmsw250d.dll"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../Release"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W4 /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /O2 /I "../lib/msw" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /I "./expat/lib" /D "NDEBUG" /D "WIN32" /D WINVER=0x0400 /D "STRICT" /D wxUSE_BASE=1 /Yu"wx/wxprec.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxmsw.lib"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../lib"
# PROP BASE Intermediate_Dir "../Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../Debug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W4 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /Zi /Od /I "../lib/mswd" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /I "./expat/lib" /D "_DEBUG" /D "__WXDEBUG__" /D "WIN32" /D WINVER=0x0400 /D "STRICT" /D wxUSE_BASE=1 /Yu"wx/wxprec.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\wxmswd.lib"

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release With Debug Info"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wxWindows___Win32_Release_With_Debug_Info"
# PROP BASE Intermediate_Dir "wxWindows___Win32_Release_With_Debug_Info"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDebug"
# PROP Intermediate_Dir "ReleaseDebug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W4 /O2 /I "../lib/msw" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /I "./expat/lib" /D "NDEBUG" /D "WIN32" /D WINVER=0x0400 /D "STRICT" /D wxUSE_BASE=1 /Yu"wx/wxprec.h" /FD /c
# ADD CPP /nologo /MD /W4 /Zi /O2 /I "../lib/msw" /I "../include" /I "./zlib" /I "./jpeg" /I "./png" /I "./regex" /I "./tiff" /I "./expat/lib" /D "NDEBUG" /D "WIN32" /D WINVER=0x0400 /D "STRICT" /D wxUSE_BASE=1 /Yu"wx/wxprec.h" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\wxmsw.lib"
# ADD LIB32 /nologo /out:"..\lib\wxmsw.lib"

!ENDIF 

# Begin Target

# Name "wxWindows - Win32 Release Unicode DLL"
# Name "wxWindows - Win32 Debug Unicode DLL"
# Name "wxWindows - Win32 Release Unicode"
# Name "wxWindows - Win32 Debug Unicode"
# Name "wxWindows - Win32 Release DLL"
# Name "wxWindows - Win32 Debug DLL"
# Name "wxWindows - Win32 Release"
# Name "wxWindows - Win32 Debug"
# Name "wxWindows - Win32 Release With Debug Info"
# Begin Source File

SOURCE="This project is deprecated, please see install.txt"
# End Source File
# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common\accesscmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\anidecod.cpp
# End Source File
# Begin Source File

SOURCE=.\common\animatecmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\appbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\appcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\archive.cpp
# End Source File
# Begin Source File

SOURCE=.\common\artprov.cpp
# End Source File
# Begin Source File

SOURCE=.\common\artstd.cpp
# End Source File
# Begin Source File

SOURCE=.\common\bmpbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\bookctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\common\choiccmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\clipcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\clntdata.cpp
# End Source File
# Begin Source File

SOURCE=.\common\clrpickercmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmdline.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmdproc.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cmndata.cpp
# End Source File
# Begin Source File

SOURCE=.\common\colourcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\combocmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\config.cpp
# End Source File
# Begin Source File

SOURCE=.\common\containr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\convauto.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cshelp.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ctrlsub.cpp
# End Source File
# Begin Source File

SOURCE=.\common\datacmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\datavcmn.cpp
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

SOURCE=.\common\dbgrid.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dbtable.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dcbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dcbufcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\debugrpt.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dircmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dndcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dobjcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\docmdi.cpp
# End Source File
# Begin Source File

SOURCE=.\common\docview.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dpycmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dseldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dummy.cpp
# ADD CPP /Yc"wx/wxprec.h"
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

SOURCE=.\common\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\common\encconv.cpp
# End Source File
# Begin Source File

SOURCE=.\common\event.cpp
# End Source File
# Begin Source File

SOURCE=.\common\evtloopcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\extended.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\common\fddlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ffile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\file.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fileback.cpp
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

SOURCE=.\common\filepickercmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\filesys.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fldlgcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fmapbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fontcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fontenumcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fontmap.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fontpickercmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\framecmn.cpp
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

SOURCE=.\common\gaugecmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\gbsizer.cpp
# End Source File
# Begin Source File

SOURCE=.\common\gdicmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\geometry.cpp
# End Source File
# Begin Source File

SOURCE=.\common\gifdecod.cpp
# End Source File
# Begin Source File

SOURCE=.\common\graphcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\hash.cpp
# End Source File
# Begin Source File

SOURCE=.\common\hashmap.cpp
# End Source File
# Begin Source File

SOURCE=.\common\helpbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\http.cpp
# End Source File
# Begin Source File

SOURCE=.\common\iconbndl.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagall.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagbmp.cpp
# End Source File
# Begin Source File

SOURCE=.\common\image.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagfill.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imaggif.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagiff.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagjpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagpcx.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagpng.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagpnm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagtiff.cpp
# End Source File
# Begin Source File

SOURCE=.\common\imagxpm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\init.cpp
# End Source File
# Begin Source File

SOURCE=.\common\intl.cpp
# End Source File
# Begin Source File

SOURCE=.\common\ipcbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\layout.cpp
# End Source File
# Begin Source File

SOURCE=.\common\lboxcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\list.cpp
# End Source File
# Begin Source File

SOURCE=.\common\listctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\log.cpp
# End Source File
# Begin Source File

SOURCE=.\common\longlong.cpp
# End Source File
# Begin Source File

SOURCE=.\common\matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mediactrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\memory.cpp
# End Source File
# Begin Source File

SOURCE=.\common\menucmn.cpp
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

SOURCE=.\common\nbkbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\object.cpp
# End Source File
# Begin Source File

SOURCE=.\common\paper.cpp
# End Source File
# Begin Source File

SOURCE=.\common\pickerbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\platinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\common\popupcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\powercmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\prntbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\process.cpp
# End Source File
# Begin Source File

SOURCE=.\common\protocol.cpp
# End Source File
# Begin Source File

SOURCE=.\common\quantize.cpp
# End Source File
# Begin Source File

SOURCE=.\common\radiocmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\regex.cpp
# End Source File
# Begin Source File

SOURCE=.\common\rendcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\rgncmn.cpp
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

SOURCE=.\common\settcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sizer.cpp
# End Source File
# Begin Source File

SOURCE=.\common\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sstream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\statbar.cpp
# End Source File
# Begin Source File

SOURCE=.\common\stdpbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\stockitem.cpp
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

SOURCE=.\common\taskbarcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\tbarbase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\textfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\timercmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\tokenzr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\toplvcmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\treebase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\txtstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\uri.cpp
# End Source File
# Begin Source File

SOURCE=.\common\url.cpp
# End Source File
# Begin Source File

SOURCE=.\common\utilscmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\valgen.cpp
# End Source File
# Begin Source File

SOURCE=.\common\validate.cpp
# End Source File
# Begin Source File

SOURCE=.\common\valtext.cpp
# End Source File
# Begin Source File

SOURCE=.\common\variant.cpp
# End Source File
# Begin Source File

SOURCE=.\common\wfstream.cpp
# End Source File
# Begin Source File

SOURCE=.\common\wincmn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\wxchar.cpp
# End Source File
# Begin Source File

SOURCE=.\common\xpmdecod.cpp
# End Source File
# Begin Source File

SOURCE=.\common\xti.cpp
# End Source File
# Begin Source File

SOURCE=.\common\xtistrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\xtixml.cpp
# End Source File
# Begin Source File

SOURCE=.\common\zipstrm.cpp
# End Source File
# Begin Source File

SOURCE=.\common\zstream.cpp
# End Source File
# End Group
# Begin Group "Generic Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\generic\aboutdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\animateg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\bmpcboxg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\busyinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\buttonbar.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\calctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\choicbkg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\choicdgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\clrpickerg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\collpaneg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\combog.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\datavgen.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\datectlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\dbgrptg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\dcpsg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\dirctrlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\dragimgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\filepickerg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\fontpickerg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\grid.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\gridctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\gridsel.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\helpext.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\htmllbox.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\laywin.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\listbkg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\logg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\numdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\odcombo.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\panelg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\printps.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\prntdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\progdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\propdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\renderg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\sashwin.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\scrlwing.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\selstore.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\spinctlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\splash.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\statusbr.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\textdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\tipdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\tipwin.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\toolbkg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\treebkg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\treectlg.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\vlbox.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\vscroll.cpp
# End Source File
# Begin Source File

SOURCE=.\generic\wizard.cpp
# End Source File
# End Group
# Begin Group "wxHTML Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\html\helpctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\html\helpdata.cpp
# End Source File
# Begin Source File

SOURCE=.\html\helpdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\html\helpfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\html\helpwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmlcell.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmlfilt.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmlpars.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmltag.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmlwin.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmprint.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_dflist.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_fonts.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_hline.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_image.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_layout.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_links.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_list.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_pre.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_style.cpp
# End Source File
# Begin Source File

SOURCE=.\html\m_tables.cpp
# End Source File
# Begin Source File

SOURCE=.\html\winpars.cpp
# End Source File
# End Group
# Begin Group "MSW Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\msw\aboutdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\accel.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\app.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\basemsw.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\bmpbuttn.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\brush.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\button.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\caret.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\checkbox.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\checklst.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\choice.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\clipbrd.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\colordlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\colour.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\combo.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\combobox.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\control.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\crashrpt.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\data.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\datectrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dc.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dcclient.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dcmemory.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dcprint.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dcscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dde.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\debughlp.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dialup.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dib.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dir.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dirdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\display.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dlmsw.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\dragimag.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\enhmeta.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\evtloop.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\fdrepdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\filedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\font.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\fontdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\fontenum.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\fontutil.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\frame.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\gauge95.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\gdiimage.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\gdiobj.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\glcanvas.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\graphics.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\gsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\gsockmsw.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\helpbest.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\helpchm.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\helpwin.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\icon.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\imaglist.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\iniconf.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\joystick.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\listctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\main.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\mdi.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\mediactrl_am.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\mediactrl_wmp10.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\menuitem.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\metafile.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\mimetype.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\minifram.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\msgdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\mslu.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\nativdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\notebook.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\access.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\activex.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\automtn.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\dataobj.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\dropsrc.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\droptgt.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\oleutils.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ole\uuid.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\ownerdrw.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\palette.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\pen.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\popupwin.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\power.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\printdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\printwin.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\radiobox.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\radiobut.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\regconf.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\region.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\scrolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\settings.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\slider95.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\snglinst.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\spinbutt.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\spinctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\stackwalk.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\statbmp.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\statbox.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\statbr95.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\statline.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\stattext.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\stdpaths.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\tabctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\taskbar.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\tbar95.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\textctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\tglbtn.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\thread.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\tooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\toplevel.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\treectrl.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\urlmsw.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\utilsexc.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\utilsgui.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\uxtheme.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\volume.cpp
# End Source File
# Begin Source File

SOURCE=.\msw\window.cpp
# End Source File
# End Group
# Begin Group "Other Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aui\auibook.cpp
# End Source File
# Begin Source File

SOURCE=.\aui\dockart.cpp
# End Source File
# Begin Source File

SOURCE=.\aui\floatpane.cpp
# End Source File
# Begin Source File

SOURCE=.\aui\framemanager.cpp
# End Source File
# Begin Source File

SOURCE=.\aui\tabmdi.cpp
# End Source File
# Begin Source File

SOURCE=.\richtext\richtextbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\richtext\richtextctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\richtext\richtextformatdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\richtext\richtexthtml.cpp
# End Source File
# Begin Source File

SOURCE=.\richtext\richtextstyledlg.cpp
# End Source File
# Begin Source File

SOURCE=.\richtext\richtextstyles.cpp
# End Source File
# Begin Source File

SOURCE=.\richtext\richtextsymboldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\richtext\richtextxml.cpp
# End Source File
# Begin Source File

SOURCE=.\xml\xml.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_animatctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_bmp.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_bmpbt.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_bmpcbox.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_bttn.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_cald.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_chckb.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_chckl.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_choic.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_choicbk.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_clrpicker.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_combo.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_datectrl.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_dirpicker.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_filepicker.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_fontpicker.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_frame.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_gauge.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_gdctl.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_grid.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_html.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_htmllbox.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_listb.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_listbk.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_listc.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_mdi.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_menu.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_notbk.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_odcombo.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_panel.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_radbt.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_radbx.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_scrol.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_scwin.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_sizer.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_slidr.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_spin.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_split.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_statbar.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_stbmp.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_stbox.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_stlin.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_sttxt.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_text.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_tglbtn.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_toolb.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_tree.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_treebk.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_unkwn.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xh_wizrd.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xmlres.cpp
# End Source File
# Begin Source File

SOURCE=.\xrc\xmlrsall.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Group "Setup"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\msw\setup.h

!IF  "$(CFG)" == "wxWindows - Win32 Release Unicode DLL"

# Begin Custom Build - Creating ..\lib\mswdllu\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswdllu/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswdllu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug Unicode DLL"

# Begin Custom Build - Creating ..\lib\mswdllud\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswdllud/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswdllud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release Unicode"

# Begin Custom Build - Creating ..\lib\mswu\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswu/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug Unicode"

# Begin Custom Build - Creating ..\lib\mswud\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswud/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release DLL"

# Begin Custom Build - Creating ..\lib\mswdll\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswdll/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswdll\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug DLL"

# Begin Custom Build - Creating ..\lib\mswdlld\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswdlld/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswdlld\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release"

# Begin Custom Build - Creating ..\lib\msw\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/msw/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\msw\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Debug"

# Begin Custom Build - Creating ..\lib\mswd\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/mswd/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\mswd\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxWindows - Win32 Release With Debug Info"

# Begin Custom Build - Creating ..\lib\msw\wx\setup.h from $(InputPath)
InputPath=..\include\wx\msw\setup.h

"../lib/msw/wx/setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\lib\msw\wx\setup.h

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\aboutdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\accel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\access.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\afterstd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\anidecod.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\animate.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\animdecod.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\app.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\apptrait.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\archive.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\arrstr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\artprov.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\aui\aui.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\aui\auibook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\aui\dockart.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\aui\floatpane.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\aui\framemanager.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\aui\tabmdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\beforestd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\bookctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\brush.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\buffer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\build.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\busyinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\button.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\calctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\caret.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\checklst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\chkconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\choicdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\choice.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\choicebk.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\clipbrd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\clntdata.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\clrpicker.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cmdline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cmdproc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cmndata.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\collpane.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\colordlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\colour.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\combo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\combobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\confbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\config.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\containr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\control.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\convauto.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cpp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cshelp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ctrlsub.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cursor.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dataobj.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dataview.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dataview.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\datectrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dateevt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\datetime.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\datstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\db.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbgrid.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbkeyg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbtable.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcbuffer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcclient.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcgraph.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcmemory.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcmirror.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcprint.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcps.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dcscreen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dde.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\debug.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\debugrpt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\defs.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dialog.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dialup.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dir.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dirctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dirdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\display.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\display_impl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dlimpexp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dnd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\docmdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\docview.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dragimag.h
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

SOURCE=..\include\wx\effects.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\encconv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\encinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\event.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\evtloop.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\except.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fdrepdlg.h
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

SOURCE=..\include\wx\fileback.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fileconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filefn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filename.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filepicker.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filesys.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fmappriv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\font.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontenc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontenum.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontpicker.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontutil.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\frame.h
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

SOURCE=..\include\wx\gauge.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gbsizer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gdicmn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gdiobj.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\geometry.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gifdecod.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\glcanvas.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\graphics.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\grid.h
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

SOURCE=..\include\wx\hashset.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\help.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\helpbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\helphtml.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\helpwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\htmllbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\icon.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\iconbndl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\iconloc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\image.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imaggif.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagiff.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagjpeg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imaglist.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagpcx.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagpng.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagpnm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagtiff.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\imagxpm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\init.h
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

SOURCE=..\include\wx\joystick.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\layout.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\laywin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\link.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\list.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\listbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\listbook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\listbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\listctrl.h
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

SOURCE=..\include\wx\matrix.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mediactrl.h
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

SOURCE=..\include\wx\menu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\metafile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\minifram.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\module.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msgdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msgout.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mstream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\notebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\numdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\object.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\odcombo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ownerdrw.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\palette.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\panel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\paper.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\pen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\pickerbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\platform.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\platinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\popupwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\power.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\print.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\printdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\prntbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\process.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\progdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\propdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\protocol\file.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\protocol\ftp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\protocol\http.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\protocol\protocol.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ptr_scpd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ptr_scpd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\quantize.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\rawbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\recguard.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\regex.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\region.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\renderer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\richtext\richtextbuffer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\richtext\richtextctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\richtext\richtextformatdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\richtext\richtextstyledlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\richtext\richtextstyles.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\richtext\richtextsymboldlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\richtext\richtextxml.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sashwin.h
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

SOURCE=..\include\wx\scopeguard.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\scopeguard.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\scrolwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\selstore.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\settings.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sizer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\slider.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\snglinst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\socket.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sound.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\spinbutt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\spinctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\splash.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\splitter.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sstream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stack.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stackwalk.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stattext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\statusbr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stdpaths.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stockitem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stopwatch.h
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

SOURCE=..\include\wx\tab.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tabctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\taskbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tbarbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textbuf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textfile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\thread.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\timer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tipdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tipwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tokenzr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\toolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\toolbook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tooltip.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\toplevel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\treebase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\treebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\treectrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\txtstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\types.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\uri.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\url.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\utils.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\valgen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\validate.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\valtext.h
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

SOURCE=..\include\wx\vidmode.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\vlbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\vms_x_fix.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\volume.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\vscroll.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wave.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wfstream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\window.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wizard.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wupdlock.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wx.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxchar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxhtml.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxprec.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xml\xml.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xpmdecod.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xpmhand.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_all.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_animatctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_bmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_bmpbt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_bttn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_cald.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_chckb.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_chckl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_choic.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_choicbk.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_clrpicker.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_combo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_datectrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_dirpicker.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_dlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_filepicker.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_fontpicker.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_frame.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_gauge.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_gdctl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_grid.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_html.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_htmllbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_listb.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_listbk.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_listc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_mdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_menu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_notbk.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_odcombo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_panel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_radbt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_radbx.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_scrol.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_scwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_sizer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_slidr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_spin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_split.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_statbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_stbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_stbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_stlin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_sttxt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_text.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_toolb.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_tree.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_treebk.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_unkwn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xh_wizrd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xrc\xmlres.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xti.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xtistrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\xtixml.h
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

SOURCE=..\include\wx\msw\accel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\app.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\apptbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\apptrait.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\brush.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\button.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\caret.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\checklst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\chkconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\choice.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\clipbrd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\colordlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\colour.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\combo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\combobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\control.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\crashrpt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\cursor.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\datectrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcclient.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcmemory.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcprint.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dcscreen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dde.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\debughlp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dialog.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dib.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dirdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dragimag.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\enhmeta.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\evtloop.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\fdrepdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\font.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\fontdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\frame.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gauge95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gccpriv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gdiimage.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\glcanvas.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gsockmsw.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\helpbest.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\helpchm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\helpwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\htmlhelp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\icon.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\imaglist.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\iniconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\joystick.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\libraries.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\listbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\listctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\mdi.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\menu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\metafile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\minifram.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\missing.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\msgdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\mslu.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\msvcrt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\notebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\access.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\activex.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\automtn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\dataform.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\dataobj.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\dataobj2.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\dropsrc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\droptgt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\oleutils.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\ole\uuid.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\palette.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\pen.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\popupwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\printdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\printwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\private.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\rcdefs.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\regconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\region.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\registry.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\seh.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\setup0.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\slider95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\sound.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\spinbutt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\spinctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\stackwalk.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statbr95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\statline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\stattext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\stdpaths.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tabctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\taskbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tbar95.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\timer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\tooltip.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\toplevel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\treectrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\uxtheme.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\uxthemep.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\window.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\winundef.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\wrapcctl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\wrapcdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\wrapwin.h
# End Source File
# End Group
# Begin Group "Generic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\generic\aboutdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\accel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\animate.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\busyinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\buttonbar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\calctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\choicdgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\clrpickerg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\clrpickerg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\colpaneg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\colrdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\combo.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dataview.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\datectrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dcpsg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dirctrlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\dragimgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\filedlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\filepickerg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\filepickerg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\fontdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\fontpickerg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\fontpickerg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\grid.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\gridctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\gridsel.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\helpext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\laywin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\logg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\msgdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\notebook.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\numdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\panelg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\printps.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\prntdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\progdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\propdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\sashwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\scrolwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\spinctlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\splash.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\splitter.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\textdlgg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\timer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\treectlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\generic\wizard.h
# End Source File
# End Group
# Begin Group "HTML"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\html\forcelnk.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpctrl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpdata.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpdlg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpfrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\helpwnd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlcell.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmldefs.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlfilt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlpars.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlproc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmltag.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmlwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\htmprint.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\m_templ.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\html\winpars.h
# End Source File
# End Group
# End Group
# End Target
# End Project
