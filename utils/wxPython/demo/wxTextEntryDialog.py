
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxTextEntryDialog(frame, 'What is your favorite programming language?',
                            'Duh??', 'Python')
    dlg.SetValue("Python is the best!")  #### this doesn't work?
    if dlg.ShowModal() == wxID_OK:
        log.WriteText('You entered: %s\n' % dlg.GetValue())
    dlg.Destroy()


#---------------------------------------------------------------------------












overview = """\
This class represents a dialog that requests a one-line text string from the user. It is implemented as a generic wxWindows dialog.

wxTextEntryDialog()
----------------------------------

wxTextEntryDialog(wxWindow* parent, const wxString& message, const wxString& caption = "Please enter text", const wxString& defaultValue = "", long style = wxOK |  wxCANCEL |  wxCENTRE, const wxPoint& pos = wxDefaultPosition)

Constructor. Use wxTextEntryDialog::ShowModal to show the dialog.

Parameters
-------------------

parent = Parent window.

message = Message to show on the dialog.

defaultValue = The default value, which may be the empty string.

style = A dialog style, specifying the buttons (wxOK, wxCANCEL) and an optional wxCENTRE style.

pos = Dialog position.
"""
