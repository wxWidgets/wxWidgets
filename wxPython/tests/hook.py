from wxPython.wx import *

class MyFrame(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, id, title,
                         wxDefaultPosition, wxSize(400, 400))
        self.panel = wxPanel(self, -1)
        wxStaticText(self.panel, -1, "wxTextCtrl", wxPoint(5, 25),
                     wxSize(75, 20))
        self.tc = wxTextCtrl(self.panel, 10, "", wxPoint(80, 25),
                             wxSize(200, 30))
        EVT_CHAR_HOOK(self, self.OnCharHook)
        #EVT_CHAR_HOOK(self.tc, self.OnCharHook)
        EVT_CHAR(self, self.OnChar)
        self.panel.Layout()
        return

    def OnCloseWindow(self, event):
        self.Destroy()
        return

    def OnChar(self, event):
        print "OnChar: %d '%c'" % (event.KeyCode(), chr(event.KeyCode()))
        event.Skip()
        return

    def OnCharHook(self, event):
        print "OnCharHook: %d" % event.KeyCode()
        event.Skip()
        return


class MyApp(wxApp):
    def OnInit(self):
        frame = MyFrame(None, -1, 'CharHook Test')
        frame.Show(1)
        self.SetTopWindow(frame)
        return 1


app = MyApp(0)
app.MainLoop()



