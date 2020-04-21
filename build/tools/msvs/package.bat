set origin_dir=%cd%

rem ============= VS Binaries Packaging Script =============
rem
rem Open a command prompt.
rem
rem cd \wxwidgets\build\tools\msvs
rem package major minor revision
rem
rem ========================================================

if "%1" == "" goto ERR_NOPARM
if "%2" == "" goto ERR_NOPARM
if "%3" == "" goto ERR_NOPARM

rem Add path info if necessary.

path=%path%;"C:\Program Files\7-Zip"

set wxMajor=%1
set wxMinor=%2
set wxBuild=%3

cd ..\..\..

mkdir build\msw\packages

set wxPath=%cd%\build\msw\packages
set wxLibVers=%1%2

if "%2" == "9" set wxDLLVers=%1%2%3
if NOT "%2" == "9" set wxDLLVers=%1%2

rem Switch to install directory so 7z files are relative to the folder.

rem Package the VS 2008 32 bit files.
rem ---------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_Dev.7z lib\vc90_dll\mswud  lib\vc90_dll\mswu lib\vc90_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc90_dll\wxbase%wxDllVers%ud_*.pdb lib\vc90_dll\wxMSW%wxDllVers%ud_*.dll lib\vc90_dll\wxbase%wxDllVers%ud_*.dll  lib\vc90_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_ReleaseDLL.7z lib\vc90_dll\wxMSW%wxDllVers%u_*.dll lib\vc90_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_ReleasePDB.7z lib\vc90_dll\wxMSW%wxDllVers%u_*.pdb lib\vc90_dll\wxbase%wxDllVers%u*_.pdb

rem Package the VS 2010 32 bit  files.
rem ----------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_Dev.7z lib\vc100_dll\mswud lib\vc100_dll\mswu lib\vc100_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc100_dll\wxbase%wxDllVers%ud_*.pdb lib\vc100_dll\wxMSW%wxDllVers%ud_*.dll lib\vc100_dll\wxbase%wxDllVers%ud_*.dll  lib\vc100_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_ReleaseDLL.7z lib\vc100_dll\wxMSW%wxDllVers%u_*.dll lib\vc100_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_ReleasePDB.7z lib\vc100_dll\wxMSW%wxDllVers%u_*.pdb lib\vc100_dll\wxbase%wxDllVers%u_*.pdb

rem Package the VS 2012 32 bit  files.
rem ----------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_Dev.7z lib\vc110_dll\mswud lib\vc110_dll\mswu lib\vc110_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc110_dll\wxbase%wxDllVers%ud_*.pdb lib\vc110_dll\wxMSW%wxDllVers%ud_*.dll lib\vc110_dll\wxbase%wxDllVers%ud_*.dll  lib\vc110_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_ReleaseDLL.7z lib\vc110_dll\wxMSW%wxDllVers%u_*.dll lib\vc110_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_ReleasePDB.7z lib\vc110_dll\wxMSW%wxDllVers%u_*.pdb lib\vc110_dll\wxbase%wxDllVers%u_*.pdb

rem Package the VS 2013 32 bit  files.
rem ----------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_Dev.7z lib\vc120_dll\mswud lib\vc120_dll\mswu lib\vc120_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc120_dll\wxbase%wxDllVers%ud_*.pdb lib\vc120_dll\wxMSW%wxDllVers%ud_*.dll lib\vc120_dll\wxbase%wxDllVers%ud_*.dll  lib\vc120_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_ReleaseDLL.7z lib\vc120_dll\wxMSW%wxDllVers%u_*.dll lib\vc120_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_ReleasePDB.7z lib\vc120_dll\wxMSW%wxDllVers%u_*.pdb lib\vc120_dll\wxbase%wxDllVers%u_*.pdb

rem Package the VS 2015 32 bit  files.
rem ----------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_Dev.7z lib\vc140_dll\mswud lib\vc140_dll\mswu lib\vc140_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc140_dll\wxbase%wxDllVers%ud_*.pdb lib\vc140_dll\wxMSW%wxDllVers%ud_*.dll lib\vc140_dll\wxbase%wxDllVers%ud_*.dll  lib\vc140_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_ReleaseDLL.7z lib\vc140_dll\wxMSW%wxDllVers%u_*.dll lib\vc140_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_ReleasePDB.7z lib\vc140_dll\wxMSW%wxDllVers%u_*.pdb lib\vc140_dll\wxbase%wxDllVers%u_*.pdb

rem Package the VS 2017 32 bit  files.
rem ----------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_Dev.7z lib\vc141_dll\mswud lib\vc141_dll\mswu lib\vc141_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc141_dll\wxbase%wxDllVers%ud_*.pdb lib\vc141_dll\wxMSW%wxDllVers%ud_*.dll lib\vc141_dll\wxbase%wxDllVers%ud_*.dll  lib\vc141_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_ReleaseDLL.7z lib\vc141_dll\wxMSW%wxDllVers%u_*.dll lib\vc141_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_ReleasePDB.7z lib\vc141_dll\wxMSW%wxDllVers%u_*.pdb lib\vc141_dll\wxbase%wxDllVers%u_*.pdb

rem Package the VS 2019 32 bit  files.
rem ----------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_Dev.7z lib\vc142_dll\mswud lib\vc142_dll\mswu lib\vc142_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc142_dll\wxbase%wxDllVers%ud_*.pdb lib\vc142_dll\wxMSW%wxDllVers%ud_*.dll lib\vc142_dll\wxbase%wxDllVers%ud_*.dll  lib\vc142_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_ReleaseDLL.7z lib\vc142_dll\wxMSW%wxDllVers%u_*.dll lib\vc142_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_ReleasePDB.7z lib\vc142_dll\wxMSW%wxDllVers%u_*.pdb lib\vc142_dll\wxbase%wxDllVers%u_*.pdb


rem Package the VS 2008 64 bit files.
rem ---------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_x64_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_x64_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_x64_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_x64_Dev.7z lib\vc90_x64_dll\mswud lib\vc90_x64_dll\mswu lib\vc90_x64_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc90_x64_dll\wxbase%wxDllVers%ud_*.pdb lib\vc90_x64_dll\wxMSW%wxDllVers%ud_*.dll lib\vc90_x64_dll\wxbase%wxDllVers%ud_*.dll lib\vc90_x64_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_x64_ReleaseDLL.7z lib\vc90_x64_dll\wxMSW%wxDllVers%u_*.dll lib\vc90_x64_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc90_x64_ReleasePDB.7z lib\vc90_x64_dll\wxMSW%wxDllVers%u_*.pdb lib\vc90_x64_dll\wxbase%wxDllVers%u_*.pdb

rem Package the VS 2010 64 bit files.
rem ---------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_x64_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_x64_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_x64_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_x64_Dev.7z lib\vc100_x64_dll\mswud lib\vc100_x64_dll\mswu lib\vc100_x64_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc100_x64_dll\wxbase%wxDllVers%ud_*.pdb lib\vc100_x64_dll\wxMSW%wxDllVers%ud_*.dll lib\vc100_x64_dll\wxbase%wxDllVers%ud_*.dll lib\vc100_x64_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_x64_ReleaseDLL.7z lib\vc100_x64_dll\wxMSW%wxDllVers%u_*.dll lib\vc100_x64_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc100_x64_ReleasePDB.7z lib\vc100_x64_dll\wxMSW%wxDllVers%u_*.pdb lib\vc100_x64_dll\wxbase%wxDllVers%u_*.pdb

rem Package the VS 2012 64 bit files.
rem ---------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_x64_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_x64_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_x64_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_x64_Dev.7z lib\vc110_x64_dll\mswud lib\vc110_x64_dll\mswu lib\vc110_x64_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc110_x64_dll\wxbase%wxDllVers%ud_*.pdb lib\vc110_x64_dll\wxMSW%wxDllVers%ud_*.dll lib\vc110_x64_dll\wxbase%wxDllVers%ud_*.dll lib\vc110_x64_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_x64_ReleaseDLL.7z lib\vc110_x64_dll\wxMSW%wxDllVers%u_*.dll lib\vc110_x64_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc110_x64_ReleasePDB.7z lib\vc110_x64_dll\wxMSW%wxDllVers%u_*.pdb lib\vc110_x64_dll\wxbase%wxDllVers%u_*.pdb


rem Package the VS 2013 64 bit files.
rem ---------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_x64_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_x64_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_x64_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_x64_Dev.7z lib\vc120_x64_dll\mswud lib\vc120_x64_dll\mswu lib\vc120_x64_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc120_x64_dll\wxbase%wxDllVers%ud_*.pdb lib\vc120_x64_dll\wxMSW%wxDllVers%ud_*.dll lib\vc120_x64_dll\wxbase%wxDllVers%ud_*.dll lib\vc120_x64_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_x64_ReleaseDLL.7z lib\vc120_x64_dll\wxMSW%wxDllVers%u_*.dll lib\vc120_x64_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc120_x64_ReleasePDB.7z lib\vc120_x64_dll\wxMSW%wxDllVers%u_*.pdb lib\vc120_x64_dll\wxbase%wxDllVers%u_*.pdb

rem Package the VS 2015 64 bit files.
rem ---------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_x64_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_x64_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_x64_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_x64_Dev.7z lib\vc140_x64_dll\mswud lib\vc140_x64_dll\mswu lib\vc140_x64_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc140_x64_dll\wxbase%wxDllVers%ud_*.pdb lib\vc140_x64_dll\wxMSW%wxDllVers%ud_*.dll lib\vc140_x64_dll\wxbase%wxDllVers%ud_*.dll lib\vc140_x64_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_x64_ReleaseDLL.7z lib\vc140_x64_dll\wxMSW%wxDllVers%u_*.dll lib\vc140_x64_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc140_x64_ReleasePDB.7z lib\vc140_x64_dll\wxMSW%wxDllVers%u_*.pdb lib\vc140_x64_dll\wxbase%wxDllVers%u_*.pdb

rem Package the VS 2017 64 bit files.
rem ---------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_x64_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_x64_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_x64_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_x64_Dev.7z lib\vc141_x64_dll\mswud lib\vc141_x64_dll\mswu lib\vc141_x64_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc141_x64_dll\wxbase%wxDllVers%ud_*.pdb lib\vc141_x64_dll\wxMSW%wxDllVers%ud_*.dll lib\vc141_x64_dll\wxbase%wxDllVers%ud_*.dll lib\vc141_x64_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_x64_ReleaseDLL.7z lib\vc141_x64_dll\wxMSW%wxDllVers%u_*.dll lib\vc141_x64_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc141_x64_ReleasePDB.7z lib\vc141_x64_dll\wxMSW%wxDllVers%u_*.pdb lib\vc141_x64_dll\wxbase%wxDllVers%u_*.pdb

rem Package the VS 2019 64 bit files.
rem ---------------------------------

rem Get rid of any files from the last run.

del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_x64_Dev.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_x64_ReleaseDLL.7z
del %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_x64_ReleasePDB.7z

7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_x64_Dev.7z lib\vc142_x64_dll\mswud lib\vc142_x64_dll\mswu lib\vc142_x64_dll\wxMSW%wxDllVers%ud_*.pdb lib\vc142_x64_dll\wxbase%wxDllVers%ud_*.pdb lib\vc142_x64_dll\wxMSW%wxDllVers%ud_*.dll lib\vc142_x64_dll\wxbase%wxDllVers%ud_*.dll lib\vc142_x64_dll\*.lib
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_x64_ReleaseDLL.7z lib\vc142_x64_dll\wxMSW%wxDllVers%u_*.dll lib\vc142_x64_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %wxPath%\wxMSW-%wxMajor%.%wxMinor%.%wxBuild%%4_vc142_x64_ReleasePDB.7z lib\vc142_x64_dll\wxMSW%wxDllVers%u_*.pdb lib\vc142_x64_dll\wxbase%wxDllVers%u_*.pdb

fciv %wxPath% -type *.7z -sha1 -wp >> %wxPath%\sha1.txt

goto End

:ERR_NOPARM
   @echo.
   @echo ERROR: NO PARAMETER SUPPLIED
   @echo MajorVers MinorVers BuildVers

:End

cd %origin_dir%
