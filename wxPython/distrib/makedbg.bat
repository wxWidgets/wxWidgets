@echo off
rem Builds a zip containing debugging versions of wxWindows and wxPython
rem that could be unziped over a wxPython installation.

setlocal

iff "%1" == "15" then
	set PCBUILD=c:\projects\Python-1.5.2\PCBuild
elseiff "%1" == "20" then
	set PCBUILD=c:\projects\Python-2.0\PCBuild
else
	echo Specivy Python version!!!
	goto end
endiff

iff "%2" == "" then
	echo Specify wxPython version!!!
	goto end
endiff


mkdir wxPython-dbg
copy README.dbg.txt wxPython-dbg
copy %WXWIN%\lib\wx*d.dll wxPython-dbg
copy %WXWIN%\wxPython\wxPython\*_d.pyd wxPython-dbg
copy %PCBUILD%\python_d.exe wxPython-dbg
copy %PCBUILD%\python%1_d.dll wxPython-dbg

zip -r wxPython-dbg-%2-Py%1.zip wxPython-dbg

del /sx wxPython-dbg

:end