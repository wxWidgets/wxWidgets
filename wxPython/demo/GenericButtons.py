
import  wx
import  wx.lib.buttons  as  buttons

import images

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log
        ##self.SetBackgroundColour("sky blue")

        sizer = wx.FlexGridSizer(1, 3, 20, 20)

        # A regular button, selected as the default button
        b = wx.Button(self, -1, "A real button")
        b.SetDefault()
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        sizer.Add(b)

        # Same thing, but NOT set as the default button
        b = wx.Button(self, -1, "non-default")
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        sizer.Add(b)
        sizer.Add((10,10))

        # Plain old text button based off GenButton()
        b = buttons.GenButton(self, -1, 'Hello')
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        sizer.Add(b)

        # Plain old text button, disabled.
        b = buttons.GenButton(self, -1, 'disabled')
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        b.Enable(False)
        sizer.Add(b)

        # This time, we let the botton be as big as it can be.
        # Also, this one is fancier, with custom colors and bezel size.
        b = buttons.GenButton(self, -1, 'bigger')
        self.Bind(wx.EVT_BUTTON, self.OnBiggerButton, b)
        b.SetFont(wx.Font(20, wx.SWISS, wx.NORMAL, wx.BOLD, False))
        b.SetBezelWidth(5)
        b.SetMinSize(wx.DefaultSize)
        b.SetBackgroundColour("Navy")
        b.SetForegroundColour(wx.WHITE)
        b.SetToolTipString("This is a BIG button...")
        # let the sizer set best size
        sizer.Add(b, flag=wx.ADJUST_MINSIZE) 

        # An image button
        bmp = images.getTest2Bitmap()
        b = buttons.GenBitmapButton(self, -1, bmp)
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        sizer.Add(b)

        # An image button, disabled.
        bmp = images.getTest2Bitmap()
        b = buttons.GenBitmapButton(self, -1, bmp)
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        sizer.Add(b)
        b.Enable(False)

        # An image button, using a mask to get rid of the
        # undesireable part of the image
        b = buttons.GenBitmapButton(self, -1, None)
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        bmp = images.getBulb1Bitmap()
        mask = wx.Mask(bmp, wx.BLUE)
        bmp.SetMask(mask)
        b.SetBitmapLabel(bmp)
        bmp = images.getBulb2Bitmap()
        mask = wx.Mask(bmp, wx.BLUE)
        bmp.SetMask(mask)
        b.SetBitmapSelected(bmp)
        b.SetInitialSize()
        sizer.Add(b)

        # A toggle button
        b = buttons.GenToggleButton(self, -1, "Toggle Button")
        self.Bind(wx.EVT_BUTTON, self.OnToggleButton, b)
        sizer.Add(b)

        # An image toggle button
        b = buttons.GenBitmapToggleButton(self, -1, None)
        self.Bind(wx.EVT_BUTTON, self.OnToggleButton, b)
        bmp = images.getBulb1Bitmap()
        mask = wx.Mask(bmp, wx.BLUE)
        bmp.SetMask(mask)
        b.SetBitmapLabel(bmp)
        bmp = images.getBulb2Bitmap()
        mask = wx.Mask(bmp, wx.BLUE)
        bmp.SetMask(mask)
        b.SetBitmapSelected(bmp)
        b.SetToggle(True)
        b.SetInitialSize()
        sizer.Add(b)

        # A bitmap button with text.
        b = buttons.GenBitmapTextButton(self, -1, None, "Bitmapped Text", size = (200, 45))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        bmp = images.getBulb1Bitmap()
        mask = wx.Mask(bmp, wx.BLUE)
        bmp.SetMask(mask)
        b.SetBitmapLabel(bmp)
        bmp = images.getBulb2Bitmap()
        mask = wx.Mask(bmp, wx.BLUE)
        bmp.SetMask(mask)
        b.SetBitmapSelected(bmp)
        b.SetUseFocusIndicator(False)
        b.SetInitialSize()
        sizer.Add(b)


        # a flat text button
        b = buttons.GenButton(self, -1, 'Flat buttons too!', style=wx.BORDER_NONE)
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        sizer.Add(b, flag=wx.ALIGN_CENTER_VERTICAL)

        # A flat image button
        bmp = images.getTest2Bitmap()
        bmp.SetMaskColour("blue")
        b = buttons.GenBitmapButton(self, -1, bmp, style=wx.BORDER_NONE)
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        sizer.Add(b)
        ##b.SetBackgroundColour("sky blue")
        ##b.SetBackgroundColour("pink")

        b = buttons.ThemedGenButton(self, -1, 'Drawn with native renderer')
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)
        sizer.Add(b)
        

        border = wx.BoxSizer(wx.VERTICAL)
        border.Add(sizer, 0, wx.ALL, 25)
        self.SetSizer(border)


    def OnButton(self, event):
        self.log.WriteText("Button Clicked: %d\n" % event.GetId())


    def OnBiggerButton(self, event):
        self.log.WriteText("Bigger Button Clicked: %d\n" % event.GetId())
        b = event.GetEventObject()
        txt = "big " + b.GetLabel()
        b.SetLabel(txt)
        self.GetSizer().Layout()


    def OnToggleButton(self, event):
        msg = (event.GetIsDown() and "on") or "off"
        self.log.WriteText("Button %d Toggled: %s\n" % (event.GetId(), msg))



#----------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#----------------------------------------------------------------------


overview = buttons.__doc__

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

