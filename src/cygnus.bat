rem Cygnus Gnu-Win32 environment variables
rem Assumes that compiler and wxWindows are installed on the g: drive.
rem
set WXWIN=d:\wx2
path C:\WINDOWS;C:\WINDOWS\COMMAND;g:\gnuwin32\b19\H-i386-cygwin32\bin;g:\gnuwin32\b19\H-i386-cygwin32\lib\gcc-lib\i386-cygwin32\cygnus-2.7.2-970404;c:\bin;g:\gnuwin32\b19\tcl\bin
set GCC_EXEC_PREFIX=G:\gnuwin32\b19\H-i386-cygwin32\lib\gcc-lib\
set RCINCLUDE=%WXWIN\include
set CPLUS_INCLUDE_PATH=/g/gnuwin32/b19/h-i386-cygwin32/i386-cygwin32/include:/g/gnuwin32/b19/include/g++:/g/gnuwin32/b19/H-i386-cygwin32/lib/gcc-lib/i386-cygwin32/cygnus-2.7.2-970404/include:/d/wx2/include:/g/gnuwin32/b19/include/g++
set MAKE_MODE=unix
mount G: /g
mount D: /d

rem 4DOS users only...
unalias make
alias makegnu make -f makefile.g95
