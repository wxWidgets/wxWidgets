
from wxPython.wx import *
import images

#----------------------------------------------------------------------

text = """\

Right-click on the panel (or Ctrl-click on the Mac) to show a popup
menu.  Then look at the code for this sample.  Notice how the
PopupMenu method is similar to the ShowModal method of a wxDialog in
that it doesn't return until the popup menu has been dismissed.  The
event handlers for the popup menu items can either be attached to the
menu itself, or to the window that invokes PopupMenu.
"""

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1)
        box = wxBoxSizer(wxVERTICAL)

        # Make and layout the controls
        fs = self.GetFont().GetPointSize()
        bf = wxFont(fs+4, wxSWISS, wxNORMAL, wxBOLD)
        nf = wxFont(fs+2, wxSWISS, wxNORMAL, wxNORMAL)

        t = wxStaticText(self, -1, "PopupMenu")
        t.SetFont(bf)
        box.Add(t, 0, wxCENTER|wxALL, 5)

        box.Add(wxStaticLine(self, -1), 0, wxEXPAND)
        box.Add(10,20)

        t = wxStaticText(self, -1, text)
        t.SetFont(nf)
        box.Add(t, 0, wxCENTER|wxALL, 5)

        self.SetSizer(box)

        EVT_RIGHT_UP(self, self.OnRightClick)


    def OnRightClick(self, event):
        self.log.WriteText("OnRightClick\n")

        # only do this part the first time so the events are only bound once
        if not hasattr(self, "popupID1"):
            self.popupID1 = wxNewId()
            self.popupID2 = wxNewId()
            self.popupID3 = wxNewId()
            self.popupID4 = wxNewId()
            self.popupID5 = wxNewId()
            self.popupID6 = wxNewId()
            self.popupID7 = wxNewId()
            self.popupID8 = wxNewId()
            self.popupID9 = wxNewId()
            EVT_MENU(self, self.popupID1, self.OnPopupOne)
            EVT_MENU(self, self.popupID2, self.OnPopupTwo)
            EVT_MENU(self, self.popupID3, self.OnPopupThree)
            EVT_MENU(self, self.popupID4, self.OnPopupFour)
            EVT_MENU(self, self.popupID5, self.OnPopupFive)
            EVT_MENU(self, self.popupID6, self.OnPopupSix)
            EVT_MENU(self, self.popupID7, self.OnPopupSeven)
            EVT_MENU(self, self.popupID8, self.OnPopupEIght)
            EVT_MENU(self, self.popupID9, self.OnPopupNine)

        # make a menu
        menu = wxMenu()
        # Show how to put an icon in the menu
        item = wxMenuItem(menu, self.popupID1,"One")
        item.SetBitmap(images.getSmilesBitmap())
        menu.AppendItem(item)
        # add some other items
        menu.Append(self.popupID2, "Two")
        menu.Append(self.popupID3, "Three")
        menu.Append(self.popupID4, "Four")
        menu.Append(self.popupID5, "Five")
        menu.Append(self.popupID6, "Six")
        # make a submenu
        sm = wxMenu()
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

    def OnPopupEIght(self, event):
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
    run.main(['', os.path.basename(sys.argv[0])])

