"""PyShellApp is a python shell application."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx
from shell import ShellFrame

True, False = 1, 0


class App(wx.wxApp):
    """PyShell standalone application."""

    def OnInit(self):
        wx.wxInitAllImageHandlers()
        locals = {'__app__': 'PyShell Standalone Application'}
        self.shellFrame = ShellFrame(locals=locals)
        self.shellFrame.SetSize((750, 525))
        self.shellFrame.Show(True)
        self.SetTopWindow(self.shellFrame)
        self.shellFrame.shell.SetFocus()
        # Add the application object to the sys module's namespace.
        # This allows a shell user to do:
        # >>> import sys
        # >>> sys.application.whatever
        import sys
        sys.application = self
        return True


def main():
    application = App(0)
    application.MainLoop()

if __name__ == '__main__':
    main()



