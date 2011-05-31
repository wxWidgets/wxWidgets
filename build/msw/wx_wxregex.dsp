# Microsoft Developer Studio Project File - Name="wx_wxregex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxregex - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxregex.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxregex.mak" CFG="wxregex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxregex - Win32 DLL Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 Universal Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 Universal Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxregex - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxregex - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivudll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_dll\mswunivu" /Zi /Fd..\..\lib\vc_dll\wxregexu.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_dll\mswunivu" /Zi /Fd..\..\lib\vc_dll\wxregexu.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxregexu.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxregexu.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswunivuddll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_dll\mswunivud" /Zi /Fd..\..\lib\vc_dll\wxregexud.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_dll\mswunivud" /Zi /Fd..\..\lib\vc_dll\wxregexud.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxregexud.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxregexud.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_dll\mswu" /Zi /Fd..\..\lib\vc_dll\wxregexu.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_dll\mswu" /Zi /Fd..\..\lib\vc_dll\wxregexu.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxregexu.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxregexu.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_dll\mswud" /Zi /Fd..\..\lib\vc_dll\wxregexud.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_dll\mswud" /Zi /Fd..\..\lib\vc_dll\wxregexud.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxregexud.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxregexud.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivu\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivu\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_lib\mswunivu" /Zi /Fd..\..\lib\vc_lib\wxregexu.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_lib\mswunivu" /Zi /Fd..\..\lib\vc_lib\wxregexu.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxregexu.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxregexu.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswunivud\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswunivud\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_lib\mswunivud" /Zi /Fd..\..\lib\vc_lib\wxregexud.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_lib\mswunivud" /Zi /Fd..\..\lib\vc_lib\wxregexud.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxregexud.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxregexud.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_lib\mswu" /Zi /Fd..\..\lib\vc_lib\wxregexu.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_lib\mswu" /Zi /Fd..\..\lib\vc_lib\wxregexu.pdb /opt:ref /opt:icf /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxregexu.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxregexu.lib"

!ELSEIF  "$(CFG)" == "wxregex - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\wxregex"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\wxregex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_lib\mswud" /Zi /Fd..\..\lib\vc_lib\wxregexud.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /I "..\..\include" /I "..\..\lib\vc_lib\mswud" /Zi /Fd..\..\lib\vc_lib\wxregexud.pdb /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "__WXMSW__" /D "_UNICODE" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxregexud.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxregexud.lib"

!ENDIF

# Begin Target

# Name "wxregex - Win32 DLL Universal Release"
# Name "wxregex - Win32 DLL Universal Debug"
# Name "wxregex - Win32 DLL Release"
# Name "wxregex - Win32 DLL Debug"
# Name "wxregex - Win32 Universal Release"
# Name "wxregex - Win32 Universal Debug"
# Name "wxregex - Win32 Release"
# Name "wxregex - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\regex\regcomp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\regex\regerror.c
# End Source File
# Begin Source File

SOURCE=..\..\src\regex\regexec.c
# End Source File
# Begin Source File

SOURCE=..\..\src\regex\regfree.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\wx\msw\genrcdefs.h

!IF  "$(CFG)" == "wxregex - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswunivud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_dll\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_dll\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswunivud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswunivud\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswu\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswu\wx\msw\rcdefs.h"

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h
InputPath=..\..\include\wx\msw\genrcdefs.h

"..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h" : "$(SOURCE)" "..\..\lib\vc_lib\mswud\wx\msw"
	cl /EP /nologo "$(InputPath)" > "..\..\lib\vc_lib\mswud\wx\msw\rcdefs.h"

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\msw\setup.h

!IF  "$(CFG)" == "wxregex - Win32 DLL Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_dll\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Universal Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Universal Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswu\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswud\wx\setup.h
InputPath=..\..\include\wx\msw\setup.h

"..\..\lib\vc_lib\mswud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswud\wx\setup.h

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=..\..\include\wx\univ\setup.h

!IF  "$(CFG)" == "wxregex - Win32 DLL Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivu\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_dll\mswunivud\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_dll\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_dll\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 DLL Debug"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Universal Release"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivu\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivu\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivu\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Universal Debug"

# Begin Custom Build - Creating ..\..\lib\vc_lib\mswunivud\wx\setup.h
InputPath=..\..\include\wx\univ\setup.h

"..\..\lib\vc_lib\mswunivud\wx\setup.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\lib\vc_lib\mswunivud\wx\setup.h

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Release"

# Begin Custom Build - 

# End Custom Build

!ELSEIF  "$(CFG)" == "wxregex - Win32 Debug"

# Begin Custom Build - 

# End Custom Build

!ENDIF

# End Source File
# End Group
# End Target
# End Project

