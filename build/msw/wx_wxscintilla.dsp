# Microsoft Developer Studio Project File - Name="wx_wxscintilla" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxscintilla - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxscintilla.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxscintilla.mak" CFG="wxscintilla - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxscintilla - Win32 DLL Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 DLL Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxscintilla - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxscintilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\wxscintilla"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswunivu" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxscintilla.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxscintilla.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxscintilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxscintilla"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswunivud" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxscintillad.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxscintillad.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxscintilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\wxscintilla"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxscintilla.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxscintilla.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxscintilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\wxscintilla"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxscintillad.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxscintillad.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxscintilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\wxscintilla"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswunivu" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxscintilla.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxscintilla.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxscintilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\wxscintilla"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswunivud" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxscintillad.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxscintillad.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\wxscintilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\wxscintilla"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxscintilla.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxscintilla.lib"

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\wxscintilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\wxscintilla"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxscintillad.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxscintillad.lib"

!ENDIF

# Begin Target

# Name "wxscintilla - Win32 DLL Universal Release"
# Name "wxscintilla - Win32 DLL Universal Debug"
# Name "wxscintilla - Win32 DLL Release"
# Name "wxscintilla - Win32 DLL Debug"
# Name "wxscintilla - Win32 Universal Release"
# Name "wxscintilla - Win32 Universal Debug"
# Name "wxscintilla - Win32 Release"
# Name "wxscintilla - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\AutoComplete.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\CallTip.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\CellBuffer.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\CharClassify.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\ContractionState.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\Decoration.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\Document.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\DocumentAccessor.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\Editor.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\ExternalLexer.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\Indicator.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\KeyMap.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\KeyWords.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexAPDL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexASY.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexAU3.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexAVE.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexAbaqus.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexAda.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexAsm.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexAsn1.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexBaan.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexBash.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexBasic.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexBullant.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexCLW.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexCOBOL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexCPP.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexCSS.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexCaml.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexCmake.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexConf.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexCrontab.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexCsound.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexD.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexEScript.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexEiffel.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexErlang.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexFlagship.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexForth.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexFortran.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexGAP.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexGui4Cli.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexHTML.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexHaskell.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexInno.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexKix.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexLisp.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexLout.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexLua.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexMMIXAL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexMPT.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexMSSQL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexMagik.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexMarkdown.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexMatlab.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexMetapost.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexMySQL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexNimrod.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexNsis.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexOpal.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexOthers.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexPB.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexPLM.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexPOV.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexPS.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexPascal.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexPerl.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexPowerPro.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexPowerShell.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexProgress.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexPython.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexR.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexRebol.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexRuby.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexSML.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexSQL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexScriptol.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexSmalltalk.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexSorcus.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexSpecman.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexSpice.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexTACL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexTADS3.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexTAL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexTCL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexTeX.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexVB.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexVHDL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexVerilog.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LexYAML.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\LineMarker.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\PerLine.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\PositionCache.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\PropSet.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\RESearch.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\RunStyles.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\ScintillaBase.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\Selection.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\Style.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\StyleContext.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\UniConversion.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\ViewStyle.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\WindowAccessor.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\XPM.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\genrcdefs.h

!IF  "$(CFG)" == "wxscintilla - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\setup.h

!IF  "$(CFG)" == "wxscintilla - Win32 DLL Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswud\wx\setup.h

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\univ\setup.h

!IF  "$(CFG)" == "wxscintilla - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 Debug"

# Begin Custom Build - 

# End Custom Build

!ENDIF

# End Source File
# End Group
# End Target
# End Project

