MinGW build tools
-----------------

This folder contains batch files used to create zip packages for official
builds with different versions of the MinGW compiler.

Building a new release
----------------------

1. If needed, update the versions list in the 'buildall.bat' script (the first 'for' loop of this file)
2. Update the 'parameters.bat" file to reflect your system and your installations of the different MinGW versions. This script is first called without any parameter, to initialize generals values. It will then be called with the compiler version, and the number of bits of the architecture (32 or 64). Here are the lines that will probably need to be changed:
  * `set /A JOBS=%NUMBER_OF_PROCESSORS%-1` : the value of this variable contains the number of processes that will be launched simultaneously. It is automatically calculated as "number of cores of your processor - 1" which is the recommended value. It can be overridden by writing something like `set JOBS=3` to directly assign a numerical value to this variable.
  * `set CLEANBUILD=1` : setting this value to 0 will not remove old builds previously done. It is not recommended to modify it, unless you know what you're doing.
  * `set COMPBINDIR=G:\msys64\mingw%2\bin` and/or `set COMPBINDIR=G:\MinGW%1-%2\bin` : these lines will surely need to be updated. The `COMPBINDIR` value must point to the binary folder of the corresponding MinGW installation. The `%1` parameter will contain the version of the current compiler (such as 730, 810, 1020, 920, ...) and the `%2` one will contain the number of bits of the architecture (32 or 64).
  * The 7Zip executable path can also be changed (see at the end of the 'parameters.bat' file).

3) Launch the builds by running the 'buildall.bat' script.

4) Check if nothing went wrong: Have a look at the main log file (a file named `_wxWidgets-x.x.x.log` in the `logs` sub directory). It will contain the result of each executed command line. If a value is different than zero, there was a problem.

Current compilers versions
--------------------------

The following versions of MinGW are actually used to create binaries (they all use Win32 threads):

  * MinGW-W64 GCC-7.3.0 using SJLJ exceptions for 32 bits architecture downloadable from [here](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/7.3.0/threads-win32/sjlj/i686-7.3.0-release-win32-sjlj-rt_v5-rev0.7z "MinGW-W64 GCC-7.3.0 i686 Win32 SJLJ") and SEH exceptions for 64 bits architecture downloadable from [here](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/7.3.0/threads-win32/seh/x86_64-7.3.0-release-win32-seh-rt_v5-rev0.7z "MinGW-W64 GCC-7.3.0 x86_64 Win32 SEH")
  * MinGW-W64 GCC-8.1.0 using SJLJ exceptions for 32 bits architecture downloadable from [here](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/8.1.0/threads-win32/sjlj/i686-8.1.0-release-win32-sjlj-rt_v6-rev0.7z "MinGW-W64 GCC-8.1.0 i686 Win32 SJLJ") and SEH exceptions for 64 bits architecture downloadable from [here](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-win32/seh/x86_64-8.1.0-release-win32-seh-rt_v6-rev0.7z "MinGW-W64 GCC-8.1.0 x86_64 Win32 SEH")
  * MinGW-TDM GCC-10.3.0 downloadable from [here](https://jmeubank.github.io/tdm-gcc/articles/2021-05/10.3.0-release"TDM GCC-10.3.0")
  * MSYS2 MinGW GCC-12.1.0 which is the last version installable using [MSYS2](https://www.msys2.org "www.msys2.org") package manager
