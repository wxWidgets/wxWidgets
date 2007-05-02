'''
treemixin.py 

This module provides three mixin classes that can be used with tree
controls: 

- VirtualTree is a class that, when mixed in with a tree control,
  makes the tree control virtual, similar to a ListCtrl in virtual mode. 
  A virtual tree control builds the tree itself by means of callbacks,
  so the programmer is freed from the burden of building the tree herself.

- DragAndDrop is a mixin class that helps with dragging and dropping of
  items. The graphical part of dragging and dropping tree items is done by
  this mixin class. You only need to implement the OnDrop method that is
  called when the drop happens.

- ExpansionState is a mixin that can be queried for the expansion state of
  all items in the tree to restore it later.

All mixin classes work with wx.TreeCtrl, wx.gizmos.TreeListCtrl, 
and wx.lib.customtree.CustomTreeCtrl. They can be used together or 
separately.

The VirtualTree and DragAndDrop mixins force the wx.TR_HIDE_ROOT style.

Author: Frank Niessink <frank@niessink.com>
License: wxWidgets license
Version: 1.0
Date: 15 April 2007

ExpansionState is based on code and ideas from Karsten Hilbert.
Andrea Gavana provided help with the CustomTreeCtrl integration.
'''


import wx, wx.lib.customtreectrl


class TreeAPIHarmonizer(object):
    ''' This class attempts to hide the differences in API between the
    different tree controls that are part of wxPython. '''

    def __callSuper(self, methodName, default, *args, **kwargs):
        # If our super class has a method called methodName, call it,
        # otherwise return the default value.
        superClass = super(TreeAPIHarmonizer, self)
        if hasattr(superClass, methodName):
            return getattr(superClass, methodName)(*args, **kwargs)
        else:
            return default

    def GetColumnCount(self, *args, **kwargs):
        # Only TreeListCtrl has columns, return 0 if we are mixed in
        # with another tree control.
        return self.__callSuper('GetColumnCount', 0, *args, **kwargs)

    def GetItemType(self, *args, **kwargs):
        # Only CustomTreeCtrl has different item types, return the
        # default item type if we are mixed in with another tree control.
        return self.__callSuper('GetItemType', 0, *args, **kwargs)

    def SetItemType(self, item, newType):
        # CustomTreeCtrl doesn't support changing the item type on the fly,
        # so we create a new item and delete the old one. We currently only
        # keep the item text, would be nicer to also retain other attributes.
        text = self.GetItemText(item)
        newItem = self.InsertItem(self.GetItemParent(item), item, text, 
                                  ct_type=newType)
        self.Delete(item)
        return newItem

    def IsItemChecked(self, *args, **kwargs):
        # Only CustomTreeCtrl supports checkable items, return False if
        # we are mixed in with another tree control.
        return self.__callSuper('IsItemChecked', False, *args, **kwargs)

    def GetItemChecked(self, *args, **kwargs):
        # For consistency's sake, provide a 'Get' and 'Set' method for 
        # checkable items.
        return self.IsItemChecked(*args, **kwargs)

    def SetItemChecked(self, *args, **kwargs):
        # For consistency's sake, provide a 'Get' and 'Set' method for 
        # checkable items.
        return self.CheckItem(*args, **kwargs)

    def GetMainWindow(self, *args, **kwargs):
        # Only TreeListCtrl has a separate main window, return self if we are 
        # mixed in with another tree control.
        return self.__callSuper('GetMainWindow', self, *args, **kwargs)

    def GetItemImage(self, item, which=wx.TreeItemIcon_Normal, column=-1):
        # CustomTreeCtrl always wants the which argument, so provide it
        # TreeListCtr.GetItemImage has a different order of arguments than
        # the other tree controls. Hide the differenes.
        if self.GetColumnCount():
            args = (item, column, which)
        else:
            args = (item, which)
        return super(TreeAPIHarmonizer, self).GetItemImage(*args)

    def SetItemImage(self, item, imageIndex, which=wx.TreeItemIcon_Normal, 
                     column=-1):
        # The SetItemImage signature is different for TreeListCtrl and
        # other tree controls. This adapter method hides the differences.
        if self.GetColumnCount():
            args = (item, imageIndex, column, which)
        else:
            args = (item, imageIndex, which)
        super(TreeAPIHarmonizer, self).SetItemImage(*args)

    def UnselectAll(self):
        # Unselect all items, regardless of whether we are in multiple 
        # selection mode or not.
        if self.HasFlag(wx.TR_MULTIPLE):
            super(TreeAPIHarmonizer, self).UnselectAll()
        else:
            # CustomTreeCtrl Unselect() doesn't seem to work in all cases,
            # also invoke UnselectAll just to be sure.
            self.Unselect()
            super(TreeAPIHarmonizer, self).UnselectAll()

    def GetCount(self):
        # TreeListCtrl correctly ignores the root item when it is hidden,
        # but doesn't count the root item when it is visible
        itemCount = super(TreeAPIHarmonizer, self).GetCount()
        if self.GetColumnCount() and not self.HasFlag(wx.TR_HIDE_ROOT):
            itemCount += 1
        return itemCount

    def GetSelections(self):
        # Always return a list of selected items, regardless of whether
        # we are in multiple selection mode or not.
        if self.HasFlag(wx.TR_MULTIPLE):
            selections = super(TreeAPIHarmonizer, self).GetSelections()
        else:
            selection = self.GetSelection()
            if selection:
                selections = [selection]
            else:
                selections = []
        # If the root item is hidden, it should never be selected, 
        # unfortunately, CustomTreeCtrl allows it to be selected.
        if self.HasFlag(wx.TR_HIDE_ROOT):
            rootItem = self.GetRootItem()
            if rootItem and rootItem in selections:
                selections.remove(rootItem)
        return selections

    def GetFirstVisibleItem(self):
        # TreeListCtrl raises an exception or even crashes when invoking 
        # GetFirstVisibleItem on an empty tree.
        if self.GetRootItem():
            return super(TreeAPIHarmonizer, self).GetFirstVisibleItem()
        else:
            return wx.TreeItemId()

    def SelectItem(self, item, *args, **kwargs):
        # Prevent the hidden root from being selected, otherwise TreeCtrl
        # crashes 
        if self.HasFlag(wx.TR_HIDE_ROOT) and item == self.GetRootItem():
            return
        else:
            return super(TreeAPIHarmonizer, self).SelectItem(item, *args, 
                                                             **kwargs)

    def HitTest(self, *args, **kwargs):
        ''' HitTest returns a two-tuple (item, flags) for tree controls
        without columns and a three-tuple (item, flags, column) for tree
        controls with columns. Our caller can indicate this method to
        always return a three-tuple no matter what tree control we're mixed
        in with by specifying the optional argument 'alwaysReturnColumn'
        to be True. '''
        alwaysReturnColumn = kwargs.pop('alwaysReturnColumn', False)
        hitTestResult = super(TreeAPIHarmonizer, self).HitTest(*args, **kwargs)
        if len(hitTestResult) == 2 and alwaysReturnColumn:
            hitTestResult += (0,)
        return hitTestResult

    def ExpandAll(self, item=None):
        # TreeListCtrl wants an item as argument. That's an inconsistency with
        # the TreeCtrl API. Also, TreeCtrl doesn't allow invoking ExpandAll 
        # on a tree with hidden root node, so prevent that.
        if self.HasFlag(wx.TR_HIDE_ROOT):
            rootItem = self.GetRootItem()
            if rootItem:
                child, cookie = self.GetFirstChild(rootItem)
                while child:
                    self.ExpandAllChildren(child)
                    child, cookie = self.GetNextChild(rootItem, cookie)
        else:
            try:
                super(TreeAPIHarmonizer, self).ExpandAll()
            except TypeError:
                if item is None:
                    item = self.GetRootItem()
                super(TreeAPIHarmonizer, self).ExpandAll(item)

    def ExpandAllChildren(self, item):
        # TreeListCtrl and CustomTreeCtrl don't have ExpandallChildren
        try:
            super(TreeAPIHarmonizer, self).ExpandAllChildren(item)
        except AttributeError:
            self.Expand(item)
            child, cookie = self.GetFirstChild(item) 
            while child:
                self.ExpandAllChildren(child)
                child, cookie = self.GetNextChild(item, cookie)


class TreeHelper(object):
    ''' This class provides methods that are not part of the API of any 
    tree control, but are convenient to have available. '''

    def GetItemChildren(self, item=None, recursively=False):
        ''' Return the children of item as a list. '''
        if not item:
            item = self.GetRootItem()
            if not item:
                return []
        children = []
        child, cookie = self.GetFirstChild(item)
        while child:
            children.append(child)
            if recursively:
                children.extend(self.GetItemChildren(child, True))
            child, cookie = self.GetNextChild(item, cookie)
        return children

    def GetIndexOfItem(self, item):
        ''' Return the index of item. '''
        parent = self.GetItemParent(item)
        if parent:
            parentIndices = self.GetIndexOfItem(parent)
            ownIndex = self.GetItemChildren(parent).index(item)
            return parentIndices + (ownIndex,)
        else:
            return ()

    def GetItemByIndex(self, index):
        ''' Return the item specified by index. '''
        item = self.GetRootItem()
        for i in index:
            children = self.GetItemChildren(item)
            item = children[i]
        return item


class VirtualTree(TreeAPIHarmonizer, TreeHelper):
    ''' This is a mixin class that can be used to allow for virtual tree
    controls. It can be mixed in with wx.TreeCtrl, wx.gizmos.TreeListCtrl, 
    wx.lib.customtree.CustomTreeCtrl.

    To use it derive a new class from this class and one of the tree
    controls, e.g.:
    class MyTree(VirtualTree, wx.TreeCtrl):
        ...

    VirtualTree uses several callbacks (such as OnGetItemText) to 
    retrieve information needed to construct the tree and render the 
    items. To specify what item the callback needs information about,
    the callback passes an item index. Whereas for list controls a simple
    integer index can be used, for tree controls indicating a specific
    item is a little bit more complicated. See below for a more detailed 
    explanation of the how index works.

    Note that VirtualTree forces the wx.TR_HIDE_ROOT style.

    In your subclass you *must* override OnGetItemText and 
    OnGetChildrenCount. These two methods are the minimum needed to 
    construct the tree and render the item labels. If you want to add 
    images, change fonts our colours, etc., you need to override the 
    appropriate OnGetXXX method as well.

    About indices: your callbacks are passed a tuple of integers that 
    identifies the item the VirtualTree wants information about. An 
    empty tuple, i.e. (), represents the hidden root item.  A tuple with 
    one integer, e.g. (3,), represents a visible root item, in this case 
    the fourth one. A tuple with two integers, e.g. (3,0), represents a 
    child of a visible root item, in this case the first child of the 
    fourth root item. 
    '''

    def __init__(self, *args, **kwargs):
        kwargs['style'] = kwargs.get('style', wx.TR_DEFAULT_STYLE) | \
                          wx.TR_HIDE_ROOT
        super(VirtualTree, self).__init__(*args, **kwargs)
        self.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.OnItemExpanding)
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.OnItemCollapsed)

    def OnGetChildrenCount(self, index):
        ''' This function *must* be overloaded in the derived class.
        It should return the number of child items of the item with the 
        provided index. If index == () it should return the number of 
        root items. '''
        raise NotImplementedError

    def OnGetItemText(self, index, column=0):
        ''' This function *must* be overloaded in the derived class. It 
        should return the string containing the text of the specified
        item. '''
        raise NotImplementedError

    def OnGetItemFont(self, index):
        ''' This function may be overloaded in the derived class. It 
        should return the wx.Font to be used for the specified item. '''
        return wx.NullFont 

    def OnGetItemTextColour(self, index):
        ''' This function may be overloaded in the derived class. It 
        should return the wx.Colour to be used as text colour for the 
        specified item. '''
        return wx.NullColour

    def OnGetItemBackgroundColour(self, index):
        ''' This function may be overloaded in the derived class. It 
        should return the wx.Colour to be used as background colour for 
        the specified item. '''
        return wx.NullColour

    def OnGetItemImage(self, index, which=wx.TreeItemIcon_Normal, column=0):
        ''' This function may be overloaded in the derived class. It 
        should return the index of the image to be used. Don't forget
        to associate an ImageList with the tree control. '''
        return -1

    def OnGetItemType(self, index):
        ''' This function may be overloaded in the derived class, but
        that only makes sense when this class is mixed in with a tree 
        control that supports checkable items, i.e. CustomTreeCtrl. 
        This method should return whether the item is to be normal (0,
        the default), a checkbox (1) or a radiobutton (2). 
        Note that OnGetItemChecked needs to be implemented as well; it
        should return whether the item is actually checked. '''
        return 0 

    def OnGetItemChecked(self, index):
        ''' This function may be overloaded in the derived class, but
        that only makes sense when this class is mixed in with a tree 
        control that supports checkable items, i.e. CustomTreeCtrl. 
        This method should return whether the item is to be checked. 
        Note that OnGetItemType should return 1 (checkbox) or 2
        (radiobutton) for this item. '''
        return False 

    def RefreshItems(self):
        ''' Redraws all visible items. '''
        rootItem = self.GetRootItem()
        if not rootItem:
            rootItem = self.AddRoot('Hidden root')
        self.RefreshChildrenRecursively(rootItem)

    def RefreshItem(self, index):
        ''' Redraws the item with the specified index. '''
        try:
            item = self.GetItemByIndex(index)
        except IndexError:
            # There's no corresponding item for index, because its parent
            # has not been expanded yet.
            return
        hasChildren = bool(self.OnGetChildrenCount(index))
        self.DoRefreshItem(item, index, hasChildren)

    def RefreshChildrenRecursively(self, item, itemIndex=None):
        ''' Refresh the children of item, reusing as much of the
        existing items in the tree as possible. '''
        if itemIndex is None:
            itemIndex = self.GetIndexOfItem(item)
        reusableChildren = self.GetItemChildren(item)
        for childIndex in self.ChildIndices(itemIndex):
            if reusableChildren: 
                child = reusableChildren.pop(0) 
            else:
                child = self.AppendItem(item, '')
            self.RefreshItemRecursively(child, childIndex)
        for child in reusableChildren:
            self.Delete(child)

    def RefreshItemRecursively(self, item, itemIndex):
        ''' Refresh the item and its children recursively. '''
        hasChildren = bool(self.OnGetChildrenCount(itemIndex))
        item = self.DoRefreshItem(item, itemIndex, hasChildren)
        # We need to refresh the children when the item is expanded and
        # when the item has no children, because in the latter case we
        # might have to delete old children from the tree:
        if self.IsExpanded(item) or not hasChildren:
            self.RefreshChildrenRecursively(item, itemIndex)
        self.SetItemHasChildren(item, hasChildren)
 
    def DoRefreshItem(self, item, index, hasChildren):
        ''' Refresh one item. '''
        item = self.RefreshItemType(item, index)
        self.RefreshItemText(item, index)
        self.RefreshColumns(item, index)
        self.RefreshItemFont(item, index)
        self.RefreshTextColour(item, index)
        self.RefreshBackgroundColour(item, index)
        self.RefreshItemImage(item, index, hasChildren)
        self.RefreshCheckedState(item, index)
        return item

    def RefreshItemText(self, item, index):
        self.__refreshAttribute(item, index, 'ItemText')

    def RefreshColumns(self, item, index):
        for columnIndex in range(1, self.GetColumnCount()):
            self.__refreshAttribute(item, index, 'ItemText', columnIndex)

    def RefreshItemFont(self, item, index):
        self.__refreshAttribute(item, index, 'ItemFont')

    def RefreshTextColour(self, item, index):
        self.__refreshAttribute(item, index, 'ItemTextColour')

    def RefreshBackgroundColour(self, item, index):
        self.__refreshAttribute(item, index, 'ItemBackgroundColour')

    def RefreshItemImage(self, item, index, hasChildren):
        regularIcons = [wx.TreeItemIcon_Normal, wx.TreeItemIcon_Selected]
        expandedIcons = [wx.TreeItemIcon_Expanded, 
                         wx.TreeItemIcon_SelectedExpanded]
        # Refresh images in first column:
        for icon in regularIcons:
            self.__refreshAttribute(item, index, 'ItemImage', icon)
        for icon in expandedIcons:
            if hasChildren:
                imageIndex = self.OnGetItemImage(index, icon)
            else:
                imageIndex = -1
            if self.GetItemImage(item, icon) != imageIndex or imageIndex == -1:
                self.SetItemImage(item, imageIndex, icon)
        # Refresh images in remaining columns, if any:
        for columnIndex in range(1, self.GetColumnCount()):
            for icon in regularIcons:
                self.__refreshAttribute(item, index, 'ItemImage', icon, 
                                        columnIndex)

    def RefreshItemType(self, item, index):
        return self.__refreshAttribute(item, index, 'ItemType')

    def RefreshCheckedState(self, item, index):
        self.__refreshAttribute(item, index, 'ItemChecked')

    def ChildIndices(self, itemIndex):
        childrenCount = self.OnGetChildrenCount(itemIndex) 
        return [itemIndex + (childNumber,) for childNumber \
                in range(childrenCount)]

    def OnItemExpanding(self, event):
        self.RefreshChildrenRecursively(event.GetItem())
        event.Skip()

    def OnItemCollapsed(self, event):
        parent = self.GetItemParent(event.GetItem())
        if not parent:
            parent = self.GetRootItem()
        self.RefreshChildrenRecursively(parent)
        event.Skip()

    def __refreshAttribute(self, item, index, attribute, *args):
        ''' Refresh the specified attribute if necessary. '''
        value = getattr(self, 'OnGet%s'%attribute)(index, *args)
        if getattr(self, 'Get%s'%attribute)(item, *args) != value:
            return getattr(self, 'Set%s'%attribute)(item, value, *args)
        else:
            return item


class DragAndDrop(TreeAPIHarmonizer, TreeHelper):
    ''' This is a mixin class that can be used to easily implement
    dragging and dropping of tree items. It can be mixed in with 
    wx.TreeCtrl, wx.gizmos.TreeListCtrl, or wx.lib.customtree.CustomTreeCtrl.

    To use it derive a new class from this class and one of the tree
    controls, e.g.:
    class MyTree(DragAndDrop, wx.TreeCtrl):
        ...

    You *must* implement OnDrop. OnDrop is called when the user has
    dropped an item on top of another item. It's up to you to decide how
    to handle the drop. If you are using this mixin together with the
    VirtualTree mixin, it makes sense to rearrange your underlying data
    and then call RefreshItems to let the virtual tree refresh itself. '''    
 
    def __init__(self, *args, **kwargs):
        kwargs['style'] = kwargs.get('style', wx.TR_DEFAULT_STYLE) | \
                          wx.TR_HIDE_ROOT
        super(DragAndDrop, self).__init__(*args, **kwargs)
        self.Bind(wx.EVT_TREE_BEGIN_DRAG, self.OnBeginDrag)

    def OnDrop(self, dropItem, dragItem):
        ''' This function must be overloaded in the derived class.
        dragItem is the item being dragged by the user. dropItem is the
        item dragItem is dropped upon. If the user doesn't drop dragItem
        on another item, dropItem equals the (hidden) root item of the
        tree control. '''
        raise NotImplementedError

    def OnBeginDrag(self, event):
        # We allow only one item to be dragged at a time, to keep it simple
        self._dragItem = event.GetItem()
        if self._dragItem and self._dragItem != self.GetRootItem(): 
            self.StartDragging()
            event.Allow()
        else:
            event.Veto()

    def OnEndDrag(self, event):
        self.StopDragging()
        dropTarget = event.GetItem()
        if not dropTarget:
            dropTarget = self.GetRootItem()
        if self.IsValidDropTarget(dropTarget):
            self.UnselectAll()
            if dropTarget != self.GetRootItem():
                self.SelectItem(dropTarget)
            self.OnDrop(dropTarget, self._dragItem)

    def OnDragging(self, event):
        if not event.Dragging():
            self.StopDragging()
            return
        item, flags, column = self.HitTest(wx.Point(event.GetX(), event.GetY()),
                                           alwaysReturnColumn=True)
        if not item:
            item = self.GetRootItem()
        if self.IsValidDropTarget(item):
            self.SetCursorToDragging()
        else:
            self.SetCursorToDroppingImpossible()
        if flags & wx.TREE_HITTEST_ONITEMBUTTON:
            self.Expand(item)
        if self.GetSelections() != [item]:
            self.UnselectAll()
            if item != self.GetRootItem(): 
                self.SelectItem(item)
        event.Skip()
        
    def StartDragging(self):
        self.GetMainWindow().Bind(wx.EVT_MOTION, self.OnDragging)
        self.Bind(wx.EVT_TREE_END_DRAG, self.OnEndDrag)
        self.SetCursorToDragging()

    def StopDragging(self):
        self.GetMainWindow().Unbind(wx.EVT_MOTION)
        self.Unbind(wx.EVT_TREE_END_DRAG)
        self.ResetCursor()
        self.UnselectAll()
        self.SelectItem(self._dragItem)

    def SetCursorToDragging(self):
        self.GetMainWindow().SetCursor(wx.StockCursor(wx.CURSOR_HAND))
        
    def SetCursorToDroppingImpossible(self):
        self.GetMainWindow().SetCursor(wx.StockCursor(wx.CURSOR_NO_ENTRY))
        
    def ResetCursor(self):
        self.GetMainWindow().SetCursor(wx.NullCursor)

    def IsValidDropTarget(self, dropTarget):
        if dropTarget: 
            allChildren = self.GetItemChildren(self._dragItem, recursively=True)
            parent = self.GetItemParent(self._dragItem) 
            return dropTarget not in [self._dragItem, parent] + allChildren
        else:
            return True        


class ExpansionState(TreeAPIHarmonizer, TreeHelper):
    ''' This is a mixin class that can be used to save and restore
    the expansion state (i.e. which items are expanded and which items
    are collapsed) of a tree. It can be mixed in with wx.TreeCtrl, 
    wx.gizmos.TreeListCtrl, or wx.lib.customtree.CustomTreeCtrl.

    To use it derive a new class from this class and one of the tree
    controls, e.g.:
    class MyTree(ExpansionState, wx.TreeCtrl):
        ...

    By default, ExpansionState uses the position of tree items in the tree 
    to keep track of which items are expanded. This should be sufficient 
    for the simple scenario where you save the expansion state of the tree 
    when the user closes the application or file so that you can restore 
    the expansion state when the user start the application or loads that 
    file for the next session.  

    If you need to add or remove items between the moments of saving and 
    restoring the expansion state (e.g. in case of a multi-user application)
    you must override GetItemIdentity so that saving and loading of the 
    expansion doesn't depend on the position of items in the tree, but 
    rather on some more stable characteristic of the underlying domain 
    object, e.g. a social security number in case of persons or an isbn 
    number in case of books. '''    

    def GetItemIdentity(self, item):
        ''' Return a hashable object that represents the identity of the
        item. By default this returns the position of the item in the 
        tree. You may want to override this to return the item label 
        (if you know that labels are unique and don't change), or return 
        something that represents the underlying domain object, e.g. 
        a database key. ''' 
        return self.GetIndexOfItem(item)
 
    def GetExpansionState(self):
        ''' GetExpansionState() -> list of expanded items. Expanded items 
        are coded as determined by the result of GetItemIdentity(item). '''
        root = self.GetRootItem()
        if not root:
            return []
        if self.HasFlag(wx.TR_HIDE_ROOT):
            return self.GetExpansionStateOfChildren(root)
        else:
            return self.GetExpansionStateOfItem(root)

    def SetExpansionState(self, listOfExpandedItems):
        ''' SetExpansionState(listOfExpandedItems). Expands all tree items 
        whose identity, as determined by GetItemIdentity(item), is present
        in the list and collapses all other tree items. '''
        root = self.GetRootItem()
        if not root:
            return
        if self.HasFlag(wx.TR_HIDE_ROOT):
            self.SetExpansionStateOfChildren(listOfExpandedItems, root)
        else:
            self.SetExpansionStateOfItem(listOfExpandedItems, root)

    ExpansionState = property(GetExpansionState, SetExpansionState)

    def GetExpansionStateOfItem(self, item):
        listOfExpandedItems = []
        if self.IsExpanded(item):
            listOfExpandedItems.append(self.GetItemIdentity(item))
            listOfExpandedItems.extend(self.GetExpansionStateOfChildren(item))
        return listOfExpandedItems

    def GetExpansionStateOfChildren(self, item):
        listOfExpandedItems = []
        for child in self.GetItemChildren(item):
            listOfExpandedItems.extend(self.GetExpansionStateOfItem(child))
        return listOfExpandedItems

    def SetExpansionStateOfItem(self, listOfExpandedItems, item):
        if self.GetItemIdentity(item) in listOfExpandedItems:
            self.Expand(item)
            self.SetExpansionStateOfChildren(listOfExpandedItems, item)
        else:
            self.Collapse(item)

    def SetExpansionStateOfChildren(self, listOfExpandedItems, item):
        for child in self.GetItemChildren(item):
            self.SetExpansionStateOfItem(listOfExpandedItems, child)
