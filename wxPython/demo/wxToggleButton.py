
from wxPython.wx import *

haveToggleBtn = 1
try:
    wxToggleButton
except NameError:
    haveToggleBtn = 0

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        panel = wxPanel(self, -1)
        buttons = wxBoxSizer(wxHORIZONTAL)
        for word in "These are toggle buttons".split():
            b = wxToggleButton(panel, -1, word)
            EVT_TOGGLEBUTTON(self, b.GetId(), self.OnToggle)
            buttons.Add(b, flag=wxALL, border=5)

        panel.SetAutoLayout(True)
        panel.SetSizer(buttons)
        buttons.Fit(panel)
        panel.Move((50,50))

    def OnToggle(self, evt):
        self.log.write("Button %d toggled\n" % evt.GetId())

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    if haveToggleBtn:
        win = TestPanel(nb, log)
        return win
    else:
        dlg = wxMessageDialog(frame, 'wxToggleButton is not available on this platform.',
                          'Sorry', wxOK | wxICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()




#----------------------------------------------------------------------


overview = """\
"""
