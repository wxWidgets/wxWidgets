# Microsoft Developer Studio Project File - Name="zip" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=zip - Win32 LIB wxUniv Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zip.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zip.mak" CFG="zip - Win32 LIB wxUniv Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zip - Win32 LIB wxUniv Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 LIB Native Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 LIB Native Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 LIB wxUniv Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 LIB Native Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 LIB wxUniv Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 LIB Native Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "zip - Win32 LIB wxUniv Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zip - Win32 LIB wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivudzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivudzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25ud_html.lib wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivud" /subsystem:windows
# ADD LINK32 wxmswuniv25ud_html.lib wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivud" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswdllzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswdllzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_html.lib wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswdllzip.exe" /libpath:".\..\..\..\lib\vc_mswdll" /subsystem:windows
# ADD LINK32 wxmsw25_html.lib wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswdllzip.exe" /libpath:".\..\..\..\lib\vc_mswdll" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivuddllzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivuddllzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25ud_html.lib wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuddllzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivuddll" /subsystem:windows
# ADD LINK32 wxmswuniv25ud_html.lib wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuddllzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivuddll" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivudllzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivudllzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25u_html.lib wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudllzip.exe" /libpath:".\..\..\..\lib\vc_mswunivudll" /subsystem:windows
# ADD LINK32 wxmswuniv25u_html.lib wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudllzip.exe" /libpath:".\..\..\..\lib\vc_mswunivudll" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivdllzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivdllzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25_html.lib wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivdllzip.exe" /libpath:".\..\..\..\lib\vc_mswunivdll" /subsystem:windows
# ADD LINK32 wxmswuniv25_html.lib wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivdllzip.exe" /libpath:".\..\..\..\lib\vc_mswunivdll" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 LIB Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswudzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswudzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25ud_html.lib wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswud" /subsystem:windows
# ADD LINK32 wxmsw25ud_html.lib wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswud" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivddllzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivddllzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25d_html.lib wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivddllzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivddll" /subsystem:windows
# ADD LINK32 wxmswuniv25d_html.lib wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivddllzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivddll" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 LIB Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswdzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswdzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_html.lib wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswdzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswd" /subsystem:windows
# ADD LINK32 wxmsw25d_html.lib wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswdzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswd" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswudllzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswudllzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25u_html.lib wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudllzip.exe" /libpath:".\..\..\..\lib\vc_mswudll" /subsystem:windows
# ADD LINK32 wxmsw25u_html.lib wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudllzip.exe" /libpath:".\..\..\..\lib\vc_mswudll" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 LIB wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuniv" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuniv" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuniv" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuniv" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25_html.lib wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivzip.exe" /libpath:".\..\..\..\lib\vc_mswuniv" /subsystem:windows
# ADD LINK32 wxmswuniv25_html.lib wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivzip.exe" /libpath:".\..\..\..\lib\vc_mswuniv" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 LIB Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswuzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswuzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25u_html.lib wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuzip.exe" /libpath:".\..\..\..\lib\vc_mswu" /subsystem:windows
# ADD LINK32 wxmsw25u_html.lib wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuzip.exe" /libpath:".\..\..\..\lib\vc_mswu" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 LIB wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivdzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivdzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25d_html.lib wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivdzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivd" /subsystem:windows
# ADD LINK32 wxmswuniv25d_html.lib wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivdzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivd" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswuddllzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswuddllzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25ud_html.lib wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuddllzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswuddll" /subsystem:windows
# ADD LINK32 wxmsw25ud_html.lib wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuddllzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswuddll" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswddllzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswddllzip.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_html.lib wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswddllzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswddll" /subsystem:windows
# ADD LINK32 wxmsw25d_html.lib wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswddllzip.exe" /debug /libpath:".\..\..\..\lib\vc_mswddll" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 LIB Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_msw" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_msw" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_msw" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_msw" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_html.lib wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswzip.exe" /libpath:".\..\..\..\lib\vc_msw" /subsystem:windows
# ADD LINK32 wxmsw25_html.lib wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswzip.exe" /libpath:".\..\..\..\lib\vc_msw" /subsystem:windows

!ELSEIF  "$(CFG)" == "zip - Win32 LIB wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\zip"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\zip"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivuzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivuzip.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25u_html.lib wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuzip.exe" /libpath:".\..\..\..\lib\vc_mswunivu" /subsystem:windows
# ADD LINK32 wxmswuniv25u_html.lib wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuzip.exe" /libpath:".\..\..\..\lib\vc_mswunivu" /subsystem:windows

!ENDIF

# Begin Target

# Name "zip - Win32 LIB wxUniv Debug Unicode"
# Name "zip - Win32 DLL Native Release Ansi"
# Name "zip - Win32 DLL wxUniv Debug Unicode"
# Name "zip - Win32 DLL wxUniv Release Unicode"
# Name "zip - Win32 DLL wxUniv Release Ansi"
# Name "zip - Win32 LIB Native Debug Unicode"
# Name "zip - Win32 DLL wxUniv Debug Ansi"
# Name "zip - Win32 LIB Native Debug Ansi"
# Name "zip - Win32 DLL Native Release Unicode"
# Name "zip - Win32 LIB wxUniv Release Ansi"
# Name "zip - Win32 LIB Native Release Unicode"
# Name "zip - Win32 LIB wxUniv Debug Ansi"
# Name "zip - Win32 DLL Native Debug Unicode"
# Name "zip - Win32 DLL Native Debug Ansi"
# Name "zip - Win32 LIB Native Release Ansi"
# Name "zip - Win32 LIB wxUniv Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\zip.cpp
# End Source File
# Begin Source File

SOURCE=.\zip.rc
# End Source File
# End Group
# End Target
# End Project

