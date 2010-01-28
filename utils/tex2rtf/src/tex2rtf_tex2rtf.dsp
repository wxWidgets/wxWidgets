# Microsoft Developer Studio Project File - Name="tex2rtf_tex2rtf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=tex2rtf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tex2rtf_tex2rtf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tex2rtf_tex2rtf.mak" CFG="tex2rtf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tex2rtf - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 DLL Universal Release" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 DLL Universal Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 DLL Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 DLL Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 DLL Release" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 DLL Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 Universal Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 Universal Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 Universal Release" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 Universal Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "tex2rtf - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tex2rtf - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivudll\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivu" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivudll\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivu" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswunivu" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswunivu" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\tex2rtf.exe" /pdb:"vc_mswunivudll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase28u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\tex2rtf.exe" /pdb:"vc_mswunivudll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswunivuddll\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivud" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswunivuddll\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivud" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswunivud" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswunivud" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\tex2rtf.exe" /debug /pdb:"vc_mswunivuddll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase28ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\tex2rtf.exe" /debug /pdb:"vc_mswunivuddll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivdll\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswuniv" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivdll\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswuniv" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /i ".\..\..\..\lib\vc_dll\mswuniv" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /i ".\..\..\..\lib\vc_dll\mswuniv" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\tex2rtf.exe" /pdb:"vc_mswunivdll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase28.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\tex2rtf.exe" /pdb:"vc_mswunivdll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswunivddll\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivd" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswunivddll\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivd" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_dll\mswunivd" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_dll\mswunivd" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\tex2rtf.exe" /debug /pdb:"vc_mswunivddll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase28d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\tex2rtf.exe" /debug /pdb:"vc_mswunivddll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswudll\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswu" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswudll\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswu" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswu" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswu" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\tex2rtf.exe" /pdb:"vc_mswudll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase28u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\tex2rtf.exe" /pdb:"vc_mswudll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswud" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswud" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswud" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswud" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\tex2rtf.exe" /debug /pdb:"vc_mswuddll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase28ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\tex2rtf.exe" /debug /pdb:"vc_mswuddll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswdll\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\msw" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswdll\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\msw" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /i ".\..\..\..\lib\vc_dll\msw" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /i ".\..\..\..\lib\vc_dll\msw" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\tex2rtf.exe" /pdb:"vc_mswdll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase28.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\tex2rtf.exe" /pdb:"vc_mswdll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswddll\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswd" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswddll\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswd" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_dll\mswd" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_dll\mswd" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\tex2rtf.exe" /debug /pdb:"vc_mswddll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console
# ADD LINK32 wxbase28d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\tex2rtf.exe" /debug /pdb:"vc_mswddll\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_dll" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivu\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivu" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivu\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivu" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswunivu" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswunivu" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\tex2rtf.exe" /pdb:"vc_mswunivu\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase28u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\tex2rtf.exe" /pdb:"vc_mswunivu\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswunivud\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivud" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswunivud\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivud" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswunivud" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswunivud" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\tex2rtf.exe" /debug /pdb:"vc_mswunivud\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase28ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\tex2rtf.exe" /debug /pdb:"vc_mswunivud\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswuniv\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswuniv" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswuniv\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswuniv" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /i ".\..\..\..\lib\vc_lib\mswuniv" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /i ".\..\..\..\lib\vc_lib\mswuniv" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\tex2rtf.exe" /pdb:"vc_mswuniv\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase28.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\tex2rtf.exe" /pdb:"vc_mswuniv\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswunivd\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivd" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswunivd\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivd" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_lib\mswunivd" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_lib\mswunivd" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\tex2rtf.exe" /debug /pdb:"vc_mswunivd\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase28d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\tex2rtf.exe" /debug /pdb:"vc_mswunivd\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswu\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswu" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswu\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswu" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswu" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswu" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\tex2rtf.exe" /pdb:"vc_mswu\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase28u.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\tex2rtf.exe" /pdb:"vc_mswu\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswud\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswud" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswud\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswud" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswud" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswud" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\tex2rtf.exe" /debug /pdb:"vc_mswud\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase28ud.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\tex2rtf.exe" /debug /pdb:"vc_mswud\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_msw\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\msw" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MD /Fdvc_msw\tex2rtf.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\msw" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /i ".\..\..\..\lib\vc_lib\msw" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /i ".\..\..\..\lib\vc_lib\msw" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\tex2rtf.exe" /pdb:"vc_msw\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase28.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\tex2rtf.exe" /pdb:"vc_msw\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console

!ELSEIF  "$(CFG)" == "tex2rtf - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\tex2rtf"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\tex2rtf"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswd\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswd" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswd\tex2rtf.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswd" /I ".\..\..\..\include" /W4 /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /D "NO_GUI" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_lib\mswd" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_lib\mswd" /i ".\..\..\..\include" /i "." /d "_CONSOLE" /d wxUSE_GUI=0 /d NO_GUI
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase28d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\tex2rtf.exe" /debug /pdb:"vc_mswd\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console
# ADD LINK32 wxbase28d.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\tex2rtf.exe" /debug /pdb:"vc_mswd\tex2rtf.pdb" /libpath:".\..\..\..\lib\vc_lib" /subsystem:console

!ENDIF

# Begin Target

# Name "tex2rtf - Win32 DLL Universal Unicode Release"
# Name "tex2rtf - Win32 DLL Universal Unicode Debug"
# Name "tex2rtf - Win32 DLL Universal Release"
# Name "tex2rtf - Win32 DLL Universal Debug"
# Name "tex2rtf - Win32 DLL Unicode Release"
# Name "tex2rtf - Win32 DLL Unicode Debug"
# Name "tex2rtf - Win32 DLL Release"
# Name "tex2rtf - Win32 DLL Debug"
# Name "tex2rtf - Win32 Universal Unicode Release"
# Name "tex2rtf - Win32 Universal Unicode Debug"
# Name "tex2rtf - Win32 Universal Release"
# Name "tex2rtf - Win32 Universal Debug"
# Name "tex2rtf - Win32 Unicode Release"
# Name "tex2rtf - Win32 Unicode Debug"
# Name "tex2rtf - Win32 Release"
# Name "tex2rtf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\htmlutil.cpp
# End Source File
# Begin Source File

SOURCE=.\readshg.cpp
# End Source File
# Begin Source File

SOURCE=.\rtfutils.cpp
# End Source File
# Begin Source File

SOURCE=.\table.cpp
# End Source File
# Begin Source File

SOURCE=.\tex2any.cpp
# End Source File
# Begin Source File

SOURCE=.\tex2rtf.cpp
# End Source File
# Begin Source File

SOURCE=.\texutils.cpp
# End Source File
# Begin Source File

SOURCE=.\xlputils.cpp
# End Source File
# End Group
# End Target
# End Project

