# 11/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Is it just me or are the graphics for the control not lining up right?
# 

import  wx
import  wx.lib.popupctl as  pop
import  wx.calendar     as  cal

class TestDateControl(pop.wxPopupControl):
    def __init__(self,*_args,**_kwargs):
        apply(pop.wxPopupControl.__init__,(self,) + _args,_kwargs)

        self.win = wx.Window(self,-1,pos = (0,0),style = 0)
        self.cal = cal.CalendarCtrl(self.win,-1,pos = (0,0))

        bz = self.cal.GetBestSize()
        self.win.SetSize(bz)

        # This method is needed to set the contents that will be displayed
        # in the popup
        self.SetPopupContent(self.win)

        # Event registration for date selection
        self.cal.Bind(cal.EVT_CALENDAR_DAY, self.OnCalSelected)

    # Method called when a day is selected in the calendar
    def OnCalSelected(self,evt):
        self.PopDown()
        date = self.cal.GetDate()

        # Format the date that was selected for the text part of the control
        self.SetValue('%02d/%02d/%04d' % (date.GetDay(),
                                          date.GetMonth()+1,
                                          date.GetYear()))
        evt.Skip()

    # Method overridden from wxPopupControl
    # This method is called just before the popup is displayed
    # Use this method to format any controls in the popup
    def FormatContent(self):
        # I parse the value in the text part to resemble the correct date in
        # the calendar control
        txtValue = self.GetValue()
        dmy = txtValue.split('/')
        didSet = False

        if len(dmy) == 3:
            date = self.cal.GetDate()
            d = int(dmy[0])
            m = int(dmy[1]) - 1
            y = int(dmy[2])

            if d > 0 and d < 31:
                if m >= 0 and m < 12:
                    if y > 1000:
                        self.cal.SetDate(wx.DateTimeFromDMY(d,m,y))
                        didSet = True

        if not didSet:
            self.cal.SetDate(wx.DateTime_Today())


#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        date = TestDateControl(self, -1, pos = (30,30), size = (100,22))

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------

overview = """<html><body>
<h2><center>wxPopupControl</center></h2>

wxPopupControl is a class that can display a value and has a button
that will popup another window similar to how a wxComboBox works.  The
popup window can contain whatever is needed to edit the value.  This
example uses a wxCalendarCtrl.

<p>Currently a wxDialog is used for the popup.  Eventually a
wxPopupWindow should be used...

</body></html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

