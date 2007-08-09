import wx
import sys, glob, random
import data

class DataSource:
    """
    A simple data source class that just uses our sample data items.
    A real data source class would manage fetching items from a
    database or similar.
    """
    def GetColumnHeaders(self):
        return data.columns

    def GetCount(self):
        return len(data.rows)

    def GetItem(self, index):
        return data.rows[index]

    def UpdateCache(self, start, end):
        pass
    

class VirtualListCtrl(wx.ListCtrl):
    """
    A generic virtual listctrl that fetches data from a DataSource.
    """
    def __init__(self, parent, dataSource):
        wx.ListCtrl.__init__(self, parent,
            style=wx.LC_REPORT|wx.LC_SINGLE_SEL|wx.LC_VIRTUAL)
        self.dataSource = dataSource
        self.Bind(wx.EVT_LIST_CACHE_HINT, self.DoCacheItems)
        self.SetItemCount(dataSource.GetCount())

        columns = dataSource.GetColumnHeaders()
        for col, text in enumerate(columns):
            self.InsertColumn(col, text)
        

    def DoCacheItems(self, evt):
        self.dataSource.UpdateCache(
            evt.GetCacheFrom(), evt.GetCacheTo())

    def OnGetItemText(self, item, col):
        data = self.dataSource.GetItem(item)
        return data[col]

    def OnGetItemAttr(self, item):  return None
    def OnGetItemImage(self, item): return -1

        

class DemoFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1,
                          "Virtual wx.ListCtrl",
                          size=(600,400))

        self.list = VirtualListCtrl(self, DataSource())



app = wx.PySimpleApp()
frame = DemoFrame()
frame.Show()
app.MainLoop()
