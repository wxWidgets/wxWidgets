

import wx, unittest
from wx.lib.combotreebox import ComboTreeBox, IterableTreeCtrl


class ComboTreeBoxTest(unittest.TestCase):
    def setUp(self):
        self.comboBoxEventReceived = False
        frame = wx.Frame(None)
        self.comboBox = ComboTreeBox(frame, platform=platform)
        self.tree = self.comboBox._popupFrame.GetTree()

    def onComboBox(self, event):
        self.comboBoxEventReceived = True

    def testComboBoxIsEmptyByDefault(self):
        self.assertEqual(0, self.comboBox.GetCount())

    def testAddingOneItem(self):
        self.comboBox.Append('Item 1')
        self.assertEqual(1, self.comboBox.GetCount())

    def testAddingTwoItems(self):
        self.comboBox.Append('Item 1')
        self.comboBox.Append('Item 2')
        self.assertEqual(2, self.comboBox.GetCount())

    def testAddingTwoParentAndChild(self):
        item1 = self.comboBox.Append('Item 1')
        self.comboBox.Append('Item 2', item1)
        self.assertEqual(2, self.comboBox.GetCount())

    def testSelectingAnItemPutsItInTheComboBox(self):
        self.comboBox.Append('Item 1')
        self.comboBox.Bind(wx.EVT_COMBOBOX, self.onComboBox)
        self.comboBox.NotifyItemSelected('Item 1')
        self.failUnless(self.comboBoxEventReceived)

    def testClear(self):
        self.comboBox.Append('Item 1')
        self.comboBox.Clear()
        self.assertEqual(0, self.comboBox.GetCount())

    def testDelete(self):
        self.comboBox.Append('Item 1')
        self.comboBox.Delete(self.tree.GetFirstItem())
        self.assertEqual(0, self.comboBox.GetCount())

    def testGetSelection_NoItems(self):
        self.failIf(self.comboBox.GetSelection().IsOk())

    def testGetSelection_NoSelection(self):
        self.comboBox.Append('Item 1')
        self.failIf(self.comboBox.GetSelection().IsOk())

    def testGetSelection_WithSelection(self):
        item1 = self.comboBox.Append('Item 1')
        self.comboBox.SetValue('Item 1')
        self.assertEqual(item1, self.comboBox.GetSelection())

    def testGetSelection_EquallyNamedNodes_SelectedInTree(self):
        item1 = self.comboBox.Append('Item')
        item2 = self.comboBox.Append('Item')
        self.tree.SelectItem(item2)
        self.assertEqual(self.tree.GetSelection(), self.comboBox.GetSelection())

    def testGetSelection_EquallyNamedNodes_TypedInTextBox(self):
        item1 = self.comboBox.Append('Item')
        item2 = self.comboBox.Append('Item')
        self.comboBox.SetValue('Item')
        self.assertEqual(item1, self.comboBox.GetSelection())

    def testFindString_NotPresent(self):
        self.comboBox.Append('Item 1')
        self.failIf(self.comboBox.FindString('Item 2').IsOk())

    def testFindString_Present(self):
        self.comboBox.Append('Item 1')
        self.assertEqual(self.tree.GetFirstItem(),
                         self.comboBox.FindString('Item 1'))

    def testFindString_Child(self):
        parent = self.comboBox.Append('Parent')
        child = self.comboBox.Append('Child', parent=parent)
        self.assertEqual(child, self.comboBox.FindString('Child'))

    def testGetString_NotPresent(self):
        self.assertEqual('', self.comboBox.GetString(self.tree.GetFirstItem()))

    def testGetString_Present(self):
        self.comboBox.Append('Item 1')
        self.assertEqual('Item 1', 
            self.comboBox.GetString(self.tree.GetFirstItem()))

    def testGetStringSelection_NotPresent(self):
        self.assertEqual('', self.comboBox.GetStringSelection())

    def testGetStringSelection_Present(self):
        self.comboBox.SetValue('Item 1')
        self.assertEqual('Item 1', self.comboBox.GetStringSelection())

    def testInsertAsFirstItem(self):
        self.comboBox.Insert('Item 1')
        self.assertEqual('Item 1', 
            self.comboBox.GetString(self.tree.GetFirstItem()))

    def testInsertAsFirstItemBeforeExistingItem(self):
        item1 = self.comboBox.Append('Item 1')
        item2 = self.comboBox.Insert('Item 2')
        self.assertEqual(item2, self.tree.GetFirstItem())

    def testInsertAsFirstChildBeforeExistingChild(self):
        parent = self.comboBox.Append('parent')
        child1 = self.comboBox.Append('child 1', parent)
        child2 = self.comboBox.Insert('child 2', parent=parent)
        self.assertEqual(child2, self.tree.GetFirstChild(parent)[0])

    def testSelect(self):
        item1 = self.comboBox.Append('Item 1')
        self.comboBox.Select(item1)
        self.assertEqual('Item 1', self.comboBox.GetValue())

    def testSetString(self):
        item1 = self.comboBox.Append('Item 1')
        self.comboBox.SetString(item1, 'Item 2')
        self.assertEqual('Item 2', self.comboBox.GetString(item1))

    def testSetStringSelection_ExistingString(self):
        self.comboBox.Append('Hi')
        self.comboBox.SetStringSelection('Hi')
        self.assertEqual('Hi', self.comboBox.GetStringSelection())

    def testSetStringSelection_NonExistingString(self):
        self.comboBox.SetStringSelection('Hi')
        self.assertEqual('', self.comboBox.GetStringSelection())

    def testAppendWithClientData(self):
        item1 = self.comboBox.Append('Item 1', clientData=[1,2,3])
        self.assertEqual([1,2,3], self.comboBox.GetClientData(item1))

    def testInsertWithClientData(self):
        item1 = self.comboBox.Append('Item 1')
        item2 = self.comboBox.Insert('Item 2', previous=item1, 
                                     clientData=[1,2,3])
        self.assertEqual([1,2,3], self.comboBox.GetClientData(item2))

    def testSetClientData(self):
        item1 = self.comboBox.Append('Item 1')
        self.comboBox.SetClientData(item1, [1,2,3])
        self.assertEqual([1,2,3], self.comboBox.GetClientData(item1))

    def testFindClientData(self):
        item1 = self.comboBox.Append('Item 1', clientData='A')
        self.assertEqual(item1, self.comboBox.FindClientData('A'))

    def testFindClientData_NoItems(self):
        self.failIf(self.comboBox.FindClientData('A'))

    def testFindClientData_NoSuchData(self):
        item1 = self.comboBox.Append('Item 1', clientData='A')
        self.failIf(self.comboBox.FindClientData('B'))

    def testSetClientDataSelection(self):
        item1 = self.comboBox.Append('Item 1', clientData='A')
        self.comboBox.SetClientDataSelection('A')
        self.assertEqual(item1, self.comboBox.GetSelection())

    def testSetClientDataSelection_NoSuchData(self):
        item1 = self.comboBox.Append('Item 1', clientData='A')
        self.comboBox.SetClientDataSelection('B')
        self.failIf(self.comboBox.GetSelection())


class SortedComboTreeBoxTest(unittest.TestCase):
    def setUp(self):
        frame = wx.Frame(None)
        self.comboBox = ComboTreeBox(frame, style=wx.CB_SORT, platform=platform)
        self.tree = self.comboBox._popupFrame.GetTree()

    def testAppend(self):
        itemB = self.comboBox.Append('B')
        itemA = self.comboBox.Append('A')
        self.assertEqual(itemA, self.tree.GetFirstItem())

    def testInsert(self):
        itemA = self.comboBox.Append('A')
        itemB = self.comboBox.Insert('B')
        self.assertEqual(itemA, self.tree.GetFirstItem())

    def testAppend_Child(self):
        itemA = self.comboBox.Append('A')
        itemA2 = self.comboBox.Append('2', parent=itemA)
        itemA1 = self.comboBox.Append('1', parent=itemA)
        self.assertEqual(itemA1, self.tree.GetFirstChild(itemA)[0])

    def testInsert_Child(self):
        itemA = self.comboBox.Append('A')
        itemA1 = self.comboBox.Append('1', parent=itemA)
        itemA2 = self.comboBox.Insert('2', parent=itemA)
        self.assertEqual(itemA1, self.tree.GetFirstChild(itemA)[0])
        
    def testSetString(self):
        itemB = self.comboBox.Append('B')
        itemC = self.comboBox.Append('C')
        self.comboBox.SetString(itemC, 'A')
        self.assertEqual(itemC, self.tree.GetFirstItem())


class ReadOnlyComboTreeBoxTest(unittest.TestCase):
    def setUp(self):
        frame = wx.Frame(None)
        self.comboBox = ComboTreeBox(frame, style=wx.CB_READONLY)
        self.tree = self.comboBox._popupFrame.GetTree()

    def testSetValue_ToNonExistingValue(self):
        self.comboBox.SetValue('Ignored value')
        self.assertEqual('', self.comboBox.GetValue())

    def testSetValue_ToExistingValue(self):
        self.comboBox.Append('This works')
        self.comboBox.SetValue('This works')
        self.assertEqual('This works', self.comboBox.GetValue())


class IterableTreeCtrlTest(unittest.TestCase):
    def setUp(self):
        self.frame = wx.Frame(None)
        self.tree = IterableTreeCtrl(self.frame)
        self.root = self.tree.AddRoot('root')

    def testPreviousOfRootIsInvalid(self):
        item = self.tree.GetPreviousItem(self.root)
        self.failIf(item.IsOk())

    def testPreviousOfChildOfRootIsRoot(self):
        child = self.tree.AppendItem(self.root, 'child')
        self.assertEqual(self.root, self.tree.GetPreviousItem(child))

    def testPreviousOfSecondChildOfRootIsFirstChild(self):
        child1 = self.tree.AppendItem(self.root, 'child1')
        child2 = self.tree.AppendItem(self.root, 'child2')
        self.assertEqual(child1, self.tree.GetPreviousItem(child2))

    def testPreviousOfGrandChildIsChild(self):
        child = self.tree.AppendItem(self.root, 'child')
        grandchild = self.tree.AppendItem(child, 'grandchild')
        self.assertEqual(child, self.tree.GetPreviousItem(grandchild))

    def testPreviousOfSecondChildWhenFirstChildHasChildIsThatChild(self):
        child1 = self.tree.AppendItem(self.root, 'child1')
        grandchild = self.tree.AppendItem(child1, 'child of child1')
        child2 = self.tree.AppendItem(self.root, 'child2')
        self.assertEqual(grandchild, self.tree.GetPreviousItem(child2))

    def testPreviousOfSecondChildWhenFirstChildHasGrandChildIsThatGrandChild(self):
        child1 = self.tree.AppendItem(self.root, 'child1')
        grandchild = self.tree.AppendItem(child1, 'child of child1')
        greatgrandchild = self.tree.AppendItem(grandchild, 
            'grandchild of child1')
        child2 = self.tree.AppendItem(self.root, 'child2')
        self.assertEqual(greatgrandchild, self.tree.GetPreviousItem(child2))

    def testNextOfRootIsInvalidWhenRootHasNoChildren(self):
        item = self.tree.GetNextItem(self.root)
        self.failIf(item.IsOk())

    def testNextOfRootIsItsChildWhenRootHasOneChild(self):
        child = self.tree.AppendItem(self.root, 'child')
        self.assertEqual(child, self.tree.GetNextItem(self.root))

    def testNextOfLastChildIsInvalid(self):
        child = self.tree.AppendItem(self.root, 'child')
        self.failIf(self.tree.GetNextItem(child).IsOk())

    def testNextOfFirstChildIsSecondChild(self):
        child1 = self.tree.AppendItem(self.root, 'child1')
        child2 = self.tree.AppendItem(self.root, 'child2')
        self.assertEqual(child2, self.tree.GetNextItem(child1))

    def testNextOfGrandChildIsItsParentsSibling(self):
        child1 = self.tree.AppendItem(self.root, 'child1')
        grandchild = self.tree.AppendItem(child1, 'child of child1')
        child2 = self.tree.AppendItem(self.root, 'child2')
        self.assertEqual(child2, self.tree.GetNextItem(grandchild))

    def testNextOfGreatGrandChildIsItsParentsSiblingRecursively(self):
        child1 = self.tree.AppendItem(self.root, 'child1')
        grandchild = self.tree.AppendItem(child1, 'child of child1')
        greatgrandchild = self.tree.AppendItem(grandchild, 
            'grandchild of child1')
        child2 = self.tree.AppendItem(self.root, 'child2')
        self.assertEqual(child2, self.tree.GetNextItem(greatgrandchild))

    def testNextOfGrandChildWhenItIsLastIsInvalid(self):
        child = self.tree.AppendItem(self.root, 'child')
        grandchild = self.tree.AppendItem(child, 'child of child')
        self.failIf(self.tree.GetNextItem(grandchild).IsOk())

    def testFirstItemIsRoot(self):
        self.assertEqual(self.root, self.tree.GetFirstItem())

    def testGetFirstItemWithoutRootIsInvalid(self):
        tree = IterableTreeCtrl(self.frame)
        self.failIf(tree.GetFirstItem().IsOk())

    def testGetSelection_NoSelection(self):
        self.tree.Unselect()
        self.failIf(self.tree.GetSelection().IsOk())

    def testGetSelection_RootItemSelected(self):
        self.tree.SelectItem(self.tree.GetRootItem())
        self.assertEqual(self.tree.GetRootItem(), self.tree.GetSelection())

    def testGetSelection_OtherItem(self):
        child = self.tree.AppendItem(self.root, 'child')
        self.tree.SelectItem(child)
        self.assertEqual(child, self.tree.GetSelection())


class IterableTreeCtrlWithHiddenRootTest(unittest.TestCase):
    def setUp(self):
        frame = wx.Frame(None)
        self.tree = IterableTreeCtrl(frame, style=wx.TR_HIDE_ROOT)
        self.root = self.tree.AddRoot('root')

    def testPreviousOfChildOfRootIsInvalid(self):
        child = self.tree.AppendItem(self.root, 'child')
        self.failIf(self.tree.GetPreviousItem(child).IsOk())

    def testNextOfGrandChildWhenItIsLastIsInvalid(self):
        child = self.tree.AppendItem(self.root, 'child')
        grandchild = self.tree.AppendItem(child, 'child of child')
        self.failIf(self.tree.GetNextItem(grandchild).IsOk())

    def testRootIsNotTheFirstItem(self):
        self.failIf(self.tree.GetFirstItem().IsOk())

    def testFirstChildOfRootIsTheFirstItem(self):
        child = self.tree.AppendItem(self.root, 'child')
        self.assertEqual(child, self.tree.GetFirstItem())

    def testGetSelection_NoSelection(self):
        self.tree.Unselect()
        self.failIf(self.tree.GetSelection().IsOk())

    def testGetSelection_RootItemSelected(self):
        # Apparently, selecting a hidden root item crashes wxPython on
        # Windows, so don't do that.
        if '__WXMSW__' not in wx.PlatformInfo:
            self.tree.SelectItem(self.tree.GetRootItem())
            self.failIf(self.tree.GetSelection().IsOk())

    def testGetSelection_OtherItem(self):
        child = self.tree.AppendItem(self.root, 'child')
        self.tree.SelectItem(child)
        self.assertEqual(child, self.tree.GetSelection())

     

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        platform = sys.argv[1].upper()
        del sys.argv[1]
    else:
        platform = None

    app = wx.App(False)
    unittest.main()

