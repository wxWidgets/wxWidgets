"""PyCrust Crust combines the shell and filling into one control."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__date__ = "July 1, 2001"
__version__ = "$Revision$"[11:-2]

from wxPython.wx import *
from shell import Shell
from filling import Filling
from version import VERSION


class Crust(wxSplitterWindow):
    """PyCrust Crust based on wxSplitterWindow."""
    
    name = 'PyCrust Crust'
    revision = __version__
    
    def __init__(self, parent, id=-1, pos=wxDefaultPosition, \
                 size=wxDefaultSize, style=wxSP_3D, name='Crust Window', \
                 rootObject=None, rootLabel=None, rootIsNamespace=1, \
                 intro='', locals=None, \
                 InterpClass=None, *args, **kwds):
        """Create a PyCrust Crust instance."""
        wxSplitterWindow.__init__(self, parent, id, pos, size, style, name)
        self.shell = Shell(parent=self, introText=intro, \
                           locals=locals, InterpClass=InterpClass, \
                           *args, **kwds)
        self.filling = Filling(parent=self, \
                               rootObject=self.shell.interp.locals, \
                               rootLabel=rootLabel, rootIsNamespace=1)
        """Add 'filling' to the interpreter's locals."""
        self.shell.interp.locals['filling'] = self.filling
        self.SplitHorizontally(self.shell, self.filling, 300)
        self.SetMinimumPaneSize(1)


# Temporary hack to share menus between PyCrust and PyShell.
from shell import ShellMenu

class CrustFrame(wxFrame, ShellMenu):
    """Frame containing all the PyCrust components."""
    
    name = 'PyCrust Frame'
    revision = __version__
    
    def __init__(self, parent=None, id=-1, title='PyCrust', \
                 rootObject=None, rootLabel=None, rootIsNamespace=1, \
                 locals=None, InterpClass=None, *args, **kwds):
        """Create a PyCrust CrustFrame instance."""
        wxFrame.__init__(self, parent, id, title)
        intro = 'Welcome To PyCrust %s - The Flakiest Python Shell' % VERSION
        self.CreateStatusBar()
        self.SetStatusText(intro)
        if wxPlatform == '__WXMSW__':
            icon = wxIcon('PyCrust.ico', wxBITMAP_TYPE_ICO)
            self.SetIcon(icon)
        self.crust = Crust(parent=self, intro=intro, \
                           rootObject=rootObject, \
                           rootLabel=rootLabel, \
                           rootIsNamespace=rootIsNamespace, \
                           locals=locals, \
                           InterpClass=InterpClass, *args, **kwds)
        # Override the filling so that status messages go to the status bar.
        self.crust.filling.fillingTree.setStatusText = self.SetStatusText
        # Override the shell so that status messages go to the status bar.
        self.crust.shell.setStatusText = self.SetStatusText
        # Fix a problem with the sash shrinking to nothing.
        self.crust.filling.SetSashPosition(200)
        # Set focus to the shell editor.
        self.crust.shell.SetFocus()
        # Temporary hack to share menus between PyCrust and PyShell.
        self.shell = self.crust.shell
        self.createMenus()


