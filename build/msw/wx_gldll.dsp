# Microsoft Developer Studio Project File - Name="gldll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=gldll - Win32 DLL Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_gldll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_gldll.mak" CFG="gldll - Win32 DLL Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gldll - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gldll - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gldll - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gldll - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gldll - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gldll - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gldll - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gldll - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gldll - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\gldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\gldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_gldll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxmsw250d_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_gldll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswddll\wxmsw250d_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswddll\wxmsw250d_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswddll" /implib:"..\..\lib\vc_mswddll\wxmsw25d_gl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswddll\wxmsw250d_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswddll" /implib:"..\..\lib\vc_mswddll\wxmsw25d_gl.lib" /debug

!ELSEIF  "$(CFG)" == "gldll - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\gldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\gldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_gldll.pch" /Fd..\..\lib\vc_mswdll\wxmsw250_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_gldll.pch" /Fd..\..\lib\vc_mswdll\wxmsw250_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\include" /i "..\..\lib\vc_mswdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\include" /i "..\..\lib\vc_mswdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswdll\wxmsw250_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswdll" /implib:"..\..\lib\vc_mswdll\wxmsw25_gl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswdll\wxmsw250_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswdll" /implib:"..\..\lib\vc_mswdll\wxmsw25_gl.lib"

!ELSEIF  "$(CFG)" == "gldll - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\gldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\gldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_gldll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxmsw250ud_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_gldll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswuddll\wxmsw250ud_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswuddll\wxmsw250ud_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswuddll" /implib:"..\..\lib\vc_mswuddll\wxmsw25ud_gl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswuddll\wxmsw250ud_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswuddll" /implib:"..\..\lib\vc_mswuddll\wxmsw25ud_gl.lib" /debug

!ELSEIF  "$(CFG)" == "gldll - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\gldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\gldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_gldll.pch" /Fd..\..\lib\vc_mswudll\wxmsw250u_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_gldll.pch" /Fd..\..\lib\vc_mswudll\wxmsw250u_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswudll\wxmsw250u_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswudll" /implib:"..\..\lib\vc_mswudll\wxmsw25u_gl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswudll\wxmsw250u_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswudll" /implib:"..\..\lib\vc_mswudll\wxmsw25u_gl.lib"

!ELSEIF  "$(CFG)" == "gldll - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\gldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\gldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_gldll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxmswuniv250d_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_gldll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivddll\wxmswuniv250d_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswunivddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\include" /i "..\..\lib\vc_mswunivddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivddll\wxmswuniv250d_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivddll" /implib:"..\..\lib\vc_mswunivddll\wxmswuniv25d_gl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivddll\wxmswuniv250d_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivddll" /implib:"..\..\lib\vc_mswunivddll\wxmswuniv25d_gl.lib" /debug

!ELSEIF  "$(CFG)" == "gldll - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\gldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\gldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_gldll.pch" /Fd..\..\lib\vc_mswunivdll\wxmswuniv250_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivdll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_gldll.pch" /Fd..\..\lib\vc_mswunivdll\wxmswuniv250_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\include" /i "..\..\lib\vc_mswunivdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\include" /i "..\..\lib\vc_mswunivdll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivdll\wxmswuniv250_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivdll" /implib:"..\..\lib\vc_mswunivdll\wxmswuniv25_gl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivdll\wxmswuniv250_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivdll" /implib:"..\..\lib\vc_mswunivdll\wxmswuniv25_gl.lib"

!ELSEIF  "$(CFG)" == "gldll - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\gldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\gldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_gldll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxmswuniv250ud_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\include" /I "..\..\lib\vc_mswunivuddll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_gldll.pch" /Zi /Gm /GZ /Fd..\..\lib\vc_mswunivuddll\wxmswuniv250ud_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivuddll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivuddll\wxmswuniv250ud_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivuddll" /implib:"..\..\lib\vc_mswunivuddll\wxmswuniv25ud_gl.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivuddll\wxmswuniv250ud_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivuddll" /implib:"..\..\lib\vc_mswunivuddll\wxmswuniv25ud_gl.lib" /debug

!ELSEIF  "$(CFG)" == "gldll - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\gldll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\gldll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_gldll.pch" /Fd..\..\lib\vc_mswunivudll\wxmswuniv250u_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\include" /I "..\..\lib\vc_mswunivudll" /I "..\..\src\tiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_gldll.pch" /Fd..\..\lib\vc_mswunivudll\wxmswuniv250u_gl_vc_custom.pdb /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_GL" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\include" /i "..\..\lib\vc_mswunivudll" /i "..\..\src\tiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_GL
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivudll\wxmswuniv250u_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivudll" /implib:"..\..\lib\vc_mswunivudll\wxmswuniv25u_gl.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_mswunivudll\wxmswuniv250u_gl_vc_custom.dll" /libpath:"..\..\lib\vc_mswunivudll" /implib:"..\..\lib\vc_mswunivudll\wxmswuniv25u_gl.lib"

!ENDIF

# Begin Target

# Name "gldll - Win32 DLL Debug"
# Name "gldll - Win32 DLL Release"
# Name "gldll - Win32 DLL Unicode Debug"
# Name "gldll - Win32 DLL Unicode Release"
# Name "gldll - Win32 DLL Universal Debug"
# Name "gldll - Win32 DLL Universal Release"
# Name "gldll - Win32 DLL Universal Unicode Debug"
# Name "gldll - Win32 DLL Universal Unicode Release"
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=..\..\src\msw\glcanvas.cpp
# End Source File
# End Group
# Begin Group "MSW Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\glcanvas.h
# End Source File
# End Group
# End Target
# End Project

