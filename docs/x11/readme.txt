
  Welcome to wxWindows/X11 2.3.2

You have downloaded version 2.3.2 of the X11 port of
the wxWindows GUI library. This runs on X11 with no
Motif, Xt, GTK+ or any other standard widget set --
instead it uses the wxUniversal widgets. The intention
is to have it run on NanoX as well as desktop X11.

More information about the wxWindows project as a whole
can be found at:

  http://www.wxwindows.org
  
Information on how to install can be found in the file 
install.txt, but if you cannot wait, this should work on
many systems:

./configure --with-x11 --with-universal
make
su <type root password>
make install
ldconfig
exit

When you run into problems, please read the install.txt and
follow those instructions. If you still don't have any success,
please send a bug report to one of our mailing lists (see
the wxWindows homepage) INCLUDING A DESCRIPTION OF YOUR SYSTEM AND
YOUR PROBLEM, SUCH AS YOUR VERSION OF MOTIF, WXMOTIF, WHAT
DISTRIBUTION YOU USE AND WHAT ERROR WAS REPORTED.
Alternatively, you may also use the bug reporting system
linked from the wxWindows web page.

The library produced by the install process will be called 
libwx_x11.a (static) and libwx_x11-2.3.so.0.0.0 (shared) so that
once a binary incompatible version of wxWindows/X11 comes out 
we'll augment the library version number to avoid linking problems.

Please send problems concerning installation, feature requests, 
bug reports or comments to the wxWindows users list. Information 
on how to subscribe is available from www.wxwindows.org.

wxWindows/X11 doesn't come with any guarantee whatsoever. It might 
crash your hard disk or destroy your monitor. It doesn't claim to be
suitable for any special or general purpose.

Status
======

This is new port and doesn't yet compile, but do please join in
and help. It's actually quite a simple port since most of the hard work
is done by the wxUniversal widgets.

  Regards,

  The wxWindows team

