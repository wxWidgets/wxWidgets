
from wxPython.wx import *

#---------------------------------------------------------------------------

RBOX1 = wxNewId()
RBOX2 = wxNewId()
RBUT1 = wxNewId()
RBUT2 = wxNewId()

class TestRadioButtons(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)
        #self.SetBackgroundColour(wxBLUE)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight']

        sizer = wxBoxSizer(wxVERTICAL)
        rb = wxRadioBox(self, RBOX1, "wxRadioBox",
                        wxDefaultPosition, wxDefaultSize,
                        sampleList, 2, wxRA_SPECIFY_COLS)
        EVT_RADIOBOX(self, RBOX1, self.EvtRadioBox)
        #rb.SetBackgroundColour(wxBLUE)
        rb.SetToolTip(wxToolTip("This is a ToolTip!"))
        #rb.SetLabel("wxRadioBox")
        sizer.Add(rb, 0, wxALL, 20)

        rb = wxRadioBox(self, RBOX2, "", wxDefaultPosition, wxDefaultSize,
                        sampleList, 3, wxRA_SPECIFY_COLS | wxNO_BORDER)
        EVT_RADIOBOX(self, RBOX2, self.EvtRadioBox)
        rb.SetToolTip(wxToolTip("This box has no label"))
        sizer.Add(rb, 0, wxLEFT|wxRIGHT|wxBOTTOM, 20)

        sizer.Add(wxStaticText(self, -1, "These are plain wxRadioButtons"),
                  0, wxLEFT|wxRIGHT, 20)
        sizer.Add(wxRadioButton(self, RBUT1, "wxRadioButton 1"),
                  0, wxLEFT|wxRIGHT, 20)
        sizer.Add(wxRadioButton(self, RBUT2, "wxRadioButton 2"),
                  0, wxLEFT|wxRIGHT, 20)
        EVT_RADIOBUTTON(self, RBUT1, self.EvtRadioButton)
        EVT_RADIOBUTTON(self, RBUT2, self.EvtRadioButton)

        self.SetSizer(sizer)


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


