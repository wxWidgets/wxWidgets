# Microsoft Developer Studio Project File - Name="wxrc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=wxrc - Win32 Static Native Release Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxrc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxrc.mak" CFG="wxrc - Win32 Static Native Release Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxrc - Win32 Static Native Release Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 Static Native Release Ansi" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 Static wxUniv Release Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 Static wxUniv Debug Ansi" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 Static Native Debug Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 Static wxUniv Release Ansi" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 Static wxUniv Debug Unicode" (based on "Win32 (x86) Console Application")
!MESSAGE "wxrc - Win32 Static Native Debug Ansi" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxrc - Win32 Static Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswu\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswu\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswu"
# ADD LINK32 wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswu"

!ELSEIF  "$(CFG)" == "wxrc - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswdll\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswdll\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswdll"
# ADD LINK32 wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswdll"

!ELSEIF  "$(CFG)" == "wxrc - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivuddll\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivuddll\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivuddll"
# ADD LINK32 wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivuddll"

!ELSEIF  "$(CFG)" == "wxrc - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswuddll\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswuddll\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswuddll"
# ADD LINK32 wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswuddll"

!ELSEIF  "$(CFG)" == "wxrc - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivdll\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivdll\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswunivdll"
# ADD LINK32 wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswunivdll"

!ELSEIF  "$(CFG)" == "wxrc - Win32 Static Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_msw\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_msw\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_msw" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_msw" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_msw" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_msw" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_msw"
# ADD LINK32 wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_msw"

!ELSEIF  "$(CFG)" == "wxrc - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivddll\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivddll\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivddll"
# ADD LINK32 wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivddll"

!ELSEIF  "$(CFG)" == "wxrc - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswudll\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswudll\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswudll"
# ADD LINK32 wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswudll"

!ELSEIF  "$(CFG)" == "wxrc - Win32 Static wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivu\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivu\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswunivu"
# ADD LINK32 wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswunivu"

!ELSEIF  "$(CFG)" == "wxrc - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivudll\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivudll\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswunivudll"
# ADD LINK32 wxbase25u_xml.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswunivudll"

!ELSEIF  "$(CFG)" == "wxrc - Win32 Static wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivd\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivd\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivd"
# ADD LINK32 wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivd"

!ELSEIF  "$(CFG)" == "wxrc - Win32 Static Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswud\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswud\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswud"
# ADD LINK32 wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswud"

!ELSEIF  "$(CFG)" == "wxrc - Win32 Static wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswuniv\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswuniv\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuniv" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MD /Fdwxrc.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuniv" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuniv" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuniv" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswuniv"
# ADD LINK32 wxbase25_xml.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /libpath:".\..\..\..\lib\vc_mswuniv"

!ELSEIF  "$(CFG)" == "wxrc - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswddll\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswddll\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswddll"
# ADD LINK32 wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswddll"

!ELSEIF  "$(CFG)" == "wxrc - Win32 Static wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivud\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivud\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivud"
# ADD LINK32 wxbase25ud_xml.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivud"

!ELSEIF  "$(CFG)" == "wxrc - Win32 Static Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswd\wxrc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswd\wxrc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_CONSOLE" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdwxrc.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_CONSOLE" /c
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _CONSOLE
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswd"
# ADD LINK32 wxbase25d_xml.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"wxrc.exe" /debug /libpath:".\..\..\..\lib\vc_mswd"

!ENDIF

# Begin Target

# Name "wxrc - Win32 Static Native Release Unicode"
# Name "wxrc - Win32 DLL Native Release Ansi"
# Name "wxrc - Win32 DLL wxUniv Debug Unicode"
# Name "wxrc - Win32 DLL Native Debug Unicode"
# Name "wxrc - Win32 DLL wxUniv Release Ansi"
# Name "wxrc - Win32 Static Native Release Ansi"
# Name "wxrc - Win32 DLL wxUniv Debug Ansi"
# Name "wxrc - Win32 DLL Native Release Unicode"
# Name "wxrc - Win32 Static wxUniv Release Unicode"
# Name "wxrc - Win32 DLL wxUniv Release Unicode"
# Name "wxrc - Win32 Static wxUniv Debug Ansi"
# Name "wxrc - Win32 Static Native Debug Unicode"
# Name "wxrc - Win32 Static wxUniv Release Ansi"
# Name "wxrc - Win32 DLL Native Debug Ansi"
# Name "wxrc - Win32 Static wxUniv Debug Unicode"
# Name "wxrc - Win32 Static Native Debug Ansi"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wxrc.cpp
# End Source File
# End Group
# End Target
# End Project

