@echo off
rem Zip up an external distribution of wxXML
set src=%WXWIN%
set dest=%WXWIN%\deliver

if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to archive an external wxXML distribution:
echo   From   %src
echo   To     %dest\xml.zip
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %dest\xml.zip
cd %src
zip32 -@ %dest\xml.zip < %src\distrib\msw\xml.rsp

cd %dest

echo wxXML archived.
goto end

:usage
echo DOS wxXML distribution.
echo Usage: zipxml source destination
echo e.g. zipxml d:\wx2\wxWidgets d:\wx2\wxWidgets\deliver

:end


