
#----------------------------------------------------------------------
# A very simple wxPython example.  Just a wxFrame, wxPanel,
# wxStaticText, wxButton, and a wxBoxSizer, but it shows the basic
# structure of any wxPython application.
#----------------------------------------------------------------------

import wx

class MyFrame(wx.Frame):
    """
    This is MyFrame.  It just shows a few controls on a wxPanel,
    and has a simple menu.
    """
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, -1, title, size=(350, 200))

        menuBar = wx.MenuBar()
        menu = wx.Menu()
        menu.Append(101, "E&xit\tAlt-X", "Exit demo")
        wx.EVT_MENU(self, 101, self.OnButton)
        menuBar.Append(menu, "&File")
        self.SetMenuBar(menuBar)

        panel = wx.Panel(self, -1)
        text = wx.StaticText(panel, -1, "Hello World!")
        text.SetFont(wx.Font(12, wx.SWISS, wx.NORMAL, wx.BOLD))
        text.SetSize(text.GetBestSize())
        btn = wx.Button(panel, -1, "Close")
        btn.SetDefault()

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(text, 0, wx.ALL, 10)
        sizer.Add(btn, 0, wx.ALL, 10)
        panel.SetSizer(sizer)
        panel.SetAutoLayout(True)
        panel.Layout()

        wx.EVT_BUTTON(self, btn.GetId(), self.OnButton)


    def OnButton(self, evt):
        """Event handler for the button click."""
        print "OnButton"
        self.Close()


app = wx.PySimpleApp()
frame = MyFrame(None, "Simple wxPython App")
frame.Show(True)
app.MainLoop()

