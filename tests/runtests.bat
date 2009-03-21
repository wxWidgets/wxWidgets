@echo off

REM Runs wxWidgets CppUnit tests
REM This script is used to return the correct return value to the caller
REM which is required by Buildbot to recognize failures.
REM $Id$

set failure=0

for /d %%x in (*) do @(
    if exist %%x\test.exe (
        %%x\test.exe
        if %errorlevel% GEQ 1 set failure=1
    )
    if exist %%x\test_gui.exe (
        %%x\test_gui.exe
        if %errorlevel% GEQ 1 set failure=1
    )
)

REM exit with code 1 if any of the test failed
if %failure% EQU 1 exit 1

REM remove the failure env var:
set failure=

REM exit with code 0 (all tests passed successfully)
exit 0
