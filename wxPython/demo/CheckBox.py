
import  wx

#---------------------------------------------------------------------------

class TestCheckBox(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        st = wx.StaticText(self, -1, "This example demonstrates the wx.CheckBox control.")#, (10, 10))

        cb1 = wx.CheckBox(self, -1, "Apples")#, (65, 40), (150, 20), wx.NO_BORDER)
        cb2 = wx.CheckBox(self, -1, "Oranges")#, (65, 60), (150, 20), wx.NO_BORDER)
        cb2.SetValue(True)
        cb3 = wx.CheckBox(self, -1, "Pears")#, (65, 80), (150, 20), wx.NO_BORDER)
        
        cb4 = wx.CheckBox(self, -1, "3-state checkbox",
                          style=wx.CHK_3STATE|wx.CHK_ALLOW_3RD_STATE_FOR_USER)
        cb5 = wx.CheckBox(self, -1, "Align Right", style=wx.ALIGN_RIGHT)
        

        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, cb1)
        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, cb2)
        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, cb3)
        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, cb4)
        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, cb5)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.AddMany( [ cb1,
                         cb2,
                         cb3,
                         (20,20),
                         cb4,
                         (20,20),
                         cb5
                         ])

        border = wx.BoxSizer(wx.VERTICAL)
        border.Add(st, 0, wx.ALL, 15)
        border.Add(sizer, 0, wx.LEFT, 50)
        self.SetSizer(border)
        

    def EvtCheckBox(self, event):
        self.log.write('EvtCheckBox: %d\n' % event.IsChecked())
        cb = event.GetEventObject()
        if cb.Is3State():
            self.log.write("\t3StateValue: %s\n" % cb.Get3StateValue())
            

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestCheckBox(nb, log)
    return win

#---------------------------------------------------------------------------


overview = """\
A checkbox is a labelled box which is either on (checkmark is visible) or off 
(no checkmark).

"""

#---------------------------------------------------------------------------

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

