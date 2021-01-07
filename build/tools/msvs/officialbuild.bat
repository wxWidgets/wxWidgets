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
if "%1" == "vc120" (
  @echo Building for vc120 / vs2013
  set comp=120
  set compvers=vc120
  call "%VS120COMNTOOLS%VsDevCmd.bat"
)
if "%1" == "vc110" (
  @echo Building for vc110 / vs2012
  set comp=110
  set compvers=vc110
  call "%VS110COMNTOOLS%VsDevCmd.bat"
)
if "%1" == "vc100" (
  @echo Building for vc100 / vs2010
  set comp=100
  set compvers=vc100
  if "%WINDOWS71SDK%" == "" set WINDOWS71SDK=C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\
  call "%WINDOWS71SDK%SetEnv.cmd"
)
if "%1" == "vc90" (
  @echo Building for vc90 / vs2008
  set comp=90
  set compvers=vc90
  if "%WINDOWS61SDK%" == "" set WINDOWS61SDK=C:\Program Files\Microsoft SDKs\Windows\v6.1\Bin\
)

if %compvers% == "Unknown" goto ERR_UNKNOWNCOMP

if %compvers% == "vc90" (
@echo ============================================================
@echo This will only succeed if run from a SDK 6.1 command prompt.
@echo ============================================================
)

rem Return to the build directory in case we have been moved elsewhere.

cd %VSCMD_START_DIR%

@echo Removing the existing destination so that a complete rebuild occurs.

rmdir %compvers%_mswuddll /s /q
rmdir %compvers%_mswuddll_x64 /s /q
rmdir %compvers%_mswudll /s /q
rmdir %compvers%_mswudll_x64 /s /q

rmdir ..\..\lib\%compvers%_dll /s /q
rmdir ..\..\lib\%compvers%_x64_dll /s /q

@echo Delete the build output files from the last run.

del %compvers%x86_Debug.txt
del %compvers%x86_Release.txt
del %compvers%x64_Debug.txt
del %compvers%x64_Release.txt

if "%compvers%" == "vc14x" call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" x64
if "%compvers%" == "vc120" call "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" x86_amd64
if "%compvers%" == "vc110" call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" x86_amd64
if "%compvers%" == "vc100" call "%WINDOWS71SDK%SetEnv.Cmd" /X64 /Release
if "%compvers%" == "vc90"  call "%WINDOWS61SDK%SetEnv.Cmd" /X64 /Release

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

if "%compvers%" == "vc100" call "%WINDOWS71SDK%SetEnv.Cmd" /X64 /Debug
if "%compvers%" == "vc90"  call "%WINDOWS61SDK%SetEnv.Cmd" /X64 /Debug

nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=%comp% OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> %compvers%x64_Debug.txt

if ERRORLEVEL 1 goto ERR_BUILD

if "%compvers%" == "vc14x" call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" x86
if "%compvers%" == "vc120" call "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" x86
if "%compvers%" == "vc110" call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" x86
if "%compvers%" == "vc100" call "%WINDOWS71SDK%SetEnv.Cmd" /X86 /Release
if "%compvers%" == "vc90"  call "%WINDOWS61SDK%SetEnv.Cmd" /X86 /Release

@echo 32 bit release build

nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=%comp% OFFICIAL_BUILD=1 CPPFLAGS=/arch:SSE CFLAGS=/arch:SSE TARGET_CPU=X86 >> %compvers%x86_Release.txt

if ERRORLEVEL 1 goto ERR_BUILD

cd ..\..\utils\wxrc

rmdir %compvers%_mswuddll /s /q
del %compvers%x86_Release.txt
nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=%comp% OFFICIAL_BUILD=1 CPPFLAGS=/arch:SSE CFLAGS=/arch:SSE TARGET_CPU=X86 >> %compvers%x86_Release.txt

cd %build_dir%

if ERRORLEVEL 1 goto ERR_BUILD

@echo 32 bit debug build

if "%compvers%" == "vc100" call "%WINDOWS71SDK%SetEnv.Cmd" /X86 /Debug
if "%compvers%" == "vc90"  call "%WINDOWS61SDK%SetEnv.Cmd" /X86 /Debug

nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=%comp% OFFICIAL_BUILD=1 CPPFLAGS=/arch:SSE CFLAGS=/arch:SSE TARGET_CPU=X86 >> %compvers%x86_Debug.txt

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
   @echo Compiler Version: One of -
   @echo vc14x (Requires VS2015)
   @echo vc120
   @echo vc110
   @echo vc100
   @echo.

:End

@echo Finished.

cd %curr_dir%
