# Microsoft Developer Studio Project File - Name="wxconfigtool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=wxconfigtool - Win32 DebugDev
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxconfigtool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxconfigtool.mak" CFG="wxconfigtool - Win32 DebugDev"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxconfigtool - Win32 Debug DLL" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 Release DLL" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 DebugStable" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 ReleaseStable" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 DebugDev" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 ReleaseDev" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxconfigtool - Win32 Debug DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDLL"
# PROP BASE Intermediate_Dir "DebugDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLL"
# PROP Intermediate_Dir "DebugDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(WXWIN)/include" /I "$(WXWIN)/lib/mswdlld" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXUSINGDLL=1 /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxvc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wininet.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /pdbtype:sept /libpath:"$(WXWIN)\lib"

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 Release DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDLL"
# PROP BASE Intermediate_Dir "ReleaseDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDLL"
# PROP Intermediate_Dir "ReleaseDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "$(WXWIN)/include" /I "$(WXWIN)/lib/mswdll" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXUSINGDLL=1 /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wxvc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wininet.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /libpath:"$(WXWIN)\lib"

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DebugStable"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wxconfigtool___Win32_DebugStable"
# PROP BASE Intermediate_Dir "wxconfigtool___Win32_DebugStable"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugStable"
# PROP Intermediate_Dir "DebugStable"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(WXWIN)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /Yu"wx/wxprec.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(WXWINSTABLE)/include" /I "$(WXWINSTABLE)/lib/mswd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxvc.lib jpeg.lib tiff.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(WXWIN)\src\Debug" /libpath:"$(WXWIN)\src\jpeg\Debug" /libpath:"$(WXWIN)\src\tiff\Debug" /libpath:"$(WXWIN)\lib"
# ADD LINK32 winmm.lib wxmswd.lib jpegd.lib tiffd.lib pngd.lib zlibd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wininet.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(WXWINSTABLE)\lib"

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 ReleaseStable"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wxconfigtool___Win32_ReleaseStable"
# PROP BASE Intermediate_Dir "wxconfigtool___Win32_ReleaseStable"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseStable"
# PROP Intermediate_Dir "ReleaseStable"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "$(WXWIN)\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(WXWINSTABLE)/include" /I "$(WXWINSTABLE)/lib/msw" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxvc.lib jpeg.lib tiff.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"$(WXWIN)\src\Release" /libpath:"$(WXWIN)\src\jpeg\Release" /libpath:"$(WXWIN)\src\tiff\Release" /libpath:"$(WXWIN)\lib"
# ADD LINK32 winmm.lib wxmsw.lib jpeg.lib tiff.lib png.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wininet.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"$(WXWINSTABLE)\lib"

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DebugDev"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "wxconfigtool___Win32_DebugDev"
# PROP BASE Intermediate_Dir "wxconfigtool___Win32_DebugDev"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDev"
# PROP Intermediate_Dir "DebugDev"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(WXWINDEV)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(WXWINDEV)/include" /I "$(WXWINDEV)/lib/mswd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxd.lib jpegd.lib tiffd.lib pngd.lib zlibd.lib xpmd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(WXWINDEV)\lib"
# ADD LINK32 winmm.lib wxmswd.lib jpegd.lib tiffd.lib pngd.lib zlibd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wininet.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(WXWIN)\lib"

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 ReleaseDev"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "wxconfigtool___Win32_ReleaseDev"
# PROP BASE Intermediate_Dir "wxconfigtool___Win32_ReleaseDev"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDev"
# PROP Intermediate_Dir "ReleaseDev"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "$(WXWINDEV)\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "$(WXWINDEV)/include" /I "$(WXWINDEV)/lib/mswd" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wx.lib jpeg.lib tiff.lib png.lib xpm.lib zlib.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"$(WXWINDEV)\lib"
# ADD LINK32 winmm.lib wxmsw.lib jpeg.lib tiff.lib png.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wininet.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"$(WXWIN)\lib"

!ENDIF 

# Begin Target

# Name "wxconfigtool - Win32 Debug DLL"
# Name "wxconfigtool - Win32 Release DLL"
# Name "wxconfigtool - Win32 DebugStable"
# Name "wxconfigtool - Win32 ReleaseStable"
# Name "wxconfigtool - Win32 DebugDev"
# Name "wxconfigtool - Win32 ReleaseDev"
# Begin Group "Source files"

# PROP Default_Filter ""
# Begin Group "Header files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\appsettings.h
# End Source File
# Begin Source File

SOURCE=.\configitem.h
# End Source File
# Begin Source File

SOURCE=.\configitemselector.h
# End Source File
# Begin Source File

SOURCE=.\configtooldoc.h
# End Source File
# Begin Source File

SOURCE=.\configtoolview.h
# End Source File
# Begin Source File

SOURCE=.\configtree.h
# End Source File
# Begin Source File

SOURCE=.\custompropertydialog.h
# End Source File
# Begin Source File

SOURCE=.\htmlparser.h
# End Source File
# Begin Source File

SOURCE=.\mainframe.h
# End Source File
# Begin Source File

SOURCE=.\propeditor.h
# End Source File
# Begin Source File

SOURCE=.\property.h
# End Source File
# Begin Source File

SOURCE=.\settingsdialog.h
# End Source File
# Begin Source File

SOURCE=.\symbols.h
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# Begin Source File

SOURCE=.\wxconfigtool.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\appsettings.cpp
# End Source File
# Begin Source File

SOURCE=.\configitem.cpp
# End Source File
# Begin Source File

SOURCE=.\configitemselector.cpp
# End Source File
# Begin Source File

SOURCE=.\configtooldoc.cpp
# End Source File
# Begin Source File

SOURCE=.\configtoolview.cpp
# End Source File
# Begin Source File

SOURCE=.\configtree.cpp
# End Source File
# Begin Source File

SOURCE=.\custompropertydialog.cpp
# End Source File
# Begin Source File

SOURCE=.\htmlparser.cpp
# End Source File
# Begin Source File

SOURCE=.\mainframe.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\makefile.b32
# End Source File
# Begin Source File

SOURCE=.\propeditor.cpp
# End Source File
# Begin Source File

SOURCE=.\property.cpp
# End Source File
# Begin Source File

SOURCE=.\settingsdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\wxconfigtool.cpp
# SUBTRACT CPP /YX /Yc
# End Source File
# Begin Source File

SOURCE=.\wxconfigtool.rc

!IF  "$(CFG)" == "wxconfigtool - Win32 Debug DLL"

# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /i "$(WXWIN)\include"

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 Release DLL"

# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /i "$(WXWIN)\include"

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DebugStable"

# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /i "$(WXWINSTABLE)\include" /d "wxUSE_NO_MANIFEST"

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 ReleaseStable"

# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /i "$(WXWINSTABLE)\include" /d "wxUSE_NO_MANIFEST"

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DebugDev"

# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /i "$(WXWINDEV)\include"

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 ReleaseDev"

# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /i "$(WXWINDEV)\include"

!ENDIF 

# End Source File
# End Group
# Begin Group "Documents"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\docs\licence.txt
# End Source File
# Begin Source File

SOURCE=..\docs\notes.txt
# End Source File
# Begin Source File

SOURCE="..\docs\readme-after.txt"
# End Source File
# Begin Source File

SOURCE=..\docs\readme.txt
# End Source File
# Begin Source File

SOURCE=..\docs\todo.txt
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# End Group
# Begin Group "Distribution"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\scripts\buildapp
# End Source File
# Begin Source File

SOURCE=..\scripts\innobott.txt
# End Source File
# Begin Source File

SOURCE=..\scripts\innotop.txt
# End Source File
# Begin Source File

SOURCE=..\scripts\makeapp
# End Source File
# Begin Source File

SOURCE=.\Makefile
# End Source File
# Begin Source File

SOURCE=..\scripts\makeinno.sh
# End Source File
# Begin Source File

SOURCE=.\makeprog.env
# End Source File
# Begin Source File

SOURCE=..\scripts\makesetup.sh
# End Source File
# Begin Source File

SOURCE=..\scripts\maketarball.sh
# End Source File
# Begin Source File

SOURCE=..\scripts\setup.var
# End Source File
# End Group
# Begin Group "Manual"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\docs\manual\configtool.tex
# End Source File
# Begin Source File

SOURCE=..\docs\manual\Makefile
# End Source File
# Begin Source File

SOURCE=..\docs\manual\makefile.vc
# End Source File
# Begin Source File

SOURCE=..\docs\manual\tex2rtf.ini
# End Source File
# End Group
# Begin Source File

SOURCE=.\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\cdrom.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\computer.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\drive.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\file1.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\floppy.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\folder1.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\folder2.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\noentry.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\query.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\removble.ico
# End Source File
# Begin Source File

SOURCE=.\wx\msw\rightarr.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\size.cur
# End Source File
# Begin Source File

SOURCE=.\wx\msw\watch1.cur
# End Source File
# Begin Source File

SOURCE=.\bitmaps\wxconfigtool.ico
# End Source File
# End Target
# End Project
