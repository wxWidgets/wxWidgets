import wx

class SplitterExampleFrame(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title)
        self.MakeMenuBar()
        self.minpane = 0
        self.initpos = 0
        self.sp = wx.SplitterWindow(self)
        self.p1 = wx.Panel(self.sp, style=wx.SUNKEN_BORDER)
        self.p2 = wx.Panel(self.sp, style=wx.SUNKEN_BORDER)
        self.p1.SetBackgroundColour("pink")
        self.p2.SetBackgroundColour("sky blue")
        self.p1.Hide()
        self.p2.Hide()

        self.sp.Initialize(self.p1)
        
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGING,
                  self.OnSashChanging, self.sp)
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGED,
                  self.OnSashChanged, self.sp)


    def MakeMenuBar(self):
        menu = wx.Menu()
        item = menu.Append(-1, "Split horizontally")
        self.Bind(wx.EVT_MENU, self.OnSplitH, item)
        self.Bind(wx.EVT_UPDATE_UI, self.OnCheckCanSplit, item)
        item = menu.Append(-1, "Split vertically")
        self.Bind(wx.EVT_MENU, self.OnSplitV, item)
        self.Bind(wx.EVT_UPDATE_UI, self.OnCheckCanSplit, item)
        item = menu.Append(-1, "Unsplit")
        self.Bind(wx.EVT_MENU, self.OnUnsplit, item)
        self.Bind(wx.EVT_UPDATE_UI, self.OnCheckCanUnsplit, item)

        menu.AppendSeparator()
        item = menu.Append(-1, "Set initial sash position")
        self.Bind(wx.EVT_MENU, self.OnSetPos, item)
        item = menu.Append(-1, "Set minimum pane size")
        self.Bind(wx.EVT_MENU, self.OnSetMin, item)
      
        menu.AppendSeparator()
        item = menu.Append(wx.ID_EXIT, "E&xit")
        self.Bind(wx.EVT_MENU, self.OnExit, item)

        mbar = wx.MenuBar()
        mbar.Append(menu, "Splitter")
        self.SetMenuBar(mbar)

        
    def OnSashChanging(self, evt):
        print "OnSashChanging:", evt.GetSashPosition()
    
    def OnSashChanged(self, evt):
        print "OnSashChanged:", evt.GetSashPosition()
    

    def OnSplitH(self, evt):
        self.sp.SplitHorizontally(self.p1, self.p2, self.initpos)
    
    def OnSplitV(self, evt):
        self.sp.SplitVertically(self.p1, self.p2, self.initpos)

    def OnCheckCanSplit(self, evt):
        evt.Enable(not self.sp.IsSplit())

    def OnCheckCanUnsplit(self, evt):
        evt.Enable(self.sp.IsSplit())

    def OnUnsplit(self, evt):
        self.sp.Unsplit()
    
    def OnSetMin(self, evt):
        minpane = wx.GetNumberFromUser(
            "Enter the minimum pane size",
            "", "Minimum Pane Size", self.minpane,
            0, 1000, self)
        if minpane != -1:
            self.minpane = minpane
            self.sp.SetMinimumPaneSize(self.minpane)

    def OnSetPos(self, evt):
        initpos = wx.GetNumberFromUser(
            "Enter the initial sash position (to be used in the Split call)",
            "", "Initial Sash Position", self.initpos,
            -1000, 1000, self)
        if initpos != -1:
            self.initpos = initpos
    

    def OnExit(self, evt):
        self.Close()


app = wx.PySimpleApp(redirect=True)
frm = SplitterExampleFrame(None, "Splitter Example")
frm.SetSize((600,500))
frm.Show()
app.SetTopWindow(frm)
app.MainLoop()
