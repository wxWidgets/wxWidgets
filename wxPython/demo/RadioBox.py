
import  wx

#---------------------------------------------------------------------------

class TestRadioBox(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        #self.SetBackgroundColour(wx.BLUE)

        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight']

        sizer = wx.BoxSizer(wx.VERTICAL)

        rb = wx.RadioBox(
                self, -1, "wx.RadioBox", wx.DefaultPosition, wx.DefaultSize,
                sampleList, 2, wx.RA_SPECIFY_COLS
                )
        
        self.Bind(wx.EVT_RADIOBOX, self.EvtRadioBox, rb)
        #rb.SetBackgroundColour(wx.BLUE)
        rb.SetToolTip(wx.ToolTip("This is a ToolTip!"))
        #rb.SetLabel("wx.RadioBox")

        sizer.Add(rb, 0, wx.ALL, 20)

        rb = wx.RadioBox(
                self, -1, "", wx.DefaultPosition, wx.DefaultSize,
                sampleList, 3, wx.RA_SPECIFY_COLS | wx.NO_BORDER
                )
        
        self.Bind(wx.EVT_RADIOBOX, self.EvtRadioBox, rb)
        rb.SetToolTip(wx.ToolTip("This box has no label"))

        sizer.Add(rb, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM, 20)

        self.SetSizer(sizer)


    def EvtRadioBox(self, event):
        self.log.WriteText('EvtRadioBox: %d\n' % event.GetInt())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestRadioBox(nb, log)
    return win



overview = """\
A RadioBox is used to select one of a number of mutually exclusive
choices.  It is displayed as a vertical column or horizontal row of
labelled buttons, surrounded by a box that can optionally have a 
label.

"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

