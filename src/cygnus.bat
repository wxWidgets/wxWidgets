@ECHO OFF
SET MAKE_MODE=UNIX
rem SET PATH=g:\GNUWIN32\B20\CYGWIN~1\H-I586~1\BIN;%PATH%
PATH C:\WINDOWS;C:\WINDOWS\command;g:\GNUWIN32\B20\CYGWIN~1\H-I586~1\BIN;d:\wx\utils\tex2rtf\bin;g:\ast\astex;g:\ast\emtex\bin;g:\cvs;c:\bin
set BISON_SIMPLE=g:\gnuwin32\b20\cygwin-b20\share\bison.simple
set BISON_HAIRY=g:\gnuwin32\b20\cygwin-b20\share\bison.hairy
rem bash
rem 4DOS users only...
unalias make
alias makegnu make -f makefile.g95
