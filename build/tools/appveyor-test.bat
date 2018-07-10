echo.
echo --- Running tests.
echo.

cd c:\projects\wxwidgets\tests

goto %TOOLSET%

:msbuild
PATH=C:\projects\wxwidgets\lib\vc_x64_dll;%PATH%
.\vc_x64_mswudll\test.exe
if errorlevel 1 goto :error
.\vc_x64_mswudll\test_gui.exe
goto :eof

:nmake
if "%BUILD%"=="debug" set debug_suffix=d
.\vc_mswu%debug_suffix%\test.exe
if errorlevel 1 goto :error
.\vc_mswu%debug_suffix%\test_gui.exe
goto :eof

:mingw
.\gcc_mswud\test.exe
if errorlevel 1 goto :error
.\gcc_mswud\test_gui.exe
goto :eof

:msys2
PATH=C:\projects\wxwidgets\lib;%PATH%
.\test.exe
if errorlevel 1 goto :error
.\test_gui.exe
goto :eof

:cygwin
PATH=C:\projects\wxwidgets\lib;%PATH%
.\test.exe
.\test_gui.exe
echo.
echo --- Note: ignoring possible test failures under Cygwin
echo.
exit /b 0

:cmake
if "%CONFIGURATION%"=="" set CONFIGURATION=Release
cd build_cmake
ctest -V -C %CONFIGURATION% --interactive-debug-mode 0 .
if errorlevel 1 goto error

:error
echo.
echo !!! Non-GUI test failed.
echo.
goto :eof
