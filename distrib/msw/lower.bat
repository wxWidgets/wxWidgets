@echo off
Rem Make all files in a directory lower-case
dir /B > %TEMP%\files.tmp

set len=%@LINES[%TEMP%\files.tmp]
do i = 0 to %len by 1
    set line=%@LINE[%TEMP%\files.tmp,%i]
    ren %line% _%line%
    ren _%line% %@LOWER[%line%]
enddo

