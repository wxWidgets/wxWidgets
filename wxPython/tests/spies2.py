from wxPython.wx import *

#---------------------------------------------------------------------------

class TestComboBox(wxDialog):
    def __init__(self, parent):

        wxDialog.__init__(self, parent, -1, "This is a test",
                          wxDefaultPosition, wxSize(550, 250))
        self.s = ['aaaaaaaaaaaaaa',
                  'bbbbbbb',
                  'cccccccccccccccccccccccccccccccccccc',
                  'ddddddddddd',
                  'eeeeeeeeeee',
                  'ffffffff',
                  'gggggggggggggggggggggggg',
                  'hhhhhhhhhhhhhhhhhh',
                  'iiiiiiiiii']


        wxStaticText(self, -1, "This example uses the wxListBox control.",
                               wxPoint(45, 10))
        xwaarde = 35*12
        wxStaticText(self, -1, "Select one:", wxPoint(15, 50), wxSize(65, -1))

        dv = self.s[4]
        self.lb= wxComboBox(self, 50, dv, wxPoint(80, 50), wxSize(xwaarde, -1),
                            self.s, wxCB_DROPDOWN)

        wxButton(self, wxID_OK, "Okay", wxPoint(10,90)).SetDefault()
        wxButton(self, wxID_CANCEL, "Cancel", wxPoint(60, 90))


    def GetSelection(self,leer):

        val1 = self.lb.GetStringSelection()
        #leer.commit()
        return val1



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

        dlg = TestComboBox(NULL)

        if dlg.ShowModal() == wxID_OK:
            values = dlg.GetSelection(NULL)
            print "Your values are: %s" % str(values)
        else:
            print "You canceled!"

        dlg.Destroy()

app = MyApp(0)
app.MainLoop()


