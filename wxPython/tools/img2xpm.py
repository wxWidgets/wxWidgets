#!/usr/bin/env python
"""
img2xpm.py  -- convert several image formats to XPM

Usage:

    img2xpm.py [options] image_files...

Options:

    -o <dir>       The directory to place the .xmp file(s), defaults to
                   the current directory.

    -m <#rrggbb>   If the original image has a mask or transparency defined
                   it will be used by default.  You can use this option to
                   override the default or provide a new mask by specifying
                   a colour in the image to mark as transparent.

    -n <name>      A filename to write the .xpm data to.  Defaults to the
                   basename of the image file + '.xpm'  This option overrides
                   the -o option.
"""


import sys, os, glob, getopt, string
from wxPython.wx import *

if wxPlatform == "__WXGTK__":
    raise SystemExit, "This tool can not be used on wxGTK until wxGTK can save XPM files."

wxInitAllImageHandlers()


def convert(file, maskClr, outputDir, outputName):
    if string.lower(os.path.splitext(file)[1]) == ".ico":
        icon = wxIcon(file, wxBITMAP_TYPE_ICO)
        img = wxBitmapFromIcon(icon)
    else:
        img = wxBitmap(file, wxBITMAP_TYPE_ANY)

    if not img.Ok():
        return 0, file + " failed to load!"
    else:
        if maskClr:
            om = img.GetMask()
            mask = wxMaskColour(img, maskClr)
            img.SetMask(mask)
            if om is not None:
                om.Destroy()
        if outputName:
            newname = outputName
        else:
            newname = os.path.join(outputDir, os.path.basename(os.path.splitext(file)[0]) + ".xpm")
        if img.SaveFile(newname, wxBITMAP_TYPE_XPM):
            return 1, file + " converted to " + newname
        else:
            img = wxImageFromBitmap(img)
            if img.SaveFile(newname, wxBITMAP_TYPE_XPM):
                return 1, "ok"
            else:
                return 0, file + " failed to save!"



def main(args):
    if not args or ("-h" in args):
        print __doc__
        return

    outputDir = ""
    maskClr = None
    outputName = None

    try:
        opts, fileArgs = getopt.getopt(args, "m:n:o:")
    except getopt.GetoptError:
        print __doc__
        return

    for opt, val in opts:
        if opt == "-m":
            maskClr = val
        elif opt == "-n":
            outputName = val
        elif opt == "-o":
            outputDir = val

    if not fileArgs:
        print __doc__
        return

    for arg in fileArgs:
        for file in glob.glob(arg):
            if not os.path.isfile(file):
                continue
            ok, msg = convert(file, maskClr, outputDir, outputName)
            print msg



if __name__ == "__main__":
    if wxPlatform == "__WXGTK__":
        app = wxPySimpleApp()     # Blech!  the GUI needs initialized before
                                  # bitmaps can be created...
    main(sys.argv[1:])


