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
!MESSAGE "richtext - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "richtext - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "richtext - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "richtext - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "richtext - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "richtext - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_richtextdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmswuniv293u_richtext_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_richtextdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmswuniv293u_richtext_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv293u_richtext_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv293u_richtext_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmswuniv29u_adv.lib ..\..\lib\vc_dll\wxmswuniv29u_html.lib ..\..\lib\vc_dll\wxbase29u_xml.lib ..\..\lib\vc_dll\wxmswuniv29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv293u_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29u_richtext.lib" /debug /pdb:"..\..\lib\vc_dll\wxmswuniv293u_richtext_vc_custom.pdb"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmswuniv29u_adv.lib ..\..\lib\vc_dll\wxmswuniv29u_html.lib ..\..\lib\vc_dll\wxbase29u_xml.lib ..\..\lib\vc_dll\wxmswuniv29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv293u_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29u_richtext.lib" /debug /pdb:"..\..\lib\vc_dll\wxmswuniv293u_richtext_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Debug"

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
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_richtextdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmswuniv293ud_richtext_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_richtextdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmswuniv293ud_richtext_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv293ud_richtext_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv293ud_richtext_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmswuniv29ud_adv.lib ..\..\lib\vc_dll\wxmswuniv29ud_html.lib ..\..\lib\vc_dll\wxbase29ud_xml.lib ..\..\lib\vc_dll\wxmswuniv29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv293ud_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29ud_richtext.lib" /debug /pdb:"..\..\lib\vc_dll\wxmswuniv293ud_richtext_vc_custom.pdb"
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmswuniv29ud_adv.lib ..\..\lib\vc_dll\wxmswuniv29ud_html.lib ..\..\lib\vc_dll\wxbase29ud_xml.lib ..\..\lib\vc_dll\wxmswuniv29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv293ud_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29ud_richtext.lib" /debug /pdb:"..\..\lib\vc_dll\wxmswuniv293ud_richtext_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_richtextdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw293u_richtext_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_richtextdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw293u_richtext_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw293u_richtext_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw293u_richtext_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29u_adv.lib ..\..\lib\vc_dll\wxmsw29u_html.lib ..\..\lib\vc_dll\wxbase29u_xml.lib ..\..\lib\vc_dll\wxmsw29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw293u_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29u_richtext.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw293u_richtext_vc_custom.pdb"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29u_adv.lib ..\..\lib\vc_dll\wxmsw29u_html.lib ..\..\lib\vc_dll\wxbase29u_xml.lib ..\..\lib\vc_dll\wxmsw29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw293u_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29u_richtext.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw293u_richtext_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Debug"

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
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_richtextdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw293ud_richtext_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_richtextdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw293ud_richtext_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_RICHTEXT" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw293ud_richtext_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw293ud_richtext_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_RICHTEXT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29ud_adv.lib ..\..\lib\vc_dll\wxmsw29ud_html.lib ..\..\lib\vc_dll\wxbase29ud_xml.lib ..\..\lib\vc_dll\wxmsw29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw293ud_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29ud_richtext.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw293ud_richtext_vc_custom.pdb"
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29ud_adv.lib ..\..\lib\vc_dll\wxmsw29ud_html.lib ..\..\lib\vc_dll\wxbase29ud_xml.lib ..\..\lib\vc_dll\wxmsw29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw293ud_richtext_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29ud_richtext.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw293ud_richtext_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmswuniv29u_richtext.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_richtextlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmswuniv29u_richtext.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_richtextlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29u_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29u_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Debug"

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
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmswuniv29ud_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_richtextlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmswuniv29ud_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_richtextlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29ud_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29ud_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\richtext"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\richtext"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29u_richtext.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_richtextlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29u_richtext.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_richtextlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29u_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29u_richtext.lib"

!ELSEIF  "$(CFG)" == "richtext - Win32 Debug"

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
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29ud_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_richtextlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29ud_richtext.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_richtextlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29ud_richtext.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29ud_richtext.lib"

!ENDIF

# Begin Target

# Name "richtext - Win32 DLL Universal Release"
# Name "richtext - Win32 DLL Universal Debug"
# Name "richtext - Win32 DLL Release"
# Name "richtext - Win32 DLL Debug"
# Name "richtext - Win32 Universal Release"
# Name "richtext - Win32 Universal Debug"
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

!IF  "$(CFG)" == "richtext - Win32 DLL Universal Release"


!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Debug"


!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Debug"

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

SOURCE=..\..\include\wx\msw\setup.h

!IF  "$(CFG)" == "richtext - Win32 DLL Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswud\wx\setup.h

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\univ\setup.h

!IF  "$(CFG)" == "richtext - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Debug"

# Begin Custom Build - 

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "MSW Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\genrcdefs.h

!IF  "$(CFG)" == "richtext - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "richtext - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextbackgroundpage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextborderspage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextbulletspage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextdialogpage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextfontpage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextformatdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtexthtml.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextimagedlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextindentspage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextliststylepage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextmarginspage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextprint.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextsizepage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextstyledlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextstylepage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextstyles.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextsymboldlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\richtext\richtextuicustomization.h
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

SOURCE=..\..\src\richtext\richtextimagedlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\richtext\richtextprint.cpp
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

