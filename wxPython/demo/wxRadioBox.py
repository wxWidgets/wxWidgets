
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestRadioButtons(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)
        #self.SetBackgroundColour(wxBLUE)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight']

        rb = wxRadioBox(self, 30, "wxRadioBox", wxPoint(35, 30), wxDefaultSize,
                        sampleList, 3, wxRA_SPECIFY_COLS)
        EVT_RADIOBOX(self, 30, self.EvtRadioBox)
        #rb.SetBackgroundColour(wxBLUE)
        rb.SetToolTip(wxToolTip("This is a ToolTip!"))

        wxRadioButton(self, 32, "wxRadioButton", (235, 35))
        wxRadioButton(self, 33, "wxRadioButton", (235, 55))
        EVT_RADIOBUTTON(self, 32, self.EvtRadioButton)
        EVT_RADIOBUTTON(self, 33, self.EvtRadioButton)

        rb = wxRadioBox(self, 35, "", wxPoint(35, 120), wxDefaultSize,
                        sampleList, 3, wxRA_SPECIFY_COLS | wxNO_BORDER)
        EVT_RADIOBOX(self, 35, self.EvtRadioBox)


    def EvtRadioBox(self, event):
        self.log.WriteText('EvtRadioBox: %d\n' % event.GetInt())

    def EvtRadioButton(self, event):
        self.log.write('EvtRadioButton:%d\n' % event.GetInt())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestRadioButtons(nb, log)
    return win

#---------------------------------------------------------------------------











overview = """\
A radio box item is used to select one of number of mutually exclusive choices.  It is displayed as a vertical column or horizontal row of labelled buttons.

"""


