@echo off
Rem Zips up an archive of Mac and generic wxWindows files.
Rem Produces wxMac-ver.zip, wxWindows-ver-HTML.zip
set src=%wxwin%
set dest=%src%\deliver

Rem Set this to the required version
set version=2.3.4

if "%src%" == "" goto usage
if "%dest%" == "" goto usage
if "%1%" == "-help" goto usage
if "%1%" == "--help" goto usage
if "%1%" == "/?" goto usage
echo About to archive a wxMac distribution:
echo   From   %src%
echo   To     %dest%
echo CTRL-C if this is not correct.
pause

erase %dest%\wx*.zip

cd %src%
echo Zipping...

zip -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\generic.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\mac.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\makefile.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\cw.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\tiff.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\jpeg.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\tex2rtf.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\dialoged.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\ogl.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\xml.rsp

zip -@ %dest%\wxWindows-%version%-HTML.zip < %src%\distrib\msw\wx_html.rsp

cd %dest%
