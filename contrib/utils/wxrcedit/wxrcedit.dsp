# Microsoft Developer Studio Project File - Name="wxrcedit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=wxrcedit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxrcedit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxrcedit.mak" CFG="wxrcedit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxrcedit - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 DLL Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 DLL Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 DLL Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 DLL Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 DLL Release" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "wxrcedit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxrcedit - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivudll\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswunivu" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivudll\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswunivu" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswunivu" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswunivu" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25u_xrc.lib wxmswuniv25u_adv.lib wxmswuniv25u_html.lib wxmswuniv25u_core.lib wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv25u_xrc.lib wxmswuniv25u_adv.lib wxmswuniv25u_html.lib wxmswuniv25u_core.lib wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivuddll\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswunivud" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivuddll\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswunivud" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswunivud" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswunivud" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25ud_xrc.lib wxmswuniv25ud_adv.lib wxmswuniv25ud_html.lib wxmswuniv25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv25ud_xrc.lib wxmswuniv25ud_adv.lib wxmswuniv25ud_html.lib wxmswuniv25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivdll\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswuniv" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivdll\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswuniv" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswuniv" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswuniv" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25_xrc.lib wxmswuniv25_adv.lib wxmswuniv25_html.lib wxmswuniv25_core.lib wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv25_xrc.lib wxmswuniv25_adv.lib wxmswuniv25_html.lib wxmswuniv25_core.lib wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivddll\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswunivd" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivddll\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswunivd" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswunivd" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswunivd" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25d_xrc.lib wxmswuniv25d_adv.lib wxmswuniv25d_html.lib wxmswuniv25d_core.lib wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv25d_xrc.lib wxmswuniv25d_adv.lib wxmswuniv25d_html.lib wxmswuniv25d_core.lib wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswudll\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswu" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswudll\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswu" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswu" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswu" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25u_xrc.lib wxmsw25u_adv.lib wxmsw25u_html.lib wxmsw25u_core.lib wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw25u_xrc.lib wxmsw25u_adv.lib wxmsw25u_html.lib wxmsw25u_core.lib wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswuddll\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswud" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswuddll\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswud" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswud" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswud" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25ud_xrc.lib wxmsw25ud_adv.lib wxmsw25ud_html.lib wxmsw25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw25ud_xrc.lib wxmsw25ud_adv.lib wxmsw25ud_html.lib wxmsw25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswdll\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\msw" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswdll\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\msw" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\msw" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\msw" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_xrc.lib wxmsw25_adv.lib wxmsw25_html.lib wxmsw25_core.lib wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw25_xrc.lib wxmsw25_adv.lib wxmsw25_html.lib wxmsw25_core.lib wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswddll\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswd" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswddll\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_dll\mswd" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswd" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_dll\mswd" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_xrc.lib wxmsw25d_adv.lib wxmsw25d_html.lib wxmsw25d_core.lib wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw25d_xrc.lib wxmsw25d_adv.lib wxmsw25d_html.lib wxmsw25d_core.lib wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivu\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswunivu" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivu\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswunivu" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswunivu" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswunivu" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25u_xrc.lib wxmswuniv25u_adv.lib wxmswuniv25u_html.lib wxmswuniv25u_core.lib wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv25u_xrc.lib wxmswuniv25u_adv.lib wxmswuniv25u_html.lib wxmswuniv25u_core.lib wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivud\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswunivud" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivud\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswunivud" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswunivud" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswunivud" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25ud_xrc.lib wxmswuniv25ud_adv.lib wxmswuniv25ud_html.lib wxmswuniv25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv25ud_xrc.lib wxmswuniv25ud_adv.lib wxmswuniv25ud_html.lib wxmswuniv25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswuniv\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswuniv" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswuniv\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswuniv" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswuniv" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswuniv" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25_xrc.lib wxmswuniv25_adv.lib wxmswuniv25_html.lib wxmswuniv25_core.lib wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv25_xrc.lib wxmswuniv25_adv.lib wxmswuniv25_html.lib wxmswuniv25_core.lib wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivd\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswunivd" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivd\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswunivd" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswunivd" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswunivd" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25d_xrc.lib wxmswuniv25d_adv.lib wxmswuniv25d_html.lib wxmswuniv25d_core.lib wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv25d_xrc.lib wxmswuniv25d_adv.lib wxmswuniv25d_html.lib wxmswuniv25d_core.lib wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswu\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswu" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswu\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswu" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswu" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswu" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25u_xrc.lib wxmsw25u_adv.lib wxmsw25u_html.lib wxmsw25u_core.lib wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw25u_xrc.lib wxmsw25u_adv.lib wxmsw25u_html.lib wxmsw25u_core.lib wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswud\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswud" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswud\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswud" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswud" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswud" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25ud_xrc.lib wxmsw25ud_adv.lib wxmsw25ud_html.lib wxmsw25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw25ud_xrc.lib wxmsw25ud_adv.lib wxmsw25ud_html.lib wxmsw25ud_core.lib wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_msw\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\msw" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_msw\wxrcedit.pdb /O2 /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\msw" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\msw" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\msw" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_xrc.lib wxmsw25_adv.lib wxmsw25_html.lib wxmsw25_core.lib wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw25_xrc.lib wxmsw25_adv.lib wxmsw25_html.lib wxmsw25_core.lib wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\wxrcedit.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxrcedit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\wxrcedit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\wxrcedit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswd\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswd" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswd\wxrcedit.pdb /Od /GR /GX /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_lib\mswd" /W4 /I "." /I ".\..\..\..\samples" /I ".\..\..\include" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswd" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_lib\mswd" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d "NOPCH" /i .\..\..\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_xrc.lib wxmsw25d_adv.lib wxmsw25d_html.lib wxmsw25d_core.lib wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw25d_xrc.lib wxmsw25d_adv.lib wxmsw25d_html.lib wxmsw25d_core.lib wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\wxrcedit.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ENDIF

# Begin Target

# Name "wxrcedit - Win32 DLL Universal Unicode Release"
# Name "wxrcedit - Win32 DLL Universal Unicode Debug"
# Name "wxrcedit - Win32 DLL Universal Release"
# Name "wxrcedit - Win32 DLL Universal Debug"
# Name "wxrcedit - Win32 DLL Unicode Release"
# Name "wxrcedit - Win32 DLL Unicode Debug"
# Name "wxrcedit - Win32 DLL Release"
# Name "wxrcedit - Win32 DLL Debug"
# Name "wxrcedit - Win32 Universal Unicode Release"
# Name "wxrcedit - Win32 Universal Unicode Debug"
# Name "wxrcedit - Win32 Universal Release"
# Name "wxrcedit - Win32 Universal Debug"
# Name "wxrcedit - Win32 Unicode Release"
# Name "wxrcedit - Win32 Unicode Debug"
# Name "wxrcedit - Win32 Release"
# Name "wxrcedit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\edapp.cpp
# End Source File
# Begin Source File

SOURCE=.\editor.cpp
# End Source File
# Begin Source File

SOURCE=.\nodehnd.cpp
# End Source File
# Begin Source File

SOURCE=.\nodesdb.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_adv.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_basic.cpp
# End Source File
# Begin Source File

SOURCE=.\preview.cpp
# End Source File
# Begin Source File

SOURCE=.\propedit.cpp
# End Source File
# Begin Source File

SOURCE=.\propframe.cpp
# End Source File
# Begin Source File

SOURCE=.\splittree.cpp
# End Source File
# Begin Source File

SOURCE=.\wxrcedit.rc
# End Source File
# Begin Source File

SOURCE=.\xmlhelpr.cpp
# End Source File
# End Group
# End Target
# End Project

