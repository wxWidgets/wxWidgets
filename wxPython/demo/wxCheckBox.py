# 11/15/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx

#---------------------------------------------------------------------------

class TestCheckBox(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        wx.StaticText(self, -1, "This example uses the wxCheckBox control.", (10, 10))

        cID = wx.NewId()
        cb1 = wx.CheckBox(self, cID,   "  Apples", (65, 40), (150, 20), wx.NO_BORDER)
        cb2 = wx.CheckBox(self, cID+1, "  Oranges", (65, 60), (150, 20), wx.NO_BORDER)
        cb2.SetValue(True)
        cb3 = wx.CheckBox(self, cID+2, "  Pears", (65, 80), (150, 20), wx.NO_BORDER)

        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, cb1)
        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, cb2)
        self.Bind(wx.EVT_CHECKBOX, self.EvtCheckBox, cb3)

    def EvtCheckBox(self, event):
        self.log.WriteText('EvtCheckBox: %d\n' % event.IsChecked())

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestCheckBox(nb, log)
    return win

#---------------------------------------------------------------------------





overview = """\
A checkbox is a labelled box which is either on (checkmark is visible) or off (no checkmark).

"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

