
import  wx

import ColorPanel
import images

colourList = [ "Aquamarine", "Black", "Blue", "Blue Violet", "Brown", "Cadet Blue",
               "Coral", "Cornflower Blue", "Cyan", "Dark Grey", "Dark Green",
               "Dark Olive Green", "Dark Orchid", "Dark Slate Blue",
               "Dark Slate Grey", "Dark Turquoise", "Dim Grey", "Firebrick",
               "Forest Green", "Gold", "Goldenrod", "Grey", "Green", "Green Yellow",
               "Indian Red", "Khaki", "Light Blue", "Light Grey", "Light Steel Blue",
               "Lime Green", "Magenta", "Maroon", "Medium Aquamarine", "Medium Blue",
               "Medium Forest Green", "Medium Goldenrod", "Medium Orchid",
               "Medium Sea Green", "Medium Slate Blue", "Medium Spring Green",
               "Medium Turquoise", "Medium Violet Red", "Midnight Blue", "Navy",
               "Orange", "Orange Red", "Orchid", "Pale Green", "Pink", "Plum",
               "Purple", "Red", "Salmon", "Sea Green", "Sienna", "Sky Blue",
               "Slate Blue", "Spring Green", "Steel Blue", "Tan", "Thistle",
               "Turquoise", "Violet", "Violet Red", "Wheat", "White", "Yellow",
               "Yellow Green"
               ]

#----------------------------------------------------------------------------

class TestLB(wx.Listbook):
    def __init__(self, parent, id, log):
        wx.Listbook.__init__(self, parent, id, style=
                            wx.LB_DEFAULT
                            #wxLB_TOP
                            #wxLB_BOTTOM
                            #wxLB_LEFT
                            #wxLB_RIGHT
                            )
        self.log = log

        # make an image list using the BlomXX images
        il = wx.ImageList(32, 32)
        for x in range(1, 16):
            f = getattr(images, 'getBlom%02dBitmap' % x)
            bmp = f()
            il.Add(bmp)
        self.AssignImageList(il)

        # Now make a bunch of panels for the list book
        first = True
        imID = 0
        for colour in colourList:
            win = self.makeColorPanel(colour)
            self.AddPage(win, colour, imageId=imID)
            imID += 1
            if imID == il.GetImageCount(): imID = 0
            if first:
                st = wx.StaticText(win.win, -1,
                          "You can put nearly any type of window here,\n"
                          "and the list can be on any side of the Listbook",
                          wx.Point(10, 10))
                #st.SetForegroundColour(wxWHITE)
                #st.SetBackgroundColour(wxBLUE)
                first = False

        self.Bind(wx.EVT_LISTBOOK_PAGE_CHANGED, self.OnPageChanged)
        self.Bind(wx.EVT_LISTBOOK_PAGE_CHANGING, self.OnPageChanging)


    def makeColorPanel(self, color):
        p = wx.Panel(self, -1)
        win = ColorPanel.ColoredPanel(p, color)
        p.win = win
        def OnCPSize(evt, win=win):
            win.SetSize(evt.GetSize())
        p.Bind(wx.EVT_SIZE, OnCPSize)
        return p


    def OnPageChanged(self, event):
        old = event.GetOldSelection()
        new = event.GetSelection()
        sel = self.GetSelection()
        self.log.write('OnPageChanged,  old:%d, new:%d, sel:%d\n' % (old, new, sel))
        event.Skip()

    def OnPageChanging(self, event):
        old = event.GetOldSelection()
        new = event.GetSelection()
        sel = self.GetSelection()
        self.log.write('OnPageChanging, old:%d, new:%d, sel:%d\n' % (old, new, sel))
        event.Skip()

#----------------------------------------------------------------------------

def runTest(frame, nb, log):
    testWin = TestLB(nb, -1, log)
    return testWin

#----------------------------------------------------------------------------


overview = """\
<html><body>
<h2>wx.Listbook</h2>
<p>
This class is a control similar to a notebook control, but with a
wx.ListCtrl instead of a set of tabs.

"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])



