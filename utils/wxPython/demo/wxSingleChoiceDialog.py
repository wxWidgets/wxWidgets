
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxSingleChoiceDialog(frame, 'Test Single Choice', 'The Caption',
                               ['zero', 'one', 'two', 'three', 'four', 'five',
                                'six', 'seven', 'eight'])
    if dlg.ShowModal() == wxID_OK:
        log.WriteText('You selected: %s\n' % dlg.GetStringSelection())
    dlg.Destroy()

#---------------------------------------------------------------------------













overview = """\
This class represents a dialog that shows a list of strings, and allows the user to select one. Double-clicking on a list item is equivalent to single-clicking and then pressing OK.

wxSingleChoiceDialog()
---------------------------------------------

wxSingleChoiceDialog(wxWindow* parent, const wxString& message, const wxString& caption, int n, const wxString* choices, char** clientData = NULL, long style = wxOK | wxCANCEL | wxCENTRE, const wxPoint& pos = wxDefaultPosition)

Constructor, taking an array of wxString choices and optional client data.

Parameters
-------------------

parent = Parent window.

message = Message to show on the dialog.

caption = The dialog caption.
n = The number of choices.

choices = An array of strings, or a string list, containing the choices.

style = A dialog style (bitlist) containing flags chosen from the following:

wxOK  Show an OK button.

wxCANCEL  Show a Cancel button.

wxCENTRE  Centre the message. Not Windows.

pos = Dialog position.
"""
