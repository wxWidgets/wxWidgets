
import  wx

import  images

#----------------------------------------------------------------------

text = """\

Right-click on any bare area of this panel (or Ctrl-click on the Mac)
to show a popup menu.  Then look at the code for this sample.  Notice
how the PopupMenu method is similar to the ShowModal method of a
wx.Dialog in that it doesn't return until the popup menu has been
dismissed.  The event handlers for the popup menu items can either be
attached to the menu itself, or to the window that invokes PopupMenu.
"""

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        box = wx.BoxSizer(wx.VERTICAL)

        # Make and layout the controls
        fs = self.GetFont().GetPointSize()
        bf = wx.Font(fs+4, wx.SWISS, wx.NORMAL, wx.BOLD)
        nf = wx.Font(fs+2, wx.SWISS, wx.NORMAL, wx.NORMAL)

        t = wx.StaticText(self, -1, "PopupMenu")
        t.SetFont(bf)
        box.Add(t, 0, wx.CENTER|wx.ALL, 5)
        self.Bind(wx.EVT_RIGHT_UP, self.OnRightClick)


        box.Add(wx.StaticLine(self, -1), 0, wx.EXPAND)
        box.Add((10,20))

        t = wx.StaticText(self, -1, text)
        t.SetFont(nf)
        box.Add(t, 0, wx.CENTER|wx.ALL, 5)

        self.SetSizer(box)

        self.Bind(wx.EVT_RIGHT_UP, self.OnRightClick)


    def OnRightClick(self, event):
        self.log.WriteText("OnRightClick\n")

        # only do this part the first time so the events are only bound once
        #
        # Yet another anternate way to do IDs. Some prefer them up top to
        # avoid clutter, some prefer them close to the object of interest
        # for clarity. 
        if not hasattr(self, "popupID1"):
            self.popupID1 = wx.NewId()
            self.popupID2 = wx.NewId()
            self.popupID3 = wx.NewId()
            self.popupID4 = wx.NewId()
            self.popupID5 = wx.NewId()
            self.popupID6 = wx.NewId()
            self.popupID7 = wx.NewId()
            self.popupID8 = wx.NewId()
            self.popupID9 = wx.NewId()

            self.Bind(wx.EVT_MENU, self.OnPopupOne, id=self.popupID1)
            self.Bind(wx.EVT_MENU, self.OnPopupTwo, id=self.popupID2)
            self.Bind(wx.EVT_MENU, self.OnPopupThree, id=self.popupID3)
            self.Bind(wx.EVT_MENU, self.OnPopupFour, id=self.popupID4)
            self.Bind(wx.EVT_MENU, self.OnPopupFive, id=self.popupID5)
            self.Bind(wx.EVT_MENU, self.OnPopupSix, id=self.popupID6)
            self.Bind(wx.EVT_MENU, self.OnPopupSeven, id=self.popupID7)
            self.Bind(wx.EVT_MENU, self.OnPopupEight, id=self.popupID8)
            self.Bind(wx.EVT_MENU, self.OnPopupNine, id=self.popupID9)

        # make a menu
        menu = wx.Menu()
        # Show how to put an icon in the menu
        item = wx.MenuItem(menu, self.popupID1,"One")
        bmp = images.getSmilesBitmap()
        item.SetBitmap(bmp)
        menu.AppendItem(item)
        # add some other items
        menu.Append(self.popupID2, "Two")
        menu.Append(self.popupID3, "Three")
        menu.Append(self.popupID4, "Four")
        menu.Append(self.popupID5, "Five")
        menu.Append(self.popupID6, "Six")
        # make a submenu
        sm = wx.Menu()
        sm.Append(self.popupID8, "sub item 1")
        sm.Append(self.popupID9, "sub item 1")
        menu.AppendMenu(self.popupID7, "Test Submenu", sm)


        # Popup the menu.  If an item is selected then its handler
        # will be called before PopupMenu returns.
        self.PopupMenu(menu, event.GetPosition())
        menu.Destroy()


    def OnPopupOne(self, event):
        self.log.WriteText("Popup one\n")

    def OnPopupTwo(self, event):
        self.log.WriteText("Popup two\n")

    def OnPopupThree(self, event):
        self.log.WriteText("Popup three\n")

    def OnPopupFour(self, event):
        self.log.WriteText("Popup four\n")

    def OnPopupFive(self, event):
        self.log.WriteText("Popup five\n")

    def OnPopupSix(self, event):
        self.log.WriteText("Popup six\n")

    def OnPopupSeven(self, event):
        self.log.WriteText("Popup seven\n")

    def OnPopupEight(self, event):
        self.log.WriteText("Popup eight\n")

    def OnPopupNine(self, event):
        self.log.WriteText("Popup nine\n")





#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>PopupMenu</center></h2>
""" + text + """
</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

