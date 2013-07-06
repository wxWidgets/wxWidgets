# Microsoft Developer Studio Project File - Name="wx_xrc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xrc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_xrc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_xrc.mak" CFG="xrc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xrc - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrc - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xrc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "xrc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xrc - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_xrcdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw295u_xrc_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswudll\wxprec_xrcdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw295u_xrc_vc_custom.pdb /opt:ref /opt:icf /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw295u_xrc_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
# ADD RSC /l 0x409 /d "__WXMSW__" /d "NDEBUG" /d "_UNICODE" /i "..\..\lib\vc_dll\mswu" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw295u_xrc_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29u_html.lib ..\..\lib\vc_dll\wxmsw29u_adv.lib ..\..\lib\vc_dll\wxmsw29u_core.lib ..\..\lib\vc_dll\wxbase29u_xml.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw295u_xrc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29u_xrc.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw295u_xrc_vc_custom.pdb"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29u_html.lib ..\..\lib\vc_dll\wxmsw29u_adv.lib ..\..\lib\vc_dll\wxmsw29u_core.lib ..\..\lib\vc_dll\wxbase29u_xml.lib ..\..\lib\vc_dll\wxbase29u.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw295u_xrc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29u_xrc.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw295u_xrc_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "xrc - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_xrcdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw295ud_xrc_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /W4 /Yu"wx/wxprec.h" /Fp"vc_mswuddll\wxprec_xrcdll.pch" /Zi /Fd..\..\lib\vc_dll\wxmsw295ud_xrc_vc_custom.pdb /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /D "WXUSINGDLL" /D "WXMAKINGDLL_XRC" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw295ud_xrc_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
# ADD RSC /l 0x409 /d "_DEBUG" /d "__WXMSW__" /d "_UNICODE" /i "..\..\lib\vc_dll\mswud" /i "..\..\include" /d "WXBUILDING" /d WXDLLNAME=wxmsw295ud_xrc_vc_custom /i "..\..\src\tiff\libtiff" /i "..\..\src\jpeg" /i "..\..\src\png" /i "..\..\src\zlib" /i "..\..\src\regex" /i "..\..\src\expat\lib" /d "WXUSINGDLL" /d WXMAKINGDLL_XRC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29ud_html.lib ..\..\lib\vc_dll\wxmsw29ud_adv.lib ..\..\lib\vc_dll\wxmsw29ud_core.lib ..\..\lib\vc_dll\wxbase29ud_xml.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw295ud_xrc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29ud_xrc.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw295ud_xrc_vc_custom.pdb"
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wininet.lib ..\..\lib\vc_dll\wxmsw29ud_html.lib ..\..\lib\vc_dll\wxmsw29ud_adv.lib ..\..\lib\vc_dll\wxmsw29ud_core.lib ..\..\lib\vc_dll\wxbase29ud_xml.lib ..\..\lib\vc_dll\wxbase29ud.lib /nologo /dll /machine:i386 /out:"..\..\lib\vc_dll\wxmsw295ud_xrc_vc_custom.dll" /libpath:"..\..\lib\vc_dll" /implib:"..\..\lib\vc_dll\wxmsw29ud_xrc.lib" /debug /pdb:"..\..\lib\vc_dll\wxmsw295ud_xrc_vc_custom.pdb"

!ELSEIF  "$(CFG)" == "xrc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29u_xrc.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_xrclib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MD /O2 /GR /EHsc /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29u_xrc.pdb /opt:ref /opt:icf /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_xrclib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "NDEBUG" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29u_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29u_xrc.lib"

!ELSEIF  "$(CFG)" == "xrc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\xrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\xrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29ud_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_xrclib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD CPP /nologo /FD /MDd /Od /Gm /GR /EHsc /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /W4 /Zi /Fd..\..\lib\vc_lib\wxmsw29ud_xrc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_xrclib.pch" /I "..\..\src\tiff\libtiff" /I "..\..\src\jpeg" /I "..\..\src\png" /I "..\..\src\zlib" /I "..\..\src\regex" /I "..\..\src\expat\lib" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "_UNICODE" /D "WXBUILDING" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29ud_xrc.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxmsw29ud_xrc.lib"

!ENDIF

# Begin Target

# Name "xrc - Win32 DLL Release"
# Name "xrc - Win32 DLL Debug"
# Name "xrc - Win32 Release"
# Name "xrc - Win32 Debug"
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

!IF  "$(CFG)" == "xrc - Win32 DLL Release"


!ELSEIF  "$(CFG)" == "xrc - Win32 DLL Debug"


!ELSEIF  "$(CFG)" == "xrc - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "xrc - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# End Group
# Begin Group "Setup Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\setup.h

!IF  "$(CFG)" == "xrc - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "xrc - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "xrc - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "xrc - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswud\wx\setup.h

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\univ\setup.h

!IF  "$(CFG)" == "xrc - Win32 DLL Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "xrc - Win32 DLL Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "xrc - Win32 Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "xrc - Win32 Debug"

# Begin Custom Build - 

# End Custom Build

!ENDIF

# End Source File
# End Group
# Begin Group "MSW Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\genrcdefs.h

!IF  "$(CFG)" == "xrc - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "xrc - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "xrc - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "xrc - Win32 Debug"

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

SOURCE=..\..\include\wx\xrc\xh_all.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_animatctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_bannerwindow.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_bmp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_bmpbt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_bmpcbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_bttn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_cald.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_chckb.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_chckl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_choic.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_choicbk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_clrpicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_cmdlinkbn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_collpane.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_combo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_comboctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_datectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_dirpicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_dlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_editlbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_filectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_filepicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_fontpicker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_frame.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_gauge.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_gdctl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_grid.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_html.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_htmllbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_listb.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_listbk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_listc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_mdi.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_menu.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_notbk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_odcombo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_panel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_propdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_radbt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_radbx.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_scrol.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_scwin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_sizer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_slidr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_spin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_split.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_srchctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_statbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_stbmp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_stbox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_stlin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_sttxt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_text.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_tglbtn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_timectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_toolb.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_toolbk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_tree.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_treebk.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_unkwn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xh_wizrd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\xrc\xmlres.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\xrc\xh_animatctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_bannerwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_bmp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_bmpbt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_bmpcbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_bttn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_cald.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_chckb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_chckl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_choic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_choicbk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_clrpicker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_cmdlinkbn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_collpane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_combo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_comboctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_datectrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_dirpicker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_editlbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_filectrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_filepicker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_fontpicker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_frame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_gauge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_gdctl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_grid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_html.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_htmllbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_listb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_listbk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_listc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_mdi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_menu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_notbk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_odcombo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_panel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_propdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_radbt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_radbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_scrol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_scwin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_sizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_slidr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_spin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_split.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_srchctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_statbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_stbmp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_stbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_stlin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_sttxt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_text.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_tglbtn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_timectrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_toolb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_toolbk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_tree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_treebk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_unkwn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xh_wizrd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xmladv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xmlres.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\xrc\xmlrsall.cpp
# End Source File
# End Group
# End Target
# End Project

