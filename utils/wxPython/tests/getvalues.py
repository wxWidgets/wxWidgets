from wxPython.wx import *

class MyDlg(wxDialog):
    def __init__(self, parent):
        wxDialog.__init__(self, parent, -1, "This is a test",
                          wxDefaultPosition, wxSize(150, 150))

        self.text1 = wxTextCtrl(self, -1, "", wxPoint(10, 10), wxSize(120, -1))
        self.text2 = wxTextCtrl(self, -1, "", wxPoint(10, 40), wxSize(120, -1))

        wxButton(self, wxID_OK, "Okay", wxPoint(10,70)).SetDefault()
        wxButton(self, wxID_CANCEL, "Cancel", wxPoint(60, 70))


    def GetValues(self):
        val1 = self.text1.GetValue()
        val2 = self.text2.GetValue()
        return (val1, val2)




class MyApp(wxApp):
    def OnInit(self):

        frame = wxFrame(NULL, -1, "")
        wxButton(frame, 101, "test it", wxDefaultPosition, wxSize(50, 25))
        EVT_BUTTON(frame, 101, self.OnClick)
        frame.Fit()
        frame.Show(true)

        self.SetTopWindow(frame)
        return true

    def OnClick(self, event):
        dlg = MyDlg(NULL)
        if dlg.ShowModal() == wxID_OK:
            values = dlg.GetValues()
            print "Your values are: %s" % str(values)
        else:
            print "You canceled!"

        dlg.Destroy()



app = MyApp(0)
app.MainLoop()
