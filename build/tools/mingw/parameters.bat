@echo off

rem Number of jobs for parallels builds
if "%NUMBER_OF_PROCESSORS%" == ""  set NUMBER_OF_PROCESSORS=2
set /A JOBS=%NUMBER_OF_PROCESSORS%-1

rem Removing (if = 1) or not (otherwise) actuals build and lib dirs
set CLEANBUILD=1

rem Check if compiler version is present
if "%1" == "" goto :NOPARAMS
rem Check if architecture is present
if "%2" == "" goto :NOPARAMS

rem Define compiler's bin directory
if "%1" == "1020" (
    set COMPBINDIR=G:\msys64\mingw%2\bin
) else (
    set COMPBINDIR=G:\MinGW%1-%2\bin
)
rem Define compiler's version string such as 810, 730_x64, 920TDM
rem Also define extra spaces for "aligned" logs lines
set COMPVERS=%1
set XTRASPCS= :
if "%1" == "920" (
    set COMPVERS=%COMPVERS%TDM
) else (
    set XTRASPCS=   %XTRASPCS%
)
if "%2" == "64" (
    set COMPVERS=%COMPVERS%_x64
) else (
    set XTRASPCS=    %XTRASPCS%
)
rem Define compiler's name, such as gcc810, gcc730_x64, gcc920TDM, ...
set COMPNAME=gcc%COMPVERS%
rem Simplified (but sufficient) PATH variable
set PATH=C:\Windows;C:\Windows\system32\;%COMPBINDIR%

goto :EOF

rem Generals parameters
:NOPARAMS
rem Normalized wxWidgets base dir
set WXDIR=%CD%\..\..\..
for %%i in ("%WXDIR%") do SET "WXDIR=%%~fi"
rem wxWidgets version
call ..\msvs\getversion.bat
rem Logs and output
set LOGDIR=%CD%\logs
set OUTPUTDIR=%CD%\output
SET MAINLOGFILE=%LOGDIR%\_wxWidgets-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%.log
rem Base of each created zip archive
set ARCHNAME=wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_
rem 7Zip executable
set ZIP="C:\Program Files\7-Zip\7z.exe"
