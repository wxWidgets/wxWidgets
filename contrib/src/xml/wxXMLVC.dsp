# Microsoft Developer Studio Project File - Name="wxXMLVC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxXMLVC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxXMLVC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxXMLVC.mak" CFG="wxXMLVC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxXMLVC - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxXMLVC - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxXMLVC - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "../../../include" /I "../../include" /I "d:\libxml\libxml2-2.1.1" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\wxxml.lib"

!ELSEIF  "$(CFG)" == "wxXMLVC - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "../../../include" /I "../../include" /I "d:\libxml\libxml2-2.1.1" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\wxxmld.lib"

!ENDIF 

# Begin Target

# Name "wxXMLVC - Win32 Release"
# Name "wxXMLVC - Win32 Debug"
# Begin Source File

SOURCE=.\xh_bttn.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_chckb.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_chckl.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_choic.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_combo.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_gauge.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_html.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_menu.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_notbk.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_panel.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_radbt.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_radbx.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_sizer.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_slidr.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_spin.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_stbmp.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_sttxt.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_text.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_listb.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_toolb.cpp
# End Source File
# Begin Source File

SOURCE=.\xml.cpp
# End Source File
# Begin Source File

SOURCE=.\xmlbin.cpp
# End Source File
# Begin Source File

SOURCE=.\xmlbinz.cpp
# End Source File
# Begin Source File

SOURCE=.\xmlpars.cpp
# End Source File
# Begin Source File

SOURCE=.\xmlres.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_bmpbt.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_cald.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_listc.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_scrol.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_stbox.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_tree.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_stlin.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_bmp.cpp
# End Source File
# Begin Source File

SOURCE=.\xh_unkwn.cpp
# End Source File
# Begin Source File

SOURCE=.\xmlrsall.cpp
# End Source File
# End Target
# End Project
