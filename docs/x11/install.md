wxWidgets for X11 installation         {#plat_x11_install}
------------------------------

[TOC]

IMPORTANT NOTE:

  If you experience problems installing, please re-read these
  instructions and other related files (todo.txt, bugs.txt and
  osname.txt for your platform if it exists) carefully before
  mailing wxwin-users or the author. Preferably, try to fix the
  problem first and then send a patch to the author.

  When sending bug reports tell us what version of wxWidgets you are
  using (including the beta) and what compiler on what system. One
  example: wxX11 2.8.0, gcc 2.95.4, Redhat 6.2

First steps                            {#x11_first_steps}
===========

- Download wxX11-x.y.z.tgz, where x.y.z is the version number.
  Download documentation in a preferred format, such as
  wxWidgets-HTML.zip or wxWidgets-PDF.zip.

- Make a directory such as ~/wx and unarchive the files into this
  directory.

- It is recommended that you install bison and flex; using yacc
  and lex may require tweaking of the makefiles. You also need
  libXpm if you want to have XPM support in wxWidgets (recommended).

- You can now use configure to build wxWidgets and the samples.

  Using configure is the recommended way to build the library. If it doesn't
  work for you for whatever reason, please report it (together with detailed
  information about your platform and the (relevant part of) contents of
  config.log file).

Compiling using configure              {#x11_configure}
=========================

The simplest case                      {#x11_simple}
-------------------

If you compile wxWidgets on Linux for the first time and don't like to read
install instructions just do (in the base dir):

    > ./configure --with-x11
    > make
    > su <type root password>
    > make install
    > ldconfig
    > exit

Afterwards you can continue with

    > make
    > su <type root password>
    > make install
    > ldconfig
    > exit

If you want to remove wxWidgets on Unix you can do this:

    > su <type root password>
    > make uninstall
    > ldconfig
    > exit

The expert case                        {#x11_expert}
-----------------

If you want to do some more serious cross-platform programming with wxWidgets,
such as for GTK and X11, you can now build two complete libraries and use
them concurrently. For this end, you have to create a directory for each build
of wxWidgets - you may also want to create different versions of wxWidgets
and test them concurrently. Most typically, this would be a version configured
with --enable-debug_flag and one without. Note, that only one build can
currently be installed, so you'd have to use local version of the library for
that purpose.

For building three versions (one GTK, one X11 and a debug version of the GTK
source) you'd do this:

    md buildx11
    cd buildx11
    ../configure --with-x11
    make
    cd ..

    md buildgtk
    cd buildgtk
    ../configure --with-gtk
    make
    cd ..

    md buildgtkd
    cd buildgtkd
    ../configure --with-gtk --enable-debug_flag
    make
    cd ..

The simplest errors                    {#x11_errors}
---------------------

You get errors during compilation: The reason is that you probably have a
broken compiler.  GCC 2.8 and earlier versions and egcs are likely to cause
problems due to incomplete support for C++ and optimisation bugs.  Best to use
GCC 2.95 or later.

You get immediate segfault when starting any sample or application: This is
either due to having compiled the library with different flags or options than
your program - typically you might have the `__WXDEBUG__` option set for the
library but not for your program - or due to using a compiler with optimisation
bugs.

The simplest program                   {#x11_simple_app}
----------------------

Now create your super-application myfoo.cpp and compile anywhere with

    g++ myfoo.cpp `wx-config --libs --cxxflags` -o myfoo

General                                {#x11_general}
---------

The Unix variants of wxWidgets use GNU configure. If you have problems with
your make use GNU make instead.

If you have general problems with installation, see the wxWidgets website at

https://www.wxwidgets.org/

for newest information. If you still don't have any success, please send a bug
report to one of our mailing lists (see my homepage) INCLUDING A DESCRIPTION OF
YOUR SYSTEM AND YOUR PROBLEM, SUCH AS YOUR VERSION OF X, WHAT DISTRIBUTION
YOU USE AND WHAT ERROR WAS REPORTED. I know this has no effect, but I tried...

GUI libraries                          {#x11_libs_gui}
---------------

wxWidgets/X11 requires the X11 library to be installed on your system.

Additional libraries                   {#x11_libs_misc}
----------------------

wxWidgets/X11 requires a thread library and X libraries known to work with
threads.  This is the case on all commercial Unix-Variants and all
Linux-Versions that are based on glibc 2 except RedHat 5.0 which is broken in
many aspects.  As of writing this, virtually all Linux distributions have
+correct glibc 2 support.

You can disable thread support by running

    ./configure --disable-threads
    make
    su <type root password>
    make install
    ldconfig
    exit

Building wxX11 on Cygwin               {#x11_cygwin}
--------------------------

The normal build instructions should work fine on Cygwin. The one difference
with Cygwin is that when using the "--enable-shared" configure option (which
is the default) the API is exported explicitly using __declspec(dllexport)
rather than all global symbols being available.

This shouldn't make a difference using the library and should be a little
more efficient. However if an export attribute has been missed somewhere you
will see linking errors. If this happens then you can work around the
problem by setting LDFLAGS=-Wl,--export-all-symbols. Please also let us know
about it on the wx-dev mailing list.

Create your configuration              {#x11_config}
---------------------------

Usage:

	./configure [options]

If you want to use system's C and C++ compiler,
set environment variables CXX and CC as

	% setenv CC cc
	% setenv CXX CC
	% ./configure [options]

to see all the options please use:

	./configure --help

The basic philosophy is that if you want to use different
configurations, like a debug and a release version,
or use the same source tree on different systems,
you have only to change the environment variable OSTYPE.
(Sadly this variable is not set by default on some systems
in some shells - on SGI's for example). So you will have to
set it there. This variable HAS to be set before starting
configure, so that it knows which system it tries to
configure for.

Configure will complain if the system variable OSTYPE has
not been defined. And Make in some circumstances as well...


General options                        {#x11_options}
-----------------

Given below are the commands to change the default behaviour,
i.e. if it says "--disable-threads" it means that threads
are enabled by default.

You have to add --with-x11 on platforms, where X11 is
not the default (on Linux, configure will default to GTK).

	--with-x11              Use X11.

The following options handle the kind of library you want to build.

	--disable-threads       Compile without thread support. Threads
	                        support is also required for the
				socket code to work.

	--disable-shared        Do not create shared libraries.

    --enable-monolithic     Build wxWidgets as single library instead
                            of as several smaller libraries (which is
                            the default since wxWidgets 2.5.0).

	--disable-optimise	Do not optimise the code. Can
	                        sometimes be useful for debugging
				and is required on some architectures
				such as Sun with gcc 2.8.X which
				would otherwise produce segvs.

	--enable-profile        Add profiling info to the object
				files. Currently broken, I think.

	--enable-no_rtti        Enable compilation without creation of
	                        C++ RTTI information in object files.
				This will speed-up compilation and reduce
				binary size.

	--enable-no_exceptions  Enable compilation without creation of
	                        C++ exception information in object files.
				This will speed-up compilation and reduce
				binary size. Also fewer crashes during the
				actual compilation...

        --enable-permissive     Enable compilation without checking for strict
                                ANSI conformance.  Useful to prevent the build
                                dying with errors as soon as you compile with
                                Solaris' ANSI-defying headers.

	--enable-mem_tracing    Add built-in memory tracing.

	--enable-dmalloc        Use the dmalloc memory debugger.
	                        Read more at www.letters.com/dmalloc/

	--enable-debug_info	Add debug info to object files and
	                        executables for use with debuggers
				such as gdb (or its many frontends).

	--enable-debug_flag	Define __DEBUG__ and __WXDEBUG__ when
	                        compiling. This enable wxWidgets' very
				useful internal debugging tricks (such
				as automatically reporting illegal calls)
				to work. Note that program and library
				must be compiled with the same debug
				options.

Feature Options                        {#x11_feature_options}
-----------------

Many of the configure options have been thoroughly tested
in wxWidgets snapshot 6, but not yet all (ODBC not).

When producing an executable that is linked statically with wxX11
you'll be surprised at its immense size. This can sometimes be
drastically reduced by removing features from wxWidgets that
are not used in your program. The most relevant such features
are

	--without-libpng	Disables PNG image format code.

	--without-libjpeg	Disables JPEG image format code.

    --without-expat         Disable XML classes based on Expat parser.

        --disable-resources     Disables the use of *.wxr type
	                        resources.

	--disable-threads       Disables threads. Will also
	                        disable sockets.

	--disable-sockets       Disables sockets.

	--disable-dnd           Disables Drag'n'Drop.

	--disable-clipboard     Disables Clipboard.

	--disable-serial        Disables object instance serialisation.

	--disable-streams       Disables the wxStream classes.

	--disable-file          Disables the wxFile class.

	--disable-textfile      Disables the wxTextFile class.

	--disable-intl          Disables the internationalisation.

	--disable-validators    Disables validators.

	--disable-accel         Disables accel.

Apart from disabling certain features you can very often "strip"
the program of its debugging information resulting in a significant
reduction in size.

Please see the output of "./configure --help" for comprehensive list
of all configurable options.


Compiling                              {#x11_compiling}
-----------

The following must be done in the base directory (e.g. ~/wxX11
or whatever)

Now the makefiles are created (by configure) and you can compile
the library by typing:

	make

make yourself some coffee, as it will take some time. On an old
386SX possibly two weeks. During compilation, you'll get a few
warning messages depending in your compiler.

If you want to be more selective, you can change into a specific
directory and type "make" there.

Then you may install the library and its header files under
/usr/local/include/wx and /usr/local/lib respectively. You
have to log in as root (i.e. run "su" and enter the root
password) and type

        make install

You can remove any traces of wxWidgets by typing

        make uninstall

If you want to save disk space by removing unnecessary
object-files:

	make clean

in the various directories will do the work for you.

Creating a new Project                 {#x11_new_app}
------------------------

1. The first way uses the installed libraries and header files
automatically using wx-config

        g++ myfoo.cpp `wx-config --libs` `wx-config --cxxflags` -o myfoo

    Using this way, a make file for the minimal sample would look
    like this

        CXX = g++

        minimal: minimal.o
            $(CXX) -o minimal minimal.o `wx-config --libs`

        minimal.o: minimal.cpp
            $(CXX) `wx-config --cxxflags` -c minimal.cpp -o minimal.o

        clean:
            rm -f *.o minimal

    This is certain to become the standard way unless we decide
    to stick to tmake.

    If your application uses only some of wxWidgets libraries, you can
    specify required libraries when running wx-config. For example,
    `wx-config --libs=html,core` will only output link command to link
    with libraries required by core GUI classes and wxHTML classes. See
    the manual for more information on the libraries.

2. The other way creates a project within the source code
directories of wxWidgets. For this endeavour, you'll need
GNU autoconf version 2.14 and add an entry to your Makefile.in
to the bottom of the configure.in script and run autoconf
and configure before you can type make.

Further notes by Julian Smart          {#x11_notes}
-------------------------------

- You may find the following script useful for compiling wxX11,
  especially if installing from zips (which don't preserve file
  permissions). Make this script executable with the command
  chmod a+x makewxx11.

        # makewxx11
        # Sets permissions (in case we extracted wxX11 from zip files)
        # and makes wxX11.
        # Call from top-level wxWidgets directory.
        # Note that this uses standard (but commonly-used) configure options;
        # if you're feeling brave, you may wish to compile with threads:
        # if they're not supported by the target platform, they will be disabled
        # anyhow
        # -- Julian Smart
        chmod a+x configure config.sub config.guess
        ./configure --with-x11 --with-shared --with-debug_flag --with-debug_info --enable-debug --without-threads --without-sockets --without-odbc
        make

  This script will build wxX11 using shared libraries. If you want to build
  a static wxWidgets library, use --disable-shared.

Troubleshooting                        {#x11_troubleshooting}
---------------

- Solaris compilation with gcc: if the compiler has problems with the variable
  argument functions, try putting the gcc fixinclude file paths early in the
  include path.

- If you get an internal compiler error in gcc, turn off optimisations.

- Some compilers, such as Sun C++, may give a lot of warnings about
  virtual functions being hidden. Please ignore these, it's correct C++ syntax.
  If you find any incorrect instances, though, such as a
  missing 'const' in an overridden function, please let us know.

Other Notes                            {#x11_misc_notes}
-----------

- Debugging mode is switched on by default in the makefiles, but using
  configure will create a release build of the library by default: it's
  recommended to use --with-debug_info and --with-debug_flag configure
  switches while developing your application. To compile in non-debug
  mode, remove the -D__WXDEBUG__ switch in make.env (or if using the
  configure system, change --with-debug_flag to --without-debug_flag
  and --with-debug_info to --without-debug_info in the makewxx11
  script).
