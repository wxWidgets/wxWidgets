
  Welcome to wxWindows/Motif 2.1.16

You have downloaded version 2.1.16 of the Motif port of
the wxWindows GUI library. Although this is not yet the
final stable release wxMotif 2.2, the current version has
been tested carefully on many systems and has been found
to work better than any other previous version.

This is the last beta release. wxWindows is now in a code
freeze and only bugs will be corrected.

More information about the wxWindows project as a whole
can be found at:

  http://www.wxwindows.org
  
Information on how to install can be found in the file 
install.txt, but if you cannot wait, this should work on
many systems:

./configure --with-motif
make
su <type root password>
make install
ldconfig
exit

Alternatively, a script (makewxmotif) for running configure can
be found in this directory.

When you run into problems, please read the install.txt and
follow those instructions. If you still don't have any success,
please send a bug report to one of our mailing lists (see
the wxWindows homepage) INCLUDING A DESCRIPTION OF YOUR SYSTEM AND
YOUR PROBLEM, SUCH AS YOUR VERSION OF MOTIF, WXMOTIF, WHAT
DISTRIBUTION YOU USE AND WHAT ERROR WAS REPORTED.
Alternatively, you may also use the bug reporting system
linked from the wxWindows web page.

The library produced by the install process will be called 
libwx_motif.a (static) and libwx_motif-2.1.so.16.0.0 (shared) so that
once a binary incompatible version of wxWindows/Motif comes out 
we'll augment the library version number to avoid linking problems.

Please send problems concerning installation, feature requests, 
bug reports or comments to the wxWindows users list. Information 
on how to subscribe is available from www.wxwindows.org.

wxWindows/Motif doesn't come with any guarantee whatsoever. It might 
crash your hard disk or destroy your monitor. It doesn't claim to be
suitable for any special or general purpose.

  Regards,

  The wxWindows team

