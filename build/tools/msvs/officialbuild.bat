@echo off

rem SetLocal EnableDelayedExpansion

if "%1" == "" goto ERR_NOPARM

set curr_dir=%cd%

cd ..\..\msw

rem VS2017 changes the build directory when environment batch files
rem are called, so remember where we are building from.

set "VSCMD_START_DIR=%CD%"

rem ================ wxWidgets Official Build ===============
rem
rem Open a command prompt and run this from the build\tools\msvs folder.
rem Specify the compiler version to use.
rem
rem ========================================================

set compvers="Unknown"

if "%1" == "vc14x" (
  @echo Building for vc14x with vs2015
  set comp=14x
  set compvers=vc14x
  call "%VS140COMNTOOLS%VsDevCmd.bat"
)

if %compvers% == "Unknown" goto ERR_UNKNOWNCOMP

rem Return to the build directory in case we have been moved elsewhere.

cd %VSCMD_START_DIR%

@echo Removing the existing destination so that a complete rebuild occurs.

rmdir %compvers%_mswuddll_x64 /s /q
rmdir %compvers%_mswudll_x64 /s /q

rmdir ..\..\lib\%compvers%_x64_dll /s /q

@echo Delete the build output files from the last run.

del %compvers%x64_Debug.txt
del %compvers%x64_Release.txt

if "%compvers%" == "vc14x" call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" x64 10.0.17763.0

@echo 64 bit release build

nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=%comp% OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> %compvers%x64_Release.txt

if ERRORLEVEL 1 goto ERR_BUILD

set build_dir=%cd%

cd ..\..\utils\wxrc

rmdir %compvers%_mswudll_x64 /s /q
del %compvers%x64_Release.txt
nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=%comp% OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> %compvers%x64_Release.txt

if ERRORLEVEL 1 goto ERR_BUILD

cd %build_dir%

@echo 64 bit debug build

nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=%comp% OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> %compvers%x64_Debug.txt

if ERRORLEVEL 1 goto ERR_BUILD

@echo Building Packages

cd %curr_dir%

call package %compvers%

goto End

:ERR_BUILD

goto End
   @echo.
   @echo BUILD ERROR

:ERR_UNKNOWNCOMP
   @echo.
   @echo UNKNOWN COMPILER VERSION
   goto VERSIONS

goto End

:ERR_NOPARM
   @echo.
   @echo ERROR: NO PARAMETER SUPPLIED
   goto VERSIONS

:VERSIONS
   @echo.
   @echo Compiler Version:
   @echo vc14x (Requires VS2015)
   @echo.

:End

@echo Finished.

cd %curr_dir%
