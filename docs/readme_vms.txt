This distribution contains makefiles for VMS. It is a very preliminary release
and many tests have still to be done.

The compilation was tested with
    -OpenVMS Alpha 7.2-1
    -DECC 6.2
    -Compac C++ 6.2
    -DECWindows 1.2-5
    -Mozilla M14 (for the GTK-stuff)
    
To get everything compiled you'll need to have installed:
    -Bison
    -Flex
    -Sed
  These programs are all on the OpenVMS freeware CD and can be downloaded from
  http://www.openvms.digital.com/freeware/
  At the moment you'll have to hack the descrip.mms files to include the right
  locations of the packages, but i intend to make this better in future.
    -sys$library:libjpeg.olb  (from ftp://ftp.uu.net/graphics/jpeg/ )
    -sys$library:libpng.olb & sys$library:libz.olb (from
          http://www.cdrom.com/pub/png/pngcode.html )
    -sys$library:libtiff.olb (from: ftp://ftp.sgi.com/graphics/tiff/ )

Currently only the MOTIF GUI can be build by typing
  MMS motif
 in the main directory of the distribution.

The GTK GUI is now in Alpha testing. Type
  MMS gtk
 in the main directory of the distribution to get a test. I builds the
MINIMAL_GTK.exe sample only. It does something but I also got it crashing.
 
 
It automatically makes the library in [.lib] and links some of the samples
and utils.

N.B.
 the following patch is needed in the tiff3.4 distribution:

polka-jj) diff TIFFCOMP.H;3 TIFFCOMP.H;1
************
File $DISK2:[JOUKJ.PUBLIC.TIFF.TIFF.LIBTIFF]TIFFCOMP.H;3
   97   #ifndef HAVE_UNISTD_H
   98   #define HAVE_UNISTD_H   1
   99   #endif
  100   #endif
******
File $DISK2:[JOUKJ.PUBLIC.TIFF.TIFF.LIBTIFF]TIFFCOMP.H;1
   97   #define HAVE_UNISTD_H   1
   98   #endif
************
************
File $DISK2:[JOUKJ.PUBLIC.TIFF.TIFF.LIBTIFF]TIFFCOMP.H;3
  108   #if !defined(SEEK_SET) && defined( HAVE_UNISTD_H )
  109   #include <unistd.h>
******
File $DISK2:[JOUKJ.PUBLIC.TIFF.TIFF.LIBTIFF]TIFFCOMP.H;1
  106   #if !defined(SEEK_SET) && HAVE_UNISTD_H
  107   #include <unistd.h>
************

Number of difference sections found: 2
Number of difference records found: 4

DIFFERENCES /IGNORE=()/MERGED=1-
    $DISK2:[JOUKJ.PUBLIC.TIFF.TIFF.LIBTIFF]TIFFCOMP.H;3-
    $DISK2:[JOUKJ.PUBLIC.TIFF.TIFF.LIBTIFF]TIFFCOMP.H;1



Finally :
 I like the idea of Robert Roebling that CD's with classical music should be
send to the authors.


>-----------------------------------------------------------------------------<

  Jouk Jansen
		 
  joukj@hrem.stm.tudelft.nl


  Technische Universiteit Delft        tttttttttt  uu     uu  ddddddd
  Nationaal centrum voor HREM          tttttttttt  uu     uu  dd    dd
  Rotterdamseweg 137                       tt      uu     uu  dd     dd
  2628 AL Delft                            tt      uu     uu  dd     dd
  Nederland                                tt      uu     uu  dd    dd
  tel. 31-15-2781536                       tt       uuuuuuu   ddddddd

>-----------------------------------------------------------------------------<
