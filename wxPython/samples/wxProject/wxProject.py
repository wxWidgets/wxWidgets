#!/usr/bin/env python

"""
This sample comes from an IBM developerWorks article at
http://www-106.ibm.com/developerworks/library/l-wxpy/index.html

This small program was adapted to demonstrate the current guide lines
on http://wiki.wxpython.org/index.cgi/wxPython_20Style_20Guide.
Changes are noted in readme.txt.
"""

import sys, os
import wx


# Process the command line.  Not much to do;
# just get the name of the project file if it's given. Simple.
projfile = 'Unnamed'
if len(sys.argv) > 1:
    projfile = sys.argv[1]


def MsgDlg(window, string, caption='wxProject', style=wx.YES_NO|wx.CANCEL):
    """Common MessageDialog."""
    dlg = wx.MessageDialog(window, string, caption, style)
    result = dlg.ShowModal()
    dlg.Destroy()
    return result


class main_window(wx.Frame):
    """wxProject MainFrame."""
    def __init__(self, parent, title):
        """Create the wxProject MainFrame."""
        wx.Frame.__init__(self, parent, title=title, size=(500, 500),
                          style=wx.DEFAULT_FRAME_STYLE|wx.NO_FULL_REPAINT_ON_RESIZE)


        # Set up menu bar for the program.
        self.mainmenu = wx.MenuBar()                  # Create menu bar.

        # Make the 'Project' menu.
        menu = wx.Menu()

        item = menu.Append(wx.ID_OPEN, '&Open', 'Open project')  # Append a new menu
        self.Bind(wx.EVT_MENU, self.OnProjectOpen, item)  # Create and assign a menu event.

        item = menu.Append(wx.ID_NEW, '&New', 'New project')
        self.Bind(wx.EVT_MENU, self.OnProjectNew, item)

        item = menu.Append(wx.ID_EXIT, 'E&xit', 'Exit program')
        self.Bind(wx.EVT_MENU, self.OnProjectExit, item)

        self.mainmenu.Append(menu, '&Project')  # Add the project menu to the menu bar.

        # Make the 'File' menu.
        menu = wx.Menu()

        item = menu.Append(wx.ID_ANY, '&Add', 'Add file to project')
        self.Bind(wx.EVT_MENU, self.OnFileAdd, item)

        item = menu.Append(wx.ID_ANY, '&Remove', 'Remove file from project')
        self.Bind(wx.EVT_MENU, self.OnFileRemove, item)

        item = menu.Append(wx.ID_ANY, '&Open', 'Open file for editing')
        self.Bind(wx.EVT_MENU, self.OnFileOpen, item)

        item = menu.Append(wx.ID_ANY, '&Save', 'Save file')
        self.Bind(wx.EVT_MENU, self.OnFileSave, item)

        self.mainmenu.Append(menu, '&File') # Add the file menu to the menu bar.

        # Attach the menu bar to the window.
        self.SetMenuBar(self.mainmenu)

        # Create the splitter window.
        splitter = wx.SplitterWindow(self, style=wx.NO_3D|wx.SP_3D)
        splitter.SetMinimumPaneSize(1)

        # Create the tree on the left.
        self.tree = wx.TreeCtrl(splitter, style=wx.TR_DEFAULT_STYLE)
        self.tree.Bind(wx.EVT_TREE_BEGIN_LABEL_EDIT, self.OnTreeLabelEdit)
        self.tree.Bind(wx.EVT_TREE_END_LABEL_EDIT, self.OnTreeLabelEditEnd)
        self.tree.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.OnTreeItemActivated)

        # Create the editor on the right.
        self.editor = wx.TextCtrl(splitter, style=wx.TE_MULTILINE)
        self.editor.Enable(0)

        # Install the tree and the editor.
        splitter.SplitVertically(self.tree, self.editor)
        splitter.SetSashPosition(180, True)

        # Some global state variables.
        self.projectdirty = False
        self.root = None
        self.close = False

        self.Bind(wx.EVT_CLOSE, self.OnProjectExit)

        self.Show(True)

    # ----------------------------------------------------------------------------------------
    # Some nice little handlers.
    # ----------------------------------------------------------------------------------------

    def project_open(self, project_file):
        """Open and process a wxProject file."""
        try:
            input = open(project_file, 'r')
            self.tree.DeleteAllItems()

            self.project_file = project_file
            name = input.readline().replace ('\n', '')
            self.SetTitle(name)

            # create the file elements in the tree control.
            self.root = self.tree.AddRoot(name)
            self.activeitem = self.root
            for line in input.readlines():
                self.tree.AppendItem(self.root, line.replace ('\n', ''))
            input.close()
            self.tree.Expand(self.root)

            self.editor.Clear()
            self.editor.Enable(False)

            self.projectdirty = False
        except IOError:
            pass

    def project_save(self):
        """Save a wxProject file."""
        try:
            output = open(self.project_file, 'w+')
            output.write(self.tree.GetItemText(self.root) + '\n')
            count = self.tree.GetChildrenCount(self.root)  # collect all file (tree) items.
            iter = 0
            child = ''
            for i in range(count):
               if i == 0:
                  child, cookie = self.tree.GetFirstChild(self.root)
               else:
                  child, cookie = self.tree.GetNextChild(self.root, cookie)
               output.write(self.tree.GetItemText(child) + '\n')
            output.close()
            self.projectdirty = False
        except IOError:
            MsgDlg(self, 'There was an error saving the new project file.', 'Error!', wx.OK)

    def CheckProjectDirty(self):
        """Were the current project changed? If so, save it before."""
        open_it = True
        if self.projectdirty:
            # save the current project file first.
            result = MsgDlg(self, 'The project has been changed.  Save?')
            if result == wx.ID_YES:
                self.project_save()
            if result == wx.ID_CANCEL:
                open_it = False
        return open_it

    def CheckTreeRootItem(self):
        """Is there any root item?"""
        if not self.root:
            MsgDlg(self, 'Please create or open a project before.', 'Error!', wx.OK)
            return False
        return True

    # ----------------------------------------------------------------------------------------
    # Event handlers from here on out.
    # ----------------------------------------------------------------------------------------

    def OnProjectOpen(self, event):
        """Open a wxProject file."""
        open_it = self.CheckProjectDirty()
        if open_it:
            dlg = wx.FileDialog(self, 'Choose a project to open', '.', '', '*.wxp', wx.OPEN)
            if dlg.ShowModal() == wx.ID_OK:
                self.project_open(dlg.GetPath())
            dlg.Destroy()

    def OnProjectNew(self, event):
        """Create a new wxProject."""
        open_it = self.CheckProjectDirty()
        if open_it:
            dlg = wx.TextEntryDialog(self, 'Name for new project:', 'New Project',
                                     'New project', wx.OK|wx.CANCEL)
            if dlg.ShowModal() == wx.ID_OK:
                newproj = dlg.GetValue()
                dlg.Destroy()
                dlg = wx.FileDialog(self, 'Place to store new project.', '.', '', '*.wxp', wx.SAVE)
                if dlg.ShowModal() == wx.ID_OK:
                    try:
                        # save the project file.
                        proj = open(dlg.GetPath(), 'w')
                        proj.write(newproj + '\n')
                        proj.close()
                        self.project_open(dlg.GetPath())
                    except IOError:
                        MsgDlg(self, 'There was an error saving the new project file.', 'Error!', wx.OK)
            dlg.Destroy()

    def SaveCurrentFile(self):
        """Check and save current file."""
        go_ahead = True
        if self.root:
            if self.activeitem != self.root:
                if self.editor.IsModified():  # Save modified file before
                    result = MsgDlg(self, 'The edited file has changed.  Save it?')
                    if result == wx.ID_YES:
                        self.editor.SaveFile(self.tree.GetItemText(self.activeitem))
                    if result == wx.ID_CANCEL:
                        go_ahead = False
                if go_ahead:
                    self.tree.SetItemBold(self.activeitem, 0)
        return go_ahead

    def OnProjectExit(self, event):
        """Quit the program."""
        if not self.close:
            self.close = True
            if not self.SaveCurrentFile():
                self.close = False
            if self.projectdirty and self.close:
                result = MsgDlg(self, 'The project has been changed.  Save?')
                if result == wx.ID_YES:
                    self.project_save()
                if result == wx.ID_CANCEL:
                    self.close = False
            if self.close:
                self.Close()
        else:
            event.Skip()

    def OnFileAdd(self, event):
        """Adds a file to the current project."""
        if not self.CheckTreeRootItem():
            return

        dlg = wx.FileDialog(self, 'Choose a file to add.', '.', '', '*.*', wx.OPEN)
        if dlg.ShowModal() == wx.ID_OK:
            path = os.path.split(dlg.GetPath())
            self.tree.AppendItem(self.root, path[1])
            self.tree.Expand(self.root)
            self.project_save()

    def OnFileRemove(self, event):
        """Removes a file to the current project."""
        if not self.CheckTreeRootItem():
            return
        item = self.tree.GetSelection()
        if item != self.root:
            self.tree.Delete(item)
            self.project_save()

    def OnFileOpen(self, event):
        """Opens current selected file."""
        if self.root:
            item = self.tree.GetSelection()
            if item != self.root:
                self.OnTreeItemActivated(None, item)
                return
        MsgDlg(self, 'There is no file to load.', 'Error!', wx.OK)

    def OnFileSave(self, event):
        """Saves current selected file."""
        if self.root:
            if self.activeitem != self.root:
                self.editor.SaveFile(self.tree.GetItemText(self.activeitem))
                return
        MsgDlg(self, 'There is no file to save.', 'Error!', wx.OK)


    def OnTreeLabelEdit(self, event):
        """Edit tree label (only root label can be edited)."""
        item = event.GetItem()
        if item != self.root:
            event.Veto()

    def OnTreeLabelEditEnd(self, event):
        """End editing the tree label."""
        self.projectdirty = True


    def OnTreeItemActivated(self, event, item=None):
        """Tree item was activated: try to open this file."""
        go_ahead = self.SaveCurrentFile()

        if go_ahead:
            if event:
                item = event.GetItem()
            self.activeitem = item
            if item != self.root:
                # load the current selected file
                self.tree.SetItemBold(item, 1)
                self.editor.Enable(1)
                self.editor.LoadFile(self.tree.GetItemText(item))
                self.editor.SetInsertionPoint(0)
                self.editor.SetFocus()
            else:
                self.editor.Clear()
                self.editor.Enable(0)


class App(wx.App):
    """wxProject Application."""
    def OnInit(self):
        """Create the wxProject Application."""
        frame = main_window(None, 'wxProject - ' + projfile)
        if projfile != 'Unnamed':
            frame.project_open(projfile)
        return True


if __name__ == '__main__':
    app = App(0)
    app.MainLoop()
