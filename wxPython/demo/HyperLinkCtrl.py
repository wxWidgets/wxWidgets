
import wx
import wx.lib.hyperlink as hl

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        self.SetFont(wx.Font(10, wx.SWISS, wx.NORMAL, wx.NORMAL, False))

        sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(sizer)        
        
        # Creator credits
        text1 = wx.StaticText(self, -1, "HyperLinkCtrl Example By Andrea Gavana")
        text1.SetFont(wx.Font(9, wx.SWISS, wx.NORMAL, wx.BOLD, False, 'Verdana'))
        
        sizer.Add((0,10))
        sizer.Add(text1, 0, wx.LEFT | wx.TOP | wx.BOTTOM, 10)
        
        text2 = wx.StaticText(self, -1, "Latest Revision: 11 May 2005")
        text2.SetFont(wx.Font(8, wx.SWISS, wx.NORMAL, wx.NORMAL, False, 'Verdana'))
        sizer.Add(text2, 0, wx.LEFT, 10)
       
        sizer.Add((0,25))

        # Default Web links:
        self._hyper1 = hl.HyperLinkCtrl(self, wx.ID_ANY, "wxPython Main Page",
                                        URL="http://www.wxpython.org/")
        sizer.Add(self._hyper1, 0, wx.ALL, 10)
        
        
        # Web link with underline rollovers, opens in window
        self._hyper2 = hl.HyperLinkCtrl(self, wx.ID_ANY, "My Home Page",
                                        URL="http://xoomer.virgilio.it/infinity77/")
        sizer.Add(self._hyper2, 0, wx.ALL, 10)
        self._hyper2.Bind(hl.EVT_HYPERLINK_MIDDLE, self.OnMiddleLink)
        self._hyper2.AutoBrowse(False)
        self._hyper2.SetColours("BLUE", "BLUE", "BLUE")
        self._hyper2.EnableRollover(True)
        self._hyper2.SetUnderlines(False, False, True)
        self._hyper2.SetBold(True)
        self._hyper2.OpenInSameWindow(True) # middle click to open in window
        self._hyper2.SetToolTip(wx.ToolTip("Middle-click to open in browser window"))
        self._hyper2.UpdateLink()


        # Intense link examples..
        self._hyper3 = hl.HyperLinkCtrl(self, wx.ID_ANY, "wxPython Mail Archive",
                                        URL="http://lists.wxwidgets.org/")
        sizer.Add(self._hyper3, 0, wx.ALL, 10)
        self._hyper3.Bind(hl.EVT_HYPERLINK_RIGHT, self.OnRightLink)
        
        self._hyper3.SetLinkCursor(wx.CURSOR_QUESTION_ARROW)
        self._hyper3.SetColours("DARK GREEN", "RED", "NAVY")
        self._hyper3.SetUnderlines(False, False, False)
        self._hyper3.EnableRollover(True)
        self._hyper3.SetBold(True)
        self._hyper3.DoPopup(False)
        self._hyper3.UpdateLink()


        self._hyper4 = hl.HyperLinkCtrl(self, wx.ID_ANY,
                                        "Open Google In Current Browser Window?",
                                        URL="http://www.google.com")
        sizer.Add(self._hyper4, 0, wx.ALL, 10)
        self._hyper4.Bind(hl.EVT_HYPERLINK_LEFT, self.OnLink)
        self._hyper4.SetToolTip(wx.ToolTip("Click link for yes, no, cancel dialog"))
        self._hyper4.AutoBrowse(False)


        


    def OnLink(self, event):
        # Goto URL, demonstrates attempt to open link in current window:
        strs = "Open Google In Current Browser Window "
        strs = strs + "(NO Opens Google In Another Browser Window)?"
        nResult = wx.MessageBox(strs, "HyperLinkCtrl", wx.YES_NO |
                                wx.CANCEL | wx.ICON_QUESTION, self)

        if nResult == wx.YES:
            self._hyper4.GotoURL("http://www.google.com", True, True)
        elif nResult == wx.NO:
            self._hyper4.GotoURL("http://www.google.com", True, False)

        
        
    def OnRightLink(self, event):
        pos = event.GetPosition()
        menuPopUp = wx.Menu("Having a nice day?")
        ID_MENU_YES = wx.NewId()
        ID_MENU_NO  = wx.NewId()
        menuPopUp.Append(ID_MENU_YES, "Yes, absolutely!")
        menuPopUp.Append(ID_MENU_NO,  "I've had better")
        self.PopupMenu(menuPopUp, self._hyper3.GetPosition())
        menuPopUp.Destroy()
        


    def OnMiddleLink(self, event):
        self._hyper2.GotoURL("http://xoomer.virgilio.it/infinity77/",
                             True, True)



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>Say something nice here</center></h2>

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])


