@echo off
rem Tar up an external distribution of OGL

if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external OGL distribution:
echo   From   %1
echo   To     %2\ogl.tgz
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\ogl.tgz
cd %1

rem First, expand the wildcards in the ogl.rsp file

rem Create empty list file
erase %1\distrib\ogl.lis
c:\bin\touch %1\distrib\ogl.lis

rem Create a .rsp file with backslashes instead
rem of forward slashes
rem No need if using ls2 (from UNIX95 distribution)
rem sed -e "s/\//\\/g" %1\distrib\ogl.rsp > %1\distrib\ogl.rs2

set len=%@LINES[%1\distrib\ogl.rsp]
rem set len=%@DEC[%len]
do i = 0 to %len by 1
  set line=%@LINE[%1\distrib\ogl.rsp,%i]
  if NOT "%line" == "" ls2 -1 %line >> %1\distrib\ogl.lis
enddo

tar -c -T %1\distrib\ogl.lis
move archive.tar ogl.tar
gzip ogl.tar
move ogl.taz %2\ogl.tgz

echo OGL archived.
goto end

:usage
echo DOS OGL distribution.
echo Usage: tarogl source destination
echo e.g. tarogl c:\wx\utils\ogl c:\wx\utils\ogl\deliver

:end


