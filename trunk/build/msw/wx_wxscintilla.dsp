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
!MESSAGE "wxscintilla - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxscintilla - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxscintilla - Win32 DLL Release"

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
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\lexlib" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\lexlib" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswu" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "_UNICODE" /c
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
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\lexlib" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\lexlib" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_dll\mswud" /I "..\..\include" /Zi /Fd..\..\lib\vc_dll\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "WXUSINGDLL" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxscintillad.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxscintillad.lib"

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
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\lexlib" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\lexlib" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswu" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintilla.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "_UNICODE" /c
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
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\lexlib" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\src\stc\scintilla\include" /I "..\..\src\stc\scintilla\lexlib" /I "..\..\src\stc\scintilla\src" /I "..\..\lib\vc_lib\mswud" /I "..\..\include" /Zi /Fd..\..\lib\vc_lib\wxscintillad.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WX__" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WXMSW__" /D "_UNICODE" /c
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

# Name "wxscintilla - Win32 DLL Release"
# Name "wxscintilla - Win32 DLL Debug"
# Name "wxscintilla - Win32 Release"
# Name "wxscintilla - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexlib\Accessor.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\AutoComplete.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\CallTip.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\Catalogue.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\CellBuffer.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\CharClassify.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexlib\CharacterSet.cxx
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

SOURCE=..\..\src\stc\scintilla\lexers\LexA68k.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexAPDL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexASY.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexAU3.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexAVE.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexAVS.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexAbaqus.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexAda.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexAsm.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexAsn1.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexBaan.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexBash.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexBasic.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexBullant.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexCLW.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexCOBOL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexCPP.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexCSS.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexCaml.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexCmake.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexCoffeeScript.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexConf.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexCrontab.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexCsound.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexD.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexECL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexEScript.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexEiffel.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexErlang.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexFlagship.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexForth.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexFortran.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexGAP.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexGui4Cli.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexHTML.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexHaskell.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexInno.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexKix.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexLisp.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexLout.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexLua.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexMMIXAL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexMPT.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexMSSQL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexMagik.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexMarkdown.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexMatlab.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexMetapost.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexModula.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexMySQL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexNimrod.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexNsis.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexOScript.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexOpal.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexOthers.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexPB.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexPLM.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexPOV.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexPS.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexPascal.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexPerl.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexPowerPro.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexPowerShell.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexProgress.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexPython.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexR.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexRebol.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexRuby.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexSML.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexSQL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexScriptol.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexSmalltalk.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexSorcus.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexSpecman.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexSpice.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexTACL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexTADS3.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexTAL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexTCL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexTCMD.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexTeX.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexTxt2tags.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexVB.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexVHDL.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexVerilog.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexVisualProlog.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexers\LexYAML.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexlib\LexerBase.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexlib\LexerModule.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexlib\LexerNoExceptions.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexlib\LexerSimple.cxx
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

SOURCE=..\..\src\stc\scintilla\lexlib\PropSetSimple.cxx
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

SOURCE=..\..\src\stc\scintilla\lexlib\StyleContext.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\UniConversion.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\ViewStyle.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\lexlib\WordList.cxx
# End Source File
# Begin Source File

SOURCE=..\..\src\stc\scintilla\src\XPM.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\genrcdefs.h

!IF  "$(CFG)" == "wxscintilla - Win32 DLL Release"

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

!IF  "$(CFG)" == "wxscintilla - Win32 DLL Release"

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

!IF  "$(CFG)" == "wxscintilla - Win32 DLL Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxscintilla - Win32 DLL Debug"

# Begin Custom Build - 

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

