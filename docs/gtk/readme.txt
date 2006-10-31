   Welcome to wxWidgets 2.7.2 for GTK+
   -----------------------------------

You have downloaded version 2.7 of the GTK+ port of the 
wxWidgets GUI library.

wxWidgets no longer supports GTK 1.0.x (as did some early
snapshots) and support for GTK 1.2.x will be phased out
over time. Nearly all work is now done on GTK 2.0.x and
quite a few of the enhancements of GTK+ 2.4.x and even
GTK 2.8.x are used.

More info about the wxWidgets project (including all the
other ports and version of wxWidgets) can be found at the
main wxWidgets homepage at:

                  http://www.wxwidgets.org/
  
Information on how to install can be found in the file 
INSTALL.txt, but if you cannot wait, this should work on
many systems:

    mkdir build_gtk
    cd build_gtk
    ../configure
    make
    su <type root password>
    make install
    ldconfig

In order to use wxGTK with GTK 1.2 you need to use --with-gtk=1
configure option. To build the library in Unicode mode (all
strings will be wide strings and so on) you need to configure
with GTK 2.0 and add --enable-unicode.

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

Do NOT send any comments directly to me.

wxWidgets doesn't come with any guarantee whatsoever. It 
might crash your harddisk or destroy your monitor. It doesn't 
claim to be suitable for any special or general purpose.

  Regards,

    Robert Roebling

