# Microsoft Developer Studio Project File - Name="plotdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plotdll - Win32 DLL Native Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plot_plotdll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plot_plotdll.mak" CFG="plotdll - Win32 DLL Native Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plotdll - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plotdll - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plotdll - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plotdll - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plotdll - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plotdll - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plotdll - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plotdll - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plotdll - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\plotdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\plotdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswuddll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_plotdll.pch" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswuddll\wxmsw250ud_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswuddll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_plotdll.pch" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswuddll\wxmsw250ud_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswuddll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswuddll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswuddll\wxmsw250ud_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswuddll\wxmsw25ud_plot.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswuddll\wxmsw250ud_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswuddll\wxmsw25ud_plot.lib" /debug

!ELSEIF  "$(CFG)" == "plotdll - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\plotdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\plotdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswdll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_plotdll.pch" /Fd..\..\src\plot\..\..\..\lib\vc_mswdll\wxmsw250_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswdll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswdll\wxprec_plotdll.pch" /Fd..\..\src\plot\..\..\..\lib\vc_mswdll\wxmsw250_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswdll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswdll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswdll\wxmsw250_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswdll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswdll\wxmsw25_plot.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswdll\wxmsw250_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswdll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswdll\wxmsw25_plot.lib"

!ELSEIF  "$(CFG)" == "plotdll - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\plotdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\plotdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_plotdll.pch" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivuddll\wxprec_plotdll.pch" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_plot.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_plot.lib" /debug

!ELSEIF  "$(CFG)" == "plotdll - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\plotdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\plotdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivudll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_plotdll.pch" /Fd..\..\src\plot\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivudll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivudll\wxprec_plotdll.pch" /Fd..\..\src\plot\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswunivudll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswunivudll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_plot.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_plot.lib"

!ELSEIF  "$(CFG)" == "plotdll - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\plotdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\plotdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswddll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_plotdll.pch" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswddll\wxmsw250d_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswddll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswddll\wxprec_plotdll.pch" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswddll\wxmsw250d_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswddll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswddll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswddll\wxmsw250d_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswddll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswddll\wxmsw25d_plot.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswddll\wxmsw250d_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswddll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswddll\wxmsw25d_plot.lib" /debug

!ELSEIF  "$(CFG)" == "plotdll - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\plotdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\plotdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivdll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_plotdll.pch" /Fd..\..\src\plot\..\..\..\lib\vc_mswunivdll\wxmswuniv250_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivdll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivdll\wxprec_plotdll.pch" /Fd..\..\src\plot\..\..\..\lib\vc_mswunivdll\wxmswuniv250_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswunivdll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswunivdll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswunivdll\wxmswuniv250_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswunivdll\wxmswuniv25_plot.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswunivdll\wxmswuniv250_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswunivdll\wxmswuniv25_plot.lib"

!ELSEIF  "$(CFG)" == "plotdll - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\plotdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\plotdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivddll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_plotdll.pch" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswunivddll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswunivddll\wxprec_plotdll.pch" /Zi /Gm /GZ /Fd..\..\src\plot\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswunivddll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswunivddll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_plot.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_plot.lib" /debug

!ELSEIF  "$(CFG)" == "plotdll - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\plotdll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\plot\..\..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\plotdll"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswudll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_plotdll.pch" /Fd..\..\src\plot\..\..\..\lib\vc_mswudll\wxmsw250u_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\plot\..\..\..\include" /I "..\..\src\plot\..\..\..\lib\vc_mswudll" /I "..\..\src\plot\..\..\..\src\tiff" /I "..\..\src\plot\..\..\..\src\jpeg" /I "..\..\src\plot\..\..\..\src\png" /I "..\..\src\plot\..\..\..\src\zlib" /I "..\..\src\plot\..\..\..\src\regex" /I "..\..\src\plot\..\..\..\src\expat\lib" /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_plotdll.pch" /Fd..\..\src\plot\..\..\..\lib\vc_mswudll\wxmsw250u_plot_vc_custom.pdb /I "..\..\src\plot\..\..\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_PLOT" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswudll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\plot\..\..\..\include" /i "..\..\src\plot\..\..\..\lib\vc_mswudll" /i "..\..\src\plot\..\..\..\src\tiff" /i "..\..\src\plot\..\..\..\src\jpeg" /i "..\..\src\plot\..\..\..\src\png" /i "..\..\src\plot\..\..\..\src\zlib" /i "..\..\src\plot\..\..\..\src\regex" /i "..\..\src\plot\..\..\..\src\expat\lib" /i "..\..\src\plot\..\..\include" /d "WXUSINGDLL" /d WXMAKINGDLL_PLOT
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswudll\wxmsw250u_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswudll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswudll\wxmsw25u_plot.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\plot\..\..\..\lib\vc_mswudll\wxmsw250u_plot_vc_custom.dll" /libpath:"..\..\src\plot\..\..\..\lib\vc_mswudll" /implib:"..\..\src\plot\..\..\..\lib\vc_mswudll\wxmsw25u_plot.lib"

!ENDIF

# Begin Target

# Name "plotdll - Win32 DLL Native Debug Unicode"
# Name "plotdll - Win32 DLL Native Release Ansi"
# Name "plotdll - Win32 DLL wxUniv Debug Unicode"
# Name "plotdll - Win32 DLL wxUniv Release Unicode"
# Name "plotdll - Win32 DLL Native Debug Ansi"
# Name "plotdll - Win32 DLL wxUniv Release Ansi"
# Name "plotdll - Win32 DLL wxUniv Debug Ansi"
# Name "plotdll - Win32 DLL Native Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/plot\..\..\..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=../../src/plot\plot.cpp
# End Source File
# End Group
# End Target
# End Project

