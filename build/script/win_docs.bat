rem Uncomment the next line to set the version; used also in wxWidgets.iss
rem SET WXW_VER=2.9.1-rc1
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
set INNO=c:\wx\inno\wxWidgets

rem svn already in my path...
set PATH=%PATH%;c:\wx\Gnu\bin;c:\progra~1\htmlhe~1;C:\PROGRA~1\INNOSE~1

echo %PATH% >>  c:\temp.log
rem add bakefile build...
set PATH=%PATH%;C:\wx\Bakefile

SET >>  c:\temp.log


rem update wxwidgets (holds docs) and inno (cvs wxMSW setup.exe only)
c:
cd %WXWIN%
svn cleanup >>  c:\temp.log
svn up >>  c:\temp.log

rem now inno
cd %INNO% >>  c:\temp.log
svn cleanup >>  c:\temp.log
svn up >>  c:\temp.log

if exist include\wx\msw\setup.h del include\wx\msw\setup.h
if exist include\wx\univ\setup.h del include\wx\univ\setup.h

rem for cygwin these need to be in unix format
del configure
del config.guess
del config.sub
svn up configure
svn up config.guess
svn up config.sub
dos2unix configure
dos2unix config.guess
dos2unix config.sub

copy include\wx\msw\setup0.h include\wx\msw\setup.h
copy include\wx\univ\setup0.h include\wx\univ\setup.h
echo SVN update  >>  c:\temp.log

rem just build the formats not in the SVN to keep down the .#makefile...
cd %INNO%\build\bakefiles
del .bakefile_gen.state
bakefile_gen -k -f dmars,dmars_smake,msevc4prj >> c:\temp.log



rem make and copy chm to inno
rem Now create standalone copies of docs (chm and htb)
cd %WXWIN%\docs\doxygen
del out\html\*.html
del out\html\*.htb
del out\*.chm
call regen chm


cd %WXWIN%\docs\doxygen\out
zip wxWidgets-%WXW_VER%-chm.zip wx.chm
copy wxWidgets-%WXW_VER%-chm.zip %DAILY%
cd html
del *.dot
del *.md5
zip -r wx.htb *.*
zip wxWidgets-%WXW_VER%-htb.zip wx.htb
copy wxWidgets-%WXW_VER%-htb.zip %DAILY%


mkdir %INNO%\docs\htmlhelp
copy %WXWIN%\docs\doxygen\out\wx.chm \wx\inno\wx29b\docs\htmlhelp\wx.chm
cd %WXWIN%\build\script
iscc wxwidgets.iss >> c:\temp.log

echo docs built for %WXW_VER%
echo docs built for %WXW_VER% >> c:\temp.log


