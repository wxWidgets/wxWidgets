set origin_dir=%cd%

if "%1" == "" goto ERR_NOPARM
if "%2" == "" goto ERR_NOPARM
if "%3" == "" goto ERR_NOPARM

cd ..\..\msw

rem ================ VS 2010 Official Build ===============
rem
rem Open a Windows SDK V7.1 command prompt.
rem
rem cd \wxwidgets\build\tools\msvs
rem build2010 x y z
rem
rem ========================================================

rem Remove the existing destination folders if the exist so that a complete rebuild occurs.

rmdir vc100_mswuddll /s /q
rmdir vc100_mswuddll_x64 /s /q
rmdir vc100_mswudll /s /q
rmdir vc100_mswudll_x64 /s /q

rmdir ..\..\lib\vc100_dll /s /q
rmdir ..\..\lib\vc100_x64_dll /s /q

rem Delete the build output files from the last run, if any.

del 2010x86_Debug.txt
del 2010x86_Release.txt
del 2010x64_Debug.txt
del 2010x64_Release.txt


rem 64 bit release build
rem ------------------
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd" /X64 /Release
nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=100 OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> 2010x64_Release.txt

rem 32 bit release build
rem ------------------
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd" /X86 /Release
nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=100 OFFICIAL_BUILD=1 >> 2010x86_Release.txt

rem 64 bit debug build
rem ------------------
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd" /X64 /Debug
nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=100 OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> 2010x64_Debug.txt

rem 64 bit release build
rem ------------------
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd" /X86 /Debug
nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=100 OFFICIAL_BUILD=1 >> 2010x86_Debug.txt

goto End

:ERR_NOPARM
   @echo.
   @echo ERROR: NO PARAMETER SUPPLIED
   @echo MajorVers MinorVers BuildVers

:End

cd %origin_dir%
