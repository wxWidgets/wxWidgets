from wxPython.wx import *


class MyFrame(wxFrame):
    def __init__(self, parent, id, title='A pxFrame!'):
        wxFrame.__init__(self, parent, id, title,
            wxPyDefaultPosition, wxSize(50, 50))

    def get_filename(self):
        dlg = wxFileDialog(self, "Choose a file", ".", "", "*.*", wxOPEN)
        dlg.ShowModal()
        self.file = dlg.GetPath()
        dlg.Destroy()
        self.Iconize(true)
        return self.file


class FilePicker(wxApp):
    def OnInit(self):
        return true

    def get_filename(self):
        dlg = wxFileDialog(NULL, "Choose a file", ".", "", "*.*", wxOPEN)
        dlg.ShowModal()
        self.file = dlg.GetPath()
        dlg.Destroy()
        return self.file

