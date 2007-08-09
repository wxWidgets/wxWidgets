import wx
import data

class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="virtual tree with icons", size=(400,500))
        il = wx.ImageList(16,16)
        self.fldridx = il.Add(
            wx.ArtProvider.GetBitmap(wx.ART_FOLDER, wx.ART_OTHER, (16,16)))
        self.fldropenidx = il.Add(
            wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN,   wx.ART_OTHER, (16,16)))
        self.fileidx = il.Add(
            wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE, wx.ART_OTHER, (16,16)))
        self.tree = wx.TreeCtrl(self)
        self.tree.AssignImageList(il)
        root = self.tree.AddRoot("wx.Object")
        self.tree.SetItemImage(root, self.fldridx,
                               wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(root, self.fldropenidx,
                               wx.TreeItemIcon_Expanded)
        
        # Instead of adding nodes for the whole tree, just attach some
        # data to the root node so that it can find and add its child
        # nodes when it is expanded, and mark it as having children so
        # it will be expandable.
        self.tree.SetItemPyData(root, data.tree)
        self.tree.SetItemHasChildren(root, True)
        
        # Bind some interesting events
        self.Bind(wx.EVT_TREE_ITEM_EXPANDED, self.OnItemExpanded, self.tree)
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.OnItemCollapsed, self.tree)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged, self.tree)
        self.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.OnActivated, self.tree)

        self.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.OnItemExpanding, self.tree)
        self.tree.Expand(root)
        

    def AddTreeNodes(self, parentItem):
        """
        Add nodes for just the children of the parentItem
        """
        items = self.tree.GetItemPyData(parentItem)
        for item in items:
            if type(item) == str:
                # a leaf node
                newItem = self.tree.AppendItem(parentItem, item)
                self.tree.SetItemImage(newItem, self.fileidx,
                                       wx.TreeItemIcon_Normal)
            else:
                # this item has children
                newItem = self.tree.AppendItem(parentItem, item[0])
                self.tree.SetItemImage(newItem, self.fldridx,
                                       wx.TreeItemIcon_Normal)
                self.tree.SetItemImage(newItem, self.fldropenidx,
                                       wx.TreeItemIcon_Expanded)
                self.tree.SetItemPyData(newItem, item[1])
                self.tree.SetItemHasChildren(newItem, True)
   
                

    def GetItemText(self, item):
        if item:
            return self.tree.GetItemText(item)
        else:
            return ""
        
    def OnItemExpanded(self, evt):
        print "OnItemExpanded: ", self.GetItemText(evt.GetItem())

    def OnItemExpanding(self, evt):
        # When the item is about to be expanded add the first level of child nodes
        print "OnItemExpanding:", self.GetItemText(evt.GetItem())
        self.AddTreeNodes(evt.GetItem())
        
    def OnItemCollapsed(self, evt):
        print "OnItemCollapsed:", self.GetItemText(evt.GetItem())
        # And remove them when collapsed as we don't need them any longer
        self.tree.DeleteChildren(evt.GetItem())

    def OnSelChanged(self, evt):
        print "OnSelChanged:   ", self.GetItemText(evt.GetItem())

    def OnActivated(self, evt):
        print "OnActivated:    ", self.GetItemText(evt.GetItem())


app = wx.PySimpleApp(redirect=True)
frame = TestFrame()
frame.Show()
app.MainLoop()

