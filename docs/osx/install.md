wxWidgets for macOS installation        {#plat_osx_install}
-----------------------------------

[TOC]

wxWidgets can be compiled using Apple's Cocoa toolkit.

Most OS X developers should start by downloading and installing Xcode
from the App Store.  It is a free IDE from Apple that provides
all of the tools you need for working with wxWidgets.

After Xcode is installed, download wxWidgets-{version}.tar.bz2 and then
double-click on it to unpack it to create a wxWidgets directory.

Next use Terminal (under Applications, Utilities, Terminal) to access a command
prompt.  Use cd to change directories to your wxWidgets directory and execute
the following sets of commands from the wxWidgets directory.

    mkdir build-cocoa-debug
    cd build-cocoa-debug
    ../configure --enable-debug
    make

Build the samples and demos

    cd samples; make;cd ..
    cd demos;   make;cd ..

After the compilation completes, use Finder to run the samples and demos
* Go to build-cocoa-debug/samples to experiment with the Cocoa samples.
* Go to build-cocoa-debug/demos to experiment with the Cocoa demos.
* Double-click on the executables which have an icon showing three small squares.
* The source code for the samples is in wxWidgets/samples
* The source code for the demos is in wxWidgets/demos

More information about building on macOS is available in the wxWiki.
Here are two useful links
  * https://wiki.wxwidgets.org/Guides_%26_Tutorials
  * https://wiki.wxwidgets.org/Development:_wxMac


Advanced topics                        {#osx_advanced}
===============

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
