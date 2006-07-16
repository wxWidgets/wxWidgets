# Microsoft Developer Studio Project File - Name="configtool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=wxconfigtool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "configtool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "configtool.mak" CFG="wxconfigtool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxconfigtool - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 DLL Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 DLL Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 DLL Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 DLL Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 DLL Release" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "wxconfigtool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxconfigtool - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivudll\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivu" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivudll\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivu" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswunivu" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswunivu" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26u_adv.lib wxmswuniv26u_html.lib wxmswuniv26u_core.lib wxbase26u_xml.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv26u_adv.lib wxmswuniv26u_html.lib wxmswuniv26u_core.lib wxbase26u_xml.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivudll\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivuddll\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivud" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivuddll\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivud" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswunivud" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswunivud" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26ud_adv.lib wxmswuniv26ud_html.lib wxmswuniv26ud_core.lib wxbase26ud_xml.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv26ud_adv.lib wxmswuniv26ud_html.lib wxmswuniv26ud_core.lib wxbase26ud_xml.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivdll\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswuniv" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivdll\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswuniv" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\lib\vc_dll\mswuniv" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\lib\vc_dll\mswuniv" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26_adv.lib wxmswuniv26_html.lib wxmswuniv26_core.lib wxbase26_xml.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv26_adv.lib wxmswuniv26_html.lib wxmswuniv26_core.lib wxbase26_xml.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivdll\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivddll\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivd" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivddll\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswunivd" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_dll\mswunivd" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_dll\mswunivd" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26d_adv.lib wxmswuniv26d_html.lib wxmswuniv26d_core.lib wxbase26d_xml.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmswuniv26d_adv.lib wxmswuniv26d_html.lib wxmswuniv26d_core.lib wxbase26d_xml.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivddll\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswudll\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswu" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswudll\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswu" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswu" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswu" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26u_adv.lib wxmsw26u_html.lib wxmsw26u_core.lib wxbase26u_xml.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw26u_adv.lib wxmsw26u_html.lib wxmsw26u_core.lib wxbase26u_xml.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswudll\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswuddll\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswud" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswuddll\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswud" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswud" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_dll\mswud" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26ud_adv.lib wxmsw26ud_html.lib wxmsw26ud_core.lib wxbase26ud_xml.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw26ud_adv.lib wxmsw26ud_html.lib wxmsw26ud_core.lib wxbase26ud_xml.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuddll\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswdll\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\msw" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswdll\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_dll\msw" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\..\lib\vc_dll\msw" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\..\lib\vc_dll\msw" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26_adv.lib wxmsw26_html.lib wxmsw26_core.lib wxbase26_xml.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw26_adv.lib wxmsw26_html.lib wxmsw26_core.lib wxbase26_xml.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswdll\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswddll\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswd" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswddll\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_dll\mswd" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_dll\mswd" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_dll\mswd" /i ".\..\..\..\include" /i "." /d "WXUSINGDLL" /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26d_adv.lib wxmsw26d_html.lib wxmsw26d_core.lib wxbase26d_xml.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows
# ADD LINK32 wxmsw26d_adv.lib wxmsw26d_html.lib wxmsw26d_core.lib wxbase26d_xml.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswddll\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_dll" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswunivu\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivu" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswunivu\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivu" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswunivu" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswunivu" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26u_adv.lib wxmswuniv26u_html.lib wxmswuniv26u_core.lib wxbase26u_xml.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv26u_adv.lib wxmswuniv26u_html.lib wxmswuniv26u_core.lib wxbase26u_xml.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivu\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivud\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivud" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivud\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivud" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswunivud" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswunivud" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26ud_adv.lib wxmswuniv26ud_html.lib wxmswuniv26ud_core.lib wxbase26ud_xml.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv26ud_adv.lib wxmswuniv26ud_html.lib wxmswuniv26ud_core.lib wxbase26ud_xml.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivud\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswuniv\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswuniv" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswuniv\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswuniv" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\lib\vc_lib\mswuniv" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\lib\vc_lib\mswuniv" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26_adv.lib wxmswuniv26_html.lib wxmswuniv26_core.lib wxbase26_xml.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv26_adv.lib wxmswuniv26_html.lib wxmswuniv26_core.lib wxbase26_xml.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswuniv\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivd\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivd" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswunivd\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswunivd" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_lib\mswunivd" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_lib\mswunivd" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26d_adv.lib wxmswuniv26d_html.lib wxmswuniv26d_core.lib wxbase26d_xml.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmswuniv26d_adv.lib wxmswuniv26d_html.lib wxmswuniv26d_core.lib wxbase26d_xml.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswunivd\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_mswu\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswu" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_mswu\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswu" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswu" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswu" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26u_adv.lib wxmsw26u_html.lib wxmsw26u_core.lib wxbase26u_xml.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw26u_adv.lib wxmsw26u_html.lib wxmsw26u_core.lib wxbase26u_xml.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswu\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswud\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswud" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswud\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswud" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswud" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i ".\..\..\..\lib\vc_lib\mswud" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26ud_adv.lib wxmsw26ud_html.lib wxmsw26ud_core.lib wxbase26ud_xml.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw26ud_adv.lib wxmsw26ud_html.lib wxmsw26ud_core.lib wxbase26ud_xml.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswud\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdvc_msw\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\msw" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MD /Fdvc_msw\wxconfigtool.pdb /O2 /GR /EHsc /I ".\..\..\..\lib\vc_lib\msw" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\..\lib\vc_lib\msw" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "__WXMSW__" /i ".\..\..\..\lib\vc_lib\msw" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26_adv.lib wxmsw26_html.lib wxmsw26_core.lib wxbase26_xml.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw26_adv.lib wxmsw26_html.lib wxmsw26_core.lib wxbase26_xml.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_msw\wxconfigtool.exe" /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ELSEIF  "$(CFG)" == "wxconfigtool - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\wxconfigtool"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\wxconfigtool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswd\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswd" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdvc_mswd\wxconfigtool.pdb /Od /GR /EHsc /I ".\..\..\..\lib\vc_lib\mswd" /I ".\..\..\..\include" /W4 /I "." /I ".\..\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_lib\mswd" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\lib\vc_lib\mswd" /i ".\..\..\..\include" /i "." /d "_WINDOWS" /i ".\..\..\..\samples" /d NOPCH
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26d_adv.lib wxmsw26d_html.lib wxmsw26d_core.lib wxbase26d_xml.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows
# ADD LINK32 wxmsw26d_adv.lib wxmsw26d_html.lib wxmsw26d_core.lib wxbase26d_xml.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /machine:i386 /out:"vc_mswd\wxconfigtool.exe" /debug /libpath:".\..\..\..\lib\vc_lib" /subsystem:windows

!ENDIF

# Begin Target

# Name "wxconfigtool - Win32 DLL Universal Unicode Release"
# Name "wxconfigtool - Win32 DLL Universal Unicode Debug"
# Name "wxconfigtool - Win32 DLL Universal Release"
# Name "wxconfigtool - Win32 DLL Universal Debug"
# Name "wxconfigtool - Win32 DLL Unicode Release"
# Name "wxconfigtool - Win32 DLL Unicode Debug"
# Name "wxconfigtool - Win32 DLL Release"
# Name "wxconfigtool - Win32 DLL Debug"
# Name "wxconfigtool - Win32 Universal Unicode Release"
# Name "wxconfigtool - Win32 Universal Unicode Debug"
# Name "wxconfigtool - Win32 Universal Release"
# Name "wxconfigtool - Win32 Universal Debug"
# Name "wxconfigtool - Win32 Unicode Release"
# Name "wxconfigtool - Win32 Unicode Debug"
# Name "wxconfigtool - Win32 Release"
# Name "wxconfigtool - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\appsettings.cpp
# End Source File
# Begin Source File

SOURCE=.\configitem.cpp
# End Source File
# Begin Source File

SOURCE=.\configitemselector.cpp
# End Source File
# Begin Source File

SOURCE=.\configtooldoc.cpp
# End Source File
# Begin Source File

SOURCE=.\configtoolview.cpp
# End Source File
# Begin Source File

SOURCE=.\configtree.cpp
# End Source File
# Begin Source File

SOURCE=.\custompropertydialog.cpp
# End Source File
# Begin Source File

SOURCE=.\htmlparser.cpp
# End Source File
# Begin Source File

SOURCE=.\mainframe.cpp
# End Source File
# Begin Source File

SOURCE=.\propeditor.cpp
# End Source File
# Begin Source File

SOURCE=.\property.cpp
# End Source File
# Begin Source File

SOURCE=.\..\..\..\samples\sample.rc
# End Source File
# Begin Source File

SOURCE=.\settingsdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\wxconfigtool.cpp
# End Source File
# End Group
# End Target
# End Project

