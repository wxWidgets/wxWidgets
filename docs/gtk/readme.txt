
  Welcome to wxWindows/Gtk 2.01 (beta 4),

you have downloaded version 2.01 of the GTK+ 1.0 port of 
the wxWindows GUI library.
 
More information is available from my homepage at:

  http://wesley.informatik.uni-freiburg.de/~wxxt
  
and about the wxWindows project as a whole (and the
Windows and Motif ports in particular) can be found
at Julian Smart's homepage at:

  http://web.ukonline.co.uk/julian.smart/wxwin
  
Information on how to install can be found in the file 
INSTALL.txt, but if you cannot wait, this should work on
all systems

./configure
make
make install

Type the following to make the samples

make samples

To start the samples, change into the directory that
corresponds to the sample and your system, e.g on a
linux-gnu machine the minimal sample would get started
from the wxWindows base dir with

./samples/minimal/linux-gnu/minimal

When you run into problems, please read the INSTALL.txt and
follow those instructions. If you still don't have any success,
please send a bug report to one of our mailing lists (see
my homepage) INCLUDING A DESCRIPTION OF YOUR SYSTEM AND 
YOUR PROBLEM, SUCH AS YOUR VERSION OF GTK, WXGTK, WHAT
DISTRIBUTION YOU USE AND WHAT ERROR WAS REPORTED. I know 
this has no effect, but I tried...

The library produced by the install process will be called 
libwx_gtk2.a (static) and libwx_gtk2.so.0.1 (shared) so that 
once a binary incompatible version of wxWindows/Gtk comes out 
we'll augment library version number to avoid linking problems.

Please send problems concerning installation, feature requests, 
bug reports or comments to the wxWindows users Information on 
how to subscribe is available from my homepage.

wxWindows/Gtk doesn't come with any guarantee whatsoever. It might 
crash your harddisk or destroy your monitor. It doesn't claim to be
suitable for any special purpose.

  Regards,

    Robert Roebling



