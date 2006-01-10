#!/usr/bin/env python
#---------------------------------------------------------------------------

import sys, os, glob
import wx
from wx.tools import helpviewer


# Figure out the path where this app is located
if __name__ == '__main__':
    basePath = os.path.dirname(sys.argv[0])
else:
    basePath = os.path.dirname(__file__)
if not basePath:
    basePath = '.'


# test for write access
if os.access(basePath, os.W_OK):

    # setup the args
    args = ['',
        '--cache='+basePath,
        os.path.join(basePath, 'wx.zip'),
        ]

    # add any other .zip files found
    for file in glob.glob(os.path.join(basePath, "*.zip")):
        if file not in args:
            args.append(file)

    # launch helpviewer
    helpviewer.main(args)

else:
    app = wx.PySimpleApp()
    dlg = wx.MessageDialog(None,
        "The wxDocs need to be located in a directory that is writable by you.  "
        "I am unable to start the viewer in its current location.",
                           "Error!", wx.OK|wx.ICON_EXCLAMATION)
    dlg.ShowModal()
    dlg.Destroy()
    app.MainLoop()

#---------------------------------------------------------------------------

