"""PyFilling is a python namespace inspection application."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

# We use this object to get more introspection when run standalone.
app = None

import filling

# These are imported just to have something interesting to inspect.
import crust
import interpreter
import introspect
import pseudo
import shell
import sys
import wx

class App(filling.App):
    def OnInit(self):
        filling.App.OnInit(self)
        self.root = self.fillingFrame.filling.tree.root
        return True

def main():
    """Create and run the application."""
    global app
    app = App(0)
    app.fillingFrame.filling.tree.Expand(app.root)
    app.MainLoop()

if __name__ == '__main__':
    main()
