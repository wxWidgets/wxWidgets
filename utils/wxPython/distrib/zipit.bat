
@echo off


zip -@ -r wxPython\wxPython-src-%1.zip < wxPython\distrib\wxPython.rsp

mkdir wxPython\docs
mkdir wxPython\docs\wx
copy %WXWIN%\docs\html\wx\*.gif wxPython\docs\wx
copy %WXWIN%\docs\html\wx\*.htm wxPython\docs\wx
copy wxPython\docs\wx.htm wxPython\docs\wx\index.htm

mkdir wxPython\docs\ogl
copy %WXWIN%\docs\html\ogl\*.gif wxPython\docs\ogl
copy %WXWIN%\docs\html\ogl\*.htm wxPython\docs\ogl
copy wxPython\docs\ogl.htm wxPython\docs\ogl\index.htm


zip -r wxPython\wxPython-docs-%1.zip wxPython\docs

del /sxzy wxPython\docs

move /R wxPython\*.zip wxPython\distrib
