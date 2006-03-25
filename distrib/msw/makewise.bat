@echo off
Rem Make a WISE Installer distribution list, where each file is represented by
Rem a section like this:
Rem 
Rem item: Install File
Rem   Source=d:\wx2\thefile.txt
Rem   Destination=%MAINDIR%\thefile.txt
Rem   Flags=0000000000000010
Rem end

Rem Generate a list of all files in the distribution.
dir /BS >& %TEMP\files1.tmp

Rem Now we iterate through the list of files, writing out the middle section of
Rem the file.
Rem We have to remove the first part of the path,
Rem by truncating the start by the size of the current directory.

set sz=%@EVAL[%@LEN[%_CWD]+1]
set len=%@LINES[%TEMP\files1.tmp]

erase /Y %TEMP\files2.tmp

Rem, first add system files
Rem NB: not doing this for now, because I'm not sure which are the right files
Rem for Win9x/NT/W2K
Rem cat %WXWIN\distrib\msw\wisesys.txt > %temp\files2.tmp

echo. > %temp\files2.tmp

do i = 0 to %len by 1
    set line=%@LINE[%TEMP\files1.tmp,%i]
    Rem Skip directories.
    if isdir %line enddo
    set sz2=%@LEN[%line]
    set n=%@EVAL[%sz2-%sz]
    set line2=%@SUBSTR[%line,%sz,%n]

    Rem Echo the file section
    echo item: Install File >> %TEMP\files2.tmp
    echo   Source=%line >> %TEMP\files2.tmp
    echo   Destination=%%MAINDIR%%\%line2 >> %TEMP\files2.tmp
    echo   Flags=0000000000000010 >> %TEMP\files2.tmp
    echo end >> %TEMP\files2.tmp
enddo

Rem Concatenate the 3 sections
cat %WXWIN\distrib\msw\wisetop.txt %TEMP\files2.tmp %WXWIN\distrib\msw\wisebott.txt > %WXWIN\distrib\msw\wxwin2.wse

erase /Y %TEMP\files1.tmp
rem erase /Y %TEMP\files2.tmp

goto end

:end
