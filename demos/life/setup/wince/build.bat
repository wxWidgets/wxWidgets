@echo off
REM
REM build.bat creates the CAB files
REM Note that when invoking the .ini file with AppMgr (e.g. via setup.exe),
REM you must use the full path, or ActiveSync will report an error with the setup.
REM

REM
REM You Must modify the following directories to point to the correct locations.
REM

set lifeDir=%WXWIN%\demos\life
set lifeARMBinary="%lifeDir%\evc_wince_armv4\life.exe"
set fileLife="%lifeDir%\setup\wince\life.inf"
set fileCabwiz="c:\program files\windows ce tools\wce420\pocket pc 2003\Tools\cabwiz.exe"

if not exist ARM_bins mkdir ARM_bins

if not exist %lifeARMBinary% goto NoBin
@echo "Copying binary to ARM_bins..."
copy %lifeARMBinary% ARM_bins

if not exist %fileLife% goto Usage
if not exist %fileCabwiz% goto Usage
@echo Building %fileCabwiz% %fileLife%
%fileCabwiz% %fileLife% /err life.err /cpu PPC2003_ARM

REM To make CABs for two CPUS, use this line and uncomment lines in the .inf file
REM %fileCabwiz% %fileLife% /err life.err /cpu PPC2003_ARM PPC2003_x86

@echo Copying setup files to the Deliver directory
if not exist Deliver mkdir Deliver
copy life.*.CAB Deliver
copy install.ini Deliver
copy life.ico Deliver
copy setup.exe Deliver

echo "Built: see the Deliver directory for the installation files."
goto Exit

:Usage
@echo ---
@echo Edit this batch file to point to the correct directories
@echo    fileLife = %fileLife%
@echo    fileCabwiz  = %fileCabwiz%
@echo       (these files are included in the Windows CE SDK)
@echo ---

goto Exit

:NoBin
@echo You need to build %lifeARMBinary%.

:Exit

