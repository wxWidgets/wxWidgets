@echo off

rem **** Make a directory to build up a distribution tree
md _distrib_zip
md _distrib_zip\wxPython-%1

cd _distrib_zip

rem **** copy the docs into the tree
md wxPython-%1\docs
md wxPython-%1\docs\wx
md wxPython-%1\docs\ogl
copy %WXWIN%\docs\html\wx\*.* wxPython-%1\docs\wx
copy wxPython-%1\docs\wx\wx.htm wxPython-%1\docs\wx\index.htm
copy %WXWIN%\docs\html\ogl\*.* wxPython-%1\docs\ogl
copy wxPython-%1\docs\ogl\ogl.htm wxPython-%1\docs\ogl\index.htm

rem **** zip up the docs
rem zip -r ..\distrib\wxPython-docs-%1.zip wxPython-%1\docs
tar cvf ..\dist\wxPythonDocs-%1.tar wxPython-%1
gzip -9 ..\dist\wxPythonDocs-%1.tar


rem **** Cleanup
cd ..
del /sxzy _distrib_zip
