# Microsoft Developer Studio Project File - Name="stclib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=stclib - Win32 Static Native Release Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stc_stclib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stc_stclib.mak" CFG="stclib - Win32 Static Native Release Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stclib - Win32 Static Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "stclib - Win32 Static wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "stclib - Win32 Static Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "stclib - Win32 Static Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "stclib - Win32 Static wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "stclib - Win32 Static wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "stclib - Win32 Static Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "stclib - Win32 Static wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stclib - Win32 Static Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswu\"
# PROP BASE Intermediate_Dir "vc_mswu\stclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswu\"
# PROP Intermediate_Dir "vc_mswu\stclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswu" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswu\wxmsw25u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswu" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswu\wxmsw25u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswu\wxmsw25u_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswu\wxmsw25u_stc.lib"

!ELSEIF  "$(CFG)" == "stclib - Win32 Static wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivu\"
# PROP BASE Intermediate_Dir "vc_mswunivu\stclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivu\"
# PROP Intermediate_Dir "vc_mswunivu\stclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivu" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivu" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivu\wxmswuniv25u_stc.lib"

!ELSEIF  "$(CFG)" == "stclib - Win32 Static Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_msw\"
# PROP BASE Intermediate_Dir "vc_msw\stclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_msw\"
# PROP Intermediate_Dir "vc_msw\stclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_msw" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_msw\wxmsw25_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_msw" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_msw\wxmsw25_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_msw\wxmsw25_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_msw\wxmsw25_stc.lib"

!ELSEIF  "$(CFG)" == "stclib - Win32 Static Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswud\"
# PROP BASE Intermediate_Dir "vc_mswud\stclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswud\"
# PROP Intermediate_Dir "vc_mswud\stclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswud" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswud\wxmsw25ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswud" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswud\wxmsw25ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswud\wxmsw25ud_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswud\wxmsw25ud_stc.lib"

!ELSEIF  "$(CFG)" == "stclib - Win32 Static wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivd\"
# PROP BASE Intermediate_Dir "vc_mswunivd\stclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivd\"
# PROP Intermediate_Dir "vc_mswunivd\stclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivd" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivd" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivd\wxmswuniv25d_stc.lib"

!ELSEIF  "$(CFG)" == "stclib - Win32 Static wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswuniv\"
# PROP BASE Intermediate_Dir "vc_mswuniv\stclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswuniv\"
# PROP Intermediate_Dir "vc_mswuniv\stclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswuniv" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswuniv\wxmswuniv25_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswuniv" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Fd..\..\src\stc\..\..\..\lib\vc_mswuniv\wxmswuniv25_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswuniv\wxmswuniv25_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswuniv\wxmswuniv25_stc.lib"

!ELSEIF  "$(CFG)" == "stclib - Win32 Static Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswd\"
# PROP BASE Intermediate_Dir "vc_mswd\stclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswd\"
# PROP Intermediate_Dir "vc_mswd\stclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswd" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswd\wxmsw25d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswd" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswd\wxmsw25d_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswd\wxmsw25d_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswd\wxmsw25d_stc.lib"

!ELSEIF  "$(CFG)" == "stclib - Win32 Static wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivud\"
# PROP BASE Intermediate_Dir "vc_mswunivud\stclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\stc\..\..\..\lib\vc_mswunivud\"
# PROP Intermediate_Dir "vc_mswunivud\stclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivud" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\..\src\stc\..\..\..\include" /I "..\..\src\stc\..\..\..\lib\vc_mswunivud" /I "..\..\src\stc\..\..\..\src\tiff" /I "..\..\src\stc\..\..\..\src\jpeg" /I "..\..\src\stc\..\..\..\src\png" /I "..\..\src\stc\..\..\..\src\zlib" /I "..\..\src\stc\..\..\..\src\regex" /I "..\..\src\stc\..\..\..\src\expat\lib" /Zi /Gm /GZ /Fd..\..\src\stc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_stc.pdb /I "..\..\src\stc\..\..\include" /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_stc.lib"
# ADD LIB32 /nologo /out:"..\..\src\stc\..\..\..\lib\vc_mswunivud\wxmswuniv25ud_stc.lib"

!ENDIF

# Begin Target

# Name "stclib - Win32 Static Native Release Unicode"
# Name "stclib - Win32 Static wxUniv Release Unicode"
# Name "stclib - Win32 Static Native Release Ansi"
# Name "stclib - Win32 Static Native Debug Unicode"
# Name "stclib - Win32 Static wxUniv Debug Ansi"
# Name "stclib - Win32 Static wxUniv Release Ansi"
# Name "stclib - Win32 Static Native Debug Ansi"
# Name "stclib - Win32 Static wxUniv Debug Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../../src/stc\PlatWX.cpp
# End Source File
# Begin Source File

SOURCE=../../src/stc\ScintillaWX.cpp
# End Source File
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

SOURCE=../../src/stc\scintilla\src\PropSet.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\RESearch.cxx
# End Source File
# Begin Source File

SOURCE=../../src/stc\scintilla\src\ScintillaBase.cxx
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

