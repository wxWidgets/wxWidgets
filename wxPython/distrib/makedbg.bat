@echo off
rem Builds a zip containing debugging versions of wxWindows and wxPython
rem that could be unziped over a wxPython installation.


mkdir wxPython
copy %WXWIN%\lib\wx*_d.dll wxPython
copy %WXWIN%\utils\wxPython\*.pyd wxPython

zip -r wxPython-dbg-%1.zip wxPython

del /sx wxPython

