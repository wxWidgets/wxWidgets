
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestComboBox(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight']

        wxStaticText(self, -1, "This example uses the wxComboBox control.",
                               wxPoint(8, 10))

        wxStaticText(self, -1, "Select one:", wxPoint(15, 50), wxSize(75, 18))
        wxComboBox(self, 500, "default value", wxPoint(80, 50), wxSize(95, 20),
                   sampleList, wxCB_DROPDOWN)
        EVT_COMBOBOX(self, 500, self.EvtComboBox)


    def EvtComboBox(self, event):
        self.log.WriteText('EvtComboBox: %s\n' % event.GetString())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestComboBox(nb, log)
    return win

#---------------------------------------------------------------------------












overview = """\
A combobox is like a combination of an edit control and a listbox. It can be displayed as static list with editable or read-only text field; or a drop-down list with text field; or a drop-down list without a text field.

A combobox permits a single selection only. Combobox items are numbered from zero.

wxComboBox()
-----------------------

Default constructor.

wxComboBox(wxWindow* parent, wxWindowID id, const wxString& value = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n, const wxString choices[], long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = "comboBox")

Constructor, creating and showing a combobox.

Parameters
-------------------

parent = Parent window. Must not be NULL.

id = Window identifier. A value of -1 indicates a default value.

pos = Window position.

size = Window size. If the default size (-1, -1) is specified then the window is sized appropriately.

n = Number of strings with which to initialise the control.

choices = An array of strings with which to initialise the control.

style = Window style. See wxComboBox.

validator = Window validator.

name = Window name.
"""
