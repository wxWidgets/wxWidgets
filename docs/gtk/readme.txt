
   Welcome to wxWidgets/Gtk 2.5

You have downloaded version 2.5 of the GTK port of the 
wxWidgets GUI library.

wxWidgets no longer supports GTK 1.0 (as did some early
snapshots) so that you will need GTK 1.2 when using it.
GTK 1.2.6 or above is recommended although some programs 
will work with GTK 1.2.3 onwards. There is now support
for GTK 2.0.

More info about the wxWidgets project (including the
Windows, X11/Motif and other ports) can be found at the main
wxWidgets homepage at:

                  http://www.wxwidgets.org
  
Information on how to install can be found in the file 
INSTALL.txt, but if you cannot wait, this should work on
many systems:

    mkdir gtk
    cd gtk
    ../configure --with-gtk
    make
    su <type root password>
    make install
    ldconfig

In order to use wxGTK with GTK 2.0 you need to use --enable-gtk2
configure option. To build the library in Unicode mode (all
strings will be wide strings and so on) you need to configure
with GTK 2.0 and add --enable-unicode.

Note that so far, support for GTK 2.0 has only been tested on
Linux with glibc 2.2.


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

The library produced by the install process will be called 
libwx_gtk.a (static, only buil if --disable-shared configure
switch was used or if shared libraries are not supported at all
on your platform which is quite unlikely) and
libwx_gtk-2.2.so.0.0.0 (shared) so that once a binary
incompatible version of wxWidgets/Gtk comes out we'll augment
the library version number to avoid linking problems.

Please send problems concerning installation, feature requests, 
bug reports or comments to the wxWidgets users list. Information 
on how to subscribe is available from my homepage.

Do NOT send any comments directly to me.

wxWidgets/Gtk doesn't come with any guarantee whatsoever. It 
might crash your harddisk or destroy your monitor. It doesn't 
claim to be suitable for any special or general purpose.

  Regards,

    Robert Roebling



