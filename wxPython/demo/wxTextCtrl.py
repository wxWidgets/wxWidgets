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
        t1 = wxTextCtrl(self, 10, "Test it out and see", size=(125, -1))
        t1.SetInsertionPoint(0)
        EVT_TEXT(self, 10, self.EvtText)
        EVT_CHAR(t1, self.EvtChar)
        EVT_SET_FOCUS(t1, self.OnSetFocus)
        EVT_KILL_FOCUS(t1, self.OnKillFocus)
        EVT_WINDOW_DESTROY(t1, self.OnWindowDestroy)

        l2 = wxStaticText(self, -1, "Passsword")
        t2 = wxTextCtrl(self, 20, "", size=(125, -1), style=wxTE_PASSWORD)
        EVT_TEXT(self, 20, self.EvtText)

        l3 = wxStaticText(self, -1, "Multi-line")
        t3 = wxTextCtrl(self, 30,
                        "Here is a looooooooooooooong line of text set in the control.\n\n"
                        "The quick brown fox jumped over the lazy dog...",
                       size=(200, 100), style=wxTE_MULTILINE)
        t3.SetInsertionPoint(0)
        EVT_TEXT(self, 30, self.EvtText)
        b = wxButton(self, -1, "Test Replace")
        EVT_BUTTON(self, b.GetId(), self.OnTestReplace)
        b2 = wxButton(self, -1, "Test GetSelection")
        EVT_BUTTON(self, b2.GetId(), self.OnTestGetSelection)
        self.tc = t3

        l4 = wxStaticText(self, -1, "Rich Text")
        t4 = wxTextCtrl(self, 40, "If supported by the native control, this is red, and this is a different font.",
                        size=(200, 100), style=wxTE_MULTILINE|wxTE_RICH)
        t4.SetInsertionPoint(0)
        t4.SetStyle(44, 47, wxTextAttr("RED", "YELLOW"))

        points = t4.GetFont().GetPointSize()  # get the current size
        f = wxFont(points+3, wxROMAN, wxITALIC, wxBOLD, true)
##         print 'a1', sys.getrefcount(f)
##         t4.SetStyle(63, 77, wxTextAttr("BLUE", font=f))
        t4.SetStyle(63, 77, wxTextAttr("BLUE", wxNullColour, f))
##         print 'a2', sys.getrefcount(f)

        bsizer = wxBoxSizer(wxVERTICAL)
        bsizer.Add(b, 0, wxGROW)
        bsizer.Add(b2, 0, wxGROW)

        sizer = wxFlexGridSizer(cols=3, hgap=6, vgap=6)
        sizer.AddMany([ l1, t1, (0,0),
                        l2, t2, (0,0),
                        l3, t3, bsizer,
                        l4, t4, (0,0),
                        ])
        border = wxBoxSizer(wxVERTICAL)
        border.Add(sizer, 0, wxALL, 25)
        self.SetSizer(border)
        self.SetAutoLayout(true)


    def EvtText(self, event):
        self.log.WriteText('EvtText: %s\n' % event.GetString())


    def EvtChar(self, event):
        self.log.WriteText('EvtChar: %d\n' % event.GetKeyCode())
        event.Skip()


    def OnTestReplace(self, evt):
        self.tc.Replace(5, 9, "IS A")
        #self.tc.Remove(5, 9)

    def OnTestGetSelection(self, evt):
        start, end = self.tc.GetSelection()
        text = self.tc.GetValue()
        if wxPlatform == "__WXMSW__":  # This is why GetStringSelection was added
            text = string.replace(text, '\n', '\r\n')
        self.log.write("GetSelection(): (%d, %d)\n"
                       "\tGetStringSelection(): %s\n"
                       "\tSelectedText: %s\n" %
                       (start, end,
                        self.tc.GetStringSelection(),
                        repr(text[start:end])))


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------




overview = """\
"""
