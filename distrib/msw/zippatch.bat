@echo off
rem Zip up a patch file
set src=%wxwin
set dest=%src\deliver
set wise=0

Rem Set this to the required patch version
set version=01

if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to archive patch%version%.zip
echo   From   %src
echo   To     %dest
if "%wise" == "1" echo with WISE setup creation.
echo CTRL-C if this is not correct.
pause

erase %dest\patch*.zip

cd %src
echo Zipping...

zip32 -@ %dest\patch%version%.zip < %src\distrib\msw\patch.rsp
echo wxWindows patch archived.

cd %dest

goto end

:usage
echo DOS wxWindows patch distribution.
echo.
echo Usage: zippatch

:end
