@echo off
Rem Replace a string with another string in the given files.
Rem The first argument is the string to be replaced.
Rem The second argument is the replacement string.
Rem The third argument is a file listing the files to be processed.

set keyword=%1
set repl=%2
set files=%3
echo s/%keyword/%repl/g > script.tmp

call %wxwin\distrib\msw\expdwild.bat %3 list.tmp

set len=%@LINES[%files]
do i = 0 to %len by 1
  set line=%@LINE[list.tmp,%i]
  if "%line" == "**EOF**" enddo
rem  echo Cmd: grep %keyword %line
  grep %keyword %line > size.tmp
  if "%@FILESIZE[size.tmp]" == "0" enddo
  sed -f script.tmp %line > file.tmp
  move file.tmp %line
enddo
erase script.tmp
erase list.tmp
erase size.tmp
:end
