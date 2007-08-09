
rem   This script is an example of how to use the McMillan Installer
rem   on a wxPython app to produce an executable.

setlocal

set INSTDIR=c:\tools\Installer

python %INSTDIR%\Makespec.py --onedir --noconsole  superdoodle.py
python %INSTDIR%\Build.py  superdoodle.spec

endlocal
