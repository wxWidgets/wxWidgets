# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wx.ProgressDialog appears to be broken. No abort button
#   and it's not possible to dismiss it otherwise.
# 

import  wx

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Create and Show a ProgressDialog", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        max = 80

        dlg = wx.ProgressDialog("Progress dialog example",
                               "An informative message",
                               maximum = max,
                               parent=self,
                               style = wx.PD_CAN_ABORT
                                | wx.PD_APP_MODAL
                                | wx.PD_ELAPSED_TIME
                                #| wx.PD_ESTIMATED_TIME
                                | wx.PD_REMAINING_TIME
                                )

        keepGoing = True
        count = 0

        while keepGoing and count < max:
            count += 1
            wx.MilliSleep(250)

            if count >= max / 2:
                (keepGoing, skip) = dlg.Update(count, "Half-time!")
            else:
                (keepGoing, skip) = dlg.Update(count)

                
        dlg.Destroy()

#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------


overview = """\
<html><body>
This class represents a dialog that shows a short message and a progress bar. 
Optionally, it can display an ABORT button
<p>
This dialog indicates the progress of some event that takes a while to accomplish, 
usually, such as file copy progress, download progress, and so on. The display
is <b>completely</b> under control of the program; you must update the dialog from
within the program creating it. 
<p>
When the dialog closes, you must check to see if the user aborted the process or
not, and act accordingly -- that is, if the PD_CAN_ABORT style flag is set. 
If not then you may progress blissfully onward.
</body></html>
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
