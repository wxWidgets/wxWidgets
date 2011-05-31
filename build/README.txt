autogen.mk

autogen.mk is a makefile provided to automatically update the GNU autotools
build system.  It will run Bakefile, aclocal, and autoconf as appropriate.

The .m4 files required for running aclocal are located in build/aclocal.
When upgrading software that wxWidgets depends on (for example, Bakefile,
cppunit, GTK, SDL, or others) it is advisable to upgrade the .m4 files
located in the build/aclocal directory.

It is particularly important that if you use a newer version of Bakefile
to generate the Makefile.in files that you use the newer bakefile*.m4.
Because build/autogen.mk is a Makefile it will automatically rerun
aclocal and autoconf as necessary whenever any m4 in build/aclocal is
newer than the generated aclocal.m4.

You can achieve this simply by copying the new bakefile*.m4 files from 
PREFIX/share/aclocal/ into the build/aclocal/ directory and
rerunning make -f build/autogen.mk.  Note that you should _not_ preserve
source file times (don't use cp -p) or else it's possible your .m4 files
will be older than the generated aclocal.m4).
Example:
cp /usr/share/aclocal/bakefile*.m4 build/aclocal/
make -f build/autogen.mk

Please don't forget to commit updated .m4 files as well as updated aclocal.m4,
configure, and Makefile.in files to wxWidgets.
