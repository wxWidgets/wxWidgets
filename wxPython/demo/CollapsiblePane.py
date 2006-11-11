
import wx

#----------------------------------------------------------------------

label1 = "Click here to show pane"
label2 = "Click here to hide pane"

btnlbl1 = "call Expand(True)"
btnlbl2 = "call Expand(False)"


class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        title = wx.StaticText(self, label="wx.CollapsiblePane")
        title.SetFont(wx.Font(18, wx.SWISS, wx.NORMAL, wx.BOLD))
        title.SetForegroundColour("blue")

        self.cp = cp = wx.CollapsiblePane(self, label=label1)
        self.Bind(wx.EVT_COLLAPSIBLEPANE_CHANGED, self.OnPaneChanged, cp)
        self.MakePaneContent(cp.GetPane())

        sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(sizer)
        sizer.Add(title, 0, wx.ALL, 25)
        sizer.Add(cp, 0, wx.RIGHT|wx.LEFT|wx.EXPAND, 25)

        self.btn = btn = wx.Button(self, label=btnlbl1)
        self.Bind(wx.EVT_BUTTON, self.OnToggle, btn)
        sizer.Add(btn, 0, wx.ALL, 25)
        
        
    def OnToggle(self, evt):
        self.cp.Collapse(self.cp.IsExpanded())
        self.OnPaneChanged()
        

    def OnPaneChanged(self, evt=None):
        self.log.write('wx.EVT_COLLAPSIBLEPANE_CHANGED')
        self.Layout()
        if self.cp.IsExpanded():
            self.cp.SetLabel(label2)
            self.btn.SetLabel(btnlbl2)
        else:
            self.cp.SetLabel(label1)
            self.btn.SetLabel(btnlbl1)
        self.btn.SetInitialSize()
        

    def MakePaneContent(self, pane):
        '''Just make a few controls to put on the collapsible pane'''
        nameLbl = wx.StaticText(pane, -1, "Name:")
        name = wx.TextCtrl(pane, -1, "");

        addrLbl = wx.StaticText(pane, -1, "Address:")
        addr1 = wx.TextCtrl(pane, -1, "");
        addr2 = wx.TextCtrl(pane, -1, "");

        cstLbl = wx.StaticText(pane, -1, "City, State, Zip:")
        city  = wx.TextCtrl(pane, -1, "", size=(150,-1));
        state = wx.TextCtrl(pane, -1, "", size=(50,-1));
        zip   = wx.TextCtrl(pane, -1, "", size=(70,-1));
        
        addrSizer = wx.FlexGridSizer(cols=2, hgap=5, vgap=5)
        addrSizer.Add((10,10))
        addrSizer.Add((10,10))
        addrSizer.AddGrowableCol(1)
        addrSizer.Add(nameLbl, 0, 
                wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        addrSizer.Add(name, 0, wx.EXPAND)
        addrSizer.Add(addrLbl, 0,
                wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        addrSizer.Add(addr1, 0, wx.EXPAND)
        addrSizer.Add((10,10)) 
        addrSizer.Add(addr2, 0, wx.EXPAND)

        addrSizer.Add(cstLbl, 0,
                wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)

        cstSizer = wx.BoxSizer(wx.HORIZONTAL)
        cstSizer.Add(city, 1)
        cstSizer.Add(state, 0, wx.LEFT|wx.RIGHT, 5)
        cstSizer.Add(zip)
        addrSizer.Add(cstSizer, 0, wx.EXPAND)

        pane.SetSizer(addrSizer)



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.CollapsiblePane</center></h2>

A collapsable panel is a container with an embedded button-like
control which can be used by the user to collapse or expand the pane's
contents.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

