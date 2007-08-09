import wx
print wx.VERSION

ID_ITEM1 = wx.NewId()
ID_ITEM2 = wx.NewId()



class TestPanelBase(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, style=wx.BORDER_SUNKEN)
        self.item1 = False
        self.item2 = True
        self.Bind(wx.EVT_CONTEXT_MENU, self.OnContextMenu)
        self.Bind(wx.EVT_MENU, self.OnItem1, id=ID_ITEM1)
        self.Bind(wx.EVT_MENU, self.OnItem2, id=ID_ITEM2)
        wx.StaticText(self, -1, self.__doc__.strip(), (0,0))

    def OnItem1(self, evt):
        print "OnItem1"
        self.item1 =  not self.item1
        
    def OnItem2(self, evt):
        print "OnItem2"
        self.item2 =  not self.item2


class TestPanel1(TestPanelBase):
    """
    Pre-create and reuse the menu
    """
    def __init__(self, parent):
        TestPanelBase.__init__(self, parent)
        self.menu = wx.Menu()
        self.menu.Append(ID_ITEM1, "Item 1", kind=wx.ITEM_CHECK)
        self.menu.Append(ID_ITEM2, "Item 2", kind=wx.ITEM_CHECK)
        self.menu.Check(ID_ITEM1, self.item1)
        self.menu.Check(ID_ITEM2, self.item2)

    def OnContextMenu(self, evt):
        self.PopupMenu(self.menu)


class TestPanel2(TestPanelBase):
    """
    This one recreates the menu and sets the checks each time
    """
    def __init__(self, parent):
        TestPanelBase.__init__(self, parent)

    def OnContextMenu(self, evt):
        menu = wx.Menu()
        menu.Append(ID_ITEM1, "Item 1", kind=wx.ITEM_CHECK)
        menu.Append(ID_ITEM2, "Item 2", kind=wx.ITEM_CHECK)
        menu.Check(ID_ITEM1, self.item1)
        menu.Check(ID_ITEM2, self.item2)
        self.PopupMenu(menu)
        menu.Destroy()


class TestPanel3(TestPanelBase):
    """
    Use an EVT_UPDATE_UI handler to set the checks
    """
    def __init__(self, parent):
        TestPanelBase.__init__(self, parent)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateUI_Item1, id=ID_ITEM1)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateUI_Item2, id=ID_ITEM2)

    def OnContextMenu(self, evt):
        menu = wx.Menu()
        menu.Append(ID_ITEM1, "Item 1", kind=wx.ITEM_CHECK)
        menu.Append(ID_ITEM2, "Item 2", kind=wx.ITEM_CHECK)
        self.PopupMenu(menu)
        menu.Destroy()

    def OnUpdateUI_Item1(self, evt):
        print "OnUpdateUI_Item1"
        evt.Check(self.item1)

    def OnUpdateUI_Item2(self, evt):
        print "OnUpdateUI_Item2"
        evt.Check(self.item2)


app = wx.App(False)
frm = wx.Frame(None, title="Right-click on a panel...")
pnl1 = TestPanel1(frm)
pnl2 = TestPanel2(frm)
pnl3 = TestPanel3(frm)
sizer = wx.BoxSizer(wx.VERTICAL)
frm.SetSizer(sizer)
sizer.Add(pnl1, 1, wx.EXPAND)
sizer.Add(pnl2, 1, wx.EXPAND)
sizer.Add(pnl3, 1, wx.EXPAND)
frm.Show()
app.MainLoop()
