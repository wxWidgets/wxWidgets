
"""PyFillingApp is a python namespace inspection application."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

# We use this object to get more introspection when run standalone.
application = None

import filling

# These are imported just to have something interesting to inspect.
import crust
import interpreter
import introspect
import pseudo
import shell

import sys
from wxPython import wx

try:
    True
except NameError:
    True = 1==1
    False = 1==0


class App(filling.App):
    def OnInit(self):
        filling.App.OnInit(self)
        root = self.fillingFrame.filling.fillingTree.root
        self.fillingFrame.filling.fillingTree.Expand(root)
        return 1


def main():
    """Create and run the application."""
    global application
    application = App(0)
    application.MainLoop()


if __name__ == '__main__':
    main()
