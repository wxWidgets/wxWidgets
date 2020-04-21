set origin_dir=%cd%

if "%1" == "" goto ERR_NOPARM
if "%2" == "" goto ERR_NOPARM
if "%3" == "" goto ERR_NOPARM

cd ..\..\msw

rem ================ VS 2019 Official Build ===============
rem
rem Copy this file to the \wxwidgets-x.y.z\build\msw folder
rem
rem Open a Visual Studio 2019 Developer command prompt.
rem
rem cd \wxwidgets\build\tools\msvs
rem build2019 x y z
rem
rem ========================================================

rem Remove the existing destination folders if the exist so that a complete rebuild occurs.

rmdir vc142_mswuddll /s /q
rmdir vc142_mswuddll_x64 /s /q
rmdir vc142_mswudll /s /q
rmdir vc142_mswudll_x64 /s /q

rmdir ..\..\lib\vc142_dll /s /q
rmdir ..\..\lib\vc142_x64_dll /s /q

rem Delete the build output files from the last run, if any.

del 2019x86_Debug.txt
del 2019x86_Release.txt
del 2019x64_Debug.txt
del 2019x64_Release.txt

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

rem 64 bit release build
rem ------------------
nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=142 OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> 2019x64_Release.txt

rem 64 bit debug build
rem ------------------
nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=142 OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> 2019x64_Debug.txt

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86

rem 32 bit release build
rem ------------------
nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=142 OFFICIAL_BUILD=1 CPPFLAGS=/arch:SSE CFLAGS=/arch:SSE >> 2019x86_Release.txt

rem 32 bit debug build
rem ------------------
nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=142 OFFICIAL_BUILD=1 CPPFLAGS=/arch:SSE CFLAGS=/arch:SSE >> 2019x86_Debug.txt

goto End

:ERR_NOPARM
   @echo.
   @echo ERROR: NO PARAMETER SUPPLIED
   @echo MajorVers MinorVers BuildVers

:End

cd %origin_dir%
