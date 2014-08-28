if "%1" == "" goto ERR_NOPARM
if "%2" == "" goto ERR_NOPARM
if "%3" == "" goto ERR_NOPARM

cd c:\wxWidgets-%1.%2.%3\build\msw

rem ================ VS 2012 Official Build ===============
rem
rem Copy this file to the \wxwidgets-x.y.z\build\msw folder
rem
rem Open a Visual Studio 2012 Developer command prompt.
rem
rem cd \wxwidgets-x.y.z\build\msw
rem build2012
rem
rem ========================================================

rem Remove the existing destination folders if the exist so that a complete rebuild occurs.

rmdir vc110_mswuddll /s /q
rmdir vc110_mswuddll_x64 /s /q
rmdir vc110_mswudll /s /q
rmdir vc110_mswudll_x64 /s /q

rmdir ..\..\lib\vc110_dll /s /q
rmdir ..\..\lib\vc110_x64_dll /s /q

rem Delete the build output files from the last run, if any.

del 2012x86_Debug.txt
del 2012x86_Release.txt
del 2012x64_Debug.txt
del 2012x64_Release.txt

call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"

rem 64 bit release build
rem ------------------
nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=110 OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> 2012x64_Release.txt

rem 64 bit debug build
rem ------------------
nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=110 OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> 2012x64_Debug.txt

call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\vcvars32.bat"

rem 32 bit release build
rem ------------------
nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=110 OFFICIAL_BUILD=1 >> 2012x86_Release.txt

rem 32 bit debug build
rem ------------------
nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=110 OFFICIAL_BUILD=1 >> 2012x86_Debug.txt

goto End

:ERR_NOPARM
   @echo.
   @echo ERROR: NO PARAMETER SUPPLIED
   @echo MajorVers MinorVers BuildVers

:End

cd c:\wxWidgets
