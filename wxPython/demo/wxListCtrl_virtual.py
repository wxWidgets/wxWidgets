
from wxPython.wx import *
import images

#----------------------------------------------------------------------

class TestVirtualList(wxListCtrl):
    def __init__(self, parent, log):
        wxListCtrl.__init__(self, parent, -1,
                            style=wxLC_REPORT|wxLC_VIRTUAL|wxLC_HRULES|wxLC_VRULES)
        self.log = log

        self.il = wxImageList(16, 16)
        self.idx1 = self.il.Add(images.getSmilesBitmap())
        self.SetImageList(self.il, wxIMAGE_LIST_SMALL)


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

        EVT_LIST_ITEM_SELECTED(self, self.GetId(), self.OnItemSelected)
        EVT_LIST_ITEM_ACTIVATED(self, self.GetId(), self.OnItemActivated)
        EVT_LIST_ITEM_DESELECTED(self, self.GetId(), self.OnItemDeselected)


    def OnItemSelected(self, event):
        self.currentItem = event.m_itemIndex
        self.log.WriteText('OnItemSelected: "%s", "%s", "%s", "%s"\n' %
                           (self.currentItem,
                            self.GetItemText(self.currentItem),
                            self.getColumnText(self.currentItem, 1),
                            self.getColumnText(self.currentItem, 2)))

    def OnItemActivated(self, event):
        self.currentItem = event.m_itemIndex
        self.log.WriteText("OnItemActivated: %s\nTopItem: %s\n" %
                           (self.GetItemText(self.currentItem), self.GetTopItem()))

    def getColumnText(self, index, col):
        item = self.GetItem(index, col)
        return item.GetText()

    def OnItemDeselected(self, evt):
        print evt.m_itemIndex


    #---------------------------------------------------
    # These methods are callbacks for implementing the
    # "virtualness" of the list...  Normally you would
    # determine the text, attributes and/or image based
    # on values from some external data source, but for
    # this demo we'll just calcualte them
    def OnGetItemText(self, item, col):
        return "Item %d, column %d" % (item, col)

    def OnGetItemImage(self, item):
        if item % 3 == 0:
            return self.idx1
        else:
            return -1

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



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

