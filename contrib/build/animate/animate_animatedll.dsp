# Microsoft Developer Studio Project File - Name="animatedll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=animatedll - Win32 DLL Native Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "animate_animatedll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "animate_animatedll.mak" CFG="animatedll - Win32 DLL Native Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "animatedll - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "animatedll - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "animatedll - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "animatedll - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "animatedll - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "animatedll - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "animatedll - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "animatedll - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "animatedll - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswuddll\"
# PROP BASE Intermediate_Dir "vc_mswuddll\animatedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswuddll\"
# PROP Intermediate_Dir "vc_mswuddll\animatedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswuddll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_animatedll.pch" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswuddll\wxmsw250ud_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswuddll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_animatedll.pch" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswuddll\wxmsw250ud_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswuddll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswuddll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswuddll\wxmsw250ud_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswuddll\wxmsw25ud_animate.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswuddll\wxmsw250ud_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswuddll\wxmsw25ud_animate.lib" /debug

!ELSEIF  "$(CFG)" == "animatedll - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswdll\"
# PROP BASE Intermediate_Dir "vc_mswdll\animatedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswdll\"
# PROP Intermediate_Dir "vc_mswdll\animatedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswdll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_animatedll.pch" /Fd..\..\src\animate\..\..\..\lib\vc_mswdll\wxmsw250_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswdll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_animatedll.pch" /Fd..\..\src\animate\..\..\..\lib\vc_mswdll\wxmsw250_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswdll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswdll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswdll\wxmsw250_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswdll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswdll\wxmsw25_animate.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswdll\wxmsw250_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswdll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswdll\wxmsw25_animate.lib"

!ELSEIF  "$(CFG)" == "animatedll - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivuddll\"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\animatedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivuddll\"
# PROP Intermediate_Dir "vc_mswunivuddll\animatedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_animatedll.pch" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_animatedll.pch" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_animate.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_animate.lib" /debug

!ELSEIF  "$(CFG)" == "animatedll - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivudll\"
# PROP BASE Intermediate_Dir "vc_mswunivudll\animatedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivudll\"
# PROP Intermediate_Dir "vc_mswunivudll\animatedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivudll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_animatedll.pch" /Fd..\..\src\animate\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivudll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_animatedll.pch" /Fd..\..\src\animate\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswunivudll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswunivudll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_animate.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_animate.lib"

!ELSEIF  "$(CFG)" == "animatedll - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswddll\"
# PROP BASE Intermediate_Dir "vc_mswddll\animatedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswddll\"
# PROP Intermediate_Dir "vc_mswddll\animatedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswddll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_animatedll.pch" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswddll\wxmsw250d_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswddll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_animatedll.pch" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswddll\wxmsw250d_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswddll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswddll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswddll\wxmsw250d_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswddll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswddll\wxmsw25d_animate.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswddll\wxmsw250d_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswddll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswddll\wxmsw25d_animate.lib" /debug

!ELSEIF  "$(CFG)" == "animatedll - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivdll\"
# PROP BASE Intermediate_Dir "vc_mswunivdll\animatedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivdll\"
# PROP Intermediate_Dir "vc_mswunivdll\animatedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivdll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_animatedll.pch" /Fd..\..\src\animate\..\..\..\lib\vc_mswunivdll\wxmswuniv250_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivdll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_animatedll.pch" /Fd..\..\src\animate\..\..\..\lib\vc_mswunivdll\wxmswuniv250_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswunivdll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswunivdll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswunivdll\wxmswuniv250_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswunivdll\wxmswuniv25_animate.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswunivdll\wxmswuniv250_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswunivdll\wxmswuniv25_animate.lib"

!ELSEIF  "$(CFG)" == "animatedll - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivddll\"
# PROP BASE Intermediate_Dir "vc_mswunivddll\animatedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswunivddll\"
# PROP Intermediate_Dir "vc_mswunivddll\animatedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivddll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_animatedll.pch" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswunivddll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_animatedll.pch" /Zi /Gm /GZ /Fd..\..\src\animate\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswunivddll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswunivddll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_animate.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_animate.lib" /debug

!ELSEIF  "$(CFG)" == "animatedll - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswudll\"
# PROP BASE Intermediate_Dir "vc_mswudll\animatedll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\animate\..\..\..\lib\vc_mswudll\"
# PROP Intermediate_Dir "vc_mswudll\animatedll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswudll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_animatedll.pch" /Fd..\..\src\animate\..\..\..\lib\vc_mswudll\wxmsw250u_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\animate\..\..\..\include" /I "..\..\src\animate\..\..\..\lib\vc_mswudll" /I "..\..\src\animate\..\..\..\src\tiff" /I "..\..\src\animate\..\..\..\src\jpeg" /I "..\..\src\animate\..\..\..\src\png" /I "..\..\src\animate\..\..\..\src\zlib" /I "..\..\src\animate\..\..\..\src\regex" /I "..\..\src\animate\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_animatedll.pch" /Fd..\..\src\animate\..\..\..\lib\vc_mswudll\wxmsw250u_animate_vc_custom.pdb /I "..\..\src\animate\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_ANIMATE" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswudll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\animate\..\..\..\include" /i "..\..\src\animate\..\..\..\lib\vc_mswudll" /i "..\..\src\animate\..\..\..\src\tiff" /i "..\..\src\animate\..\..\..\src\jpeg" /i "..\..\src\animate\..\..\..\src\png" /i "..\..\src\animate\..\..\..\src\zlib" /i "..\..\src\animate\..\..\..\src\regex" /i "..\..\src\animate\..\..\..\src\expat\lib" /i "..\..\src\animate\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_ANIMATE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswudll\wxmsw250u_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswudll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswudll\wxmsw25u_animate.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\animate\..\..\..\lib\vc_mswudll\wxmsw250u_animate_vc_custom.dll" /libpath:"..\..\src\animate\..\..\..\lib\vc_mswudll" /implib:"..\..\src\animate\..\..\..\lib\vc_mswudll\wxmsw25u_animate.lib"

!ENDIF

# Begin Target

# Name "animatedll - Win32 DLL Native Debug Unicode"
# Name "animatedll - Win32 DLL Native Release Ansi"
# Name "animatedll - Win32 DLL wxUniv Debug Unicode"
# Name "animatedll - Win32 DLL wxUniv Release Unicode"
# Name "animatedll - Win32 DLL Native Debug Ansi"
# Name "animatedll - Win32 DLL wxUniv Release Ansi"
# Name "animatedll - Win32 DLL wxUniv Debug Ansi"
# Name "animatedll - Win32 DLL Native Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/animate\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/animate\animate.cpp
# End Source File
# End Group
# End Target
# End Project

