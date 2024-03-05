wxWidgets for macOS installation        {#plat_osx_install}
-----------------------------------

[TOC]

wxWidgets can be compiled using Apple's Cocoa toolkit.

Most OS X developers should start by downloading and installing Xcode
from the App Store.  It is a free IDE from Apple that provides
all of the tools you need for working with wxWidgets.

After Xcode is installed, download `wxWidgets-{version}.tar.bz2` and then
double-click on it to unpack it to create a wxWidgets directory.

Next use Terminal (under Applications, Utilities, Terminal) to access a command
prompt.  Use `cd` to change directories to your wxWidgets directory and execute
the following sets of commands from the wxWidgets directory.

    mkdir build-cocoa-debug
    cd build-cocoa-debug
    ../configure --enable-debug
    make

It is recommended to use `-jN` option with the last command, where `N` is a
number of the processors in your system (which can be checked using `sysctl -n
hw.ncpu` command if you are not sure), as this will dramatically speed up the
build on modern systems. So in practice you should use a command like this:

    make -j8

(but don't use it unless you actually have 8 CPUs and enough memory for that
many parallel compiler invocations).

You may also prefer to add `-s` option to avoid normal output from make and/or
redirect it you to a log file for further inspection.

You should build at least the smallest possible wxWidgets sample to verify that
everything is working as intended, by doing

    cd samples/minimal
    make

and then running `minimal.app` from this directory from Finder.

If you'd like to, you can also build all the other samples and demos

    cd samples; make;cd ..
    cd demos;   make;cd ..


Advanced topics                        {#osx_advanced}
===============

Building library for distribution
---------------------------------

When building library for the distribution with your application, you shouldn't
use `--enable-debug` option above but you may want to use `--disable-sys-libs`
option to ensure that it has no dependencies on the other libraries available
on the current system as they might not be present on all systems where the
application is used.

It is also often desirable to build the final version of the application as
"universal binary", i.e. a combination of binaries for several different
architectures. In this case, you should build wxWidgets as universal binary
too, using `--enable-universal_binary` option. By default, this option enables
building for the usually wanted set of architectures (currently ARM and Intel)
but you may override this by listing the architectures you want to use
explicitly, separating them with commas.


Installing library                     {#osx_install}
------------------

It is rarely desirable to install non-Apple software into system directories,
so the recommended way of using wxWidgets under macOS is to skip the `make
install` step and simply use the full path to `wx-config` under the build
directory when building application using the library.

If you want to install the library into the system directories you'll need
to do this as root.  The accepted way of running commands as root is to
use the built-in sudo mechanism.  First of all, you must be using an
account marked as a "Computer Administrator".  Then

    sudo make install
    type \<YOUR OWN PASSWORD\>

Distributing applications using wxWidgets
-----------------------------------------

If you build wxWidgets as static libraries, i.e. pass `--disable-shared` option
to configure, you don't need to do anything special to distribute them, as all
the required code is linked into your application itself. When using shared
libraries (which is the default), you need to copy the libraries into your
application bundle and change their paths using `install_name_tool` so that
they are loaded from their new locations.

Apple Developer Tools: Xcode           {#osx_xcode}
----------------------------

You can use the project in build/osx/wxcocoa.xcodeproj to build the Cocoa
version of wxWidgets (wxOSX/Cocoa). There are also sample
projects supplied with the minimal sample.

Notice that the command line build above builds not just the library itself but
also wxrc tool which doesn't have its own Xcode project. If you need this tool,
the simplest possibility is to build it from the command line after installing
the libraries using commands like this:

    $ cd utils/wxrc
    $ g++ -o wxrc wxrc.cpp `wx-config --cxxflags --libs base,xml`
