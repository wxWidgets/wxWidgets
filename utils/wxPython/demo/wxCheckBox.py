
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestCheckBox(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        wxStaticText(self, -1, "This example uses the wxCheckBox control.",
                               wxPoint(10, 10))

        cID = NewId()
        cb1 = wxCheckBox(self, cID,   "  Apples", wxPoint(65, 40), wxSize(150, 20), wxNO_BORDER)
        cb2 = wxCheckBox(self, cID+1, "  Oranges", wxPoint(65, 60), wxSize(150, 20), wxNO_BORDER)
        cb2.SetValue(true)
        cb3 = wxCheckBox(self, cID+2, "  Pears", wxPoint(65, 80), wxSize(150, 20), wxNO_BORDER)

        EVT_CHECKBOX(self, cID,   self.EvtCheckBox)
        EVT_CHECKBOX(self, cID+1, self.EvtCheckBox)
        EVT_CHECKBOX(self, cID+2, self.EvtCheckBox)


    def EvtCheckBox(self, event):
        self.log.WriteText('EvtCheckBox: %d\n' % event.Checked())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestCheckBox(nb, log)
    return win

#---------------------------------------------------------------------------















overview = """\
A checkbox is a labelled box which is either on (checkmark is visible) or off (no checkmark).

wxCheckBox()
-----------------------

Default constructor.

wxCheckBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& val, const wxString& name = "checkBox")

Constructor, creating and showing a checkbox.

Parameters
-------------------

parent = Parent window. Must not be NULL.

id = Checkbox identifier. A value of -1 indicates a default value.

label = Text to be displayed next to the checkbox.

pos = Checkbox position. If the position (-1, -1) is specified then a default position is chosen.

size = Checkbox size. If the default size (-1, -1) is specified then a default size is chosen.

style = Window style. See wxCheckBox.

validator = Window validator.

name = Window name.
"""
