# Microsoft Developer Studio Project File - Name="odbclib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=odbclib - Win32 LIB wxUniv Release Ansi
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_odbclib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_odbclib.mak" CFG="odbclib - Win32 LIB wxUniv Release Ansi"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "odbclib - Win32 LIB wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "odbclib - Win32 LIB Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "odbclib - Win32 LIB wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "odbclib - Win32 LIB Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "odbclib - Win32 LIB Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "odbclib - Win32 LIB wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "odbclib - Win32 LIB wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "odbclib - Win32 LIB Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "odbclib - Win32 LIB wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswuniv\"
# PROP BASE Intermediate_Dir "vc_mswuniv\odbclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswuniv\"
# PROP Intermediate_Dir "vc_mswuniv\odbclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswuniv" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswuniv\wxmswuniv25_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswuniv" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswuniv\wxmswuniv25_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswuniv\wxmswuniv25_odbc.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswuniv\wxmswuniv25_odbc.lib"

!ELSEIF  "$(CFG)" == "odbclib - Win32 LIB Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswu\"
# PROP BASE Intermediate_Dir "vc_mswu\odbclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswu\"
# PROP Intermediate_Dir "vc_mswu\odbclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswu\wxmsw25u_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswu\wxmsw25u_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswu\wxmsw25u_odbc.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswu\wxmsw25u_odbc.lib"

!ELSEIF  "$(CFG)" == "odbclib - Win32 LIB wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivd\"
# PROP BASE Intermediate_Dir "vc_mswunivd\odbclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivd\"
# PROP Intermediate_Dir "vc_mswunivd\odbclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxmswuniv25d_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxmswuniv25d_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivd\wxmswuniv25d_odbc.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivd\wxmswuniv25d_odbc.lib"

!ELSEIF  "$(CFG)" == "odbclib - Win32 LIB Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_msw\"
# PROP BASE Intermediate_Dir "vc_msw\odbclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_msw\"
# PROP Intermediate_Dir "vc_msw\odbclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_msw" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_msw\wxmsw25_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_msw" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_msw\wxmsw25_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_msw\wxmsw25_odbc.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_msw\wxmsw25_odbc.lib"

!ELSEIF  "$(CFG)" == "odbclib - Win32 LIB Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswud\"
# PROP BASE Intermediate_Dir "vc_mswud\odbclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswud\"
# PROP Intermediate_Dir "vc_mswud\odbclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxmsw25ud_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxmsw25ud_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswud\wxmsw25ud_odbc.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswud\wxmsw25ud_odbc.lib"

!ELSEIF  "$(CFG)" == "odbclib - Win32 LIB wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivu\"
# PROP BASE Intermediate_Dir "vc_mswunivu\odbclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivu\"
# PROP Intermediate_Dir "vc_mswunivu\odbclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswunivu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswunivu\wxmswuniv25u_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MD /O1 /I "..\include" /I "..\lib\vc_mswunivu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswunivu\wxmswuniv25u_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivu\wxmswuniv25u_odbc.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivu\wxmswuniv25u_odbc.lib"

!ELSEIF  "$(CFG)" == "odbclib - Win32 LIB wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivud\"
# PROP BASE Intermediate_Dir "vc_mswunivud\odbclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivud\"
# PROP Intermediate_Dir "vc_mswunivud\odbclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxmswuniv25ud_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswunivud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxmswuniv25ud_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivud\wxmswuniv25ud_odbc.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivud\wxmswuniv25ud_odbc.lib"

!ELSEIF  "$(CFG)" == "odbclib - Win32 LIB Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswd\"
# PROP BASE Intermediate_Dir "vc_mswd\odbclib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswd\"
# PROP Intermediate_Dir "vc_mswd\odbclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxmsw25d_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD CPP /nologo /FD /W4 /MDd /Od /I "..\include" /I "..\lib\vc_mswd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxmsw25d_odbc.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_odbclib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswd\wxmsw25d_odbc.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswd\wxmsw25d_odbc.lib"

!ENDIF

# Begin Target

# Name "odbclib - Win32 LIB wxUniv Release Ansi"
# Name "odbclib - Win32 LIB Native Release Unicode"
# Name "odbclib - Win32 LIB wxUniv Debug Ansi"
# Name "odbclib - Win32 LIB Native Release Ansi"
# Name "odbclib - Win32 LIB Native Debug Unicode"
# Name "odbclib - Win32 LIB wxUniv Release Unicode"
# Name "odbclib - Win32 LIB wxUniv Debug Unicode"
# Name "odbclib - Win32 LIB Native Debug Ansi"
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\db.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbgrid.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbkeyg.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\dbtable.h
# End Source File
# End Group
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\common\db.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dbgrid.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\dbtable.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# End Group
# End Target
# End Project

