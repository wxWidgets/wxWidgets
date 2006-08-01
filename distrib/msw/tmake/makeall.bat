@echo off
rem File:      makeall.bat
rem Purpose:   create wxWindows makefiles for all compilers
rem Author:    Vadim Zeitlin
rem Created:   14.07.99
rem Copyright: (c) 1999 Vadim Zeitlin
rem Licence:   wxWindows Licence
rem Version:   $Id$
rem
rem Makefile to create the makefiles for all compilers from the templates using
rem tmake. The environment variable WX or WXWIN should be defined and contain
rem the root directory of wxWindows installation. TMAKE program should be in
rem path or, alternatively, TMAKE environment variable should be set.

if "x%WX%" == "x" goto skip
set WXDIR=%WX%
goto ok

:skip
if "x%WXWIN%" == "x" goto no_wx
set WXDIR=%WXWIN%
goto ok

:no_wx
echo "Please set WX environment variable!"
goto end

:ok
rem Let's assume that you have tmake in your PATH but let override it with
rem TMAKE env var
set TM=call tmake
if "x%TMAKE%" == "x" goto skip2
set TM=%TMAKE%

:skip2

rem this loop just doesn't want to work under command.com and cmd.exe and
rem 4nt.exe, so I preferred to unroll it.
rem for %%c in (b32 bcc dos g95 sc vc wat) %TM% -t %c% wxwin.pro -o %WXDIR%\src\msw\makefile.%c%
echo Generating for Visual C++ 6.0 wxBase C++ (32 bits)...
%TM% -t basevc wxwin.pro -o %WXDIR%\src\msw\makebase.vc
echo Generating for Visual C++ 6.0 wxWindows.dsp
%TM% -t vc6msw wxwin.pro -o %WXDIR%\src\wxWindows.dsp
echo Generating for Visual C++ 6.0 wxUniv.dsp
%TM% -t vc6univ wxwin.pro -o %WXDIR%\src\wxUniv.dsp
echo Generating for Visual C++ 6.0 wxBase.dsp
%TM% -t vc6base wxwin.pro -o %WXDIR%\src\wxBase.dsp
echo Generating for Visual C++ 4.0...
%TM% -t vc wxwin.pro -o %WXDIR%\src\msw\makefile.vc
echo Generating for Borland C++ (32 bits)...
%TM% -t b32 wxwin.pro -o %WXDIR%\src\msw\makefile.b32
%TM% -t b32base wxwin.pro -o %WXDIR%\src\msw\makebase.b32
echo Generating for Borland C++ (16 bits)...
%TM% -t bcc wxwin.pro -o %WXDIR%\src\msw\makefile.bcc
echo Generating for Cygwin/Mingw32
%TM% -t g95 wxwin.pro -o %WXDIR%\src\msw\makefile.g95
echo Generating for Watcom C++...
%TM% -t wat wxwin.pro -o %WXDIR%\src\msw\makefile.wat
echo Generating for Unix and Configure...
%TM% -t gtk wxwin.pro -o %WXDIR%\src\gtk\files.lst
%TM% -t mgl wxwin.pro -o %WXDIR%\src\mgl\files.lst
%TM% -t micro wxwin.pro -o %WXDIR%\src\microwin\files.lst
%TM% -t univ wxwin.pro -o %WXDIR%\src\univ\files.lst
%TM% -t msw wxwin.pro -o %WXDIR%\src\msw\files.lst
%TM% -t mac wxwin.pro -o %WXDIR%\src\mac\files.lst
%TM% -t motif wxwin.pro -o %WXDIR%\src\motif\files.lst
%TM% -t x11 wxwin.pro -o %WXDIR%\src\x11\files.lst
%TM% -t os2 wxwin.pro -o %WXDIR%\src\os2\files.lst
:end
