import wx

class ScrollbarFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, 'Scrollbar Example', 
                size=(300, 200))
        self.scroll = wx.ScrolledWindow(self, -1)
        self.scroll.SetScrollbars(1, 1, 600, 400)
        self.button = wx.Button(self.scroll, -1, "Scroll Me", pos=(50, 20))
        self.Bind(wx.EVT_BUTTON,  self.OnClickTop, self.button)
        self.button2 = wx.Button(self.scroll, -1, "Scroll Back", pos=(500, 350))
        self.Bind(wx.EVT_BUTTON, self.OnClickBottom, self.button2)

    def OnClickTop(self, event):
        self.scroll.Scroll(600, 400)
        
    def OnClickBottom(self, event):
        self.scroll.Scroll(1, 1)
        
if __name__ == '__main__':
    app = wx.PySimpleApp()
    frame = ScrollbarFrame()
    frame.Show()
    app.MainLoop()


