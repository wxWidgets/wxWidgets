#!/usr/bin/env python
"""PyCrust is a python shell and namespace browser application."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__date__ = "July 1, 2001"
__version__ = "$Revision$"[11:-2]

from wxPython.wx import *
from crust import CrustFrame


class App(wxApp):
    """PyCrust standalone application."""
    
    def OnInit(self):
        locals = {'__app__': 'PyCrust Standalone Application'}
        self.crustFrame = CrustFrame(locals=locals)
        self.crustFrame.Show(true)
        # Set focus to the shell editor.
        #self.crustFrame.crust.shell.SetFocus()
        self.SetTopWindow(self.crustFrame)
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

