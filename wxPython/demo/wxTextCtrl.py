import sys
from wxPython.wx import *

#---------------------------------------------------------------------------

class TestPanel(wxPanel):
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
        wxPanel.__init__(self, parent, -1)
        self.log = log

        l1 = wxStaticText(self, -1, "wxTextCtrl")
        t1 = wxTextCtrl(self, -1, "Test it out and see", size=(125, -1))
        t1.SetInsertionPoint(0)
        self.tc1 = t1
        EVT_TEXT(self, t1.GetId(), self.EvtText)
        EVT_CHAR(t1, self.EvtChar)
        EVT_SET_FOCUS(t1, self.OnSetFocus)
        EVT_KILL_FOCUS(t1, self.OnKillFocus)
        EVT_WINDOW_DESTROY(t1, self.OnWindowDestroy)

        l2 = wxStaticText(self, -1, "Passsword")
        t2 = wxTextCtrl(self, -1, "", size=(125, -1), style=wxTE_PASSWORD)
        EVT_TEXT(self, t2.GetId(), self.EvtText)

        l3 = wxStaticText(self, -1, "Multi-line")
        t3 = wxTextCtrl(self, -1,
                        "Here is a looooooooooooooong line of text set in the control.\n\n"
                        "The quick brown fox jumped over the lazy dog...",
                       size=(200, 100), style=wxTE_MULTILINE)
        t3.SetInsertionPoint(0)
        EVT_TEXT(self, t3.GetId(), self.EvtText)
        b = wxButton(self, -1, "Test Replace")
        EVT_BUTTON(self, b.GetId(), self.OnTestReplace)
        b2 = wxButton(self, -1, "Test GetSelection")
        EVT_BUTTON(self, b2.GetId(), self.OnTestGetSelection)
        b3 = wxButton(self, -1, "Test WriteText")
        EVT_BUTTON(self, b3.GetId(), self.OnTestWriteText)
        self.tc = t3
        b4 = wxButton(self, -1, "Test Simulated Event")
        EVT_BUTTON(self, b4.GetId(), self.OnTestEvent)


        l4 = wxStaticText(self, -1, "Rich Text")
        t4 = wxTextCtrl(self, -1, "If supported by the native control, this is red, and this is a different font.",
                        size=(200, 100), style=wxTE_MULTILINE|wxTE_RICH2)
        t4.SetInsertionPoint(0)
        t4.SetStyle(44, 47, wxTextAttr("RED", "YELLOW"))
        points = t4.GetFont().GetPointSize()  # get the current size
        f = wxFont(points+3, wxROMAN, wxITALIC, wxBOLD, True)
        t4.SetStyle(63, 77, wxTextAttr("BLUE", wxNullColour, f))

        l5 = wxStaticText(self, -1, "Test Positions")
        t5 = wxTextCtrl(self, -1, "0123456789\n" * 5, size=(200, 100),
                        style = wxTE_MULTILINE
                        #| wxTE_RICH
                        | wxTE_RICH2
                        )
        EVT_LEFT_DOWN(t5, self.OnT5LeftDown)
        self.t5 = t5


        bsizer = wxBoxSizer(wxVERTICAL)
        bsizer.Add(b, 0, wxGROW|wxALL, 4)
        bsizer.Add(b2, 0, wxGROW|wxALL, 4)
        bsizer.Add(b3, 0, wxGROW|wxALL, 4)
        bsizer.Add(b4, 0, wxGROW|wxALL, 4)

        sizer = wxFlexGridSizer(cols=3, hgap=6, vgap=6)
        sizer.AddMany([ l1, t1, (0,0),
                        l2, t2, (0,0),
                        l3, t3, bsizer,
                        l4, t4, (0,0),
                        l5, t5, (0,0),
                        ])
        border = wxBoxSizer(wxVERTICAL)
        border.Add(sizer, 0, wxALL, 25)
        self.SetSizer(border)
        self.SetAutoLayout(True)


    def EvtText(self, event):
        self.log.WriteText('EvtText: %s\n' % event.GetString())


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
        if wxPlatform == "__WXMSW__":  # This is why GetStringSelection was added
            text = text.replace('\n', '\r\n')
        self.log.write("GetSelection(): (%d, %d)\n"
                       "\tGetStringSelection(): %s\n"
                       "\tSelectedText: %s\n" %
                       (start, end,
                        self.tc.GetStringSelection(),
                        repr(text[start:end])))

    def OnT5LeftDown(self, evt):
        evt.Skip()
        wxCallAfter(self.LogT5Position, evt)

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


    def OnTestEvent(self, evt):
        ke = wxKeyEvent(wxEVT_CHAR)
        ke.SetEventObject(self.tc1)
        ke.SetId(self.tc1.GetId())
        ke.m_keyCode = ord('A')
        self.tc1.GetEventHandler().ProcessEvent(ke)


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------




overview = """\
"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

