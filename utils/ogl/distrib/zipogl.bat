@echo off
rem Zip up an external distribution of OGL
set src=d:\wx2\wxWindows\utils\ogl
set dest=%src\deliver

if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to archive an external OGL distribution:
echo   From   %src
echo   To     %dest\ogl.zip
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %dest\ogl.zip
cd %src
zip32 -@ %dest\ogl.zip < %src\distrib\ogl.rsp

echo OGL archived.
goto end

:usage
echo DOS OGL distribution.
echo Usage: zipogl source destination
echo e.g. zipogl c:\wx\utils\ogl c:\wx\utils\ogl\deliver

:end


