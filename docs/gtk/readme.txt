
  Welcome to wxWindows/Gtk 2.1 snapshot 8,

you have downloaded version 2.1 of the GTK+ 1.2 port of 
the wxWindows GUI library. This is a developers release
and is it not suited for production development. Beware
that major changes can happen before a final release.

Beginning with snapshot 6, wxWindows uses a completely
new make file system on Unix that uses GNU automake,
GNU autoconf and GNU autoheader. You do not need these
programs in order to use the library, but for taking
part in its development, they are required. Read the
INSTALL.txt file for learning what you can do with the
new configure/automake/libtool system.

More information is available from my homepage at:

  http://wesley.informatik.uni-freiburg.de/~wxxt
  
and about the wxWindows project as a whole (and the
Windows and Motif ports in particular) can be found
at Julian Smart's homepage at:

  http://web.ukonline.co.uk/julian.smart/wxwin
  
Information on how to install can be found in the file 
INSTALL.txt, but if you cannot wait, this should work on
many systems (when using GTK 1.0 or when not using Linux
read the INSTALL.txt):

./configure
make
su <type root password>
make install
ldconfig
exit

When you run into problems, please read the INSTALL.txt and
follow those instructions. If you still don't have any success,
please send a bug report to one of our mailing lists (see
my homepage) INCLUDING A DESCRIPTION OF YOUR SYSTEM AND 
YOUR PROBLEM, SUCH AS YOUR VERSION OF GTK, WXGTK, WHAT
DISTRIBUTION YOU USE AND WHAT ERROR WAS REPORTED. I know 
this has no effect, but I tried...

The library produced by the install process will be called 
libwx_gtk.a (static) and libwx_gtk-2.1.so.0.0.0 (shared) so that 
once a binary incompatible version of wxWindows/Gtk comes out 
we'll augment the library version number to avoid linking problems.

Please send problems concerning installation, feature requests, 
bug reports or comments to the wxWindows users list. Information 
on how to subscribe is available from my homepage.

wxWindows/Gtk doesn't come with any guarantee whatsoever. It might 
crash your harddisk or destroy your monitor. It doesn't claim to be
suitable for any special or general purpose.

  Regards,

    Robert Roebling



