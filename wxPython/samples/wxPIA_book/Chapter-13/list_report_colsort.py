import wx
import wx.lib.mixins.listctrl
import sys, glob, random
import data

class DemoFrame(wx.Frame, wx.lib.mixins.listctrl.ColumnSorterMixin):
    def __init__(self):
        wx.Frame.__init__(self, None, -1,
                          "wx.ListCtrl with ColumnSorterMixin",
                          size=(600,400))

        # load some images into an image list
        il = wx.ImageList(16,16, True)
        for name in glob.glob("smicon??.png"):
            bmp = wx.Bitmap(name, wx.BITMAP_TYPE_PNG)
            il_max = il.Add(bmp)

        # add some arrows for the column sorter
        self.up = il.AddWithColourMask(
            wx.Bitmap("sm_up.bmp", wx.BITMAP_TYPE_BMP), "blue")
        self.dn = il.AddWithColourMask(
            wx.Bitmap("sm_down.bmp", wx.BITMAP_TYPE_BMP), "blue")
        
        # create the list control
        self.list = wx.ListCtrl(self, -1, style=wx.LC_REPORT)

        # assign the image list to it
        self.list.AssignImageList(il, wx.IMAGE_LIST_SMALL)

        # Add some columns
        for col, text in enumerate(data.columns):
            self.list.InsertColumn(col, text)

        # add the rows
        self.itemDataMap = {}
        for item in data.rows:
            index = self.list.InsertStringItem(sys.maxint, item[0])
            for col, text in enumerate(item[1:]):
                self.list.SetStringItem(index, col+1, text)

            # give each item a data value, and map it back to the
            # item values, for the column sorter
            self.list.SetItemData(index, index)
            self.itemDataMap[index] = item
            
            # give each item a random image
            img = random.randint(0, il_max)
            self.list.SetItemImage(index, img, img)
                
        # set the width of the columns in various ways
        self.list.SetColumnWidth(0, 120)
        self.list.SetColumnWidth(1, wx.LIST_AUTOSIZE)
        self.list.SetColumnWidth(2, wx.LIST_AUTOSIZE)
        self.list.SetColumnWidth(3, wx.LIST_AUTOSIZE_USEHEADER)

        # initialize the column sorter
        wx.lib.mixins.listctrl.ColumnSorterMixin.__init__(self,
                                                          len(data.columns))

    def GetListCtrl(self):
        return self.list

    def GetSortImages(self):
        return (self.dn, self.up)


app = wx.PySimpleApp()
frame = DemoFrame()
frame.Show()
app.MainLoop()
