# Microsoft Developer Studio Project File - Name="console" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=console - Win32 DLL Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "console.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "console.mak" CFG="console - Win32 DLL Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "console - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Universal Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Universal Release" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Universal Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 Universal Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 DLL Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 DLL Release" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 DLL Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 DLL Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 DLL Universal Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 DLL Universal Release" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "console - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "console - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswd\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswd\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswd\console.exe" /debug /libpath:".\..\..\lib\vc_mswd" /subsystem:console
# ADD LINK32 wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswd\console.exe" /debug /libpath:".\..\..\lib\vc_mswd" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_msw\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_msw" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_msw\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_msw" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i ".\..\..\include" /i ".\..\..\lib\vc_msw" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "__WXMSW__" /i ".\..\..\include" /i ".\..\..\lib\vc_msw" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_msw\console.exe" /libpath:".\..\..\lib\vc_msw" /subsystem:console
# ADD LINK32 wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_msw\console.exe" /libpath:".\..\..\lib\vc_msw" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswud\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswud\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswud\console.exe" /debug /libpath:".\..\..\lib\vc_mswud" /subsystem:console
# ADD LINK32 wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswud\console.exe" /debug /libpath:".\..\..\lib\vc_mswud" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswu\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswu\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswu\console.exe" /libpath:".\..\..\lib\vc_mswu" /subsystem:console
# ADD LINK32 wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswu\console.exe" /libpath:".\..\..\lib\vc_mswu" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivd\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivd\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivd\console.exe" /debug /libpath:".\..\..\lib\vc_mswunivd" /subsystem:console
# ADD LINK32 wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivd\console.exe" /debug /libpath:".\..\..\lib\vc_mswunivd" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswuniv\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswuniv" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswuniv\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswuniv" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswuniv" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswuniv" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuniv\console.exe" /libpath:".\..\..\lib\vc_mswuniv" /subsystem:console
# ADD LINK32 wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuniv\console.exe" /libpath:".\..\..\lib\vc_mswuniv" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivud\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivud\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivud\console.exe" /debug /libpath:".\..\..\lib\vc_mswunivud" /subsystem:console
# ADD LINK32 wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivud\console.exe" /debug /libpath:".\..\..\lib\vc_mswunivud" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivu\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivu\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivu\console.exe" /libpath:".\..\..\lib\vc_mswunivu" /subsystem:console
# ADD LINK32 wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivu\console.exe" /libpath:".\..\..\lib\vc_mswunivu" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswddll\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswddll\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswddll\console.exe" /debug /libpath:".\..\..\lib\vc_mswddll" /subsystem:console
# ADD LINK32 wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswddll\console.exe" /debug /libpath:".\..\..\lib\vc_mswddll" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswdll\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswdll\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "__WXMSW__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswdll\console.exe" /libpath:".\..\..\lib\vc_mswdll" /subsystem:console
# ADD LINK32 wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswdll\console.exe" /libpath:".\..\..\lib\vc_mswdll" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswuddll\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswuddll\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuddll\console.exe" /debug /libpath:".\..\..\lib\vc_mswuddll" /subsystem:console
# ADD LINK32 wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuddll\console.exe" /debug /libpath:".\..\..\lib\vc_mswuddll" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswudll\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswudll\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudll\console.exe" /libpath:".\..\..\lib\vc_mswudll" /subsystem:console
# ADD LINK32 wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudll\console.exe" /libpath:".\..\..\lib\vc_mswudll" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivddll\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivddll\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivddll\console.exe" /debug /libpath:".\..\..\lib\vc_mswunivddll" /subsystem:console
# ADD LINK32 wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivddll\console.exe" /debug /libpath:".\..\..\lib\vc_mswunivddll" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivdll\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivdll\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivdll\console.exe" /libpath:".\..\..\lib\vc_mswunivdll" /subsystem:console
# ADD LINK32 wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivdll\console.exe" /libpath:".\..\..\lib\vc_mswunivdll" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivuddll\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivuddll\console.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\console.exe" /debug /libpath:".\..\..\lib\vc_mswunivuddll" /subsystem:console
# ADD LINK32 wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuddll\console.exe" /debug /libpath:".\..\..\lib\vc_mswunivuddll" /subsystem:console

!ELSEIF  "$(CFG)" == "console - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\console"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\console"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivudll\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivudll\console.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
# ADD RSC /l 0x405 /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d "_CONSOLE" /d wxUSE_GUI=0
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudll\console.exe" /libpath:".\..\..\lib\vc_mswunivudll" /subsystem:console
# ADD LINK32 wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudll\console.exe" /libpath:".\..\..\lib\vc_mswunivudll" /subsystem:console

!ENDIF

# Begin Target

# Name "console - Win32 Debug"
# Name "console - Win32 Release"
# Name "console - Win32 Unicode Debug"
# Name "console - Win32 Unicode Release"
# Name "console - Win32 Universal Debug"
# Name "console - Win32 Universal Release"
# Name "console - Win32 Universal Unicode Debug"
# Name "console - Win32 Universal Unicode Release"
# Name "console - Win32 DLL Debug"
# Name "console - Win32 DLL Release"
# Name "console - Win32 DLL Unicode Debug"
# Name "console - Win32 DLL Unicode Release"
# Name "console - Win32 DLL Universal Debug"
# Name "console - Win32 DLL Universal Release"
# Name "console - Win32 DLL Universal Unicode Debug"
# Name "console - Win32 DLL Universal Unicode Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\console.cpp
# End Source File
# End Group
# End Target
# End Project

