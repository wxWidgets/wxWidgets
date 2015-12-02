@echo off
setlocal enabledelayedexpansion

rem This script builds the CHM manual and the Windows installer for release.

if "%1" == "" (
    echo You must specify a release version.
    exit /b 1
)

pushd "%~dp0..\.."

set root=%cd%
set version=%1
set prefix=wxWidgets-%1
set release_path=%root%\distrib\release\%version%

where /q 7z
if %ERRORLEVEL% neq 0 (
    echo Error: 7-Zip was not found in your PATH.
    exit /b 1
)
where /q dot
if %ERRORLEVEL% neq 0 (
    echo Error: GraphViz was not found in your PATH.
    exit /b 1
)
where /q doxygen
if %ERRORLEVEL% neq 0 (
    echo Error: Doxygen was not found in your PATH.
    exit /b 1
)
where /q hhc
if %ERRORLEVEL% neq 0 (
    echo Error: HTML Help Workshop was not found in your PATH.
    exit /b 1
)
where /q iscc
if %ERRORLEVEL% neq 0 (
    echo Error: Inno Setup was not found in your PATH.
    exit /b 1
)

if not exist "%release_path%\%prefix%.zip" (
    echo Error: Release ZIP package not found: %release_path%\%prefix%.zip
    exit /b 1
)

pushd "%root%\docs\doxygen"
echo Building the CHM manual...
call regen.bat chm
if %errorlevel% neq 0 exit /b %errorlevel%
copy out\wx.chm "%release_path%\%prefix%.chm"
popd

if exist "%release_path%\%prefix%" rd /s /q "%release_path%\%prefix%"
md "%release_path%\%prefix%"
pushd "%release_path%\%prefix%"
echo Extracting release ZIP for installer...
7z x "..\%prefix%.zip"
if %errorlevel% neq 0 exit /b %errorlevel%
md docs\htmlhelp
copy "..\%prefix%.chm" docs\htmlhelp\wx.chm
set WXW_VER=%1
echo Building the installer...
iscc /Qp build\tools\wxwidgets.iss
if %errorlevel% neq 0 exit /b %errorlevel%
popd

echo Cleaning up installer files...
rd /s /q "%release_path%\%prefix%"
