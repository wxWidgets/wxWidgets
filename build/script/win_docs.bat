
rem  This builds the docs in %WXWIN% in a number of formats 
rem  and a clean inno setup in a second tree
rem  it uses a number of tools nmake, gnuwin32 zip, ghostscript, MS word, cvsNT
rem  cvs is in the path already from CVSNT install
rem  writes a log file in c:\

echo Building wxWidgets docs... > c:\temp.log

set WXWIN=c:\wx\wxWidgets
set DAILY=c:\daily
set PATH=%PATH%;C:\wx\wxw26b\utils\tex2rtf\src\vc_based;C:\wx\Gnu\bin;c:\progra~1\htmlhe~1;C:\PROGRA~1\INNOSE~1
set PATH=%PATH%;C:\Program Files\gs\gs8.51\lib;C:\Program Files\gs\gs8.51\bin
echo %PATH% >> c:\temp.log

rem update wxwidgets (holds docs) and inno (cvs wxMSW module only)
c:
cd %WXWIN%
cvs up -P -d
cd \wx\inno\wxWidgets
cvs up -P
echo CVS update  >>  c:\temp.log

rem add bakefile build...
set PATH=%PATH%;C:\wx\Bakefile\src
cd \wx\inno\wxWidgets\build\bakefiles
del .bakefile_gen.state
bakefile_gen 


rem add nmake to the path and build the docs
call  \vc6
echo %PATH% >>  c:\temp.log
SET >>  c:\temp.log
cd %WXWIN%\build\script
nmake -f makedocs.vc cleandocs
nmake -f makedocs.vc alldocs

del %DAILY%\in\*.p*
mkdir %WXWIN%\docs\pdf
mkdir %WXWIN%\docs\htmlhelp
mkdir %WXWIN%\docs\htb
echo starting word >>  c:\temp.log
start /WAIT winword /mwx_ps


echo cvs doc up part 2 >>  c:\temp.log

rem use ghostscript ps2pdf - add extra path first
rem set PATH=%PATH%;C:\Program Files\gs\gs8.51\lib;C:\Program Files\gs\gs8.51\bin
rem set PATH=%PATH%;C:\wx\GnuWin32\bin;C:\PROGRA~1\INNOSE~1

cd %DAILY%\in
call ps2pdf wx.ps >>  c:\temp.log
call ps2pdf fl.ps >> c:\temp.log
call ps2pdf gizmos.ps >> c:\temp.log
call ps2pdf mmedia.ps >> c:\temp.log
call ps2pdf ogl.ps >> c:\temp.log
call ps2pdf svg.ps >> c:\temp.log
call ps2pdf tex2rtf.ps >> c:\temp.log

echo Zipping
cd %WXWIN%
del %DAILY%\*.zip
zip %DAILY%\wxWidgets-Docs-CHM.zip docs\htmlhelp\wx.chm utils/tex2rtf/docs/*.chm docs/htmlhelp/*.chm
zip %DAILY%\wxWidgets-Docs-HLP.zip docs\winhelp\wx.hlp docs\winhelp\wx.cnt utils/tex2rtf/docs/*.HLP utils/tex2rtf/docs/*.cnt docs/winhelp/*.hlp docs/winhelp/*.cnt
Rem zip %DAILY%\wx-docs-extra-hlp.ZIP utils/tex2rtf/docs/*.HLP utils/tex2rtf/docs/*.cnt docs/winhelp/*.hlp docs/winhelp/*.cnt -x  docs/winhelp/wx.hlp docs/winhelp/wx.*
Rem zip %DAILY%\wx-docs-extra-chm.ZIP utils/tex2rtf/docs/*.chm docs/htmlhelp/*.chm 
cd %DAILY%\
mkdir docs
mkdir docs\pdf
del docs\pdf\*.pdf
move in\*.pdf docs\pdf
zip wxWidgets-Docs-PDF.zip docs\pdf\*.pdf
Rem zip wx-docs-extra-pdf.ZIP docs\pdf\*.pdf -x docs\pdf\wx.pdf

rem copy chm to inno
mkdir c:\wx\inno\wxWidgets\docs\htmlhelp
copy docs\htmlhelp\wx.chm cd \wx\inno\wxWidgets\docs\htmlhelp\wx.chm
cd %WXWIN%\build\script
iscc wxwidgets.iss >> c:\temp.log

echo setting S
echo yes > net use s: /delete
net use s: \\biolpc22\bake 
net use >> c:\temp.log

copy %DAILY%\*.ZIP s:\bkl-cronjob\archives\win
copy %DAILY%\*.exe s:\bkl-cronjob\archives\win\*.EXE
dir s: /od >> c:\temp.log



