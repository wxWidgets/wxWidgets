Rem @echo off
Rem This script compiles wxWindows and some samples/demos with
Rem a range of compilers.
Rem Note that it currently uses batch scripts local to Julian Smart's machine,
Rem to set up the environment correctly.
Rem This is a 4DOS batch file, though 4DOS dependencies have been kept to
Rem a minimum.
Rem Julian Smart, August 12th, 2000
Rem TODO: making a Unicode distribution by modifying setup.h programmatically.
Rem TODO: check differing setup.h options, again by programmatic modification

set WXDIR=%WXWIN%
set FATWXWIN=%WXDIR%
set FULLWXWIN=%WXDIR%
if not %@INDEX["%WXDIR%","wxWindows"] == "-1" set FATWXWIN=%@PATH[%WXDIR%]wxwind~1
if not %@INDEX["%WXDIR%","wxWindows"] == "-1" set FULLWXWIN=%@PATH[%WXDIR%]wxWindows

Rem The following line is for a specific environment -- edit, or comment out
if not %@INDEX["%WXDIR%","wx2stable"] == "-1" set FATWXWIN=d:\wx2sta~1\wxwind~1

echo Full wxWindows path is %FULLWXWIN%, FAT path is %FATWXWIN%
set BUILDDEST=o:\wxWinBuild
Rem Only carry on if we haven't specified a single target
set CARRYON=0

rem goto end

if "%1" == "vc6" goto dovc6
if "%1" == "bc55" goto dobc55
if "%1" == "bc50" goto dobc50
if "%1" == "ming2992" goto doming2952
if "%1" == "cygwin11" goto docygwin11
if "%1" == "vc1" goto dovc1
if "%1" == "bc16" goto dobc16

set CARRYON=1

if not direxist %BUILDDEST% mkdir /S %BUILDDEST%


:dovc6

cd %WXDIR%\samples
erase /S *.exe
cd %WXDIR%\demos
erase /S *.exe
cd %WXDIR%\contrib\samples
erase /S *.exe
cd %WXDIR%\utils\dialoged\src
erase /S *.exe
cd %WXDIR%\utils\tex2rtf\src
erase /S *.exe

echo Starting wxWindows build at %_TIME, %_DATE

Rem First, VC++ 6.

echo === VC++ 6 ===
echo ==============

Rem Set up the environment
call msvc6

echo *** VC++ Debug ***
Rem VC++ Debug
cd %WXDIR%\src\msw
nmake -f makefile.vc cleanall
nmake -f makefile.vc
if not direxist %BUILDDEST%\VC6Debug mkdir /S %BUILDDEST%\VC6Debug
erase /F /Y %BUILDDEST%\VC6Debug
cd %WXDIR%\samples
nmake -f makefile.vc clean

Rem Make one sample, just to make sure Debug mode is OK
cd %WXDIR%\samples\controls
nmake -f makefile.vc
copy controls.exe %BUILDDEST%\VC6Debug
nmake -f makefile.vc clean

cd %WXDIR%\src\msw
nmake -f makefile.vc cleanall

echo *** VC++ Release ***

nmake -f makefile.vc cleanall FINAL=1
nmake -f makefile.vc FINAL=1

if not direxist %BUILDDEST%\VC6Release mkdir /S %BUILDDEST%\VC6Release
erase /F /Y %BUILDDEST%\VC6Release

cd %WXDIR%\samples
nmake -f makefile.vc clean FINAL=1
nmake -f makefile.vc FINAL=1
Rem Copy all executables to VC6Release
Rem Euch, this is the only way I can think of right now
Rem to copy them to the same directory. Daft but...
erase %TEMP%\temp.zip
zip16 -r %TEMP%\temp.zip *.exe
unzip32 -o %TEMP%\temp.zip -d %BUILDDEST%\VC6Release
nmake -f makefile.vc clean FINAL=1

cd %WXDIR%\demos
nmake -f makefile.vc clean FINAL=1
nmake -f makefile.vc FINAL=1
Rem Copy all executables to VC6Release
erase %TEMP%\temp.zip
zip16 -r %TEMP%\temp.zip *.exe
unzip32 -o %TEMP%\temp.zip -d %BUILDDEST%\VC6Release
nmake -f makefile.vc clean FINAL=1

Rem Compile OGL
cd %WXDIR%\contrib\src\ogl
nmake -f makefile.vc clean FINAL=1
nmake -f makefile.vc FINAL=1
cd %WXDIR%\contrib\samples\ogl\ogledit
nmake -f makefile.vc clean FINAL=1
nmake -f makefile.vc FINAL=1
copy *.exe %BUILDDEST%\VC6Release
nmake -f makefile.vc clean FINAL=1
cd %WXDIR%\contrib\samples\ogl\studio
nmake -f makefile.vc clean FINAL=1
nmake -f makefile.vc FINAL=1
copy *.exe %BUILDDEST%\VC6Release
nmake -f makefile.vc clean FINAL=1
cd %WXDIR%\contrib\src\ogl
nmake -f makefile.vc clean FINAL=1

Rem Compile Dialog Editor
cd %WXDIR%\utils\dialoged\src
nmake -f makefile.vc clean FINAL=1
nmake -f makefile.vc FINAL=1
copy *.exe %BUILDDEST%\VC6Release
nmake -f makefile.vc clean FINAL=1

Rem Compile Tex2RTF
cd %WXDIR%\utils\tex2rtf\src
nmake -f makefile.vc clean FINAL=1
nmake -f makefile.vc FINAL=1
copy *.exe %BUILDDEST%\VC6Release
nmake -f makefile.vc clean FINAL=1

Rem Clean up

cd %WXDIR%\src\msw
nmake -f makefile.vc cleanall FINAL=1

echo *** VC++ Debug DLL ***

nmake -f makefile.vc cleanall WXMAKINGDLL=1
nmake -f makefile.vc dll pch

if not direxist %BUILDDEST%\VC6DebugDLL mkdir /S %BUILDDEST%\VC6DebugDLL
erase /F /Y %BUILDDEST%\VC6DebugDLL

copy %WXDIR%\lib\*d.dll %BUILDDEST%\VC6DebugDLL

Rem Make one sample, for a sanity check

cd %WXDIR%\samples\controls
nmake -f makefile.vc WXUSINGDLL=1
copy controls.exe %BUILDDEST%\VC6DebugDLL
nmake -f makefile.vc clean

cd %WXDIR%\src\msw
nmake -f makefile.vc cleanall WXMAKINGDLL=1

echo *** VC++ Release DLL ***

nmake -f makefile.vc cleanall FINAL=1 WXMAKINGDLL=1
nmake -f makefile.vc dll pch FINAL=1

if not direxist %BUILDDEST%\VC6ReleaseDLL mkdir /S %BUILDDEST%\VC6ReleaseDLL
erase /F /Y %BUILDDEST%\VC6ReleaseDLL

copy %WXDIR%\lib\*.dll %BUILDDEST%\VC6ReleaseDLL

Rem Make one sample, for a sanity check

cd %WXDIR%\samples\controls
nmake -f makefile.vc WXUSINGDLL=1 FINAL=1
copy controls.exe %BUILDDEST%\VC6ReleaseDLL
nmake -f makefile.vc clean FINAL=1

cd %WXDIR%\src\msw
nmake -f makefile.vc cleanall WXMAKINGDLL=1 FINAL=1

if "%CARRYON" == "0" goto end

:dobc55

echo === BC++ 5.5 ===
echo ================

call bcb55

echo *** WIN32 BC++ Debug ***

Rem WIN32 BC++ Debug
cd %WXDIR%\src\msw
make -f makefile.b32 cleanall
make -f makefile.b32
if not direxist %BUILDDEST%\BC55Debug mkdir /S %BUILDDEST%\BC55Debug
erase /F /Y %BUILDDEST%\BC55Debug
cd %WXDIR%\samples
make -f makefile.b32 clean

Rem Make one sample, just to make sure Debug mode is OK
cd %WXDIR%\samples\controls
make -f makefile.b32
copy controls.exe %BUILDDEST%\BC55Debug
make -f makefile.b32 clean

Rem Compile OGL
cd %WXDIR%\contrib\src\ogl
make -f makefile.b32 clean
make -f makefile.b32
cd %WXDIR%\contrib\samples\ogl\ogledit
make -f makefile.b32 clean
make -f makefile.b32
copy *.exe %BUILDDEST%\BC55Release
make -f makefile.b32 clean
cd %WXDIR%\contrib\samples\ogl\studio
make -f makefile.b32 clean
make -f makefile.b32
copy *.exe %BUILDDEST%\BC55Release
make -f makefile.b32 clean
cd %WXDIR%\contrib\src\ogl
make -f makefile.b32 clean

cd %WXDIR%\src\msw
make -f makefile.b32 cleanall

goto bcdebugdll

echo *** WIN32 BC++ Release ***

make -f makefile.b32 cleanall FINAL=1
make -f makefile.b32 FINAL=1

if not direxist %BUILDDEST%\BC55Release mkdir /S %BUILDDEST%\BC55Release
erase /F /Y %BUILDDEST%\BC55Release

cd %WXDIR%\samples
make -f makefile.b32 clean FINAL=1
make -f makefile.b32 FINAL=1
Rem Copy all executables to BC55Release
erase %TEMP%\temp.zip
zip16 -r %TEMP%\temp.zip *.exe
unzip32 -o %TEMP%\temp.zip -d %BUILDDEST%\BC55Release
make -f makefile.b32 clean FINAL=1

cd %WXDIR%\demos
make -f makefile.b32 clean FINAL=1
make -f makefile.b32 FINAL=1
Rem Copy all executables to BC55Release
erase %TEMP%\temp.zip
zip16 -r %TEMP%\temp.zip *.exe
unzip32 -o %TEMP%\temp.zip -d %BUILDDEST%\BC55Release
make -f makefile.b32 clean FINAL=1

Rem Compile OGL
cd %WXDIR%\contrib\src\ogl
make -f makefile.b32 clean FINAL=1
make -f makefile.b32 FINAL=1
cd %WXDIR%\contrib\samples\ogl\ogledit
make -f makefile.b32 clean FINAL=1
make -f makefile.b32 FINAL=1
copy *.exe %BUILDDEST%\BC55Release
make -f makefile.b32 clean FINAL=1
cd %WXDIR%\contrib\samples\ogl\studio
make -f makefile.b32 clean FINAL=1
make -f makefile.b32 FINAL=1
copy *.exe %BUILDDEST%\BC55Release
make -f makefile.b32 clean FINAL=1
cd %WXDIR%\contrib\src\ogl
make -f makefile.b32 clean FINAL=1

Rem Compile Dialog Editor
cd %WXDIR%\utils\dialoged\src
make -f makefile.b32 clean FINAL=1
make -f makefile.b32 FINAL=1
copy *.exe %BUILDDEST%\BC55Release
make -f makefile.b32 clean FINAL=1

Rem Compile Tex2RTF
cd %WXDIR%\utils\tex2rtf\src
make -f makefile.b32 clean FINAL=1
make -f makefile.b32 FINAL=1
copy *.exe %BUILDDEST%\BC55Release
make -f makefile.b32 clean FINAL=1

Rem Clean up

cd %WXDIR%\src\msw
make -f makefile.b32 cleanall FINAL=1

:bcdebugdll

echo *** WIN32 BC++ Debug DLL ***

make -f makefile.b32 cleanall
make -f makefile.b32 dll

if not direxist %BUILDDEST%\BC55DebugDLL mkdir /S %BUILDDEST%\BC55DebugDLL
erase /F /Y %BUILDDEST%\BC55DebugDLL

copy %WXDIR%\lib\*.dll %BUILDDEST%\BC55DebugDLL

Rem Make one sample, for a sanity check

cd %WXDIR%\samples\controls
make -f makefile.b32 WXUSINGDLL=1
copy controls.exe %BUILDDEST%\BC55DebugDLL
make -f makefile.b32 clean

cd %WXDIR%\src\msw
make -f makefile.b32 cleanall

if "%CARRYON" == "0" goto end

:dobc50


echo === BC++ 5.0 ===
echo ================

call borland

echo *** WIN32 BC++ Debug ***

Rem WIN32 BC++ Debug
cd %WXDIR%\src\msw
make -f makefile.b32 cleanall
make -f makefile.b32
if not direxist %BUILDDEST%\BC50Debug mkdir /S %BUILDDEST%\BC50Debug
erase /F /Y %BUILDDEST%\BC50Debug
cd %WXDIR%\samples
make -f makefile.b32 clean

Rem Make one sample, just to make sure Debug mode is OK
cd %WXDIR%\samples\controls
make -f makefile.b32
copy controls.exe %BUILDDEST%\BC50Debug
make -f makefile.b32 clean

cd %WXDIR%\src\msw
make -f makefile.b32 cleanall

if "%CARRYON" == "0" goto end

:docygwin11


echo === Cygwin 1.1 ===
echo ==================

call cygwin11

echo *** Cygwin Debug ***

Rem Cygwin Release
cd %WXDIR%\src\msw
make -f makefile.g95 cleanall
make -f makefile.g95 FINAL=1
if not direxist %BUILDDEST%\Cygwin11Release mkdir /S %BUILDDEST%\Cygwin11Release
erase /F /Y %BUILDDEST%\Cygwin11Release
cd %WXDIR%\samples
make -f makefile.g95 clean FINAL=1

if not direxist %BUILDDEST%\Cygwin11Release mkdir /S %BUILDDEST%\Cygwin11Release
erase /F /Y %BUILDDEST%\Cygwin11Release

cd %WXDIR%\samples
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
Rem Copy all executables to Cygwin11Release
erase %TEMP%\temp.zip
zip16 -r %TEMP%\temp.zip *.exe
unzip32 -o %TEMP%\temp.zip -d %BUILDDEST%\Cygwin11Release
make -f makefile.g95 clean FINAL=1

cd %WXDIR%\demos
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
Rem Copy all executables to Cygwin11Release
erase %TEMP%\temp.zip
zip16 -r %TEMP%\temp.zip *.exe
unzip32 -o %TEMP%\temp.zip -d %BUILDDEST%\Cygwin11Release
make -f makefile.g95 clean FINAL=1

Rem Compile OGL
cd %WXDIR%\contrib\src\ogl
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
cd %WXDIR%\contrib\samples\ogl\ogledit
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
copy *.exe %BUILDDEST%\Cygwin11Release
make -f makefile.g95 clean FINAL=1
cd %WXDIR%\contrib\samples\ogl\studio
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
copy *.exe %BUILDDEST%\Cygwin11Release
make -f makefile.g95 clean FINAL=1
cd %WXDIR%\contrib\src\ogl
make -f makefile.g95 clean FINAL=1

Rem Compile Dialog Editor
cd %WXDIR%\utils\dialoged\src
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
copy *.exe %BUILDDEST%\Cygwin11Release
make -f makefile.g95 clean FINAL=1

Rem Compile Tex2RTF
cd %WXDIR%\utils\tex2rtf\src
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
copy *.exe %BUILDDEST%\Cygwin11Release
make -f makefile.g95 clean FINAL=1

echo Stripping executables...
strip %BUILDDEST%\Cygwin11Release\*.exe

Rem Cleanup

cd %WXDIR%\src\msw
make -f makefile.g95 cleanall

if "%CARRYON" == "0" goto end

:doming2952

echo === Mingw32 2.95.2 ===
echo ======================

call ming2952

echo *** Cygwin Debug ***

Rem Cygwin Release
cd %WXDIR%\src\msw
make -f makefile.g95 cleanall
make -f makefile.g95 FINAL=1
if not direxist %BUILDDEST%\Ming2952Release mkdir /S %BUILDDEST%\Ming2952Release
erase /F /Y %BUILDDEST%\Ming2952Release
cd %WXDIR%\samples
make -f makefile.g95 clean FINAL=1

if not direxist %BUILDDEST%\Ming2952Release mkdir /S %BUILDDEST%\Ming2952Release
erase /F /Y %BUILDDEST%\Ming2952Release

cd %WXDIR%\samples
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
Rem Copy all executables to Ming2952Release
erase %TEMP%\temp.zip
zip16 -r %TEMP%\temp.zip *.exe
unzip32 -o %TEMP%\temp.zip -d %BUILDDEST%\Ming2952Release
make -f makefile.g95 clean FINAL=1

cd %WXDIR%\demos
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
Rem Copy all executables to Ming2952Release
erase %TEMP%\temp.zip
zip16 -r %TEMP%\temp.zip *.exe
unzip32 -o %TEMP%\temp.zip -d %BUILDDEST%\Ming2952Release
make -f makefile.g95 clean FINAL=1

Rem Compile OGL
cd %WXDIR%\contrib\src\ogl
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
cd %WXDIR%\contrib\samples\ogl\ogledit
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
copy *.exe %BUILDDEST%\Ming2952Release
make -f makefile.g95 clean FINAL=1
cd %WXDIR%\contrib\samples\ogl\studio
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
copy *.exe %BUILDDEST%\Ming2952Release
make -f makefile.g95 clean FINAL=1
cd %WXDIR%\contrib\src\ogl
make -f makefile.g95 clean FINAL=1

Rem Compile Dialog Editor
cd %WXDIR%\utils\dialoged\src
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
copy *.exe %BUILDDEST%\Ming2952Release
make -f makefile.g95 clean FINAL=1

Rem Compile Tex2RTF
cd %WXDIR%\utils\tex2rtf\src
make -f makefile.g95 clean FINAL=1
make -f makefile.g95 FINAL=1
copy *.exe %BUILDDEST%\Ming2952Release
make -f makefile.g95 clean FINAL=1

echo Stripping executables...
strip %BUILDDEST%\Ming2952Release\*.exe

Rem Cleanup

cd %WXDIR%\src\msw
make -f makefile.g95 cleanall

if "%CARRYON" == "0" goto end

:dovc1

echo === 16-bit VC++ ===
echo ===================

Rem Set up the environment
call msvc1

Rem Needs full wxWindows path for making libraries
set WXWIN=%FULLWXWIN%

echo *** VC++ Debug ***
Rem VC++ Debug
cd %WXDIR%\src\msw
nmake -f makefile.dos cleanall
nmake -f makefile.dos
if not direxist %BUILDDEST%\VC1Debug mkdir /S %BUILDDEST%\VC1Debug
erase /F /Y %BUILDDEST%\VC1Debug

Rem Make one sample, just to make sure Debug mode is OK
cd %WXDIR%\samples\minimal
nmake -f makefile.dos clean
Rem Needs FAT wxWindows path for making executables
set WXWIN=%FATWXWIN%
nmake -f makefile.dos

copy minimal.exe /S %BUILDDEST%\VC1Debug
nmake -f makefile.dos clean

cd %WXDIR%\src\msw
nmake -f makefile.dos cleanall

Rem Needs full wxWindows path for making libraries
set WXWIN=%FULLWXWIN%

echo *** VC++ Release ***

nmake -f makefile.dos cleanall FINAL=1
nmake -f makefile.dos FINAL=1

if not direxist %BUILDDEST%\VC1Release mkdir /S %BUILDDEST%\VC1Release
erase /F /Y %BUILDDEST%\VC1Release

Rem Make one sample, just to make sure Release mode is OK
cd %WXDIR%\samples\minimal
nmake -f makefile.dos clean
nmake -f makefile.dos FINAL=1
Rem Needs FAT wxWindows path for making executables
set WXWIN=%FATWXWIN%
nmake -f makefile.dos FINAL=1
copy minimal.exe /S %BUILDDEST%\VC1Release
nmake -f makefile.dos clean
set WXWIN=%tmpwxwin%

Rem Clean up

cd %WXDIR%\src\msw
nmake -f makefile.dos cleanall FINAL=1

if "%CARRYON" == "0" goto end

:dobc16

echo === 16-bit BC++ ===
echo ===================

Rem Set up the environment
call borland

echo *** 16-bit BC++ Debug ***
Rem 16-bit BC++ Debug
cd %WXDIR%\src\msw
make -f makefile.bcc cleanall
make -f makefile.bcc
if not direxist %BUILDDEST%\BC16Debug mkdir /S %BUILDDEST%\BC16Debug
erase /F /Y %BUILDDEST%\BC16Debug

Rem Make one sample, just to make sure Debug mode is OK
cd %WXDIR%\samples\minimal
make -f makefile.bcc clean
make -f makefile.bcc
copy minimal.exe /S %BUILDDEST%\BC16Debug
make -f makefile.bcc clean

cd %WXDIR%\src\msw
make -f makefile.bcc cleanall

echo *** 16-bit BC++ Release ***

make -f makefile.bcc cleanall FINAL=1
make -f makefile.bcc FINAL=1

if not direxist %BUILDDEST%\BC16Release mkdir /S %BUILDDEST%\BC16Release
erase /F /Y %BUILDDEST%\BC16Release

Rem Make one sample, just to make sure Release mode is OK
cd %WXDIR%\samples\minimal
make -f makefile.bcc clean
make -f makefile.bcc FINAL=1
copy minimal.exe /S %BUILDDEST%\BC16Release
make -f makefile.bcc clean

Rem Clean up

cd %WXDIR%\src\msw
make -f makefile.bcc cleanall FINAL=1

:end

echo Finished wxWindows build at %_TIME, %_DATE

