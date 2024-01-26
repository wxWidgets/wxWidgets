@echo off

rem Initialize general parameters
call parameters.bat

rem Create output and logs directories if needed
if not exist %OUTPUTDIR% mkdir %OUTPUTDIR%
if not exist %LOGDIR% mkdir %LOGDIR%

rem Initialize the main log file with the current date and time
echo %date% %time% : wxWidgets-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER% build started > %MAINLOGFILE%

rem Loop through all versions of the compiler
for %%c in ( 730 810 1030 1210 ) do (
    rem For each version, create a 32 and a 64 bits build
    for %%a in ( 32 64 ) do (
        call officialbuild.bat %%c %%a
    )
)

echo %date% %time% : wxWidgets-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER% build ended >> %MAINLOGFILE%
