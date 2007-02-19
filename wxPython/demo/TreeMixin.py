import wx, wx.lib.customtreectrl, wx.gizmos
import wx.lib.mixins.treemixin as treemixin


class TreeModel(object):
    def __init__(self, *args, **kwargs):
        self.items = []
        self.itemCounter = 0
        super(TreeModel, self).__init__(*args, **kwargs)

    def GetItem(self, indices):
        text, children = 'Hidden root', self.items
        for index in indices:
            text, children = children[index]
        return text, children

    def GetText(self, indices):
        return self.GetItem(indices)[0]

    def GetChildren(self, indices):
        return self.GetItem(indices)[1]

    def GetChildrenCount(self, indices):
        return len(self.GetChildren(indices))

    def SetChildrenCount(self, indices, count):
        children = self.GetChildren(indices)
        while len(children) > count:
            children.pop()
        while len(children) < count:
            children.append(('item %d'%self.itemCounter, []))
            self.itemCounter += 1

    def MoveItem(self, itemToMoveIndices, newParentIndices):
        itemToMove = self.GetItem(itemToMoveIndices)
        newParentChildren = self.GetChildren(newParentIndices)
        newParentChildren.append(itemToMove)
        oldParentChildren = self.GetChildren(itemToMoveIndices[:-1])
        oldParentChildren.remove(itemToMove)


class DemoTreeMixin(treemixin.VirtualTree, treemixin.DragAndDrop, 
                    treemixin.ExpansionState):
    def __init__(self, *args, **kwargs):
        self.model = kwargs.pop('treemodel')
        self.log = kwargs.pop('log')
        super(DemoTreeMixin, self).__init__(*args, **kwargs)
        self.CreateImageList()

    def CreateImageList(self):
        size = (16, 16)
        self.imageList = wx.ImageList(*size)
        for art in wx.ART_FOLDER, wx.ART_FILE_OPEN, wx.ART_NORMAL_FILE:
            self.imageList.Add(wx.ArtProvider.GetBitmap(art, wx.ART_OTHER, 
                                                        size))
        self.AssignImageList(self.imageList)

    def OnGetItemText(self, indices):
        return self.model.GetText(indices)

    def OnGetChildrenCount(self, indices):
        return self.model.GetChildrenCount(indices)

    def OnGetItemFont(self, indices):
        if self.model.GetChildrenCount(indices) > 0:
            return wx.SMALL_FONT
        else:
            return super(DemoTreeMixin, self).OnGetItemFont(indices)

    def OnGetItemTextColour(self, indices):
        if len(indices) % 2 == 0:
            return wx.RED
        elif len(indices) % 3 == 0:
            return wx.BLUE
        else:
            return super(DemoTreeMixin, self).OnGetItemTextColour(indices)

    def OnGetItemBackgroundColour(self, indices):
        if indices[-1] == len(indices):
            return wx.GREEN
        else: 
            return super(DemoTreeMixin, 
                         self).OnGetItemBackgroundColour(indices)

    def OnGetItemImage(self, indices, which):
        if which in [wx.TreeItemIcon_Normal, wx.TreeItemIcon_Selected]:
            if self.model.GetChildrenCount(indices):
                return 0
            else:
                return 2
        else:
            return 1

    def OnDrop(self, dropTarget, dragItem):
        dropIndices = self.ItemIndices(dropTarget)
        dropText = self.model.GetText(dropIndices)
        dragIndices = self.ItemIndices(dragItem)
        dragText = self.model.GetText(dragIndices)
        self.log.write('drop %s %s on %s %s'%(dragText, dragIndices,
            dropText, dropIndices))
        self.model.MoveItem(dragIndices, dropIndices)
        self.GetParent().RefreshItems()


class VirtualTreeCtrl(DemoTreeMixin, wx.TreeCtrl):
    pass


class VirtualTreeListCtrl(DemoTreeMixin, wx.gizmos.TreeListCtrl):
    def __init__(self, *args, **kwargs):
        kwargs['style'] = wx.TR_DEFAULT_STYLE | wx.TR_FULL_ROW_HIGHLIGHT
        super(VirtualTreeListCtrl, self).__init__(*args, **kwargs)
        self.AddColumn('Column 0')
        self.AddColumn('Column 1')
        for art in wx.ART_TIP, wx.ART_WARNING:
            self.imageList.Add(wx.ArtProvider.GetBitmap(art, wx.ART_OTHER, 
                                                        (16, 16)))

    def OnGetItemText(self, indices, column=0):
        return '%s, column %d'%\
            (super(VirtualTreeListCtrl, self).OnGetItemText(indices), column)

    def OnGetItemImage(self, indices, which, column=0):
        if column == 0:
            return super(VirtualTreeListCtrl, self).OnGetItemImage(indices, 
                                                                   which)
        elif self.OnGetChildrenCount(indices):
            return 4
        else:
            return 3


class VirtualCustomTreeCtrl(DemoTreeMixin, 
                            wx.lib.customtreectrl.CustomTreeCtrl):
    def __init__(self, *args, **kwargs):
        self.checked = {}
        kwargs['ctstyle'] = wx.TR_DEFAULT_STYLE | wx.TR_HIDE_ROOT | \
                            wx.TR_HAS_BUTTONS | wx.TR_FULL_ROW_HIGHLIGHT
        super(VirtualCustomTreeCtrl, self).__init__(*args, **kwargs)
        self.Bind(wx.lib.customtreectrl.EVT_TREE_ITEM_CHECKED,
                  self.OnItemChecked)

    def OnGetItemType(self, indices):
        if len(indices) == 1:
            return 1
        elif len(indices) == 2:
            return 2
        else:
            return 0

    def OnGetItemChecked(self, indices):
        return self.checked.get(tuple(indices), False)

    def OnItemChecked(self, event):
        item = event.GetItem()
        indices = tuple(self.ItemIndices(item))
        if self.GetItemType(item) == 2: 
            # It's a radio item; reset other items on the same level
            for index in range(self.GetChildrenCount(self.GetItemParent(item))):
                self.checked[indices[:-1]+(index,)] = False
        self.checked[indices] = True



class TreeNotebook(wx.Notebook):
    def __init__(self, *args, **kwargs):
        treemodel = kwargs.pop('treemodel')
        log = kwargs.pop('log')
        super(TreeNotebook, self).__init__(*args, **kwargs)
        self.trees = []
        for class_, title in [(VirtualTreeCtrl, 'TreeCtrl'),
                              (VirtualTreeListCtrl, 'TreeListCtrl'),
                              (VirtualCustomTreeCtrl, 'CustomTreeCtrl')]:
            tree = class_(self, treemodel=treemodel, log=log)
            self.trees.append(tree)
            self.AddPage(tree, title)
        self.RefreshItems()

    def RefreshItems(self):
        for tree in self.trees:
            tree.RefreshItems()
            tree.UnselectAll()

    def GetSelectedItemIndices(self):
        tree = self.trees[self.GetSelection()]
        if tree.GetSelection():
            return tree.ItemIndices(tree.GetSelection())
        else:
            return []


class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        super(TestPanel, self).__init__(parent)
        self.treemodel = TreeModel()
        self.CreateControls()
        self.LayoutControls()
        self.BindEvents()

    def CreateControls(self):
        self.notebook = TreeNotebook(self, treemodel=self.treemodel, 
                                     log=self.log)
        self.label = wx.StaticText(self, label='Number of children: ')
        self.childrenCountCtrl = wx.SpinCtrl(self, value='0', max=10000)
        self.button = wx.Button(self, label='Update children')

    def LayoutControls(self):
        hSizer = wx.BoxSizer(wx.HORIZONTAL)
        options = dict(flag=wx.ALIGN_CENTER_VERTICAL|wx.ALL, border=2)
        hSizer.Add(self.label, **options)
        hSizer.Add(self.childrenCountCtrl, 2, **options)
        hSizer.Add(self.button, **options)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.notebook, 1, wx.EXPAND)
        sizer.Add(hSizer, 0, wx.EXPAND)
        self.SetSizer(sizer)

    def BindEvents(self):
        self.notebook.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.OnPageChanged)
        self.button.Bind(wx.EVT_BUTTON, self.OnEnter)

    def OnPageChanged(self, event):
        if self.IsBeingDeleted():
            return
        oldTree = self.notebook.GetPage(event.OldSelection)
        newTree = self.notebook.GetPage(event.Selection)
        newTree.SetExpansionState(oldTree.GetExpansionState())
        event.Skip()

    def OnEnter(self, event):
        indices = self.notebook.GetSelectedItemIndices()
        text = self.treemodel.GetText(indices)
        oldChildrenCount = self.treemodel.GetChildrenCount(indices)
        newChildrenCount = self.childrenCountCtrl.GetValue()
        self.log.write('%s %s now has %d children (was %d)'%(text, indices,
            newChildrenCount, oldChildrenCount))
        self.treemodel.SetChildrenCount(indices, newChildrenCount)
        self.notebook.RefreshItems()


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


if __name__ == '__main__':
    import sys, os, run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

