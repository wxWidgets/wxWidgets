#!/usr/bin/env python
"""PyCrust is a python shell application.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__date__ = "July 1, 2001"
__version__ = "$Revision$"[11:-2]

from wxPython.wx import *

from PyCrustVersion import version
from PyCrustShell import Shell


class Frame(wxFrame):
    """Main window for the PyCrust application."""
    def __init__(self, parent, id, title):
        """Create the main frame object for the application."""
        wxFrame.__init__(self, parent, id, title)
        intro = 'Welcome To PyCrust %s - The Flakiest Python Shell' % version
        self.CreateStatusBar()
        self.SetStatusText(intro)
        self.createMenus()
        # Create the shell, which will create a default editor and
        # a default interpreter.
        self.shell = Shell(editorParent=self, introText=intro)
        # Override the editor so that status messages go to the status bar.
        self.shell.editor.setStatusText = self.SetStatusText

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

        m = self.helpMenu = wxMenu()
        m.AppendSeparator()
        m.Append(wxID_ABOUT, '&About...', 'About PyCrust')

        b = self.menuBar = wxMenuBar()
        b.Append(self.fileMenu, '&File')
        b.Append(self.editMenu, '&Edit')
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

        EVT_UPDATE_UI(self, wxID_UNDO, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_REDO, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_CUT, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_COPY, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_PASTE, self.OnUpdateMenu)
        EVT_UPDATE_UI(self, wxID_CLEAR, self.OnUpdateMenu)

    def OnExit(self, event):
        self.Close(true)

    def OnUndo(self, event):
        self.shell.editor.Undo()

    def OnRedo(self, event):
        self.shell.editor.Redo()

    def OnCut(self, event):
        self.shell.editor.Cut()

    def OnCopy(self, event):
        self.shell.editor.Copy()

    def OnPaste(self, event):
        self.shell.editor.Paste()

    def OnClear(self, event):
        self.shell.editor.Clear()

    def OnSelectAll(self, event):
        self.shell.editor.SelectAll()

    def OnAbout(self, event):
        """Display an About PyCrust window."""
        title = 'About PyCrust'
        text = 'PyCrust %s\n\n' % version + \
               'Yet another Python shell, only flakier.\n\n' + \
               'Half-baked by Patrick K. O\'Brien,\n' + \
               'the other half is still in the oven.'
        dialog = wxMessageDialog(self, text, title, wxOK | wxICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()

    def OnUpdateMenu(self, event):
        """Update menu items based on which should be enabled/disabled."""
        id = event.GetId()
        if id == wxID_UNDO:
            event.Enable(self.shell.editor.CanUndo())
        elif id == wxID_REDO:
            event.Enable(self.shell.editor.CanRedo())
        elif id == wxID_CUT:
            event.Enable(self.shell.editor.CanCut())
        elif id == wxID_COPY:
            event.Enable(self.shell.editor.CanCopy())
        elif id == wxID_PASTE:
            event.Enable(self.shell.editor.CanPaste())
        elif id == wxID_CLEAR:
            event.Enable(self.shell.editor.CanCut())


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
