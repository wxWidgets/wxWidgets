@echo off

rem **** Make a directory to build up a distribution tree
md _distrib_zip
md _distrib_zip\wxPython-%1

REM rem **** Copy the license files
REM copy %WXWIN%\docs\gpl.txt _distrib_zip\wxPython-%1
REM copy %WXWIN%\docs\lgpl.txt _distrib_zip\wxPython-%1
REM copy %WXWIN%\docs\licence.txt _distrib_zip\wxPython-%1
REM copy %WXWIN%\docs\licendoc.txt _distrib_zip\wxPython-%1
REM copy %WXWIN%\docs\preamble.txt _distrib_zip\wxPython-%1

REM rem **** Make a zip fron the live files
REM zip -@ -r _distrib_zip\temp.zip < distrib\wxPython.rsp

REM rem **** Unzip it in our build dir
REM cd _distrib_zip\wxPython-%1
REM unzip ..\temp.zip

REM rem **** zip up the build dir
REM cd ..
REM zip -r ..\distrib\wxPython-src-%1.zip wxPython-%1

cd _distrib_zip

rem **** copy the docs into the tree
md wxPython-%1\docs
md wxPython-%1\docs\wx
md wxPython-%1\docs\ogl
copy %WXWIN%\docs\html\wx\*.gif wxPython-%1\docs\wx
copy %WXWIN%\docs\html\wx\*.htm wxPython-%1\docs\wx
copy wxPython-%1\docs\wx\wx.htm wxPython-%1\docs\wx\index.htm
copy %WXWIN%\docs\html\ogl\*.gif wxPython-%1\docs\ogl
copy %WXWIN%\docs\html\ogl\*.htm wxPython-%1\docs\ogl
copy wxPython-%1\docs\ogl\ogl.htm wxPython-%1\docs\ogl\index.htm

rem **** zip up the docs
zip -r ..\distrib\wxPython-docs-%1.zip wxPython-%1\docs


rem **** Cleanup
cd ..
del /sxzy _distrib_zip
