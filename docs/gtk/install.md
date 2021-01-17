wxWidgets for GTK installation        {#plat_gtk_install}
-------------------------------

[TOC]

Installation                           {#gtk_install}
============


The simplest case                      {#gtk_simple}
-------------------

If you compile wxWidgets on Linux for the first time and don't like to read
install instructions just do the following in wxWidgets directory:

    > mkdir buildgtk
    > cd buildgtk
    > ../configure --with-gtk
    > make
    > su <type root password>
    > make install
    > ldconfig

[if you get "ldconfig: command not found", try using "/sbin/ldconfig"]

If you don't do the 'make install' part, you can still use the libraries from
the buildgtk directory, but they may not be available to other users.

If you want to remove wxWidgets on Unix you can do this:

    > su <type root password>
    > make uninstall
    > ldconfig

Note that by default, GTK 3 is used. GTK 2 can be specified
with --with-gtk=2.

If you use CMake, please see @ref overview_cmake for
building wxWidgets using it.


Troubleshooting                    {#gtk_errors_simple}
---------------

IMPORTANT NOTE:

  When sending bug reports tell us what version of wxWidgets you are
  using (including the beta) and what compiler on what system. One
  example: wxGTK 3.0.5, GCC 9.3.1, Fedora 31.

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



Building wxGTK on Cygwin               {#gtk_cygwin}
--------------------------

The normal build instructions should work fine on Cygwin. The one difference
with Cygwin is that when using the "--enable-shared" configure option (which
is the default) the API is exported explicitly using `__declspec(dllexport)`
rather than all global symbols being available.

This shouldn't make a difference using the library and should be a little
more efficient. However if an export attribute has been missed somewhere you
will see linking errors. If this happens then you can work around the
problem by setting `LDFLAGS=-Wl,--export-all-symbols`. Please also let us know
about it on the wx-dev mailing list.


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

    --disable-shared        Do not create shared libraries, but
                            build static libraries instead.

    --enable-monolithic     Build wxWidgets as single library instead
                            of as several smaller libraries (which is
                            the default since wxWidgets 2.5.0).

Options for third party dependencies: wxWidgets may use other
libraries present on the current system. For some of these
libraries, wxWidgets also provides built-in versions, that can be
linked into wx libraries themselves, which can be useful to
minimize external dependencies.

    --disable-sys-libs      Don't use system libraries at all.
                            Use built-in ones when possible or
                            disable support for the corresponding
                            feature otherwise.

    --without-libpng        Disables PNG image format code.
                            Don't use libpng (although GTK
                            itself still uses it).

    --without-libjpeg       Disables JPEG image format code.
                            Don't use libjpeg.

    --without-libtiff       Disables TIFF image format code.
                            Don't use libtiff.

    --without-expat         Disable XML classes based on Expat parser.
                            Don't use expat library.

    --without-liblzma       Disable LZMA compression support.
                            Don't use liblzma.

    --without-libcurl       Don't use libcurl even if it's available.
                            Disables wxWebRequest.

    --without-opengl        Disable OpenGL integration with wxGLCanvas.
                            Don't use OpenGL or EGL libraries.

    --disable-glcanvasegl   Disable EGL support even if it is available
                            (it would be used if it is, by default).

    --disable-mediactrl     Disable wxMediaCtrl.
                            Don't use GStreamer libraries.

    --disable-webview       Disable wxWebView.
                            Don't use webkit2gtk and its multiple
                            dependencies.

Normally, you won't have to choose a toolkit, because configure
defaults to wxGTK anyhow. However you need to use this option to
explicitly specify the version of GTK to use, e.g.:

    --with-gtk=3            Use GTK 3. Default.
    --with-gtk=2            Use GTK 2.
    --with-gtk=1            Use GTK 1.2. Obsolete.

Some other general compilation options:

    --disable-optimise      Do not optimise the code. Can be useful
                            for debugging but shouldn't be used
                            for production builds.

    --disable-unicode       Disable Unicode support. Not recommended.

    --enable-no_rtti        Enable compilation without creation of
                            C++ RTTI information in object files.
                            This will speed-up compilation and reduce
                            binary size.

    --enable-no_exceptions  Enable compilation without creation of
                            C++ exception information in object files.
                            This will speed-up compilation and reduce
                            binary size.

    --enable-debug_info     Add debug info to object files and
                            executables for use with debuggers
                            such as gdb (or its many frontends).

    --enable-debug_flag     Define __DEBUG__ and __WXDEBUG__ when
                            compiling. This enable wxWidgets' very
                            useful internal debugging tricks (such
                            as automatically reporting illegal calls)
                            to work. Note that program and library
                            must be compiled with the same debug
                            options.

    --enable-debug          Same as --enable-debug_info and
                            --enable-debug_flag together. Unless you have
                            some very specific needs, you should use this
                            option instead of --enable-debug_info/flag ones
                            separately.

To reduce the final libraries (or executables, when linking
statically) size, many wxWidgets features may be disabled. Here
is a list of some of them:

    --disable-pnm           Disables PNM image format code.

    --disable-gif           Disables GIF image format code.

    --disable-pcx           Disables PCX image format code.

    --disable-iff           Disables IFF image format code.

    --disable-resources     Disables the use of *.wxr type resources.

    --disable-threads       Disables threads. Will also disable sockets.

    --disable-sockets       Disables sockets.

    --disable-dnd           Disables Drag'n'Drop.

    --disable-clipboard     Disables Clipboard.

    --disable-streams       Disables the wxStream classes.

    --disable-file          Disables the wxFile class.

    --disable-textfile      Disables the wxTextFile class.

    --disable-intl          Disables the internationalisation.

    --disable-validators    Disables validators.

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

        make install

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

```make
program: program.o
    $(CXX) -o program program.o `wx-config --libs`

program.o: program.cpp
    $(CXX) `wx-config --cxxflags` -c program.cpp -o program.o

clean:
    $(RM) program.o program
.PHONY: clean
```

If your application uses only some of wxWidgets libraries, you can
specify required libraries when running wx-config. For example,
`wx-config --libs=html,core` will only output link command to link
with libraries required by core GUI classes and wxHTML classes. See
the manual for more information on the libraries.
