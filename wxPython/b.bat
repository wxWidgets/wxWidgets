@echo off
REM ----------------------------------------------------------------------
REM Note:  This is a 4NT script
REM ----------------------------------------------------------------------
setlocal

set FLAGS=USE_SWIG=1 IN_CVS_TREE=1


rem  Use non-default python?
iff "%1" == "15" .or. "%1" == "20" .or. "%1" == "21" .or. "%1" == "22" .or. "%1" == "23" then
	set VER=%1
	set PYTHON=%TOOLS%\python%1%\python.exe
	shift
else
	beep
	echo You must specify Python version as first parameter.
	quit
endiff
set SETUP=%PYTHON% -u setup.py


%PYTHON% -c "import sys;print '\n', sys.version, '\n'"


rem "c" --> clean
iff "%1" == "c" then
	shift
	set CMD=%SETUP% %FLAGS% clean %1 %2 %3 %4 %5 %6 %7 %8 %9
	set OTHERCMD=del wxPython\*.pyd

rem just remove the *.pyd's
elseiff "%1" == "d" then
	shift
	set CMD=del wxPython\*.pyd
	set OTHERCMD=del wxPython\*.pdb

rem touch all the *.i files so swig will regenerate
elseiff "%1" == "t" then
	shift
	set CMD=echo Finished!
	find . -name "*.i" | xargs -l touch

rem "i" --> install
elseiff "%1" == "i" then
	shift
	set CMD=%SETUP% build install

rem "r" --> make installer
elseiff "%1" == "r" then
	shift
	set CMD=%PYTHON% distrib\make_installer.py %1 %2 %3 %4 %5 %6 %7 %8 %9

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

rem "a" --> make all installers
elseiff "%1" == "a" then
	shift
	set CMD=echo Finished!

	call b.bat 21 d
	call b.bat 21 h
	call b.bat 21 r
	call b.bat 21 d UNICODE=1
	call b.bat 21 h UNICODE=1
	call b.bat 21 r UNICODE=1

	call b.bat 22 d
	call b.bat 22 h
	call b.bat 22 r
	call b.bat 22 d UNICODE=1
	call b.bat 22 h UNICODE=1
	call b.bat 22 r UNICODE=1

	call b.bat 23 d
	call b.bat 23 h
	call b.bat 23 r
	call b.bat 23 d UNICODE=1
	call b.bat 23 h UNICODE=1
	call b.bat 23 r UNICODE=1


rem "b" --> both debug and hybrid builds
elseiff "%1" == "b" then
	shift
	set CMD=echo Finished!
	call b.bat %VER%   %1 %2 %3 %4 %5 %6 %7 %8 %9
	call b.bat %VER% h %1 %2 %3 %4 %5 %6 %7 %8 %9

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

