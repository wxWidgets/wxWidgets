@echo off
Rem Zip up WinCE project files

erase %WXWIN%\wxWinCE.zip
zip -@ %WXWIN%\wxWinCE.zip < %WXWIN%\distrib\msw\wince.rsp

echo Done.
