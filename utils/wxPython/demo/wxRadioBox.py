
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestRadioButtons(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight']

        rb = wxRadioBox(self, 30, "wxRadioBox", wxPoint(35, 30), wxDefaultSize,
                        sampleList, 3, wxRA_SPECIFY_COLS)
        EVT_RADIOBOX(self, 30, self.EvtRadioBox)


        rb = wxRadioBox(self, 30, "wxRadioBox", wxPoint(35, 120), wxDefaultSize,
                        sampleList, 3, wxRA_SPECIFY_COLS | wxNO_BORDER)
        EVT_RADIOBOX(self, 30, self.EvtRadioBox)


    def EvtRadioBox(self, event):
        self.log.WriteText('EvtRadioBox: %d\n' % event.GetInt())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestRadioButtons(nb, log)
    return win

#---------------------------------------------------------------------------











overview = """\
A radio box item is used to select one of number of mutually exclusive choices.  It is displayed as a vertical column or horizontal row of labelled buttons.

wxRadioBox()
----------------------

Default constructor.

wxRadioBox(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, int majorDimension = 0, long style = wxRA_SPECIFY_COLS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "radioBox")

Constructor, creating and showing a radiobox.

Parameters
-------------------

parent = Parent window. Must not be NULL.

id = Window identifier. A value of -1 indicates a default value.

label = Label for the static box surrounding the radio buttons.

pos = Window position. If the position (-1, -1) is specified then a default position is chosen.

size = Window size. If the default size (-1, -1) is specified then a default size is chosen.

n = Number of choices with which to initialize the radiobox.

choices = An array of choices with which to initialize the radiobox.

majorDimension = Specifies the maximum number of rows (if style contains wxRA_SPECIFY_ROWS) or columns (if style contains wxRA_SPECIFY_COLS) for a two-dimensional radiobox.

style = Window style. See wxRadioBox.

validator = Window validator.

name = Window name.
"""
