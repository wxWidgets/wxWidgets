import wx

class MyFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1,
                          "Toggle Items Example")
        p = wx.Panel(self)
        menuBar = wx.MenuBar()
        menu = wx.Menu()
        exit = menu.Append(-1, "Exit")
        self.Bind(wx.EVT_MENU, self.OnExit, exit)
        menuBar.Append(menu, "Menu")

        menu = wx.Menu()
        menu.AppendCheckItem(-1, "Check Item 1")
        menu.AppendCheckItem(-1, "Check Item 2")
        menu.AppendCheckItem(-1, "Check Item 3")
        menu.AppendSeparator()
        menu.AppendRadioItem(-1, "Radio Item 1")
        menu.AppendRadioItem(-1, "Radio Item 2")
        menu.AppendRadioItem(-1, "Radio Item 3")
        menuBar.Append(menu, "Toggle Items")
        
        self.SetMenuBar(menuBar)
        
    def OnExit(self, event):
        self.Close()
        

if __name__ == "__main__":
    app = wx.PySimpleApp()
    frame = MyFrame()
    frame.Show()
    app.MainLoop()

    
