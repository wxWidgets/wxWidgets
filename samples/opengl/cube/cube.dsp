# Microsoft Developer Studio Project File - Name="cube" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=cube - Win32 Static Native Release Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cube.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cube.mak" CFG="cube - Win32 Static Native Release Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cube - Win32 Static Native Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 DLL Native Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 DLL wxUniv Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 DLL Native Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 DLL wxUniv Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 Static Native Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 DLL wxUniv Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 DLL Native Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 Static wxUniv Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 DLL wxUniv Release Unicode" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 Static wxUniv Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 Static Native Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 Static wxUniv Release Ansi" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 DLL Native Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 Static wxUniv Debug Unicode" (based on "Win32 (x86) Application")
!MESSAGE "cube - Win32 Static Native Debug Ansi" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cube - Win32 Static Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswu\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswu\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25u_gl.lib wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswu"
# ADD LINK32 wxmsw25u_gl.lib wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswu"

!ELSEIF  "$(CFG)" == "cube - Win32 DLL Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswdll\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswdll\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_gl.lib wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswdll"
# ADD LINK32 wxmsw25_gl.lib wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswdll"

!ELSEIF  "$(CFG)" == "cube - Win32 DLL wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivuddll\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivuddll\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25ud_gl.lib wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivuddll"
# ADD LINK32 wxmswuniv25ud_gl.lib wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivuddll"

!ELSEIF  "$(CFG)" == "cube - Win32 DLL Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswuddll\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswuddll\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25ud_gl.lib wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswuddll"
# ADD LINK32 wxmsw25ud_gl.lib wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswuddll"

!ELSEIF  "$(CFG)" == "cube - Win32 DLL wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivdll\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivdll\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivdll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivdll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25_gl.lib wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswunivdll"
# ADD LINK32 wxmswuniv25_gl.lib wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswunivdll"

!ELSEIF  "$(CFG)" == "cube - Win32 Static Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_msw\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_msw\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_msw" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_msw" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_msw" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_msw" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25_gl.lib wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_msw"
# ADD LINK32 wxmsw25_gl.lib wxmsw25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_msw"

!ELSEIF  "$(CFG)" == "cube - Win32 DLL wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivddll\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivddll\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25d_gl.lib wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivddll"
# ADD LINK32 wxmswuniv25d_gl.lib wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivddll"

!ELSEIF  "$(CFG)" == "cube - Win32 DLL Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswudll\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswudll\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25u_gl.lib wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswudll"
# ADD LINK32 wxmsw25u_gl.lib wxmsw25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswudll"

!ELSEIF  "$(CFG)" == "cube - Win32 Static wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivu\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivu\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivu" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivu" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25u_gl.lib wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswunivu"
# ADD LINK32 wxmswuniv25u_gl.lib wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswunivu"

!ELSEIF  "$(CFG)" == "cube - Win32 DLL wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivudll\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivudll\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivudll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivudll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25u_gl.lib wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswunivudll"
# ADD LINK32 wxmswuniv25u_gl.lib wxmswuniv25u_core.lib wxbase25u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswunivudll"

!ELSEIF  "$(CFG)" == "cube - Win32 Static wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivd\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivd\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25d_gl.lib wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivd"
# ADD LINK32 wxmswuniv25d_gl.lib wxmswuniv25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivd"

!ELSEIF  "$(CFG)" == "cube - Win32 Static Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswud\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswud\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25ud_gl.lib wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswud"
# ADD LINK32 wxmsw25ud_gl.lib wxmsw25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswud"

!ELSEIF  "$(CFG)" == "cube - Win32 Static wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswuniv\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswuniv\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuniv" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /Fdcube.pdb /O1 /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswuniv" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuniv" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "NDEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswuniv" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25_gl.lib wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswuniv"
# ADD LINK32 wxmswuniv25_gl.lib wxmswuniv25_core.lib wxbase25.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /libpath:".\..\..\..\lib\vc_mswuniv"

!ELSEIF  "$(CFG)" == "cube - Win32 DLL Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswddll\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswddll\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswddll" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswddll" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d "WXUSINGDLL" /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_gl.lib wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswddll"
# ADD LINK32 wxmsw25d_gl.lib wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswddll"

!ELSEIF  "$(CFG)" == "cube - Win32 Static wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswunivud\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswunivud\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswunivud" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d wxUSE_UNICODE=1 /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswunivud" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv25ud_gl.lib wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivud"
# ADD LINK32 wxmswuniv25ud_gl.lib wxmswuniv25ud_core.lib wxbase25ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswunivud"

!ELSEIF  "$(CFG)" == "cube - Win32 Static Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir "vc_mswd\cube"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "vc_mswd\cube"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MDd /Zi /Gm /GZ /Fdcube.pdb /Od /I ".\..\..\..\include" /I ".\..\..\..\lib\vc_mswd" /I ".\..\..\..\src\tiff" /I ".\..\..\..\src\jpeg" /I ".\..\..\..\src\png" /I ".\..\..\..\src\zlib" /I ".\..\..\..\src\regex" /I ".\..\..\..\src\expat\lib" /I "." /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
# ADD RSC /l 0x405 /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /i ".\..\..\..\include" /i ".\..\..\..\lib\vc_mswd" /i ".\..\..\..\src\tiff" /i ".\..\..\..\src\jpeg" /i ".\..\..\..\src\png" /i ".\..\..\..\src\zlib" /i ".\..\..\..\src\regex" /i ".\..\..\..\src\expat\lib" /i "." /d _WINDOWS
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw25d_gl.lib wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswd"
# ADD LINK32 wxmsw25d_gl.lib wxmsw25d_core.lib wxbase25d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib odbc32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /nologo /machine:i386 /out:"cube.exe" /debug /libpath:".\..\..\..\lib\vc_mswd"

!ENDIF

# Begin Target

# Name "cube - Win32 Static Native Release Unicode"
# Name "cube - Win32 DLL Native Release Ansi"
# Name "cube - Win32 DLL wxUniv Debug Unicode"
# Name "cube - Win32 DLL Native Debug Unicode"
# Name "cube - Win32 DLL wxUniv Release Ansi"
# Name "cube - Win32 Static Native Release Ansi"
# Name "cube - Win32 DLL wxUniv Debug Ansi"
# Name "cube - Win32 DLL Native Release Unicode"
# Name "cube - Win32 Static wxUniv Release Unicode"
# Name "cube - Win32 DLL wxUniv Release Unicode"
# Name "cube - Win32 Static wxUniv Debug Ansi"
# Name "cube - Win32 Static Native Debug Unicode"
# Name "cube - Win32 Static wxUniv Release Ansi"
# Name "cube - Win32 DLL Native Debug Ansi"
# Name "cube - Win32 Static wxUniv Debug Unicode"
# Name "cube - Win32 Static Native Debug Ansi"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cube.cpp
# End Source File
# Begin Source File

SOURCE=.\cube.rc
# End Source File
# End Group
# End Target
# End Project

