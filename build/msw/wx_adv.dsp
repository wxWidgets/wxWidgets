# Microsoft Developer Studio Project File - Name="wx_adv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=adv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_adv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_adv.mak" CFG="adv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "adv - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "adv - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "adv - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "adv - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "adv - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "adv - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "adv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "adv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\adv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\adv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_advdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmswuniv293u_adv_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_advdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmswuniv293u_adv_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv293u_adv_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ADV
# ADD RSC /l 0x409 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv293u_adv_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ADV
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmswuniv29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv293u_adv_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29u_adv.lib" /debug /pdb:"..\..\lib\vc_dll\wxmswuniv293u_adv_vc_custom.pdb"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmswuniv29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv293u_adv_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29u_adv.lib" /debug /pdb:"..\..\lib\vc_dll\wxmswuniv293u_adv_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\adv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\adv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_advdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmswuniv293ud_adv_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_advdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmswuniv293ud_adv_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv293ud_adv_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ADV
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswunivud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmswuniv293ud_adv_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ADV
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmswuniv29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv293ud_adv_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29ud_adv.lib" /debug /pdb:"..\..\lib\vc_dll\wxmswuniv293ud_adv_vc_custom.pdb"
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmswuniv29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmswuniv293ud_adv_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmswuniv29ud_adv.lib" /debug /pdb:"..\..\lib\vc_dll\wxmswuniv293ud_adv_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\adv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\adv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_advdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw293u_adv_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_advdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw293u_adv_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw293u_adv_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ADV
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw293u_adv_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ADV
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw293u_adv_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29u_adv.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw293u_adv_vc_custom.pdb"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29u_core.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw293u_adv_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29u_adv.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw293u_adv_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\adv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\adv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_advdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw293ud_adv_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_advdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw293ud_adv_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_ADV" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw293ud_adv_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ADV
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw293ud_adv_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_ADV
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw293ud_adv_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29ud_adv.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw293ud_adv_vc_custom.pdb"
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29ud_core.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw293ud_adv_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29ud_adv.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw293ud_adv_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\adv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\adv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmswuniv29u_adv.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_advlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmswuniv29u_adv.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_advlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29u_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29u_adv.lib"

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\adv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\adv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmswuniv29ud_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_advlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmswuniv29ud_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_advlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29ud_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmswuniv29ud_adv.lib"

!ELSEIF  "$(CFG)" == "adv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\adv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\adv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29u_adv.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_advlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29u_adv.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_advlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29u_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29u_adv.lib"

!ELSEIF  "$(CFG)" == "adv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\adv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\adv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29ud_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_advlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29ud_adv.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_advlib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29ud_adv.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29ud_adv.lib"

!ENDIF

# Begin Target

# Name "adv - Win32 DLL Universal Release"
# Name "adv - Win32 DLL Universal Debug"
# Name "adv - Win32 DLL Release"
# Name "adv - Win32 DLL Debug"
# Name "adv - Win32 Universal Release"
# Name "adv - Win32 Universal Debug"
# Name "adv - Win32 Release"
# Name "adv - Win32 Debug"
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\common\animatecmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\bmpcboxcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\calctrlcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\datavcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=..\..\src\common\gridcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\hyperlnkcmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\odcombocmn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\common\taskbarcmn.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\msw\aboutdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\bmpcbox.cpp

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Release"


!ELSEIF  "$(CFG)" == "adv - Win32 Debug"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\src\msw\calctrl.cpp

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Release"


!ELSEIF  "$(CFG)" == "adv - Win32 Debug"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\src\msw\commandlinkbutton.cpp

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Release"


!ELSEIF  "$(CFG)" == "adv - Win32 Debug"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\src\msw\datecontrols.cpp

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Release"


!ELSEIF  "$(CFG)" == "adv - Win32 Debug"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\src\msw\datectrl.cpp

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Release"


!ELSEIF  "$(CFG)" == "adv - Win32 Debug"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\src\msw\hyperlink.cpp

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Release"


!ELSEIF  "$(CFG)" == "adv - Win32 Debug"


!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\src\msw\joystick.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\notifmsg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\sound.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\taskbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\version.rc

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"


!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"


!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "adv - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# Begin Group "Generic Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\generic\aboutdlgg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\animateg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\bannerwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\bmpcboxg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\calctrlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\commandlinkbuttong.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\datavgen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\datectlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\editlbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\grid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\gridctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\grideditors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\gridsel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\helpext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\hyperlinkg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\laywin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\notifmsgg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\odcombo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\propdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\sashwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\splash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\tipdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\generic\wizard.cpp
# End Source File
# End Group
# Begin Group "Setup Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\setup.h

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswud\wx\setup.h

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\univ\setup.h

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Debug"

# Begin Custom Build - 

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "MSW Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\calctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\commandlinkbutton.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\datectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\genrcdefs.h

!IF  "$(CFG)" == "adv - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "adv - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\joystick.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\notifmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\sound.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\taskbar.h
# End Source File
# End Group
# Begin Group "Generic Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\generic\aboutdlgg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\animate.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\calctrlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\dataview.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\datectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\dvrenderer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\dvrenderers.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\grid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\gridctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\grideditors.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\gridsel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\helpext.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\laywin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\notifmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\propdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\sashwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\splash.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\generic\wizard.h
# End Source File
# End Group
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\aboutdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\animate.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\bannerwindow.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\calctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\commandlinkbutton.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dataview.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\datectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dateevt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dcbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\dvrenderers.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\editlbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\grid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\joystick.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\laywin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\notifmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\odcombo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\propdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\sashwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\sound.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\splash.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\taskbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\tipdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\wizard.h
# End Source File
# End Group
# End Target
# End Project

