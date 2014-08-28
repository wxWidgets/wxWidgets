   Welcome to wxWidgets for Qt
   ---------------------------

You have downloaded the Qt port of the wxWidgets GUI library.

It is required to use at least Qt 5 and some features are
only available when using Qt 5.2 or higher. 

More info about the wxWidgets project (including all the
other ports and version of wxWidgets) can be found at the
main wxWidgets homepage at:

                  http://www.wxwidgets.org/
  
Information on how to install can be found in the file 
INSTALL.txt, but if you cannot wait, this should work on
many systems:

    mkdir build_qt
    cd build_qt
    ../configure --with-qt
    make
    su <type root password>
    make install
    ldconfig

Please note that it is strongly advised to build the library in
a separate directory, as shown above, as it allows you to have
different builds of it (for example with and without debug) in
parallel. If you build in the source directory, don't forget to
do "make clean" before changing the configuration!

When you run into problems, please read the INSTALL.txt and 
follow those instructions. If you still don't have any success,
please send a bug report to one of our mailing list, INCLUDING 
A DESCRIPTION OF YOUR SYSTEM AND YOUR PROBLEM, SUCH AS YOUR 
VERSION OF GTK, WXGTK, WHAT DISTRIBUTION YOU USE AND WHAT ERROR 
WAS REPORTED. I know this has no effect, but I tried...

Please send problems concerning installation, feature requests, 
bug reports or comments to the wxWidgets users list. These can
be found at http://www.wxwidgets.org and more specifically at 
http://lists.wxwidgets.org.

wxWidgets doesn't come with any guarantee whatsoever. It 
might crash your harddisk or destroy your monitor. It doesn't 
claim to be suitable for any special or general purpose.

  Regards,

    The wxWidgets Team

