
from wxPython.wx import *
import os

#----------------------------------------------------------------------

text = """\
Right-click on the panel to get a menu.  This menu will be managed by
a wxFileHistory object and so the files you select will automatically
be added to the end of the menu and will be selectable the next time
the menu is viewed.  The filename selcted, either via the Open menu
item, or from the history, will be displayed in the log window below.
"""

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)
        box = wxBoxSizer(wxVERTICAL)

        # Make and layout the controls
        fs = self.GetFont().GetPointSize()
        bf = wxFont(fs+4, wxSWISS, wxNORMAL, wxBOLD)
        nf = wxFont(fs+2, wxSWISS, wxNORMAL, wxNORMAL)

        t = wxStaticText(self, -1, "wxFileHistory")
        t.SetFont(bf)
        box.Add(t, 0, wxCENTER|wxALL, 5)

        box.Add(wxStaticLine(self, -1), 0, wxEXPAND)
        box.Add(10,20)

        t = wxStaticText(self, -1, text)
        t.SetFont(nf)
        box.Add(t, 0, wxCENTER|wxALL, 5)

        self.SetSizer(box)
        self.SetAutoLayout(True)

        # Make a menu
        self.menu = m = wxMenu()
        m.Append(wxID_NEW,     "&New")
        m.Append(wxID_OPEN,    "&Open...")
        m.Append(wxID_CLOSE,   "&Close")
        m.Append(wxID_SAVE,    "&Save")
        m.Append(wxID_SAVEAS,  "Save &as...")
        m.Enable(wxID_NEW, False)
        m.Enable(wxID_CLOSE, False)
        m.Enable(wxID_SAVE, False)
        m.Enable(wxID_SAVEAS, False)

        # and a file history
        self.filehistory = wxFileHistory()
        self.filehistory.UseMenu(self.menu)

        # and finally the event handler bindings
        EVT_RIGHT_UP(self, self.OnRightClick)
        EVT_MENU(self, wxID_OPEN, self.OnFileOpenDialog)
        EVT_MENU_RANGE(self, wxID_FILE1, wxID_FILE9, self.OnFileHistory)
        EVT_WINDOW_DESTROY(self, self.Cleanup)


    def Cleanup(self, *args):
        del self.filehistory
        self.menu.Destroy()


    def OnRightClick(self, evt):
        self.PopupMenu(self.menu, evt.GetPosition())


    def OnFileOpenDialog(self, evt):
        dlg = wxFileDialog(self,
                           defaultDir = os.getcwd(),
                           wildcard = "All Files|*",
                           style = wxOPEN | wxCHANGE_DIR)
        if dlg.ShowModal() == wxID_OK:
            path = dlg.GetPath()
            self.log.write("You selected %s\n" % path)

            # add it to the history
            self.filehistory.AddFileToHistory(path)

        dlg.Destroy()


    def OnFileHistory(self, evt):
        # get the file based on the menu ID
        fileNum = evt.GetId() - wxID_FILE1
        path = self.filehistory.GetHistoryFile(fileNum)
        self.log.write("You selected %s\n" % path)

        # add it back to the history so it will be moved up the list
        self.filehistory.AddFileToHistory(path)



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h3>wxFileHistory</h3>

wxFileHistory encapsulates functionality to record the last few files
visited, and to allow the user to quickly load these files using the
list appended to a menu, such as the File menu.

</body></html>
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

