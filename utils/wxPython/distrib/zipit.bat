
@echo off


zip -@ -r wxPython\wxPython-src-%1.zip < wxPython\distrib\wxPython.rsp

mkdir wxPython\docs
copy %WXWIN%\docs\html\wx\*.* wxPython\docs
copy wxPython\docs\wx.htm wxPython\docs\index.htm

zip -r wxPython\wxPython-docs-%1.zip wxPython\docs

del /y wxPython\docs\*.*

move /R wxPython\*.zip wxPython\distrib