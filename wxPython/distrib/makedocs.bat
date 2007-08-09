
setlocal
set DOCDIR=%WXWIN%\docs

cd %DOCDIR%\latex\wx
mkdir %DOCDIR%\html\wx
copy *.gif %DOCDIR%\html\wx
copy *.css %DOCDIR%\html\wx
start /wait tex2rtf %DOCDIR%\latex\wx\manual.tex %DOCDIR%\html\wx\wx.htm -twice -html -macros %DOCDIR%\latex\wx\tex2rtf_css.ini

erase %DOCDIR%\html\wx\*.con
erase %DOCDIR%\html\wx\*.ref
erase %DOCDIR%\latex\wx\*.con
erase %DOCDIR%\latex\wx\*.ref


cd %DOCDIR%\html\wx
copy %DOCDIR%\latex\wx\wx.css .
hhc wx.hhp
mkdir ..\..\htmlhelp
erase %DOCDIR%\htmlhelp\wx.chm
move wx.chm ..\..\htmlhelp
