# Microsoft Developer Studio Project File - Name="HelpGenVC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=HelpGenVC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HelpGenVC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HelpGenVC.mak" CFG="HelpGenVC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HelpGenVC - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "HelpGenVC - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "HelpGenVC - Win32 Debug DLL" (based on "Win32 (x86) Application")
!MESSAGE "HelpGenVC - Win32 Release DLL" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HelpGenVC - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "../../../include" /I "../../../contrib/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wx.lib xpm.lib png.lib zlib.lib jpeg.lib tiff.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /out:"Release/helpgen.exe" /libpath:"../../../lib" /libpath:"../../../contrib/lib"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../include" /I "../../../contrib/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /Yu"wx/wxprec.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wxd.lib xpmd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /out:"Debug/helpgen.exe" /pdbtype:sept /libpath:"../../../lib" /libpath:"../../../contrib/lib"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDLL"
# PROP BASE Intermediate_Dir "DebugDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLL"
# PROP Intermediate_Dir "DebugDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../include" /I "../../../contrib/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXUSINGDLL=1 /Yu"wx/wxprec.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wx23_0d.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /out:"DebugDLL/helpgen.exe" /pdbtype:sept /libpath:"../../../lib" /libpath:"../../../contrib/lib"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Release DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDLL"
# PROP BASE Intermediate_Dir "ReleaseDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDLL"
# PROP Intermediate_Dir "ReleaseDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "../../../include" /I "../../../contrib/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D WXUSINGDLL=1 /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib wx23_0.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /out:"ReleaseDLL/helpgen.exe" /libpath:"../../../lib" /libpath:"../../../contrib/lib"

!ENDIF 

# Begin Target

# Name "HelpGenVC - Win32 Release"
# Name "HelpGenVC - Win32 Debug"
# Name "HelpGenVC - Win32 Debug DLL"
# Name "HelpGenVC - Win32 Release DLL"
# Begin Source File

SOURCE=.\cjparser.cpp

!IF  "$(CFG)" == "HelpGenVC - Win32 Release"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug DLL"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Release DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cjparser.h
# End Source File
# Begin Source File

SOURCE=.\docripper.cpp

!IF  "$(CFG)" == "HelpGenVC - Win32 Release"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug DLL"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Release DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\docripper.h
# End Source File
# Begin Source File

SOURCE=.\HelpGen.cpp

!IF  "$(CFG)" == "HelpGenVC - Win32 Release"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug DLL"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Release DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HelpGen.rc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "..\..\..\include"
# End Source File
# Begin Source File

SOURCE=.\ifcontext.cpp

!IF  "$(CFG)" == "HelpGenVC - Win32 Release"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug DLL"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Release DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ifcontext.h
# End Source File
# Begin Source File

SOURCE=.\markup.cpp

!IF  "$(CFG)" == "HelpGenVC - Win32 Release"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug DLL"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Release DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\markup.h
# End Source File
# Begin Source File

SOURCE=.\ripper_main.cpp

!IF  "$(CFG)" == "HelpGenVC - Win32 Release"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug DLL"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Release DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\scriptbinder.cpp

!IF  "$(CFG)" == "HelpGenVC - Win32 Release"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug DLL"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Release DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\scriptbinder.h
# End Source File
# Begin Source File

SOURCE=.\sourcepainter.cpp

!IF  "$(CFG)" == "HelpGenVC - Win32 Release"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug DLL"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Release DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sourcepainter.h
# End Source File
# Begin Source File

SOURCE=.\srcparser.cpp

!IF  "$(CFG)" == "HelpGenVC - Win32 Release"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Debug DLL"

!ELSEIF  "$(CFG)" == "HelpGenVC - Win32 Release DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\srcparser.h
# End Source File
# Begin Source File

SOURCE=.\wxstlac.h
# End Source File
# Begin Source File

SOURCE=.\wxstllst.h
# End Source File
# Begin Source File

SOURCE=.\wxstlvec.h
# End Source File
# End Target
# End Project
