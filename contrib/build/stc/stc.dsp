# Microsoft Developer Studio Project File - Name="stc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=stc - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stc.mak" CFG="stc - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Universal Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 Universal Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stc - Win32 DLL Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Universal Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Universal Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stc - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswd"
# PROP Intermediate_Dir "vc_mswd\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswd" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswd\wxmsw25d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswd" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswd\wxmsw25d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswd\wxmsw25d_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswd\wxmsw25d_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_msw"
# PROP Intermediate_Dir "vc_msw\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_msw" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_msw\wxmsw25_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_msw" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_msw\wxmsw25_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_msw\wxmsw25_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_msw\wxmsw25_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswud"
# PROP Intermediate_Dir "vc_mswud\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswud" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswud\wxmsw25ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswud" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswud\wxmsw25ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswud\wxmsw25ud_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswud\wxmsw25ud_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswu"
# PROP Intermediate_Dir "vc_mswu\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswu" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswu\wxmsw25u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswu" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswu\wxmsw25u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswu\wxmsw25u_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswu\wxmsw25u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivd" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivd" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswuniv" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswuniv\wxmswuniv25_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswuniv" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswuniv\wxmswuniv25_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswuniv\wxmswuniv25_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswuniv\wxmswuniv25_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivud" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivud" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivu" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivu" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswddll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswddll\wxmsw250d_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswddll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswddll\wxmsw250d_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswddll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswddll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswddll\wxmsw250d_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswddll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswddll\wxmsw25d_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswddll\wxmsw250d_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswddll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswddll\wxmsw25d_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswdll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswdll\wxmsw250_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswdll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswdll\wxmsw250_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswdll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x405 /d "__WXMSW__" /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswdll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswdll\wxmsw250_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswdll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswdll\wxmsw25_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswdll\wxmsw250_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswdll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswdll\wxmsw25_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswuddll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswuddll\wxmsw250ud_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswuddll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswuddll\wxmsw250ud_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswuddll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswuddll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswuddll\wxmsw250ud_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswuddll\wxmsw25ud_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswuddll\wxmsw250ud_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswuddll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswuddll\wxmsw25ud_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswudll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswudll\wxmsw250u_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswudll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswudll\wxmsw250u_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswudll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswudll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswudll\wxmsw250u_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswudll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswudll\wxmsw25u_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmsw25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswudll\wxmsw250u_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswudll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswudll\wxmsw25u_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivddll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivddll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswunivddll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswunivddll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25d_core.lib wxbase25d.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswunivddll\wxmswuniv250d_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswunivddll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswunivddll\wxmswuniv25d_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivdll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswunivdll\wxmswuniv250_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivdll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswunivdll\wxmswuniv250_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswunivdll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswunivdll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswunivdll\wxmswuniv250_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswunivdll\wxmswuniv25_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25_core.lib wxbase25.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswunivdll\wxmswuniv250_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswunivdll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswunivdll\wxmswuniv25_stc.lib"

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivuddll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswunivuddll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_stc.lib" /debug
# ADD LINK32 wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25ud_core.lib wxbase25ud.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswunivuddll\wxmswuniv250ud_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswunivuddll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswunivuddll\wxmswuniv25ud_stc.lib" /debug

!ELSEIF  "$(CFG)" == "stc - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\stc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\stc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivudll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivudll" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_stc_vc_custom.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "WXMAKINGDLL_STC" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswunivudll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i "..\..\src\stc\..\..\..\include" /i "..\..\src\stc\..\..\..\lib\vc_mswunivudll" /i "..\..\src\stc\..\..\..\src\tiff" /i "..\..\src\stc\..\..\..\src\jpeg" /i "..\..\src\stc\..\..\..\src\png" /i "..\..\src\stc\..\..\..\src\zlib" /i "..\..\src\stc\..\..\..\src\regex" /i "..\..\src\stc\..\..\..\src\expat\lib" /i "..\..\src\stc\..\..\include" /i "..\..\src\stc\scintilla\include" /i "..\..\src\stc\scintilla\src" /d "__WX__" /d "SCI_LEXER" /d "LINK_LEXERS" /d "WXUSINGDLL" /d WXMAKINGDLL_STC
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_stc.lib"
# ADD LINK32 wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib wxmswuniv25u_core.lib wxbase25u.lib /nologo /dll /machine:i386 /out:"..\..\src\stc\..\..\..\lib\vc_mswunivudll\wxmswuniv250u_stc_vc_custom.dll" /libpath:"..\..\src\stc\..\..\..\lib\vc_mswunivudll" /implib:"..\..\src\stc\..\..\..\lib\vc_mswunivudll\wxmswuniv25u_stc.lib"

!ENDIF

# Begin Target

# Name "stc - Win32 Debug"
# Name "stc - Win32 Release"
# Name "stc - Win32 Unicode Debug"
# Name "stc - Win32 Unicode Release"
# Name "stc - Win32 Universal Debug"
# Name "stc - Win32 Universal Release"
# Name "stc - Win32 Universal Unicode Debug"
# Name "stc - Win32 Universal Unicode Release"
# Name "stc - Win32 DLL Debug"
# Name "stc - Win32 DLL Release"
# Name "stc - Win32 DLL Unicode Debug"
# Name "stc - Win32 DLL Unicode Release"
# Name "stc - Win32 DLL Universal Debug"
# Name "stc - Win32 DLL Universal Release"
# Name "stc - Win32 DLL Universal Unicode Debug"
# Name "stc - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/stc\scintilla\src\AutoComplete.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\CallTip.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\CellBuffer.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\ContractionState.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\Document.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\DocumentAccessor.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\Editor.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\ExternalLexer.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\Indicator.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\KeyMap.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\KeyWords.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexAVE.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexAda.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexAsm.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexBaan.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexBullant.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexCPP.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexCSS.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexConf.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexCrontab.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexEScript.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexEiffel.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexFortran.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexHTML.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexLisp.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexLout.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexLua.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexMatlab.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexOthers.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexPOV.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexPascal.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexPerl.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexPython.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexRuby.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexSQL.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LexVB.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\LineMarker.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\PlatWX.cpp
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\PropSet.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\RESearch.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\ScintillaBase.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\ScintillaWX.cpp
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\Style.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\StyleContext.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\UniConversion.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\ViewStyle.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\WindowAccessor.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\XPM.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\stc.cpp
# End Source File
# End Group
# End Target
# End Project

