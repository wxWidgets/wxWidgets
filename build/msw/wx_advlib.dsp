# Microsoft Developer Studio Project File - Name="advlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=advlib - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_advlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_advlib.mak" CFG="advlib - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "advlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "advlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "advlib - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "advlib - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "advlib - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "advlib - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "advlib - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "advlib - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "advlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\advlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\advlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswd" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswd\wxmsw25d_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswd" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswd\wxmsw25d_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswd\wxmsw25d_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswd\wxmsw25d_adv.lib"

!ELSEIF  "$(CFG)" == "advlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\advlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\advlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_msw" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_msw\wxmsw25_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_msw" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_msw\wxmsw25_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_msw\wxmsw25_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_msw\wxmsw25_adv.lib"

!ELSEIF  "$(CFG)" == "advlib - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\advlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\advlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswud" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswud\wxmsw25ud_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswud" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswud\wxmsw25ud_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswud\wxmsw25ud_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswud\wxmsw25ud_adv.lib"

!ELSEIF  "$(CFG)" == "advlib - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\advlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\advlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswu" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswu\wxmsw25u_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswu" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswu\wxmsw25u_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswu\wxmsw25u_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswu\wxmsw25u_adv.lib"

!ELSEIF  "$(CFG)" == "advlib - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\advlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\advlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivd" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivd\wxmswuniv25d_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivd" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivd\wxmswuniv25d_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivd\wxmswuniv25d_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivd\wxmswuniv25d_adv.lib"

!ELSEIF  "$(CFG)" == "advlib - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\advlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\advlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswuniv" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswuniv\wxmswuniv25_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswuniv" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswuniv\wxmswuniv25_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswuniv\wxmswuniv25_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswuniv\wxmswuniv25_adv.lib"

!ELSEIF  "$(CFG)" == "advlib - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\advlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\advlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivud" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivud\wxmswuniv25ud_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivud" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivud\wxmswuniv25ud_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivud\wxmswuniv25ud_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivud\wxmswuniv25ud_adv.lib"

!ELSEIF  "$(CFG)" == "advlib - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\advlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\advlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivu" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswunivu\wxmswuniv25u_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivu" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Fd..\..\lib\vc_mswunivu\wxmswuniv25u_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_advlib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_mswunivu\wxmswuniv25u_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_mswunivu\wxmswuniv25u_adv.lib"

!ENDIF

# Begin Target

# Name "advlib - Win32 Debug"
# Name "advlib - Win32 Release"
# Name "advlib - Win32 Unicode Debug"
# Name "advlib - Win32 Unicode Release"
# Name "advlib - Win32 Universal Debug"
# Name "advlib - Win32 Universal Release"
# Name "advlib - Win32 Universal Unicode Debug"
# Name "advlib - Win32 Universal Unicode Release"
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\calctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\calctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\grid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\gridctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\gridsel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\helpext.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\laywin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\sashwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\splash.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\wizard.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\grid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\joystick.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\laywin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\joystick.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\taskbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\wave.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\sashwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\splash.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\taskbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\tipdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wave.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wizard.h
# End Source File
# End Group
# Begin Group "Generic Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\generic\calctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\dcbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\grid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\gridctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\gridsel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\helpext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\laywin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\sashwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\splash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\tipdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\wizard.cpp
# End Source File
# End Group
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\common\taskbarcmn.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\joystick.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\taskbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\wave.cpp
# End Source File
# End Group
# End Target
# End Project

