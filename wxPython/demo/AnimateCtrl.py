
import wx
import wx.animate
from Main import opj

GIFNames = [
    'bitmaps/AG00178_.gif',
    'bitmaps/BD13656_.gif',  
    'bitmaps/AG00185_.gif',
    'bitmaps/AG00039_.gif',
    'bitmaps/AG00183_.gif',
    'bitmaps/AG00028_.gif',
    ]

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        sizer = wx.FlexGridSizer(2,3,5,5)
        for name in GIFNames:
            ani = wx.animate.Animation(opj(name))
            ctrl = wx.animate.AnimationCtrl(self, -1, ani)
            ctrl.SetUseWindowBackgroundColour()
            ctrl.Play()
            sizer.Add(ctrl, 0, wx.ALL, 10)

        border = wx.BoxSizer()
        border.Add(sizer, 1, wx.EXPAND|wx.ALL, 20)
        self.SetSizer(border)
        

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.animate.AnimationCtrl</center></h2>

wx.animate.AnimationCtrl is like a wx.StaticBitmap but is able to
display an animation by extracing frames from a multi-images GIF file.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

