#!/bin/python
import sys, os
from   wxPython.wx import *
from   string import *

# Process the command line.  Not much to do;
# just get the name of the project file if it's given. Simple.
projfile = 'Unnamed'
if len(sys.argv) > 1:
   projfile = sys.argv[1]


def MsgBox (window, string):
    dlg=wxMessageDialog(window, string, 'wxProject', wxOK)
    dlg.ShowModal()
    dlg.Destroy()

class main_window(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, -1, title, size = (500, 500),
                         style=wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE)


        # ------------------------------------------------------------------------------------
        # Set up menu bar for the program.
        # ------------------------------------------------------------------------------------
        self.mainmenu = wxMenuBar()   # Create menu bar.
        mainwindow = self

        menu=wxMenu()                                 # Make a menu (will be the Project menu)

        exitID=wxNewId()                              # Make a new ID for a menu entry.
        menu.Append(exitID, '&Open', 'Open project')  # Name the ID by adding it to the menu.
        EVT_MENU(self, exitID, self.OnProjectOpen)    # Create and assign a menu event.
        exitID=wxNewId()
        menu.Append(exitID, '&New', 'New project')
        EVT_MENU(self, exitID, self.OnProjectNew)
        exitID=wxNewId()
        menu.Append(exitID, 'E&xit', 'Exit program')
        EVT_MENU(self, exitID, self.OnProjectExit)

        self.mainmenu.Append (menu, '&Project')       # Add the project menu to the menu bar.


        menu=wxMenu()                                 # Make a menu (will be the File menu)

        exitID=wxNewId()
        menu.Append(exitID, '&Add', 'Add file to project')
        EVT_MENU(self, exitID, self.OnFileAdd)
        exitID=wxNewId()
        menu.Append(exitID, '&Remove', 'Remove file from project')
        EVT_MENU(self, exitID, self.OnFileRemove)
        exitID=wxNewId()
        menu.Append(exitID, '&Open', 'Open file for editing')
        EVT_MENU(self, exitID, self.OnFileOpen)
        exitID=wxNewId()
        menu.Append(exitID, '&Save', 'Save file')
        EVT_MENU(self, exitID, self.OnFileSave)

        self.mainmenu.Append (menu, '&File')          # Add the file menu to the menu bar.

        self.SetMenuBar (self.mainmenu)               # Attach the menu bar to the window.


        # ------------------------------------------------------------------------------------
        # Create the splitter window.
        # ------------------------------------------------------------------------------------
        splitter = wxSplitterWindow (self, -1, style=wxNO_3D|wxSP_3D)
        splitter.SetMinimumPaneSize (1)

        # ------------------------------------------------------------------------------------
        # Create the tree on the left.
        # ------------------------------------------------------------------------------------
        tID = wxNewId()
        self.tree = wxTreeCtrl (splitter, tID, style=wxTR_HAS_BUTTONS |
                                   wxTR_EDIT_LABELS |
                                   wxTR_HAS_VARIABLE_ROW_HEIGHT)
        EVT_TREE_BEGIN_LABEL_EDIT(self.tree, tID, self.OnTreeLabelEdit)
        EVT_TREE_END_LABEL_EDIT(self.tree, tID, self.OnTreeLabelEditEnd)
        EVT_TREE_ITEM_ACTIVATED(self.tree, tID, self.OnTreeItemActivated)

        # ------------------------------------------------------------------------------------
        # Create the editor on the right.
        # ------------------------------------------------------------------------------------
        self.editor = wxTextCtrl(splitter, -1, style=wxTE_MULTILINE)
        self.editor.Enable (0)

        # ------------------------------------------------------------------------------------
        # Install the tree and the editor.
        # ------------------------------------------------------------------------------------
        splitter.SplitVertically (self.tree, self.editor)
        splitter.SetSashPosition (180, true)

        self.Show(true)

        # Some global state variables.
        self.projectdirty = false

    # ----------------------------------------------------------------------------------------
    # Some nice little handlers.
    # ----------------------------------------------------------------------------------------


    def project_open(self, project_file):
        try:
            input = open (project_file, 'r')

            self.tree.DeleteAllItems()

            self.project_file = project_file
            name = replace (input.readline(), "\n", "")
            self.SetTitle (name)
            self.root = self.tree.AddRoot(name)
            self.activeitem = self.root
            for line in input.readlines():
                self.tree.AppendItem (self.root, replace(line, "\n", ""))
            input.close
            self.tree.Expand (self.root)

            self.editor.Clear()
            self.editor.Enable (false)

            self.projectdirty = false
        except IOError:
            pass

    def project_save(self):
        try:
            output = open (self.project_file, 'w+')
            output.write (self.tree.GetItemText (self.root) + "\n")

            count = self.tree.GetChildrenCount (self.root)
            iter = 0
            child = ''
            for i in range(count):
               if i == 0:
                  (child,iter) = self.tree.GetFirstChild(self.root,iter)
               else:
                  (child,iter) = self.tree.GetNextChild(self.root,iter)
               output.write (self.tree.GetItemText(child) + "\n")
            output.close()
            self.projectdirty = false
        except IOError:
            dlg_m = wxMessageDialog (self, 'There was an error saving the project file.',
                                     'Error!', wxOK)
            dlg_m.ShowModal()
            dlg_m.Destroy()

    # ----------------------------------------------------------------------------------------
    # Event handlers from here on out.
    # ----------------------------------------------------------------------------------------

    def OnProjectOpen(self, event):
        open_it = true
        if self.projectdirty:
           dlg=wxMessageDialog(self, 'The project has been changed.  Save?', 'wxProject',
                               wxYES_NO | wxCANCEL)
           result = dlg.ShowModal()
           if result == wxID_YES:
              self.project_save()
           if result == wxID_CANCEL:
              open_it = false
           dlg.Destroy()
        if open_it:
           dlg = wxFileDialog(self, "Choose a project to open", ".", "", "*.wxp", wxOPEN)
           if dlg.ShowModal() == wxID_OK:
              self.project_open(dlg.GetPath())
           dlg.Destroy()

    def OnProjectNew(self, event):
        open_it = true
        if self.projectdirty:
           dlg=wxMessageDialog(self, 'The project has been changed.  Save?', 'wxProject',
                               wxYES_NO | wxCANCEL)
           result = dlg.ShowModal()
           if result == wxID_YES:
              self.project_save()
           if result == wxID_CANCEL:
              open_it = false
           dlg.Destroy()

        if open_it:
           dlg = wxTextEntryDialog (self, "Name for new project:", "New Project",
                                    "New project", wxOK | wxCANCEL)
           if dlg.ShowModal() == wxID_OK:
               newproj = dlg.GetValue()
               dlg.Destroy()
               dlg = wxFileDialog (self, "Place to store new project", ".", "", "*.wxp",
                                   wxSAVE)
               if dlg.ShowModal() == wxID_OK:
                  try:
                     proj = open (dlg.GetPath(), 'w')
                     proj.write (newproj + "\n")
                     proj.close()
                     self.project_open (dlg.GetPath())
                  except IOError:
                     dlg_m = wxMessageDialog (self,
                                              'There was an error saving the new project file.',
                                              'Error!', wxOK)
                     dlg_m.ShowModal()
                     dlg_m.Destroy()
        dlg.Destroy()

    def OnProjectExit(self, event):
        close = true
        if self.projectdirty:
           dlg=wxMessageDialog(self, 'The project has been changed.  Save?', 'wxProject',
                               wxYES_NO | wxCANCEL)
           result = dlg.ShowModal()
           if result == wxID_YES:
              self.project_save()
           if result == wxID_CANCEL:
              close = false
           dlg.Destroy()
        if close:
           self.Close()

    def OnFileAdd(self, event):
        dlg = wxFileDialog (self, "Choose a file to add", ".", "", "*.*", wxOPEN)
        if dlg.ShowModal() == wxID_OK:
           path = os.path.split(dlg.GetPath())
           self.tree.AppendItem (self.root, path[1])
           self.tree.Expand (self.root)
           self.project_save()

    def OnFileRemove(self, event):
        item = self.tree.GetSelection()
        if item != self.root:
           self.tree.Delete (item)
           self.project_save()

    def OnFileOpen(self, event):
        item = self.tree.GetSelection()

    def OnFileSave(self, event):
        if self.activeitem != self.root:
           self.editor.SaveFile (self.tree.GetItemText (self.activeitem))


    def OnTreeLabelEdit(self, event):
        item=event.GetItem()
        if item != self.root:
           event.Veto()

    def OnTreeLabelEditEnd(self, event):
        self.projectdirty = true

    def OnTreeItemActivated(self, event):
        go_ahead = true
        if self.activeitem != self.root:
           if self.editor.IsModified():
              dlg=wxMessageDialog(self, 'The edited file has changed.  Save it?',
                                  'wxProject', wxYES_NO | wxCANCEL)
              result = dlg.ShowModal()
              if result == wxID_YES:
                 self.editor.SaveFile (self.tree.GetItemText (self.activeitem))
              if result == wxID_CANCEL:
                 go_ahead = false
              dlg.Destroy()
           if go_ahead:
              self.tree.SetItemBold (self.activeitem, 0)

        if go_ahead:
           item=event.GetItem()
           self.activeitem = item
           if item != self.root:
              self.tree.SetItemBold (item, 1)
              self.editor.Enable (1)
              self.editor.LoadFile (self.tree.GetItemText(item))
              self.editor.SetInsertionPoint (0)
              self.editor.SetFocus()
           else:
              self.editor.Clear()
              self.editor.Enable (0)

class App(wxApp):
    def OnInit(self):
        frame = main_window(None, -1, "wxProject - " + projfile)
        self.SetTopWindow(frame)
        if (projfile != 'Unnamed'):
            frame.project_open (projfile)
        return true

app = App(0)
app.MainLoop()

