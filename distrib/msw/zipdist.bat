@echo off
rem Zip up an external, generic + Windows distribution of wxWindows 2.0
if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external wxWindows distribution:
echo   From   %1
echo   To     %2\wx200gen.zip, %2\wx200doc.zip, %2\wx200msw.zip, %2\wx200ps.zip, %2\wx200hlp.zip, %2\wx200htm.zip, %2\wx200pdf.zip
echo CTRL-C if this is not correct.
pause

erase %2\wx200*.zip

cd %1
echo Zipping...
zip32 -@ %2\wx200gen.zip < %1\distrib\generic.rsp
zip32 -@ %2\wx200msw.zip < %1\distrib\msw.rsp
zip32 -@ %2\wx200doc.zip < %1\distrib\docsrc.rsp

rem zip32 -@ %2\wx200ps.zip < %1\distrib\wx_ps.rsp
zip32 -@ %2\wx200hlp.zip < %1\distrib\wx_hlp.rsp
zip32 -@ %2\wx200htm.zip < %1\distrib\wx_html.rsp
zip32 -@ %2\wx200pdf.zip < %1\distrib\wx_pdf.rsp

cd %2

echo wxWindows archived.
goto end

:usage
echo DOS wxWindows distribution.
echo Usage: zipdistgeneric source destination
echo e.g. zipdistgeneric c:\wx b:

:end
