
import wx
from wx.lib.expando import ExpandoTextCtrl, EVT_ETC_LAYOUT_NEEDED

#----------------------------------------------------------------------

class TestFrame(wx.Frame):
    def __init__(self, parent, log):
        wx.Frame.__init__(self, parent, title="Test ExpandoTextCtrl")
        self.log = log
        self.pnl = p = wx.Panel(self)
        self.eom = ExpandoTextCtrl(p, size=(250,-1),
                                   value="This control will expand as you type")
        self.Bind(EVT_ETC_LAYOUT_NEEDED, self.OnRefit, self.eom)

        # create some buttons and sizers to use in testing some
        # features and also the layout
        vBtnSizer = wx.BoxSizer(wx.VERTICAL)

        btn = wx.Button(p, -1, "Set MaxHeight")
        self.Bind(wx.EVT_BUTTON, self.OnSetMaxHeight, btn)
        vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)

        btn = wx.Button(p, -1, "Set Font")
        self.Bind(wx.EVT_BUTTON, self.OnSetFont, btn)
        vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)

        btn = wx.Button(p, -1, "Write Text")
        self.Bind(wx.EVT_BUTTON, self.OnWriteText, btn)
        vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)

        btn = wx.Button(p, -1, "Append Text")
        self.Bind(wx.EVT_BUTTON, self.OnAppendText, btn)
        vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)

        btn = wx.Button(p, -1, "Set Value")
        self.Bind(wx.EVT_BUTTON, self.OnSetValue, btn)
        vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)

        btn = wx.Button(p, -1, "Get Value")
        self.Bind(wx.EVT_BUTTON, self.OnGetValue, btn)
        vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)

        for x in range(3):
            btn = wx.Button(p, -1, " ")
            vBtnSizer.Add(btn, 0, wx.ALL|wx.EXPAND, 5)
            self.Bind(wx.EVT_BUTTON, self.OnOtherBtn, btn)

        hBtnSizer = wx.BoxSizer(wx.HORIZONTAL)
        for x in range(3):
            btn = wx.Button(p, -1, " ")
            hBtnSizer.Add(btn, 0, wx.ALL, 5)
            self.Bind(wx.EVT_BUTTON, self.OnOtherBtn, btn)

        sizer = wx.BoxSizer(wx.HORIZONTAL)
        col1 = wx.BoxSizer(wx.VERTICAL)
        col1.Add(self.eom, 0, wx.ALL, 10)
        col1.Add(hBtnSizer)
        sizer.Add(col1)
        sizer.Add(vBtnSizer)
        p.SetSizer(sizer)

        # Put the panel in a sizer for the frame so we can use self.Fit()
        frameSizer = wx.BoxSizer()
        frameSizer.Add(p, 1, wx.EXPAND)
        self.SetSizer(frameSizer)
        
        self.Fit()


    def OnRefit(self, evt):
        # The Expando control will redo the layout of the
        # sizer it belongs to, but sometimes this may not be
        # enough, so it will send us this event so we can do any
        # other layout adjustments needed.  In this case we'll
        # just resize the frame to fit the new needs of the sizer.
        self.Fit()


    def OnSetMaxHeight(self, evt):
        mh = self.eom.GetMaxHeight()
        dlg = wx.NumberEntryDialog(self, "", "Enter new max height:",
                                   "MaxHeight", mh, -1, 1000)
        if dlg.ShowModal() == wx.ID_OK:
            self.eom.SetMaxHeight(dlg.GetValue())
        dlg.Destroy()
        
        
    def OnSetFont(self, evt):
        dlg = wx.FontDialog(self, wx.FontData())
        dlg.GetFontData().SetInitialFont(self.eom.GetFont())
        if dlg.ShowModal() == wx.ID_OK:
            self.eom.SetFont(dlg.GetFontData().GetChosenFont())
        dlg.Destroy()
        
        
    def OnWriteText(self, evt):
        self.eom.WriteText("\nThis is a test...  Only a test.  If this had "
                           "been a real emergency you would have seen the "
                           "quick brown fox jump over the lazy dog.\n")
    
    def OnAppendText(self, evt):
        self.eom.AppendText("\nAppended text.")

    def OnSetValue(self, evt):
        self.eom.SetValue("A new value.")

    def OnGetValue(self, evt):
        self.log.write("-----------------\n" + self.eom.GetValue())

    def OnOtherBtn(self, evt):
        # just for testing...
        #print self.eom.numLines,
        self.eom._adjustCtrl()
        #print self.eom.numLines
        
#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, " Test ExpandoTextCtrl ", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        self.win = TestFrame(self, self.log)
        self.win.Show(True)

  
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>ExpandoTextCtrl</center></h2>

The ExpandoTextCtrl is a multi-line wx.TextCtrl that will
adjust its height on the fly as needed to accomodate the number of
lines needed to display the current content of the control.  It is
assumed that the width of the control will be a fixed value and
that only the height will be adjusted automatically.  If the
control is used in a sizer then the width should be set as part of
the initial or min size of the control.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

