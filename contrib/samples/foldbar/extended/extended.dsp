# Microsoft Developer Studio Project File - Name="extended" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=extended - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "extended.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "extended.mak" CFG="extended - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "extended - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 DLL Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 DLL Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 DLL Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 DLL Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 DLL Release" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "extended - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "extended - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivudll\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswunivu" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivudll\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswunivu" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_dll\mswunivu" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_dll\mswunivu" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28u_foldbar.lib wxmswuniv28u_adv.lib wxmswuniv28u_html.lib wxbase28u_xml.lib wxmswuniv28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\extended.exe" /pdb:"vc_mswunivudll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv28u_foldbar.lib wxmswuniv28u_adv.lib wxmswuniv28u_html.lib wxbase28u_xml.lib wxmswuniv28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\extended.exe" /pdb:"vc_mswunivudll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswunivuddll\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswunivud" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswunivuddll\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswunivud" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_dll\mswunivud" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_dll\mswunivud" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28ud_foldbar.lib wxmswuniv28ud_adv.lib wxmswuniv28ud_html.lib wxbase28ud_xml.lib wxmswuniv28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\extended.exe" /debug /pdb:"vc_mswunivuddll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv28ud_foldbar.lib wxmswuniv28ud_adv.lib wxmswuniv28ud_html.lib wxbase28ud_xml.lib wxmswuniv28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\extended.exe" /debug /pdb:"vc_mswunivuddll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivdll\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswuniv" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivdll\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswuniv" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /i ".\..\..\..\..\lib\vc_dll\mswuniv" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /i ".\..\..\..\..\lib\vc_dll\mswuniv" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28_foldbar.lib wxmswuniv28_adv.lib wxmswuniv28_html.lib wxbase28_xml.lib wxmswuniv28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\extended.exe" /pdb:"vc_mswunivdll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv28_foldbar.lib wxmswuniv28_adv.lib wxmswuniv28_html.lib wxbase28_xml.lib wxmswuniv28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\extended.exe" /pdb:"vc_mswunivdll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswunivddll\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswunivd" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswunivddll\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswunivd" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\..\lib\vc_dll\mswunivd" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\..\lib\vc_dll\mswunivd" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28d_foldbar.lib wxmswuniv28d_adv.lib wxmswuniv28d_html.lib wxbase28d_xml.lib wxmswuniv28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\extended.exe" /debug /pdb:"vc_mswunivddll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv28d_foldbar.lib wxmswuniv28d_adv.lib wxmswuniv28d_html.lib wxbase28d_xml.lib wxmswuniv28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\extended.exe" /debug /pdb:"vc_mswunivddll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswudll\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswu" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswudll\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswu" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_dll\mswu" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_dll\mswu" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28u_foldbar.lib wxmsw28u_adv.lib wxmsw28u_html.lib wxbase28u_xml.lib wxmsw28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\extended.exe" /pdb:"vc_mswudll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw28u_foldbar.lib wxmsw28u_adv.lib wxmsw28u_html.lib wxbase28u_xml.lib wxmsw28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\extended.exe" /pdb:"vc_mswudll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswud" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswuddll\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswud" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_dll\mswud" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_dll\mswud" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28ud_foldbar.lib wxmsw28ud_adv.lib wxmsw28ud_html.lib wxbase28ud_xml.lib wxmsw28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\extended.exe" /debug /pdb:"vc_mswuddll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw28ud_foldbar.lib wxmsw28ud_adv.lib wxmsw28ud_html.lib wxbase28ud_xml.lib wxmsw28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\extended.exe" /debug /pdb:"vc_mswuddll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswdll\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\msw" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswdll\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\msw" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /i ".\..\..\..\..\lib\vc_dll\msw" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /i ".\..\..\..\..\lib\vc_dll\msw" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28_foldbar.lib wxmsw28_adv.lib wxmsw28_html.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\extended.exe" /pdb:"vc_mswdll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw28_foldbar.lib wxmsw28_adv.lib wxmsw28_html.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\extended.exe" /pdb:"vc_mswdll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswddll\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswd" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswddll\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_dll\mswd" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\..\lib\vc_dll\mswd" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\..\lib\vc_dll\mswd" /i ".\..\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28d_foldbar.lib wxmsw28d_adv.lib wxmsw28d_html.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\extended.exe" /debug /pdb:"vc_mswddll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw28d_foldbar.lib wxmsw28d_adv.lib wxmsw28d_html.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\extended.exe" /debug /pdb:"vc_mswddll\extended.pdb" /libpath:".\..\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivu\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswunivu" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivu\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswunivu" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_lib\mswunivu" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_lib\mswunivu" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28u_foldbar.lib wxmswuniv28u_adv.lib wxmswuniv28u_html.lib wxbase28u_xml.lib wxmswuniv28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\extended.exe" /pdb:"vc_mswunivu\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv28u_foldbar.lib wxmswuniv28u_adv.lib wxmswuniv28u_html.lib wxbase28u_xml.lib wxmswuniv28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\extended.exe" /pdb:"vc_mswunivu\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswunivud\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswunivud" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswunivud\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswunivud" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_lib\mswunivud" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_lib\mswunivud" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28ud_foldbar.lib wxmswuniv28ud_adv.lib wxmswuniv28ud_html.lib wxbase28ud_xml.lib wxmswuniv28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\extended.exe" /debug /pdb:"vc_mswunivud\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv28ud_foldbar.lib wxmswuniv28ud_adv.lib wxmswuniv28ud_html.lib wxbase28ud_xml.lib wxmswuniv28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\extended.exe" /debug /pdb:"vc_mswunivud\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswuniv\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswuniv" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswuniv\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswuniv" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /i ".\..\..\..\..\lib\vc_lib\mswuniv" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /i ".\..\..\..\..\lib\vc_lib\mswuniv" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28_foldbar.lib wxmswuniv28_adv.lib wxmswuniv28_html.lib wxbase28_xml.lib wxmswuniv28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\extended.exe" /pdb:"vc_mswuniv\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv28_foldbar.lib wxmswuniv28_adv.lib wxmswuniv28_html.lib wxbase28_xml.lib wxmswuniv28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\extended.exe" /pdb:"vc_mswuniv\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswunivd\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswunivd" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswunivd\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswunivd" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\..\lib\vc_lib\mswunivd" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\..\lib\vc_lib\mswunivd" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv28d_foldbar.lib wxmswuniv28d_adv.lib wxmswuniv28d_html.lib wxbase28d_xml.lib wxmswuniv28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\extended.exe" /debug /pdb:"vc_mswunivd\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv28d_foldbar.lib wxmswuniv28d_adv.lib wxmswuniv28d_html.lib wxbase28d_xml.lib wxmswuniv28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\extended.exe" /debug /pdb:"vc_mswunivd\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswu\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswu" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswu\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswu" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_lib\mswu" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_lib\mswu" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28u_foldbar.lib wxmsw28u_adv.lib wxmsw28u_html.lib wxbase28u_xml.lib wxmsw28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\extended.exe" /pdb:"vc_mswu\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw28u_foldbar.lib wxmsw28u_adv.lib wxmsw28u_html.lib wxbase28u_xml.lib wxmsw28u_core.lib wxbase28u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\extended.exe" /pdb:"vc_mswu\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswud\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswud" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswud\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswud" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_lib\mswud" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\..\lib\vc_lib\mswud" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28ud_foldbar.lib wxmsw28ud_adv.lib wxmsw28ud_html.lib wxbase28ud_xml.lib wxmsw28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\extended.exe" /debug /pdb:"vc_mswud\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw28ud_foldbar.lib wxmsw28ud_adv.lib wxmsw28ud_html.lib wxbase28ud_xml.lib wxmsw28ud_core.lib wxbase28ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\extended.exe" /debug /pdb:"vc_mswud\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_msw\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\msw" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_msw\extended.pdb /O2 /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\msw" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "NDEBUG" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /i ".\..\..\..\..\lib\vc_lib\msw" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /i ".\..\..\..\..\lib\vc_lib\msw" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28_foldbar.lib wxmsw28_adv.lib wxmsw28_html.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\extended.exe" /pdb:"vc_msw\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw28_foldbar.lib wxmsw28_adv.lib wxmsw28_html.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\extended.exe" /pdb:"vc_msw\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "extended - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\extended"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\extended"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Fdvc_mswd\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswd" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Fdvc_mswd\extended.pdb /Od /Gm /GR /EHsc /I ".\..\..\..\..\lib\vc_lib\mswd" /I ".\..\..\..\..\include" /W4 /I "." /I ".\..\..\..\..\samples" /I ".\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\..\lib\vc_lib\mswd" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\..\lib\vc_lib\mswd" /i ".\..\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\..\samples" /d "NOPCH" /i .\..\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28d_foldbar.lib wxmsw28d_adv.lib wxmsw28d_html.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\extended.exe" /debug /pdb:"vc_mswd\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw28d_foldbar.lib wxmsw28d_adv.lib wxmsw28d_html.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\extended.exe" /debug /pdb:"vc_mswd\extended.pdb" /libpath:".\..\..\..\..\lib\vc_lib" /subsystem:windows

!ENDIF

# Begin Target

# Name "extended - Win32 DLL Universal Unicode Release"
# Name "extended - Win32 DLL Universal Unicode Debug"
# Name "extended - Win32 DLL Universal Release"
# Name "extended - Win32 DLL Universal Debug"
# Name "extended - Win32 DLL Unicode Release"
# Name "extended - Win32 DLL Unicode Debug"
# Name "extended - Win32 DLL Release"
# Name "extended - Win32 DLL Debug"
# Name "extended - Win32 Universal Unicode Release"
# Name "extended - Win32 Universal Unicode Debug"
# Name "extended - Win32 Universal Release"
# Name "extended - Win32 Universal Debug"
# Name "extended - Win32 Unicode Release"
# Name "extended - Win32 Unicode Debug"
# Name "extended - Win32 Release"
# Name "extended - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\extended.cpp
# End Source File
# Begin Source File

SOURCE=.\extended.rc
# End Source File
# End Group
# End Target
# End Project

