
import os
import wx
from wx import html

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        hwin = html.HtmlWindow(self, -1)
        hwin.LoadFile(os.path.join(os.path.dirname(wx.__file__), 'wx.html'))

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(hwin, 1, wx.EXPAND)

        self.SetSizer(sizer)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Using the New Namespace</center></h2>

This sample isn't really a demo, but rather a place to display the
introductory doc for using the new namespace.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

