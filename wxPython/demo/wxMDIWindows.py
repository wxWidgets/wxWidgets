
from wxPython.wx import *

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)

        b1 = wxButton(self, -1, "MDI demo")
        EVT_BUTTON(self, b1.GetId(), self.ShowMDIDemo)

        b2 = wxButton(self, -1, "MDI with SashWindows demo")
        EVT_BUTTON(self, b2.GetId(), self.ShowMDISashDemo)

        box = wxBoxSizer(wxVERTICAL)
        box.Add(20, 30)
        box.Add(b1, 0, wxALIGN_CENTER|wxALL, 15)
        box.Add(b2, 0, wxALIGN_CENTER|wxALL, 15)
        self.SetAutoLayout(True)
        self.SetSizer(box)


    def ShowMDIDemo(self, evt):
        import MDIDemo
        frame = MDIDemo.MyParentFrame()
        frame.Show()

    def ShowMDISashDemo(self, evt):
        import MDISashDemo
        frame = MDISashDemo.MyParentFrame()
        frame.Show()



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Multiple Document Interface</center></h2>

Although Microsoft has deprecated the MDI model, wxWindows still supports
it.  Here are a couple samples of how to use it.

</body></html>
"""





if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
