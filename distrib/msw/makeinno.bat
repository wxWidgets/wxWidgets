@echo off
Rem Make an Inno Setup distribution list, where files and dirs are represented by
Rem sections like this:
Rem [Dirs]
Rem     Name: {app}\docs
Rem 
Rem [Files]
Rem     Source: c:\wx2stable\wxWindows\deliver2\docs\readme.txt; DestDir: {app}\docs\; DestName: readme.txt

set wxwindir=%WXWIN

Rem Generate a list of all files in the distribution.
Rem We have to be in the right directory already.
dir /BS >& %TEMP\files1.tmp

Rem Now we iterate through the list of files, writing out the middle section of
Rem the file.
Rem We have to remove the first part of the path,
Rem by truncating the start by the size of the current directory.

set sz=%@EVAL[%@LEN[%_CWD]+1]
set len=%@LINES[%TEMP\files1.tmp]

erase /Y %TEMP\files2.tmp

Rem Add directories

echo. >> %temp\files2.tmp
echo [Dirs] >> %temp\files2.tmp

do i = 0 to %len by 1
    set line=%@LINE[%TEMP\files1.tmp,%i]
    Rem Skip files.
    if not isdir %line enddo
    set sz2=%@LEN[%line]
    set n=%@EVAL[%sz2-%sz]
    set line2=%@SUBSTR[%line,%sz,%n]
    if "%line2%" == "" enddo

    echo   Name: {app}\%line2 >> %TEMP\files2.tmp
enddo

echo. >>%TEMP\files2.tmp

Rem Add files

echo. >> %temp\files2.tmp
echo [Files] >> %temp\files2.tmp

do i = 0 to %len by 1
    set line=%@LINE[%TEMP\files1.tmp,%i]
    Rem Skip directories.
    if isdir %line enddo
    set sz2=%@LEN[%line]
    set n=%@EVAL[%sz2-%sz]
    set line2=%@SUBSTR[%line,%sz,%n]
    set filename=%@FILENAME[%line2]
    set dirname=%@PATH[%line2]

    echo   Source: %line; DestDir: {app}\%dirname; DestName: %filename >> %TEMP\files2.tmp
enddo

Rem Concatenate the 3 sections
cat %wxwindir\distrib\msw\innotop.txt %TEMP\files2.tmp %wxwindir\distrib\msw\innobott.txt > %wxwindir\distrib\msw\wxwin2.iss

erase /Y %TEMP\files1.tmp
rem erase /Y %TEMP\files2.tmp

goto end

:end
