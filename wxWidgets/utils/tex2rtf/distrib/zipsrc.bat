@echo off
rem Zip up a source distribution of Tex2RTF and associated apps.
set src=c:\wx2dev\wxWindows\utils\tex2rtf
set dest=%src\deliver
if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to archive Tex2RTF
echo   From   %src
echo   To     %dest
echo CTRL-C if this is not correct.
pause

erase %dest\tex2rtf_src.zip

cd %src
echo Zipping...

zip32 -@ %dest\tex2rtf_src.zip < %src\tex2rtf\distrib\src.rsp

cd %dest

echo Tex2RTF source archived.
goto end

:usage
echo Tex2RTF source distribution.

:end
