@echo off
rem Builds a zip containing debugging versions of wxWindows and wxPython
rem that could be unziped over a wxPython installation.


mkdir wxPython-dbg
copy README.dbg.txt wxPython-dbg
copy %WXWIN%\lib\wx*d.dll wxPython-dbg
copy %WXWIN%\wxPython\wxPython\*.pyd wxPython-dbg

zip -r wxPython-dbg-%1.zip wxPython-dbg

del /sx wxPython-dbg

