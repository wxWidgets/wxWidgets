"""PyShell is a python shell application."""

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
        return True

'''
The main() function needs to handle being imported, such as with the
pyshell script that wxPython installs:

    #!/usr/bin/env python

    from wx.py.PyShell import main
    main()
'''

def main():
    """The main function for the PyShell program."""
    # Cleanup the main namespace, leaving the App class.
    import __main__
    md = __main__.__dict__
    keepers = original
    keepers.append('App')
    for key in md.keys():
        if key not in keepers:
            del md[key]
    # Create an application instance.
    app = App(0)
    # Cleanup the main namespace some more.
    if md.has_key('App') and md['App'] is App:
        del md['App']
    if md.has_key('__main__') and md['__main__'] is __main__:
        del md['__main__']
    # Mimic the contents of the standard Python shell's sys.path.
    import sys
    if sys.path[0]:
        sys.path[0] = ''
    # Add the application object to the sys module's namespace.
    # This allows a shell user to do:
    # >>> import sys
    # >>> sys.app.whatever
    sys.app = app
    del sys
    # Start the wxPython event loop.
    app.MainLoop()

if __name__ == '__main__':
    main()
