@echo off
rem Zip up an external, generic + Windows distribution of wxWindows 2.0
set src=d:\wx2\wxWindows
set dest=%src\deliver
if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to archive an external wxWindows distribution:
echo   From   %src
echo   To     %dest
echo CTRL-C if this is not correct.
pause

erase %dest\wx200*.zip
erase %dest\glcanvas.zip
erase %dest\ogl3.zip
erase %dest\tex2rtf2.zip

cd %src
echo Zipping...

zip32 -@ %dest\wx200gen.zip < %src\distrib\msw\generic.rsp
zip32 -@ %dest\wx200msw.zip < %src\distrib\msw\msw.rsp
zip32 -@ %dest\wx200gtk.zip < %src\distrib\msw\gtk.rsp
zip32 -@ %dest\wx200stubs.zip < %src\distrib\msw\stubs.rsp
zip32 -@ %dest\wx200mot.zip < %src\distrib\msw\motif.rsp
zip32 -@ %dest\wx200user.zip < %src\distrib\msw\user.rsp

zip32 -@ %dest\wx200doc.zip < %src\distrib\msw\docsrc.rsp
zip32 -@ %dest\wx200hlp.zip < %src\distrib\msw\wx_hlp.rsp
zip32 -@ %dest\wx200htm.zip < %src\distrib\msw\wx_html.rsp
zip32 -@ %dest\wx200pdf.zip < %src\distrib\msw\wx_pdf.rsp
zip32 -@ %dest\wx200wrd.zip < %src\distrib\msw\wx_word.rsp

rem VC++ project files
zip32 -@ %dest\wx200vc.zip < %src\distrib\msw\vc.rsp

rem CodeWarrior project files
zip32 -@ %dest\wx200cw.zip < %src\distrib\msw\cw.rsp

rem OGL 3
zip32 -@ %dest\ogl3.zip < %src\distrib\msw\ogl.rsp

rem GLCanvas
zip32 -@ %dest\glcanvas.zip < %src\distrib\msw\glcanvas.rsp

rem Tex2RTF
zip32 -@ %dest\tex2rtf2.zip < %src\distrib\msw\tex2rtf.rsp

rem wxTreeLayout
zip32 -@ %dest\treedraw.zip < %src\distrib\msw\wxtree.rsp

copy %src\docs\changes.txt %dest
copy %src\docs\msw\install.txt %dest\install_msw.txt
copy %src\docs\motif\install.txt %dest\install_motif.txt
copy %src\docs\gtk\install.txt %dest\install_gtk.txt
copy %src\docs\readme.txt %dest
copy %src\docs\motif\makewxmotif %dest
copy %src\docs\gtk\makewxgtk %dest

cd %dest

echo wxWindows archived.
goto end

:usage
echo DOS wxWindows distribution.
echo Usage: zipdist source destination
echo e.g. zipdist d:\wx2\wxWindows d:\wx2\wxWindows\deliver

:end
