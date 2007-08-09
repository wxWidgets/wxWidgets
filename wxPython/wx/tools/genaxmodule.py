"""


When this script is run it will create a .py module (output to the
current directory) containing a class derived from
wx.activex.ActiveXWindow for the progID or CLSID given on the command
line.  By default the class name will be used as the module name as
well, but this is just because I am lazy, not trying to define a
standard or anything.  Feel free to rename the module, I do.

Usage:

    python genax.py CLSID|progID className
    
"""

import wx
import wx.activex
import sys


def main(args):
    if len(args) < 3:
        print __doc__
        sys.exit(1)

    # unfortunatly we need to make an app, frame and an instance of
    # the ActiceX control in order to get the TypeInfo about it...
    app = wx.PySimpleApp()
    f = wx.Frame(None, -1, "")
    clsid = wx.activex.CLSID(args[1])
    axw = wx.activex.ActiveXWindow(f, clsid)

    wx.activex.GernerateAXModule(axw, args[2], '.', verbose=True)

    # Cleanup
    f.Close()
    app.MainLoop()

    
if __name__ == "__main__":
    main(sys.argv)
    
