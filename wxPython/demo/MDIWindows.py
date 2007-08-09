
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
        exe, spawn = self.GetPyExecutable()
        spawn(os.P_NOWAIT, exe, exe, "MDIDemo.py")

    def ShowMDISashDemo(self, evt):
        exe, spawn = self.GetPyExecutable()
        spawn(os.P_NOWAIT, exe, exe, "MDISashDemo.py")

    # TODO: This hack can be removed once we fix the way the Python
    # app bundles are generated so that they are not bundling and 
    # pointing to an otherwise unused and non-GUI-friendly version of
    # Python on OS X.
    def GetPyExecutable(self):
        if 'wxMac' in wx.PlatformInfo:
            # sys.executable will be wrong if running the demo from
            # an app bundle.  But the bundle is always using a system
            # framework so just hardcode the path to it.
            if sys.version[:3] == "2.4":
                return '/usr/local/bin/pythonw', os.spawnl
            else:
                return '/usr/bin/pythonw', os.spawnl    
        else:
            return sys.executable, os.spawnl
        
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
