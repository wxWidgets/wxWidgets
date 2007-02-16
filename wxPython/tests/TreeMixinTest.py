import wx, wx.gizmos, wx.lib.customtreectrl, unittest, treemixin


class VirtualTreeCtrl(treemixin.VirtualTree, wx.TreeCtrl):
    def __init__(self, *args, **kwargs):
        self.children = {}
        super(VirtualTreeCtrl, self).__init__(*args, **kwargs)

    def OnGetItemText(self, indices):
        return 'item %s'%'.'.join([str(index) for index in indices])

    def OnGetChildrenCount(self, indices=None):
        indices = indices or []
        return self.children.get(tuple(indices), 0)

    def SetChildrenCount(self, indices, childrenCount):
        self.children[tuple(indices)] = childrenCount 


class VirtualTreeCtrlTest_NoRootItems(unittest.TestCase):
    def setUp(self):
        self.frame = wx.Frame(None)
        self.tree = VirtualTreeCtrl(self.frame)
        self.tree.RefreshItems()

    def testNoRootItems(self):
        self.assertEqual(0, self.tree.GetCount())

    def testAddTwoRootItems(self):
        self.tree.SetChildrenCount([], 2)
        self.tree.RefreshItems()
        self.assertEqual(2, self.tree.GetCount())

    def testAddOneRootItemAndOneChild(self):
        self.tree.SetChildrenCount([], 1)
        self.tree.SetChildrenCount([0], 1)
        self.tree.RefreshItems()
        self.tree.ExpandAll()
        self.assertEqual(2, self.tree.GetCount())

    def testAddOneRootItemAndTwoChildren(self):
        self.tree.SetChildrenCount([], 1)
        self.tree.SetChildrenCount([0], 2)
        self.tree.RefreshItems()
        self.tree.ExpandAll()
        self.assertEqual(3, self.tree.GetCount())


class VirtualTreeCtrlTest_OneRoot(unittest.TestCase):
    def setUp(self):
        self.frame = wx.Frame(None)
        self.tree = VirtualTreeCtrl(self.frame)
        self.tree.SetChildrenCount([], 1)
        self.tree.RefreshItems()

    def testOneRoot(self):
        self.assertEqual(1, self.tree.GetCount())

    def testDeleteRootItem(self):
        self.tree.SetChildrenCount([], 0)
        self.tree.RefreshItems()
        self.assertEqual(0, self.tree.GetCount())

    def testAddOneChild(self):
        self.tree.SetChildrenCount([0], 1)
        self.tree.RefreshItems()
        self.tree.ExpandAll()
        self.assertEqual(2, self.tree.GetCount())

    def testAddTwoChildren(self):
        self.tree.SetChildrenCount([0], 2)
        self.tree.RefreshItems()
        self.tree.ExpandAll()
        self.assertEqual(3, self.tree.GetCount())



class TreeAPIHarmonizerTestCase(unittest.TestCase):
    style = wx.TR_DEFAULT_STYLE

    def setUp(self):
        self.frame = wx.Frame(None)
        class HarmonizedTreeCtrl(treemixin.TreeAPIHarmonizer, self.TreeClass):
            pass
        self.tree = HarmonizedTreeCtrl(self.frame, style=self.style)
        self.populateTree()

    def populateTree(self):
        self.root = self.tree.AddRoot('Root')
        self.item = self.tree.AppendItem(self.root, 'item')


class TreeAPIHarmonizerCommonTests(object):
    def testSetItemImage(self):
        self.tree.SetItemImage(self.item, -1, wx.TreeItemIcon_Normal)
        self.assertEqual(-1, 
            self.tree.GetItemImage(self.item, wx.TreeItemIcon_Normal))

    def testGetColumnCount(self):
        self.assertEqual(0, self.tree.GetColumnCount())

    def testGetItemType(self):
        self.assertEqual(0, self.tree.GetItemType(self.item))

    def testGetMainWindow(self):
        self.assertEqual(self.tree, self.tree.GetMainWindow())


class TreeAPIHarmonizerSingleSelectionTests(object):
    def testUnselectAll(self):
        self.tree.SelectItem(self.item)
        self.tree.UnselectAll()
        self.assertEqual([], self.tree.GetSelections())

    def testGetSelections_NoSelection(self):
        self.tree.UnselectAll()
        self.assertEqual([], self.tree.GetSelections())

    def testGetSelections_OneSelectedItem(self):
        self.tree.UnselectAll()
        self.tree.SelectItem(self.item)
        self.assertEqual([self.item], self.tree.GetSelections())


class TreeAPIHarmonizerMultipleSelectionTests(object):
    style = wx.TR_DEFAULT_STYLE | wx.TR_MULTIPLE
    
    def testUnselectAll(self):
        self.tree.SelectItem(self.item)
        self.tree.UnselectAll()
        self.assertEqual([], self.tree.GetSelections())

    def testGetSelections_NoSelection(self):
        self.tree.UnselectAll()
        self.assertEqual([], self.tree.GetSelections())

    def testGetSelections_OneSelectedItem(self):
        self.tree.UnselectAll()
        self.tree.SelectItem(self.item)
        self.assertEqual([self.item], self.tree.GetSelections())

    def testGetSelections_TwoSelectedItems(self):
        self.tree.UnselectAll()
        item2 = self.tree.AppendItem(self.root, 'item 2')
        self.tree.SelectItem(self.item)
        self.tree.SelectItem(item2)
        self.assertEqual([self.item, item2], self.tree.GetSelections())


class TreeAPIHarmonizerWithTreeCtrlTestCase(TreeAPIHarmonizerTestCase):
    TreeClass = wx.TreeCtrl

class TreeAPIHarmonizerWithTreeCtrl_SingleSelection( \
        TreeAPIHarmonizerCommonTests, TreeAPIHarmonizerSingleSelectionTests,
        TreeAPIHarmonizerWithTreeCtrlTestCase):
    pass

class TreeAPIHarmonizerWithTreeCtrl_MultipleSelection( \
        TreeAPIHarmonizerCommonTests, TreeAPIHarmonizerMultipleSelectionTests, 
        TreeAPIHarmonizerWithTreeCtrlTestCase):
    pass


class TreeAPIHarmonizerWithTreeListCtrlTestCase(TreeAPIHarmonizerTestCase):
    TreeClass = wx.gizmos.TreeListCtrl

    def populateTree(self):
        self.tree.AddColumn('Column')
        super(TreeAPIHarmonizerWithTreeListCtrlTestCase, self).populateTree()


class TreeAPIHarmonizerWithTreeListCtrl_SingleSelection( \
        TreeAPIHarmonizerCommonTests, TreeAPIHarmonizerSingleSelectionTests,
        TreeAPIHarmonizerWithTreeListCtrlTestCase):

    def testGetColumnCount(self):
        self.assertEqual(1, self.tree.GetColumnCount())

    def testGetMainWindow(self):
        self.assertNotEqual(self.tree, self.tree.GetMainWindow())


class TreeAPIHarmonizerWithTreeListCtrl_MultipleSelection( \
        TreeAPIHarmonizerCommonTests, TreeAPIHarmonizerMultipleSelectionTests,
        TreeAPIHarmonizerWithTreeListCtrlTestCase):

    def testGetColumnCount(self):
        self.assertEqual(1, self.tree.GetColumnCount())

    def testGetMainWindow(self):
        self.assertNotEqual(self.tree, self.tree.GetMainWindow())

    def testGetSelections_TwoSelectedItems(self):
        ''' Override TreeAPIHarmonizerMultipleSelectionTests.-
            testGetSelections_TwoSelectedItems, because 
            TreeListCtrl.SelectItem needs an extra parameter. '''
        self.tree.UnselectAll()
        item2 = self.tree.AppendItem(self.root, 'item 2')
        self.tree.SelectItem(self.item)
        self.tree.SelectItem(item2, unselect_others=False)
        self.assertEqual([self.item, item2], self.tree.GetSelections())


class TreeAPIHarmonizerWithCustomTreeCtrlTestCase(TreeAPIHarmonizerTestCase):
    TreeClass = wx.lib.customtreectrl.CustomTreeCtrl


class TreeAPIHarmonizerCustomTreeCtrlTests(object):
    def testGetCheckItemType(self):
        item = self.tree.AppendItem(self.root, 'item', ct_type=1)
        self.assertEqual(1, self.tree.GetItemType(item))

    def testGetRadioItemType(self):
        item = self.tree.AppendItem(self.root, 'item', ct_type=2)
        self.assertEqual(2, self.tree.GetItemType(item))


class TreeAPIHarmonizerWithCustomTreeCtrl_SingleSelection( \
        TreeAPIHarmonizerCommonTests, TreeAPIHarmonizerSingleSelectionTests,
        TreeAPIHarmonizerCustomTreeCtrlTests,
        TreeAPIHarmonizerWithCustomTreeCtrlTestCase):
    pass


class TreeAPIHarmonizerWithCustomTreeCtrl_MultipleSelection( \
        TreeAPIHarmonizerCommonTests, TreeAPIHarmonizerMultipleSelectionTests,
        TreeAPIHarmonizerCustomTreeCtrlTests,
        TreeAPIHarmonizerWithCustomTreeCtrlTestCase):
    pass



if __name__ == '__main__':
    app = wx.App(False)
    unittest.main()


