@echo off
rem Zip up an external distribution of OGL

if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external OGL distribution:
echo   From   %1
echo   To     %2\ogl.zip
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\ogl.zip
cd %1
zip -P %3 %4 %5 %6 %7 %8 %2\ogl.zip @%1\distrib\ogl.rsp

echo OGL archived.
goto end

:usage
echo DOS OGL distribution.
echo Usage: zipogl source destination
echo e.g. zipogl c:\wx\utils\ogl c:\wx\utils\ogl\deliver

:end


