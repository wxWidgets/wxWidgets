
@echo off

rem cd %WXWIN%\utils

zip -@ -r wxPython\wxPython-%1.zip < wxPython\distrib\wxPython.rsp

cd -
