
import  sys
import  wx

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def OnSetFocus(self, evt):
        print "OnSetFocus"
        evt.Skip()
    def OnKillFocus(self, evt):
        print "OnKillFocus"
        evt.Skip()
    def OnWindowDestroy(self, evt):
        print "OnWindowDestroy"
        evt.Skip()


    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        l1 = wx.StaticText(self, -1, "wx.TextCtrl")
        t1 = wx.TextCtrl(self, -1, "Test it out and see", size=(125, -1))
        wx.CallAfter(t1.SetInsertionPoint, 0)
        self.tc1 = t1

        self.Bind(wx.EVT_TEXT, self.EvtText, t1)
        t1.Bind(wx.EVT_CHAR, self.EvtChar)
        t1.Bind(wx.EVT_SET_FOCUS, self.OnSetFocus)
        t1.Bind(wx.EVT_KILL_FOCUS, self.OnKillFocus)
        t1.Bind(wx.EVT_WINDOW_DESTROY, self.OnWindowDestroy)

        l2 = wx.StaticText(self, -1, "Password")
        t2 = wx.TextCtrl(self, -1, "", size=(125, -1), style=wx.TE_PASSWORD)
        self.Bind(wx.EVT_TEXT, self.EvtText, t2)

        l3 = wx.StaticText(self, -1, "Multi-line")
        t3 = wx.TextCtrl(self, -1,
                        "Here is a looooooooooooooong line of text set in the control.\n\n"
                        "The quick brown fox jumped over the lazy dog...",
                       size=(200, 100), style=wx.TE_MULTILINE|wx.TE_PROCESS_ENTER)

        t3.SetInsertionPoint(0)
        self.Bind(wx.EVT_TEXT, self.EvtText, t3)
        self.Bind(wx.EVT_TEXT_ENTER, self.EvtTextEnter, t3)
        
        b = wx.Button(self, -1, "Test Replace")
        self.Bind(wx.EVT_BUTTON, self.OnTestReplace, b)
        b2 = wx.Button(self, -1, "Test GetSelection")
        self.Bind(wx.EVT_BUTTON, self.OnTestGetSelection, b2)
        b3 = wx.Button(self, -1, "Test WriteText")
        self.Bind(wx.EVT_BUTTON, self.OnTestWriteText, b3)
        self.tc = t3


        l4 = wx.StaticText(self, -1, "Rich Text")
        t4 = wx.TextCtrl(self, -1, "If supported by the native control, this is red, and this is a different font.",
                        size=(200, 100), style=wx.TE_MULTILINE|wx.TE_RICH2)
        t4.SetInsertionPoint(0)
        t4.SetStyle(44, 47, wx.TextAttr("RED", "YELLOW"))
        points = t4.GetFont().GetPointSize()  # get the current size
        f = wx.Font(points+3, wx.ROMAN, wx.ITALIC, wx.BOLD, True)
        t4.SetStyle(63, 77, wx.TextAttr("BLUE", wx.NullColour, f))

        l5 = wx.StaticText(self, -1, "Test Positions")
        t5 = wx.TextCtrl(self, -1, "0123456789\n" * 5, size=(200, 100),
                        style = wx.TE_MULTILINE
                        #| wx.TE_RICH
                        | wx.TE_RICH2
                        )
        t5.Bind(wx.EVT_LEFT_DOWN, self.OnT5LeftDown)
        self.t5 = t5

        space = 4
        bsizer = wx.BoxSizer(wx.VERTICAL)
        bsizer.Add(b, 0, wx.GROW|wx.ALL, space)
        bsizer.Add(b2, 0, wx.GROW|wx.ALL, space)
        bsizer.Add(b3, 0, wx.GROW|wx.ALL, space)

        sizer = wx.FlexGridSizer(cols=3, hgap=space, vgap=space)
        sizer.AddMany([ l1, t1, (0,0),
                        l2, t2, (0,0),
                        l3, t3, bsizer,
                        l4, t4, (0,0),
                        l5, t5, (0,0),
                        ])
        border = wx.BoxSizer(wx.VERTICAL)
        border.Add(sizer, 0, wx.ALL, 25)
        self.SetSizer(border)
        self.SetAutoLayout(True)


    def EvtText(self, event):
        self.log.WriteText('EvtText: %s\n' % event.GetString())

    def EvtTextEnter(self, event):
        self.log.WriteText('EvtTextEnter\n')
        event.Skip()

    def EvtChar(self, event):
        self.log.WriteText('EvtChar: %d\n' % event.GetKeyCode())
        event.Skip()


    def OnTestReplace(self, evt):
        self.tc.Replace(5, 9, "IS A")
        #self.tc.Remove(5, 9)

    def OnTestWriteText(self, evt):
        self.tc.WriteText("TEXT")

    def OnTestGetSelection(self, evt):
        start, end = self.tc.GetSelection()
        text = self.tc.GetValue()
        if wx.Platform == "__WXMSW__":  # This is why GetStringSelection was added
            text = text.replace('\n', '\r\n')

        self.log.write("multi-line GetSelection(): (%d, %d)\n"
                       "\tGetStringSelection(): %s\n"
                       "\tSelectedText: %s\n" %
                       (start, end,
                        self.tc.GetStringSelection(),
                        repr(text[start:end])))

        start, end = self.tc1.GetSelection()
        text = self.tc1.GetValue()

        if wx.Platform == "__WXMSW__":  # This is why GetStringSelection was added
            text = text.replace('\n', '\r\n')

        self.log.write("single-line GetSelection(): (%d, %d)\n"
                       "\tGetStringSelection(): %s\n"
                       "\tSelectedText: %s\n" %
                       (start, end,
                        self.tc1.GetStringSelection(),
                        repr(text[start:end])))


    def OnT5LeftDown(self, evt):
        evt.Skip()
        wx.CallAfter(self.LogT5Position, evt)

    def LogT5Position(self, evt):
        text = self.t5.GetValue()
        ip = self.t5.GetInsertionPoint()
        lp = self.t5.GetLastPosition()
        self.log.write("LogT5Position:\n"
                       "\tGetInsertionPoint:\t%d\n"
                       "\ttext[insertionpoint]:\t%s\n"
                       "\tGetLastPosition:\t%d\n"
                       "\tlen(text):\t\t%d\n"
                       % (ip, text[ip], lp, len(text)))


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------


overview = """\
A TextCtrl allows text to be displayed and (possibly) edited. It may be single 
line or multi-line, support styles or not, be read-only or not, and even supports
text masking for such things as passwords.


"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

