# Microsoft Developer Studio Project File - Name="stcVC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=stcVC - Win32 Release DLL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "StcVC.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "StcVC.mak" CFG="stcVC - Win32 Release DLL"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "stcVC - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stcVC - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stcVC - Win32 Debug DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "stcVC - Win32 Release DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "stcVC - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
LINK32=link.exe
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "../../../include" /I "../../include" /I "../../../lib/msw" /I "./scintilla/include/" /I "./scintilla/src" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__HACK_MY_MSDEV40__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WX__" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\stc.lib"

!ELSEIF  "$(CFG)" == "stcVC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
LINK32=link.exe
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswd" /I "./scintilla/include/" /I "./scintilla/src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WX__" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\stcd.lib"

!ELSEIF  "$(CFG)" == "stcVC - Win32 Debug DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_DLL"
# PROP BASE Intermediate_Dir "Debug_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "Debug_DLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\stcd.lib"
# ADD LIB32 /nologo /out:"..\..\..\lib\stcd.lib"
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswdlld" /I "./scintilla/include/" /I "./scintilla/src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "WXUSINGDLL" /D "WXMAKING_STC_DLL" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WX__" /YX /FD /c
# SUBTRACT CPP /u
MTL=midl.exe
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib wxmsw24d.lib /nologo /version:1.0 /dll /debug /machine:IX86 /out:"../../../lib/stcdlld.dll" /libpath:"../../../lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "stcVC - Win32 Release DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "stcVC___Win32_Release_DLL"
# PROP BASE Intermediate_Dir "stcVC___Win32_Release_DLL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\lib"
# PROP Intermediate_Dir "Release_DLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\stc.lib"
# ADD LIB32 /nologo /out:"..\..\..\lib\stc.lib"
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../../include" /I "../../include" /I "../../../lib/mswdll" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXUSINGDLL=1 /D "WXBUILD_STC_DLL" /YX /FD /c
# SUBTRACT BASE CPP /u
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "../../../include" /I "../../include" /I "../../../lib/mswdll" /I "./scintilla/include/" /I "./scintilla/src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "WXUSINGDLL" /D "WXMAKING_STC_DLL" /D "SCI_LEXER" /D "LINK_LEXERS" /D "__WX__" /YX /FD /c
# SUBTRACT CPP /u
MTL=midl.exe
RSC=rc.exe
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib wxmsw24.lib /nologo /version:1.0 /dll /debug /machine:IX86 /out:"../../../lib/stcdlld.dll" /libpath:"../../../lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib odbc32.lib uuid.lib rpcrt4.lib comctl32.lib wsock32.lib winmm.lib wxmsw24.lib /nologo /version:1.0 /dll /machine:IX86 /out:"../../../lib/stcdll.dll" /libpath:"../../../lib"
# SUBTRACT LINK32 /pdb:none /incremental:no /debug

!ENDIF

# Begin Target

# Name "stcVC - Win32 Release"
# Name "stcVC - Win32 Debug"
# Name "stcVC - Win32 Debug DLL"
# Name "stcVC - Win32 Release DLL"
# Begin Group "wxStyledTextCtrl Src"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\PlatWX.cpp
# End Source File
# Begin Source File

SOURCE=.\ScintillaWX.cpp
# End Source File
# Begin Source File

SOURCE=.\stc.cpp
# End Source File
# End Group
# Begin Group "wxStyledTextCtrl Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PlatWX.h
# End Source File
# Begin Source File

SOURCE=.\ScintillaWX.h
# End Source File
# Begin Source File

SOURCE=..\..\include\wx\stc\stc.h
# End Source File
# End Group
# Begin Group "Scintilla"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scintilla\include\Accessor.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\AutoComplete.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\AutoComplete.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\CallTip.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\CallTip.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\CellBuffer.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\CellBuffer.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\ContractionState.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\ContractionState.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\Document.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\Document.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\DocumentAccessor.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\DocumentAccessor.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\Editor.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\Editor.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\Indicator.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\Indicator.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\KeyMap.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\KeyMap.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\KeyWords.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\include\KeyWords.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexAda.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexAsm.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexAVE.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexBaan.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexBullant.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexConf.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexCPP.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexCrontab.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexCSS.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexEiffel.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexFortran.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexHTML.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexLisp.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexLua.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexMatlab.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexOthers.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexPOV.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexPascal.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexPerl.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexPython.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexRuby.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexSQL.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexVB.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexMMIXAL.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexNsis.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexPS.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LexScriptol.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LineMarker.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LineMarker.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\include\Platform.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\PropSet.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\include\PropSet.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\RESearch.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\RESearch.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\include\SciLexer.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\include\Scintilla.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\ScintillaBase.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\ScintillaBase.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\include\ScintillaWidget.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\include\SString.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\Style.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\Style.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\StyleContext.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\StyleContext.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\SVector.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\UniConversion.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\UniConversion.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\ViewStyle.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\ViewStyle.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\WindowAccessor.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\XPM.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\include\WindowAccessor.h
# End Source File
# End Group
# End Target
# End Project
