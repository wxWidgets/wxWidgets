@echo off
rem Zip up an external distribution of OGL
set src=d:\wx2\wxWindows
set dest=%src\utils\ogl\deliver

if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to archive an external OGL distribution:
echo   From   %src
echo   To     %dest\ogl3.zip
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %dest\ogl3.zip
cd %src
zip32 -@ %dest\ogl3.zip < %src\utils\ogl\distrib\ogl.rsp

cd %dest

echo OGL archived.
goto end

:usage
echo DOS OGL distribution.
echo Usage: zipogl source destination
echo e.g. zipogl d:\wx2\wxWindows d:\wx2\wxWindows\utils\ogl\deliver

:end


