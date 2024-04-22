rem ================ VS Official Build Package Tool ===============
rem
rem This is used to assemble the official build binaries into
rem three 7zip files for one compiler version. It is normally
rem run by the makepackages.bat tool but can be run from the command line.
rem
rem Do not run this until after the official packages have
rem been built.
rem
rem Open a command prompt. Switch to the build\tools\msvs
rem directory.
rem
rem package major minor revision compilerversion
rem for example: package 3 1 0 vc140
rem
rem ========================================================

call getversion.bat

if "%1" == "" goto ERR_NOPARM

path=%path%;"C:\Program Files\7-Zip"

rem Add path to fciv (the sha tool) if it is not already
rem in the path.

set VCver=%1

rem packages will be located in wx install folder\packages

set packagePath=build\msw\packages

rem Change to wx root folder

set curr_dir1=%cd%
cd ..\..\..
set base_dir=%cd%

mkdir %packagePath%
mkdir %packagePath%\%VCver%

set wxLibVers=%wxMAJOR_VERSION%%wxMINOR_VERSION%

set wxDLLVers=%wxMAJOR_VERSION%%wxMINOR_VERSION%
if "%wxMINOR_VERSION%" == "1" set wxDLLVers=%wxMAJOR_VERSION%%wxMINOR_VERSION%%wxRELEASE_NUMBER%
if "%wxMINOR_VERSION%" == "3" set wxDLLVers=%wxMAJOR_VERSION%%wxMINOR_VERSION%%wxRELEASE_NUMBER%
if "%wxMINOR_VERSION%" == "5" set wxDLLVers=%wxMAJOR_VERSION%%wxMINOR_VERSION%%wxRELEASE_NUMBER%
if "%wxMINOR_VERSION%" == "7" set wxDLLVers=%wxMAJOR_VERSION%%wxMINOR_VERSION%%wxRELEASE_NUMBER%
if "%wxMINOR_VERSION%" == "9" set wxDLLVers=%wxMAJOR_VERSION%%wxMINOR_VERSION%%wxRELEASE_NUMBER%

rem Get rid of any files from the last run.

del %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_Dev.7z
del %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_ReleaseDLL.7z
del %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_ReleasePDB.7z
del %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_x64_Dev.7z
del %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_x64_ReleaseDLL.7z
del %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_x64_ReleasePDB.7z

rem Package the 32 bit files

REM Copy wxrc.exe to the dll folder.
copy utils\wxrc\%VCver%_mswudll\wxrc.exe lib\%VCver%_dll

7z a -t7z %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_Dev.7z lib\%VCver%_dll\mswud  lib\%VCver%_dll\mswu lib\%VCver%_dll\wxMSW%wxDllVers%ud_*.pdb lib\%VCver%_dll\wxbase%wxDllVers%ud_*.pdb lib\%VCver%_dll\wxMSW%wxDllVers%ud_*.dll lib\%VCver%_dll\wxbase%wxDllVers%u*.dll  lib\%VCver%_dll\*.lib lib\%VCver%_dll\wxrc.exe
7z a -t7z %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_ReleaseDLL.7z lib\%VCver%_dll\wxMSW%wxDllVers%u_*.dll lib\%VCver%_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_ReleasePDB.7z lib\%VCver%_dll\wxMSW%wxDllVers%u_*.pdb lib\%VCver%_dll\wxbase%wxDllVers%u_*.pdb

rem Package the 64 bit files

REM Copy wxrc.exe to the dll folder.
copy utils\wxrc\%VCver%_x64_mswudll\wxrc.exe lib\%VCver%_x64_dll

7z a -t7z %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_x64_Dev.7z lib\%VCver%_x64_dll\mswud lib\%VCver%_x64_dll\mswu lib\%VCver%_x64_dll\wxMSW%wxDllVers%ud_*.pdb lib\%VCver%_x64_dll\wxbase%wxDllVers%ud_*.pdb lib\%VCver%_x64_dll\wxMSW%wxDllVers%ud_*.dll lib\%VCver%_x64_dll\wxbase%wxDllVers%u*.dll lib\%VCver%_x64_dll\*.lib  lib\%VCver%_x64_dll\wxrc.exe
7z a -t7z %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_x64_ReleaseDLL.7z lib\%VCver%_x64_dll\wxMSW%wxDllVers%u_*.dll lib\%VCver%_x64_dll\wxbase%wxDllVers%u_*.dll
7z a -t7z %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_x64_ReleasePDB.7z lib\%VCver%_x64_dll\wxMSW%wxDllVers%u_*.pdb lib\%VCver%_x64_dll\wxbase%wxDllVers%u_*.pdb

rem Include the props files: we need wxwidgets.props itself and wx_setup.props
rem included from it.
7z a -t7z %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_Dev.7z build\msw\wx_setup.props
7z a -t7z %packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_x64_Dev.7z build\msw\wx_setup.props

copy wxwidgets.props %packagePath%\%VCver%\wxwidgets.props

rem Change to the directory containing wxwidgets.props in order to include it
rem into the archive without any path.
cd %packagePath%\%VCver%

7z a -t7z wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_Dev.7z wxwidgets.props
7z a -t7z wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_x64_Dev.7z wxwidgets.props

del wxwidgets.props

rem Change to the directory containing licence file in order to include it
rem into the archives without any path.
cd %base_dir%\docs

7z a -t7z ..\%packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_Dev.7z licence.txt
7z a -t7z ..\%packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_ReleaseDLL.7z licence.txt
7z a -t7z ..\%packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_x64_Dev.7z licence.txt
7z a -t7z ..\%packagePath%\%VCver%\wxMSW-%wxMAJOR_VERSION%.%wxMINOR_VERSION%.%wxRELEASE_NUMBER%_%VCver%_x64_ReleaseDLL.7z licence.txt

cd %packagePath%\%VCver%
del sha1.txt
rem fciv requires a complete path to files
fciv %cd%\. -type *.7z -sha1 -wp >> sha1.txt


goto End

:ERR_NOPARM
   @echo.
   @echo ERROR: NO PARAMETER SUPPLIED
   @echo MajorVers MinorVers BuildVers

:End

cd %curr_dir1%
