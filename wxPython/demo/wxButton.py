# 11/15/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx
import  images

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1,
                         style=wx.NO_FULL_REPAINT_ON_RESIZE)
        self.log = log

        b = wx.Button(self, 10, "Default Button", (20, 20))
        self.Bind(EVT_BUTTON, self.OnClick, b)
        b.SetDefault()
        b.SetSize(b.GetBestSize())

        b = wx.Button(self, 20, "HELLO AGAIN!", (20, 80), (120, 45))
        self.Bind(EVT_BUTTON, self.OnClick, b)
        b.SetToolTipString("This is a Hello button...")

        if 0:  # a test case for catching wx.PyAssertionError

            #wx.GetApp().SetAssertMode(wx.PYAPP_ASSERT_SUPPRESS)
            #wx.GetApp().SetAssertMode(wx.PYAPP_ASSERT_EXCEPTION)
            #wx.GetApp().SetAssertMode(wx.PYAPP_ASSERT_DIALOG)
            #wx.GetApp().SetAssertMode(wx.PYAPP_ASSERT_EXCEPTION | wx.PYAPP_ASSERT_DIALOG)

            try:
                bmp = wx.Bitmap("nosuchfile.bmp", wx.BITMAP_TYPE_BMP)
                mask = wx.MaskColour(bmp, wx.BLUE)
            except wx.PyAssertionError:
                self.log.write("Caught wx.PyAssertionError!  I will fix the problem.\n")
                bmp = images.getTest2Bitmap()
                mask = wx.MaskColour(bmp, wx.BLUE)
        else:
            bmp = images.getTest2Bitmap()
            mask = wx.MaskColour(bmp, wx.BLUE)

        bmp.SetMask(mask)
        wx.BitmapButton(self, 30, bmp, (160, 20),
                       (bmp.GetWidth()+10, bmp.GetHeight()+10))
        self.Bind(wx.EVT_BUTTON, self.OnClick, id=30)


    def OnClick(self, event):
        self.log.write("Click! (%d)\n" % event.GetId())
        ##wxLogDebug("debug message")


## wxLog_SetLogLevel(wxLOG_Message)  # ignore everything above wxLOG_Message

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
    run.main(['', os.path.basename(sys.argv[0])])

