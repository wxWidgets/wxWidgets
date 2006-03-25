@echo off
rem Zip up an external distribution of STC (wxStyledTextCtrl)
set src=d:\wx2\wxWindows
set dest=d:\wx2\wxWindows\deliver

if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to archive an external STC distribution:
echo   From   %src
echo   To     %dest\stc.zip
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %dest\stc.zip
cd %src
zip32 -@ %dest\stc.zip < %src\distrib\msw\stc.rsp

cd %dest

echo STC archived.
goto end

:usage
echo DOS STC distribution.
echo Usage: zipstc source destination
echo e.g. zipstc d:\wx2\wxWindows d:\wx2\wxWindows\deliver

:end


