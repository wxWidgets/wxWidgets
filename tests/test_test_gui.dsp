# Microsoft Developer Studio Project File - Name="test_test_gui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=test_gui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test_test_gui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_test_gui.mak" CFG="test_gui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_gui - Win32 DLL Universal Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test_gui - Win32 DLL Universal Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "test_gui - Win32 DLL Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test_gui - Win32 DLL Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "test_gui - Win32 Universal Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test_gui - Win32 Universal Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "test_gui - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test_gui - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test_gui - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\test_gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\test_gui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Zi /Fdvc_mswunivudll\test_gui.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswunivu" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswunivudll\testprec_test_gui.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "NOPCH" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Zi /Fdvc_mswunivudll\test_gui.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswunivu" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswunivudll\testprec_test_gui.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "NOPCH" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswunivu" /i ".\..\include" /i "." /d "WXUSINGDLL" /i ".\..\samples" /d "NOPCH" /d _CONSOLE
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswunivu" /i ".\..\include" /i "." /d "WXUSINGDLL" /i ".\..\samples" /d "NOPCH" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29u_media.lib wxmswuniv29u_xrc.lib wxbase29u_xml.lib wxmswuniv29u_adv.lib wxmswuniv29u_html.lib wxmswuniv29u_core.lib wxbase29u_net.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudll\test_gui.exe" /debug /pdb:"vc_mswunivudll\test_gui.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxmswuniv29u_media.lib wxmswuniv29u_xrc.lib wxbase29u_xml.lib wxmswuniv29u_adv.lib wxmswuniv29u_html.lib wxmswuniv29u_core.lib wxbase29u_net.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudll\test_gui.exe" /debug /pdb:"vc_mswunivudll\test_gui.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "test_gui - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\test_gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\test_gui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswunivuddll\test_gui.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_dll\mswunivud" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswunivuddll\testprec_test_gui.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "NOPCH" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswunivuddll\test_gui.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_dll\mswunivud" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswunivuddll\testprec_test_gui.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "NOPCH" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswunivud" /i ".\..\include" /i "." /d "WXUSINGDLL" /i ".\..\samples" /d "NOPCH" /d _CONSOLE
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswunivud" /i ".\..\include" /i "." /d "WXUSINGDLL" /i ".\..\samples" /d "NOPCH" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29ud_media.lib wxmswuniv29ud_xrc.lib wxbase29ud_xml.lib wxmswuniv29ud_adv.lib wxmswuniv29ud_html.lib wxmswuniv29ud_core.lib wxbase29ud_net.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\test_gui.exe" /debug /pdb:"vc_mswunivuddll\test_gui.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxmswuniv29ud_media.lib wxmswuniv29ud_xrc.lib wxbase29ud_xml.lib wxmswuniv29ud_adv.lib wxmswuniv29ud_html.lib wxmswuniv29ud_core.lib wxbase29ud_net.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\test_gui.exe" /debug /pdb:"vc_mswunivuddll\test_gui.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "test_gui - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\test_gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\test_gui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Zi /Fdvc_mswudll\test_gui.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswu" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswudll\testprec_test_gui.pch" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "NOPCH" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Zi /Fdvc_mswudll\test_gui.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswu" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswudll\testprec_test_gui.pch" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "NOPCH" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswu" /i ".\..\include" /i "." /d "WXUSINGDLL" /i ".\..\samples" /d "NOPCH" /d _CONSOLE
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswu" /i ".\..\include" /i "." /d "WXUSINGDLL" /i ".\..\samples" /d "NOPCH" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29u_media.lib wxmsw29u_xrc.lib wxbase29u_xml.lib wxmsw29u_adv.lib wxmsw29u_html.lib wxmsw29u_core.lib wxbase29u_net.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudll\test_gui.exe" /debug /pdb:"vc_mswudll\test_gui.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxmsw29u_media.lib wxmsw29u_xrc.lib wxbase29u_xml.lib wxmsw29u_adv.lib wxmsw29u_html.lib wxmsw29u_core.lib wxbase29u_net.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudll\test_gui.exe" /debug /pdb:"vc_mswudll\test_gui.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "test_gui - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\test_gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\test_gui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\test_gui.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_dll\mswud" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswuddll\testprec_test_gui.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "NOPCH" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\test_gui.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_dll\mswud" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswuddll\testprec_test_gui.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "NOPCH" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswud" /i ".\..\include" /i "." /d "WXUSINGDLL" /i ".\..\samples" /d "NOPCH" /d _CONSOLE
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswud" /i ".\..\include" /i "." /d "WXUSINGDLL" /i ".\..\samples" /d "NOPCH" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29ud_media.lib wxmsw29ud_xrc.lib wxbase29ud_xml.lib wxmsw29ud_adv.lib wxmsw29ud_html.lib wxmsw29ud_core.lib wxbase29ud_net.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuddll\test_gui.exe" /debug /pdb:"vc_mswuddll\test_gui.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxmsw29ud_media.lib wxmsw29ud_xrc.lib wxbase29ud_xml.lib wxmsw29ud_adv.lib wxmsw29ud_html.lib wxmsw29ud_core.lib wxbase29ud_net.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuddll\test_gui.exe" /debug /pdb:"vc_mswuddll\test_gui.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "test_gui - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\test_gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\test_gui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Zi /Fdvc_mswunivu\test_gui.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswunivu" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswunivu\testprec_test_gui.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "NOPCH" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Zi /Fdvc_mswunivu\test_gui.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswunivu" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswunivu\testprec_test_gui.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "NOPCH" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswunivu" /i ".\..\include" /i "." /i ".\..\samples" /d "NOPCH" /d _CONSOLE
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswunivu" /i ".\..\include" /i "." /i ".\..\samples" /d "NOPCH" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29u_media.lib wxmswuniv29u_xrc.lib wxbase29u_xml.lib wxmswuniv29u_adv.lib wxmswuniv29u_html.lib wxmswuniv29u_core.lib wxbase29u_net.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivu\test_gui.exe" /debug /pdb:"vc_mswunivu\test_gui.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxmswuniv29u_media.lib wxmswuniv29u_xrc.lib wxbase29u_xml.lib wxmswuniv29u_adv.lib wxmswuniv29u_html.lib wxmswuniv29u_core.lib wxbase29u_net.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivu\test_gui.exe" /debug /pdb:"vc_mswunivu\test_gui.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "test_gui - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\test_gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\test_gui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswunivud\test_gui.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_lib\mswunivud" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswunivud\testprec_test_gui.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "NOPCH" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswunivud\test_gui.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_lib\mswunivud" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswunivud\testprec_test_gui.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "NOPCH" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswunivud" /i ".\..\include" /i "." /i ".\..\samples" /d "NOPCH" /d _CONSOLE
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswunivud" /i ".\..\include" /i "." /i ".\..\samples" /d "NOPCH" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29ud_media.lib wxmswuniv29ud_xrc.lib wxbase29ud_xml.lib wxmswuniv29ud_adv.lib wxmswuniv29ud_html.lib wxmswuniv29ud_core.lib wxbase29ud_net.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivud\test_gui.exe" /debug /pdb:"vc_mswunivud\test_gui.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxmswuniv29ud_media.lib wxmswuniv29ud_xrc.lib wxbase29ud_xml.lib wxmswuniv29ud_adv.lib wxmswuniv29ud_html.lib wxmswuniv29ud_core.lib wxbase29ud_net.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivud\test_gui.exe" /debug /pdb:"vc_mswunivud\test_gui.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "test_gui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\test_gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\test_gui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Zi /Fdvc_mswu\test_gui.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswu" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswu\testprec_test_gui.pch" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "NOPCH" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Zi /Fdvc_mswu\test_gui.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswu" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswu\testprec_test_gui.pch" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "NOPCH" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswu" /i ".\..\include" /i "." /i ".\..\samples" /d "NOPCH" /d _CONSOLE
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswu" /i ".\..\include" /i "." /i ".\..\samples" /d "NOPCH" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29u_media.lib wxmsw29u_xrc.lib wxbase29u_xml.lib wxmsw29u_adv.lib wxmsw29u_html.lib wxmsw29u_core.lib wxbase29u_net.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswu\test_gui.exe" /debug /pdb:"vc_mswu\test_gui.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxmsw29u_media.lib wxmsw29u_xrc.lib wxbase29u_xml.lib wxmsw29u_adv.lib wxmsw29u_html.lib wxmsw29u_core.lib wxbase29u_net.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswu\test_gui.exe" /debug /pdb:"vc_mswu\test_gui.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "test_gui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\test_gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\test_gui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswud\test_gui.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_lib\mswud" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswud\testprec_test_gui.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "NOPCH" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswud\test_gui.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_lib\mswud" /I ".\..\include" /W4 /I "." /I ".\..\samples" /Yu"testprec.h" /Fp"vc_mswud\testprec_test_gui.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "NOPCH" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswud" /i ".\..\include" /i "." /i ".\..\samples" /d "NOPCH" /d _CONSOLE
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswud" /i ".\..\include" /i "." /i ".\..\samples" /d "NOPCH" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29ud_media.lib wxmsw29ud_xrc.lib wxbase29ud_xml.lib wxmsw29ud_adv.lib wxmsw29ud_html.lib wxmsw29ud_core.lib wxbase29ud_net.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswud\test_gui.exe" /debug /pdb:"vc_mswud\test_gui.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxmsw29ud_media.lib wxmsw29ud_xrc.lib wxbase29ud_xml.lib wxmsw29ud_adv.lib wxmsw29ud_html.lib wxmsw29ud_core.lib wxbase29ud_net.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswud\test_gui.exe" /debug /pdb:"vc_mswud\test_gui.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console

!ENDIF

# Begin Target

# Name "test_gui - Win32 DLL Universal Release"
# Name "test_gui - Win32 DLL Universal Debug"
# Name "test_gui - Win32 DLL Release"
# Name "test_gui - Win32 DLL Debug"
# Name "test_gui - Win32 Universal Release"
# Name "test_gui - Win32 Universal Debug"
# Name "test_gui - Win32 Release"
# Name "test_gui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\window\clientsize.cpp
# End Source File
# Begin Source File

SOURCE=.\controls\comboboxtest.cpp
# End Source File
# Begin Source File

SOURCE=.\config\config.cpp
# End Source File
# Begin Source File

SOURCE=.\dummy.cpp
# ADD BASE CPP /Yc"testprec.h"
# ADD CPP /Yc"testprec.h"
# End Source File
# Begin Source File

SOURCE=.\misc\garbage.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\guifuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\controls\headerctrltest.cpp
# End Source File
# Begin Source File

SOURCE=.\html\htmlwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\controls\listctrltest.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\point.cpp
# End Source File
# Begin Source File

SOURCE=.\image\rawbmp.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\rect.cpp
# End Source File
# Begin Source File

SOURCE=.\..\samples\sample.rc
# End Source File
# Begin Source File

SOURCE=.\misc\selstoretest.cpp
# End Source File
# Begin Source File

SOURCE=.\window\setsize.cpp
# End Source File
# Begin Source File

SOURCE=.\geometry\size.cpp
# End Source File
# Begin Source File

SOURCE=.\net\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\test.cpp
# End Source File
# Begin Source File

SOURCE=.\controls\textctrltest.cpp
# End Source File
# Begin Source File

SOURCE=.\controls\textentrytest.cpp
# End Source File
# End Group
# End Target
# End Project

