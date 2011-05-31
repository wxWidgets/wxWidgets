@echo off
REM
REM register.bat registers the Life app with CEAppMgr.exe.
REM You can also provide a setup.exe to do this, for example from:
REM
REM http://www.pocketpcdn.com/articles/creatingsetup.html
REM
REM This assumes that the file install.ini is the same directory
REM as the CAB and setup.
REM

REM
REM You must modify the following directories to point to the correct locations. 
REM Make sure the CAB file(s) to be installed are in the appinst\ root directory.
REM

set fileLife="%WXWIN%\demos\life\setup\wince\install.ini"
set fileCEAppMgr="C:\Program Files\Microsoft ActiveSync\ceappmgr.exe"

if not exist %fileLife% goto Usage
if not exist %fileCEAppMgr% goto Usage

%fileCEAppMgr% %fileLife%
goto Exit

:Usage
@echo ---
@echo Edit this batch file to point to the correct directories
@echo    fileLife  = %fileLife%
@echo    fileCEAppMgr = %fileCEAppMgr%
@echo       (this file is installed by Windows CE Services)
@echo ---

:Exit
