@echo off
rem Zip up an external, generic + Windows distribution of wxWindows 2
rem using Inno Setup + ScriptMaker
set src=%wxwin
set dest=%src\deliver
set webfiles=c:\wx2dev\wxWebSite
set inno=0

Rem Set this to the required version
set version=2.3.3

if "%src" == "" goto usage
if "%dest" == "" goto usage
if "%1" == "-help" goto usage
if "%1" == "--help" goto usage
if "%1" == "/?" goto usage
if "%1" == "innoonly" goto dounzip
if "%1" == "inno" set inno=1
if "%1" == "tidyup" goto tidyup
echo About to archive an external wxWindows distribution:
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
if direxist %dest%\wxWindows-%version% erase /sxyz %dest%\wxWindows-%version%

# Copy FAQ from wxWebSite CVS
if not direxist %webfiles% echo Error - %webfiles% does not exist
if not direxist %webfiles% goto end
echo Copying FAQ and other files from %webfiles
copy %webfiles%\site\faq*.htm %src\docs\html
copy %webfiles%\site\platform.htm %src\docs\html
copy %webfiles%\site\i18n.htm %src\docs\html

cd %src
echo Zipping...

Rem Zip up the complete wxOS2-xxx.zip file
zip32 -@ %dest\wxOS2-%version%.zip < %src\distrib\msw\generic.rsp
zip32 -@ -u %dest%\wxOS2-%version%.zip < %src\distrib\msw\os2.rsp
zip32 -@ -u %dest%\wxOS2-%version%.zip < %src\distrib\msw\wx_chm.rsp
zip32 -@ -u %dest%\wxOS2-%version%.zip  < %src\distrib\msw\jpeg.rsp
zip32 -@ -u %dest%\wxOS2-%version%.zip < %src\distrib\msw\tiff.rsp
zip32 -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\tiff.rsp
zip32 -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\jpeg.rsp
zip32 -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\utils.rsp
zip32 -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\tex2rtf.rsp
zip32 -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\dialoged.rsp
zip32 -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\ogl.rsp
zip32 -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\xml.rsp
zip32 -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\contrib.rsp
zip32 -u -@ %dest%\wxOS2-%version%.zip < %src%\distrib\msw\makefile.rsp

Rem Rearchive under wxWindows-%version%
call %WXWIN%\distrib\msw\rearchive wxOS2-%version%.zip wxWindows-%version% %dest%

echo Zipping wxMac distribution
zip32 -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\generic.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\mac.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\cw.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\tiff.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\jpeg.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\utils.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\tex2rtf.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\dialoged.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\ogl.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\xml.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\stc.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\contrib.rsp
zip32 -u -@ %dest%\wxMac-%version%.zip < %src%\distrib\msw\makefile.rsp
call %WXWIN%\distrib\msw\rearchive wxMac-%version%.zip wxWindows-%version% %dest%

Rem Create wxWindows-%version%-win.zip which is used to create wxMSW
echo Zipping individual components
zip32 -@ %dest\wxWindows-%version%-win.zip < %src\distrib\msw\generic.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\makefile.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\msw.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\ogl.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\mmedia.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\stc.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\tex2rtf.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\jpeg.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\tiff.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\xml.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\contrib.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\dialoged.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\utils.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\utilmake.rsp
zip32 -@ -u %dest\wxWindows-%version%-win.zip < %src\distrib\msw\univ.rsp

call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-win.zip wxWindows-%version% %dest%

zip32 -@ %dest\wxWindows-%version%-DocSource.zip < %src\distrib\msw\docsrc.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-DocSource.zip wxWindows-%version% %dest%

zip32 -@ %dest\wxWindows-%version%-WinHelp.zip < %src\distrib\msw\wx_hlp.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-WinHelp.zip wxWindows-%version% %dest%

zip32 -@ %dest\wxWindows-%version%-HTML.zip < %src\distrib\msw\wx_html.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-HTML.zip wxWindows-%version% %dest%

zip32 -@ %dest\wxWindows-%version%-PDF.zip < %src\distrib\msw\wx_pdf.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-PDF.zip wxWindows-%version% %dest%

Rem zip32 -@ %dest\wxWindows-%version%-Word.zip < %src\distrib\msw\wx_word.rsp
Rem call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-Word.zip wxWindows-%version% %dest%

zip32 -@ %dest\wxWindows-%version%-HTB.zip < %src\distrib\msw\wx_htb.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-HTB.zip wxWindows-%version% %dest%

zip32 -@ %dest\wxWindows-%version%-HTMLHelp.zip < %src\distrib\msw\wx_chm.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-HTMLHelp.zip wxWindows-%version% %dest%

Rem PDF/HTML docs that should go into the Windows setup because
Rem there are no WinHelp equivalents
zip32 -@ %dest\wxWindows-%version%-ExtraDoc.zip < %src\distrib\msw\extradoc.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-ExtraDoc.zip wxWindows-%version% %dest%

Rem zip up Univ-specific files
zip32 -@ %dest\wxWindows-%version%-Univ.zip < %src\distrib\msw\univ.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-Univ.zip wxWindows-%version% %dest%

rem VC++ project files
zip32 -@ %dest\wxWindows-%version%-VC.zip < %src\distrib\msw\vc.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-VC.zip wxWindows-%version% %dest%

rem BC++ project files
zip32 -@ %dest\wxWindows-%version%-BC.zip < %src\distrib\msw\bc.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-BC.zip wxWindows-%version% %dest%

rem CodeWarrior project files
zip32 -@ %dest\wxWindows-%version%-CW.zip < %src\distrib\msw\cw.rsp
call %WXWIN%\distrib\msw\rearchive wxWindows-%version%-CW.zip wxWindows-%version% %dest%

rem Dialog Editor source and binary
Rem erase %dest\dialoged-source.zip
Rem zip32 -@ %dest\dialoged-source.zip < %src\distrib\msw\dialoged.rsp
Rem zip32 -j %dest\dialoged-%version%.zip %dest\dialoged-source.zip %src\bin\dialoged.exe %src\docs\winhelp\dialoged.hlp %src\docs\winhelp\dialoged.cnt
Rem erase %dest\dialoged-source.zip

copy %src\docs\changes.txt %dest\changes-%version%.txt
copy %src\docs\msw\install.txt %dest\install-msw-%version%.txt
copy %src\docs\mac\install.txt %dest\install-mac-%version%.txt
copy %src\docs\motif\install.txt %dest\install-motif-%version%.txt
copy %src\docs\gtk\install.txt %dest\install-gtk-%version%.txt
copy %src\docs\readme.txt %dest\readme-%version%.txt
copy %src\docs\motif\readme.txt %dest\readme-motif-%version%.txt
copy %src\docs\gtk\readme.txt %dest\readme-gtk-%version%.txt
copy %src\docs\msw\readme.txt %dest\readme-msw-%version%.txt
copy %src\docs\mac\readme.txt %dest\readme-mac-%version%.txt
copy %src\docs\os2\install.txt %dest\install-os2-%version%.txt
copy %src\docs\readme_vms.txt %dest\readme-vms-%version%.txt
copy %src\docs\motif\makewxmotif %dest\makewxmotif-%version%
copy %src\docs\gtk\makewxgtk %dest\makewxgtk-%version%

Rem Skip Inno Setup if inno is 0.
if "%inno" == "0" goto end

:dounzip

Rem Make dialoged-win32.zip and tex2rtf-win32.zip

Rem cd %src\bin
Rem zip32 %dest\dialoged-win32-%version%.zip dialoged.*
Rem zip32 %dest\tex2rtf-win32-%version%.zip tex2rtf.*

cd %dest

rem Unzip the Windows files into 'wxWindows-%version%'

unzip32 -o wxWindows-%version%-win.zip
unzip32 -o wxWindows-%version%-VC.zip
unzip32 -o wxWindows-%version%-BC.zip
unzip32 -o wxWindows-%version%-HTMLHelp.zip
unzip32 -o wxWindows-%version%-ExtraDoc.zip
Rem Need Word file, for Remstar DB classes
Rem Not any more
Rem unzip32 -o wxWindows-%version%-Word.zip

Rem After this change of directory, we're in the
Rem temporary 'wx' directory and not acting on
Rem the source wxWindows directory.
cd %dest%\wxWindows-%version%

rem Now delete a few files that are unnecessary
attrib -R *
erase /Y BuildCVS.txt descrip.mms
erase /Y setup.h_vms
erase /Y docs\html\wxbook.htm docs\html\roadmap.htm
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
copy %src\bin\dialoged.exe bin
copy %src\bin\tex2rtf.exe bin
copy %src\bin\dbgview.* bin
copy %src\bin\widgets.exe bin
copy %src\bin\widgets.exe.manifest bin
copy %src\bin\life.exe bin
copy %src\bin\life.exe.manifest bin
copy %src\demos\life\breeder.lif bin
copy %src\docs\htmlhelp\dialoged.chm bin
copy %src\docs\htmlhelp\tex2rtf.chm bin

if not direxist docs\pdf mkdir docs\pdf
copy %src\docs\pdf\wxTutorial.pdf docs\pdf

Rem Make wxMSW-xxx.zip
cd %dest%
zip32 -r wxMSW-%version%.zip wxWindows-%version%/*
cd wxWindows-%version%

echo Calling 'makeinno' to generate wxwin2.iss...
call %WXWIN\distrib\msw\makeinno.bat

erase /Y %dest\setup.*

rem Now invoke Inno Setup on the new wxwin2.iss
set innocmd="C:\Program Files\Inno Setup 2\compil32.exe" /cc %WXWIN\distrib\msw\wxwin2.iss
echo Invoking %innocmd...
start "Inno Setup" /w %innocmd%
Rem pause Press any key to continue with the wxWindows distribution...

:tidyup
cd %dest

rem Put all the setup files into a single zip archive.
zip32 wxMSW-%version%-setup.zip readme-%version%.txt setup*.*

erase /Y wxWindows-%version%-win.zip
erase /Y wxWindows-%version%-ExtraDoc.zip

echo wxWindows archived.

goto end

:usage
echo DOS wxWindows distribution. Zips up all MSW, OS/2, Mac and doc files,
echo and optionally makes a setup.exe echo if you specify 'inno'
echo (skipping the zipping if you use 'innoonly').
echo.
echo "Usage: zipdistinno [inno | innoonly]"

:end
