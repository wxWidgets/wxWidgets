
"""PyShellApp is a python shell application."""

# The next two lines, and the other code below that makes use of
# ``__main__`` and ``original``, serve the purpose of cleaning up the
# main namespace to look as much as possible like the regular Python
# shell environment.
import __main__
original = __main__.__dict__.keys()

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx

try:
    True
except NameError:
    True = 1==1
    False = 1==0


class App(wx.wxApp):
    """PyShell standalone application."""

    def OnInit(self):
        from wxPython import wx
        wx.wxInitAllImageHandlers()
        locals = __main__.__dict__
        from shell import ShellFrame
        self.shellFrame = ShellFrame(locals=locals)
        self.shellFrame.SetSize((750, 525))
        self.shellFrame.Show()
        self.SetTopWindow(self.shellFrame)
        self.shellFrame.shell.SetFocus()
        # Add the application object to the sys module's namespace.
        # This allows a shell user to do:
        # >>> import sys
        # >>> sys.application.whatever
        import sys
        sys.application = self
        return 1


def main():
    locals = __main__.__dict__
    keepers = original
    keepers.append('App')
    for key in locals.keys():
        if key not in keepers:
            del locals[key]
    application = App(0)
    if locals.has_key('App') and locals['App'] is App:
        del locals['App']
    if locals.has_key('__main__') and locals['__main__'] is __main__:
        del locals['__main__']
    application.MainLoop()

if __name__ == '__main__':
    main()
