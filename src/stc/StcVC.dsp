# Microsoft Developer Studio Project File - Name="StcVC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=StcVC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "StcVC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "StcVC.mak" CFG="StcVC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "StcVC - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "StcVC - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "StcVC - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "../../../include" /I "../../include" /I "scintilla/include" /I "scintilla/src" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "__WX__" /D "SCI_LEXER" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\stc.lib"

!ELSEIF  "$(CFG)" == "StcVC - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "../../../include" /I "../../include" /I "scintilla/include" /I "scintilla/src" /D "_DEBUG" /D DEBUG=1 /D "__WXDEBUG__" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "__WX__" /D "SCI_LEXER" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\stcd.lib"

!ENDIF 

# Begin Target

# Name "StcVC - Win32 Release"
# Name "StcVC - Win32 Debug"
# Begin Group "Stc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PlatWX.cpp
# End Source File
# Begin Source File

SOURCE=.\ScintillaWX.cpp
# End Source File
# Begin Source File

SOURCE=.\ScintillaWX.h
# End Source File
# Begin Source File

SOURCE=.\stc.cpp
# End Source File
# End Group
# Begin Group "Scintilla"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scintilla\src\Accessor.cxx
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

SOURCE=.\scintilla\src\LineMarker.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\LineMarker.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\PropSet.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\ScintillaBase.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\ScintillaBase.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\Style.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\Style.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\SVector.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\ViewStyle.cxx
# End Source File
# Begin Source File

SOURCE=.\scintilla\src\ViewStyle.h
# End Source File
# End Group
# End Target
# End Project
