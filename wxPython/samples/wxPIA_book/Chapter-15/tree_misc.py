import wx
import data

class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="tree: misc tests", size=(400,500))

        # Create an image list
        il = wx.ImageList(16,16)

        # Get some standard images from the art provider and add them
        # to the image list
        self.fldridx = il.Add(
            wx.ArtProvider.GetBitmap(wx.ART_FOLDER, wx.ART_OTHER, (16,16)))
        self.fldropenidx = il.Add(
            wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN,   wx.ART_OTHER, (16,16)))
        self.fileidx = il.Add(
            wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE, wx.ART_OTHER, (16,16)))
        

        # Create the tree
        self.tree = wx.TreeCtrl(self, style=wx.TR_DEFAULT_STYLE | wx.TR_EDIT_LABELS)

        # Give it the image list
        self.tree.AssignImageList(il)

        
        # Add a root node and assign it some images
        root = self.tree.AddRoot("wx.Object")
        self.tree.SetItemPyData(root, None)
        self.tree.SetItemImage(root, self.fldridx,
                               wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(root, self.fldropenidx,
                               wx.TreeItemIcon_Expanded)
        
        # Add nodes from our data set
        self.AddTreeNodes(root, data.tree)

        # Bind some interesting events
        self.Bind(wx.EVT_TREE_ITEM_EXPANDED, self.OnItemExpanded, self.tree)
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.OnItemCollapsed, self.tree)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged, self.tree)
        self.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.OnActivated, self.tree)

        self.Bind(wx.EVT_TREE_BEGIN_LABEL_EDIT, self.OnBeginEdit, self.tree)
        

        # Expand the first level
        self.tree.Expand(root)


        menu = wx.Menu()
        mi = menu.Append(-1, "Edit Item")
        self.Bind(wx.EVT_MENU, self.OnEditItem, mi)
        mi = menu.Append(-1, "Sort Children")
        self.Bind(wx.EVT_MENU, self.OnSortChildren, mi)
        mi = menu.Append(-1, "Delete Children")
        self.Bind(wx.EVT_MENU, self.OnDeleteChildren, mi)
        mi = menu.Append(-1, "Find all Children")
        self.Bind(wx.EVT_MENU, self.OnFindChildren, mi)
        mb = wx.MenuBar()
        mb.Append(menu, "Tests")
        self.SetMenuBar(mb)


    def AddTreeNodes(self, parentItem, items):
        """
        Recursively traverses the data structure, adding tree nodes to
        match it.
        """
        for item in items:
            if type(item) == str:
                newItem = self.tree.AppendItem(parentItem, item)
                self.tree.SetItemPyData(newItem, None)
                self.tree.SetItemImage(newItem, self.fileidx,
                                       wx.TreeItemIcon_Normal)
            else:
                newItem = self.tree.AppendItem(parentItem, item[0])
                self.tree.SetItemPyData(newItem, None)
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
        
    def OnItemExpanded(self, evt):
        print "OnItemExpanded: ", self.GetItemText(evt.GetItem())
        
    def OnItemCollapsed(self, evt):
        print "OnItemCollapsed:", self.GetItemText(evt.GetItem())

    def OnSelChanged(self, evt):
        print "OnSelChanged:   ", self.GetItemText(evt.GetItem())

    def OnActivated(self, evt):
        print "OnActivated:    ", self.GetItemText(evt.GetItem())


    def OnBeginEdit(self, evt):
        print "OnBeginEdit:    ", self.GetItemText(evt.GetItem())
        # we can prevent nodes from being edited, for example let's
        # not let the root node be edited...
        item = evt.GetItem()
        if item == self.tree.GetRootItem():
            evt.Veto()
            print "*** Edit was vetoed!"
            
        
    def OnEditItem(self, evt):
        item = self.tree.GetSelection()
        if item:
            self.tree.EditLabel(item)
            
    def OnSortChildren(self, evt):
        item = self.tree.GetSelection()
        if item:
            self.tree.SortChildren(item)

    def OnDeleteChildren(self, evt):
        item = self.tree.GetSelection()
        if item:
            self.tree.DeleteChildren(item)

    def OnFindChildren(self, evt):
        item = self.tree.GetSelection()
        if item:
            self.PrintAllItems(item)

    def PrintAllItems(self, parent, indent=0):
        text = self.tree.GetItemText(parent)
        print ' ' * indent, text
        indent += 4
        item, cookie = self.tree.GetFirstChild(parent)
        while item:
            if self.tree.ItemHasChildren(item):
                self.PrintAllItems(item, indent)
            else:
                text = self.tree.GetItemText(item)
                print ' ' * indent, text
            item, cookie = self.tree.GetNextChild(parent, cookie)


app = wx.PySimpleApp(redirect=True)
frame = TestFrame()
frame.Show()
app.MainLoop()

