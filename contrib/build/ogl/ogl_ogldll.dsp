# Microsoft Developer Studio Project File - Name="ogldll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ogldll - Win32 DLL Native Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ogl_ogldll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ogl_ogldll.mak" CFG="ogldll - Win32 DLL Native Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ogldll - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogldll - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogldll - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogldll - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogldll - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogldll - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogldll - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogldll - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ogldll - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswuddll\"
# PROP BASE Intermediate_Dir "vc_mswuddll\ogldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswuddll\"
# PROP Intermediate_Dir "vc_mswuddll\ogldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswuddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw250ud_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswuddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw250ud_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswuddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswuddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw250ud_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw25ud_ogl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw250ud_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswuddll\wxmsw25ud_ogl.lib" /debug

!ELSEIF  "$(CFG)" == "ogldll - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswdll\"
# PROP BASE Intermediate_Dir "vc_mswdll\ogldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswdll\"
# PROP Intermediate_Dir "vc_mswdll\ogldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswdll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw250_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswdll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw250_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswdll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswdll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw250_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswdll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw25_ogl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw250_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswdll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswdll\wxmsw25_ogl.lib"

!ELSEIF  "$(CFG)" == "ogldll - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\ogldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\"
# PROP Intermediate_Dir "vc_mswunivuddll\ogldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_ogl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_ogl.lib" /debug

!ELSEIF  "$(CFG)" == "ogldll - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivudll\"
# PROP BASE Intermediate_Dir "vc_mswunivudll\ogldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivudll\"
# PROP Intermediate_Dir "vc_mswunivudll\ogldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_ogl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_ogl.lib"

!ELSEIF  "$(CFG)" == "ogldll - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswddll\"
# PROP BASE Intermediate_Dir "vc_mswddll\ogldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswddll\"
# PROP Intermediate_Dir "vc_mswddll\ogldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw250d_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw250d_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw250d_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw25d_ogl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw250d_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswddll\wxmsw25d_ogl.lib" /debug

!ELSEIF  "$(CFG)" == "ogldll - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivdll\"
# PROP BASE Intermediate_Dir "vc_mswunivdll\ogldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivdll\"
# PROP Intermediate_Dir "vc_mswunivdll\ogldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv25_ogl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv250_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivdll\wxmswuniv25_ogl.lib"

!ELSEIF  "$(CFG)" == "ogldll - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivddll\"
# PROP BASE Intermediate_Dir "vc_mswunivddll\ogldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswunivddll\"
# PROP Intermediate_Dir "vc_mswunivddll\ogldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_ogldll.pch" /Zi /Gm /GZ /Fd..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_ogl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_ogl.lib" /debug

!ELSEIF  "$(CFG)" == "ogldll - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswudll\"
# PROP BASE Intermediate_Dir "vc_mswudll\ogldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ogl\..\..\..\lib\vc_mswudll\"
# PROP Intermediate_Dir "vc_mswudll\ogldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswudll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw250u_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\ogl\..\..\..\include" /I "..\..\src\ogl\..\..\..\lib\vc_mswudll" /I "..\..\src\ogl\..\..\..\src\tiff" /I "..\..\src\ogl\..\..\..\src\jpeg" /I "..\..\src\ogl\..\..\..\src\png" /I "..\..\src\ogl\..\..\..\src\zlib" /I "..\..\src\ogl\..\..\..\src\regex" /I "..\..\src\ogl\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_ogldll.pch" /Fd..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw250u_ogl_vc_custom.pdb /I "..\..\src\ogl\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_OGL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswudll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\ogl\..\..\..\include" /i "..\..\src\ogl\..\..\..\lib\vc_mswudll" /i "..\..\src\ogl\..\..\..\src\tiff" /i "..\..\src\ogl\..\..\..\src\jpeg" /i "..\..\src\ogl\..\..\..\src\png" /i "..\..\src\ogl\..\..\..\src\zlib" /i "..\..\src\ogl\..\..\..\src\regex" /i "..\..\src\ogl\..\..\..\src\expat\lib" /i "..\..\src\ogl\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_OGL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw250u_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswudll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw25u_ogl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw250u_ogl_vc_custom.dll" /libpath:"..\..\src\ogl\..\..\..\lib\vc_mswudll" /implib:"..\..\src\ogl\..\..\..\lib\vc_mswudll\wxmsw25u_ogl.lib"

!ENDIF

# Begin Target

# Name "ogldll - Win32 DLL Native Debug Unicode"
# Name "ogldll - Win32 DLL Native Release Ansi"
# Name "ogldll - Win32 DLL wxUniv Debug Unicode"
# Name "ogldll - Win32 DLL wxUniv Release Unicode"
# Name "ogldll - Win32 DLL Native Debug Ansi"
# Name "ogldll - Win32 DLL wxUniv Release Ansi"
# Name "ogldll - Win32 DLL wxUniv Debug Ansi"
# Name "ogldll - Win32 DLL Native Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/ogl\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/ogl\basic.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\basic2.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\bmpshape.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\canvas.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\composit.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\constrnt.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\divided.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\drawn.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\lines.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\mfutils.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\ogldiag.cpp
# End Source File
# Begin Source File

SOURCE=../../src/ogl\oglmisc.cpp
# End Source File
# End Group
# End Target
# End Project

