# Microsoft Developer Studio Project File - Name="test_printfbench" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=printfbench - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test_printfbench.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_printfbench.mak" CFG="printfbench - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "printfbench - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 DLL Universal Release" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 DLL Universal Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 DLL Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 DLL Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 DLL Release" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 DLL Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 Universal Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 Universal Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 Universal Release" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 Universal Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "printfbench - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "printfbench - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivudll\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswunivu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivudll\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivudll\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswunivu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivudll\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswunivu" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswunivu" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\printfbench.exe" /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase27u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\printfbench.exe" /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivuddll\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_dll\mswunivud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivuddll\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivuddll\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_dll\mswunivud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivuddll\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswunivud" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswunivud" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\printfbench.exe" /debug /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase27ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\printfbench.exe" /debug /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivdll\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswuniv" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivdll\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivdll\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswuniv" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivdll\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\lib\vc_dll\mswuniv" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\lib\vc_dll\mswuniv" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\printfbench.exe" /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase27.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\printfbench.exe" /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivddll\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_dll\mswunivd" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivddll\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivddll\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_dll\mswunivd" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivddll\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\lib\vc_dll\mswunivd" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\lib\vc_dll\mswunivd" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\printfbench.exe" /debug /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase27d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\printfbench.exe" /debug /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswudll\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswudll\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /Fdvc_mswudll\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\mswu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswudll\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswu" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswu" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\printfbench.exe" /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase27u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\printfbench.exe" /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswuddll\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_dll\mswud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswuddll\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswuddll\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_dll\mswud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswuddll\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswud" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_dll\mswud" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\printfbench.exe" /debug /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase27ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\printfbench.exe" /debug /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswdll\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\msw" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswdll\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /Fdvc_mswdll\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_dll\msw" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswdll\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i ".\..\lib\vc_dll\msw" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "__WXMSW__" /i ".\..\lib\vc_dll\msw" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\printfbench.exe" /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase27.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\printfbench.exe" /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswddll\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_dll\mswd" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswddll\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswddll\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_dll\mswd" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswddll\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\lib\vc_dll\mswd" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\lib\vc_dll\mswd" /i ".\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\printfbench.exe" /debug /libpath:".\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase27d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\printfbench.exe" /debug /libpath:".\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivu\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswunivu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivu\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivu\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswunivu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivu\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswunivu" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswunivu" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\printfbench.exe" /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase27u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\printfbench.exe" /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivud\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_lib\mswunivud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivud\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivud\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_lib\mswunivud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivud\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswunivud" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswunivud" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\printfbench.exe" /debug /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase27ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\printfbench.exe" /debug /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswuniv\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswuniv" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswuniv\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /Fdvc_mswuniv\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswuniv" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswuniv\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\lib\vc_lib\mswuniv" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\lib\vc_lib\mswuniv" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\printfbench.exe" /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase27.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\printfbench.exe" /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivd\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_lib\mswunivd" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivd\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivd\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_lib\mswunivd" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswunivd\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\lib\vc_lib\mswunivd" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\lib\vc_lib\mswunivd" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\printfbench.exe" /debug /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase27d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\printfbench.exe" /debug /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswu\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswu\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /Fdvc_mswu\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\mswu" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswu\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswu" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswu" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\printfbench.exe" /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase27u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\printfbench.exe" /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswud\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_lib\mswud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswud\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswud\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_lib\mswud" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswud\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswud" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\lib\vc_lib\mswud" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\printfbench.exe" /debug /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase27ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\printfbench.exe" /debug /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_msw\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\msw" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_msw\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /Fdvc_msw\printfbench.pdb /O2 /GR /EHsc /I ".\..\lib\vc_lib\msw" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_msw\testprec_printfbench.pch" /D "WIN32" /D "__WXMSW__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i ".\..\lib\vc_lib\msw" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "__WXMSW__" /i ".\..\lib\vc_lib\msw" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\printfbench.exe" /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase27.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\printfbench.exe" /libpath:".\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "printfbench - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\printfbench"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\printfbench"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswd\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_lib\mswd" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswd\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswd\printfbench.pdb /Od /GR /EHsc /I ".\..\lib\vc_lib\mswd" /I ".\..\include" /W4 /I "." /Yu"testprec.h" /Fp"vc_mswd\testprec_printfbench.pch" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\lib\vc_lib\mswd" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\lib\vc_lib\mswd" /i ".\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase27d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\printfbench.exe" /debug /libpath:".\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase27d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\printfbench.exe" /debug /libpath:".\..\lib\vc_lib" /subsystem:console

!ENDIF

# Begin Target

# Name "printfbench - Win32 DLL Universal Unicode Release"
# Name "printfbench - Win32 DLL Universal Unicode Debug"
# Name "printfbench - Win32 DLL Universal Release"
# Name "printfbench - Win32 DLL Universal Debug"
# Name "printfbench - Win32 DLL Unicode Release"
# Name "printfbench - Win32 DLL Unicode Debug"
# Name "printfbench - Win32 DLL Release"
# Name "printfbench - Win32 DLL Debug"
# Name "printfbench - Win32 Universal Unicode Release"
# Name "printfbench - Win32 Universal Unicode Debug"
# Name "printfbench - Win32 Universal Release"
# Name "printfbench - Win32 Universal Debug"
# Name "printfbench - Win32 Unicode Release"
# Name "printfbench - Win32 Unicode Debug"
# Name "printfbench - Win32 Release"
# Name "printfbench - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dummy.cpp
# ADD BASE CPP /Yc"testprec.h"
# ADD CPP /Yc"testprec.h"
# End Source File
# Begin Source File

SOURCE=.\benchmarks\printfbench.cpp
# End Source File
# End Group
# End Target
# End Project

