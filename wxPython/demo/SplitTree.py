# 11/13/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
#
# 11/26/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Bigtime errors on startup. Blows up with a program error.
#   Error:
#
#   21:04:11: Debug: ..\..\src\msw\treectrl.cpp(1508): assert "IsVisible(item)" 
#   failed: The item you call GetNextVisible() for must be visible itself!
#
#   I suspect this error is in the lib itself.
#

import  wx
import  wx.gizmos as gizmos


#----------------------------------------------------------------------

class TestTree(gizmos.RemotelyScrolledTreeCtrl):
    def __init__(self, parent, style=wx.TR_HAS_BUTTONS):
        gizmos.RemotelyScrolledTreeCtrl.__init__(self, parent, -1, style=style)

        # make an image list
        im1 = im2 = -1
        self.il = wx.ImageList(16, 16)
        im1 = self.il.Add(wx.ArtProvider.GetBitmap(wx.ART_FOLDER, wx.ART_TOOLBAR, (16,16)))
        im2 = self.il.Add(wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE, wx.ART_TOOLBAR, (16,16)))
        self.SetImageList(self.il)

        # Add some items
        root = self.AddRoot("Root")

        for i in range(30):
            item = self.AppendItem(root, "Item %d" % i, im1)

            for j in range(10):
                child = self.AppendItem(item, "Child %d" % j, im2)

        self.Expand(root)



class TestValueWindow(gizmos.TreeCompanionWindow):
    def __init__(self, parent, style=0):
        gizmos.TreeCompanionWindow.__init__(self, parent, -1, style=style)
        self.SetBackgroundColour("WHITE")

    # This method is called to draw each item in the value window
    def DrawItem(self, dc, itemId, rect):
        tree = self.GetTreeCtrl()

        if tree:
            text = "This is "
            parent = tree.GetItemParent(itemId)

            if parent.IsOk():
                ptext = tree.GetItemText(parent)
                text = text + ptext + " --> "

            text = text + tree.GetItemText(itemId)
            pen = wx.Pen(
                wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DLIGHT), 
                1, wx.SOLID
                )

            dc.SetPen(pen)
            dc.SetBrush(wx.Brush(self.GetBackgroundColour(), wx.SOLID))
            dc.DrawRectangle(rect.x, rect.y, rect.width+1, rect.height+1)
            dc.SetTextForeground("BLACK")
            dc.SetBackgroundMode(wx.TRANSPARENT)
            tw, th = dc.GetTextExtent(text)
            x = 5
            y = rect.y + max(0, (rect.height - th) / 2)
            dc.DrawText(text, x, y)



class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1, size=(640,480))
        self.log = log

        scroller = gizmos.SplitterScrolledWindow(
            self, -1, style=wx.NO_BORDER | wx.CLIP_CHILDREN | wx.VSCROLL
            )

        splitter = gizmos.ThinSplitterWindow(
            scroller, -1, style=wx.SP_3DBORDER | wx.CLIP_CHILDREN
            )
            
        splitter.SetSashSize(2)
        tree = TestTree(splitter, style = wx.TR_HAS_BUTTONS |
                                          wx.TR_NO_LINES |
                                          wx.TR_ROW_LINES |
                                          #wx.TR_HIDE_ROOT |
                                          wx.NO_BORDER )

        valueWindow = TestValueWindow(splitter, style=wx.NO_BORDER)

        wx.CallAfter(splitter.SplitVertically, tree, valueWindow, 150)
        scroller.SetTargetWindow(tree)
        scroller.EnableScrolling(False, False)

        valueWindow.SetTreeCtrl(tree)
        tree.SetCompanionWindow(valueWindow)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(scroller, 1, wx.EXPAND|wx.ALL, 25)
        self.SetSizer(sizer)
        self.Layout()

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    if wx.Platform == "__WXMAC__":
        from Main import MessagePanel
        win = MessagePanel(nb, 'This demo currently fails on the Mac. The problem is being looked into...',
                           'Sorry', wx.ICON_WARNING)
        return win

    win = TestPanel(nb, log)
    return win


#----------------------------------------------------------------------


overview = """\
This demo shows a collection of classes that were designed to operate
together and provide a tree control with additional columns for each
item.  The classes are wx.RemotelyScrolledTreeCtrl, wx.TreeCompanionWindow,
wx.ThinSplitterWindow, and wx.SplitterScrolledWindow, some of which may
also be useful by themselves.
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

