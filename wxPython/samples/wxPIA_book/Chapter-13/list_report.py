import wx
import sys, glob, random
import data

class DemoFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1,
                          "wx.ListCtrl in wx.LC_REPORT mode",
                          size=(600,400))

        il = wx.ImageList(16,16, True)
        for name in glob.glob("smicon??.png"):
            bmp = wx.Bitmap(name, wx.BITMAP_TYPE_PNG)
            il_max = il.Add(bmp)
        self.list = wx.ListCtrl(self, -1, style=wx.LC_REPORT)
        self.list.AssignImageList(il, wx.IMAGE_LIST_SMALL)

        # Add some columns
        for col, text in enumerate(data.columns):
            self.list.InsertColumn(col, text)

        # add the rows
        for item in data.rows:
            index = self.list.InsertStringItem(sys.maxint, item[0])
            for col, text in enumerate(item[1:]):
                self.list.SetStringItem(index, col+1, text)

            # give each item a random image
            img = random.randint(0, il_max)
            self.list.SetItemImage(index, img, img)
                
        # set the width of the columns in various ways
        self.list.SetColumnWidth(0, 120)
        self.list.SetColumnWidth(1, wx.LIST_AUTOSIZE)
        self.list.SetColumnWidth(2, wx.LIST_AUTOSIZE)
        self.list.SetColumnWidth(3, wx.LIST_AUTOSIZE_USEHEADER)


app = wx.PySimpleApp()
frame = DemoFrame()
frame.Show()
app.MainLoop()
