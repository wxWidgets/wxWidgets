@echo off
rem Zip up a wxMac distribution, .zip and .sit
rem using Inno Setup + ScriptMaker
set src=%wxwin
set dest=%src\deliver
set webfiles=c:\wx2dev\wxWebSite
set stuffit="c:\Program Files\Aladdin Systems\StuffIt 7.5\StuffIt.exe"

Rem Set this to the required version
set version=2.3.4

if "%src" == "" goto usage
if "%dest" == "" goto usage
if "%1" == "-help" goto usage
if "%1" == "--help" goto usage
if "%1" == "/?" goto usage
echo About to archive an external wxMac distribution:
echo   From   %src
echo   To     %dest
echo CTRL-C if this is not correct.
pause

erase %dest\wxMac*.zip
erase %dest\wxMac*.sit

if not direxist %dest mkdir %dest
if direxist %dest%\wxWindows-%version% erase /sxyz %dest%\wxWindows-%version%

echo Zipping wxMac distribution

cd %src%
copy %src%\include\wx\mac\setup0.h %src%\include\wx\setup.h

zip -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\generic.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\mac.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\cw_mac.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\tiff.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\jpeg.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\utils.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\tex2rtf.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\dialoged.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\ogl.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\xml.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\stc.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\contrib.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\makefile.rsp

erase /Y %src%\include\wx\setup.h

if direxist %dest%\wxMac-%version% erase /sxyz %dest%\wxMac-%version%

mkdir %dest%\wxMac-%version%
cd %dest%\wxMac-%version%
unzip ..\wxMac-%version%.zip
erase /Y src\gtk\descrip.mms src\motif\descrip.mms docs\pdf\*.pdf
erase /SXY docs\html\dialoged docs\html\tex2rtf

cd %dest%

erase wxMac-%version%.zip
zip -r wxMac-%version%.zip wxMac-%version%/*

erase /sxyz %dest%\wxMac-%version%

Rem echo Making StuffIt archive...
Rem set stuffitcmd=%stuffit% -stuff -create wxMac-%version%.sit wxWindows-%version%\*
Rem echo Invoking %stuffitcmd...
Rem start "StuffIt" /w %stuffitcmd%

echo wxMac archived.

goto end

:usage
echo DOS wxWindows distribution. Zips up wxMac.
echo.
echo "Usage: zipwxmac"

:end
