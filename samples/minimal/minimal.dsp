# Microsoft Developer Studio Project File - Name="minimal" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=minimal - Win32 LIB wxUniv Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "minimal.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "minimal.mak" CFG="minimal - Win32 LIB wxUniv Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "minimal - Win32 LIB wxUniv Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 LIB Native Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 LIB Native Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 LIB wxUniv Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 LIB Native Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 LIB wxUniv Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 LIB Native Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "minimal - Win32 LIB wxUniv Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "minimal - Win32 LIB wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivudminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivudminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudminimal.exe" /debug /libpath:".\..\..\lib\vc_mswunivud" /subsystem:windows
# ADD LINK32 wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudminimal.exe" /debug /libpath:".\..\..\lib\vc_mswunivud" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswdllminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswdllminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswdllminimal.exe" /libpath:".\..\..\lib\vc_mswdll" /subsystem:windows
# ADD LINK32 wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswdllminimal.exe" /libpath:".\..\..\lib\vc_mswdll" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivuddllminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivuddllminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuddllminimal.exe" /debug /libpath:".\..\..\lib\vc_mswunivuddll" /subsystem:windows
# ADD LINK32 wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuddllminimal.exe" /debug /libpath:".\..\..\lib\vc_mswunivuddll" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivudllminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivudllminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudllminimal.exe" /libpath:".\..\..\lib\vc_mswunivudll" /subsystem:windows
# ADD LINK32 wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivudllminimal.exe" /libpath:".\..\..\lib\vc_mswunivudll" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivdllminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivdllminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivdll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivdll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivdllminimal.exe" /libpath:".\..\..\lib\vc_mswunivdll" /subsystem:windows
# ADD LINK32 wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivdllminimal.exe" /libpath:".\..\..\lib\vc_mswunivdll" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 LIB Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswudminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswudminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswud" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswud" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudminimal.exe" /debug /libpath:".\..\..\lib\vc_mswud" /subsystem:windows
# ADD LINK32 wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudminimal.exe" /debug /libpath:".\..\..\lib\vc_mswud" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivddllminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivddllminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivddllminimal.exe" /debug /libpath:".\..\..\lib\vc_mswunivddll" /subsystem:windows
# ADD LINK32 wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivddllminimal.exe" /debug /libpath:".\..\..\lib\vc_mswunivddll" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 LIB Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswdminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswdminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswdminimal.exe" /debug /libpath:".\..\..\lib\vc_mswd" /subsystem:windows
# ADD LINK32 wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswdminimal.exe" /debug /libpath:".\..\..\lib\vc_mswd" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswudllminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswudllminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswudll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswudll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudllminimal.exe" /libpath:".\..\..\lib\vc_mswudll" /subsystem:windows
# ADD LINK32 wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswudllminimal.exe" /libpath:".\..\..\lib\vc_mswudll" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 LIB wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswuniv" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswuniv" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswuniv" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswuniv" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivminimal.exe" /libpath:".\..\..\lib\vc_mswuniv" /subsystem:windows
# ADD LINK32 wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivminimal.exe" /libpath:".\..\..\lib\vc_mswuniv" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 LIB Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswuminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswuminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuminimal.exe" /libpath:".\..\..\lib\vc_mswu" /subsystem:windows
# ADD LINK32 wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuminimal.exe" /libpath:".\..\..\lib\vc_mswu" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 LIB wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivdminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswunivdminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivd" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivd" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivdminimal.exe" /debug /libpath:".\..\..\lib\vc_mswunivd" /subsystem:windows
# ADD LINK32 wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivdminimal.exe" /debug /libpath:".\..\..\lib\vc_mswunivd" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswuddllminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswuddllminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswuddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswuddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuddllminimal.exe" /debug /libpath:".\..\..\lib\vc_mswuddll" /subsystem:windows
# ADD LINK32 wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswuddllminimal.exe" /debug /libpath:".\..\..\lib\vc_mswuddll" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswddllminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MDd /Zi /Gm /GZ /Fdvc_mswddllminimal.pdb /Od /I ".\..\..\include" /I ".\..\..\lib\vc_mswddll" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\include" /i ".\..\..\lib\vc_mswddll" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswddllminimal.exe" /debug /libpath:".\..\..\lib\vc_mswddll" /subsystem:windows
# ADD LINK32 wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswddllminimal.exe" /debug /libpath:".\..\..\lib\vc_mswddll" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 LIB Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_msw" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_msw" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\include" /i ".\..\..\lib\vc_msw" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\include" /i ".\..\..\lib\vc_msw" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswminimal.exe" /libpath:".\..\..\lib\vc_msw" /subsystem:windows
# ADD LINK32 wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswminimal.exe" /libpath:".\..\..\lib\vc_msw" /subsystem:windows

!ELSEIF  "$(CFG)" == "minimal - Win32 LIB wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\minimal"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\minimal"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /Fdvc_mswunivuminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /W4 /MD /Fdvc_mswunivuminimal.pdb /O1 /I ".\..\..\include" /I ".\..\..\lib\vc_mswunivu" /I ".\..\..\src\tiff" /I ".\..\..\src\jpeg" /I ".\..\..\src\png" /I ".\..\..\src\zlib" /I ".\..\..\src\regex" /I ".\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\include" /i ".\..\..\lib\vc_mswunivu" /i ".\..\..\src\tiff" /i ".\..\..\src\jpeg" /i ".\..\..\src\png" /i ".\..\..\src\zlib" /i ".\..\..\src\regex" /i ".\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuminimal.exe" /libpath:".\..\..\lib\vc_mswunivu" /subsystem:windows
# ADD LINK32 wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"vc_mswunivuminimal.exe" /libpath:".\..\..\lib\vc_mswunivu" /subsystem:windows

!ENDIF

# Begin Target

# Name "minimal - Win32 LIB wxUniv Debug Unicode"
# Name "minimal - Win32 DLL Native Release Ansi"
# Name "minimal - Win32 DLL wxUniv Debug Unicode"
# Name "minimal - Win32 DLL wxUniv Release Unicode"
# Name "minimal - Win32 DLL wxUniv Release Ansi"
# Name "minimal - Win32 LIB Native Debug Unicode"
# Name "minimal - Win32 DLL wxUniv Debug Ansi"
# Name "minimal - Win32 LIB Native Debug Ansi"
# Name "minimal - Win32 DLL Native Release Unicode"
# Name "minimal - Win32 LIB wxUniv Release Ansi"
# Name "minimal - Win32 LIB Native Release Unicode"
# Name "minimal - Win32 LIB wxUniv Debug Ansi"
# Name "minimal - Win32 DLL Native Debug Unicode"
# Name "minimal - Win32 DLL Native Debug Ansi"
# Name "minimal - Win32 LIB Native Release Ansi"
# Name "minimal - Win32 LIB wxUniv Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\minimal.cpp
# End Source File
# Begin Source File

SOURCE=.\minimal.rc
# End Source File
# End Group
# End Target
# End Project

