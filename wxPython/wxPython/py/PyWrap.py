"""PyWrap is a command line utility that runs a wxPython program with
additional runtime-tools, such as PyCrust."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import os
import sys
from wxPython import wx
from crust import CrustFrame as Frame

try:
    True
except NameError:
    True = 1==1
    False = 1==0


def wrap(app):
    wx.wxInitAllImageHandlers()
    frame = Frame()
    frame.SetSize((750, 525))
    frame.Show(True)
    frame.shell.interp.locals['app'] = app
    app.MainLoop()


def main(argv):
    if len(argv) < 2:
        print "Please specify a module name."
        raise SystemExit
    name = argv[1]
    if name[-3:] == '.py':
        name = name[:-3]
    module = __import__(name)
    # Find the App class.
    App = None
    d = module.__dict__
    for item in d.keys():
        try:
            if issubclass(d[item], wx.wxApp):
                App = d[item]
        except (NameError, TypeError):
            pass
    if App is None:
        print "No App class found."
        raise SystemExit
    app = App()
    wrap(app)


if __name__ == '__main__':
    sys.path.insert(0, os.curdir)
    main(sys.argv)
