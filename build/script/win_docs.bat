rem Uncomment the next line to set the version; used also in wxWidgets.iss
rem SET WXW_VER=2.9.0
if (%WXW_VER%)==() SET WXW_VER=SVN

echo docs building for %WXW_VER%

rem  This builds the docs in %WXWIN% in a number of formats 
rem  and a clean inno setup in a second tree
rem  it uses a number of tools nmake, gnuwin32 zip & dos2unix, ghostscript, MS word, cvsNT
rem  cvs is in the path already from CVSNT install
rem  writes a log file in c:\

echo Building wxWidgets-%WXW_VER% docs... > c:\temp.log

set WXWIN=c:\wx\wxWidgets
set DAILY=c:\daily

rem svn already in my path...
set PATH=%PATH%;C:\wx\WXWIDG~1.0\utils\tex2rtf\src\vc_based;c:\wx\Gnu\bin;c:\progra~1\htmlhe~1;C:\PROGRA~1\INNOSE~1
set PATH=%PATH%;C:\Program Files\gs\gs8.51\lib;C:\Program Files\gs\gs8.51\bin
rem add nmake to the path to build the docs
call  \vc6
echo %PATH% >>  c:\temp.log
rem add bakefile build...
set PATH=%PATH%;C:\wx\Bakefile\src

SET >>  c:\temp.log


rem update wxwidgets (holds docs) and inno (cvs wxMSW setup.exe only)
c:
cd %WXWIN%
svn cleanup >>  c:\temp.log
svn up >>  c:\temp.log

rem now inno
cd \wx\inno\wxWidgets >>  c:\temp.log
svn cleanup >>  c:\temp.log
svn up >>  c:\temp.log
del c*.*
if exist include\wx\msw\setup.h del include\wx\msw\setup.h
if exist include\wx\univ\setup.h del include\wx\univ\setup.h

dos2unix configure
dos2unix config.guess
dos2unix config.sub
copy include\wx\msw\setup0.h include\wx\msw\setup.h
copy include\wx\univ\setup0.h include\wx\univ\setup.h
echo SVN update  >>  c:\temp.log

rem just build the formats not in the SVN to keep down the .#makefile...
cd \wx\inno\wxWidgets\build\bakefiles
del .bakefile_gen.state
bakefile_gen -f dmars,dmars_smake,msevc4prj >> c:\temp.log


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

cd %DAILY%\in
call ps2pdf wx.ps >>  c:\temp.log
call ps2pdf tex2rtf.ps >> c:\temp.log

echo Zipping
cd %WXWIN%
del %DAILY%\*.zip
zip %DAILY%\wxWidgets-%WXW_VER%-CHM.zip docs\htmlhelp\wx.chm utils/tex2rtf/docs/*.chm 
zip %DAILY%\wxWidgets-%WXW_VER%-HTB.zip docs\htb\*.htb utils/tex2rtf/docs/*.htb 
zip %DAILY%\wxWidgets-%WXW_VER%-HLP.zip docs\winhelp\wx.hlp docs\winhelp\wx.cnt utils/tex2rtf/docs/*.HLP utils/tex2rtf/docs/*.cnt 
zip -r %DAILY%\wxWidgets-%WXW_VER%-HTML.zip docs\html\* utils\tex2rtf\docs\html\* -x CVS -x *.con -x *.hh* -x *.ref -x *.htx -x *.cn1 -x docs\html\CVS\*

del %DAILY%\*.tar.gz
bsdtar zcvf %DAILY%\wxWidgets-%WXW_VER%-HTB.tar.gz docs/htb/*.htb utils/tex2rtf/docs/*.htb 
bsdtar zcvf %DAILY%\wxWidgets-%WXW_VER%-HTML.tar.gz  --exclude CVS --exclude *.con --exclude *.hh* --exclude *.ref --exclude *.htx --exclude *.cn1 --exclude docs/html/CVS/* docs/html/* utils/tex2rtf/docs/html/*

cd %DAILY%\
mkdir docs
mkdir docs\pdf
del docs\pdf\*.pdf
move in\*.pdf docs\pdf
zip wxWidgets-%WXW_VER%-PDF.zip docs\pdf\*.pdf

rem copy chm to inno
cd %WXWIN%
mkdir c:\wx\inno\wxWidgets\docs\htmlhelp
copy docs\htmlhelp\wx.chm \wx\inno\wxWidgets\docs\htmlhelp\wx.chm
cd %WXWIN%\build\script
iscc wxwidgets.iss >> c:\temp.log

echo docs built for %WXW_VER%
echo docs built for %WXW_VER% >> c:\temp.log


