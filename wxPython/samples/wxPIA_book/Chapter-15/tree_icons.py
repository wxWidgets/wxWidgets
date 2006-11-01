import wx
import data

class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, 
                title="simple tree with icons", size=(400,500))

        # Create an image list
        il = wx.ImageList(16,16)

        # Get some standard images from the art provider and add them
        # to the image list
        self.fldridx = il.Add(
            wx.ArtProvider.GetBitmap(wx.ART_FOLDER, 
                    wx.ART_OTHER, (16,16)))
        self.fldropenidx = il.Add(
            wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN,   
                    wx.ART_OTHER, (16,16)))
        self.fileidx = il.Add(
            wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE, 
                    wx.ART_OTHER, (16,16)))
        

        # Create the tree
        self.tree = wx.TreeCtrl(self)
        # Give it the image list
        self.tree.AssignImageList(il)
        root = self.tree.AddRoot("wx.Object")
        self.tree.SetItemImage(root, self.fldridx,
                               wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(root, self.fldropenidx,
                               wx.TreeItemIcon_Expanded)
        
        self.AddTreeNodes(root, data.tree)
        self.tree.Expand(root)
        

    def AddTreeNodes(self, parentItem, items):
        for item in items:
            if type(item) == str:
                newItem = self.tree.AppendItem(parentItem, item)
                self.tree.SetItemImage(newItem, self.fileidx,
                                       wx.TreeItemIcon_Normal)
            else:
                newItem = self.tree.AppendItem(parentItem, item[0])
                self.tree.SetItemImage(newItem, self.fldridx,
                                       wx.TreeItemIcon_Normal)
                self.tree.SetItemImage(newItem, self.fldropenidx,
                                       wx.TreeItemIcon_Expanded)
   
                self.AddTreeNodes(newItem, item[1])
                

    def GetItemText(self, item):
        if item:
            return self.tree.GetItemText(item)
        else:
            return ""
      
app = wx.PySimpleApp(redirect=True)
frame = TestFrame()
frame.Show()
app.MainLoop()

