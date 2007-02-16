'''
treemixin.py 

This module provides two mixin classes that can be used with tree
controls: 
- VirtualTree is a class that, when mixed in with a tree control,
makes the tree control virtual, similar to a ListCtrl in virtual mode. 
A virtual tree control builds the tree itself by means of callbacks,
so the programmer is freed from the burden of building the tree herself.
- DragAndDrop is a mixin class that helps with dragging and dropping of
items. The graphical part of dragging and dropping tree items is done by
this mixin class. You only need to implement the OnDrop method that is
called when the drop happens.

Both mixin classes work with wx.TreeCtrl, wx.gizmos.TreeListCtrl, 
and wx.lib.customtree.CustomTreeCtrl. They can be used together or 
separately.

Both mixins force the wx.TR_HIDE_ROOT style.

Frank Niessink <frank@niessink.com>
License: wxWidgets license
Version: 0.5
Date: 11 February 2007
'''

import wx

class TreeAPIHarmonizer(object):
    ''' This class attempts to hide the differences in API between the
    different tree controls currently supported. '''

    def GetColumnCount(self):
        # Only TreeListCtrl has columns, return 0 if we are mixed in
        # with another tree control.
        try:
            return super(TreeAPIHarmonizer, self).GetColumnCount()
        except AttributeError:
            return 0

    def GetItemType(self, item):
        # Only CustomTreeCtrl has different item types, return the
        # default item type if we are mixed in with another tree control.
        try:
            return super(TreeAPIHarmonizer, self).GetItemType(item)
        except AttributeError:
            return 0

    def SetItemImage(self, item, imageIndex, which, column=0):
        # The SetItemImage signature is different for TreeListCtrl and
        # other tree controls. This adapter method hides the differences.
        if self.GetColumnCount():
            args = (item, imageIndex, column, which)
        else:
            args = (item, imageIndex, which)
        super(TreeAPIHarmonizer, self).SetItemImage(*args)

    def GetMainWindow(self):
        # Only TreeListCtrl has a separate main window, return self if we are 
        # mixed in with another tree control.
        try:
            return super(TreeAPIHarmonizer, self).GetMainWindow()
        except AttributeError:
            return self

    def UnselectAll(self):
        if self.GetWindowStyle() & wx.TR_MULTIPLE:
            super(TreeAPIHarmonizer, self).UnselectAll()
        else:
            self.Unselect()

    def GetSelections(self):
        if self.GetWindowStyle() & wx.TR_MULTIPLE:
            return super(TreeAPIHarmonizer, self).GetSelections()
        else:
            selection = self.GetSelection()
            if selection:
                return [selection]
            else:
                return []

    def HitTest(self, *args, **kwargs):
        # Only TreeListCtrl has columns, return 0 for the column if we are 
        # mixed in with another tree control.
        hitTestResult = super(TreeAPIHarmonizer, self).HitTest(*args, **kwargs)
        try:
            item, flags, column = hitTestResult
        except ValueError:
            (item, flags), column = hitTestResult, 0
        return item, flags, column

    def GetItemChildren(self, item, recursively=False):
        ''' Return the children of item as a list. This method is not
        part of the API of any tree control, but merely convenient to
        have available. '''
        children = []
        child, cookie = self.GetFirstChild(item)
        while child:
            children.append(child)
            if recursively:
                children.extend(self.GetItemChildren(child, True))
            child, cookie = self.GetNextChild(item, cookie)
        return children


class VirtualTree(TreeAPIHarmonizer):
    ''' This is a mixin class that can be used to allow for virtual tree
    controls. It can be mixed in with wx.TreeCtrl, wx.gizmos.TreeListCtrl, 
    wx.lib.customtree.CustomTreeCtrl.

    To use it derive a new class from this class and one of the tree
    controls, e.g.:
    class MyTree(VirtualTree, wx.TreeCtrl):
        ...

    You *must* implement OnGetChildrenCount and OnGetItemText so that
    this class knows how many items to draw and what text to use. To
    also draw images, change colours, etc., override one or more of the
    other OnGetXXX methods.

    About specifying item indices: the VirtualTree uses various item 
    callbacks (such as OnGetItemText) to retrieve information needed 
    to display the items. To specify what item a callback needs
    information about, the callback passes a list of indices. The first
    index in the list is the index of the root item. If no more indices
    are in the list (i.e. the length of the list of indices is exactly
    1), the callback should return the information about that root item.
    If the list contains two indices, the first index is the index of
    the root item, and the second index is the index of the child of
    that root item. For example, if OnGetItemText is called with
    indices=[0,2], the callback should return information about that the
    third child of the first root item. OnGetChildrenCount may also be
    called with indices == [] to get the number of root items in the
    tree.
    '''

    def __init__(self, *args, **kwargs):
        kwargs['style'] = kwargs.get('style', wx.TR_DEFAULT_STYLE) | \
                          wx.TR_HIDE_ROOT
        super(VirtualTree, self).__init__(*args, **kwargs)
        self.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.OnItemExpanding)
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.OnItemCollapsed)

    def OnGetChildrenCount(self, indices):
        ''' This function must be overloaded in the derived class. It
        should return the number of child items of the item with the
        provided indices. If indices == [] it should return the number
        of root items. '''
        raise NotImplementedError

    def OnGetItemText(self, indices, column=0):
        ''' This function must be overloaded in the derived class. It 
        should return the string containing the text of the specified
        item. '''
        raise NotImplementedError

    def OnGetItemFont(self, indices):
        ''' This function may be overloaded in the derived class. It 
        should return the wx.Font to be used for the specified item. '''
        return wx.NullFont 

    def OnGetItemTextColour(self, indices):
        ''' This function may be overloaded in the derived class. It 
        should return the wx.Colour to be used as text colour for the 
        specified item. '''
        return wx.NullColour

    def OnGetItemBackgroundColour(self, indices):
        ''' This function may be overloaded in the derived class. It 
        should return the wx.Colour to be used as background colour for 
        the specified item. '''
        return wx.NullColour

    def OnGetItemImage(self, indices, which=wx.TreeItemIcon_Normal, column=0):
        ''' This function may be overloaded in the derived class. It 
        should return the index of the image to be used. '''
        return -1

    def OnGetItemType(self, indices):
        ''' This function may be overloaded in the derived class, but
        that only makes sense when this class is mixed in with a tree 
        control that supports checkable items, e.g. CustomTreeCtrl. 
        This method should return whether the item is to be normal (0,
        the default), a checkbox (1) or a radiobutton (2). 
        Note that OnGetItemChecked should return whether the item is
        actually checked. '''
        return 0 

    def OnGetItemChecked(self, indices):
        ''' This function may be overloaded in the derived class, but
        that only makes sense when this class is mixed in with a tree 
        control that supports checkable items, e.g. CustomTreeCtrl. 
        This method should return whether the item is to be checked. 
        Note that OnGetItemType should return 1 (checkbox) or 2
        (radiobutton) for this item. '''
        return False 

    def RefreshItems(self):
        ''' Redraws all visible items. '''
        rootItem = self.GetRootItem()
        if not rootItem:
            rootItem = self.AddRoot('Hidden root')
        self.RefreshChildrenRecursively(rootItem, [], 
                                        self.OnGetChildrenCount([]))

    def RefreshChildrenRecursively(self, item, indices, childrenCount):
        ''' Refresh the children of item, reusing as much of the
        existing items in the tree as possible. '''
        existingChildren = self.GetItemChildren(item)
        for childIndex in range(childrenCount):
            childIndices = indices + [childIndex]
            if childIndex < len(existingChildren):
                child = existingChildren[childIndex]
            else:
                child = self.AppendItem(item, '')
            self.RefreshRecursively(child, childIndices)
        # Delete left-over items:
        for existingChild in existingChildren[childrenCount:]:
            self.Delete(existingChild)

    def RefreshRecursively(self, item, indices):
        childrenCount = self.OnGetChildrenCount(indices)
        self.RefreshItem(item, indices, bool(childrenCount))
        # We need to refresh the children when the item is expanded and
        # when the item has no children, because in the latter case we
        # might have to delete old children from the tree:
        if self.IsExpanded(item) or not childrenCount:
            self.RefreshChildrenRecursively(item, indices, childrenCount)
        self.SetItemHasChildren(item, bool(childrenCount))
            
    def RefreshItem(self, item, indices, hasChildren):
        ''' Refresh one item. '''
        item = self.RefreshItemType(item, indices)
        self.RefreshItemText(item, indices)
        self.RefreshColumns(item, indices)
        self.RefreshFont(item, indices)
        self.RefreshTextColour(item, indices)
        self.RefreshBackgroundColour(item, indices)
        self.RefreshItemImage(item, indices, hasChildren)
        self.RefreshCheckedState(item, indices)

    def RefreshItemType(self, item, indices):
        try:
            currentType = self.GetItemType(item)
        except AttributeError:
            return item
        type = self.OnGetItemType(indices)
        if type != currentType:
            # There's no way to change the type so we create a new item
            # and delete the old one.
            oldItem = item
            item = self.InsertItem(self.GetItemParent(oldItem), item, '', 
                ct_type=type)
            self.Delete(oldItem)
        return item

    def RefreshItemText(self, item, indices):
        itemText = self.OnGetItemText(indices)
        if self.GetItemText(item) != itemText:
            self.SetItemText(item, itemText)

    def RefreshColumns(self, item, indices):
        for columnIndex in range(1, self.GetColumnCount()):
            itemText = self.OnGetItemText(indices, columnIndex)
            if self.GetItemText(item, columnIndex) != itemText:
                self.SetItemText(item, itemText, columnIndex)

    def RefreshFont(self, item, indices):
        font = self.OnGetItemFont(indices)
        if self.GetItemFont(item) != font:
            self.SetItemFont(item, font)

    def RefreshTextColour(self, item, indices):
        colour = self.OnGetItemTextColour(indices)
        if self.GetItemTextColour(item) != colour:
            self.SetItemTextColour(item, colour)

    def RefreshBackgroundColour(self, item, indices):
        # This one only seems to work for wx.TreeCtrl and not for
        # wx.gizmos.TreeListCtrl and wx.lib.customtreectrl.CustomTreeCtrl...
        colour = self.OnGetItemBackgroundColour(indices)
        if self.GetItemBackgroundColour(item) != colour:
            self.SetItemBackgroundColour(item, colour)

    def RefreshItemImage(self, item, indices, hasChildren):
        regularIcons = [wx.TreeItemIcon_Normal, wx.TreeItemIcon_Selected]
        expandedIcons = [wx.TreeItemIcon_Expanded, 
                         wx.TreeItemIcon_SelectedExpanded]
        # Refresh images in first column:
        for icon in regularIcons:
            imageIndex = self.OnGetItemImage(indices, icon)
            if self.GetItemImage(item, icon) != imageIndex:
                self.SetItemImage(item, imageIndex, icon)
        for icon in expandedIcons:
            if hasChildren:
                imageIndex = self.OnGetItemImage(indices, icon)
            else:
                imageIndex = -1
            if self.GetItemImage(item, icon) != imageIndex or imageIndex == -1:
                self.SetItemImage(item, imageIndex, icon)
        # Refresh images in remaining columns, if any:
        for columnIndex in range(1, self.GetColumnCount()):
            for icon in regularIcons:
                imageIndex = self.OnGetItemImage(indices, icon, columnIndex)
                if self.GetItemImage(item, columnIndex, icon) != imageIndex:
                    self.SetItemImage(item, imageIndex, icon, columnIndex)

    def RefreshCheckedState(self, item, indices):
        try: 
            isChecked = self.IsItemChecked(item)
        except AttributeError:
            return # Checking not supported
        shouldBeChecked = self.OnGetItemChecked(indices)
        if isChecked != shouldBeChecked:
            self.CheckItem(item, shouldBeChecked)

    def OnItemExpanding(self, event):
        indices = self.ItemIndices(event.GetItem())
        childrenCount = self.OnGetChildrenCount(indices)
        self.RefreshChildrenRecursively(event.GetItem(), indices, childrenCount)
        event.Skip()

    def OnItemCollapsed(self, event):
        parent = self.GetItemParent(event.GetItem())
        if not parent:
            parent = self.GetRootItem()
        indices = self.ItemIndices(parent)
        childrenCount = self.OnGetChildrenCount(indices)
        self.RefreshChildrenRecursively(parent, indices, childrenCount)
        event.Skip()
        
    def ItemIndices(self, item):
        ''' Construct index list for item. '''
        parent = self.GetItemParent(item)
        if parent:
            parentIndices = self.ItemIndices(parent)
            ownIndex = self.GetItemChildren(parent).index(item)
            return parentIndices + [ownIndex]
        else:
            return []


class DragAndDrop(TreeAPIHarmonizer):
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
    and then call RefreshItems to let de virtual tree refresh itself.
    '''    
 
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
        self._dragItem = self.GetSelections()[0]
        if self._dragItem: 
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
        item, flags, column = self.HitTest(wx.Point(event.GetX(), event.GetY()))
        if not item:
            item = self.GetRootItem()
        if self.IsValidDropTarget(item):
            self.SetCursorToDragging()
        else:
            self.SetCursorToDroppingImpossible()
        if flags & wx.TREE_HITTEST_ONITEMBUTTON:
            self.Expand(item)
        selections = self.GetSelections()
        if selections != [item]:
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


