
rem   This script is an example of how to use cx_Freeze on a wxPython 
rem   app to produce an executable.

setlocal
set CXDIR=c:\tools\cx_Freeze-2.2
rem set CXBASE=ConsoleBase.exe
set CXBASE=Win32GUIBase.exe

mkdir cxdist
%CXDIR%\FreezePython.exe --base-binary %CXDIR%\%CXBASE% --install-dir cxdist superdoodle.py

rem NOTE: Only the python DLL is copied to cxdist, you'll still need
rem       to copy the wxWidgets DLLs and any others that are needed...

endlocal
