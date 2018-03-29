@echo off

rem This is used to set up the VS2017 and later (hopefully) environment.

rem Called with two paramaters, the inclusive low and the exclusisve high.
if "%1" == "" goto ERR_NOPARM
if "%2" == "" goto ERR_NOPARM

rem If not running from official build, remember where we are.

if "%curr_dir%" == "" (
  set curr_dir=%cd%
)

@echo Checking if vswhere in path
vswhere 1>nul

if errorlevel 9009 (
@echo "vswhere not found, trying C:\Program Files (x86)\Microsoft Visual Studio\Installer"
set "PATH=C:\Program Files (x86)\Microsoft Visual Studio\Installer;%PATH%"
)

vswhere 1>nul
if errorlevel 9009 (
@echo "vswhere not found"
goto ERR_EXIT
)

for /f "usebackq tokens=*" %%i in (`vswhere -latest -version "[%1,%2)" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set InstallDir=%%i
)

if "%InstallDir%" == "" (
  @echo Install directory not found
  goto ERR_EXIT
)

if NOT exist "%InstallDir%\Common7\Tools\vsdevcmd.bat" (
  @echo Can't find an vsdevcmd.bat
  goto ERR_EXIT
)

call "%InstallDir%"\Common7\Tools\vsdevcmd.bat

rem Go back to where we were before vsdevcmd changed things.
cd %curr_dir%

goto end

:ERR_NOPARM
@echo Need to specifiy version min/max range, eg.: 15.0 16.0
:ERR_EXIT
exit /b 1
:end
