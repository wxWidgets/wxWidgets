#----------------------------------------------------------------------
# Name:        wxPython.tools.img2xpm
# Purpose:     Convert an image to XPM format
#
# Author:      Robin Dunn
#
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

"""
img2xpm.py  -- convert several image formats to XPM

Usage:

    img2xpm.py [options] image_files...

Options:

    -o <dir>       The directory to place the .xpm file(s), defaults to
                   the current directory.

    -m <#rrggbb>   If the original image has a mask or transparency defined
                   it will be used by default.  You can use this option to
                   override the default or provide a new mask by specifying
                   a colour in the image to mark as transparent.

    -n <name>      A filename to write the .xpm data to.  Defaults to the
                   basename of the image file + '.xpm'  This option overrides
                   the -o option.
"""


import sys
import img2img
from wxPython import wx

def main():
    img2img.main(sys.argv[1:], wx.wxBITMAP_TYPE_XPM, ".xpm", __doc__)


if __name__ == '__main__':
    main()



