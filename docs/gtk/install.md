wxWidgets for GTK installation        {#plat_gtk_install}
-------------------------------

[TOC]

Installation                           {#gtk_install}
============


The simplest case                      {#gtk_simple}
-------------------

If you compile wxWidgets on Linux for the first time and don't like to read
install instructions just do the following in wxWidgets directory:

    $ mkdir buildgtk
    $ cd buildgtk
    $ ../configure --with-gtk
    $ make
    $ sudo make install
    $ sudo ldconfig

(if you get "ldconfig: command not found", try using `/sbin/ldconfig`)

If you don't do the `make install` part, you can still use the libraries from
the `buildgtk` directory, but they may not be available to other users.

Note that by default, GTK 3 is used. GTK 2 can be specified
with `--with-gtk=2` configure option.

If you want to use CMake, please see @ref overview_cmake for
building wxWidgets using it.


Prerequisites                      {#gtk_prereq}
-------------

To build wxWidgets, you need to have a number of libraries on your system.
While all of them may be compiled from source, under Linux systems it is much
more common to install the development packages containing the headers and
library files.

At the very least, you will need GTK libraries themselves. Unfortunately the
exact package names differ between different Linux distributions, but for
Debian and Debian-based distribution these libraries are part of `libgtk-3-dev`
package, while in Fedora and other RPM-based distributions the same package is
known as `gtk3-devel`.

For OpenGL support, you need `libgl1-mesa-dev` and `libglu1-mesa-dev` packages
under Debian and `mesa-libGL-devel` and `mesa-libGLU-devel` under Fedora. For
EGL support, `libegl1-mesa-dev` or `mesa-libEGL-devel` is needed.

wxMediaCtrl implementation requires GStreamer and its plugins development
packages, i.e. either `libgstreamer-plugins-baseVERSION-dev` or
`gstreamerVERSION-plugins-base-devel`, where `VERSION` should be 1.0, but 0.10
is still supported as well for compatibility with old systems.

wxWebRequest requites libcurl development package, e.g. `libcurl4-openssl-dev`
or `libcurl-devel`.

wxWebView requires `libwebkit2gtk-4.0-dev` under Debian and `webkit2gtk3-devel`
under Fedora.

For the different image format libraries (PNG, JPEG, TIFF) as well as zlib, you
may install the corresponding `libNAME-dev` or `-devel` libraries or use the
built-in versions of them, i.e. the same libraries compiled as part of
wxWidgets, instead if you prefer.



Troubleshooting                    {#gtk_errors_simple}
---------------

*IMPORTANT NOTE:* When sending bug reports tell us the exact version of
wxWidgets you are using as well as what compiler on what system. One example:
wxGTK 3.0.5, GCC 9.3.1, Fedora 31.

For any configure errors: please look at `config.log` file which was generated
during configure run, it usually contains some useful information.

If configure reports that you don't have GTK installed, please
check that the appropriate _development_ package is available on
your system and not just the GTK libraries themselves. You can
use `pkg-config --modversion gtk+-3.0` (or `2.0`) to check that
this is the case.

You get errors from make: please use GNU make instead of the native make
program. Currently wxWidgets can be built only with GNU make, BSD make and
Solaris make. Other versions might work or not.

You get immediate crashes when starting any sample or application: This may
be due to having compiled the library with different flags or
compiler options than your program or using the headers from a
different version of the library that is being used during
run-time. If you have multiple versions of wxWidgets installed,
please try uninstalling the ones you don't need and rebuilding.


The simplest program                   {#gtk_simple_app}
----------------------

Now create your super-application myfoo.cpp and compile anywhere with

    g++ myfoo.cpp `wx-config --libs --cxxflags` -o myfoo




Configure options                        {#gtk_options}
-----------------

This section documents some of the most important configure
options. It is not exhaustive, please refer to `configure --help`
for the full list of options.

Note that the options documented below are given in the form
indicating how to change the default behaviour, i.e. if it says
"--disable-optimise" it means that optimizations are enabled by
default.

All standard configure options are supported, e.g. you can choose
`--prefix` to select the directory to install the libraries to.
Also note that cross-compiling is fully supported, just specify
the `--host` option as usual (please refer to autoconf manual for
more information).

The following options can be used to specify the kind and number
of libraries to build:

 *  `--disable-shared` \n
    Do not create shared libraries, but build static libraries instead.

 * `--enable-monolithic` \n
   Build wxWidgets as single library instead of as several smaller libraries
   (which is the default since wxWidgets 2.5.0).

Options for third party dependencies: wxWidgets may use other
libraries present on the current system, see the @ref gtk_prereq
"prerequisites section above". For some of these libraries,
wxWidgets also provides built-in versions, that can be
linked into wx libraries themselves, which can be useful to
minimize external dependencies. By default, system versions will be
used if available, but `--with-xxx=builtin` configure option may be
used to override this.

 * `--disable-sys-libs` \n
   Don't use system libraries when there is a built-in version included in
   wxWidgets. This is equivalent to using --with-xxx=builtin for all libraries
   that have built-in versions. Note that other system libraries can, and
   typically will, still be used if found.

 * `--without-libpng` \n
   Disables PNG image format code. Use neither the system nor the builtin
   libpng (although GTK itself still uses it).

 * `--without-libjpeg` \n
   Disables JPEG image format code. Don't use libjpeg.

 * `--without-libtiff` \n
   Disables TIFF image format code. Don't use libtiff.

 * `--without-expat` \n
   Disable XML classes based on Expat parser. Don't use expat library.

 * `--without-liblzma` \n
   Disable LZMA compression support. Don't use liblzma.

 * `--without-libcurl` \n
   Don't use libcurl even if it's available. Disables wxWebRequest.

 * `--without-opengl` \n
   Disable OpenGL integration with wxGLCanvas. Don't use OpenGL or EGL libraries.

 * `--disable-glcanvasegl` \n
   Disable EGL support even if it is available (it would be used if it is, by default).

 * `--disable-mediactrl` \n
   Disable wxMediaCtrl. Don't use GStreamer libraries.

 * `--disable-webview` \n
   Disable wxWebView. Don't use webkit2gtk and its multiple dependencies.

Normally, you won't have to choose a toolkit, because configure
defaults to wxGTK anyhow. However you need to use this option to
explicitly specify the version of GTK to use, e.g.:

 * `--with-gtk=3` \n            Use GTK 3. Default.
 * `--with-gtk=2` \n            Use GTK 2.
 * `--with-gtk=1` \n            Use GTK 1.2. Obsolete.

Some other general compilation options:

 * `--disable-optimise` \n
   Do not optimise the code. Can be useful for debugging but shouldn't be used
   for production builds.

 * `--enable-no_rtti` \n
   Enable compilation without creation of C++ RTTI information in object files.
   This will speed-up compilation and reduce binary size.

 * `--enable-no_exceptions` \n
   Enable compilation without creation of C++ exception information in object
   files. This will speed-up compilation and reduce binary size.

 * `--enable-debug_info` \n
   Add debug info to object files and executables for use with debuggers such
   as gdb (or its many frontends).

 * `--enable-debug_flag` \n
   Enable internal debugging checks, that are very useful during development
   and allow to diagnose illegal parameters to wxWidgets functions as long as
   the application doesn't predefine `NDEBUG` (see @ref overview_debugging).

 * `--enable-debug` \n
   Same as `--enable-debug_info` and `--enable-debug_flag` together. Typically
   this is the option you want to use.

To reduce the final libraries (or executables, when linking
statically) size, many wxWidgets features may be disabled. Here
is a list of some of them:

 * `--disable-pnm` \n           Disables PNM image format code.

 * `--disable-gif` \n           Disables GIF image format code.

 * `--disable-pcx` \n           Disables PCX image format code.

 * `--disable-iff` \n           Disables IFF image format code.

 * `--disable-resources` \n     Disables the use of *.wxr type resources.

 * `--disable-threads` \n       Disables threads. Will also disable sockets.

 * `--disable-sockets` \n       Disables sockets.

 * `--disable-dnd` \n           Disables Drag'n'Drop.

 * `--disable-clipboard` \n     Disables Clipboard.

 * `--disable-streams` \n       Disables the wxStream classes.

 * `--disable-file` \n          Disables the wxFile class.

 * `--disable-textfile` \n      Disables the wxTextFile class.

 * `--disable-intl` \n          Disables the internationalisation.

 * `--disable-validators` \n    Disables validators.

Please remember that the full list of options can be seen in
`configure --help` output.



Building the libraries                              {#gtk_compling}
----------------------

After running configure, just run `make` from the same directory.
Building can take some time and it's strongly recommended to use
`--jobs` option with GNU make, e.g. `make -j8` if you have 8
logical CPU cores.

Then you may install the library and its header files under
`/usr/local/include/wx` and `/usr/local/lib` respectively. You
have to perform the following command as root, using either `su`
or `sudo`:

    $ make install

After installing, you can run `make clean` in the original
directory or just remove it entirely if you don't plan to build
wxWidgets again in the near future.

Note that installing the library is _not_ required and it can
also be used from the build directory by simply specifying the
full path to `wx-config` script located in it when building your
application.


Building your project                 {#gtk_new_project}
---------------------

You need to use the compilation flags returned by `wx-config
--cxxflags` and linking flags returned by `wx-config --libs`,
which may be combined into a single invocation:

        g++ myfoo.cpp `wx-config --cxxflags --libs` -o myfoo

A simple makefile for a program using wxWidgets could be written
in the following way (note that if you are copying and pasting
this into your makefile, the leading spaces must be replaced by a
`TAB` character):

    program: program.o
        $(CXX) -o program program.o `wx-config --libs`

    program.o: program.cpp
        $(CXX) `wx-config --cxxflags` -c program.cpp -o program.o

    clean:
        $(RM) program.o program
    .PHONY: clean

If your application uses only some of wxWidgets libraries, you can
specify required libraries when running wx-config. For example,
`wx-config --libs=html,core` will only output link command to link
with libraries required by core GUI classes and wxHTML classes. See
the manual for more information on the libraries.
