# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx

#---------------------------------------------------------------------------

class MySplitter(wx.SplitterWindow):
    def __init__(self, parent, ID, log):
        wx.SplitterWindow.__init__(self, parent, ID)
        self.log = log
        
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGED, self.OnSashChanged)
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGING, self.OnSashChanging)

    def OnSashChanged(self, evt):
        self.log.WriteText("sash changed to %s\n" % str(evt.GetSashPosition()))
        # uncomment this to not allow the change
        #evt.SetSashPosition(-1)

    def OnSashChanging(self, evt):
        self.log.WriteText("sash changing to %s\n" % str(evt.GetSashPosition()))
        # uncomment this to not allow the change
        #evt.SetSashPosition(-1)

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    splitter = MySplitter(nb, -1, log)

    p1 = wx.Window(splitter, -1)
    p1.SetBackgroundColour(wx.RED)
    wx.StaticText(p1, -1, "Panel One", (5,5)).SetBackgroundColour(wx.RED)

    p2 = wx.Window(splitter, -1)
    p2.SetBackgroundColour(wx.BLUE)
    wx.StaticText(p2, -1, "Panel Two", (5,5)).SetBackgroundColour(wx.BLUE)

    splitter.SetMinimumPaneSize(20)
    splitter.SplitVertically(p1, p2, 100)

    return splitter


#---------------------------------------------------------------------------


overview = """\
This class manages up to two subwindows. The current view can be split
into two programmatically (perhaps from a menu command), and unsplit
either programmatically or via the wxSplitterWindow user interface.
"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

