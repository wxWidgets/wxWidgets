@echo off
if "%WXWIN%=="" goto nowxdir
if "%1"=="" goto bad
if not exist %1.cpp goto nocpp
find /c "BIDE" %1.cpp
if not errorlevel 1 goto already




@echo off
echo #this is for mak/bpr files in  sample\dir  > %1.mak

echo #-----------------------------------------------------------------------------  >> %1.mak
echo #this is a source makefile for Borland CBuilder IDE v1,3  >> %1.mak
echo #You may need to add \ at the end of the lines if your editor breaks them up  >> %1.mak
echo #-----------------------------------------------------------------------------  >> %1.mak

echo PROJECT = %1.exe >> %1.mak  >> %1.mak
echo RESFILES = %1.res >> %1.mak  >> %1.mak

echo OBJFILES =  >> %1.mak
echo RESDEPEN = $(RESFILES)  >> %1.mak
echo LIBFILES =   >> %1.mak

echo !ifndef BCB  >> %1.mak
echo BCB = $(MAKEDIR)\..  >> %1.mak
echo !endif  >> %1.mak
echo #-----------------------------------------------------------------------------  >> %1.mak
echo CFLAG1 = -Od -v -a1 -c -w-hid  >> %1.mak
echo #disable optimisations debug byte align (matches my make for the library)  >> %1.mak
echo CFLAG2 = -DINC_OLE2;__WIN95__;__WXMSW__;__WINDOWS__;WIN32;__BIDE__ -I$(BCB)\include;$(BCB)\include\vcl;%WXWIN%\include -H=BC32.CSM   >> %1.mak
echo PFLAGS =   >> %1.mak
echo RFLAGS = -DINC_OLE2;__WIN95__;__WXMSW__;__WINDOWS__;WIN32;__BIDE__ -I$(BCB)\include;%WXWIN%\include  >> %1.mak
echo LFLAGS = -L$(BCB)\lib;$(BCB)\lib\obj;%WXWIN%\lib -aa -Tpe -v -V4.0 -c   >> %1.mak
echo IFLAGS =   >> %1.mak
echo LINKER = ilink32   >> %1.mak
echo #-----------------------------------------------------------------------------  >> %1.mak
echo ALLOBJ = c0w32.obj $(OBJFILES)  >> %1.mak
echo ALLRES = $(RESFILES)  >> %1.mak
rem xpm.lib seems to have disappeared from libraries ?? 14 Jun 2001
rem echo ALLLIB = $(LIBFILES) vcl.lib xpm.lib wx32.lib ole2w32.lib import32.lib cw32mt.lib odbc32.lib winpng.lib tiff.lib jpeg.lib zlib.lib  >> %1.mak
echo ALLLIB = $(LIBFILES) vcl.lib wx32.lib ole2w32.lib import32.lib cw32mt.lib odbc32.lib winpng.lib tiff.lib jpeg.lib zlib.lib  >> %1.mak
echo #---------------------------------------------------------------------------  >> %1.mak
echo .autodepend  >> %1.mak
echo.  >> %1.mak

echo #-----------------------------------------------------------------------------  >> %1.mak
echo $(PROJECT): $(OBJFILES) $(RESDEPEN)  >> %1.mak


copy %1.mak %1.bpr
rem bpr is for cb4 mak for cb1
copy %1.cpp %1_old.cpp

echo //Source code file for CBuilder/ wxWindows modified by Chris Elliott (biol75@.york.ac.uk) > %1.cpp
echo #ifdef __BIDE__ >> %1.cpp
echo #define _NO_VCL >> %1.cpp
echo #include "condefs.h" >> %1.cpp
echo #define WinMain WinMain >> %1.cpp

if "%2"=="" goto skipfile
echo     USEUNIT ("%2.cpp"); // use a line like this if you have more than one .cpp file >> %1.cpp
if "%3"=="" goto skipfile
echo     USEUNIT ("%3.cpp"); // use a line like this if you have more than one .cpp file >> %1.cpp
if "%4"=="" goto skipfile
echo     USEUNIT ("%4.cpp"); // use a line like this if you have more than one .cpp file >> %1.cpp
if "%5"=="" goto skipfile
echo     USEUNIT ("%5.cpp"); // use a line like this if you have more than one .cpp file >> %1.cpp
:skipfile
echo     USERC ("%1.rc"); >> %1.cpp
echo #endif >> %1.cpp

type %1_old.cpp >> %1.cpp

goto end

:nowxdir
echo Please use the MSDOS command SET WXWIN=DRV:PATH_TO_WX before trying this batch file
echo e.g. SET WXWIN=c:\wx
goto end

:bad
echo No file to modify passed on command line : 
echo    Usage: process_sample_bcb filename_without_ext
echo No file to modify passed on command line : Usage process_sample_bcb filename_without_ext >> %WXWIN%\distrib\msw\proc_bcb.log
goto end


:nocpp
echo original %1.cpp file not found 
echo original %1.cpp file not found >> %WXWIN%\distrib\msw\proc_bcb.log
goto end 

:already
echo Source file %1.cpp appears to already be converted  with __BIDE__ defined
echo Source file %1.cpp appears to already be converted  with __BIDE__ defined >> %WXWIN%\distrib\msw\proc_bcb.log




:end
