
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
        self.frame = ShellFrame(locals=locals)
        self.frame.SetSize((750, 525))
        self.frame.Show()
        self.SetTopWindow(self.frame)
        self.frame.shell.SetFocus()
        # Add the application object to the sys module's namespace.
        # This allows a shell user to do:
        # >>> import sys
        # >>> sys.app.whatever
        import sys
        sys.app = self
        return 1

'''
The main() function needs to handle being imported, such as with the
pycrust script that wxPython installs:

    #!/usr/bin/env python

    from wxPython.lib.PyCrust.PyCrustApp import main
    main()
'''

def main():
    import __main__
    md = __main__.__dict__
    keepers = original
    keepers.append('App')
    for key in md.keys():
        if key not in keepers:
            del md[key]
    app = App(0)
    if md.has_key('App') and md['App'] is App:
        del md['App']
    if md.has_key('__main__') and md['__main__'] is __main__:
        del md['__main__']
    app.MainLoop()

if __name__ == '__main__':
    main()
