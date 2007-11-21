@echo off
rem Tar up an external distribution of wxWidgets 2.0: but
rem putting in separate ASCII and binary files
rem This seems to be the one that works, using
rem separate tar programs for conversion/non-conversion
rem of ASCII/binary files.

if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external wxWidgets 2.0 distribution:
echo   From   %1
echo   To     %2\wx200_1.tgz, %2\wx200_2.tgz, %2\wx200hlp.tgz, %2\wx200ps.tgz, %2\wx200htm.tgz
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\*.tgz
cd %1

rem First, expand the wildcards in the rsp files

rem Create empty list file
erase %1\distrib\*.lis
c:\bin\touch %1\distrib\wx200asc.lis
c:\bin\touch %1\distrib\wx200bin.lis
c:\bin\touch %1\distrib\wx200hlp.lis
c:\bin\touch %1\distrib\wx200ps.lis
c:\bin\touch %1\distrib\wx200xlp.lis

rem Create a .rsp file with backslashes instead
rem of forward slashes
rem No need if using ls2 (from UNIX95 distribution)
rem sed -e "s/\//\\/g" %1\distrib\wx_asc.rsp > %1\distrib\wx_asc.rs2

call %1\distrib\expdwild.bat %1\distrib\wx_asc.rsp %1\distrib\wx200asc.lis
call %1\distrib\expdwild.bat %1\distrib\util_asc.rsp %1\distrib\wx200asc.lis
call %1\distrib\expdwild.bat %1\distrib\smpl_asc.rsp %1\distrib\wx200asc.lis
rem call %1\distrib\expdwild.bat %1\distrib\wxim1asc.rsp %1\distrib\wx200asc.lis
rem call %1\distrib\expdwild.bat %1\distrib\wxim2asc.rsp %1\distrib\wx200asc.lis

call %1\distrib\expdwild.bat %1\distrib\wx_bin.rsp %1\distrib\wx200bin.lis
call %1\distrib\expdwild.bat %1\distrib\util_bin.rsp %1\distrib\wx200bin.lis
call %1\distrib\expdwild.bat %1\distrib\smpl_bin.rsp %1\distrib\wx200bin.lis
rem call %1\distrib\expdwild.bat %1\distrib\wxim1bin.rsp %1\distrib\wx200bin.lis

rem Docs
call %1\distrib\expdwild.bat %1\distrib\wx_hlp.rsp %1\distrib\wx200hlp.lis
call %1\distrib\expdwild.bat %1\distrib\wx_ps.rsp %1\distrib\wx200ps.lis
call %1\distrib\expdwild.bat %1\distrib\wx_html.rsp %1\distrib\wx200htm.lis
call %1\distrib\expdwild.bat %1\distrib\wx_pdf.rsp %1\distrib\wx200pdf.lis

rem Do some further massaging of the .lis files
sed -e "s/\\/\//g" %1\distrib\wx200asc.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx200asc.lis

sed -e "s/\\/\//g" %1\distrib\wx200bin.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx200bin.lis

sed -e "s/\\/\//g" %1\distrib\wx200hlp.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx200hlp.lis

sed -e "s/\\/\//g" %1\distrib\wx200ps.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx200ps.lis

sed -e "s/\\/\//g" %1\distrib\wx200htm.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx200htm.lis

sed -e "s/\\/\//g" %1\distrib\wx200pdf.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx200pdf.lis

rem 'tar' converts linefeeds.
tar -c -T %1\distrib\wx200asc.lis -f %2\wx200.tar
rem pause Press a key to continue.

rem This converts to lower case
ren %2\wx200.tar %2\wx200_1.tar
gzip32 %2\wx200_1.tar
ren %2\wx200_1.tar.gz %2\wx200_1.tgz

rem No linefeed conversion wanted
rem Note: GNU tar seems to crash with a full destination path, so
rem pander to it.
targnu -c -T %1\distrib\wx200bin.lis -f wx200_2.tar
move wx200_2.tar %2
gzip32 %2\wx200_2.tar
ren %2\wx200_2.tar.gz %2\wx200_2.tgz

targnu -c -T %1\distrib\wx200hlp.lis -f wx200_hlp.tar
move wx200_hlp.tar %2
gzip32 %2\wx200_hlp.tar
ren %2\wx200_hlp.tar.gz %2\wx200hlp.tgz

tar -c -T %1\distrib\wx200ps.lis -f %2\wx200ps.tar
gzip32 %2\wx200ps.tar
ren %2\wx200ps.tar.gz %2\wx200ps.tgz

targnu -c -T %1\distrib\wx200htm.lis -f wx200htm.tar
move wx200htm.tar %2
gzip32 %2\wx200htm.tar
ren %2\wx200htm.tar.gz %2\wx200htm.tgz

targnu -c -T %1\distrib\wx200pdf.lis -f wx200pdf.tar
move wx200pdf.tar %2
gzip32 %2\wx200pdf.tar
ren %2\wx200pdf.tar.gz %2\wx200pdf.tgz

cd %2
echo wxWidgets archived.
goto end

:usage
echo Tar/gzip wxWidgets distribution under DOS, making an ASCII and binary file
echo Usage: tardist source destination
echo e.g. tardist d:\wx d:\wx\deliver

:end


