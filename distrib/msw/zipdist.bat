@echo off
rem Zip up an external, generic + Windows distribution of wxWindows 2.0
set src=%wxwin
set dest=%src\deliver
if "%src" == "" goto usage
if "%dest" == "" goto usage
echo About to archive an external wxWindows distribution:
echo   From   %src
echo   To     %dest
echo CTRL-C if this is not correct.
pause

rem goto dounzip

erase %dest\wx200*.zip
erase %dest\glcanvas.zip
erase %dest\ogl3.zip
erase %dest\tex2rtf2.zip
erase %dest\jpeg.zip

if direxist %dest\wx deltree /Y %dest\wx

cd %src
echo Zipping...

zip32 -@ %dest\wx200gen.zip < %src\distrib\msw\generic.rsp
zip32 -@ -u %dest\wx200gen.zip < %src\distrib\msw\makefile.rsp
zip32 -@ %dest\wx200msw.zip < %src\distrib\msw\msw.rsp
zip32 -@ -u %dest\wx200msw.zip < %src\distrib\msw\makefile.rsp
zip32 -@ %dest\wx200gtk.zip < %src\distrib\msw\gtk.rsp
zip32 -@ -u %dest\wx200gtk.zip < %src\distrib\msw\makefile.rsp
zip32 -@ %dest\wx200stubs.zip < %src\distrib\msw\stubs.rsp
zip32 -@ %dest\wx200mot.zip < %src\distrib\msw\motif.rsp
zip32 -@ -u %dest\wx200mot.zip < %src\distrib\msw\makefile.rsp
zip32 -@ %dest\wx200user.zip < %src\distrib\msw\user.rsp

zip32 -@ %dest\wx200doc.zip < %src\distrib\msw\docsrc.rsp
zip32 -@ %dest\wx200hlp.zip < %src\distrib\msw\wx_hlp.rsp
zip32 -@ %dest\wx200htm.zip < %src\distrib\msw\wx_html.rsp
zip32 -@ %dest\wx200pdf.zip < %src\distrib\msw\wx_pdf.rsp
zip32 -@ %dest\wx200wrd.zip < %src\distrib\msw\wx_word.rsp

rem VC++ project files
zip32 -@ %dest\wx200vc.zip < %src\distrib\msw\vc.rsp

rem BC++ project files
zip32 -@ %dest\wx200bc.zip < %src\distrib\msw\bc.rsp

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

rem JPEG source
zip32 -@ %dest\jpeg.zip < %src\distrib\msw\jpeg.rsp

copy %src\docs\changes.txt %dest
copy %src\docs\msw\install.txt %dest\install_msw.txt
copy %src\docs\motif\install.txt %dest\install_motif.txt
copy %src\docs\gtk\install.txt %dest\install_gtk.txt
copy %src\docs\readme.txt %dest
copy %src\docs\motif\makewxmotif %dest
copy %src\docs\gtk\makewxgtk %dest

:dounzip

cd %dest

rem Unzip the Windows files into 'wx'
mkdir %dest\wx

Rem After this change of directory, we're in the
Rem temporary 'wx' directory and not acting on
Rem the source wxWindows directory.
cd %dest\wx
unzip32 -o ..\wx200msw.zip
unzip32 -o ..\wx200gen.zip
unzip32 -o ..\wx200vc.zip
unzip32 -o ..\wx200bc.zip
unzip32 -o ..\wx200hlp.zip
unzip32 -o ..\glcanvas.zip
unzip32 -o ..\treedraw.zip
unzip32 -o ..\ogl3.zip
unzip32 -o ..\jpeg.zip

rem unzip32 -o ..\wx200doc.zip
rem unzip32 -o ..\wx200bc.zip
rem unzip32 -o ..\wx200cw.zip

rem Now delete a few files that are unnecessary
erase /Y *.in *.spec *.guess *.sub mkinstalldirs modules install-sh *.sh
erase /SY Makefile.in
erase /Y docs\pdf\ogl.pdf
deltree /Y docs\html\ogl

rem Now copy some binary files to 'bin'
if not isdir bin mkdir bin
copy %src\bin\dialoged.exe bin
copy %src\docs\winhelp\dialoged.hlp %src\docs\winhelp\dialoged.cnt bin

rem Time to regenerate the WISE install script, wxwin2.wse.
rem NB: if you've changed wxwin2.wse using WISE, call splitwise.exe
rem from within distrib\msw, to split off wisetop.txt and wisebott.txt.
echo Calling 'makewise' to generate wxwin2.wse...
call %WXWIN\distrib\msw\makewise.bat

rem Now invoke WISE install on the new wxwin2.wse
set wisecmd="c:\Program Files\wise\wise32.exe" /C %WXWIN\distrib\msw\wxwin2.wse
echo Invoking %wisecmd...
start /w %wisecmd

cd %dest

echo wxWindows archived.
goto end

:usage
echo DOS wxWindows distribution.
echo Usage: zipdist source destination
echo e.g. zipdist d:\wx2\wxWindows d:\wx2\wxWindows\deliver

:end
