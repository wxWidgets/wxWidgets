
from wxPython.wx import *

#---------------------------------------------------------------------------

buttonDefs = {
    814 : ('GridSimple',     'Simple wxGrid, catching all events'),
    815 : ('GridCustEdRend', 'wxGrid showing custom Editors and Renderers'),
    816 : ('GridCustTable',  'wxGrid using a custom Table'),
    817 : ('GridHugeTable',  'A wxGrid with a HUGE table (100M cells!)'),
    }


class ButtonPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        box = wxBoxSizer(wxVERTICAL)
        box.Add(20, 30)
        keys = buttonDefs.keys()
        keys.sort()
        for k in keys:
            text = buttonDefs[k][1]
            btn = wxButton(self, k, text)
            box.Add(btn, 0, wxALIGN_CENTER|wxALL, 15)
            EVT_BUTTON(self, k, self.OnButton)

        self.SetAutoLayout(true)
        self.SetSizer(box)


    def OnButton(self, evt):
        modName = buttonDefs[evt.GetId()][0]
        module = __import__(modName)
        frame = module.TestFrame(self, self.log)
        frame.Show(true)


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = ButtonPanel(nb, log)
    return win

#---------------------------------------------------------------------------










overview = """\
This demo shows various ways of using the ** NEW ** wxGrid.  For more details see wxPython/demo/Grid*.py.
"""
