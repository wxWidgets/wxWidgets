@echo off
rem Zip up a minimal wxMSW source distribution
set src=%wxwin
set dest=%src\deliver
set webfiles=c:\wx2dev\wxWebSite

Rem Set this to the required version
set version=2.3.3

if "%src" == "" goto usage
if "%dest" == "" goto usage
if "%1" == "-help" goto usage
if "%1" == "--help" goto usage
if "%1" == "/?" goto usage
echo About to archive an Minimal wxMSW distribution:
echo   From   %src
echo   To     %dest
echo CTRL-C if this is not correct.
pause

erase %dest\wxMSW-%VERSION%-Minimal*.zip

if not direxist %dest mkdir %dest
if direxist %dest%\wxWindows-%version% erase /sxyz %dest%\wxWindows-%version%

echo Zipping wxMSW-Minimal distribution

cd %src%

zip32 -@ %dest%\wxMSW-%version%-Minimal.zip < %src%\distrib\msw\msw_minimal.rsp
zip32 -u -@ %dest%\wxMSW-%version%-Minimal.zip < %src%\distrib\msw\jpeg.rsp
zip32 -u -@ %dest%\wxMSW-%version%-Minimal.zip < %src%\distrib\msw\png.rsp
zip32 -u -@ %dest%\wxMSW-%version%-Minimal.zip < %src%\distrib\msw\zlib.rsp
zip32 -u -@ %dest%\wxMSW-%version%-Minimal.zip < %src%\distrib\msw\tiff.rsp

if direxist %dest%\wxWindows-%version% erase /sxyz %dest%\wxWindows-%version%

mkdir %dest%\wxWindows-%version%
cd %dest%\wxWindows-%version%

unzip32 ..\wxMSW-%version%-Minimal.zip
erase /Y src\gtk\descrip.mms src\motif\descrip.mms docs\pdf\*.pdf

echo Deleting some redundant files

erase /XY src\generic\treectrlg.cpp include\wx\generic\treectrlg.cpp
erase /XY src\generic\listctrl.cpp include\wx\generic\listctrl.cpp
Rem erase /XY src\generic\prop*.cpp include\wx\generic\prop*.cpp
erase /XY src\generic\printps.cpp src\generic\fontdlgg.cpp src\generic\accel.cpp src\generic\colrdlgg.cpp
erase /XY include\wx\generic\printps.cpp include\wx\generic\fontdlgg.h include\wx\generic\accel.h include\wx\generic\colrdlgg.h
erase /XY include\wx\generic\mdig.h src\generic\mdig.cpp
erase /XY include\wx\*cw*.h

erase /SXY docs\html\dialoged docs\html\tex2rtf

cd %dest%

erase wxMSW-%version%-Minimal.zip
zip32 -r wxMSW-%version%-Minimal.zip wxWindows-%version%/*

echo wxMSW-Minimal archived.

cd %src%\distrib\msw

goto end

:usage
echo DOS wxWindows distribution. Zips up wxMSW-Minimal.
echo.
echo "Usage: zipwinminimal.bat"

:end
