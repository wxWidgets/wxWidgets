"""Shell menu mixin shared by shell and crust."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx
import sys
from version import VERSION

try:
    True
except NameError:
    True = 1==1
    False = 1==0

ID_AUTOCOMP = wx.wxNewId()
ID_AUTOCOMP_SHOW = wx.wxNewId()
ID_AUTOCOMP_INCLUDE_MAGIC = wx.wxNewId()
ID_AUTOCOMP_INCLUDE_SINGLE = wx.wxNewId()
ID_AUTOCOMP_INCLUDE_DOUBLE = wx.wxNewId()
ID_CALLTIPS = wx.wxNewId()
ID_CALLTIPS_SHOW = wx.wxNewId()
ID_COPY_PLUS = wx.wxNewId()
ID_PASTE_PLUS = wx.wxNewId()
ID_WRAP = wx.wxNewId()


class ShellMenu:
    """Mixin class to add standard menu items."""

    def createMenus(self):
        m = self.fileMenu = wx.wxMenu()
        m.AppendSeparator()
        m.Append(wx.wxID_EXIT, 'E&xit', 'Exit PyCrust')

        m = self.editMenu = wx.wxMenu()
        m.Append(wx.wxID_UNDO, '&Undo \tCtrl+Z',
                 'Undo the last action')
        m.Append(wx.wxID_REDO, '&Redo \tCtrl+Y',
                 'Redo the last undone action')
        m.AppendSeparator()
        m.Append(wx.wxID_CUT, 'Cu&t \tCtrl+X',
                 'Cut the selection')
        m.Append(wx.wxID_COPY, '&Copy \tCtrl+C',
                 'Copy the selection - removing prompts')
        m.Append(ID_COPY_PLUS, 'Cop&y Plus \tCtrl+Shift+C',
                 'Copy the selection - retaining prompts')
        m.Append(wx.wxID_PASTE, '&Paste \tCtrl+V', 'Paste')
        m.Append(ID_PASTE_PLUS, 'Past&e Plus \tCtrl+Shift+V',
                 'Paste and run commands')
        m.AppendSeparator()
        m.Append(wx.wxID_CLEAR, 'Cle&ar',
                 'Delete the selection')
        m.Append(wx.wxID_SELECTALL, 'Select A&ll \tCtrl+A',
                 'Select all text')

        m = self.autocompMenu = wx.wxMenu()
        m.Append(ID_AUTOCOMP_SHOW, 'Show Auto Completion',
                 'Show auto completion during dot syntax', 1)
        m.Append(ID_AUTOCOMP_INCLUDE_MAGIC, 'Include Magic Attributes',
                 'Include attributes visible to __getattr__ and __setattr__',
                 1)
        m.Append(ID_AUTOCOMP_INCLUDE_SINGLE, 'Include Single Underscores',
                 'Include attibutes prefixed by a single underscore', 1)
        m.Append(ID_AUTOCOMP_INCLUDE_DOUBLE, 'Include Double Underscores',
                 'Include attibutes prefixed by a double underscore', 1)

        m = self.calltipsMenu = wx.wxMenu()
        m.Append(ID_CALLTIPS_SHOW, 'Show Call Tips',
                 'Show call tips with argument specifications', 1)

        m = self.optionsMenu = wx.wxMenu()
        m.AppendMenu(ID_AUTOCOMP, '&Auto Completion', self.autocompMenu,
                     'Auto Completion Options')
        m.AppendMenu(ID_CALLTIPS, '&Call Tips', self.calltipsMenu,
                     'Call Tip Options')
        m.Append(ID_WRAP, '&Wrap Lines',
                 'Wrap lines at right edge', 1)

        m = self.helpMenu = wx.wxMenu()
        m.AppendSeparator()
        m.Append(wx.wxID_ABOUT, '&About...', 'About PyCrust')

        b = self.menuBar = wx.wxMenuBar()
        b.Append(self.fileMenu, '&File')
        b.Append(self.editMenu, '&Edit')
        b.Append(self.optionsMenu, '&Options')
        b.Append(self.helpMenu, '&Help')
        self.SetMenuBar(b)

        wx.EVT_MENU(self, wx.wxID_EXIT, self.OnExit)
        wx.EVT_MENU(self, wx.wxID_UNDO, self.OnUndo)
        wx.EVT_MENU(self, wx.wxID_REDO, self.OnRedo)
        wx.EVT_MENU(self, wx.wxID_CUT, self.OnCut)
        wx.EVT_MENU(self, wx.wxID_COPY, self.OnCopy)
        wx.EVT_MENU(self, ID_COPY_PLUS, self.OnCopyPlus)
        wx.EVT_MENU(self, wx.wxID_PASTE, self.OnPaste)
        wx.EVT_MENU(self, ID_PASTE_PLUS, self.OnPastePlus)
        wx.EVT_MENU(self, wx.wxID_CLEAR, self.OnClear)
        wx.EVT_MENU(self, wx.wxID_SELECTALL, self.OnSelectAll)
        wx.EVT_MENU(self, wx.wxID_ABOUT, self.OnAbout)
        wx.EVT_MENU(self, ID_AUTOCOMP_SHOW,
                    self.OnAutoCompleteShow)
        wx.EVT_MENU(self, ID_AUTOCOMP_INCLUDE_MAGIC,
                    self.OnAutoCompleteIncludeMagic)
        wx.EVT_MENU(self, ID_AUTOCOMP_INCLUDE_SINGLE,
                    self.OnAutoCompleteIncludeSingle)
        wx.EVT_MENU(self, ID_AUTOCOMP_INCLUDE_DOUBLE,
                    self.OnAutoCompleteIncludeDouble)
        wx.EVT_MENU(self, ID_CALLTIPS_SHOW,
                    self.OnCallTipsShow)
        wx.EVT_MENU(self, ID_WRAP, self.OnWrap)

        wx.EVT_UPDATE_UI(self, wx.wxID_UNDO, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.wxID_REDO, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.wxID_CUT, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.wxID_COPY, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, ID_COPY_PLUS, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.wxID_PASTE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, ID_PASTE_PLUS, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.wxID_CLEAR, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, ID_AUTOCOMP_SHOW, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, ID_AUTOCOMP_INCLUDE_MAGIC, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, ID_AUTOCOMP_INCLUDE_SINGLE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, ID_AUTOCOMP_INCLUDE_DOUBLE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, ID_CALLTIPS_SHOW, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, ID_WRAP, self.OnUpdateMenu)

    def OnExit(self, event):
        self.Close(True)

    def OnUndo(self, event):
        self.shell.Undo()

    def OnRedo(self, event):
        self.shell.Redo()

    def OnCut(self, event):
        self.shell.Cut()

    def OnCopy(self, event):
        self.shell.Copy()

    def OnCopyPlus(self, event):
        self.shell.CopyWithPrompts()

    def OnPaste(self, event):
        self.shell.Paste()

    def OnPastePlus(self, event):
        self.shell.PasteAndRun()

    def OnClear(self, event):
        self.shell.Clear()

    def OnSelectAll(self, event):
        self.shell.SelectAll()

    def OnAbout(self, event):
        """Display an About PyCrust window."""
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

    def OnAutoCompleteShow(self, event):
        self.shell.autoComplete = event.IsChecked()

    def OnAutoCompleteIncludeMagic(self, event):
        self.shell.autoCompleteIncludeMagic = event.IsChecked()

    def OnAutoCompleteIncludeSingle(self, event):
        self.shell.autoCompleteIncludeSingle = event.IsChecked()

    def OnAutoCompleteIncludeDouble(self, event):
        self.shell.autoCompleteIncludeDouble = event.IsChecked()

    def OnCallTipsShow(self, event):
        self.shell.autoCallTip = event.IsChecked()

    def OnWrap(self, event):
        self.shell.SetWrapMode(event.IsChecked())

    def OnUpdateMenu(self, event):
        """Update menu items based on current status."""
        id = event.GetId()
        if id == wx.wxID_UNDO:
            event.Enable(self.shell.CanUndo())
        elif id == wx.wxID_REDO:
            event.Enable(self.shell.CanRedo())
        elif id == wx.wxID_CUT:
            event.Enable(self.shell.CanCut())
        elif id == wx.wxID_COPY:
            event.Enable(self.shell.CanCopy())
        elif id == ID_COPY_PLUS:
            event.Enable(self.shell.CanCopy())
        elif id == wx.wxID_PASTE:
            event.Enable(self.shell.CanPaste())
        elif id == ID_PASTE_PLUS:
            event.Enable(self.shell.CanPaste())
        elif id == wx.wxID_CLEAR:
            event.Enable(self.shell.CanCut())
        elif id == ID_AUTOCOMP_SHOW:
            event.Check(self.shell.autoComplete)
        elif id == ID_AUTOCOMP_INCLUDE_MAGIC:
            event.Check(self.shell.autoCompleteIncludeMagic)
        elif id == ID_AUTOCOMP_INCLUDE_SINGLE:
            event.Check(self.shell.autoCompleteIncludeSingle)
        elif id == ID_AUTOCOMP_INCLUDE_DOUBLE:
            event.Check(self.shell.autoCompleteIncludeDouble)
        elif id == ID_CALLTIPS_SHOW:
            event.Check(self.shell.autoCallTip)
        elif id == ID_WRAP:
            event.Check(self.shell.GetWrapMode())

