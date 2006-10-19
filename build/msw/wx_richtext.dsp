# Microsoft Developer Studio Project File - Name="wx_richtext" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=richtext - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_richtext.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_richtext.mak" CFG="richtext - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "richtext - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "richtext - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "richtext - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "richtext - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "richtext - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "richtext - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "richtext - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "richtext - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "richtext - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_richtextdll.pch" /Fd..\..\lib\vc_dll\wxmswuniv271u_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_richtextdll.pch" /Fd..\..\lib\vc_dll\wxmswuniv271u_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivu" /i "..\..\include" /d WXDLLNAME=wxmswuniv271u_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivu" /i "..\..\include" /d WXDLLNAME=wxmswuniv271u_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmswuniv27u_adv.lib ..\..\lib\vc_dll\wxmswuniv27u_html.lib ..\..\lib\vc_dll\wxbase27u_xml.lib ..\..\lib\vc_dll\wxmswuniv27u_core.lib ..\..\lib\vc_dll\wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv271u_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv27u_richtext.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmswuniv27u_adv.lib ..\..\lib\vc_dll\wxmswuniv27u_html.lib ..\..\lib\vc_dll\wxbase27u_xml.lib ..\..\lib\vc_dll\wxmswuniv27u_core.lib ..\..\lib\vc_dll\wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv271u_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv27u_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_richtextdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmswuniv271ud_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_richtextdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmswuniv271ud_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivud" /i "..\..\include" /d WXDLLNAME=wxmswuniv271ud_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivud" /i "..\..\include" /d WXDLLNAME=wxmswuniv271ud_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmswuniv27ud_adv.lib ..\..\lib\vc_dll\wxmswuniv27ud_html.lib ..\..\lib\vc_dll\wxbase27ud_xml.lib ..\..\lib\vc_dll\wxmswuniv27ud_core.lib ..\..\lib\vc_dll\wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv271ud_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv27ud_richtext.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmswuniv27ud_adv.lib ..\..\lib\vc_dll\wxmswuniv27ud_html.lib ..\..\lib\vc_dll\wxbase27ud_xml.lib ..\..\lib\vc_dll\wxmswuniv27ud_core.lib ..\..\lib\vc_dll\wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv271ud_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv27ud_richtext.lib" /debug

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivdll\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswuniv" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_richtextdll.pch" /Fd..\..\lib\vc_dll\wxmswuniv271_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswuniv" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_richtextdll.pch" /Fd..\..\lib\vc_dll\wxmswuniv271_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\lib\vc_dll\mswuniv" /i "..\..\include" /d WXDLLNAME=wxmswuniv271_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\lib\vc_dll\mswuniv" /i "..\..\include" /d WXDLLNAME=wxmswuniv271_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmswuniv27_adv.lib ..\..\lib\vc_dll\wxmswuniv27_html.lib ..\..\lib\vc_dll\wxbase27_xml.lib ..\..\lib\vc_dll\wxmswuniv27_core.lib ..\..\lib\vc_dll\wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv271_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv27_richtext.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmswuniv27_adv.lib ..\..\lib\vc_dll\wxmswuniv27_html.lib ..\..\lib\vc_dll\wxbase27_xml.lib ..\..\lib\vc_dll\wxmswuniv27_core.lib ..\..\lib\vc_dll\wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv271_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv27_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivddll\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_richtextdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmswuniv271d_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswunivd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_richtextdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmswuniv271d_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswunivd" /i "..\..\include" /d WXDLLNAME=wxmswuniv271d_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswunivd" /i "..\..\include" /d WXDLLNAME=wxmswuniv271d_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmswuniv27d_adv.lib ..\..\lib\vc_dll\wxmswuniv27d_html.lib ..\..\lib\vc_dll\wxbase27d_xml.lib ..\..\lib\vc_dll\wxmswuniv27d_core.lib ..\..\lib\vc_dll\wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv271d_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv27d_richtext.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmswuniv27d_adv.lib ..\..\lib\vc_dll\wxmswuniv27d_html.lib ..\..\lib\vc_dll\wxbase27d_xml.lib ..\..\lib\vc_dll\wxmswuniv27d_core.lib ..\..\lib\vc_dll\wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv271d_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv27d_richtext.lib" /debug

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_richtextdll.pch" /Fd..\..\lib\vc_dll\wxmsw271u_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_richtextdll.pch" /Fd..\..\lib\vc_dll\wxmsw271u_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d WXDLLNAME=wxmsw271u_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d WXDLLNAME=wxmsw271u_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmsw27u_adv.lib ..\..\lib\vc_dll\wxmsw27u_html.lib ..\..\lib\vc_dll\wxbase27u_xml.lib ..\..\lib\vc_dll\wxmsw27u_core.lib ..\..\lib\vc_dll\wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw271u_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw27u_richtext.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmsw27u_adv.lib ..\..\lib\vc_dll\wxmsw27u_html.lib ..\..\lib\vc_dll\wxbase27u_xml.lib ..\..\lib\vc_dll\wxmsw27u_core.lib ..\..\lib\vc_dll\wxbase27u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw271u_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw27u_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_richtextdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmsw271ud_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_richtextdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmsw271ud_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d WXDLLNAME=wxmsw271ud_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d WXDLLNAME=wxmsw271ud_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmsw27ud_adv.lib ..\..\lib\vc_dll\wxmsw27ud_html.lib ..\..\lib\vc_dll\wxbase27ud_xml.lib ..\..\lib\vc_dll\wxmsw27ud_core.lib ..\..\lib\vc_dll\wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw271ud_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw27ud_richtext.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmsw27ud_adv.lib ..\..\lib\vc_dll\wxmsw27ud_html.lib ..\..\lib\vc_dll\wxbase27ud_xml.lib ..\..\lib\vc_dll\wxmsw27ud_core.lib ..\..\lib\vc_dll\wxbase27ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw271ud_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw27ud_richtext.lib" /debug

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswdll\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswdll\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\msw" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_richtextdll.pch" /Fd..\..\lib\vc_dll\wxmsw271_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\msw" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_richtextdll.pch" /Fd..\..\lib\vc_dll\wxmsw271_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i "..\..\lib\vc_dll\msw" /i "..\..\include" /d WXDLLNAME=wxmsw271_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "__WXMSW__" /i "..\..\lib\vc_dll\msw" /i "..\..\include" /d WXDLLNAME=wxmsw271_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmsw27_adv.lib ..\..\lib\vc_dll\wxmsw27_html.lib ..\..\lib\vc_dll\wxbase27_xml.lib ..\..\lib\vc_dll\wxmsw27_core.lib ..\..\lib\vc_dll\wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw271_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw27_richtext.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmsw27_adv.lib ..\..\lib\vc_dll\wxmsw27_html.lib ..\..\lib\vc_dll\wxbase27_xml.lib ..\..\lib\vc_dll\wxmsw27_core.lib ..\..\lib\vc_dll\wxbase27.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw271_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw27_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswddll\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswddll\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_richtextdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmsw271d_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_dll\mswd" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_richtextdll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_dll\wxmsw271d_richtext_vc_custom.pdb /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswd" /i "..\..\include" /d WXDLLNAME=wxmsw271d_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\lib\vc_dll\mswd" /i "..\..\include" /d WXDLLNAME=wxmsw271d_richtext_vc_custom /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmsw27d_adv.lib ..\..\lib\vc_dll\wxmsw27d_html.lib ..\..\lib\vc_dll\wxbase27d_xml.lib ..\..\lib\vc_dll\wxmsw27d_core.lib ..\..\lib\vc_dll\wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw271d_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw27d_richtext.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib ..\..\lib\vc_dll\wxmsw27d_adv.lib ..\..\lib\vc_dll\wxmsw27d_html.lib ..\..\lib\vc_dll\wxbase27d_xml.lib ..\..\lib\vc_dll\wxmsw27d_core.lib ..\..\lib\vc_dll\wxbase27d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw271d_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw27d_richtext.lib" /debug

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmswuniv27u_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmswuniv27u_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv27u_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv27u_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmswuniv27ud_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmswuniv27ud_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv27ud_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv27ud_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswuniv\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswuniv\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswuniv" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmswuniv27_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswuniv" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmswuniv27_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv27_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv27_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivd\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivd\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmswuniv27d_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswunivd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmswuniv27d_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv27d_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv27d_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmsw27u_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmsw27u_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw27u_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw27u_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmsw27ud_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmsw27ud_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw27ud_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw27ud_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_msw\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_msw\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\msw" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmsw27_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\msw" /I "..\..\include" /W4 /Fd..\..\lib\vc_lib\wxmsw27_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw27_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw27_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswd\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswd\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmsw27d_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /MDd /Od /GR /EHsc /I "..\..\lib\vc_lib\mswd" /I "..\..\include" /W4 /Zi /Gm /GZ /Fd..\..\lib\vc_lib\wxmsw27d_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_richtextlib.pch" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw27d_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw27d_richtext.lib"

!ENDIF

# Begin Target

# Name "richtext - Win32 DLL Universal Unicode Release"
# Name "richtext - Win32 DLL Universal Unicode Debug"
# Name "richtext - Win32 DLL Universal Release"
# Name "richtext - Win32 DLL Universal Debug"
# Name "richtext - Win32 DLL Unicode Release"
# Name "richtext - Win32 DLL Unicode Debug"
# Name "richtext - Win32 DLL Release"
# Name "richtext - Win32 DLL Debug"
# Name "richtext - Win32 Universal Unicode Release"
# Name "richtext - Win32 Universal Unicode Debug"
# Name "richtext - Win32 Universal Release"
# Name "richtext - Win32 Universal Debug"
# Name "richtext - Win32 Unicode Release"
# Name "richtext - Win32 Unicode Debug"
# Name "richtext - Win32 Release"
# Name "richtext - Win32 Debug"
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\common\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\msw\version.rc

!IF  "$(CFG)" == "richtext - Win32 DLL Universal Unicode Release"


!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Unicode Debug"


!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Release"


!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Debug"


!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Unicode Release"


!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Unicode Debug"


!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "richtext - Win32 Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "richtext - Win32 Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "richtext - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "richtext - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# Begin Group "Setup Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\univ\setup.h

!IF  "$(CFG)" == "richtext - Win32 DLL Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswuniv\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswuniv\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswuniv\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivd\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivd\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswuniv\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswuniv\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswuniv\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivd\wx\setup.h
InputPath=..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivd\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Debug"

# Begin Custom Build - 

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\setup.h

!IF  "$(CFG)" == "richtext - Win32 DLL Universal Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\msw\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_dll\msw\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\msw\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswd\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswd\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Unicode Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Unicode Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\msw\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_lib\msw\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\msw\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswd\wx\setup.h
InputPath=..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswd\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswd\wx\setup.h

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "MSW Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\genrcdefs.h

!IF  "$(CFG)" == "richtext - Win32 DLL Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswuniv\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswuniv\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswuniv\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivd\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswunivd\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivd\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\msw\wx\msw\rcdefs.h

"..\..\lib\vc_dll\msw\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\msw\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswd\wx\msw\rcdefs.h

"..\..\lib\vc_dll\mswd\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswd\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswuniv\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswuniv\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswuniv\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivd\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswunivd\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivd\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Unicode Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Unicode Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\msw\wx\msw\rcdefs.h

"..\..\lib\vc_lib\msw\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\msw\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswd\wx\msw\rcdefs.h

"..\..\lib\vc_lib\mswd\wx\msw\rcdefs.h" : "$(SOURCE)" "$(SETUPHDIR)\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswd\wx\msw\rcdefs.h"

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextformatdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextstyledlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextstyles.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextsymboldlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextxml.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\richtext\richtextbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\richtext\richtextctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\richtext\richtextformatdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\richtext\richtexthtml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\richtext\richtextstyledlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\richtext\richtextstyles.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\richtext\richtextsymboldlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\richtext\richtextxml.cpp
# End Source File
# End Group
# End Target
# End Project

