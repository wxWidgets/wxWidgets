@echo off
rem Zip up a patch file

if "%wxwin%" == "" goto usage

set src=%wxwin%
set dest=%src%\deliver

Rem Set this to the required wxWidgets version
set wxversion=2.3.3
Rem Set this to the required patch version
set version=01

set patchname=wxWidgets-%wxversion%-patch-%version%.zip

if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to create %dest%\%patchname%
echo   From   %src
echo   To     %dest
echo CTRL-C if this is not correct.
pause

erase /Q /E %dest%\wxWidgets-*-patch*.zip

cd %src%
echo Zipping...

zip32 -@ %dest%\%patchname% < %src%\distrib\msw\patch.rsp
echo wxWidgets patch archived.

cd %dest

goto end

:usage
echo wxWidgets patch distribution.
echo.
echo Usage: zippatch
echo You must set WXWIN appropriately before calling this script.

:end
