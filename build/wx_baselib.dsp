# Microsoft Developer Studio Project File - Name="baselib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=baselib - Win32 Static Native Release Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_baselib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_baselib.mak" CFG="baselib - Win32 Static Native Release Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "baselib - Win32 Static Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "baselib - Win32 Static wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "baselib - Win32 Static Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "baselib - Win32 Static Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "baselib - Win32 Static wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "baselib - Win32 Static wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "baselib - Win32 Static Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "baselib - Win32 Static wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "baselib - Win32 Static Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswu\"
# PROP BASE Intermediate_Dir "vc_mswu\baselib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswu\"
# PROP Intermediate_Dir "vc_mswu\baselib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswu\wxbase25u.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswu\wxbase25u.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswu\wxbase25u.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswu\wxbase25u.lib"

!ELSEIF  "$(CFG)" == "baselib - Win32 Static wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivu\"
# PROP BASE Intermediate_Dir "vc_mswunivu\baselib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivu\"
# PROP Intermediate_Dir "vc_mswunivu\baselib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswunivu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswunivu\wxbase25u.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswunivu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswunivu\wxbase25u.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivu\wxbase25u.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivu\wxbase25u.lib"

!ELSEIF  "$(CFG)" == "baselib - Win32 Static Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_msw\"
# PROP BASE Intermediate_Dir "vc_msw\baselib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_msw\"
# PROP Intermediate_Dir "vc_msw\baselib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_msw" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_msw\wxbase25.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_msw" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_msw\wxbase25.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_msw\wxbase25.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_msw\wxbase25.lib"

!ELSEIF  "$(CFG)" == "baselib - Win32 Static Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswud\"
# PROP BASE Intermediate_Dir "vc_mswud\baselib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswud\"
# PROP Intermediate_Dir "vc_mswud\baselib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxbase25ud.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxbase25ud.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswud\wxbase25ud.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswud\wxbase25ud.lib"

!ELSEIF  "$(CFG)" == "baselib - Win32 Static wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivd\"
# PROP BASE Intermediate_Dir "vc_mswunivd\baselib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivd\"
# PROP Intermediate_Dir "vc_mswunivd\baselib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxbase25d.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxbase25d.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivd\wxbase25d.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivd\wxbase25d.lib"

!ELSEIF  "$(CFG)" == "baselib - Win32 Static wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswuniv\"
# PROP BASE Intermediate_Dir "vc_mswuniv\baselib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswuniv\"
# PROP Intermediate_Dir "vc_mswuniv\baselib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswuniv" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswuniv\wxbase25.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswuniv" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswuniv\wxbase25.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswuniv\wxbase25.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswuniv\wxbase25.lib"

!ELSEIF  "$(CFG)" == "baselib - Win32 Static Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswd\"
# PROP BASE Intermediate_Dir "vc_mswd\baselib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswd\"
# PROP Intermediate_Dir "vc_mswd\baselib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxbase25d.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxbase25d.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswd\wxbase25d.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswd\wxbase25d.lib"

!ELSEIF  "$(CFG)" == "baselib - Win32 Static wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivud\"
# PROP BASE Intermediate_Dir "vc_mswunivud\baselib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivud\"
# PROP Intermediate_Dir "vc_mswunivud\baselib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxbase25ud.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxbase25ud.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_baselib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D wxUSE_BASE=1 /D wxUSE_GUI=0 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivud\wxbase25ud.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivud\wxbase25ud.lib"

!ENDIF

# Begin Target

# Name "baselib - Win32 Static Native Release Unicode"
# Name "baselib - Win32 Static wxUniv Release Unicode"
# Name "baselib - Win32 Static Native Release Ansi"
# Name "baselib - Win32 Static Native Debug Unicode"
# Name "baselib - Win32 Static wxUniv Debug Ansi"
# Name "baselib - Win32 Static wxUniv Release Ansi"
# Name "baselib - Win32 Static Native Debug Ansi"
# Name "baselib - Win32 Static wxUniv Debug Unicode"
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\app.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\apptrait.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\arrstr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\beforestd.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\buffer.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\build.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\chkconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\clntdata.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\cmdline.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\confbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\config.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\containr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\datetime.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\datstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\db.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbkeyg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbtable.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dde.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\debug.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\defs.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dir.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dynarray.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dynlib.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dynload.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\encconv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\event.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\features.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ffile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\file.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fileconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filefn.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filename.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\filesys.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontenc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fontmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fs_mem.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\fs_zip.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\hash.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\hashmap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\iconloc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\init.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\intl.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\iosfwrap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ioswrap.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ipc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\ipcbase.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\isql.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\isqlext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\list.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\log.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\longlong.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\math.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\memconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\memory.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\memtext.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\module.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msgout.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\mstream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\chkconf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\crashrpt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\dde.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\gccpriv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\mimetype.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\winundef.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\msw\wrapwin.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\object.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\platform.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\process.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\regex.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\snglinst.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stopwatch.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\strconv.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\stream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\string.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sysopt.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textbuf.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\textfile.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\thread.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\tokenzr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\txtstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\types.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\utils.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\variant.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\vector.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\version.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\volume.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wfstream.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wx.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxchar.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\wxprec.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\zipstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\zstream.h
# End Source File
# End Group
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\common\appbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\clntdata.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\cmdline.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\config.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\datetime.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\datstrm.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\db.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dbtable.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dircmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dynarray.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dynlib.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dynload.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\encconv.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\event.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\extended.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\src\common\ffile.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\file.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\fileconf.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\filefn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\filename.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\filesys.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\fmapbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\fs_mem.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\fs_zip.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\hash.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\hashmap.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\init.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\intl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\ipcbase.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\list.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\log.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\longlong.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\memory.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\mimecmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\module.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\msgout.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\mstream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\object.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\process.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\regex.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\stopwatch.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\strconv.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\stream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\string.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\sysopt.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\textbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\textfile.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\tokenzr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\txtstrm.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\unzip.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\src\common\utilscmn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\variant.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\wfstream.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\wxchar.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\zipstrm.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\zstream.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\msw\basemsw.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\crashrpt.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dde.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dir.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=..\src\msw\main.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\mimetype.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\mslu.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\regconf.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\registry.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\snglinst.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\thread.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\utils.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\utilsexc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\msw\volume.cpp
# End Source File
# End Group
# End Target
# End Project

