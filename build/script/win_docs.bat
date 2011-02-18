rem Uncomment the next line to set the version; used also in wxWidgets.iss
SET WXW_VER=2.8.12


if (%WXW_VER%)==() SET WXW_VER=CVS

echo docs building for %WXW_VER%

rem  This builds the docs in %WXWIN% in a number of formats
rem  and a clean inno setup in a second tree
rem  it uses a number of tools nmake, gnuwin32 zip & dos2unix, ghostscript, MS word, cvsNT
rem  cvs is in the path already from CVSNT install
rem  writes a log file in c:\

echo Building wxWidgets-%WXW_VER% docs... > c:\temp.log

set WXWIN=c:\wx\wxw28b
set DAILY=c:\daily28b
set PATH=%PATH%;k:\wx\WXWIDG~1.0\utils\tex2rtf\src\vc_based;k:\wx\Gnu\bin;k:\progra~1\htmlhe~1;k:\PROGRA~1\INNOSE~1
set PATH=%PATH%;k:\Program Files\gs\gs8.51\lib;k:\Program Files\gs\gs8.51\bin;k:\progra~1\micros~4\vc98\bin;k:\progra~1\micros~4\Common\Tools
echo %PATH% >> c:\temp.log

rem update wxw28b (holds docs) and inno (cvs wxMSW setup.exe only)
c:
cd %WXWIN%
svn up

rem now inno
cd \wx\inno\wxw28b
del c*.*
if exist include\wx\msw\setup.h del include\wx\msw\setup.h
if exist include\wx\univ\setup.h del include\wx\univ\setup.h
svn up
dos2unix configure
dos2unix config.guess
dos2unix config.sub
copy include\wx\msw\setup0.h include\wx\msw\setup.h
copy include\wx\univ\setup0.h include\wx\univ\setup.h
echo CVS update  >>  c:\temp.log

rem add bakefile build...
rem just build the formats not in the CVS to keep down the .#makefile...
set PATH=%PATH%;C:\wx\Bakefile
cd \wx\inno\wxw28b\build\bakefiles
del .bakefile_gen.state
if not exist Bakefiles.local.bkgen echo "Bakefiles.local.bkgen missing" >> c:\temp.log
bakefile_gen -f dmars,msevc4prj,dmars_smake >> c:\temp.log


rem add nmake to the path and build the docs
call  \vc6
echo %PATH% >>  c:\temp.log
SET >>  c:\temp.log
cd %WXWIN%\build\script
mkdir %DAILY%\in
nmake -f makedocs.vc cleandocs
nmake -f makedocs.vc alldocs
nmake -f makedocs.vc allpdfrtf
nmake -f makedocs.vc htmlhelp
nmake -f makedocs.vc htb

del %DAILY%\in\*.p*
mkdir %WXWIN%\docs\pdf
mkdir %WXWIN%\docs\htmlhelp
mkdir %WXWIN%\docs\htb
echo starting word >>  c:\temp.log
start /WAIT winword /mwx28_ps


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
call ps2pdf tex2rtf_rtf.ps >> c:\temp.log

echo Zipping
cd %WXWIN%
del %DAILY%\*.zip
zip %DAILY%\wxWidgets-%WXW_VER%-CHM.zip docs\htmlhelp\wx.chm utils/tex2rtf/docs/*.chm docs/htmlhelp/*.chm
zip %DAILY%\wxWidgets-%WXW_VER%-HLP.zip docs\winhelp\wx.hlp docs\winhelp\wx.cnt utils/tex2rtf/docs/*.HLP utils/tex2rtf/docs/*.cnt docs/winhelp/*.hlp docs/winhelp/*.cnt
zip %DAILY%\wxWidgets-%WXW_VER%-HTB.zip docs\htb\*.htb utils/tex2rtf/docs/*.htb
zip %DAILY%\wxWidgets-%WXW_VER%-HTML.zip docs\mshtml\wx\*.* utils/tex2rtf/docs/*.html utils/tex2rtf/docs/*.gif

cd %DAILY%\
mkdir docs
mkdir docs\pdf
del docs\pdf\*.pdf
move in\*.pdf docs\pdf
zip wxWidgets-%WXW_VER%-PDF.zip docs\pdf\*.pdf

rem copy chm to inno
cd %WXWIN%
mkdir c:\wx\inno\wxw28b\docs\htmlhelp
del \wx\inno\wxw28b\docs\htmlhelp\wx.chm
copy docs\htmlhelp\wx.chm \wx\inno\wxw28b\docs\htmlhelp\wx.chm
cd %WXWIN%\build\script
iscc wxwidgets.iss >> c:\temp.log


echo docs built for %WXW_VER%
echo docs built for %WXW_VER% >> c:\temp.log


