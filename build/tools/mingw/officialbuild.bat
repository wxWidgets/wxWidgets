@echo off

rem Check if compiler version is present
if "%1" == "" goto :ERR_NOPARM
rem Check if architecture is present
if "%2" == "" goto :ERR_NOPARM

rem Initialize specifics parameters related to the current build
call parameters.bat %1 %2

echo Starting official build for %COMPNAME% ( %COMPBINDIR% )

if %CLEANBUILD%==1 (
    echo - Cleanup
    for %%d in ( lib\%COMPNAME%_dll build\msw\%COMPNAME%_mswuddll build\msw\%COMPNAME%_mswudll utils\wxrc\%COMPNAME%_mswudll ) do (
        if exist %WXDIR%\%%d (
            echo    * removing %%d
            rmdir /s /q %WXDIR%\%%d
        )
    )
)

rem Common part of the build command line
set MAINCMDLINE=SHARED=1 CXXFLAGS="-fno-keep-inline-dllexport" OFFICIAL_BUILD=1 COMPILER_VERSION=%COMPVERS%
if "%2" == "64" (
    set MAINCMDLINE=%MAINCMDLINE% CPP="gcc -E -D_M_AMD64"
)
pushd %WXDIR%\build\msw
rem ============================================ Build debug version
title Debug for %COMPNAME%
echo - Debug
set BUILDLOGFILE=%LOGDIR%\wxWidgets-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%-%COMPNAME%-Debug
set CMDLINE=BUILD=debug %MAINCMDLINE%
echo mingw32-make -f makefile.gcc setup_h %CMDLINE% >%BUILDLOGFILE%-build.log
echo mingw32-make -f makefile.gcc setup_h %CMDLINE% >%BUILDLOGFILE%-errors.log
mingw32-make -f makefile.gcc setup_h %CMDLINE% 1>>%BUILDLOGFILE%-build.log 2>>%BUILDLOGFILE%-errors.log
set ERRLVL=%ERRORLEVEL%
echo %date% %time% : %COMPNAME%%XTRASPCS% Dbg setup : ERRORLEVEL=%ERRLVL% >> %MAINLOGFILE%
if not %ERRLVL%==0 goto :ENDBUILD
echo mingw32-make -j%JOBS% -f makefile.gcc %CMDLINE% >>%BUILDLOGFILE%-build.log
echo mingw32-make -j%JOBS% -f makefile.gcc %CMDLINE% >>%BUILDLOGFILE%-errors.log
mingw32-make -j%JOBS% -f makefile.gcc %CMDLINE% 1>>%BUILDLOGFILE%-build.log 2>>%BUILDLOGFILE%-errors.log
set ERRLVL=%ERRORLEVEL%
echo %date% %time% : %COMPNAME%%XTRASPCS% Dbg build : ERRORLEVEL=%ERRLVL% >> %MAINLOGFILE%
if not %ERRLVL%==0 goto :ENDBUILD
rem ============================================ Build release version
title Release for %COMPNAME%
echo - Release
set BUILDLOGFILE=%LOGDIR%\wxWidgets-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%-%COMPNAME%-Release
set CMDLINE=BUILD=release %MAINCMDLINE%
echo mingw32-make -f makefile.gcc setup_h %CMDLINE% >%BUILDLOGFILE%-build.log
echo mingw32-make -f makefile.gcc setup_h %CMDLINE% >%BUILDLOGFILE%-errors.log
mingw32-make -f makefile.gcc setup_h %CMDLINE% 1>>%BUILDLOGFILE%-build.log 2>>%BUILDLOGFILE%-errors.log
set ERRLVL=%ERRORLEVEL%
echo %date% %time% : %COMPNAME%%XTRASPCS% Rel setup : ERRORLEVEL=%ERRLVL% >> %MAINLOGFILE%
if not %ERRLVL%==0 goto :ENDBUILD
echo mingw32-make -j%JOBS% -f makefile.gcc %CMDLINE% >>%BUILDLOGFILE%-build.log
echo mingw32-make -j%JOBS% -f makefile.gcc %CMDLINE% >>%BUILDLOGFILE%-errors.log
mingw32-make -j%JOBS% -f makefile.gcc %CMDLINE% 1>>%BUILDLOGFILE%-build.log 2>>%BUILDLOGFILE%-errors.log
set ERRLVL=%ERRORLEVEL%
echo %date% %time% : %COMPNAME%%XTRASPCS% Rel build : ERRORLEVEL=%ERRLVL% >> %MAINLOGFILE%
if not %ERRLVL%==0 goto :ENDBUILD
rem ============================================ Build wxrc executable
title wxrc for %COMPNAME%
echo - wxrc.exe
set BUILDLOGFILE=%LOGDIR%\wxWidgets-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%-%COMPNAME%-wxrc
cd %WXDIR%\utils\wxrc
echo mingw32-make -j%JOBS% -f makefile.gcc %CMDLINE% >%BUILDLOGFILE%-build.log
echo mingw32-make -j%JOBS% -f makefile.gcc %CMDLINE% >%BUILDLOGFILE%-errors.log
mingw32-make -j%JOBS% -f makefile.gcc %CMDLINE% 1>>%BUILDLOGFILE%-build.log 2>>%BUILDLOGFILE%-errors.log
set ERRLVL=%ERRORLEVEL%
echo %date% %time% : %COMPNAME%%XTRASPCS% wxrc.exe  : ERRORLEVEL=%ERRLVL% >> %MAINLOGFILE%
if not %ERRLVL%==0 goto :ENDBUILD
copy %WXDIR%\utils\wxrc\%COMPNAME%_mswudll\wxrc.exe %WXDIR%\lib\%COMPNAME%_dll\
rem ============================================ Create archives
title Archives for %COMPNAME%
cd %WXDIR%\
if exist %OUTPUTDIR%\%ARCHNAME%%COMPNAME%_Dev.7z del %OUTPUTDIR%\%ARCHNAME%%COMPNAME%_Dev.7z
%ZIP% a -t7z -mx4 -bb %OUTPUTDIR%\%ARCHNAME%%COMPNAME%_Dev.7z -r %COMPNAME%_dll
set ERRLVL=%ERRORLEVEL%
if exist %OUTPUTDIR%\%ARCHNAME%%COMPNAME%_ReleaseDLL.7z del %OUTPUTDIR%\%ARCHNAME%%COMPNAME%_ReleaseDLL.7z
%ZIP% a -t7z -mx4 -bb %OUTPUTDIR%\%ARCHNAME%%COMPNAME%_ReleaseDLL.7z -r -ir!lib\%COMPNAME%_dll\*.dll -xr!*ud_*.dll
set ERRLVL=%ERRLVL% / %ERRORLEVEL%
echo %date% %time% : %COMPNAME%%XTRASPCS% archives  : ERRORLEVEL=%ERRLVL% >> %MAINLOGFILE%

:ENDBUILD
popd
goto :EOF

:ERR_NOPARM
   echo.
   echo ERROR: NO PARAMETER SUPPLIED:
   echo officialbuildgcc.bat cmpVers arch
   echo cmpVers = Compiler version (730, 810, 920, ...)
   echo arch    = architecture (32 or 64)
