@echo off
rem Zip up a patch file

if "%wxwin%" == "" goto usage

set src=%wxwin%
set dest=%src%\deliver

Rem Set this to the required wxWindows version
set wxversion=2.3.3
Rem Set this to the required patch version
set version=01

set patchname=wxWindows-%wxversion%-patch-%version%.zip

if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to create %dest%\%patchname%
echo   From   %src
echo   To     %dest
echo CTRL-C if this is not correct.
pause

erase /Q /E %dest%\wxWindows-*-patch*.zip

cd %src%
echo Zipping...

zip -@ %dest%\%patchname% < %src%\distrib\msw\patch.rsp
echo wxWindows patch archived.

cd %dest

goto end

:usage
echo wxWindows patch distribution.
echo.
echo Usage: zippatch
echo You must set WXWIN appropriately before calling this script.

:end
