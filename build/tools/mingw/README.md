MinGW build tools
-----------------

This folder contains Ms Batch scripts used to create zip packages for officials
builds with differents versions of the MinGW compiler.

Building a new release
----------------------

1) If needed, update the versions list in the 'buildall.bat' script (the first 'for' loop of this file)

2) Update the 'parameters.bat" file to reflect your system and your installations of the differents MinGW versions. This script is first called without any parameter, to initialize generals values. It will then be called with the compiler version, and the number of bits of the architecture (32 or 64). Here are the lines that will probably need to be changed:
   * `set JOBS=3` : the value of this variable contains the number of processes that will be launched simultaneously. It is generally recommended to define it as "number of cores of your processor - 1"

   * `set CLEANBUILD=1` : setting this value to 0 will not remove old builds previously done. It is not recommended to modify it, unless you know what you're doing.

    * `set COMPBINDIR=G:\MinGW%1-%2\bin` : this line will surely need to be updated. The `COMPBINDIR` value must point to the binary folder of the corresponding MinGW installation. The `%1` parameter will contain the version of the current compiler (such as 730, 810, 920, ...) and the `%2` one will contain the number of bits of the architecture (32 or 64).

    * The 7Zip executable path can also be changed (see at the end of the 'parameters.bat' file).

3) Launch the builds by running the 'buildall.bat' script.

4) Check if nothing went wrong: Have a look at the main log file (a file named `_wxWidgets-x.x.x.log` in the `logs` sub directory). It will contain the result of each runned command line. If a value is different than zero, there was a problem
