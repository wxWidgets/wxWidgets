   Welcome to wxWidgets for Qt
   ---------------------------

You have downloaded the Qt port of the wxWidgets GUI library.

It is required to use at least Qt 5 and some features are
only available when using Qt 5.2 or higher.

More info about the wxWidgets project (including all the
other ports and version of wxWidgets) can be found at the
main wxWidgets homepage at:

    https://www.wxwidgets.org/

More information on how to install can be found in the file
install.md in this directory, but this works in a Unix-like
environment, including Cygwin/MSYS2 under Windows:

    mkdir build_qt
    cd build_qt
    ../configure --with-qt
    make
    # Optionally
    su <type root password>
    make install
    ldconfig

Please note that it is strongly advised to build the library in
a separate directory, as shown above, as it allows you to have
different builds of it (for example with and without debug) in
parallel. If you build in the source directory, don't forget to
do "make clean" before changing the configuration!

When you run into problems, please read the install.md and
follow those instructions. If you still don't have any success,
please send a bug report to one of our mailing list, INCLUDING
A DESCRIPTION OF YOUR SYSTEM AND YOUR PROBLEM, SUCH AS YOUR
VERSION OF QT, WXQT, WHAT DISTRIBUTION YOU USE AND WHAT ERROR
WAS REPORTED.

Please send problems concerning installation, feature requests,
bug reports or comments to the wxWidgets users list. These can
be found at https://www.wxwidgets.org/support/mailing-lists/

  Regards,

    The wxWidgets Team
