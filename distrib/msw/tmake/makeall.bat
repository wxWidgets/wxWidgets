@echo off
rem File:      makeall.bat
rem Purpose:   create wxWindows makefiles for all compilers
rem Author:    Vadim Zeitlin
rem Created:   14.07.99
rem Copyright: (c) 1999 Vadim Zeitlin
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
set TM=tmake
if "x%TMAKE%" == "x" goto skip2
set TM=%TMAKE%

:skip2

rem this loop just doesn't want to work under command.com and cmd.exe and
rem 4nt.exe, so I preferred to unroll it.
rem for %%c in (b32 bcc dos g95 sc vc wat) %TM% -t %c% wxwin.pro -o %WXDIR%\src\msw\makefile.%c%

echo Generating for Visual C++ 4.0...
E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake -t vc wxwin.pro -o %WXDIR%\src\msw\makefile.vc
echo Generating for Borland C++ (32 bits)...
E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake -t b32 wxwin.pro -o %WXDIR%\src\msw\makefile.b32
echo Generating for Visual C++ 1.52...
E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake -t dos wxwin.pro -o %WXDIR%\src\msw\makefile.dos
echo Generating for Borland C++ (16 bits)...
E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake -t bcc wxwin.pro -o %WXDIR%\src\msw\makefile.bcc
echo Generating for Cygwin/Mingw32
E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake -t g95 wxwin.pro -o %WXDIR%\src\msw\makefile.g95
echo Generating for Symantec C++...
E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake -t sc wxwin.pro -o %WXDIR%\src\msw\makefile.sc
echo Generating for Watcom C++...
E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake -t wat wxwin.pro -o %WXDIR%\src\msw\makefile.wat
echo Generating for Unix and Configure...
E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake -t gtk wxwin.pro -o %WXDIR%\src\gtk\files.lst
E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake -t motif wxwin.pro -o %WXDIR%\src\motif\files.lst
E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake -t os2 wxwin.pro -o %WXDIR%\src\os2\files.lst
:end
