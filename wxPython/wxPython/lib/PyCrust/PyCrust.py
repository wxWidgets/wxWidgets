#!/usr/bin/env python
"""PyCrust is a python shell application.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__date__ = "July 1, 2001"
__version__ = "$Revision$"[11:-2]

from wxPython.wx import *

from version import VERSION
from shell import Shell

ID_AUTOCOMP = NewId()
ID_AUTOCOMP_SHOW = NewId()
ID_AUTOCOMP_INCLUDE_MAGIC = NewId()
ID_AUTOCOMP_INCLUDE_SINGLE = NewId()
ID_AUTOCOMP_INCLUDE_DOUBLE = NewId()
ID_CALLTIPS = NewId()
ID_CALLTIPS_SHOW = NewId()


class Frame(wxFrame):
    """Main window for the PyCrust application."""
    def __init__(self, parent, id, title):
        """Create the main frame object for the application."""
        wxFrame.__init__(self, parent, id, title)
        intro = 'Welcome To PyCrust %s - The Flakiest Python Shell' % VERSION
        self.CreateStatusBar()
        self.SetStatusText(intro)
        self.icon = wxIcon('PyCrust.ico', wxBITMAP_TYPE_ICO)
        self.SetIcon(self.icon)
        self.createMenus()
        # Create the shell, which will create a default interpreter.
        locals = {'__app__': 'PyCrust Application'}
        self.shell = Shell(parent=self, id=-1, introText=intro, locals=locals)
        # Override the shell so that status messages go to the status bar.
        self.shell.setStatusText = self.SetStatusText

    def createMenus(self):
        m = self.fileMenu = wxMenu()
        m.AppendSeparator()
        m.Append(wxID_EXIT, 'E&xit', 'Exit PyCrust')

        m = self.editMenu = wxMenu()
        m.Append(wxID_UNDO, '&Undo \tCtrl+Z', 'Undo the last action')
        m.Append(wxID_REDO, '&Redo \tCtrl+Y', 'Redo the last undone action')
        m.AppendSeparator()
        m.Append(wxID_CUT, 'Cu&t \tCtrl+X', 'Cut the selection')
        m.Append(wxID_COPY, '&Copy \tCtrl+C', 'Copy the selection')
        m.Append(wxID_PASTE, '&Paste \tCtrl+V', 'Paste')
        m.AppendSeparator()
        m.Append(wxID_CLEAR, 'Cle&ar \tDel', 'Delete the selection')
        m.Append(wxID_SELECTALL, 'Select A&ll \tCtrl+A', 'Select all text')

        m = self.autocompMenu = wxMenu()
        m.Append(ID_AUTOCOMP_SHOW, 'Show Auto Completion', \
                 'Show auto completion during dot syntax', checkable=1)
        m.Append(ID_AUTOCOMP_INCLUDE_MAGIC, 'Include Magic Attributes', \
                 'Include attributes visible to __getattr__ and __setattr__', checkable=1)
        m.Append(ID_AUTOCOMP_INCLUDE_SINGLE, 'Include Single Underscores', \
                 'Include attibutes prefixed by a single underscore', checkable=1)
        m.Append(ID_AUTOCOMP_INCLUDE_DOUBLE, 'Include Double Underscores', \
                 'Include attibutes prefixed by a double underscore', checkable=1)

        m = self.calltipsMenu = wxMenu()
        m.Append(ID_CALLTIPS_SHOW, 'Show Call Tips', \
                 'Show call tips with argument specifications', checkable=1)

        m = self.optionsMenu = wxMenu()
        m.AppendMenu(ID_AUTOCOMP, '&Auto Completion', self.autocompMenu, \
                     'Auto Completion Options')
        m.AppendMenu(ID_CALLTIPS, '&Call Tips', self.calltipsMenu, \
                     'Call Tip Options')

        m = self.helpMenu = wxMenu()
        m.AppendSeparator()
        m.Append(wxID_ABOUT, '&About...', 'About PyCrust')

        b = self.menuBar = wxMenuBar()
        b.Append(self.fileMenu, '&File')
        b.Append(self.editMenu, '&Edit')
        b.Append(self.optionsMenu, '&Options')
        b.Append(self.helpMenu, '&Help')
        self.SetMenuBar(b)

        EVT_MENU(self, wxID_EXIT, self.OnExit)
        EVT_MENU(self, wxID_UNDO, self.OnUndo)
        EVT_MENU(self, wxID_REDO, self.OnRedo)
        EVT_MENU(self, wxID_CUT, self.OnCut)
        EVT_MENU(self, wxID_COPY, self.OnCopy)
        EVT_MENU(self, wxID_PASTE, self.OnPaste)
        EVT_MENU(self, wxID_CLEAR, self.OnClear)
        EVT_MENU(self, wxID_SELECTALL, self.OnSelectAll)
        EVT_MENU(self, wxID_ABOUT, self.OnAbout)
        EVT_MENU(self, ID_AUTOCOMP_SHOW, self.OnAutoCompleteShow)
        EVT_MENU(self, ID_AUTOCOMP_INCLUDE_MAGIC, self.OnAutoCompleteIncludeMagic)
        EVT_MENU(self, ID_AUTOCOMP_INCLUDE_SINGLE, self.OnAutoCompleteIncludeSingle)
        EVT_MENU(self, ID_AUTOCOMP_INCLUDE_DOUBLE, self.OnAutoCompleteIncludeDouble)
        EVT_MENU(self, ID_CALLTIPS_SHOW, self.OnCallTipsShow)

        EVT_UPDATE_UI(self, wxID_UNDO, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_REDO, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_CUT, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_COPY, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_PASTE, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_CLEAR, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, ID_AUTOCOMP_SHOW, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, ID_AUTOCOMP_INCLUDE_MAGIC, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, ID_AUTOCOMP_INCLUDE_SINGLE, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, ID_AUTOCOMP_INCLUDE_DOUBLE, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, ID_CALLTIPS_SHOW, self.OnUpdateMenu)

    def OnExit(self, event):
        self.Close(true)

    def OnUndo(self, event):
        self.shell.Undo()

    def OnRedo(self, event):
        self.shell.Redo()

    def OnCut(self, event):
        self.shell.Cut()

    def OnCopy(self, event):
        self.shell.Copy()

    def OnPaste(self, event):
        self.shell.Paste()

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
               'Interpreter Revision: %s\n' % self.shell.interp.revision
        dialog = wxMessageDialog(self, text, title, wxOK | wxICON_INFORMATION)
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

    def OnUpdateMenu(self, event):
        """Update menu items based on current status."""
        id = event.GetId()
        if id == wxID_UNDO:
            event.Enable(self.shell.CanUndo())
        elif id == wxID_REDO:
            event.Enable(self.shell.CanRedo())
        elif id == wxID_CUT:
            event.Enable(self.shell.CanCut())
        elif id == wxID_COPY:
            event.Enable(self.shell.CanCopy())
        elif id == wxID_PASTE:
            event.Enable(self.shell.CanPaste())
        elif id == wxID_CLEAR:
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
            

class App(wxApp):
    def OnInit(self):
        parent = None
        id = -1
        title = 'PyCrust'
        self.frame = Frame(parent, id, title)
        self.frame.Show(true)
        self.SetTopWindow(self.frame)
        return true


def main():
    import sys
    application = App(0)
    # Add the application object to the sys module's namespace.
    # This allows a shell user to do:
    # >>> import sys
    # >>> sys.application.whatever
    sys.application = application
    application.MainLoop()

if __name__ == '__main__':
    main()
