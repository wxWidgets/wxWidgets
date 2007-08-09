import wx

class MyFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1,
                          "Sub-menu Example")
        p = wx.Panel(self)
        menu = wx.Menu()

        submenu = wx.Menu()
        submenu.Append(-1, "Sub-item 1")
        submenu.Append(-1, "Sub-item 2")
        menu.AppendMenu(-1, "Sub-menu", submenu)
        
        menu.AppendSeparator()
        exit = menu.Append(-1, "Exit")
        self.Bind(wx.EVT_MENU, self.OnExit, exit)
                  
        menuBar = wx.MenuBar()
        menuBar.Append(menu, "Menu")
        self.SetMenuBar(menuBar)
        

    def OnExit(self, event):
        self.Close()
        

if __name__ == "__main__":
    app = wx.PySimpleApp()
    frame = MyFrame()
    frame.Show()
    app.MainLoop()

    
