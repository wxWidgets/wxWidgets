echo.
echo --- Running tests.
echo.

cd c:\projects\wxwidgets\tests

goto %TOOLSET%

:msbuild
PATH=C:\projects\wxwidgets\lib\vc_x64_dll;%PATH%
.\vc_x64_mswudll\test.exe
if %errorlevel% NEQ 0 goto :error
.\vc_x64_mswudll\test_gui.exe
goto :eof

:nmake
if "%BUILD%"=="debug" set debug_suffix=d
.\vc_mswu%debug_suffix%\test.exe
if %errorlevel% NEQ 0 goto :error
.\vc_mswu%debug_suffix%\test_gui.exe
goto :eof

:mingw
.\gcc_mswud\test.exe
if %errorlevel% NEQ 0 goto :error
.\gcc_mswud\test_gui.exe
goto :eof

:msys2
PATH=C:\projects\wxwidgets\lib;%PATH%
.\test.exe
if %errorlevel% NEQ 0 goto :error
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

:cmake_qt
set CMAKE_TEST_REGEX="test_base"
goto :cmake

:cmake
if "%CONFIGURATION%"=="" set CONFIGURATION=Release
if "%CMAKE_TEST_REGEX%"=="" set CMAKE_TEST_REGEX="test_[base|gui]"
cd ..\build_cmake
ctest -V -C %CONFIGURATION% -R %CMAKE_TEST_REGEX% --output-on-failure --interactive-debug-mode 0 .
if %errorlevel% NEQ 0 goto :error
goto :eof

:error
echo.
echo !!! Non-GUI test failed.
echo.
goto :eof
