rem Mingw32 environment variables
rem
set WXWIN=d:\wx2
path C:\WINDOWS;C:\WINDOWS\COMMAND;e:\mingw32\bin;e:\mingw32\lib\gcc-lib\i386-mingw32\2.8.1;c:\bin
rem set GCC_EXEC_PREFIX=G:\gnuwin32\b18\H-i386-cygwin32\lib\gcc-lib\
set RCINCLUDE=%WXWIN\include
rem set CPLUS_INCLUDE_PATH=/g/gnuwin32/b18/h-i386-cygwin32/i386-cygwin32/include:/g/gnuwin32/b18/include/g++:/g/gnuwin32/b18/H-i386-cygwin32/lib/gcc-lib/i386-cygwin32/cygnus-2.7.2-970404/include:/d/wx2/include:/g/gnuwin32/b18/include/g++
set CPLUS_INCLUDE_PATH=e:\mingw32\include;e:\mingw32\lib\gcc-lib\i386-mingw32\2.8.1\include;e:\mingw32\include\g++
set C_INCLUDE_PATH=e:\mingw32\include;e:\mingw32\lib\gcc-lib\i386-mingw32\2.8.1\include;e:\mingw32\include\g++
set LIBRARY_PATH=e:\mingw32\lib;e:\mingw32\lib\gcc-lib\i386-mingw32\2.8.1
set BISON_SIMPLE=e:\mingw32\share\bison.simple
set BISON_HAIRY=e:\mingw32\share\bison.hairy

rem 4DOS users only...
unalias make
alias makeming make -f makefile.g95
