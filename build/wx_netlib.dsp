# Microsoft Developer Studio Project File - Name="netlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=netlib - Win32 Static Native Release Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_netlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_netlib.mak" CFG="netlib - Win32 Static Native Release Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "netlib - Win32 Static Native Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "netlib - Win32 Static wxUniv Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "netlib - Win32 Static Native Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "netlib - Win32 Static Native Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "netlib - Win32 Static wxUniv Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "netlib - Win32 Static wxUniv Release Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "netlib - Win32 Static Native Debug Ansi" (based on "Win32 (x86) Static Library")
!MESSAGE "netlib - Win32 Static wxUniv Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "netlib - Win32 Static Native Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswu\"
# PROP BASE Intermediate_Dir "vc_mswu\netlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswu\"
# PROP Intermediate_Dir "vc_mswu\netlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswu\wxbase25u_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswu\wxbase25u_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswu\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswu\wxbase25u_net.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswu\wxbase25u_net.lib"

!ELSEIF  "$(CFG)" == "netlib - Win32 Static wxUniv Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswunivu\"
# PROP BASE Intermediate_Dir "vc_mswunivu\netlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswunivu\"
# PROP Intermediate_Dir "vc_mswunivu\netlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswunivu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswunivu\wxbase25u_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswunivu" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswunivu\wxbase25u_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivu\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivu\wxbase25u_net.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivu\wxbase25u_net.lib"

!ELSEIF  "$(CFG)" == "netlib - Win32 Static Native Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_msw\"
# PROP BASE Intermediate_Dir "vc_msw\netlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_msw\"
# PROP Intermediate_Dir "vc_msw\netlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_msw" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_msw\wxbase25_net.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_msw" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_msw\wxbase25_net.pdb /Yu"wx/wxprec.h" /Fp"vc_msw\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_msw\wxbase25_net.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_msw\wxbase25_net.lib"

!ELSEIF  "$(CFG)" == "netlib - Win32 Static Native Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswud\"
# PROP BASE Intermediate_Dir "vc_mswud\netlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswud\"
# PROP Intermediate_Dir "vc_mswud\netlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxbase25ud_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswud\wxbase25ud_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswud\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswud\wxbase25ud_net.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswud\wxbase25ud_net.lib"

!ELSEIF  "$(CFG)" == "netlib - Win32 Static wxUniv Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivd\"
# PROP BASE Intermediate_Dir "vc_mswunivd\netlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivd\"
# PROP Intermediate_Dir "vc_mswunivd\netlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxbase25d_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivd\wxbase25d_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivd\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivd\wxbase25d_net.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivd\wxbase25d_net.lib"

!ELSEIF  "$(CFG)" == "netlib - Win32 Static wxUniv Release Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\vc_mswuniv\"
# PROP BASE Intermediate_Dir "vc_mswuniv\netlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\vc_mswuniv\"
# PROP Intermediate_Dir "vc_mswuniv\netlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswuniv" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswuniv\wxbase25_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD CPP /nologo /FD /MD /O1 /I "..\include" /I "..\lib\vc_mswuniv" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Fd..\lib\vc_mswuniv\wxbase25_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswuniv\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswuniv\wxbase25_net.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswuniv\wxbase25_net.lib"

!ELSEIF  "$(CFG)" == "netlib - Win32 Static Native Debug Ansi"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswd\"
# PROP BASE Intermediate_Dir "vc_mswd\netlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswd\"
# PROP Intermediate_Dir "vc_mswd\netlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxbase25d_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswd" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswd\wxbase25d_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswd\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswd\wxbase25d_net.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswd\wxbase25d_net.lib"

!ELSEIF  "$(CFG)" == "netlib - Win32 Static wxUniv Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\lib\vc_mswunivud\"
# PROP BASE Intermediate_Dir "vc_mswunivud\netlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\vc_mswunivud\"
# PROP Intermediate_Dir "vc_mswunivud\netlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxbase25ud_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD CPP /nologo /FD /MDd /Od /I "..\include" /I "..\lib\vc_mswunivud" /I "..\src\tiff" /I "..\src\jpeg" /I "..\src\png" /I "..\src\zlib" /I "..\src\regex" /I "..\src\expat\lib" /Zi /Gm /GZ /Fd..\lib\vc_mswunivud\wxbase25ud_net.pdb /Yu"wx/wxprec.h" /Fp"vc_mswunivud\wxprec_netlib.pch" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D wxUSE_UNICODE=1 /D "WXUSINGDLL" /D "WXMAKINGDLL_NET" /c
# ADD BASE RSC /l 0x405
# ADD RSC /l 0x405
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\vc_mswunivud\wxbase25ud_net.lib"
# ADD LIB32 /nologo /out:"..\lib\vc_mswunivud\wxbase25ud_net.lib"

!ENDIF

# Begin Target

# Name "netlib - Win32 Static Native Release Unicode"
# Name "netlib - Win32 Static wxUniv Release Unicode"
# Name "netlib - Win32 Static Native Release Ansi"
# Name "netlib - Win32 Static Native Debug Unicode"
# Name "netlib - Win32 Static wxUniv Debug Ansi"
# Name "netlib - Win32 Static wxUniv Release Ansi"
# Name "netlib - Win32 Static Native Debug Ansi"
# Name "netlib - Win32 Static wxUniv Debug Unicode"
# Begin Group "Common Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\wx\fs_inet.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\gsocket.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\protocol\file.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\protocol\ftp.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\protocol\http.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\protocol\protocol.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sckaddr.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sckipc.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\sckstrm.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\socket.h
# End Source File
# Begin Source File

SOURCE=..\include\wx\url.h
# End Source File
# End Group
# Begin Group "Common Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\common\fs_inet.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\ftp.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\http.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\protocol.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\sckaddr.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\sckfile.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\sckipc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\sckstrm.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\socket.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\url.cpp
# End Source File
# End Group
# Begin Group "MSW Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\msw\dummy.cpp
# ADD BASE CPP /Yc"wx/wxprec.h"
# ADD CPP /Yc"wx/wxprec.h"
# End Source File
# Begin Source File

SOURCE=..\src\msw\gsocket.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\src\msw\gsockmsw.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Target
# End Project

