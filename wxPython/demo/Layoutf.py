
import  wx
import  wx.lib.layoutf  as layoutf

#---------------------------------------------------------------------------

class TestLayoutf(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)

        self.SetAutoLayout(True)
        self.Bind(wx.EVT_BUTTON, self.OnButton)

        self.panelA = wx.Window(self, -1, style=wx.SIMPLE_BORDER)
        self.panelA.SetBackgroundColour(wx.BLUE)
        self.panelA.SetConstraints(
            layoutf.Layoutf('t=t10#1;l=l10#1;b=b10#1;r%r50#1',(self,))
            )

        self.panelB = wx.Window(self, -1, style=wx.SIMPLE_BORDER)
        self.panelB.SetBackgroundColour(wx.RED)
        self.panelB.SetConstraints(
            layoutf.Layoutf('t=t10#1;r=r10#1;b%b30#1;l>10#2', (self,self.panelA))
            )

        self.panelC = wx.Window(self, -1, style=wx.SIMPLE_BORDER)
        self.panelC.SetBackgroundColour(wx.WHITE)
        self.panelC.SetConstraints(
            layoutf.Layoutf('t_10#3;r=r10#1;b=b10#1;l>10#2', (self,self.panelA,self.panelB))
            )

        b = wx.Button(self.panelA, -1, ' Panel A ')
        b.SetConstraints(layoutf.Layoutf('X=X#1;Y=Y#1;h*;w%w50#1', (self.panelA,)))

        b = wx.Button(self.panelB, -1, ' Panel B ')
        b.SetConstraints(layoutf.Layoutf('t=t2#1;r=r4#1;h*;w*', (self.panelB,)))

        self.panelD = wx.Window(self.panelC, -1, style=wx.SIMPLE_BORDER)
        self.panelD.SetBackgroundColour(wx.GREEN)
        self.panelD.SetConstraints(
            layoutf.Layoutf('b%h50#1;r%w50#1;h=h#2;w=w#2', (self.panelC, b))
            )

        b = wx.Button(self.panelC, -1, ' Panel C ')
        b.SetConstraints(layoutf.Layoutf('t_#1;l>#1;h*;w*', (self.panelD,)))

        wx.StaticText(self.panelD, -1, "Panel D", (4, 4)).SetBackgroundColour(wx.GREEN)

    def OnButton(self, event):
        wx.Bell()



#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestLayoutf(nb)
    return win

#---------------------------------------------------------------------------

overview = layoutf.Layoutf.__doc__

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

