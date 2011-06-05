# Microsoft Developer Studio Project File - Name="mobiledemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=mobiledemo - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mobiledemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mobiledemo.mak" CFG="mobiledemo - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mobiledemo - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "mobiledemo - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF "$(CFG)" == "mobiledemo - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "VCProjectUnicodeDebug"
# PROP BASE Intermediate_Dir "VCProjectUnicodeDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "VCProjectUnicodeDebug"
# PROP Intermediate_Dir "VCProjectUnicodeDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /Fd"VCProjectUnicodeDebug\mobiledemo.pdb" /MDd /Od /GR /EHsc /W4 /Zi /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D__WXDEBUG__ /D_DEBUG /D_UNICODE  /I "C:\wxWidgets-2.8.10/include" /I "C:\wxWidgets-2.8.10/contrib/include" /I "C:\wxWidgets-2.8.10/lib/vc_lib/mswud" /I "../../../include" /I "C:\Program Files\Microsoft Visual Studio\vc98\include" /c
# ADD CPP /nologo /FD /Fd"VCProjectUnicodeDebug\mobiledemo.pdb" /MDd /Od /GR /EHsc /W4 /Zi /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D__WXDEBUG__ /D_DEBUG /D_UNICODE  /I "C:\wxWidgets-2.8.10/include" /I "C:\wxWidgets-2.8.10/contrib/include" /I "C:\wxWidgets-2.8.10/lib/vc_lib/mswud" /I "../../../include" /I "C:\Program Files\Microsoft Visual Studio\vc98\include" /c
# ADD BASE MTL /nologo /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D__WXDEBUG__ /D_DEBUG /D_UNICODE /mktyplib203 /win32
# ADD MTL /nologo /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D__WXDEBUG__ /D_DEBUG /D_UNICODE /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D__WXDEBUG__ /D_DEBUG /D_UNICODE /I "C:\wxWidgets-2.8.10/include" /I "C:\wxWidgets-2.8.10/contrib/include" /I "C:\wxWidgets-2.8.10/lib/vc_lib/mswud" /I "C:\Program Files\Microsoft Visual Studio\vc98\include"
# ADD RSC /l 0x405 /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D__WXDEBUG__ /D_DEBUG /D_UNICODE /I "C:\wxWidgets-2.8.10/include" /I "C:\wxWidgets-2.8.10/contrib/include" /I "C:\wxWidgets-2.8.10/lib/vc_lib/mswud" /I "C:\Program Files\Microsoft Visual Studio\vc98\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28ud_richtext.lib wxmsw28ud_aui.lib wxmsw28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxmsw28ud_adv.lib wxmsw28ud_html.lib wxmsw28ud_xrc.lib wxbase28ud_net.lib wxbase28ud_xml.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /libpath:"C:\wxWidgets-2.8.10/lib/vc_lib" /libpath:"C:\Program Files\Microsoft Visual Studio\vc98\lib" /nologo /SUBSYSTEM:WINDOWS /machine:i386 /DEBUG /out:"VCProjectUnicodeDebug\mobiledemo.exe"
# ADD LINK32 wxmsw28ud_richtext.lib wxmsw28ud_aui.lib wxmsw28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxmsw28ud_adv.lib wxmsw28ud_html.lib wxmsw28ud_xrc.lib wxbase28ud_net.lib wxbase28ud_xml.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /libpath:"C:\wxWidgets-2.8.10/lib/vc_lib" /libpath:"C:\Program Files\Microsoft Visual Studio\vc98\lib" /nologo /SUBSYSTEM:WINDOWS /machine:i386 /DEBUG /out:"VCProjectUnicodeDebug\mobiledemo.exe"

!ELSEIF "$(CFG)" == "mobiledemo - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "VCProjectUnicodeRelease"
# PROP BASE Intermediate_Dir "VCProjectUnicodeRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "VCProjectUnicodeRelease"
# PROP Intermediate_Dir "VCProjectUnicodeRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /Fd"VCProjectUnicodeRelease\mobiledemo.pdb" /MD /O2 /GR /EHsc /W4  /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D_UNICODE  /I "C:\wxWidgets-2.8.10/include" /I "C:\wxWidgets-2.8.10/contrib/include" /I "C:\wxWidgets-2.8.10/lib/vc_lib/mswu" /I "../../../include" /I "C:\Program Files\Microsoft Visual Studio\vc98\include" /c
# ADD CPP /nologo /FD /Fd"VCProjectUnicodeRelease\mobiledemo.pdb" /MD /O2 /GR /EHsc /W4  /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D_UNICODE  /I "C:\wxWidgets-2.8.10/include" /I "C:\wxWidgets-2.8.10/contrib/include" /I "C:\wxWidgets-2.8.10/lib/vc_lib/mswu" /I "../../../include" /I "C:\Program Files\Microsoft Visual Studio\vc98\include" /c
# ADD BASE MTL /nologo /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D_UNICODE /mktyplib203 /win32
# ADD MTL /nologo /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D_UNICODE /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D_UNICODE /I "C:\wxWidgets-2.8.10/include" /I "C:\wxWidgets-2.8.10/contrib/include" /I "C:\wxWidgets-2.8.10/lib/vc_lib/mswu" /I "C:\Program Files\Microsoft Visual Studio\vc98\include"
# ADD RSC /l 0x405 /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /D_UNICODE /I "C:\wxWidgets-2.8.10/include" /I "C:\wxWidgets-2.8.10/contrib/include" /I "C:\wxWidgets-2.8.10/lib/vc_lib/mswu" /I "C:\Program Files\Microsoft Visual Studio\vc98\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28u_richtext.lib wxmsw28u_aui.lib wxmsw28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxmsw28u_adv.lib wxmsw28u_html.lib wxmsw28u_xrc.lib wxbase28u_net.lib wxbase28u_xml.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /libpath:"C:\wxWidgets-2.8.10/lib/vc_lib" /libpath:"C:\Program Files\Microsoft Visual Studio\vc98\lib" /nologo /SUBSYSTEM:WINDOWS /machine:i386 /out:"VCProjectUnicodeRelease\mobiledemo.exe"
# ADD LINK32 wxmsw28u_richtext.lib wxmsw28u_aui.lib wxmsw28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxmsw28u_adv.lib wxmsw28u_html.lib wxmsw28u_xrc.lib wxbase28u_net.lib wxbase28u_xml.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /libpath:"C:\wxWidgets-2.8.10/lib/vc_lib" /libpath:"C:\Program Files\Microsoft Visual Studio\vc98\lib" /nologo /SUBSYSTEM:WINDOWS /machine:i386 /out:"VCProjectUnicodeRelease\mobiledemo.exe"

!ENDIF

# Begin Target

# Name "mobiledemo - Win32 Unicode Debug"
# Name "mobiledemo - Win32 Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_app_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_barbutton_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_bmpbutton_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_button_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_buttonstrip_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_frame_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_gauge_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_keyboard_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_listbox_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_navbar_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_navctrl_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_navitem_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_notebook_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_pagectrl_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_panel_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_scrollwin_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_searchctrl_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_segctrl_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_settings_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_sheets_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_simulator.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_slider_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_statbmp_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_stattext_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_switch_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_tabctrl_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_tablecell_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_tablectrl_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_textctrl_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_toolbar_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_utils_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_viewcontroller_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_vlbox_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_vscroll_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_webctrl_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_wheelsctrl_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\generic\mo_window_g.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\web\iehtmlwin.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\src\mobile\web\wxactivex.cpp"
# End Source File
# Begin Source File

SOURCE="demoapp.cpp"
# End Source File
# Begin Source File

SOURCE="demoframe.cpp"
# End Source File
# Begin Source File

SOURCE="mobiledemo.rc"
# End Source File

# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\app.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\barbutton.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\bmpbutton.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\button.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\buttonstrip.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\defs.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\frame.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\gauge.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\keyboard.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\listbox.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\navbar.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\navctrl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\navitem.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\notebook.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\pagectrl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\panel.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\scrollwin.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\searchctrl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\segctrl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\settings.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\sheets.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\simulator.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\slider.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\statbmp.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\stattext.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\switch.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\tabctrl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\tablecell.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\tablectrl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\textctrl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\toolbar.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\utils.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\viewcontroller.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\vlbox.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\vscroll.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\webctrl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\wheelsctrl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\wx\mobile\generic\window.h"
# End Source File
# Begin Source File

SOURCE="demoapp.h"
# End Source File
# Begin Source File

SOURCE="demoframe.h"
# End Source File

# End Group
# End Target
# End Project

