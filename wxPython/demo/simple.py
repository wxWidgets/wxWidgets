
#----------------------------------------------------------------------
# A very simple wxPython example.  Just a wxFrame, wxPanel,
# wxStaticText, wxButton, and a wxBoxSizer, but it shows the basic
# structure of any wxPython application.
#----------------------------------------------------------------------

from wxPython.wx import *

class MyFrame(wxFrame):
    """
    This is MyFrame.  It just shows a few controls on a wxPanel,
    and has a simple menu.
    """
    def __init__(self, parent, title):
        wxFrame.__init__(self, parent, -1, title, size=(350, 200))

        menuBar = wxMenuBar()
        menu = wxMenu()
        menu.Append(101, "E&xit\tAlt-X", "Exit demo")
        EVT_MENU(self, 101, self.OnButton)
        menuBar.Append(menu, "&File")
        self.SetMenuBar(menuBar)

        panel = wxPanel(self, -1)
        text = wxStaticText(panel, -1, "Hello World!")
        text.SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD))
        text.SetSize(text.GetBestSize())
        btn = wxButton(panel, -1, "Close")
        btn.SetDefault()

        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(text, 0, wxALL, 10)
        sizer.Add(btn, 0, wxALL, 10)
        panel.SetSizer(sizer)
        panel.SetAutoLayout(True)
        panel.Layout()

        EVT_BUTTON(self, btn.GetId(), self.OnButton)


    def OnButton(self, evt):
        """Event handler for the button click."""
        print "OnButton"
        self.Close()

app = wxPySimpleApp()
frame = MyFrame(None, "Simple wxPython App")
frame.Show(True)
app.MainLoop()

