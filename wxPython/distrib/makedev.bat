@echo off
rem   Builds a zip containing stuff needed to link with the wxWindows DLL
rem   shipped with wxPython.  This allows other developers to create apps
rem   or extensions that can share the same DLL.

mkdir wxWin
mkdir wxWin\lib
copy %WXWIN%\lib\*.lib wxWin\lib
del wxWin\lib\*_d.lib

mkdir wxWin\src
copy %WXWIN%\src\makevc.env wxWin\src
copy %WXWIN%\src\*.vc wxWin\src

mkdir wxWin\src\msw
mkdir wxWin\src\msw\ReleaseDLL
copy %WXWIN%\src\msw\*.pch wxWin\src\msw
del wxWin\src\msw\*_d.pch
copy %WXWIN%\src\msw\ReleaseDLL\dummy*.obj wxWin\src\msw\ReleaseDLL

mkdir wxWin\include
mkdir wxWin\include\wx
mkdir wxWin\include\wx\msw
mkdir wxWin\include\wx\generic
mkdir wxWin\include\wx\html
mkdir wxWin\include\wx\protocol

copy %WXWIN%\include\wx\*.* wxWin\include\wx
copy /s %WXWIN%\include\wx\msw\* wxWin\include\wx\msw
copy /s %WXWIN%\include\wx\generic\* wxWin\include\wx\generic
copy /s %WXWIN%\include\wx\html\* wxWin\include\wx\html
copy /s %WXWIN%\include\wx\protocol\* wxWin\include\wx\protocol


zip -r wxPython-dev-%1.zip wxWin


del /sx wxWin



