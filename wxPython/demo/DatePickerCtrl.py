
import wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        dpc = wx.DatePickerCtrl(self, size=(120,-1),
                                style=wx.DP_DROPDOWN | wx.DP_SHOWCENTURY)
        
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(dpc, 0, wx.ALL, 50)

        self.SetSizer(sizer)
        

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.DatePickerCtrl</center></h2>

This control allows the user to select a date. Unlike
wx.calendar.CalendarCtrl, which is a relatively big control,
wx.DatePickerCtrl is implemented as a small window showing the
currently selected date. The control can be edited using the keyboard,
and can also display a popup window for more user-friendly date
selection, depending on the styles used and the platform.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

