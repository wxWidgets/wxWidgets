"""PyCrust Crust combines the shell and filling into one control."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx

import os
import sys

import dispatcher
from filling import Filling
import frame
from shell import Shell
from version import VERSION

try:
    True
except NameError:
    True = 1==1
    False = 1==0


class Crust(wx.wxSplitterWindow):
    """PyCrust Crust based on wxSplitterWindow."""

    name = 'PyCrust Crust'
    revision = __revision__

    def __init__(self, parent, id=-1, pos=wx.wxDefaultPosition, 
                 size=wx.wxDefaultSize, style=wx.wxSP_3D,
                 name='Crust Window', rootObject=None, rootLabel=None,
                 rootIsNamespace=True, intro='', locals=None, 
                 InterpClass=None, *args, **kwds):
        """Create a PyCrust Crust instance."""
        wx.wxSplitterWindow.__init__(self, parent, id, pos, size, style, name)
        self.shell = Shell(parent=self, introText=intro, 
                           locals=locals, InterpClass=InterpClass, 
                           *args, **kwds)
        self.editor = self.shell
        if rootObject is None:
            rootObject = self.shell.interp.locals
        self.notebook = wx.wxNotebook(parent=self, id=-1)
        self.shell.interp.locals['notebook'] = self.notebook
        self.filling = Filling(parent=self.notebook, 
                               rootObject=rootObject, 
                               rootLabel=rootLabel, 
                               rootIsNamespace=rootIsNamespace)
        # Add 'filling' to the interpreter's locals.
        self.shell.interp.locals['filling'] = self.filling
        self.notebook.AddPage(page=self.filling, text='Namespace', select=True)
        self.calltip = Calltip(parent=self.notebook)
        self.notebook.AddPage(page=self.calltip, text='Calltip')
        self.sessionlisting = SessionListing(parent=self.notebook)
        self.notebook.AddPage(page=self.sessionlisting, text='Session')
        self.dispatcherlisting = DispatcherListing(parent=self.notebook)
        self.notebook.AddPage(page=self.dispatcherlisting, text='Dispatcher')
        from wxd import wx_
        self.wxdocs = Filling(parent=self.notebook, 
                              rootObject=wx_,
                              rootLabel='wx', 
                              rootIsNamespace=False,
                              static=True)
        self.notebook.AddPage(page=self.wxdocs, text='wxPython Docs')
        from wxd import stc_
        self.stcdocs = Filling(parent=self.notebook, 
                               rootObject=stc_.StyledTextCtrl,
                               rootLabel='StyledTextCtrl', 
                               rootIsNamespace=False,
                               static=True)
        self.notebook.AddPage(page=self.stcdocs, text='StyledTextCtrl Docs')
        self.SplitHorizontally(self.shell, self.notebook, 300)
        self.SetMinimumPaneSize(1)


class Calltip(wx.wxTextCtrl):
    """Text control containing the most recent shell calltip."""

    def __init__(self, parent=None, id=-1):
        style = wx.wxTE_MULTILINE | wx.wxTE_READONLY | wx.wxTE_RICH2
        wx.wxTextCtrl.__init__(self, parent=parent, id=id, style=style)
        self.SetBackgroundColour(wx.wxColour(255, 255, 232))
        dispatcher.connect(receiver=self.display, signal='Shell.calltip')

    def display(self, calltip):
        """Receiver for Shell.calltip signal."""
        self.SetValue(calltip)


class SessionListing(wx.wxTextCtrl):
    """Text control containing all commands for session."""

    def __init__(self, parent=None, id=-1):
        style = wx.wxTE_MULTILINE | wx.wxTE_READONLY | \
                wx.wxTE_RICH2 | wx.wxTE_DONTWRAP
        wx.wxTextCtrl.__init__(self, parent=parent, id=id, style=style)
        dispatcher.connect(receiver=self.push, signal='Interpreter.push')

    def push(self, command, more):
        """Receiver for Interpreter.push signal."""
        if command and not more:
            self.SetInsertionPointEnd()
            start, end = self.GetSelection()
            if start != end:
                self.SetSelection(0, 0)
            self.AppendText(command + '\n')


class DispatcherListing(wx.wxTextCtrl):
    """Text control containing all dispatches for session."""

    def __init__(self, parent=None, id=-1):
        style = wx.wxTE_MULTILINE | wx.wxTE_READONLY | \
                wx.wxTE_RICH2 | wx.wxTE_DONTWRAP
        wx.wxTextCtrl.__init__(self, parent=parent, id=id, style=style)
        dispatcher.connect(receiver=self.spy)

    def spy(self, signal, sender):
        """Receiver for Any signal from Any sender."""
        text = '%r from %s' % (signal, sender)
        self.SetInsertionPointEnd()
        start, end = self.GetSelection()
        if start != end:
            self.SetSelection(0, 0)
        self.AppendText(text + '\n')


class CrustFrame(frame.Frame):
    """Frame containing all the PyCrust components."""

    name = 'PyCrust Frame'
    revision = __revision__

    def __init__(self, parent=None, id=-1, title='PyCrust',
                 pos=wx.wxDefaultPosition, size=wx.wxDefaultSize,
                 style=wx.wxDEFAULT_FRAME_STYLE,
                 rootObject=None, rootLabel=None, rootIsNamespace=True,
                 locals=None, InterpClass=None, *args, **kwds):
        """Create a PyCrust CrustFrame instance."""
        frame.Frame.__init__(self, parent, id, title, pos, size, style)
        intro = 'PyCrust %s - The Flakiest Python Shell' % VERSION
        intro += '\nSponsored by Orbtech - '
        intro += 'Your source for Python programming expertise.'
        self.SetStatusText(intro.replace('\n', ', '))
        self.crust = Crust(parent=self, intro=intro,
                           rootObject=rootObject,
                           rootLabel=rootLabel,
                           rootIsNamespace=rootIsNamespace,
                           locals=locals,
                           InterpClass=InterpClass, *args, **kwds)
        self.shell = self.crust.shell
        # Override the filling so that status messages go to the status bar.
        self.crust.filling.tree.setStatusText = self.SetStatusText
        # Override the shell so that status messages go to the status bar.
        self.shell.setStatusText = self.SetStatusText
        # Fix a problem with the sash shrinking to nothing.
        self.crust.filling.SetSashPosition(200)
        # Set focus to the shell editor.
        self.shell.SetFocus()

    def OnClose(self, event):
        """Event handler for closing."""
        self.crust.shell.destroy()
        self.Destroy()

    def OnAbout(self, event):
        """Display an About window."""
        title = 'About PyCrust'
        text = 'PyCrust %s\n\n' % VERSION + \
               'Yet another Python shell, only flakier.\n\n' + \
               'Half-baked by Patrick K. O\'Brien,\n' + \
               'the other half is still in the oven.\n\n' + \
               'Shell Revision: %s\n' % self.shell.revision + \
               'Interpreter Revision: %s\n\n' % self.shell.interp.revision + \
               'Python Version: %s\n' % sys.version.split()[0] + \
               'wxPython Version: %s\n' % wx.__version__ + \
               'Platform: %s\n' % sys.platform
        dialog = wx.wxMessageDialog(self, text, title,
                                    wx.wxOK | wx.wxICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()
