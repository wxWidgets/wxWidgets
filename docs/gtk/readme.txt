
   Welcome to wxWindows/Gtk 2.1.13

you have downloaded version 2.1 of the GTK+ 1.2 port of 
the wxWindows GUI library. Although this is not yet the
final stable release wxGTK 2.2, the current version has
been tested carefully on many systems and has been found
to work better than any other previous version.

Nonetheless, beware that major changes can happen before 
a final release.

wxWindows no longer supports GTK 1.0 (as did some early
snapshots) so that you will need GTK 1.2 when using it.

Beginning with snapshot 9, wxWindows uses a completely
new make file system on Unix which no longer uses the
various GNU tools and I hope that I'll never again have
to write any more makefiles.

More information is available from my homepage at:

  http://wesley.informatik.uni-freiburg.de/~wxxt
  
and about the wxWindows project as a whole (and the MSW
and Motif ports in particular) can be found at Julian's 
homepage at:

  http://web.ukonline.co.uk/julian.smart/wxwin
  
Information on how to install can be found in the file 
INSTALL.txt, but if you cannot wait, this should work on
many systems:

./configure --with-gtk
make
su <type root password>
make install
ldconfig
exit

When you run into problems, please read the INSTALL.txt and 
follow those instructions. If you still don't have any success,
please send a bug report to one of our mailing list, INCLUDING 
A DESCRIPTION OF YOUR SYSTEM AND YOUR PROBLEM, SUCH AS YOUR 
VERSION OF GTK, WXGTK, WHAT DISTRIBUTION YOU USE AND WHAT ERROR 
WAS REPORTED. I know this has no effect, but I tried...

The library produced by the install process will be called 
libwx_gtk.a (static) and libwx_gtk-2.1.so.13.0.0 (shared) so 
that once a binary incompatible version of wxWindows/Gtk comes 
out we'll augment the library version number to avoid linking 
problems.

Please send problems concerning installation, feature requests, 
bug reports or comments to the wxWindows users list. Information 
on how to subscribe is available from my homepage.

wxWindows/Gtk doesn't come with any guarantee whatsoever. It 
might crash your harddisk or destroy your monitor. It doesn't 
claim to be suitable for any special or general purpose.

  Regards,

    Robert Roebling



