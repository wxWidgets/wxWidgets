
import wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        box = wx.StaticBox(self, -1, "This is a wx.StaticBox")
        bsizer = wx.StaticBoxSizer(box, wx.VERTICAL)

        t = wx.StaticText(self, -1, "Controls placed \"inside\" the box are really its siblings")
        bsizer.Add(t, 0, wx.TOP|wx.LEFT, 10)


        border = wx.BoxSizer()
        border.Add(bsizer, 1, wx.EXPAND|wx.ALL, 25)
        self.SetSizer(border)
        

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.StaticBox</center></h2>

This control draws a box and can be used to group other controls.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

