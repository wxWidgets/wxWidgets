#----------------------------------------------------------------------
# Name:        wxPython.tools.helpviewer
# Purpose:     HTML Help viewer
#
# Author:      Robin Dunn
#
# Created:     11-Dec-2002
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

"""
helpviewer.py   --  Displays HTML Help in a wxHtmlHelpController window.

Usage:
    helpviewer [--cache=path] helpfile [helpfile(s)...]

    Where helpfile is the path to either a .hhp file or a .zip file
    which contians a .hhp file.  The .hhp files are the same as those
    used by Microsoft's HTML Help Workshop for creating CHM files.
"""


import sys, os

#---------------------------------------------------------------------------

def main(args=sys.argv):
    if len(args) < 2:
        print __doc__
        return

    args = args[1:]
    cachedir = None
    if args[0][:7] == '--cache':
        cachedir = os.path.expanduser(args[0].split('=')[1])
        args = args[1:]

    if len(args) == 0:
        print __doc__
        return

    import wx
    import wx.html

    app = wx.PySimpleApp()
    #wx.Log.SetActiveTarget(wx.LogStderr())
    wx.Log.SetLogLevel(wx.LOG_Error)

    # Set up the default config so the htmlhelp frame can save its preferences
    app.SetVendorName('wxWindows')
    app.SetAppName('helpviewer')
    cfg = wx.ConfigBase.Get()

    # Add the Zip filesystem
    wx.FileSystem.AddHandler(wx.ZipFSHandler())

    # Create the viewer
    helpctrl = wx.html.HtmlHelpController()
    if cachedir:
        helpctrl.SetTempDir(cachedir)

    # and add the books
    for helpfile in args:
        print "Adding %s..." % helpfile
        helpctrl.AddBook(helpfile, 1)

    # start it up!
    helpctrl.DisplayContents()
    app.MainLoop()


if __name__ == '__main__':
    main()


