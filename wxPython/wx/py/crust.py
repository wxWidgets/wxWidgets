"""Crust combines the shell and filling into one control."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import wx

import os
import pprint
import sys

import dispatcher
import editwindow
from filling import Filling
import frame
from shell import Shell
from version import VERSION


class Crust(wx.SplitterWindow):
    """Crust based on SplitterWindow."""

    name = 'Crust'
    revision = __revision__

    def __init__(self, parent, id=-1, pos=wx.DefaultPosition, 
                 size=wx.DefaultSize, style=wx.SP_3D,
                 name='Crust Window', rootObject=None, rootLabel=None,
                 rootIsNamespace=True, intro='', locals=None, 
                 InterpClass=None, *args, **kwds):
        """Create Crust instance."""
        wx.SplitterWindow.__init__(self, parent, id, pos, size, style, name)
        self.shell = Shell(parent=self, introText=intro, 
                           locals=locals, InterpClass=InterpClass, 
                           *args, **kwds)
        self.editor = self.shell
        if rootObject is None:
            rootObject = self.shell.interp.locals
        self.notebook = wx.Notebook(parent=self, id=-1)
        self.shell.interp.locals['notebook'] = self.notebook
        self.filling = Filling(parent=self.notebook, 
                               rootObject=rootObject, 
                               rootLabel=rootLabel, 
                               rootIsNamespace=rootIsNamespace)
        # Add 'filling' to the interpreter's locals.
        self.shell.interp.locals['filling'] = self.filling
        self.notebook.AddPage(page=self.filling, text='Namespace', select=True)
        self.display = Display(parent=self.notebook)
        self.notebook.AddPage(page=self.display, text='Display')
        # Add 'pp' (pretty print) to the interpreter's locals.
        self.shell.interp.locals['pp'] = self.display.setItem
        self.calltip = Calltip(parent=self.notebook)
        self.notebook.AddPage(page=self.calltip, text='Calltip')
        self.sessionlisting = SessionListing(parent=self.notebook)
        self.notebook.AddPage(page=self.sessionlisting, text='Session')
        self.dispatcherlisting = DispatcherListing(parent=self.notebook)
        self.notebook.AddPage(page=self.dispatcherlisting, text='Dispatcher')
##         from wxd import wx_
##         self.wxdocs = Filling(parent=self.notebook, 
##                               rootObject=wx_,
##                               rootLabel='wx', 
##                               rootIsNamespace=False,
##                               static=True)
##         self.notebook.AddPage(page=self.wxdocs, text='wxPython Docs')
##         from wxd import stc_
##         self.stcdocs = Filling(parent=self.notebook, 
##                                rootObject=stc_.StyledTextCtrl,
##                                rootLabel='StyledTextCtrl', 
##                                rootIsNamespace=False,
##                                static=True)
##         self.notebook.AddPage(page=self.stcdocs, text='StyledTextCtrl Docs')
        self.SplitHorizontally(self.shell, self.notebook, 300)
        self.SetMinimumPaneSize(1)


class Display(editwindow.EditWindow):
    """STC used to display an object using Pretty Print."""

    def __init__(self, parent, id=-1, pos=wx.DefaultPosition,
                 size=wx.DefaultSize,
                 style=wx.CLIP_CHILDREN | wx.SUNKEN_BORDER,
                 static=False):
        """Create Display instance."""
        editwindow.EditWindow.__init__(self, parent, id, pos, size, style)
        # Configure various defaults and user preferences.
        self.SetReadOnly(True)
        self.SetWrapMode(False)
        if not static:
            dispatcher.connect(receiver=self.push, signal='Interpreter.push')

    def push(self, command, more):
        """Receiver for Interpreter.push signal."""
        self.Refresh()

    def Refresh(self):
        if not hasattr(self, "item"):
            return
        self.SetReadOnly(False)
        text = pprint.pformat(self.item)
        self.SetText(text)
        self.SetReadOnly(True)

    def setItem(self, item):
        """Set item to pretty print in the notebook Display tab."""
        self.item = item
        self.Refresh()


class Calltip(wx.TextCtrl):
    """Text control containing the most recent shell calltip."""

    def __init__(self, parent=None, id=-1):
        style = (wx.TE_MULTILINE | wx.TE_READONLY | wx.TE_RICH2)
        wx.TextCtrl.__init__(self, parent, id, style=style)
        self.SetBackgroundColour(wx.Colour(255, 255, 232))
        dispatcher.connect(receiver=self.display, signal='Shell.calltip')

    def display(self, calltip):
        """Receiver for Shell.calltip signal."""
        ## self.SetValue(calltip)  # Caused refresh problem on Windows.
        self.Clear()
        self.AppendText(calltip)


class SessionListing(wx.TextCtrl):
    """Text control containing all commands for session."""

    def __init__(self, parent=None, id=-1):
        style = (wx.TE_MULTILINE | wx.TE_READONLY |
                 wx.TE_RICH2 | wx.TE_DONTWRAP)
        wx.TextCtrl.__init__(self, parent, id, style=style)
        dispatcher.connect(receiver=self.push, signal='Interpreter.push')

    def push(self, command, more):
        """Receiver for Interpreter.push signal."""
        if command and not more:
            self.SetInsertionPointEnd()
            start, end = self.GetSelection()
            if start != end:
                self.SetSelection(0, 0)
            self.AppendText(command + '\n')


class DispatcherListing(wx.TextCtrl):
    """Text control containing all dispatches for session."""

    def __init__(self, parent=None, id=-1):
        style = (wx.TE_MULTILINE | wx.TE_READONLY |
                 wx.TE_RICH2 | wx.TE_DONTWRAP)
        wx.TextCtrl.__init__(self, parent, id, style=style)
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

    name = 'CrustFrame'
    revision = __revision__

    def __init__(self, parent=None, id=-1, title='PyCrust',
                 pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=wx.DEFAULT_FRAME_STYLE,
                 rootObject=None, rootLabel=None, rootIsNamespace=True,
                 locals=None, InterpClass=None, *args, **kwds):
        """Create CrustFrame instance."""
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
               'wxPython Version: %s\n' % wx.VERSION_STRING + \
               'Platform: %s\n' % sys.platform
        dialog = wx.MessageDialog(self, text, title,
                                  wx.OK | wx.ICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()
