
import  wx  
import  wx.lib.rcsizer  as rcs

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)

        sizer = rcs.RowColSizer()

        text = "This sizer lays out it's items by row and column "\
               "that are specified explicitly when the item is \n"\
               "added to the sizer.  Grid cells with nothing in "\
               "them are supported and column- or row-spanning is \n"\
               "handled as well.  Growable rows and columns are "\
               "specified just like the wxFlexGridSizer."

        sizer.Add(wx.StaticText(self, -1, text), row=1, col=1, colspan=5)

        sizer.Add(wx.TextCtrl(self, -1, "(3,1)"), flag=wx.EXPAND, row=3, col=1)
        sizer.Add(wx.TextCtrl(self, -1, "(3,2)"), row=3, col=2)
        sizer.Add(wx.TextCtrl(self, -1, "(3,3)"), row=3, col=3)
        sizer.Add(wx.TextCtrl(self, -1, "(3,4)"), row=3, col=4)
        sizer.Add(
            wx.TextCtrl(self, -1, "(4,2) span:(2,2)"), 
            flag=wx.EXPAND, row=4, col=2, rowspan=2, colspan=2
            )

        sizer.Add(wx.TextCtrl(self, -1, "(6,4)"), row=6, col=4)
        sizer.Add(wx.TextCtrl(self, -1, "(7,2)"), row=7, col=2)
        sizer.Add(wx.TextCtrl(self, -1, "(8,3)"), row=8, col=3)
        sizer.Add(
            wx.TextCtrl(self, -1, "(10,1) colspan: 4"), 
            flag=wx.EXPAND, pos=(10,1), colspan=4
            )
        
        sizer.Add(
            wx.TextCtrl(self, -1, "(3,5) rowspan: 8, growable col", style=wx.TE_MULTILINE),
            flag=wx.EXPAND, pos=(3,5), size=(8,1)
            )

        box = wx.BoxSizer(wx.VERTICAL)
        box.Add(wx.Button(self, -1, "A vertical box"), flag=wx.EXPAND)
        box.Add(wx.Button(self, -1, "sizer put in the"), flag=wx.EXPAND)
        box.Add(wx.Button(self, -1, "RowColSizer at (12,1)"), flag=wx.EXPAND)
        sizer.Add(box, pos=(12,1))

        sizer.Add(
            wx.TextCtrl(self, -1, "(12,2) align bottom"), 
            flag=wx.ALIGN_BOTTOM, pos=(12,2)
            )

        sizer.Add(
            wx.TextCtrl(self, -1, "(12,3) align center"), 
            flag=wx.ALIGN_CENTER_VERTICAL, pos=(12,3)
            )

        sizer.Add(wx.TextCtrl(self, -1, "(12,4)"),pos=(12,4))
        sizer.Add(
            wx.TextCtrl(self, -1, "(12,5) full border"), 
            flag=wx.EXPAND|wx.ALL, border=15, pos=(12,5)
            )

        sizer.AddGrowableCol(5)
        sizer.AddGrowableRow(9)

        sizer.AddSpacer(10,10, pos=(1,6))
        sizer.AddSpacer(10,10, pos=(13,1))

        self.SetSizer(sizer)
        self.SetAutoLayout(True)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb)
    return win


#----------------------------------------------------------------------


overview = rcs.__doc__

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

