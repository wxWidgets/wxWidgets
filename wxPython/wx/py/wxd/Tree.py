"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import Object
from Controls import Control
import Parameters as wx


class TreeCtrl(Control):
    """A tree control presents information as a hierarchy, with items
    that may be expanded to show further items. Items in a tree
    control are referenced by wx.TreeItemId handles, which may be
    tested for validity by calling TreeItemId.IsOk()."""

    def AddRoot(self):
        """"""
        pass

    def AppendItem(self):
        """"""
        pass

    def AssignImageList(self):
        """"""
        pass

    def AssignStateImageList(self):
        """"""
        pass

    def Collapse(self):
        """"""
        pass

    def CollapseAndReset(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def Delete(self):
        """"""
        pass

    def DeleteAllItems(self):
        """"""
        pass

    def DeleteChildren(self):
        """"""
        pass

    def EditLabel(self):
        """"""
        pass

    def EnsureVisible(self):
        """"""
        pass

    def Expand(self):
        """"""
        pass

    def GetBoundingRect(self):
        """"""
        pass

    def GetChildrenCount(self):
        """"""
        pass

    def GetCount(self):
        """"""
        pass

    def GetEditControl(self):
        """"""
        pass

    def GetFirstChild(self):
        """"""
        pass

    def GetFirstVisibleItem(self):
        """"""
        pass

    def GetImageList(self):
        """"""
        pass

    def GetIndent(self):
        """"""
        pass

    def GetItemBackgroundColour(self):
        """"""
        pass

    def GetItemData(self):
        """"""
        pass

    def GetItemFont(self):
        """"""
        pass

    def GetItemImage(self):
        """"""
        pass

    def GetItemParent(self):
        """"""
        pass

    def GetItemSelectedImage(self):
        """"""
        pass

    def GetItemText(self):
        """"""
        pass

    def GetItemTextColour(self):
        """"""
        pass

    def GetLastChild(self):
        """"""
        pass

    def GetNextChild(self):
        """"""
        pass

    def GetNextSibling(self):
        """"""
        pass

    def GetNextVisible(self):
        """"""
        pass

    def GetPrevSibling(self):
        """"""
        pass

    def GetPrevVisible(self):
        """"""
        pass

    def GetPyData(self):
        """"""
        pass

    def GetRootItem(self):
        """"""
        pass

    def GetSelection(self):
        """"""
        pass

    def GetSelections(self):
        """"""
        pass

    def GetSpacing(self):
        """"""
        pass

    def GetStateImageList(self):
        """"""
        pass

    def HitTest(self):
        """"""
        pass

    def InsertItem(self):
        """"""
        pass

    def InsertItemBefore(self):
        """"""
        pass

    def IsBold(self):
        """"""
        pass

    def IsExpanded(self):
        """"""
        pass

    def IsSelected(self):
        """"""
        pass

    def IsVisible(self):
        """"""
        pass

    def ItemHasChildren(self):
        """"""
        pass

    def PrependItem(self):
        """"""
        pass

    def ScrollTo(self):
        """"""
        pass

    def SelectItem(self):
        """"""
        pass

    def SetImageList(self):
        """"""
        pass

    def SetIndent(self):
        """"""
        pass

    def SetItemBackgroundColour(self):
        """"""
        pass

    def SetItemBold(self):
        """"""
        pass

    def SetItemData(self):
        """"""
        pass

    def SetItemFont(self):
        """"""
        pass

    def SetItemHasChildren(self):
        """"""
        pass

    def SetItemImage(self):
        """"""
        pass

    def SetItemSelectedImage(self):
        """"""
        pass

    def SetItemText(self):
        """"""
        pass

    def SetItemTextColour(self):
        """"""
        pass

    def SetPyData(self):
        """"""
        pass

    def SetSpacing(self):
        """"""
        pass

    def SetStateImageList(self):
        """"""
        pass

    def SortChildren(self):
        """"""
        pass

    def Toggle(self):
        """"""
        pass

    def Unselect(self):
        """"""
        pass

    def UnselectAll(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def _setCallbackInfo(self):
        """"""
        pass


class TreeItemAttr:
    """"""

    def GetBackgroundColour(self):
        """"""
        pass

    def GetFont(self):
        """"""
        pass

    def GetTextColour(self):
        """"""
        pass

    def HasBackgroundColour(self):
        """"""
        pass

    def HasFont(self):
        """"""
        pass

    def HasTextColour(self):
        """"""
        pass

    def SetBackgroundColour(self):
        """"""
        pass

    def SetFont(self):
        """"""
        pass

    def SetTextColour(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class TreeItemData(Object):
    """TreeItemData is some (arbitrary) user class associated with
    some item. The main advantage of having this class is that
    TreeItemData objects are destroyed automatically by the tree and
    the memory and any other resources associated with a tree item
    will be automatically freed when it is deleted."""

    def __init__(self, obj=wx.NULL):
        """Associate any Python object with tree item using
        wxTreeItemData as container."""
        pass

    def GetData(self):
        """Return the Python object."""
        pass

    def GetId(self):
        """Return the item associated with this node."""
        pass

    def SetData(self, obj):
        """Associate Python object with tree item."""
        pass

    def SetId(self, id):
        """Set the item associated with this node."""
        pass


class TreeItemId:
    """Item in a TreeCtrl."""

## You wouldn't create these directly.

##     def __init__(self):
##         """"""
##         pass

    def IsOk(self):
        """Return True if item is valid."""
        pass

    def Ok(self):
        """Synonym for IsOk."""
        pass
