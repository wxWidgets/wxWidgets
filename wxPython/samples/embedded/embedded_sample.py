from wxPython.wx import *
from wxPython.lib.PyCrust import shell, version

class MyPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)
        print parent

        text = wxStaticText(self, -1,
                            "Everything on this side of the splitter comes from Python.")
        text.SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD))

        intro = 'Welcome To PyCrust %s - The Flakiest Python Shell' % version.VERSION
        pycrust = shell.Shell(self, -1, introText=intro)
        #pycrust = wxTextCtrl(self, -1, intro)

        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(text, 0, wxEXPAND|wxALL, 10)
        sizer.Add(pycrust, 1, wxEXPAND|wxBOTTOM|wxLEFT|wxRIGHT, 10)

        self.SetSizer(sizer)
        #self.SetAutoLayout(true)
        #self.Layout()

