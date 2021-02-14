CMake Overview                         {#overview_cmake}
==============
[TOC]

CMake allows building wxWidgets on various platforms with your preferred build
system.

Most linux distributions contain CMake as a package on Windows and macOS you can
download an installer at the [CMake Page](https://cmake.org).

Using the CMake GUI                    {#cmake_gui}
===================
1. Start the CMake GUI
2. Specify the wxWidgets root as your source folder
3. Specify a path where the build files should be created. It's recommended to
   use a path outside the wxWidgets root folder.
4. Press the *Configure* button and you will be asked which IDE or build system
   you wish to use
5. *Optionally*: Customize any of the options
6. Press the *Generate* button
7. Open the wxWidgets project with your preferred IDE

Using the CMake Command Line           {#cmake_cli}
============================
1. Create a folder where the build/project files should be created
2. Change into the created folder
3. Run `cmake -G "Unix Makefiles" path_to_wxWidgets_root`
4. After that you can run `cmake --build .` to start the build process or
   directly use your chosen build system.

Run `cmake --help` to see a list of available generators on your platform.
These can than be specified using the -G command line option. On Windows it
is recommended to use Visual Studio and on macOS Xcode is recommended.
Various build options can be specified using -D see
[available options](#cmake_options).

Command Line Examples                  {#cmake_cli_samples}
---------------------
Building with tests using [Ninja](https://ninja-build.org/):
~~~{.sh}
  cmake -G "Ninja" ~/Downloads/wxWidgets_3.1 -DwxBUILD_TESTS=ALL
  cmake --build .
~~~

Building the minimal sample:
~~~{.sh}
    cmake ~/Downloads/wxWidgets_3.1 -DwxBUILD_SAMPLES=SOME
    cmake --build . --target minimal
~~~

Installing static libraries to some path:
~~~{.sh}
    cmake ~/Downloads/wxWidgets_3.1 \
      -DCMAKE_INSTALL_PREFIX=~/wx_install \
      -DwxBUILD_SHARED=OFF
    cmake --build . --target install
~~~


Build options                          {#cmake_options}
=======================
The following list of build options can either be configured in the CMake UI
or specified via the -D command line option when running the cmake command.

Option Name               | Type   | Default | Description
------------------------- | -----  | ------- | ----------------------------
wxBUILD_SHARED            | BOOL   | ON      | Build shared or static libraries
wxBUILD_TESTS             | STRING | OFF     | CONSOLE_ONLY, ALL or OFF
wxBUILD_SAMPLES           | STRING | OFF     | SOME, ALL or OFF
wxBUILD_DEMOS             | BOOL   | OFF     | Build demo applications
wxUSE_GUI                 | BOOL   | ON      | Build the UI libraries
wxBUILD_COMPATIBILITY     | STRING | 3.0     | 2.8, 3.0 or 3.1 API compatibility
wxBUILD_PRECOMP           | BOOL   | ON      | Use precompiled headers
wxBUILD_MONOLITHIC        | BOOL   | OFF     | Build a single library

A complete list of options and advanced options can be found when using the
CMake GUI.

Recommendations                       {#cmake_recommendations}
=======================
While CMake in wxWidgets aims to support most generators available
in CMake the following generators are recommended:
* Windows: Visual Studio (any supported version)
* macOS: Xcode
* Linux: Ninja or Makefiles

CMake 3.10 or newer is recommended. The minimum version required is 2.8.12.

Using CMake with your applications     {#cmake_apps}
==================================
If you are using CMake with your own application there are various ways to use
wxWidgets:
* Using an installed, binary or compiled version of wxWidgets
using `find_package()`
* wxWidgets as a sub directory.
* CMake's [ExternalProject module](https://cmake.org/cmake/help/latest/module/ExternalProject.html).

Using find_package                     {#cmake_find_package}
------------------
You can use `find_package(wxWidgets)` to use a compiled version of wxWidgets.
Have a look at the [CMake Documentation](https://cmake.org/cmake/help/latest/module/FindwxWidgets.html)
for detailed instructions.

Your *CMakeLists.txt* would look like this:
~~~
...

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})
add_executable(myapp myapp.cpp)
target_link_libraries(myapp ${wxWidgets_LIBRARIES})
~~~

Using a sub directory                  {#cmake_subdir}
---------------------
You can use wxWidgets as a subdirectory in your application's build tree
e.g. as a git submodule. This way the wxWidgets libraries will be part
of your applications build process.

Your *CMakeLists.txt* would look like this:
~~~
...
add_subdirectory(libs/wxWidgets)
add_executable(myapp myapp.cpp)
target_link_libraries(myapp wx::net wx::core wx::base)
~~~
