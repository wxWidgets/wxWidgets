
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxDirDialog(frame)
    if dlg.ShowModal() == wxID_OK:
        log.WriteText('You selected: %s\n' % dlg.GetPath())
    dlg.Destroy()

#---------------------------------------------------------------------------


















overview = """\
This class represents the directory chooser dialog.

wxDirDialog()
------------------------

wxDirDialog(wxWindow* parent, const wxString& message = "Choose a directory", const wxString& defaultPath = "", long style = 0, const wxPoint& pos = wxDefaultPosition)

Constructor. Use wxDirDialog::ShowModal to show the dialog.

Parameters
-------------------

parent = Parent window.

message = Message to show on the dialog.

defaultPath = The default path, or the empty string.

style = A dialog style, currently unused.

pos = Dialog position.
"""
