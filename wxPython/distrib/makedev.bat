@echo off
rem   Builds a zip containing stuff needed to link with the wxWindows DLL
rem   shipped with wxPython.  This allows other developers to create apps
rem   or extensions that can share the same DLL.

setlocal

set BASE=wxPython-devel

mkdir %BASE%

mkdir %BASE%\ReleaseDLL
mkdir %BASE%\HybridDLL
copy /s %WXWIN%\ReleaseDLL %BASE%\ReleaseDLL
copy /s %WXWIN%\HybridDLL %BASE%\HybridDLL

mkdir %BASE%\lib
copy %WXWIN%\lib\*.lib %BASE\lib
del %BASE%\lib\*d.lib
copy %WXWIN%\lib\*.dll %BASE\lib
del %BASE%\lib\*d.dll
copy %WXWIN%\lib\*.pdb %BASE\lib
del %BASE%\lib\*d.pdb
copy /s %WXWIN%\lib\mswdll %BASE\lib\mswdll
copy /s %WXWIN%\lib\mswdllh %BASE\lib\mswdllh


mkdir %BASE%\src
copy %WXWIN%\src\makevc.env %BASE%\src
copy %WXWIN%\src\*.vc %BASE%\src

mkdir %BASE%\src\msw
mkdir %BASE%\src\msw\ReleaseDLL
copy %WXWIN%\src\msw\ReleaseDLL\dummy*.obj %BASE%\src\msw\ReleaseDLL
mkdir %BASE%\src\msw\HybridDLL
copy %WXWIN%\src\msw\HybridDLL\dummy*.obj %BASE%\src\msw\HybridDLL

mkdir %BASE%\include
mkdir %BASE%\include\wx
copy /s %WXWIN%\include\wx\* %BASE%\include\wx


zip -r dist\wxPython-devel-win32-%1.zip %BASE%
del /sxzy %BASE%

endlocal



