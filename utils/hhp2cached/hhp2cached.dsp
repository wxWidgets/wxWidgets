# Microsoft Developer Studio Project File - Name="hhp2cached" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=hhp2cached - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hhp2cached.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hhp2cached.mak" CFG="hhp2cached - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hhp2cached - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 DLL Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 DLL Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 DLL Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 DLL Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 DLL Release" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "hhp2cached - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hhp2cached - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivudll\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_dll\mswunivu" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivudll\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_dll\mswunivu" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswunivu" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswunivu" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29u_html.lib wxmswuniv29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\hhp2cached.exe" /libpath:".\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv29u_html.lib wxmswuniv29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\hhp2cached.exe" /libpath:".\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivuddll\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_dll\mswunivud" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivuddll\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_dll\mswunivud" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswunivud" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswunivud" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29ud_html.lib wxmswuniv29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv29ud_html.lib wxmswuniv29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivdll\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_dll\mswuniv" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivdll\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_dll\mswuniv" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\lib\vc_dll\mswuniv" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\lib\vc_dll\mswuniv" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29_html.lib wxmswuniv29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\hhp2cached.exe" /libpath:".\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv29_html.lib wxmswuniv29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\hhp2cached.exe" /libpath:".\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivddll\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_dll\mswunivd" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivddll\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_dll\mswunivd" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\lib\vc_dll\mswunivd" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\lib\vc_dll\mswunivd" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29d_html.lib wxmswuniv29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv29d_html.lib wxmswuniv29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswudll\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_dll\mswu" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswudll\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_dll\mswu" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswu" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswu" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29u_html.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\hhp2cached.exe" /libpath:".\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw29u_html.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\hhp2cached.exe" /libpath:".\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswuddll\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_dll\mswud" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswuddll\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_dll\mswud" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswud" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\lib\vc_dll\mswud" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29ud_html.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw29ud_html.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswdll\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_dll\msw" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswdll\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_dll\msw" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\lib\vc_dll\msw" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\lib\vc_dll\msw" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29_html.lib wxmsw29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\hhp2cached.exe" /libpath:".\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw29_html.lib wxmsw29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\hhp2cached.exe" /libpath:".\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswddll\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_dll\mswd" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswddll\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_dll\mswd" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\lib\vc_dll\mswd" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\lib\vc_dll\mswd" /i ".\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29d_html.lib wxmsw29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw29d_html.lib wxmsw29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivu\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_lib\mswunivu" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivu\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_lib\mswunivu" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswunivu" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswunivu" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29u_html.lib wxmswuniv29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\hhp2cached.exe" /libpath:".\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv29u_html.lib wxmswuniv29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\hhp2cached.exe" /libpath:".\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivud\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_lib\mswunivud" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivud\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_lib\mswunivud" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswunivud" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswunivud" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29ud_html.lib wxmswuniv29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv29ud_html.lib wxmswuniv29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswuniv\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_lib\mswuniv" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswuniv\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_lib\mswuniv" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\lib\vc_lib\mswuniv" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\lib\vc_lib\mswuniv" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29_html.lib wxmswuniv29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\hhp2cached.exe" /libpath:".\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv29_html.lib wxmswuniv29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\hhp2cached.exe" /libpath:".\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivd\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_lib\mswunivd" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivd\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_lib\mswunivd" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\lib\vc_lib\mswunivd" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\lib\vc_lib\mswunivd" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv29d_html.lib wxmswuniv29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv29d_html.lib wxmswuniv29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswu\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_lib\mswu" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswu\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_lib\mswu" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswu" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswu" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29u_html.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\hhp2cached.exe" /libpath:".\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw29u_html.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\hhp2cached.exe" /libpath:".\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswud\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_lib\mswud" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswud\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_lib\mswud" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswud" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\lib\vc_lib\mswud" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29ud_html.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw29ud_html.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_msw\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_lib\msw" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_msw\hhp2cached.pdb /O2 /GR /EHsc /I ".\..\..\lib\vc_lib\msw" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\lib\vc_lib\msw" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\lib\vc_lib\msw" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29_html.lib wxmsw29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\hhp2cached.exe" /libpath:".\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw29_html.lib wxmsw29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\hhp2cached.exe" /libpath:".\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "hhp2cached - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\hhp2cached"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\hhp2cached"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswd\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_lib\mswd" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswd\hhp2cached.pdb /Od /GR /EHsc /I ".\..\..\lib\vc_lib\mswd" /I ".\..\..\include" /W4 /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\lib\vc_lib\mswd" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\lib\vc_lib\mswd" /i ".\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw29d_html.lib wxmsw29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw29d_html.lib wxmsw29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\hhp2cached.exe" /debug /libpath:".\..\..\lib\vc_lib" /subsystem:windows

!ENDIF

# Begin Target

# Name "hhp2cached - Win32 DLL Universal Unicode Release"
# Name "hhp2cached - Win32 DLL Universal Unicode Debug"
# Name "hhp2cached - Win32 DLL Universal Release"
# Name "hhp2cached - Win32 DLL Universal Debug"
# Name "hhp2cached - Win32 DLL Unicode Release"
# Name "hhp2cached - Win32 DLL Unicode Debug"
# Name "hhp2cached - Win32 DLL Release"
# Name "hhp2cached - Win32 DLL Debug"
# Name "hhp2cached - Win32 Universal Unicode Release"
# Name "hhp2cached - Win32 Universal Unicode Debug"
# Name "hhp2cached - Win32 Universal Release"
# Name "hhp2cached - Win32 Universal Debug"
# Name "hhp2cached - Win32 Unicode Release"
# Name "hhp2cached - Win32 Unicode Debug"
# Name "hhp2cached - Win32 Release"
# Name "hhp2cached - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hhp2cached.cpp
# End Source File
# Begin Source File

SOURCE=.\hhp2cached.rc
# End Source File
# End Group
# End Target
# End Project

