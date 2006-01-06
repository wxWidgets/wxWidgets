
import wx

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

        lbl = wx.StaticText(self, -1, "X pos:")
        self.x = wx.StaticText(self, -1, "00000")
        fgs.Add(lbl)
        fgs.Add(self.x)

        lbl = wx.StaticText(self, -1, "Y pos:")
        self.y = wx.StaticText(self, -1, "00000")
        fgs.Add(lbl)
        fgs.Add(self.y)


        lbl = wx.StaticText(self, -1, "Left down:")
        self.lft = wx.StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.lft)

        lbl = wx.StaticText(self, -1, "Middle Down:")
        self.mid = wx.StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.mid)

        lbl = wx.StaticText(self, -1, "Right down:")
        self.rgt = wx.StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.rgt)

        fgs = wx.FlexGridSizer(cols=2, hgap=5, vgap=10)
        row.Add(fgs, 0, wx.ALL, 30)

        lbl = wx.StaticText(self, -1, "Control down:")
        self.ctrl = wx.StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.ctrl)

        lbl = wx.StaticText(self, -1, "Shift down:")
        self.shft = wx.StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.shft)

        lbl = wx.StaticText(self, -1, "Alt down:")
        self.alt = wx.StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.alt)

        lbl = wx.StaticText(self, -1, "Meta down:")
        self.meta = wx.StaticText(self, -1, "False")
        fgs.Add(lbl)
        fgs.Add(self.meta)

        lbl = wx.StaticText(self, -1, "Cmd down:")
        self.cmd = wx.StaticText(self, -1, "False")
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
coordinants, as well as boolean values indicating the up/down status
of the mouse buttons and the modifier keys.


</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

