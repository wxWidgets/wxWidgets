

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




    def OnGetItemText(self, item, col):
        return "Item %d, column %d" % (item, col)

    def OnGetItemImage(self, item):
        return 0

    def OnGetItemAttr(self, item):
        return None


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestVirtualList(nb, log)
    return win

#----------------------------------------------------------------------








overview = """\
"""
