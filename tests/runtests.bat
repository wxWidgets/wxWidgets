@echo off

REM Runs wxWidgets CppUnit tests
REM This script is used to return the correct return value to the caller
REM which is required by Buildbot to recognize failures.
REM Note that in DOS error level is not the return code of the previous
REM command; it is for (some!) built-in DOS commands like FIND but
REM in general it's not. Thus to get the return code of the test utility
REM we need some hack; see the guide:
REM    http://www.infionline.net/~wtnewton/batch/batguide.html
REM for general info about DOS batch files.

REM Author: Francesco Montorsi
REM $Id$

rem set the path for running the tests if they use DLL build of wx
for /d %%x in ("..\lib\*_dll") do @set PATH=%%x;%PATH%

set failure=0

for /d %%x in (*) do @(
    if exist %%x\test.exe (
        %%x\test.exe >tmp
        
        REM show the output of the test in the buildbot log:
        type tmp
        
        REM hack to understand if the tests succeeded or not
        REM (failure=1 is set if "OK" does not appear in the test output)
        type tmp | find "OK" >NUL
        if ERRORLEVEL 1 set failure=1

        REM separe the output of the test we just executed from the next one
        echo.
        echo.
        echo ========================================================================
        echo ========================================================================
        echo.
        echo.
    )
    
    if exist %%x\test_gui.exe (
        %%x\test_gui.exe >tmp
        
        REM show the output of the test in the buildbot log:
        type tmp
        
        REM hack to understand if the tests succeeded or not
        REM (failure=1 is set if "OK" does not appear in the test output)
        type tmp | find "OK" >NUL
        if ERRORLEVEL 1 set failure=1
    )
)

REM exit with code 1 if any of the test failed
del tmp
if %failure% EQU 1 (
    echo.
    echo One or more test failed
    echo.
    exit 1
)

REM remove the failure env var:
set failure=

REM exit with code 0 (all tests passed successfully)
exit 0
