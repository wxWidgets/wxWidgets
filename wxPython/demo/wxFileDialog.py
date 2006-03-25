
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxFileDialog(frame, "Choose a file", ".", "", "*.*", wxOPEN|wxMULTIPLE)
    if dlg.ShowModal() == wxID_OK:
        for path in dlg.GetPaths():
            log.WriteText('You selected: %s\n' % path)
    dlg.Destroy()

#---------------------------------------------------------------------------














overview = """\
This class represents the file chooser dialog.

wxFileDialog()
----------------------------

wxFileDialog(wxWindow* parent, const wxString& message = "Choose a file", const wxString& defaultDir = ""
, const wxString& defaultFile = "", const wxString& wildcard = "*.*", long style = 0, const wxPoint& pos = wxDefaultPosition)

Constructor. Use wxFileDialog::ShowModal to show the dialog.

Parameters
-------------------

parent = Parent window.

message = Message to show on the dialog.

defaultDir = The default directory, or the empty string.

defaultFile = The default filename, or the empty string.

wildcard = A wildcard, such as "*.*".

style = A dialog style. A bitlist of:

wxOPEN	This is an open dialog (Windows only).

wxSAVE	This is a save dialog (Windows only).

wxHIDE_READONLY	Hide read-only files (Windows only).

wxOVERWRITE_PROMPT	Prompt for a conformation if a file will be overridden (Windows only).

pos = Dialog position.
"""
