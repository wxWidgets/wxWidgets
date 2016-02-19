goto %TOOLSET%

:msbuild
cd build\msw
msbuild /m:2 /v:n /p:Platform=%ARCH% /p:Configuration="%CONFIGURATION%" wx_vc12.sln
goto :eof

:nmake
cd build\msw
call "C:\Program Files (x86)\Microsoft Visual Studio %VS%\VC\vcvarsall.bat" %ARCH%
nmake -f makefile.vc BUILD=%BUILD%
goto :eof

:mingw
cd build\msw
path C:\MinGW\bin;%path%
echo --- Tools versions:
g++ --version | head -1
mingw32-make --version | head -1
ar --version | head -1
path C:\MinGW\bin;
echo.
echo --- Starting the build
echo.
mingw32-make -f makefile.gcc setup_h BUILD=debug SHARED=0
mingw32-make -j3 -f makefile.gcc BUILD=debug SHARED=0
goto :eof

:msys2
path C:\msys64\%MSYSTEM%\bin;C:\msys64\usr\bin;%path%
set CHERE_INVOKING=yes
:: Workaround for "configure: Bad file descriptor"
perl -i".bak" -pe "s/^test -n \".DJDIR\"/#$&/" configure
bash -lc "g++ --version"
bash -lc "CXXFLAGS=-Wno-deprecated-declarations ./configure --disable-optimise --disable-xrc && make -j3"
goto :eof

:cygwin
C:\cygwin\setup-x86.exe -qnNdO -R C:/cygwin -s http://cygwin.mirror.constant.com -l C:/cygwin/var/cache/setup -P libjpeg-devel -P libpng-devel -P libtiff-devel -P libexpat-devel
path c:\cygwin\bin;%path%
set CHERE_INVOKING=yes
:: Workaround for "configure: Bad file descriptor"
perl -i".bak" -pe "s/^test -n \".DJDIR\"/#$&/" configure
bash -lc "g++ --version"
bash -lc "LDFLAGS=-L/usr/lib/w32api ./configure --disable-optimise --disable-shared --disable-aui --disable-html --disable-ribbon --disable-richtext --disable-stc && make -j3"
goto :eof
