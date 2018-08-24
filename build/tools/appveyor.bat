set MSBUILD_LOGGER=/logger:"C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll"
goto %TOOLSET%

:msbuild
cd build\msw
msbuild /m:2 /v:n /p:Platform=%ARCH% /p:Configuration="%CONFIGURATION%" wx_vc12.sln %MSBUILD_LOGGER%
cd ..\..\tests
msbuild /m:2 /v:n /p:Platform=%ARCH% /p:Configuration="%CONFIGURATION%" test_vc12.sln %MSBUILD_LOGGER%
msbuild /m:2 /v:n /p:Platform=%ARCH% /p:Configuration="%CONFIGURATION%" test_gui_vc12.sln %MSBUILD_LOGGER%
goto :eof

:nmake
cd build\msw
call "C:\Program Files (x86)\Microsoft Visual Studio %VS%\VC\vcvarsall.bat" %ARCH%
nmake -f makefile.vc BUILD=%BUILD%
cd ..\..\tests
nmake -f makefile.vc BUILD=%BUILD%
goto :eof

:mingw
cd build\msw
path C:\MinGW\bin;%path%
echo --- Tools versions:
g++ --version | "C:\Program Files\Git\usr\bin\head" -n 1
mingw32-make --version | "C:\Program Files\Git\usr\bin\head" -n 1
ar --version | "C:\Program Files\Git\usr\bin\head" -n 1
echo.
echo --- Starting the build
echo.
mingw32-make -f makefile.gcc setup_h BUILD=debug SHARED=0
mingw32-make -j3 -f makefile.gcc BUILD=debug SHARED=0
cd ..\..\tests
mingw32-make -j3 -f makefile.gcc BUILD=debug SHARED=0
goto :eof

:msys2
path C:\msys64\%MSYSTEM%\bin;C:\msys64\usr\bin;%path%
set CHERE_INVOKING=yes
:: Workaround for "configure: Bad file descriptor"
perl -i".bak" -pe "s/^test -n \".DJDIR\"/#$&/" configure
bash -lc "g++ --version"
bash -lc "CXXFLAGS=-Wno-deprecated-declarations ./configure --disable-optimise && make -j3 && make -j3 -C tests"
goto :eof

:cygwin
C:\cygwin\setup-x86.exe -qgnNdO -R C:/cygwin -s http://cygwin.mirror.constant.com -l C:/cygwin/var/cache/setup -P libjpeg-devel -P libpng-devel -P libtiff-devel -P libexpat-devel
path c:\cygwin\bin;%path%
set CHERE_INVOKING=yes
:: Workaround for "configure: Bad file descriptor"
perl -i".bak" -pe "s/^test -n \".DJDIR\"/#$&/" configure
bash -lc "g++ --version"
bash -lc "LDFLAGS=-L/usr/lib/w32api ./configure --disable-optimise --disable-shared && make -j3 && make -j3 -C tests"
goto :eof

:cmake_msys
if "%MSYSTEM%"=="" set MSYSTEM=MINGW32
path C:\msys64\%MSYSTEM%\bin;C:\msys64\usr\bin;%path%
set GENERATOR=MSYS Makefiles
set SKIPTESTS=1
set SKIPINSTALL=1
set CMAKE_NATIVE_FLAGS=-j3
goto cmake

:cmake_cygwin
C:\cygwin\setup-x86.exe -qnNdO -R C:/cygwin -s http://cygwin.mirror.constant.com -l C:/cygwin/var/cache/setup -P libjpeg-devel -P libpng-devel -P libtiff-devel -P libexpat-devel
path c:\cygwin\bin;%path%
set GENERATOR=Unix Makefiles
set SKIPTESTS=1
set SKIPINSTALL=1
set CMAKE_NATIVE_FLAGS=-j3
goto cmake

:cmake_mingw
:: CMake requires a path without sh (added by git on AppVeyor)
path C:\Program Files (x86)\CMake\bin;C:\MinGW\bin
set GENERATOR=MinGW Makefiles
set SKIPTESTS=1
set CMAKE_NATIVE_FLAGS=-j3
goto cmake

:cmake
echo --- Tools versions:
cmake --version

if "%SHARED%"=="" set SHARED=ON
if "%CONFIGURATION%"=="" set CONFIGURATION=Release
echo.%GENERATOR% | findstr /C:"Visual Studio">nul && (
    set CMAKE_LOGGER=%MSBUILD_LOGGER%
)

if "%SKIPTESTS%"=="1" (
    set BUILD_TESTS=OFF
) else (
    set BUILD_TESTS=ALL
)
echo.
echo --- Generating project files
echo.
set WX_INSTALL_PATH=%HOMEDRIVE%%HOMEPATH%\wx_install_target
mkdir %WX_INSTALL_PATH%
mkdir build_cmake
pushd build_cmake
cmake -G "%GENERATOR%" -DwxBUILD_TESTS=%BUILD_TESTS% -DCMAKE_INSTALL_PREFIX=%WX_INSTALL_PATH% -DwxBUILD_SHARED=%SHARED% %CMAKE_CONFIGURE_FLAGS% ..
if ERRORLEVEL 1 goto error
echo.
echo --- Starting the build
echo.
cmake --build . --config %CONFIGURATION% -- %CMAKE_NATIVE_FLAGS% %CMAKE_LOGGER%
if ERRORLEVEL 1 goto error

:: Package binaries as artifact
where 7z
if ERRORLEVEL 1 goto cmake_test
7z a -r wxWidgets_Binaries.zip lib/*.*
appveyor PushArtifact wxWidgets_Binaries.zip

:cmake_test
if NOT "%SKIPINSTALL%"=="1" (
    echo.
    echo --- Installing
    echo.
    cmake --build . --config %CONFIGURATION% --target install -- %CMAKE_LOGGER%
    if ERRORLEVEL 1 goto error
    popd

    echo.
    echo --- Test installed library
    echo.
    set WXWIN=%WX_INSTALL_PATH%
    mkdir build_cmake_install_test
    pushd build_cmake_install_test
    echo --- Configure minimal sample
    cmake -G "%GENERATOR%" ..\samples\minimal
    if ERRORLEVEL 1 goto error
    echo --- Building minimal sample with installed library
    cmake --build . --config %CONFIGURATION% -- %CMAKE_LOGGER%
    if ERRORLEVEL 1 goto error
    popd
)
popd

goto :eof

:error
echo.
echo --- Build failed !
echo.
