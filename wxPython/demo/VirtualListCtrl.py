

from wxPython.wx import *

#----------------------------------------------------------------------

class TestVirtualList(wxListCtrl):
    def __init__(self, parent, log):
        wxListCtrl.__init__(self, parent, -1,
                            style=wxLC_REPORT|wxLC_VIRTUAL|wxLC_HRULES|wxLC_VRULES)
        self.log = log

        self.InsertColumn(0, "First")
        self.InsertColumn(1, "Second")
        self.InsertColumn(2, "Third")
        self.SetColumnWidth(0, 175)
        self.SetColumnWidth(1, 175)
        self.SetColumnWidth(2, 175)

        self.SetItemCount(1000000)

        self.attr1 = wxListItemAttr()
        self.attr1.SetBackgroundColour("yellow")

        self.attr2 = wxListItemAttr()
        self.attr2.SetBackgroundColour("light blue")


    def OnGetItemText(self, item, col):
        return "Item %d, column %d" % (item, col)

    def OnGetItemImage(self, item):
        return 0

    def OnGetItemAttr(self, item):
        if item % 3 == 1:
            return self.attr1
        elif item % 3 == 2:
            return self.attr2
        else:
            return None


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestVirtualList(nb, log)
    return win

#----------------------------------------------------------------------








overview = """\
"""
