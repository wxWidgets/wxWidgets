#----------------------------------------------------------------------
# Name:        wxPython.tools.img2png
# Purpose:     Convert an image to PNG format
#
# Author:      Robin Dunn
#
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 12/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o V2.5 compatability update 
#

"""
img2png.py  -- convert several image formats to PNG format.

Usage:

    img2png.py [options] image_files...

Options:

    -o <dir>       The directory to place the .png file(s), defaults to
                   the current directory.

    -m <#rrggbb>   If the original image has a mask or transparency defined
                   it will be used by default.  You can use this option to
                   override the default or provide a new mask by specifying
                   a colour in the image to mark as transparent.

    -n <name>      A filename to write the .png data to.  Defaults to the
                   basename of the image file + '.png'  This option overrides
                   the -o option.
"""


import  sys
import  wx
import  img2img


def main():
    # some bitmap related things need to have a wxApp initialized...
    if wx.GetApp() is None:
        app = wx.PySimpleApp()
    img2img.main(sys.argv[1:], wx.BITMAP_TYPE_PNG, ".png", __doc__)

if __name__ == '__main__':
    main()



