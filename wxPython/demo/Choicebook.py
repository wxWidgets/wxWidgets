
import  wx

#----------------------------------------------------------------------------

pageTexts = [ "Yet",
              "Another",
              "Way",
              "To",
              "Select",
              "Pages"
              ]
          

class TestCB(wx.Choicebook):
    def __init__(self, parent, id, log):
        wx.Choicebook.__init__(self, parent, id)
        self.log = log

        # Now make a bunch of panels for the choice book
        count = 1
        for txt in pageTexts:
            win = wx.Panel(self)
            if count == 1:
                st = wx.StaticText(win, -1,
                          "wx.Choicebook is yet another way to switch between 'page' windows",
                          (10, 10))
            else:
                st = wx.StaticText(win, -1, "Page: %d" % count, (10,10))
            count += 1
            
            self.AddPage(win, txt)

        self.Bind(wx.EVT_CHOICEBOOK_PAGE_CHANGED, self.OnPageChanged)
        self.Bind(wx.EVT_CHOICEBOOK_PAGE_CHANGING, self.OnPageChanging)


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
    testWin = TestCB(nb, -1, log)
    return testWin

#----------------------------------------------------------------------------


overview = """\
<html><body>
<h2>wx.Choicebook</h2>
<p>

This class is a control similar to a notebook control, but uses a
wx.Choice to manage the selection of the pages.

"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])



