# Microsoft Developer Studio Project File - Name="wx_wxtiff" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wxtiff - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxtiff.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wx_wxtiff.mak" CFG="wxtiff - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxtiff - Win32 DLL Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 DLL Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wxtiff - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxtiff - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswudll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswudll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiff.pdb /opt:ref /opt:icf /I "..\..\src\zlib" /I "..\..\src\jpeg" /I "..\..\src\tiff\libtiff" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_CRT_NONSTDC_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiff.pdb /opt:ref /opt:icf /I "..\..\src\zlib" /I "..\..\src\jpeg" /I "..\..\src\tiff\libtiff" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_CRT_NONSTDC_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_dll"
# PROP BASE Intermediate_Dir "vc_mswuddll\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_dll"
# PROP Intermediate_Dir "vc_mswuddll\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /I "..\..\src\tiff\libtiff" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_CRT_NONSTDC_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_dll\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /I "..\..\src\tiff\libtiff" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_CRT_NONSTDC_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_dll\wxtiffd.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswu\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswu\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiff.pdb /opt:ref /opt:icf /I "..\..\src\zlib" /I "..\..\src\jpeg" /I "..\..\src\tiff\libtiff" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_CRT_NONSTDC_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MD /W1 /O2 /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiff.pdb /opt:ref /opt:icf /I "..\..\src\zlib" /I "..\..\src\jpeg" /I "..\..\src\tiff\libtiff" /D "WIN32" /D "_LIB" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_CRT_NONSTDC_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiff.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiff.lib"

!ELSEIF  "$(CFG)" == "wxtiff - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\lib\vc_lib"
# PROP BASE Intermediate_Dir "vc_mswud\wxtiff"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\vc_lib"
# PROP Intermediate_Dir "vc_mswud\wxtiff"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /I "..\..\src\tiff\libtiff" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_CRT_NONSTDC_NO_WARNINGS" /c
# ADD CPP /nologo /FD /MDd /W1 /Od /Gm /GR /EHsc /Zi /Fd..\..\lib\vc_lib\wxtiffd.pdb /I "..\..\src\zlib" /I "..\..\src\jpeg" /I "..\..\src\tiff\libtiff" /D "WIN32" /D "_LIB" /D "_DEBUG" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "_CRT_NONSTDC_NO_WARNINGS" /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiffd.lib"
# ADD LIB32 /nologo /out:"..\..\lib\vc_lib\wxtiffd.lib"

!ENDIF

# Begin Target

# Name "wxtiff - Win32 DLL Release"
# Name "wxtiff - Win32 DLL Debug"
# Name "wxtiff - Win32 Release"
# Name "wxtiff - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_aux.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_close.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_codec.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_color.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_compress.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_dir.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_dirinfo.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_dirread.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_dirwrite.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_dumpmode.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_error.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_extension.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_fax3.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_fax3sm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_flush.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_getimage.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_jbig.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_jpeg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_jpeg_12.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_luv.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_lzma.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_lzw.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_next.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_ojpeg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_open.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_packbits.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_pixarlog.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_predict.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_print.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_read.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_strip.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_swab.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_thunder.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_tile.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_version.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_warning.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_win32.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_write.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tiff\libtiff\tif_zip.c
# End Source File
# End Group
# End Target
# End Project

