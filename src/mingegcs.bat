REM
REM replace g:\egcs-mingw32 with whatever your installation root may be.
REM
path C:\WINDOWS;C:\WINDOWS\COMMAND;g:\egcs-mingw32\bin;c:\bin

SET GCC_EXEC_PREFIX=g:\egcs-mingw32\lib\gcc-lib\
set BISON_SIMPLE=g:\egcs-mingw32\share\bison.simple
set BISON_HAIRY=g:\egcs-mingw32\share\bison.hairy

rem 4DOS users only...
unalias make
alias makeming make -f makefile.g95

