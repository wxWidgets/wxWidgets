
from wxPython.wx import *
from wxPython.calendar import *
from wxPython.utils import *

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, ID, log):
        wxPanel.__init__(self, parent, ID)
        self.log = log

        cal = wxCalendarCtrl(self, 101, wxDateTime_Now(), pos = (25,50),
                             style = wxCAL_SHOW_HOLIDAYS | wxCAL_SUNDAY_FIRST)

        EVT_CALENDAR(self, 101, self.OnCalSelected)


    def OnCalSelected(self, evt):
        self.log.write('OnCalSelected: %s\n' % evt.GetDate())


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, -1, log)
    return win

#----------------------------------------------------------------------


overview = """\
<html><body>
<h2>wxCalendarCtrl</h2>

Yet <i>another</i> calendar control.  This one is a wrapper around the C++
version described in the docs.  This one will probably be a bit more efficient
than the one in wxPython.lib.calendar, but I like a few things about it better,
so I think both will stay in wxPython.
"""
