@echo off
rem Zip up an external, generic + Windows distribution of wxWindows 2.0
set src=d:\wx2\wxWindows
set dest=%src\deliver
if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to archive an external wxWindows distribution:
echo   From   %src
echo   To     %dest\wx200gen.zip, %dest\wx200doc.zip, %dest\wx200msw.zip, %dest\wx200ps.zip, %dest\wx200hlp.zip, %dest\wx200htm.zip, %dest\wx200pdf.zip
echo CTRL-C if this is not correct.
pause

erase %dest\wx200*.zip

cd %src
echo Zipping...
zip32 -@ %dest\wx200gen.zip < %src\distrib\msw\generic.rsp
zip32 -@ %dest\wx200msw.zip < %src\distrib\msw\msw.rsp
zip32 -@ %dest\wx200gtk.zip < %src\distrib\msw\gtk.rsp
zip32 -@ %dest\wx200stubs.zip < %src\distrib\msw\stubs.rsp
zip32 -@ %dest\wx200doc.zip < %src\distrib\msw\docsrc.rsp

zip32 -@ %dest\wx200hlp.zip < %src\distrib\msw\wx_hlp.rsp
zip32 -@ %dest\wx200htm.zip < %src\distrib\msw\wx_html.rsp
zip32 -@ %dest\wx200pdf.zip < %src\distrib\msw\wx_pdf.rsp

cd %dest

echo wxWindows archived.
goto end

:usage
echo DOS wxWindows distribution.
echo Usage: zipdist source destination
echo e.g. zipdist d:\wx2\wxWindows d:\wx2\wxWindows\deliver

:end
