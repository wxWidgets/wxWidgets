#!/usr/bin/env python
"""
img2png.py  -- convert several image formats to PNG format

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


import sys
import img2img
from wxPython import wx

img2img.main(sys.argv[1:], wx.wxBITMAP_TYPE_PNG, ".png", __doc__)


