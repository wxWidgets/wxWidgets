
import  wx

import ColorPanel
import images

colourList = [ "Aquamarine", "Grey", "Blue", "Blue Violet", "Brown", "Cadet Blue",
               "Coral", "Wheat", "Cyan", "Dark Grey", "Dark Green",
               "Steel Blue",
               ]

#----------------------------------------------------------------------------

def getNextImageID(count):
    imID = 0
    while True:
        yield imID
        imID += 1
        if imID == count:
            imID = 0
    
    
class TestTB(wx.Treebook):
    def __init__(self, parent, id, log):
        wx.Treebook.__init__(self, parent, id, style=
                             wx.BK_DEFAULT
                             #wx.BK_TOP
                             #wx.BK_BOTTOM
                             #wx.BK_LEFT
                             #wx.BK_RIGHT
                            )
        self.log = log

        # make an image list using the LBXX images
        il = wx.ImageList(32, 32)
        for x in range(12):
            f = getattr(images, 'getLB%02dBitmap' % (x+1))
            bmp = f()
            il.Add(bmp)
        self.AssignImageList(il)
        imageIdGenerator = getNextImageID(il.GetImageCount())
        
        # Now make a bunch of panels for the list book
        first = True
        for colour in colourList:
            win = self.makeColorPanel(colour)
            self.AddPage(win, colour, imageId=imageIdGenerator.next())
            if first:
                st = wx.StaticText(win.win, -1,
                          "You can put nearly any type of window here,\n"
                          "and the wx.TreeCtrl can be on either side of the\n"
                          "Treebook",
                          wx.Point(10, 10))
                first = False

            win = self.makeColorPanel(colour)
            st = wx.StaticText(win.win, -1, "this is a sub-page", (10,10))
            self.AddSubPage(win, 'a sub-page', imageId=imageIdGenerator.next())

        self.Bind(wx.EVT_TREEBOOK_PAGE_CHANGED, self.OnPageChanged)
        self.Bind(wx.EVT_TREEBOOK_PAGE_CHANGING, self.OnPageChanging)

        # This is a workaround for a sizing bug on Mac...
        wx.FutureCall(100, self.AdjustSize)

    def AdjustSize(self):
        print self.GetTreeCtrl().GetBestSize()
        self.GetTreeCtrl().InvalidateBestSize()
        self.SendSizeEvent()
        print self.GetTreeCtrl().GetBestSize()
        

    def makeColorPanel(self, color):
        p = wx.Panel(self, -1)
        win = ColorPanel.ColoredPanel(p, color)
        p.win = win
        def OnCPSize(evt, win=win):
            win.SetPosition((0,0))
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
    testWin = TestTB(nb, -1, log)
    return testWin

#----------------------------------------------------------------------------


overview = """\
<html><body>
<h2>wx.Treebook</h2>
<p>
This class is a control similar to a notebook control, but with a
wx.TreeCtrl instead of a set of tabs.

"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])



