@echo off
setlocal

set FLAGS=USE_SWIG=1 IN_CVS_TREE=1


rem  Use non-default python?
iff "%1" == "15" .or. "%1" == "20" .or. "%1" == "21" then
	set PYTHON=c:\tools\python%1%\python.exe
	shift
else
	set PYTHON=python
endiff
set SETUP=%PYTHON% -u setup.py


%PYTHON% -c "import sys;print '\n', sys.version, '\n'"


rem "c" --> clean
iff "%1" == "c" then
	shift
	set CMD=%SETUP% %FLAGS% clean
	set OTHERCMD=del wxPython\*.pyd

rem just remove the *.pyd's
elseiff "%1" == "d" then
	shift
	set CMD=del wxPython\*.pyd

rem "i" --> install
elseiff "%1" == "i" then
	shift
	set CMD=%SETUP% build install

rem "s" --> source dist
elseiff "%1" == "s" then
	shift
	set CMD=%SETUP sdist

rem "f" --> FINAL
elseiff "%1" == "f" then
	shift
	set CMD=%SETUP% %FLAGS% FINAL=1 build_ext --inplace %1 %2 %3 %4 %5 %6 %7 %8 %9

rem "h" --> HYBRID
elseiff "%1" == "h" then
	shift
	set CMD=%SETUP% %FLAGS% HYBRID=1 build_ext --inplace %1 %2 %3 %4 %5 %6 %7 %8 %9

rem (no command arg) --> normal build for development
else
	set CMD=%SETUP% %FLAGS% HYBRID=0 build_ext --inplace --debug %1 %2 %3 %4 %5 %6 %7 %8 %9
endiff



echo %CMD%
%CMD%

iff "%OTHERCMD%" != "" then
	echo %OTHERCMD%
	%OTHERCMD%
endiff

