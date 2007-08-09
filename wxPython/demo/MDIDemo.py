
import  wx

# Importing ScrolledWindow demo to make use of the MyCanvas 
# class defined within.
import  ScrolledWindow 
import  images

SHOW_BACKGROUND = 1

#----------------------------------------------------------------------
ID_New  = wx.NewId()
ID_Exit = wx.NewId()
#----------------------------------------------------------------------

class MyParentFrame(wx.MDIParentFrame):
    def __init__(self):
        wx.MDIParentFrame.__init__(self, None, -1, "MDI Parent", size=(600,400))

        self.winCount = 0
        menu = wx.Menu()
        menu.Append(ID_New, "&New Window")
        menu.AppendSeparator()
        menu.Append(ID_Exit, "E&xit")

        menubar = wx.MenuBar()
        menubar.Append(menu, "&File")
        self.SetMenuBar(menubar)

        self.CreateStatusBar()

        self.Bind(wx.EVT_MENU, self.OnNewWindow, id=ID_New)
        self.Bind(wx.EVT_MENU, self.OnExit, id=ID_Exit)

        if SHOW_BACKGROUND:
            self.bg_bmp = images.getGridBGBitmap()
            self.GetClientWindow().Bind(
                wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground
                )


    def OnExit(self, evt):
        self.Close(True)


    def OnNewWindow(self, evt):
        self.winCount = self.winCount + 1
        win = wx.MDIChildFrame(self, -1, "Child Window: %d" % self.winCount)
        canvas = ScrolledWindow.MyCanvas(win)
        win.Show(True)


    def OnEraseBackground(self, evt):
        dc = evt.GetDC()

        if not dc:
            dc = wx.ClientDC(self.GetClientWindow())

        # tile the background bitmap
        sz = self.GetClientSize()
        w = self.bg_bmp.GetWidth()
        h = self.bg_bmp.GetHeight()
        x = 0
        
        while x < sz.width:
            y = 0

            while y < sz.height:
                dc.DrawBitmap(self.bg_bmp, x, y)
                y = y + h

            x = x + w


#----------------------------------------------------------------------

if __name__ == '__main__':
    class MyApp(wx.App):
        def OnInit(self):
            wx.InitAllImageHandlers()
            frame = MyParentFrame()
            frame.Show(True)
            self.SetTopWindow(frame)
            return True


    app = MyApp(False)
    app.MainLoop()



