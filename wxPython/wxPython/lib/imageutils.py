#----------------------------------------------------------------------
# Name:        wxPython.lib.imageutils
# Purpose:     A collection of functions for simple image manipulations
#
# Author:      Robb Shecter
#
# Created:     7-Nov-2002
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by
# Licence:     wxWindows license
#----------------------------------------------------------------------

from __future__ import nested_scopes


def grayOut(anImage):
    """
    Convert the given image (in place) to a grayed-out
    version, appropriate for a 'disabled' appearance.
    """
    factor = 0.7        # 0 < f < 1.  Higher is grayer.
    if anImage.HasMask():
        maskColor = (anImage.GetMaskRed(), anImage.GetMaskGreen(), anImage.GetMaskBlue())
    else:
        maskColor = None
    data = map(ord, list(anImage.GetData()))

    for i in range(0, len(data), 3):
        pixel = (data[i], data[i+1], data[i+2])
        pixel = makeGray(pixel, factor, maskColor)
        for x in range(3):
            data[i+x] = pixel[x]
    anImage.SetData(''.join(map(chr, data)))


def makeGray((r,g,b), factor, maskColor):
    """
    Make a pixel grayed-out. If the pixel
    matches the maskColor, it won't be
    changed.
    """
    if (r,g,b) != maskColor:
        return map(lambda x: int((230 - x) * factor) + x, (r,g,b))
    else:
        return (r,g,b)
