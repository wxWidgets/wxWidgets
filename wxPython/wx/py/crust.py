"""Crust combines the shell and filling into one control."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import wx

import os
import pprint
import re
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
    sashoffset = 300

    def __init__(self, parent, id=-1, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.SP_3D|wx.SP_LIVE_UPDATE,
                 name='Crust Window', rootObject=None, rootLabel=None,
                 rootIsNamespace=True, intro='', locals=None,
                 InterpClass=None,
                 startupScript=None, execStartupScript=True,
                 *args, **kwds):
        """Create Crust instance."""
        wx.SplitterWindow.__init__(self, parent, id, pos, size, style, name)

        # Turn off the tab-traversal style that is automatically
        # turned on by wx.SplitterWindow.  We do this because on
        # Windows the event for Ctrl-Enter is stolen and used as a
        # navigation key, but the Shell window uses it to insert lines.
        style = self.GetWindowStyle()
        self.SetWindowStyle(style & ~wx.TAB_TRAVERSAL)
        
        self.shell = Shell(parent=self, introText=intro,
                           locals=locals, InterpClass=InterpClass,
                           startupScript=startupScript,
                           execStartupScript=execStartupScript,
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
        self.display.nbTab = self.notebook.GetPageCount()-1
        
        self.calltip = Calltip(parent=self.notebook)
        self.notebook.AddPage(page=self.calltip, text='Calltip')
        
        self.sessionlisting = SessionListing(parent=self.notebook)
        self.notebook.AddPage(page=self.sessionlisting, text='History')
        
        self.dispatcherlisting = DispatcherListing(parent=self.notebook)
        self.notebook.AddPage(page=self.dispatcherlisting, text='Dispatcher')

        
        # Initialize in an unsplit mode, and check later after loading
        # settings if we should split or not.
        self.shell.Hide()
        self.notebook.Hide()
        self.Initialize(self.shell)
        self._shouldsplit = True
        wx.CallAfter(self._CheckShouldSplit)
        self.SetMinimumPaneSize(100)

        self.Bind(wx.EVT_SIZE, self.SplitterOnSize)
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGED, self.OnChanged)
        self.Bind(wx.EVT_SPLITTER_DCLICK, self.OnSashDClick)

    def _CheckShouldSplit(self):
        if self._shouldsplit:
            self.SplitHorizontally(self.shell, self.notebook, -self.sashoffset)
            self.lastsashpos = self.GetSashPosition()
        else:
            self.lastsashpos = -1

    def ToggleTools(self):
        """Toggle the display of the filling and other tools"""
        if self.issplit:
            self.Unsplit()
        else:
            self.SplitHorizontally(self.shell, self.notebook, -self.sashoffset)
            self.lastsashpos = self.GetSashPosition()
        self.issplit = self.IsSplit()

    def ToolsShown(self):
        return self.issplit

    def OnChanged(self, event):
        """update sash offset from the bottom of the window"""
        self.sashoffset = self.GetSize().height - event.GetSashPosition()
        self.lastsashpos = event.GetSashPosition()
        event.Skip()

    def OnSashDClick(self, event):
        self.Unsplit()
        self.issplit = False

    # Make the splitter expand the top window when resized
    def SplitterOnSize(self, event):
        splitter = event.GetEventObject()
        sz = splitter.GetSize()
        splitter.SetSashPosition(sz.height - self.sashoffset, True)
        event.Skip()


    def LoadSettings(self, config):
        self.shell.LoadSettings(config)
        self.filling.LoadSettings(config)

        pos = config.ReadInt('Sash/CrustPos', 400)
        wx.CallAfter(self.SetSashPosition, pos)
        def _updateSashPosValue():
            sz = self.GetSize()
            self.sashoffset = sz.height - self.GetSashPosition()
        wx.CallAfter(_updateSashPosValue)
        zoom = config.ReadInt('View/Zoom/Display', -99)
        if zoom != -99:
            self.display.SetZoom(zoom)
        self.issplit = config.ReadInt('Sash/IsSplit', True)
        if not self.issplit:
            self._shouldsplit = False

    def SaveSettings(self, config):
        self.shell.SaveSettings(config)
        self.filling.SaveSettings(config)

        if self.lastsashpos != -1:
            config.WriteInt('Sash/CrustPos', self.lastsashpos)
        config.WriteInt('Sash/IsSplit', self.issplit)
        config.WriteInt('View/Zoom/Display', self.display.GetZoom())
        

           


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
        if self.GetParent().GetSelection() != self.nbTab:
            focus = wx.Window.FindFocus()
            self.GetParent().SetSelection(self.nbTab)
            wx.CallAfter(focus.SetFocus)
            

# TODO: Switch this to a editwindow.EditWindow
class Calltip(wx.TextCtrl):
    """Text control containing the most recent shell calltip."""

    def __init__(self, parent=None, id=-1):
        style = (wx.TE_MULTILINE | wx.TE_READONLY | wx.TE_RICH2)
        wx.TextCtrl.__init__(self, parent, id, style=style)
        self.SetBackgroundColour(wx.Colour(255, 255, 208))
        dispatcher.connect(receiver=self.display, signal='Shell.calltip')

        df = self.GetFont()
        font = wx.Font(df.GetPointSize(), wx.TELETYPE, wx.NORMAL, wx.NORMAL)
        self.SetFont(font)

    def display(self, calltip):
        """Receiver for Shell.calltip signal."""
        ## self.SetValue(calltip)  # Caused refresh problem on Windows.
        self.Clear()
        self.AppendText(calltip)


# TODO: Switch this to a editwindow.EditWindow
class SessionListing(wx.TextCtrl):
    """Text control containing all commands for session."""

    def __init__(self, parent=None, id=-1):
        style = (wx.TE_MULTILINE | wx.TE_READONLY |
                 wx.TE_RICH2 | wx.TE_DONTWRAP)
        wx.TextCtrl.__init__(self, parent, id, style=style)
        dispatcher.connect(receiver=self.addHistory, signal="Shell.addHistory")
        dispatcher.connect(receiver=self.clearHistory, signal="Shell.clearHistory")
        dispatcher.connect(receiver=self.loadHistory, signal="Shell.loadHistory")

        df = self.GetFont()
        font = wx.Font(df.GetPointSize(), wx.TELETYPE, wx.NORMAL, wx.NORMAL)
        self.SetFont(font)

    def loadHistory(self, history):
        # preload the existing history, if any
        hist = history[:]
        hist.reverse()
        self.SetValue('\n'.join(hist) + '\n')
        self.SetInsertionPointEnd()

    def addHistory(self, command):
        if command:
            self.SetInsertionPointEnd()
            self.AppendText(command + '\n')

    def clearHistory(self):
        self.SetValue("")


class DispatcherListing(wx.TextCtrl):
    """Text control containing all dispatches for session."""

    def __init__(self, parent=None, id=-1):
        style = (wx.TE_MULTILINE | wx.TE_READONLY |
                 wx.TE_RICH2 | wx.TE_DONTWRAP)
        wx.TextCtrl.__init__(self, parent, id, style=style)
        dispatcher.connect(receiver=self.spy)

        df = self.GetFont()
        font = wx.Font(df.GetPointSize(), wx.TELETYPE, wx.NORMAL, wx.NORMAL)
        self.SetFont(font)

    def spy(self, signal, sender):
        """Receiver for Any signal from Any sender."""
        text = '%r from %s' % (signal, sender)
        self.SetInsertionPointEnd()
        start, end = self.GetSelection()
        if start != end:
            self.SetSelection(0, 0)
        self.AppendText(text + '\n')



class CrustFrame(frame.Frame, frame.ShellFrameMixin):
    """Frame containing all the PyCrust components."""

    name = 'CrustFrame'
    revision = __revision__


    def __init__(self, parent=None, id=-1, title='PyCrust',
                 pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=wx.DEFAULT_FRAME_STYLE,
                 rootObject=None, rootLabel=None, rootIsNamespace=True,
                 locals=None, InterpClass=None,
                 config=None, dataDir=None,
                 *args, **kwds):
        """Create CrustFrame instance."""
        frame.Frame.__init__(self, parent, id, title, pos, size, style)
        frame.ShellFrameMixin.__init__(self, config, dataDir)
        
        if size == wx.DefaultSize:
            self.SetSize((800, 600))

        intro = 'PyCrust %s - The Flakiest Python Shell' % VERSION
        self.SetStatusText(intro.replace('\n', ', '))
        self.crust = Crust(parent=self, intro=intro,
                           rootObject=rootObject,
                           rootLabel=rootLabel,
                           rootIsNamespace=rootIsNamespace,
                           locals=locals,
                           InterpClass=InterpClass,
                           startupScript=self.startupScript,
                           execStartupScript=self.execStartupScript,
                           *args, **kwds)
        self.shell = self.crust.shell

        # Override the filling so that status messages go to the status bar.
        self.crust.filling.tree.setStatusText = self.SetStatusText
        
        # Override the shell so that status messages go to the status bar.
        self.shell.setStatusText = self.SetStatusText
        
        self.shell.SetFocus()
        self.LoadSettings()


    def OnClose(self, event):
        """Event handler for closing."""
        self.SaveSettings()
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
               'Platform: %s\n' % sys.platform + \
               'Python Version: %s\n' % sys.version.split()[0] + \
               'wxPython Version: %s\n' % wx.VERSION_STRING + \
               ('\t(%s)\n' % ", ".join(wx.PlatformInfo[1:]))
        dialog = wx.MessageDialog(self, text, title,
                                  wx.OK | wx.ICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()


    def ToggleTools(self):
        """Toggle the display of the filling and other tools"""
        return self.crust.ToggleTools()

    def ToolsShown(self):
        return self.crust.ToolsShown()

    def OnHelp(self, event):
        """Show a help dialog."""
        frame.ShellFrameMixin.OnHelp(self, event)


    def LoadSettings(self):
        if self.config is not None:
            frame.ShellFrameMixin.LoadSettings(self)
            frame.Frame.LoadSettings(self, self.config)
            self.crust.LoadSettings(self.config)


    def SaveSettings(self, force=False):
        if self.config is not None:
            frame.ShellFrameMixin.SaveSettings(self)
            if self.autoSaveSettings or force:
                frame.Frame.SaveSettings(self, self.config)
                self.crust.SaveSettings(self.config)


    def DoSaveSettings(self):
        if self.config is not None:
            self.SaveSettings(force=True)
            self.config.Flush()
        


