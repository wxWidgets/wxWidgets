
import wx

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1, style=0)

        # Create controls
        sb = wx.StaticBox(self, -1, "Options")
        searchBtnOpt = wx.CheckBox(self, -1, "Search button")
        searchBtnOpt.SetValue(True)
        cancelBtnOpt = wx.CheckBox(self, -1, "Cancel button")
        menuBtnOpt   = wx.CheckBox(self, -1, "Search menu")

        self.search = wx.SearchCtrl(self, size=(200,-1), style=wx.TE_PROCESS_ENTER)

        # Setup the layout
        box = wx.StaticBoxSizer(sb, wx.VERTICAL)
        box.Add(searchBtnOpt, 0, wx.ALL, 5)
        box.Add(cancelBtnOpt, 0, wx.ALL, 5)
        box.Add(menuBtnOpt,   0, wx.ALL, 5)

        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(box, 0, wx.ALL, 15)
        sizer.Add((15,15))
        sizer.Add(self.search, 0, wx.ALL, 15)

        self.tc = wx.TextCtrl(self)  # just for testing that heights match...
        sizer.Add(self.tc, 0, wx.TOP, 15)

        self.SetSizer(sizer)


        # Set event bindings
        self.Bind(wx.EVT_CHECKBOX, self.OnToggleSearchButton, searchBtnOpt)
        self.Bind(wx.EVT_CHECKBOX, self.OnToggleCancelButton, cancelBtnOpt)
        self.Bind(wx.EVT_CHECKBOX, self.OnToggleSearchMenu,   menuBtnOpt)

        self.Bind(wx.EVT_SEARCHCTRL_SEARCH, self.OnSearch, self.search)
        self.Bind(wx.EVT_SEARCHCTRL_CANCEL, self.OnCancel, self.search)
        self.search.Bind(wx.EVT_TEXT_ENTER,        self.OnDoSearch) #, self.search)
        

    def OnToggleSearchButton(self, evt):
        self.search.SetSearchButtonVisible( evt.GetInt() )
            
    def OnToggleCancelButton(self, evt):
        self.search.SetCancelButtonVisible( evt.GetInt() )
        
    def OnToggleSearchMenu(self, evt):
        if evt.GetInt():
            self.search.SetMenu( self.MakeMenu() )
        else:
            self.search.SetMenu(None)


    def OnSearch(self, evt):
        self.log.write("OnSearch")
            
    def OnCancel(self, evt):
        self.log.write("OnCancel")

    def OnDoSearch(self, evt):
        self.log.write("OnDoSearch: " + self.search.GetValue())
        

    def MakeMenu(self):
        menu = wx.Menu()
        item = menu.Append(-1, "Recent Searches")
        item.Enable(False)
        for txt in [ "You can maintain",
                     "a list of old",
                     "search strings here",
                     "and bind EVT_MENU to",
                     "catch their selections" ]:
            menu.Append(-1, txt)
        return menu
         

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.SearchCtrl</center></h2>

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

