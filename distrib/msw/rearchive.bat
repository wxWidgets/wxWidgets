@echo off
rem Convert a zip archive to one rooted at a particular directory.
rem This script is used by zipdistinno.bat.

set archive=%1
set dirname=%2
set changeto=%3

if "%archive%" == "" goto usage
if "%dirname%" == "" goto usage
if "%1" == "-help" goto usage
if "%1" == "--help" goto usage
if "%1" == "/?" goto usage

pushd %changeto%

if direxist %dirname% erase /sxyz %dirname%
mkdir %dirname%
cd %dirname%
unzip ..\%archive%
cd ..
erase %archive%
zip -r %archive% %dirname%/*

popd

goto end

:usage
echo rearchive archive.zip dir changetodir
echo E.g. rearchive wxWindows-2.3.3-html.zip wxWindows-2.3.3 c:\wx2dev\wxWindows\deliver

:end
