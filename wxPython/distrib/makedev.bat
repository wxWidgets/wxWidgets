@echo off
rem   Builds a zip containing stuff needed to link with the wxWindows DLL
rem   shipped with wxPython.  This should allow other developers to create apps
rem   or extensions that can share the same DLL.

setlocal
set BASE=_distrib_zip\wxPython-%1

rem **** Make a directory to build up a distribution tree
mkdir _distrib_zip
mkdir %BASE%


rem *** copy files
copy distrib\README.devel.txt %BASE%\README.txt

mkdir %BASE%\include
mkdir %BASE%\include\wx
copy /s %WXWIN%\include\wx\* 	%BASE%\include\wx

mkdir %BASE%\lib
mkdir %BASE%\lib\mswdllh
mkdir %BASE%\lib\mswdlluh
copy /s %WXWIN%\lib\mswdllh\* 	%BASE%\lib\mswdllh
copy /s %WXWIN%\lib\mswdlluh\* 	%BASE%\lib\mswdlluh
copy %WXWIN%\lib\wxmsw*h.lib 	%BASE%\lib
copy %WXWIN%\lib\wxmsw*h.dll 	%BASE%\lib
copy %WXWIN%\lib\mslu.txt 	%BASE%\lib
copy %WXWIN%\lib\unicows.dll 	%BASE%\lib
copy %WXWIN%\lib\unicows.lib 	%BASE%\lib

mkdir %BASE%\src
copy %WXWIN%\src\makevc.env 	%BASE%\src
copy %WXWIN%\src\makelib.vc 	%BASE%\src


rem *** remove unneeded files
del /sxzy %BASE%\include\wx\CVS
del /sxzy %BASE%\include\wx\generic\CVS
del /sxzy %BASE%\include\wx\gtk\CVS
del /sxzy %BASE%\include\wx\html\CVS
del /sxzy %BASE%\include\wx\mac\CVS
del /sxzy %BASE%\include\wx\mgl\CVS
del /sxzy %BASE%\include\wx\motif\CVS
del /sxzy %BASE%\include\wx\msw\CVS
del /sxzy %BASE%\include\wx\msw\ctl3d\CVS
del /sxzy %BASE%\include\wx\msw\gnuwin32\CVS
del /sxzy %BASE%\include\wx\msw\gnuwin32\gl\CVS
del /sxzy %BASE%\include\wx\msw\ole\CVS
del /sxzy %BASE%\include\wx\os2\CVS
del /sxzy %BASE%\include\wx\protocol\CVS
del /sxzy %BASE%\include\wx\univ\CVS
del /sxzy %BASE%\include\wx\unix\CVS
del /sxzy %BASE%\include\wx\x11\CVS
del /sxzy %BASE%\include\wx\x11\nanox\CVS
del /sxzy %BASE%\include\wx\x11\nanox\X11\CVS



rem *** bundle it all up
cd _distrib_zip
tar cvf ../dist/wxPythonWIN32-devel-%1.tar wxPython-%1
gzip -9 ../dist/wxPythonWIN32-devel-%1.tar

rem *** cleanup
cd ..
del /sxzy _distrib_zip

endlocal



