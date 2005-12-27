
import  wx
import os
import sys

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b1 = wx.Button(self, -1, "MDI demo")
        self.Bind(wx.EVT_BUTTON, self.ShowMDIDemo, b1)

        b2 = wx.Button(self, -1, "MDI with SashWindows demo")
        self.Bind(wx.EVT_BUTTON, self.ShowMDISashDemo, b2)

        box = wx.BoxSizer(wx.VERTICAL)
        box.Add((20, 30))
        box.Add(b1, 0, wx.ALIGN_CENTER|wx.ALL, 15)
        box.Add(b2, 0, wx.ALIGN_CENTER|wx.ALL, 15)
        self.SetAutoLayout(True)
        self.SetSizer(box)


    # These are spawned as new processes because on Mac there can be
    # some problems related to having regular frames and MDI frames in
    # the same app.
    def ShowMDIDemo(self, evt):
        os.spawnl(os.P_NOWAIT, sys.executable, sys.executable, "MDIDemo.py")

    def ShowMDISashDemo(self, evt):
        os.spawnl(os.P_NOWAIT, sys.executable, sys.executable, "MDISashDemo.py")


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Multiple Document Interface</center></h2>

Although Microsoft has deprecated the MDI model, wxWindows still supports
it.  Here are a couple samples of how to use it - one straightforward, the other
showing how the MDI interface can be integrated into a SashWindow interface.

</body></html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
