
from wxPython.wx import *

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    dlg = wxMessageDialog(frame, 'Hello from Python and wxPython!',
                          'A Message Box', wxOK | wxICON_INFORMATION)
                          #wxYES_NO | wxNO_DEFAULT | wxCANCEL | wxICON_INFORMATION)
    dlg.ShowModal()
    dlg.Destroy()

#---------------------------------------------------------------------------













overview = """\
wxMessageDialog()
----------------------------------

wxMessageDialog(wxWindow* parent, const wxString& message, const wxString& caption = "Message box", long style = wxOK | wxCANCEL | wxCENTRE, const wxPoint& pos = wxDefaultPosition)

Constructor. Use wxMessageDialog::ShowModal to show the dialog.

Parameters
-------------------

parent = Parent window.

message = Message to show on the dialog.

caption = The dialog caption.

style = A dialog style (bitlist) containing flags chosen from the following:

wxOK  Show an OK button.

wxCANCEL  Show a Cancel button.

wxYES_NO  Show Yes and No buttons.

wxCENTRE  Centre the message. Not Windows.

wxICON_EXCLAMATION  Shows an exclamation mark icon. Windows only.

wxICON_HAND  Shows a hand icon. Windows only.

wxICON_QUESTION  Shows a question mark icon. Windows only.

wxICON_INFORMATION  Shows an information (i) icon. Windows only.

pos = Dialog position.
"""
