

import sys, string

from wxPython.wx   import *
from wxPython.html import *

#----------------------------------------------------------------------


class TestPanel(wxPanel):
    def __init__(self, parent, id=-1, size=wxDefaultSize, bgcolor=None):
        wxPanel.__init__(self, parent, id, size=size)

        if bgcolor:
            self.SetBackgroundColour(bgcolor)

        wxStaticText(self, -1, 'Name:', wxPoint(10, 10))
        wxStaticText(self, -1, 'Email:', wxPoint(10, 40))

        self.name  = wxTextCtrl(self, -1, '', wxPoint(50, 10), wxSize(100, -1))
        self.email = wxTextCtrl(self, -1, '', wxPoint(50, 40), wxSize(100, -1))

        wxButton(self, 12121, 'Okay', wxPoint(50, 70))
        EVT_BUTTON(self, 12121, self.OnButton)


    def OnButton(self, event):
        name = self.name.GetValue()
        email = self.email.GetValue()
        dlg = wxMessageDialog(self,
                              'You entered:\n    %s\n    %s' % (name, email),
                              'Results', style = wxOK | wxICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()



#----------------------------------------------------------------------

class TestHtmlPanel(wxPanel):
    def __init__(self, parent, id=-1, size=wxDefaultSize):
        import About
        wxPanel.__init__(self, parent, id, size=size)
        self.html = wxHtmlWindow(self, -1, wxPoint(5,5), wxSize(400, 350))
        py_version = string.split(sys.version)[0]
        self.html.SetPage(About.MyAboutBox.text % (wx.__version__, py_version))
        ir = self.html.GetInternalRepresentation()
        self.html.SetSize( (ir.GetWidth()+5, ir.GetHeight()+5) )
        self.Fit()

#----------------------------------------------------------------------


