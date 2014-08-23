if "%1" == "" goto ERR_NOPARM
if "%2" == "" goto ERR_NOPARM
if "%3" == "" goto ERR_NOPARM

cd c:\wxWidgets-%1.%2.%3\build\msw

rem
rem Copy this file to the \wxwidgets-x.y.z\build\msw folder
rem
rem Open a Visual Studio 2005 command prompt.
rem
rem cd \wxwidgets-x.y.z\build\msw
rem build2005
rem
rem ========================================================

rem Set the path if necessary.

set INCLUDE=C:\Program Files (x86)\Microsoft Visual Studio 8\VC\include;C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\INCLUDE;C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\INCLUDE\gl;C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\INCLUDE\crt;
set LIB=C:\Program Files (x86)\Microsoft Visual Studio 8\VC\lib;C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Lib;C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Lib\X64
set LIBPATH=C:\Program Files (x86)\Microsoft Visual Studio 8\VC\lib;C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Lib;C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Lib\AMD64

rem Remove the existing destination folders if the exist so that a complete rebuild occurs.

rmdir vc80_mswudll /s /q
rmdir vc80_mswuddll /s /q
rmdir ..\..\lib\vc80_dll /s /q

rem Delete the build output files from the last run, if any.

del 2005x86_Debug.txt
del 2005x86_Release.txt

nmake -f makefile.vc BUILD=debug SHARED=1 OFFICIAL_BUILD=1 COMPILER_VERSION=80 >> 2005x86_Debug.txt

nmake -f makefile.vc BUILD=release SHARED=1 OFFICIAL_BUILD=1 COMPILER_VERSION=80 >> 2005x86_Release.txt

goto End

:ERR_NOPARM
   @echo.
   @echo ERROR: NO PARAMETER SUPPLIED
   @echo MajorVers MinorVers BuildVers

:End

cd c:\wxWidgets
