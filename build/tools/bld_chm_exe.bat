rem Uncomment the next line to set the version; used also in wxWidgets.iss
SET WXW_VER=3.1.0
if (%WXW_VER%)==() SET WXW_VER=SVN

echo docs building for %WXW_VER%

rem  This builds the docs in %WXWIN% in a number of formats 
rem  and a clean inno setup in a second tree
rem  it uses a number of tools, gnuwin32 zip & dos2unix, 
rem  writes a log file in c:\

echo Building wxWidgets-%WXW_VER% docs... > c:\temp.log

set WXWIN=c:\wx\wxWidgets
set DAILY=c:\daily
set INNO=c:\wx\inno\wxWidgets

rem svn already in my path...
set PATH=%PATH%;c:\wx\Gnu\bin;c:\progra~1\htmlhe~1;C:\PROGRA~1\INNOSE~1;c:\wx\doxygen\bin
set PATH=%PATH%;C:\wx\Graphviz\bin
echo %PATH% >>  c:\temp.log


rem update wxwidgets (holds docs) and inno (wxMSW setup.exe only)
c:
cd %WXWIN%
svn cleanup >>  c:\temp.log
svn up >>  c:\temp.log

rem now inno
cd %INNO% >>  c:\temp.log
rem Don't update the svn automatically
rem svn cleanup >>  c:\temp.log
rem svn up >>  c:\temp.log

if exist include\wx\msw\setup.h del include\wx\msw\setup.h
if exist include\wx\univ\setup.h del include\wx\univ\setup.h

del c*.*
svn up configure
svn up configure.in
svn up config.guess
svn up config.sub
dos2unix configure
dos2unix config.guess
dos2unix config.sub
copy include\wx\msw\setup0.h include\wx\msw\setup.h
copy include\wx\univ\setup0.h include\wx\univ\setup.h
echo SVN update  >>  c:\temp.log

rem make   chm
rem Now create standalone copies of docs (chm and htb)
cd %WXWIN%\docs\doxygen
del out\html\*.html
del out\html\*.htb
del out\html\*.zip
del out\*.chm
call regen chm


cd %WXWIN%\docs\doxygen\out
zip wxWidgets-%WXW_VER%-docs-chm.zip wx.chm
copy wxWidgets-%WXW_VER%-docs-chm.zip %DAILY%
cd html
del *.dot
del *.md5
zip -r wx.htb *.*
copy wx.htb %DAILY%\wxWidgets-%WXW_VER%-docs-html.zip
zip wxWidgets-%WXW_VER%-docs-htb.zip wx.htb
copy wxWidgets-%WXW_VER%-docs-htb.zip %DAILY%


mkdir %INNO%\docs\htmlhelp
copy %WXWIN%\docs\doxygen\out\wx.chm %INNO%\docs\htmlhelp\wx.chm
cd %WXWIN%\build\tools
iscc wxwidgets.iss >> c:\temp.log
copy %INNO%\..\wxMSW-%WXW_VER%-Setup.exe c:\daily

echo docs built for %WXW_VER%
echo docs built for %WXW_VER% >> c:\temp.log


