#!/usr/bin/env python
"""PyShellApp is a python shell application."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__version__ = "$Revision$"[11:-2]

from wxPython.wx import *
from shell import ShellFrame


class App(wxApp):
    """PyShell standalone application."""

    def OnInit(self):
        locals = {'__app__': 'PyShell Standalone Application'}
        self.shellFrame = ShellFrame(locals=locals, size=(800,600))
        self.shellFrame.Show(true)
        self.SetTopWindow(self.shellFrame)
        # Add the application object to the sys module's namespace.
        # This allows a shell user to do:
        # >>> import sys
        # >>> sys.application.whatever
        import sys
        sys.application = self
        return true


def main():
    application = App(0)
    application.MainLoop()

if __name__ == '__main__':
    main()



