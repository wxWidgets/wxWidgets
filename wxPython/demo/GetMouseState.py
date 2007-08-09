
import wx

#----------------------------------------------------------------------

class StaticText(wx.StaticText):
    """
    A StaticText that only updates the label if it has changed, to
    help reduce potential flicker since these controls would be
    updated very frequently otherwise.
    """
    def SetLabel(self, label):
        if label <> self.GetLabel():
            wx.StaticText.SetLabel(self, label)

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(sizer)
        sizer.Add((25,25))
        sizer.Add(wx.StaticText(
                      self, -1,
                      "Mouse and modifier state can be polled with wx.GetMouseState"),
                  0, wx.CENTER|wx.ALL, 10)
        sizer.Add(wx.StaticLine(self), 0, wx.EXPAND|wx.TOP, 10)
        
        row = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(row, 0, wx.CENTER)

        fgs = wx.FlexGridSizer(cols=2, hgap=5, vgap=10)
        row.Add(fgs, 0, wx.ALL, 30)

        lbl = StaticText(self, -1, "X pos:")
        self.x = StaticText(self, -1, "00000")
        fgs.Add(lbl)
        fgs.Add(self.x)

        lbl = StaticText(self, -1, "Y pos:")
        self.y = StaticText(self, -1, "00000")
        fgs.Add(lbl)
        fgs.Add(self.y)


        lbl = StaticText(self, -1, "Left down:")
        self.lft = StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.lft)

        lbl = StaticText(self, -1, "Middle Down:")
        self.mid = StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.mid)

        lbl = StaticText(self, -1, "Right down:")
        self.rgt = StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.rgt)

        fgs = wx.FlexGridSizer(cols=2, hgap=5, vgap=10)
        row.Add(fgs, 0, wx.ALL, 30)

        lbl = StaticText(self, -1, "Control down:")
        self.ctrl = StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.ctrl)

        lbl = StaticText(self, -1, "Shift down:")
        self.shft = StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.shft)

        lbl = StaticText(self, -1, "Alt down:")
        self.alt = StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.alt)

        lbl = StaticText(self, -1, "Meta down:")
        self.meta = StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.meta)

        lbl = StaticText(self, -1, "Cmd down:")
        self.cmd = StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.cmd)
        
        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.OnTimer, self.timer)
        self.timer.Start(100)


    def OnTimer(self, evt):
        ms = wx.GetMouseState()
        self.x.SetLabel( str(ms.x) )
        self.y.SetLabel( str(ms.y) )
        
        self.lft.SetLabel( str(ms.leftDown) )
        self.mid.SetLabel( str(ms.middleDown) )
        self.rgt.SetLabel( str(ms.rightDown) )

        self.ctrl.SetLabel( str(ms.controlDown) )
        self.shft.SetLabel( str(ms.shiftDown) )
        self.alt.SetLabel( str(ms.altDown) )
        self.meta.SetLabel( str(ms.metaDown) )
        self.cmd.SetLabel( str(ms.cmdDown) )


    def ShutdownDemo(self):
        self.timer.Stop()
        del self.timer
        
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.GetMouseState</center></h2>

The mouse and modifier state can be polled with the wx.GetMouseState
function.  It returns an instance of a wx.MouseState object that
contains the current position of the mouse pointer in screen
coordinates, as well as boolean values indicating the up/down status
of the mouse buttons and the modifier keys.


</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

