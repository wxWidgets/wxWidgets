REM
REM replace c:\gcc-2.95 with whatever your installation root may be.
REM GCC_EXEC_PREFIX is optional, and hardly ever needs to be set (read:
REM leave it alone).
REM
PATH=c:\gcc-2.95\bin;%PATH%
set BISON_SIMPLE=c:\gcc-2.95\share\bison.simple
set BISON_HAIRY=c:\gcc-2.95\share\bison.hairy

REM SET GCC_EXEC_PREFIX=c:\gcc-2.95\lib\gcc-lib\set LIBRARY_PATH=e:\mingw32\lib;e:\mingw32\lib\gcc-lib\i386-mingw32\2.8.1

rem 4DOS users only...
unalias make
alias makeming make -f makefile.g95
