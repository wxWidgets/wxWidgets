
from wxPython.wx import *
from wxPython.calendar import *
from wxPython.utils import *

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, ID, log):
        wxPanel.__init__(self, parent, ID)
        self.log = log

        cal = wxCalendarCtrl(self, -1, wxDateTime_Now(), pos = (25,50),
                             style = wxCAL_SHOW_HOLIDAYS | wxCAL_SUNDAY_FIRST)

        EVT_CALENDAR(self, cal.GetId(), self.OnCalSelected)

        b = wxButton(self, -1, "Destroy the Calendar", pos = (250, 50))
        EVT_BUTTON(self, b.GetId(), self.OnButton)
        self.cal = cal

        # Set up control to display a set of holidays:
        EVT_CALENDAR_MONTH(self, cal.GetId(), self.OnChangeMonth)
        self.holidays = [(1,1), (10,31), (12,25) ]    # (these don't move around)
        self.OnChangeMonth()

    def OnButton(self, evt):
        self.cal.Destroy()
        self.cal = None

    def OnCalSelected(self, evt):
        self.log.write('OnCalSelected: %s\n' % evt.GetDate())

    def OnChangeMonth(self, evt=None):
        cur_month = self.cal.GetDate().GetMonth() + 1   # convert wxDateTime 0-11 => 1-12
        for month, day in self.holidays:
            if month == cur_month:
                self.cal.SetHoliday(day)

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
