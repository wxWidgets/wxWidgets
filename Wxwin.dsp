# Microsoft Developer Studio Project File - Name="wxWin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxWin - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Wxwin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Wxwin.mak" CFG="wxWin - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxWin - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxWin - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "wxWin - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /w /W0 /GX /O2 /I ".\include" /I ".\include\wx\msw" /I ".\include\wx\generic" /I ".\include\wx\other" /I ".\src\png" /I ".\src\zlib" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D "__WINDOWS__" /YX"wx\wxprec.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /w /W0 /GX /Z7 /Od /Gf /Gy /I ".\include" /I ".\include\wx\msw" /I ".\include\wx\generic" /I ".\include\wx\other" /I ".\src\png" /I ".\src\zlib" /D "__WXDEBUG__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WXDEBUG=1 /D "__WINDOWS__" /YX"wx\wxprec.h" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "wxWin - Win32 Release"
# Name "wxWin - Win32 Debug"
# Begin Group "Resources"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\include\wx\msw\wx.rc
# End Source File
# End Group
# Begin Group "win-sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\msw\accel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\app.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\bitmap.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\bmpbuttn.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\brush.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\button.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\checkbox.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\checklst.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\choice.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\clipbrd.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\colordlg.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\colour.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\combobox.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\control.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\curico.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\cursor.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\data.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dc.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dcclient.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dcmemory.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dcprint.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dcscreen.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dde.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dialog.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dib.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dibutils.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dirdlg.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\dummy.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\filedlg.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\font.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\fontdlg.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\frame.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\gauge95.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\gaugemsw.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\gdiobj.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\helpwin.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\icon.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\imaglist.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\iniconf.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\joystick.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\listbox.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\listctrl.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\main.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\mdi.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\menu.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\menuitem.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\metafile.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\minifram.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\msgdlg.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\nativdlg.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\notebook.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\ownerdrw.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\palette.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\pen.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\penwin.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\pnghand.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\printdlg.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\printwin.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\radiobox.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\radiobut.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\regconf.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\region.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\registry.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\scrolbar.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\settings.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\slider95.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\slidrmsw.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\spinbutt.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\statbmp.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\statbox.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\statbr95.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\stattext.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\tabctrl.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\taskbar.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\tbar95.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\tbarmsw.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\textctrl.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\thread.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\timer.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\treectrl.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\utils.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\utilsexc.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\wave.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\msw\window.cpp

!IF  "$(CFG)" == "wxWin - Win32 Release"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "STRICT"

!ENDIF 

# End Source File
# End Group
# Begin Group "win-header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\wx\msw\app.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\bitmap.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\brush.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\button.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\checkbox.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\checklst.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\choice.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\clipbrd.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\colordlg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\colour.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\combobox.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\control.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\curico.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\curicop.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\cursor.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\dc.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\dcclient.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\dcmemory.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\dcprint.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\dcscreen.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\dde.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\dialog.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\dib.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\dibutils.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\dirdlg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\filedlg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\font.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\fontdlg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\frame.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\gauge.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\gauge95.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\gaugemsw.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\gdiobj.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\helpwin.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\icon.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\imaglist.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\joystick.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\listbox.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\listctrl.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\mdi.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\menu.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\metafile.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\minifram.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\msgdlg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\notebook.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\palette.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\pen.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\pnghand.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\pngread.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\printdlg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\printwin.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\private.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\radiobox.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\radiobut.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\regconf.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\region.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\registry.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\scrolbar.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\settings.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\setup.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\slider.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\slider95.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\slidrmsw.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\spinbutt.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\statbmp.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\statbox.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\statbr95.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\stattext.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\tabctrl.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\taskbar.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\tbar95.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\tbarmsw.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\textctrl.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\timer.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\treectrl.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\wave.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\msw\window.h
# End Source File
# End Group
# Begin Group "generic-sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\generic\choicdgg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\colrdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\fontdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\gridg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\helpxlp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\msgdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\panelg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\printps.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\prntdlgg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\scrolwin.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\statusbr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\tabg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generic\textdlgg.cpp
# End Source File
# End Group
# Begin Group "generic-header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\wx\generic\choicdgg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\colrdlgg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\fontdlgg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\gridg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\helpxlp.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\imaglist.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\listctrl.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\msgdlgg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\panelg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\printps.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\prntdlgg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\scrolwin.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\splitter.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\statusbr.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\tabg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\textdlgg.h
# End Source File
# Begin Source File

SOURCE=.\include\wx\generic\treectrl.h
# End Source File
# End Group
# Begin Group "common-sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\common\cmndata.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\config.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\date.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\db.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\dbtable.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\docmdi.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\docview.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\dynarray.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\event.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\extended.c
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

SOURCE=.\src\common\framecmn.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\gdicmn.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\hash.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\helpbase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\intl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\ipcbase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\layout.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\list.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\log.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\memory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\module.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\object.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\odbc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\postscrp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\prntbase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\process.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\resource.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\serbase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\string.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\tbarbase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\tbarsmpl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\textfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\time.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\timercmn.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\utilscmn.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\validate.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\valtext.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\wincmn.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\wxexpr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\common\Y_tab.c

!IF  "$(CFG)" == "wxWin - Win32 Release"

# ADD CPP /D "YY_USE_PROTOS" /D "USE_DEFINE"

!ELSEIF  "$(CFG)" == "wxWin - Win32 Debug"

# ADD CPP /D "USE_DEFINE" /D "YY_USE_PROTOS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\common\zstream.cpp
# End Source File
# End Group
# Begin Group "ole-sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\msw\ole\dataobj.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\ole\dropsrc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\ole\droptgt.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\ole\oleutils.cpp
# End Source File
# Begin Source File

SOURCE=.\src\msw\ole\uuid.cpp
# End Source File
# End Group
# Begin Group "png"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\png\png.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngerror.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngget.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngmem.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngpread.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngread.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngrio.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngrtran.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngrutil.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngset.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngtrans.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngwio.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngwrite.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngwtran.c
# End Source File
# Begin Source File

SOURCE=.\src\png\pngwutil.c
# End Source File
# End Group
# Begin Group "zlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\compress.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\infblock.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\infcodes.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\infutil.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\trees.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\src\zlib\zutil.c
# End Source File
# End Group
# End Target
# End Project
