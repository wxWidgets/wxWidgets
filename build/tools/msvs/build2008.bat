if "%1" == "" goto ERR_NOPARM
if "%2" == "" goto ERR_NOPARM
if "%3" == "" goto ERR_NOPARM

cd c:\wxWidgets-%1.%2.%3\build\msw

rem ================ VS 2008 Official Build ===============
rem
rem Copy this file to the \wxwidgets-x.y.z\build\msw folder
rem
rem Open a Windows SDK V6.1 command prompt.
rem
rem cd \wxwidgets-x.y.z\build\msw
rem build2008
rem
rem ========================================================


rem Remove the existing destination folders if the exist so that a complete rebuild occurs.

rmdir vc90_mswudll /s /q
rmdir vc90_mswuddll /s /q
rmdir ..\..\lib\vc90_dll /s /q
rmdir vc90_mswudll_x64 /s /q
rmdir vc90_mswuddll_x64 /s /q
rmdir ..\..\lib\vc90_x64_dll /s /q

rem Delete the build output files from the last run, if any.

del 2008x86_Debug.txt
del 2008x86_Release.txt
del 2008x64_Debug.txt
del 2008x64_Release.txt

rem 32 bit debug build
rem ------------------

call "C:\Program Files\Microsoft SDKs\Windows\v6.1\Bin\SetEnv.Cmd" /X86 /Debug
nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=90 OFFICIAL_BUILD=1 >> 2008x86_Debug.txt

rem 32 bit release build
rem ------------------
call "C:\Program Files\Microsoft SDKs\Windows\v6.1\Bin\SetEnv.Cmd" /X86 /Release
nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=90 OFFICIAL_BUILD=1 >> 2008x86_Release.txt


rem 64 bit debug build
rem ------------------
call "C:\Program Files\Microsoft SDKs\Windows\v6.1\Bin\SetEnv.Cmd" /X64 /Debug
nmake -f makefile.vc BUILD=debug SHARED=1 COMPILER_VERSION=90 OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> 2008x64_Debug.txt

rem 64 bit release build
rem ------------------
call "C:\Program Files\Microsoft SDKs\Windows\v6.1\Bin\SetEnv.Cmd" /X64 /Release
nmake -f makefile.vc BUILD=release SHARED=1 COMPILER_VERSION=90 OFFICIAL_BUILD=1 TARGET_CPU=AMD64 >> 2008x64_Release.txt

goto End

:ERR_NOPARM
   @echo.
   @echo ERROR: NO PARAMETER SUPPLIED
   @echo MajorVers MinorVers BuildVers

:End

cd c:\wxWidgets
