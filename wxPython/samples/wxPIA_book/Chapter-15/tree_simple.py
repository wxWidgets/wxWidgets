import wx
import data

class TestFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="simple tree", size=(400,500))

        # Create the tree
        self.tree = wx.TreeCtrl(self)

        # Add a root node
        root = self.tree.AddRoot("wx.Object")

        # Add nodes from our data set
        self.AddTreeNodes(root, data.tree)

        # Bind some interesting events
        self.Bind(wx.EVT_TREE_ITEM_EXPANDED, self.OnItemExpanded, self.tree)
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.OnItemCollapsed, self.tree)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged, self.tree)
        self.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.OnActivated, self.tree)

        # Expand the first level
        self.tree.Expand(root)


    def AddTreeNodes(self, parentItem, items):
        """
        Recursively traverses the data structure, adding tree nodes to
        match it.
        """
        for item in items:
            if type(item) == str:
                self.tree.AppendItem(parentItem, item)
            else:
                newItem = self.tree.AppendItem(parentItem, item[0])
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


app = wx.PySimpleApp(redirect=True)
frame = TestFrame()
frame.Show()
app.MainLoop()

