@echo off
rem Zip up a wxOS2 distribution.
set src=%wxwin
set dest=%src\deliver
set webfiles=c:\wx2dev\wxWebSite

Rem Set this to the required version
set version=2.3.4

if "%src" == "" goto usage
if "%dest" == "" goto usage
if "%1" == "-help" goto usage
if "%1" == "--help" goto usage
if "%1" == "/?" goto usage
echo About to archive an external wxOS2 distribution:
echo   From   %src
echo   To     %dest
echo CTRL-C if this is not correct.
pause

erase %dest\wxOS2*.zip

if not direxist %dest mkdir %dest
if direxist %dest%\wxWindows-%version% erase /sxyz %dest%\wxWindows-%version%

echo Zipping wxOS2 distribution

cd %src%
Rem copy %src%\include\wx\os2\setup0.h %src%\include\wx\setup.h

zip -@ %dest\wxOS2-%version%.zip < %src\distrib\msw\generic.rsp
zip -@ -u %dest%\wxOS2-%version%.zip < %src\distrib\msw\os2.rsp
zip -@ -u %dest%\wxOS2-%version%.zip  < %src\distrib\msw\jpeg.rsp
zip -@ -u %dest%\wxOS2-%version%.zip < %src\distrib\msw\tiff.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\tiff.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\jpeg.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\utils.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\tex2rtf.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\dialoged.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\ogl.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\xml.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\contrib.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\makefile.rsp

Rem erase /Y %src%\include\wx\setup.h

if direxist %dest%\wxOS2-%version% erase /sxyz %dest%\wxOS2-%version%

mkdir %dest%\wxOS2-%version%
cd %dest%\wxOS2-%version%
unzip ..\wxOS2-%version%.zip
echo Overwriting with OS2-specific versions of configure files...
unzip -o %src%\distrib\os2\os2-specific.zip
erase /Y src\gtk\descrip.mms src\motif\descrip.mms docs\pdf\*.pdf
erase /Y src\tiff\*.mcp src\jpeg\*.mcp src\png\*.mcp src\zlib\*.mcp
erase /SXY docs\html\dialoged docs\html\tex2rtf

echo Making files lower case...
cd src\os2
call %src%\distrib\msw\lower.bat
cd ..\..\include\wx\os2
call %src%\distrib\msw\lower.bat

cd %dest%

erase wxOS2-%version%.zip
zip -r wxOS2-%version%.zip wxOS2-%version%/*
erase /sxyz wxOS2-%version%

echo wxOS2 archived.

goto end

:usage
echo DOS wxWindows distribution. Zips up wxOS2.
echo.
echo "Usage: zipwxos2"

:end
