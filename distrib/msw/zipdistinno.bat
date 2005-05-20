@echo off
rem Zip up an external, generic + Windows distribution of wxWidgets 2
rem using Inno Setup + ScriptMaker
set src=%wxwin
set dest=%src\deliver
set webfiles=c:\wx2dev\wxWebSite
set inno=0

Rem Set this to the required version
set version=2.4.3

if "%src" == "" goto usage
if "%dest" == "" goto usage
if "%1" == "-help" goto usage
if "%1" == "--help" goto usage
if "%1" == "/?" goto usage
if "%1" == "innoonly" goto dounzip
if "%1" == "inno" set inno=1
if "%1" == "tidyup" goto tidyup
echo About to archive an external wxWidgets distribution:
echo   From   %src
echo   To     %dest
if "%inno" == "1" echo with Inno Setup creation.
echo CTRL-C if this is not correct.
pause

erase %dest\wx*.zip
erase %dest\*.htb
erase %dest\ogl3*.zip
erase %dest\contrib*.zip
erase %dest\tex2rtf2*.zip
erase %dest\mmedia*.zip
erase %dest\jpeg*.zip
erase %dest\tiff*.zip
erase %dest\dialoged*.zip
erase %dest\utils*.zip
erase %dest\extradoc*.zip
erase %dest\stc*.zip
erase %dest\*-win32*.zip
erase %dest\setup*.*
erase %dest\*.txt
erase %dest\make*

if direxist %dest\wx erase /sxyz %dest\wx\
if not direxist %dest mkdir %dest
if direxist %dest%\wxWidgets-%version% erase /sxyz %dest%\wxWidgets-%version%

Rem Copy FAQ from wxWebSite CVS
if not direxist %webfiles% echo Error - %webfiles% does not exist
if not direxist %webfiles% goto end
echo Copying FAQ and other files from %webfiles
copy %webfiles%\site\faq*.htm %src\docs\html
copy %webfiles%\site\platform.htm %src\docs\html
copy %webfiles%\site\i18n.htm %src\docs\html

Rem Copy setup0.h files to setup.h
copy %src%\include\wx\os2\setup0.h %src%\include\wx\os2\setup.h
copy %src%\include\wx\msw\setup0.h %src%\include\wx\msw\setup.h
copy %src%\include\wx\univ\setup0.h %src%\include\wx\univ\setup.h

cd %src%
echo Zipping...

Rem Zip up the complete wxOS2-xxx.zip file
zip -@ %dest\wxOS2-%version%.zip < %src\distrib\msw\generic.rsp
zip -@ -u %dest%\wxOS2-%version%.zip < %src\distrib\msw\os2.rsp
zip -@ -u %dest%\wxOS2-%version%.zip  < %src\distrib\msw\jpeg.rsp
zip -@ -u %dest%\wxOS2-%version%.zip < %src\distrib\msw\tiff.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\tiff.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\jpeg.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\utils.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\tex2rtf.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\dialoged.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\ogl.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\xml.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\contrib.rsp
zip -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\makefile.rsp

Rem Rearchive under wxWidgets-%version%
if direxist %dest%\wxWidgets-%version% erase /sxyz %dest%\wxWidgets-%version%

mkdir %dest%\wxWidgets-%version%
cd %dest%\wxWidgets-%version%
unzip ..\wxOS2-%version%.zip
echo Overwriting with OS2-specific versions of configure files...
unzip -o %src%\distrib\os2\os2-specific.zip
erase /Y src\gtk\descrip.mms src\motif\descrip.mms docs\pdf\*.pdf
erase /Y src\tiff\*.mcp src\jpeg\*.mcp src\png\*.mcp src\zlib\*.mcp
erase /SXY docs\html\dialoged docs\html\tex2rtf

echo Making files lower case...
cd src\os2
call %src%\distrib\msw\lower.bat
cd ..\..\include\wx\os2
call %src%\distrib\msw\lower.bat

cd %dest%

erase wxOS2-%version%.zip
zip -r wxOS2-%version%.zip wxWidgets-%version%/*

cd %src%

echo Zipping wxMac distribution

copy %src%\include\wx\mac\setup0.h %src%\include\wx\setup.h

zip -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\generic.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\mac.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\cw_mac.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\tiff.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\jpeg.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\utils.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\tex2rtf.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\dialoged.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\ogl.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\xml.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\stc.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\contrib.rsp
zip -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\makefile.rsp

zip -@ %dest%\wxWidgets-%version%-CW-Mac.zip < %src%\distrib\msw\cw_mac.rsp

erase /Y %src%\include\wx\setup.h

if direxist %dest%\wxWidgets-%version% erase /sxyz %dest%\wxWidgets-%version%

mkdir %dest%\wxWidgets-%version%
cd %dest%\wxWidgets-%version%
unzip ..\wxMac-%version%.zip
erase /Y src\gtk\descrip.mms src\motif\descrip.mms docs\pdf\*.pdf
erase /SXY docs\html\dialoged docs\html\tex2rtf docs\htmlhelp

cd %dest%

erase wxMac-%version%.zip
zip -r wxMac-%version%.zip wxWidgets-%version%/*

cd %src%

Rem Create wxWidgets-%version%-win.zip which is used to create wxMSW
echo Zipping individual components
zip -@ %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\generic.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\makefile.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\msw.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\ogl.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\mmedia.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\stc.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\tex2rtf.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\jpeg.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\tiff.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\xml.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\contrib.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\dialoged.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\utils.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\utilmake.rsp
zip -@ -u %dest\wxWidgets-%version%-win.zip < %src\distrib\msw\univ.rsp

echo Re-archiving wxWidgets-%version%-win.zip
call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-win.zip wxWidgets-%version% %dest%

echo Removing .mms files
zip -d %dest%\wxWidgets-%version%-win.zip wxWidgets-%version%/src/gtk/descrip.mms wxWidgets-%version%/src/motif/descrip.mms

echo Creating %dest\wxWidgets-%version%-DocSource.zip
zip -@ %dest\wxWidgets-%version%-DocSource.zip < %src\distrib\msw\docsrc.rsp
call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-DocSource.zip wxWidgets-%version% %dest%

echo Creating %dest\wxWidgets-%version%-WinHelp.zip
zip -@ %dest\wxWidgets-%version%-WinHelp.zip < %src\distrib\msw\wx_hlp.rsp
call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-WinHelp.zip wxWidgets-%version% %dest%

echo Creating %dest\wxWidgets-%version%-HTML.zip
zip -@ %dest\wxWidgets-%version%-HTML.zip < %src\distrib\msw\wx_html.rsp
call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-HTML.zip wxWidgets-%version% %dest%

echo Creating %dest\wxWidgets-%version%-PDF.zip
zip -@ %dest\wxWidgets-%version%-PDF.zip < %src\distrib\msw\wx_pdf.rsp
call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-PDF.zip wxWidgets-%version% %dest%

Rem zip -@ %dest\wxWidgets-%version%-Word.zip < %src\distrib\msw\wx_word.rsp
Rem call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-Word.zip wxWidgets-%version% %dest%

echo Creating %dest\wxWidgets-%version%-HTB.zip
zip -@ %dest\wxWidgets-%version%-HTB.zip < %src\distrib\msw\wx_htb.rsp
call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-HTB.zip wxWidgets-%version% %dest%

echo Creating %dest\wxWidgets-%version%-HTMLHelp.zip
zip -@ %dest\wxWidgets-%version%-HTMLHelp.zip < %src\distrib\msw\wx_chm.rsp
call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-HTMLHelp.zip wxWidgets-%version% %dest%

Rem Add Linuxy docs to a separate archive to be transported to Linux for the
Rem Linux-based releases
echo Creating %dest\wxWidgets-%version%-LinuxDocs.zip
zip -@ %dest\wxWidgets-%version%-LinuxDocs.zip < %src\distrib\msw\wx_html.rsp
zip -@ -u %dest\wxWidgets-%version%-LinuxDocs.zip < %src\distrib\msw\wx_pdf.rsp
zip -@ -u %dest\wxWidgets-%version%-LinuxDocs.zip < %src\distrib\msw\wx_htb.rsp

Rem PDF/HTML docs that should go into the Windows setup because
Rem there are no WinHelp equivalents
echo Creating %dest\wxWidgets-%version%-ExtraDoc.zip
zip -@ %dest\wxWidgets-%version%-ExtraDoc.zip < %src\distrib\msw\extradoc.rsp
call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-ExtraDoc.zip wxWidgets-%version% %dest%

Rem zip up Univ-specific files
echo Creating %dest\wxWidgets-%version%-Univ.zip
zip -@ %dest\wxWidgets-%version%-Univ.zip < %src\distrib\msw\univ.rsp
call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-Univ.zip wxWidgets-%version% %dest%

rem VC++ project files
echo Creating %dest\wxWidgets-%version%-VC.zip
zip -@ %dest\wxWidgets-%version%-VC.zip < %src\distrib\msw\vc.rsp
Rem call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-VC.zip wxWidgets-%version% %dest%

rem BC++ project files
echo Creating %dest\wxWidgets-%version%-BC.zip
zip -@ %dest\wxWidgets-%version%-BC.zip < %src\distrib\msw\bc.rsp
Rem call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-BC.zip wxWidgets-%version% %dest%

rem CodeWarrior project files
echo Creating %dest\wxWidgets-%version%-CW.zip
zip -@ %dest\wxWidgets-%version%-CW.zip < %src\distrib\msw\cw.rsp
Rem call %WXWIN%\distrib\msw\rearchive wxWidgets-%version%-CW.zip wxWidgets-%version% %dest%

rem Dialog Editor source and binary
Rem erase %dest\dialoged-source.zip
Rem zip -@ %dest\dialoged-source.zip < %src\distrib\msw\dialoged.rsp
Rem zip -j %dest\dialoged-%version%.zip %dest\dialoged-source.zip %src\bin\dialoged.exe %src\docs\winhelp\dialoged.hlp %src\docs\winhelp\dialoged.cnt
Rem erase %dest\dialoged-source.zip

copy %src\docs\changes.txt %dest\changes-%version%.txt
copy %src\docs\msw\install.txt %dest\install-msw-%version%.txt
copy %src\docs\mac\install.txt %dest\install-mac-%version%.txt
copy %src\docs\motif\install.txt %dest\install-motif-%version%.txt
copy %src\docs\gtk\install.txt %dest\install-gtk-%version%.txt
copy %src\docs\x11\install.txt %dest\install-x11-%version%.txt
copy %src\docs\readme.txt %dest\readme-%version%.txt
copy %src\docs\motif\readme.txt %dest\readme-motif-%version%.txt
copy %src\docs\gtk\readme.txt %dest\readme-gtk-%version%.txt
copy %src\docs\x11\readme.txt %dest\readme-x11-%version%.txt
copy %src\docs\x11\readme-nanox.txt %dest\readme-nanox-%version%.txt
copy %src\docs\msw\readme.txt %dest\readme-msw-%version%.txt
copy %src\docs\mac\readme.txt %dest\readme-mac-%version%.txt
copy %src\docs\base\readme.txt %dest\readme-base-%version%.txt
copy %src\docs\os2\install.txt %dest\install-os2-%version%.txt
copy %src\docs\univ\readme.txt %dest\readme-univ-%version%.txt
copy %src\docs\readme_vms.txt %dest\readme-vms-%version%.txt
Rem copy %src\docs\motif\makewxmotif %dest\makewxmotif-%version%
Rem copy %src\docs\gtk\makewxgtk %dest\makewxgtk-%version%

Rem Skip Inno Setup if inno is 0.
if "%inno" == "0" goto end

:dounzip

Rem Make dialoged-win32.zip and tex2rtf-win32.zip

Rem cd %src\bin
Rem zip %dest\dialoged-win32-%version%.zip dialoged.*
Rem zip %dest\tex2rtf-win32-%version%.zip tex2rtf.*

cd %dest

rem Put all archives for transit to Linux in a zip file
echo Creating %dest\wxWidgets-%version%-LinuxTransit.zip
erase %dest\wxWidgets-%version%-LinuxTransit.zip
zip %dest\wxWidgets-%version%-LinuxTransit.zip wxWidgets-%version%-LinuxDocs.zip wxWidgets-%version%-VC.zip wxWidgets-%version%-CW-Mac.zip

echo Unzipping the Windows files into wxWidgets-%version%

unzip -o wxWidgets-%version%-win.zip
unzip -o wxWidgets-%version%-VC.zip -d wxWidgets-%version
unzip -o wxWidgets-%version%-BC.zip -d wxWidgets-%version
unzip -o wxWidgets-%version%-CW.zip -d wxWidgets-%version
unzip -o wxWidgets-%version%-HTMLHelp.zip
unzip -o wxWidgets-%version%-ExtraDoc.zip
Rem Need Word file, for Remstar DB classes
Rem Not any more
Rem unzip -o wxWidgets-%version%-Word.zip

Rem After this change of directory, we're in the
Rem temporary 'wx' directory and not acting on
Rem the source wxWidgets directory.
cd %dest%\wxWidgets-%version%

rem Now delete a few files that are unnecessary
attrib -R *
erase /Y BuildCVS.txt descrip.mms
erase /Y setup.h_vms
erase /Y docs\html\roadmap.htm
Rem erase /Y contrib\docs\winhelp\mmedia.*
Rem erase /Y contrib\docs\winhelp\stc.*
Rem erase /Y contrib\docs\htmlhelp\mmedia.*
Rem erase /Y contrib\docs\htmlhelp\stc.*
Rem erase /Y contrib\docs\pdf\*.*
erase /sxzy contrib\docs\latex\ogl
erase /Y src\mingegcs.bat
erase /Y distrib

rem Now copy some binary files to 'bin'
if not isdir bin mkdir bin
Rem copy %src\bin\dialoged.exe bin
Rem copy %src\bin\dialoged.hlp bin
Rem copy %src\bin\dialoged.chm bin
copy %src\bin\tex2rtf.exe bin
copy %src\bin\tex2rtf.hlp bin
copy %src\bin\tex2rtf.cnt bin
Rem copy %src\bin\dbgview.* bin
copy %src\bin\widgets.exe bin
copy %src\bin\widgets.exe.manifest bin
copy %src\bin\life.exe bin
copy %src\bin\life.exe.manifest bin
copy %src\demos\life\breeder.lif bin
Rem copy %src\docs\htmlhelp\dialoged.chm bin
copy %src\docs\htmlhelp\tex2rtf.chm bin

if not direxist docs\pdf mkdir docs\pdf
Rem copy %src\docs\pdf\wxTutorial.pdf docs\pdf

Rem Make wxMSW-xxx.zip
cd %dest%
zip -r wxMSW-%version%.zip wxWidgets-%version%/*
cd wxWidgets-%version%

echo Calling 'makeinno' to generate wxwin2.iss...
call %WXWIN\distrib\msw\makeinno.bat

erase /Y %dest\setup.*

rem Now invoke Inno Setup on the new wxwin2.iss
set innocmd="C:\Program Files\Inno Setup 5\compil32.exe" /cc %WXWIN\distrib\msw\wxwin2.iss
echo Invoking %innocmd...
start "Inno Setup" /w %innocmd%
Rem pause Press any key to continue with the wxWidgets distribution...

:tidyup
cd %dest

rem Put all the setup files into a single zip archive.
zip wxMSW-%version%-setup.zip readme-%version%.txt setup*.*

erase /Y wxWidgets-%version%-win.zip
erase /Y wxWidgets-%version%-ExtraDoc.zip

echo wxWidgets archived.

goto end

:usage
echo DOS wxWidgets distribution. Zips up all MSW, OS/2, Mac and doc files,
echo and optionally makes a setup.exe echo if you specify 'inno'
echo (skipping the zipping if you use 'innoonly').
echo.
echo "Usage: zipdistinno [inno | innoonly]"

:end
