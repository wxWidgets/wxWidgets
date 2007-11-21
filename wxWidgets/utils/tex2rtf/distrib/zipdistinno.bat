@echo off
rem Zip up a distribution of Tex2RTF using Inno Setup.
Rem This batch file requires the 4DOS/4NT command processor.
set src=%WXWIN%\utils\tex2rtf
set dest=%WXWIN%\deliver
set inno=1
set ReleaseDir=Release

if "%src%" == "" goto usage
if "%dest%" == "" goto usage

if "%1" == "innoonly" set inno=1
if "%1" == "innoonly" goto dounzip
if "%1" == "inno" set inno=1

echo About to archive Tex2RTF
echo   From   %src%
echo   To     %dest%
echo CTRL-C if this is not correct.
pause

erase /E %dest%\tex2rtf.zip
erase /E %dest%\tex2rtf_setup.exe
erase /E %dest\tex2rtf_src.zip

if direxist %dest%\tex2rtf erase /sxzy %dest%\tex2rtf\

Rem *** First, determine version from symbols.h
call ming2952
echo `#include <stdio.h>` > tex2rtfver.c
echo `#include "symbols.h"` >> tex2rtfver.c
echo `int main() { printf("%.2f", TEX2RTF_VERSION_NUMBER); }` >> tex2rtfver.c
gcc tex2rtfver.c -I%src%\src -o tex2rtfver.exe
tex2rtfver.exe > tex2rtfversion.txt
set ver=%@LINE[tex2rtfversion.txt,0]
echo %ver%
erase tex2rtfver.exe tex2rtfver.c
set ver_filename=%ver%

echo Updating docs...
call msvc6

pushd %src%\docs
erase /Y tex2rtf_contents.html
nmake -f makefile.vc htmlhelp htb
popd

cd %src%
echo Zipping source...

zip32 -@ %src%\tex2rtf_src.zip < %src%\distrib\src.rsp

echo Adding files...

copy %src%\src\%ReleaseDir%\tex2rtf.exe %src%
copy %src%\docs\licence.txt %src%
copy %src%\docs\lgpl.txt %src%
copy %src%\docs\gpl.txt %src%
copy %src%\docs\readme.txt %src%
copy %src%\docs\tex2rtf.chm %src%
copy %src%\distrib\tex2rtf.exe.manifest %src%
copy %src%\tools\lacheck.exe %src%
copy %src%\tools\lacheck.txt %src%
Rem goto end

upx %src%\tex2rtf.exe

zip -@ %dest%\tex2rtf.zip < %src%\distrib\distrib.rsp
zip -j -u %dest%\tex2rtf.zip %src%\tex2rtf.exe

:dounzip

cd %dest%

mkdir tex2rtf
cd tex2rtf
unzip32 -o ..\tex2rtf.zip

cd %dest%

echo inno = %inno%

Rem Skip Inno Setup setup if inno is 0.
if "%inno%" == "0" goto end

set tex2rtfver=%ver%

echo ver is %ver%
echo tex2rtfver is %tex2rtfver%

echo Calling 'makeinno' to generate tex2rtf.iss...
cd %dest%\tex2rtf
call %src%\distrib\makeinno.bat

sed -e "s/TEX2RTFVERSION/%tex2rtfver%/g" %src%\distrib\tex2rtf.iss > %TEMP%\tex2rtf.iss

move %TEMP%\tex2rtf.iss %src%\distrib\tex2rtf.iss

rem Now invoke Inno Setup install on the new tex2rtf.iss
set innocmd="C:\Program Files\Inno Setup 2\compil32.exe" /cc %src%\distrib\tex2rtf.iss
echo Invoking %innocmd%...
cd %dest%
start "Inno Setup" /w %innocmd%

Rem Rename to give it a version
move setup.exe tex2rtf_%ver_filename%_setup.exe

Rem Copy the source
move %src%\tex2rtf_src.zip %dest%\tex2rtf-source-%tex2rtfver%.zip

Rem clean up files
cd %src%
erase /EFY *.bmp *.htm *.exe *.hlp *.cnt *.txt *.rtf *.doc *.gif *.jpg *.manifest *.chm

cd %dest%

echo Tex2RTF archived.
goto end

:usage
echo Tex2RTF distribution.

:end
