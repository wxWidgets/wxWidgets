@echo off
rem Zip up an external source distribution of GLCanvas
set src=%1
set dest=%2
if "%src" == "" set src=%WXWIN\utils\glcanvas
if "%dest" == "" set dest=%WXWIN\utils\glcanvas\deliver
echo About to archive an external GLCanvas distribution:
echo   From   %src
echo   To     %dest\glcanvas.zip
echo CTRL-C if this is not correct.
inkey /W10 `Press any key to continue...` %%input

erase %dest\glcanvas.zip
cd %src

zip32 -@ %dest\glcanvas.zip < %src\distrib\glcanvas.rsp

echo GLCanvas archived.
goto end

:usage
echo GLCanvas distribution.
echo Usage: zipsrc source destination

:end


