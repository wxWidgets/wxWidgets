
  Welcome to wxWindows for GTK 1.0,
  
you have downloaded the first beta version of the upcoming
release 2.0 of the GTK+ 1.0 port of wxWindows library.
 
Information on how to install can be found in the file 
INSTALL.txt, but if you cannot wait, this should work on
all systems

configure
make
su (PASSWORD)
make install
ldconfig

When you run into problems, please read the INSTALL.txt and
follow those instructions. If you still don't have any success,
please send a bug report to one of our mailing lists (see
my homepage) INCLUDING A DESCRIPTION OF YOUR SYSTEM AND 
YOUR PROBLEM, SUCH AS YOUR VERSION OF GTK, WXGTK, WHAT
DISTRIBUTION YOU USE AND WHAT ERROR WAS REPORTED. I know 
this has no effect, but I tried...

This is the beta release which means that we have a feature
freeze. We'll move up to version 2.0 rather soon and from then
on there will be no more binary incompatible changes. We might
add more classes, but none that would alter the behaviour of
the existing ones. The library produced by the install process
of the final version will be called libwx_gtk_1_0.a (static)
and libwx_gtk_1_0.so.2.0.0 (shared) so that once a version
of wxWindows/Gtk for GTK 1.2 comes out we'll change the name
of the library to avoid linking problems.

More information is available from my homepage at

  http://wesley.informatik.uni-freiburg.de/~wxxt
  
Please send problems concerning installation, feature requests, 
bug reports or comments to either the wxGTK mailing list or to
the wxWindows developers list. Information on how to subscribe
is available from my homepage.

wxWindows/Gtk doesn't come with any guarantee whatsoever. It might 
crash your harddisk or destroy your monitor. It doesn't claim to be
suitable for any special purpose.

  Regards,

    Robert Roebling



