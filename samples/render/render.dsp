# Microsoft Developer Studio Project File - Name="render" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=render - Win32 LIB wxUniv Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "render.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "render.mak" CFG="render - Win32 LIB wxUniv Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "render - Win32 LIB wxUniv Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 LIB Native Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 LIB Native Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 LIB wxUniv Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 LIB Native Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 LIB wxUniv Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 LIB Native Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "render - Win32 LIB wxUniv Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "render - Win32 LIB wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /Fd"vc_mswunivud\render.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /Fd"vc_mswunivud\render.pdb" /FD /GZ /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswunivud"
# ADD LINK32 wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswunivud"

!ELSEIF  "$(CFG)" == "render - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswdll\render.pdb" /FD /c
# ADD CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswdll\render.pdb" /FD /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "WXUSINGDLL" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "WXUSINGDLL" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswdll"
# ADD LINK32 wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswdll"

!ELSEIF  "$(CFG)" == "render - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswunivuddll\render.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswunivuddll\render.pdb" /FD /GZ /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /d "WXUSINGDLL" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /d "WXUSINGDLL" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswunivuddll"
# ADD LINK32 wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswunivuddll"

!ELSEIF  "$(CFG)" == "render - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswunivudll\render.pdb" /FD /c
# ADD CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswunivudll\render.pdb" /FD /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /d "WXUSINGDLL" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /d "WXUSINGDLL" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswunivudll"
# ADD LINK32 wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswunivudll"

!ELSEIF  "$(CFG)" == "render - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswunivdll\render.pdb" /FD /c
# ADD CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswunivdll\render.pdb" /FD /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "WXUSINGDLL" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "WXUSINGDLL" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswunivdll"
# ADD LINK32 wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswunivdll"

!ELSEIF  "$(CFG)" == "render - Win32 LIB Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /Fd"vc_mswud\render.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /Fd"vc_mswud\render.pdb" /FD /GZ /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswud"
# ADD LINK32 wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswud"

!ELSEIF  "$(CFG)" == "render - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswunivddll\render.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswunivddll\render.pdb" /FD /GZ /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswunivddll"
# ADD LINK32 wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswunivddll"

!ELSEIF  "$(CFG)" == "render - Win32 LIB Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /Fd"vc_mswd\render.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /Fd"vc_mswd\render.pdb" /FD /GZ /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswd"
# ADD LINK32 wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswd"

!ELSEIF  "$(CFG)" == "render - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswudll\render.pdb" /FD /c
# ADD CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswudll\render.pdb" /FD /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d wxUSE_UNICODE=1 /d "WXUSINGDLL" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d wxUSE_UNICODE=1 /d "WXUSINGDLL" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswudll"
# ADD LINK32 wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswudll"

!ELSEIF  "$(CFG)" == "render - Win32 LIB wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswuniv" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /Fd"vc_mswuniv\render.pdb" /FD /c
# ADD CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswuniv" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /Fd"vc_mswuniv\render.pdb" /FD /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswuniv" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswuniv" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswuniv"
# ADD LINK32 wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswuniv"

!ELSEIF  "$(CFG)" == "render - Win32 LIB Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /Fd"vc_mswu\render.pdb" /FD /c
# ADD CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /Fd"vc_mswu\render.pdb" /FD /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d wxUSE_UNICODE=1 /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d wxUSE_UNICODE=1 /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswu"
# ADD LINK32 wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswu"

!ELSEIF  "$(CFG)" == "render - Win32 LIB wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /Fd"vc_mswunivd\render.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /Fd"vc_mswunivd\render.pdb" /FD /GZ /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswunivd"
# ADD LINK32 wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswunivd"

!ELSEIF  "$(CFG)" == "render - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswuddll\render.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswuddll\render.pdb" /FD /GZ /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /d "WXUSINGDLL" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /d "WXUSINGDLL" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswuddll"
# ADD LINK32 wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswuddll"

!ELSEIF  "$(CFG)" == "render - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswddll\render.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /Fd"vc_mswddll\render.pdb" /FD /GZ /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswddll"
# ADD LINK32 wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\..\..\lib\vc_mswddll"

!ELSEIF  "$(CFG)" == "render - Win32 LIB Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_msw" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /Fd"vc_msw\render.pdb" /FD /c
# ADD CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_msw" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /Fd"vc_msw\render.pdb" /FD /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_msw" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_msw" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_msw"
# ADD LINK32 wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_msw"

!ELSEIF  "$(CFG)" == "render - Win32 LIB wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\render"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\render"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /Fd"vc_mswunivu\render.pdb" /FD /c
# ADD CPP /nologo /MD /W4 /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /Fd"vc_mswunivu\render.pdb" /FD /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /d "_WINDOWS"
# ADD RSC /l 0x405 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /d "_WINDOWS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswunivu"
# ADD LINK32 wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:".\..\..\lib\vc_mswunivu"

!ENDIF 

# Begin Target

# Name "render - Win32 LIB wxUniv Debug Unicode"
# Name "render - Win32 DLL Native Release Ansi"
# Name "render - Win32 DLL wxUniv Debug Unicode"
# Name "render - Win32 DLL wxUniv Release Unicode"
# Name "render - Win32 DLL wxUniv Release Ansi"
# Name "render - Win32 LIB Native Debug Unicode"
# Name "render - Win32 DLL wxUniv Debug Ansi"
# Name "render - Win32 LIB Native Debug Ansi"
# Name "render - Win32 DLL Native Release Unicode"
# Name "render - Win32 LIB wxUniv Release Ansi"
# Name "render - Win32 LIB Native Release Unicode"
# Name "render - Win32 LIB wxUniv Debug Ansi"
# Name "render - Win32 DLL Native Debug Unicode"
# Name "render - Win32 DLL Native Debug Ansi"
# Name "render - Win32 LIB Native Release Ansi"
# Name "render - Win32 LIB wxUniv Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\render.cpp
# End Source File
# Begin Source File

SOURCE=.\..\sample.rc
# End Source File
# End Group
# End Target
# End Project
