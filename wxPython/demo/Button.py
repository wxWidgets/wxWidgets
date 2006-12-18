
import  wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1,
                         style=wx.NO_FULL_REPAINT_ON_RESIZE)
        self.log = log

        b = wx.Button(self, 10, "Default Button", (20, 20))
        self.Bind(wx.EVT_BUTTON, self.OnClick, b)
        b.SetDefault()
        b.SetSize(b.GetBestSize())

        b = wx.Button(self, 20, "HELLO AGAIN!", (20, 80)) ##, (120, 45))
        self.Bind(wx.EVT_BUTTON, self.OnClick, b)
        b.SetToolTipString("This is a Hello button...")

        b = wx.Button(self, 40, "Flat Button?", (20,150), style=wx.NO_BORDER)
        b.SetToolTipString("This button has a style flag of wx.NO_BORDER.\nOn some platforms that will give it a flattened look.")
        self.Bind(wx.EVT_BUTTON, self.OnClick, b)


    def OnClick(self, event):
        self.log.write("Click! (%d)\n" % event.GetId())

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = """<html><body>
<h2>Button</h2>

A button is a control that contains a text string or a bitmap and can be
placed on nearly any kind of window.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

