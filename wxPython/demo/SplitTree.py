from wxPython.wx import *
from wxPython.gizmos import *

import images

#----------------------------------------------------------------------

class TestTree(wxRemotelyScrolledTreeCtrl):
    def __init__(self, parent, ID, pos=wxDefaultPosition, size=wxDefaultSize,
                 style=wxTR_HAS_BUTTONS):
        wxRemotelyScrolledTreeCtrl.__init__(self, parent, ID, pos, size, style)

        # make an image list
        im1 = im2 = -1
        self.il = wxImageList(16, 16)
        im1 = self.il.Add(images.getFolder1Bitmap())
        im2 = self.il.Add(images.getFile1Bitmap())
        self.SetImageList(self.il)

        # Add some items
        root = self.AddRoot("Root")
        for i in range(30):
            item = self.AppendItem(root, "Item %d" % i, im1)
            for j in range(10):
                child = self.AppendItem(item, "Child %d" % j, im2)

        self.Expand(root)



class TestValueWindow(wxTreeCompanionWindow):
    def __init__(self, parent, ID, pos=wxDefaultPosition, size=wxDefaultSize, style=0):
        wxTreeCompanionWindow.__init__(self, parent, ID, pos, size, style)
        self.SetBackgroundColour("WHITE")
        EVT_ERASE_BACKGROUND(self, self.OEB)

    def OEB(self, evt):
        pass

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
            pen = wxPen(wxSystemSettings_GetSystemColour(wxSYS_COLOUR_3DLIGHT), 1, wxSOLID)
            dc.SetPen(pen)
            dc.SetBrush(wxBrush(self.GetBackgroundColour(), wxSOLID))
            dc.DrawRectangle(rect.x, rect.y, rect.width+1, rect.height+1)
            dc.SetTextForeground("BLACK")
            dc.SetBackgroundMode(wxTRANSPARENT)
            tw, th = dc.GetTextExtent(text)
            x = 5
            y = rect.y + max(0, (rect.height - th) / 2)
            dc.DrawText(text, x, y)



class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        scroller = wxSplitterScrolledWindow(self, -1, #(50,50), (350, 250),
                                      style=wxNO_BORDER | wxCLIP_CHILDREN | wxVSCROLL)
        splitter = wxThinSplitterWindow(scroller, -1, style=wxSP_3DBORDER | wxCLIP_CHILDREN)
        splitter.SetSashSize(2)
        tree = TestTree(splitter, -1, style = wxTR_HAS_BUTTONS |
                                              wxTR_NO_LINES |
                                              wxTR_ROW_LINES |
                                              #wxTR_HIDE_ROOT |
                                              wxNO_BORDER )
        valueWindow = TestValueWindow(splitter, -1, style=wxNO_BORDER)

        splitter.SplitVertically(tree, valueWindow, 150)
        scroller.SetTargetWindow(tree)
        scroller.EnableScrolling(False, False)

        valueWindow.SetTreeCtrl(tree)
        tree.SetCompanionWindow(valueWindow)

        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(scroller, 1, wxEXPAND|wxALL, 25)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    if wxPlatform == "__WXMAC__":
        wxMessageBox("This demo currently fails on the Mac. The problem is being looked into...", "Sorry")
        return

    win = TestPanel(nb, log)
    return win


#----------------------------------------------------------------------





overview = """\
This demo shows a collection of classes that were designed to operate
together and provide a tree control with additional columns for each
item.  The classes are wxRemotelyScrolledTreeCtrl, wxTreeCompanionWindow,
wxThinSplitterWindow, and wxSplitterScrolledWindow, some of which may
also be useful by themselves.
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

