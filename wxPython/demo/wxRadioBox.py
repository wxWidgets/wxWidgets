
from wxPython.wx import *

#---------------------------------------------------------------------------

RBUT1 = wxNewId()
RBUT2 = wxNewId()
RBUT3 = wxNewId()
RBUT4 = wxNewId()

RBOX1 = wxNewId()
RBOX2 = wxNewId()

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

        self.SetSizer(sizer)


    def EvtRadioBox(self, event):
        self.log.WriteText('EvtRadioBox: %d\n' % event.GetInt())

    def EvtRadioButton(self, event):
        self.log.write('EvtRadioButton:%d\n' % event.GetId())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestRadioButtons(nb, log)
    return win



overview = """\
A radio box item is used to select one of number of mutually exclusive
choices.  It is displayed as a vertical column or horizontal row of
labelled buttons.

"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

