@echo off
rem Zip up a patch file
Rem m:\wx2 contains the latest version + any crucial patches
set src=m:\wx2
set dest=%wxwin\deliver
set wise=0

Rem Set this to the required patch version
set version=03

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

zip32 -@ %dest\patch%version%.zip < %wxwin\distrib\msw\patch.rsp
echo wxWindows patch archived.

cd %dest

goto end

:usage
echo DOS wxWindows patch distribution.
echo.
echo Usage: zippatch

:end
