
from wxPython.wx import *

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        panel = wxPanel(self, -1)
        buttons = wxBoxSizer(wxHORIZONTAL)
        for word in string.split("These are toggle buttons"):
            b = wxToggleButton(panel, -1, word)
            EVT_TOGGLEBUTTON(self, b.GetId(), self.OnToggle)
            buttons.Add(b, flag=wxALL, border=5)

        panel.SetAutoLayout(true)
        panel.SetSizer(buttons)
        buttons.Fit(panel)
        panel.Move((50,50))

    def OnToggle(self, evt):
        self.log.write("Button %d toggled\n" % evt.GetId())

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#----------------------------------------------------------------------


overview = """\
"""
