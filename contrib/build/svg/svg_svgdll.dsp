# Microsoft Developer Studio Project File - Name="svgdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=svgdll - Win32 DLL Native Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "svg_svgdll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "svg_svgdll.mak" CFG="svgdll - Win32 DLL Native Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "svgdll - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svgdll - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svgdll - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svgdll - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svgdll - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svgdll - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svgdll - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "svgdll - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "svgdll - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswuddll\"
# PROP BASE Intermediate_Dir "vc_mswuddll\svgdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswuddll\"
# PROP Intermediate_Dir "vc_mswuddll\svgdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswuddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw250ud_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswuddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw250ud_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswuddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswuddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw250ud_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw25ud_svg.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw250ud_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswuddll\wxmsw25ud_svg.lib" /debug

!ELSEIF  "$(CFG)" == "svgdll - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswdll\"
# PROP BASE Intermediate_Dir "vc_mswdll\svgdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswdll\"
# PROP Intermediate_Dir "vc_mswdll\svgdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswdll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw250_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswdll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw250_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswdll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswdll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw250_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswdll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw25_svg.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw250_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswdll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswdll\wxmsw25_svg.lib"

!ELSEIF  "$(CFG)" == "svgdll - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivuddll\"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\svgdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivuddll\"
# PROP Intermediate_Dir "vc_mswunivuddll\svgdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_svg.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_svg.lib" /debug

!ELSEIF  "$(CFG)" == "svgdll - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivudll\"
# PROP BASE Intermediate_Dir "vc_mswunivudll\svgdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivudll\"
# PROP Intermediate_Dir "vc_mswunivudll\svgdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivudll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivudll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivudll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivudll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_svg.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_svg.lib"

!ELSEIF  "$(CFG)" == "svgdll - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswddll\"
# PROP BASE Intermediate_Dir "vc_mswddll\svgdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswddll\"
# PROP Intermediate_Dir "vc_mswddll\svgdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw250d_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw250d_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw250d_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw25d_svg.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw250d_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswddll\wxmsw25d_svg.lib" /debug

!ELSEIF  "$(CFG)" == "svgdll - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivdll\"
# PROP BASE Intermediate_Dir "vc_mswunivdll\svgdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivdll\"
# PROP Intermediate_Dir "vc_mswunivdll\svgdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivdll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv250_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivdll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv250_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivdll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivdll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv250_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv25_svg.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv250_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivdll\wxmswuniv25_svg.lib"

!ELSEIF  "$(CFG)" == "svgdll - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivddll\"
# PROP BASE Intermediate_Dir "vc_mswunivddll\svgdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswunivddll\"
# PROP Intermediate_Dir "vc_mswunivddll\svgdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswunivddll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_svgdll.pch" /Zi /Gm /GZ /Fd..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswunivddll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_svg.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_svg.lib" /debug

!ELSEIF  "$(CFG)" == "svgdll - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswudll\"
# PROP BASE Intermediate_Dir "vc_mswudll\svgdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\svg\..\..\..\lib\vc_mswudll\"
# PROP Intermediate_Dir "vc_mswudll\svgdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswudll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw250u_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\svg\..\..\..\include" /I "..\..\src\svg\..\..\..\lib\vc_mswudll" /I "..\..\src\svg\..\..\..\src\tiff" /I "..\..\src\svg\..\..\..\src\jpeg" /I "..\..\src\svg\..\..\..\src\png" /I "..\..\src\svg\..\..\..\src\zlib" /I "..\..\src\svg\..\..\..\src\regex" /I "..\..\src\svg\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_svgdll.pch" /Fd..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw250u_svg_vc_custom.pdb /I "..\..\src\svg\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_SVG" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswudll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\svg\..\..\..\include" /i "..\..\src\svg\..\..\..\lib\vc_mswudll" /i "..\..\src\svg\..\..\..\src\tiff" /i "..\..\src\svg\..\..\..\src\jpeg" /i "..\..\src\svg\..\..\..\src\png" /i "..\..\src\svg\..\..\..\src\zlib" /i "..\..\src\svg\..\..\..\src\regex" /i "..\..\src\svg\..\..\..\src\expat\lib" /i "..\..\src\svg\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_SVG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw250u_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswudll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw25u_svg.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw250u_svg_vc_custom.dll" /libpath:"..\..\src\svg\..\..\..\lib\vc_mswudll" /implib:"..\..\src\svg\..\..\..\lib\vc_mswudll\wxmsw25u_svg.lib"

!ENDIF

# Begin Target

# Name "svgdll - Win32 DLL Native Debug Unicode"
# Name "svgdll - Win32 DLL Native Release Ansi"
# Name "svgdll - Win32 DLL wxUniv Debug Unicode"
# Name "svgdll - Win32 DLL wxUniv Release Unicode"
# Name "svgdll - Win32 DLL Native Debug Ansi"
# Name "svgdll - Win32 DLL wxUniv Release Ansi"
# Name "svgdll - Win32 DLL wxUniv Debug Ansi"
# Name "svgdll - Win32 DLL Native Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/svg\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/svg\dcsvg.cpp
# End Source File
# End Group
# End Target
# End Project

