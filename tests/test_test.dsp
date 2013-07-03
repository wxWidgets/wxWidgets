# Microsoft Developer Studio Project File - Name="test_test" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=test - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test_test.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_test.mak" CFG="test - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test - Win32 DLL Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test - Win32 DLL Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "test - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\test"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\test"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Zi /Fdvc_mswudll\test.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswudll\testprec_test.pch" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /Zi /Fdvc_mswudll\test.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswudll\testprec_test.pch" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\lib\vc_dll\mswu" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\lib\vc_dll\mswu" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase29u_net.lib wxbase29u_xml.lib wxbase29u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /machine:i386 /out:"vc_mswudll\test.exe" /debug /pdb:"vc_mswudll\test.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase29u_net.lib wxbase29u_xml.lib wxbase29u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /machine:i386 /out:"vc_mswudll\test.exe" /debug /pdb:"vc_mswudll\test.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "test - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\test"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\test"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\test.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_dll\mswud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswuddll\testprec_test.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\test.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_dll\mswud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswuddll\testprec_test.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswud" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswud" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase29ud_net.lib wxbase29ud_xml.lib wxbase29ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /machine:i386 /out:"vc_mswuddll\test.exe" /debug /pdb:"vc_mswuddll\test.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase29ud_net.lib wxbase29ud_xml.lib wxbase29ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /machine:i386 /out:"vc_mswuddll\test.exe" /debug /pdb:"vc_mswuddll\test.pdb" /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "test - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\test"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\test"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Zi /Fdvc_mswu\test.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswu\testprec_test.pch" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /Zi /Fdvc_mswu\test.pdb /opt:ref /opt:icf /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswu\testprec_test.pch" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\lib\vc_lib\mswu" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\lib\vc_lib\mswu" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase29u_net.lib wxbase29u_xml.lib wxbase29u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /machine:i386 /out:"vc_mswu\test.exe" /debug /pdb:"vc_mswu\test.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase29u_net.lib wxbase29u_xml.lib wxbase29u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /machine:i386 /out:"vc_mswu\test.exe" /debug /pdb:"vc_mswu\test.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "test - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\test"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\test"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswud\test.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_lib\mswud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswud\testprec_test.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswud\test.pdb /Od /Gm /GR /EHsc /I ".\..\lib\vc_lib\mswud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswud\testprec_test.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswud" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswud" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase29ud_net.lib wxbase29ud_xml.lib wxbase29ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /machine:i386 /out:"vc_mswud\test.exe" /debug /pdb:"vc_mswud\test.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase29ud_net.lib wxbase29ud_xml.lib wxbase29ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib /nologo /machine:i386 /out:"vc_mswud\test.exe" /debug /pdb:"vc_mswud\test.pdb" /libpath:".\..\lib\vc_lib" /subsystem:console

!ENDIF

# Begin Target

# Name "test - Win32 DLL Release"
# Name "test - Win32 DLL Debug"
# Name "test - Win32 Release"
# Name "test - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\any\anytest.cpp
# End Source File
# Begin Source File

SOURCE=.\archive\archivetest.cpp
# End Source File
# Begin Source File

SOURCE=.\arrays\arrays.cpp
# End Source File
# Begin Source File

SOURCE=.\thread\atomic.cpp
# End Source File
# Begin Source File

SOURCE=.\base64\base64.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\bstream.cpp
# End Source File
# Begin Source File

SOURCE=.\cmdline\cmdlinetest.cpp
# End Source File
# Begin Source File

SOURCE=.\mbconv\convautotest.cpp
# End Source File
# Begin Source File

SOURCE=.\strings\crt.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\datastreamtest.cpp
# End Source File
# Begin Source File

SOURCE=.\datetime\datetimetest.cpp
# End Source File
# Begin Source File

SOURCE=.\file\dir.cpp
# End Source File
# Begin Source File

SOURCE=.\dummy.cpp
# ADD BASE CPP /Yc"testprec.h"
# ADD CPP /Yc"testprec.h"
# End Source File
# Begin Source File

SOURCE=.\misc\dynamiclib.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\environ.cpp
# End Source File
# Begin Source File

SOURCE=.\weakref\evtconnection.cpp
# End Source File
# Begin Source File

SOURCE=.\events\evthandler.cpp
# End Source File
# Begin Source File

SOURCE=.\events\evtlooptest.cpp
# End Source File
# Begin Source File

SOURCE=.\events\evtsource.cpp
# End Source File
# Begin Source File

SOURCE=.\exec\exec.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\ffilestream.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\fileback.cpp
# End Source File
# Begin Source File

SOURCE=.\config\fileconf.cpp
# End Source File
# Begin Source File

SOURCE=.\file\filefn.cpp
# End Source File
# Begin Source File

SOURCE=.\filekind\filekind.cpp
# End Source File
# Begin Source File

SOURCE=.\filename\filenametest.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\filestream.cpp
# End Source File
# Begin Source File

SOURCE=.\filesys\filesystest.cpp
# End Source File
# Begin Source File

SOURCE=.\file\filetest.cpp
# End Source File
# Begin Source File

SOURCE=.\fontmap\fontmaptest.cpp
# End Source File
# Begin Source File

SOURCE=.\formatconverter\formatconvertertest.cpp
# End Source File
# Begin Source File

SOURCE=.\fswatcher\fswatchertest.cpp
# End Source File
# Begin Source File

SOURCE=.\uris\ftp.cpp
# End Source File
# Begin Source File

SOURCE=.\hashes\hashes.cpp
# End Source File
# Begin Source File

SOURCE=.\interactive\input.cpp
# End Source File
# Begin Source File

SOURCE=.\intl\intltest.cpp
# End Source File
# Begin Source File

SOURCE=.\strings\iostream.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\iostreams.cpp
# End Source File
# Begin Source File

SOURCE=.\net\ipc.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\largefile.cpp
# End Source File
# Begin Source File

SOURCE=.\lists\lists.cpp
# End Source File
# Begin Source File

SOURCE=.\log\logtest.cpp
# End Source File
# Begin Source File

SOURCE=.\longlong\longlongtest.cpp
# End Source File
# Begin Source File

SOURCE=.\mbconv\mbconvtest.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\memstream.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\metatest.cpp
# End Source File
# Begin Source File

SOURCE=.\thread\misc.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\misctests.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\module.cpp
# End Source File
# Begin Source File

SOURCE=.\strings\numformatter.cpp
# End Source File
# Begin Source File

SOURCE=.\interactive\output.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\pathlist.cpp
# End Source File
# Begin Source File

SOURCE=.\thread\queue.cpp
# End Source File
# Begin Source File

SOURCE=.\config\regconf.cpp
# End Source File
# Begin Source File

SOURCE=.\regex\regextest.cpp
# End Source File
# Begin Source File

SOURCE=.\scopeguard\scopeguardtest.cpp
# End Source File
# Begin Source File

SOURCE=.\net\socket.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\socketstream.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\sstream.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\stdstream.cpp
# End Source File
# Begin Source File

SOURCE=.\strings\stdstrings.cpp
# End Source File
# Begin Source File

SOURCE=.\events\stopwatch.cpp
# End Source File
# Begin Source File

SOURCE=.\strings\strings.cpp
# End Source File
# Begin Source File

SOURCE=.\archive\tartest.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\tempfile.cpp
# End Source File
# Begin Source File

SOURCE=.\test.cpp
# End Source File
# Begin Source File

SOURCE=.\textfile\textfiletest.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\textstreamtest.cpp
# End Source File
# Begin Source File

SOURCE=.\events\timertest.cpp
# End Source File
# Begin Source File

SOURCE=.\thread\tls.cpp
# End Source File
# Begin Source File

SOURCE=.\strings\tokenizer.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\typeinfotest.cpp
# End Source File
# Begin Source File

SOURCE=.\strings\unichar.cpp
# End Source File
# Begin Source File

SOURCE=.\strings\unicode.cpp
# End Source File
# Begin Source File

SOURCE=.\uris\uris.cpp
# End Source File
# Begin Source File

SOURCE=.\uris\url.cpp
# End Source File
# Begin Source File

SOURCE=.\strings\vararg.cpp
# End Source File
# Begin Source File

SOURCE=.\vectors\vectors.cpp
# End Source File
# Begin Source File

SOURCE=.\strings\vsnprintf.cpp
# End Source File
# Begin Source File

SOURCE=.\weakref\weakref.cpp
# End Source File
# Begin Source File

SOURCE=.\regex\wxregextest.cpp
# End Source File
# Begin Source File

SOURCE=.\xlocale\xlocale.cpp
# End Source File
# Begin Source File

SOURCE=.\xml\xmltest.cpp
# End Source File
# Begin Source File

SOURCE=.\archive\ziptest.cpp
# End Source File
# Begin Source File

SOURCE=.\streams\zlibstream.cpp
# End Source File
# End Group
# End Target
# End Project

