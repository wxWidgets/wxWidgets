import sys
import wx
from wx.lib.mixins.listctrl import CheckListCtrlMixin

from ListCtrl import musicdata

#----------------------------------------------------------------------

class CheckListCtrl(wx.ListCtrl, CheckListCtrlMixin):
    def __init__(self, parent, log):
        wx.ListCtrl.__init__(self, parent, -1, style=wx.LC_REPORT)
        CheckListCtrlMixin.__init__(self)
        self.log = log
        self.Bind(wx.EVT_LIST_ITEM_ACTIVATED, self.OnItemActivated)


    def OnItemActivated(self, evt):
        self.ToggleItem(evt.m_itemIndex)


    # this is called by the base class when an item is checked/unchecked
    def OnCheckItem(self, index, flag):
        data = self.GetItemData(index)
        title = musicdata[data][1]
        if flag:
            what = "checked"
        else:
            what = "unchecked"
        self.log.write('item "%s", at index %d was %s\n' % (title, index, what))



class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        self.list = CheckListCtrl(self, log)
        sizer = wx.BoxSizer()
        sizer.Add(self.list, 1, wx.EXPAND)
        self.SetSizer(sizer)

        self.list.InsertColumn(0, "Artist")
        self.list.InsertColumn(1, "Title", wx.LIST_FORMAT_RIGHT)
        self.list.InsertColumn(2, "Genre")

        for key, data in musicdata.iteritems():
            index = self.list.InsertStringItem(sys.maxint, data[0])
            self.list.SetStringItem(index, 1, data[1])
            self.list.SetStringItem(index, 2, data[2])
            self.list.SetItemData(index, key)
      
        self.list.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.list.SetColumnWidth(1, wx.LIST_AUTOSIZE)
        self.list.SetColumnWidth(2, 100)

        self.list.CheckItem(4)
        self.list.CheckItem(7)

        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnItemSelected, self.list)
        self.Bind(wx.EVT_LIST_ITEM_DESELECTED, self.OnItemDeselected, self.list)

        
    def OnItemSelected(self, evt):
        self.log.write('item selected: %s\n' % evt.m_itemIndex)
        
    def OnItemDeselected(self, evt):
        self.log.write('item deselected: %s\n' % evt.m_itemIndex)
        

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><centerCheckListCtrlMixin></center></h2>

CheckListCtrlMixin is a simple mixin class that can add a checkbox to
the first column of a wx.ListCtrl.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

