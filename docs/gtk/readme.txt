   Welcome to wxWidgets for GTK
   ----------------------------

You have downloaded the GTK port of the wxWidgets GUI library.

This port works best with GTK 3, but also supports versions as
old as GTK 2.6 (although some features are only available when
using GTK+ 2.10 or higher). Note that even if you compile
wxWidgets against a later GTK version, your application will
still work with older GTK versions with the same major version
as wxWidgets tests for the presence of newer features at run-time
and it will fall back to a generic implementation if these
features are not present in the GTK version used.

More information about the wxWidgets project (including all the
other ports and version of wxWidgets) can be found at the
main wxWidgets homepage at:

    https://www.wxwidgets.org/

Information on how to install can be found in the file
install.md, but if you cannot wait, this should work on
many systems:

    mkdir build_gtk
    cd build_gtk
    ../configure
    make
    su <type root password>
    make install
    ldconfig

In order to use wxGTK with GTK 2 you need to use --with-gtk=2
configure option.

Please note that it is strongly advised to build the library in
a separate directory, as shown above, as it allows you to have
different builds of it (for example with and without debug) in
parallel. If you build in the source directory, don't forget to
do "make clean" before changing the configuration!

Please send problems concerning installation, feature requests,
bug reports or comments to the wxWidgets users list. These can
be found at https://www.wxwidgets.org/support/mailing-lists/

Please see the manual for further information.
