# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _gizmos

import windows
import core
import controls
wx = core 
wxEVT_DYNAMIC_SASH_SPLIT = _gizmos.wxEVT_DYNAMIC_SASH_SPLIT
wxEVT_DYNAMIC_SASH_UNIFY = _gizmos.wxEVT_DYNAMIC_SASH_UNIFY
DS_MANAGE_SCROLLBARS = _gizmos.DS_MANAGE_SCROLLBARS
DS_DRAG_CORNER = _gizmos.DS_DRAG_CORNER
class DynamicSashSplitEvent(core.CommandEvent):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_DynamicSashSplitEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDynamicSashSplitEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class DynamicSashSplitEventPtr(DynamicSashSplitEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DynamicSashSplitEvent
_gizmos.DynamicSashSplitEvent_swigregister(DynamicSashSplitEventPtr)

class DynamicSashUnifyEvent(core.CommandEvent):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_DynamicSashUnifyEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDynamicSashUnifyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class DynamicSashUnifyEventPtr(DynamicSashUnifyEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DynamicSashUnifyEvent
_gizmos.DynamicSashUnifyEvent_swigregister(DynamicSashUnifyEventPtr)

class DynamicSashWindow(core.Window):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_DynamicSashWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _gizmos.DynamicSashWindow_Create(*args, **kwargs)
    def GetHScrollBar(*args, **kwargs): return _gizmos.DynamicSashWindow_GetHScrollBar(*args, **kwargs)
    def GetVScrollBar(*args, **kwargs): return _gizmos.DynamicSashWindow_GetVScrollBar(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDynamicSashWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class DynamicSashWindowPtr(DynamicSashWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DynamicSashWindow
_gizmos.DynamicSashWindow_swigregister(DynamicSashWindowPtr)

def PreDynamicSashWindow(*args, **kwargs):
    val = _gizmos.new_PreDynamicSashWindow(*args, **kwargs)
    val.thisown = 1
    return val

EVT_DYNAMIC_SASH_SPLIT = wx.PyEventBinder( wxEVT_DYNAMIC_SASH_SPLIT, 1 )
EVT_DYNAMIC_SASH_UNIFY = wx.PyEventBinder( wxEVT_DYNAMIC_SASH_UNIFY, 1 )

EL_ALLOW_NEW = _gizmos.EL_ALLOW_NEW
EL_ALLOW_EDIT = _gizmos.EL_ALLOW_EDIT
EL_ALLOW_DELETE = _gizmos.EL_ALLOW_DELETE
class EditableListBox(windows.Panel):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_EditableListBox(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def SetStrings(*args, **kwargs): return _gizmos.EditableListBox_SetStrings(*args, **kwargs)
    def GetStrings(*args, **kwargs): return _gizmos.EditableListBox_GetStrings(*args, **kwargs)
    def GetListCtrl(*args, **kwargs): return _gizmos.EditableListBox_GetListCtrl(*args, **kwargs)
    def GetDelButton(*args, **kwargs): return _gizmos.EditableListBox_GetDelButton(*args, **kwargs)
    def GetNewButton(*args, **kwargs): return _gizmos.EditableListBox_GetNewButton(*args, **kwargs)
    def GetUpButton(*args, **kwargs): return _gizmos.EditableListBox_GetUpButton(*args, **kwargs)
    def GetDownButton(*args, **kwargs): return _gizmos.EditableListBox_GetDownButton(*args, **kwargs)
    def GetEditButton(*args, **kwargs): return _gizmos.EditableListBox_GetEditButton(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEditableListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class EditableListBoxPtr(EditableListBox):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EditableListBox
_gizmos.EditableListBox_swigregister(EditableListBoxPtr)

class RemotelyScrolledTreeCtrl(controls.TreeCtrl):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_RemotelyScrolledTreeCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def HideVScrollbar(*args, **kwargs): return _gizmos.RemotelyScrolledTreeCtrl_HideVScrollbar(*args, **kwargs)
    def AdjustRemoteScrollbars(*args, **kwargs): return _gizmos.RemotelyScrolledTreeCtrl_AdjustRemoteScrollbars(*args, **kwargs)
    def GetScrolledWindow(*args, **kwargs): return _gizmos.RemotelyScrolledTreeCtrl_GetScrolledWindow(*args, **kwargs)
    def ScrollToLine(*args, **kwargs): return _gizmos.RemotelyScrolledTreeCtrl_ScrollToLine(*args, **kwargs)
    def SetCompanionWindow(*args, **kwargs): return _gizmos.RemotelyScrolledTreeCtrl_SetCompanionWindow(*args, **kwargs)
    def GetCompanionWindow(*args, **kwargs): return _gizmos.RemotelyScrolledTreeCtrl_GetCompanionWindow(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRemotelyScrolledTreeCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class RemotelyScrolledTreeCtrlPtr(RemotelyScrolledTreeCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RemotelyScrolledTreeCtrl
_gizmos.RemotelyScrolledTreeCtrl_swigregister(RemotelyScrolledTreeCtrlPtr)

class TreeCompanionWindow(core.Window):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_TreeCompanionWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, TreeCompanionWindow)
    def _setCallbackInfo(*args, **kwargs): return _gizmos.TreeCompanionWindow__setCallbackInfo(*args, **kwargs)
    def GetTreeCtrl(*args, **kwargs): return _gizmos.TreeCompanionWindow_GetTreeCtrl(*args, **kwargs)
    def SetTreeCtrl(*args, **kwargs): return _gizmos.TreeCompanionWindow_SetTreeCtrl(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTreeCompanionWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class TreeCompanionWindowPtr(TreeCompanionWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeCompanionWindow
_gizmos.TreeCompanionWindow_swigregister(TreeCompanionWindowPtr)

class ThinSplitterWindow(windows.SplitterWindow):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_ThinSplitterWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxThinSplitterWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ThinSplitterWindowPtr(ThinSplitterWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ThinSplitterWindow
_gizmos.ThinSplitterWindow_swigregister(ThinSplitterWindowPtr)

class SplitterScrolledWindow(windows.ScrolledWindow):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_SplitterScrolledWindow(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxSplitterScrolledWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class SplitterScrolledWindowPtr(SplitterScrolledWindow):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = SplitterScrolledWindow
_gizmos.SplitterScrolledWindow_swigregister(SplitterScrolledWindowPtr)

LED_ALIGN_LEFT = _gizmos.LED_ALIGN_LEFT
LED_ALIGN_RIGHT = _gizmos.LED_ALIGN_RIGHT
LED_ALIGN_CENTER = _gizmos.LED_ALIGN_CENTER
LED_ALIGN_MASK = _gizmos.LED_ALIGN_MASK
LED_DRAW_FADED = _gizmos.LED_DRAW_FADED
class LEDNumberCtrl(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_LEDNumberCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _gizmos.LEDNumberCtrl_Create(*args, **kwargs)
    def GetAlignment(*args, **kwargs): return _gizmos.LEDNumberCtrl_GetAlignment(*args, **kwargs)
    def GetDrawFaded(*args, **kwargs): return _gizmos.LEDNumberCtrl_GetDrawFaded(*args, **kwargs)
    def GetValue(*args, **kwargs): return _gizmos.LEDNumberCtrl_GetValue(*args, **kwargs)
    def SetAlignment(*args, **kwargs): return _gizmos.LEDNumberCtrl_SetAlignment(*args, **kwargs)
    def SetDrawFaded(*args, **kwargs): return _gizmos.LEDNumberCtrl_SetDrawFaded(*args, **kwargs)
    def SetValue(*args, **kwargs): return _gizmos.LEDNumberCtrl_SetValue(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxLEDNumberCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class LEDNumberCtrlPtr(LEDNumberCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = LEDNumberCtrl
_gizmos.LEDNumberCtrl_swigregister(LEDNumberCtrlPtr)

def PreLEDNumberCtrl(*args, **kwargs):
    val = _gizmos.new_PreLEDNumberCtrl(*args, **kwargs)
    val.thisown = 1
    return val

TL_ALIGN_LEFT = _gizmos.TL_ALIGN_LEFT
TL_ALIGN_RIGHT = _gizmos.TL_ALIGN_RIGHT
TL_ALIGN_CENTER = _gizmos.TL_ALIGN_CENTER
TREE_HITTEST_ONITEMCOLUMN = _gizmos.TREE_HITTEST_ONITEMCOLUMN
class TreeListColumnInfo(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_TreeListColumnInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetAlignment(*args, **kwargs): return _gizmos.TreeListColumnInfo_GetAlignment(*args, **kwargs)
    def GetText(*args, **kwargs): return _gizmos.TreeListColumnInfo_GetText(*args, **kwargs)
    def GetImage(*args, **kwargs): return _gizmos.TreeListColumnInfo_GetImage(*args, **kwargs)
    def GetSelectedImage(*args, **kwargs): return _gizmos.TreeListColumnInfo_GetSelectedImage(*args, **kwargs)
    def GetWidth(*args, **kwargs): return _gizmos.TreeListColumnInfo_GetWidth(*args, **kwargs)
    def SetAlignment(*args, **kwargs): return _gizmos.TreeListColumnInfo_SetAlignment(*args, **kwargs)
    def SetText(*args, **kwargs): return _gizmos.TreeListColumnInfo_SetText(*args, **kwargs)
    def SetImage(*args, **kwargs): return _gizmos.TreeListColumnInfo_SetImage(*args, **kwargs)
    def SetSelectedImage(*args, **kwargs): return _gizmos.TreeListColumnInfo_SetSelectedImage(*args, **kwargs)
    def SetWidth(*args, **kwargs): return _gizmos.TreeListColumnInfo_SetWidth(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxTreeListColumnInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class TreeListColumnInfoPtr(TreeListColumnInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeListColumnInfo
_gizmos.TreeListColumnInfo_swigregister(TreeListColumnInfoPtr)

class TreeListCtrl(core.Control):
    def __init__(self, *args, **kwargs):
        newobj = _gizmos.new_TreeListCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self);self._setCallbackInfo(self, TreeListCtrl)
    def Create(*args, **kwargs): return _gizmos.TreeListCtrl_Create(*args, **kwargs)
    def _setCallbackInfo(*args, **kwargs): return _gizmos.TreeListCtrl__setCallbackInfo(*args, **kwargs)
    def GetCount(*args, **kwargs): return _gizmos.TreeListCtrl_GetCount(*args, **kwargs)
    def GetIndent(*args, **kwargs): return _gizmos.TreeListCtrl_GetIndent(*args, **kwargs)
    def SetIndent(*args, **kwargs): return _gizmos.TreeListCtrl_SetIndent(*args, **kwargs)
    def GetSpacing(*args, **kwargs): return _gizmos.TreeListCtrl_GetSpacing(*args, **kwargs)
    def SetSpacing(*args, **kwargs): return _gizmos.TreeListCtrl_SetSpacing(*args, **kwargs)
    def GetLineSpacing(*args, **kwargs): return _gizmos.TreeListCtrl_GetLineSpacing(*args, **kwargs)
    def SetLineSpacing(*args, **kwargs): return _gizmos.TreeListCtrl_SetLineSpacing(*args, **kwargs)
    def GetImageList(*args, **kwargs): return _gizmos.TreeListCtrl_GetImageList(*args, **kwargs)
    def GetStateImageList(*args, **kwargs): return _gizmos.TreeListCtrl_GetStateImageList(*args, **kwargs)
    def GetButtonsImageList(*args, **kwargs): return _gizmos.TreeListCtrl_GetButtonsImageList(*args, **kwargs)
    def SetImageList(*args, **kwargs): return _gizmos.TreeListCtrl_SetImageList(*args, **kwargs)
    def SetStateImageList(*args, **kwargs): return _gizmos.TreeListCtrl_SetStateImageList(*args, **kwargs)
    def SetButtonsImageList(*args, **kwargs): return _gizmos.TreeListCtrl_SetButtonsImageList(*args, **kwargs)
    def AssignImageList(*args, **kwargs): 
        val = _gizmos.TreeListCtrl_AssignImageList(*args, **kwargs)
        args[1].thisown = 0
        return val
    def AssignStateImageList(*args, **kwargs): 
        val = _gizmos.TreeListCtrl_AssignStateImageList(*args, **kwargs)
        args[1].thisown = 0
        return val
    def AssignButtonsImageList(*args, **kwargs): 
        val = _gizmos.TreeListCtrl_AssignButtonsImageList(*args, **kwargs)
        args[1].thisown = 0
        return val
    def AddColumn(*args, **kwargs): return _gizmos.TreeListCtrl_AddColumn(*args, **kwargs)
    def AddColumnInfo(*args, **kwargs): return _gizmos.TreeListCtrl_AddColumnInfo(*args, **kwargs)
    def InsertColumn(*args, **kwargs): return _gizmos.TreeListCtrl_InsertColumn(*args, **kwargs)
    def InsertColumnInfo(*args, **kwargs): return _gizmos.TreeListCtrl_InsertColumnInfo(*args, **kwargs)
    def RemoveColumn(*args, **kwargs): return _gizmos.TreeListCtrl_RemoveColumn(*args, **kwargs)
    def GetColumnCount(*args, **kwargs): return _gizmos.TreeListCtrl_GetColumnCount(*args, **kwargs)
    def SetColumnWidth(*args, **kwargs): return _gizmos.TreeListCtrl_SetColumnWidth(*args, **kwargs)
    def GetColumnWidth(*args, **kwargs): return _gizmos.TreeListCtrl_GetColumnWidth(*args, **kwargs)
    def SetMainColumn(*args, **kwargs): return _gizmos.TreeListCtrl_SetMainColumn(*args, **kwargs)
    def GetMainColumn(*args, **kwargs): return _gizmos.TreeListCtrl_GetMainColumn(*args, **kwargs)
    def SetColumnText(*args, **kwargs): return _gizmos.TreeListCtrl_SetColumnText(*args, **kwargs)
    def GetColumnText(*args, **kwargs): return _gizmos.TreeListCtrl_GetColumnText(*args, **kwargs)
    def SetColumn(*args, **kwargs): return _gizmos.TreeListCtrl_SetColumn(*args, **kwargs)
    def GetColumn(*args, **kwargs): return _gizmos.TreeListCtrl_GetColumn(*args, **kwargs)
    def SetColumnAlignment(*args, **kwargs): return _gizmos.TreeListCtrl_SetColumnAlignment(*args, **kwargs)
    def GetColumnAlignment(*args, **kwargs): return _gizmos.TreeListCtrl_GetColumnAlignment(*args, **kwargs)
    def SetColumnImage(*args, **kwargs): return _gizmos.TreeListCtrl_SetColumnImage(*args, **kwargs)
    def GetColumnImage(*args, **kwargs): return _gizmos.TreeListCtrl_GetColumnImage(*args, **kwargs)
    def GetItemText(*args, **kwargs): return _gizmos.TreeListCtrl_GetItemText(*args, **kwargs)
    def GetItemImage(*args, **kwargs): return _gizmos.TreeListCtrl_GetItemImage(*args, **kwargs)
    def SetItemText(*args, **kwargs): return _gizmos.TreeListCtrl_SetItemText(*args, **kwargs)
    def SetItemImage(*args, **kwargs): return _gizmos.TreeListCtrl_SetItemImage(*args, **kwargs)
    def GetItemData(*args, **kwargs): return _gizmos.TreeListCtrl_GetItemData(*args, **kwargs)
    def SetItemData(*args, **kwargs): return _gizmos.TreeListCtrl_SetItemData(*args, **kwargs)
    def GetItemPyData(*args, **kwargs): return _gizmos.TreeListCtrl_GetItemPyData(*args, **kwargs)
    def SetItemPyData(*args, **kwargs): return _gizmos.TreeListCtrl_SetItemPyData(*args, **kwargs)
    def SetItemHasChildren(*args, **kwargs): return _gizmos.TreeListCtrl_SetItemHasChildren(*args, **kwargs)
    def SetItemBold(*args, **kwargs): return _gizmos.TreeListCtrl_SetItemBold(*args, **kwargs)
    def SetItemTextColour(*args, **kwargs): return _gizmos.TreeListCtrl_SetItemTextColour(*args, **kwargs)
    def SetItemBackgroundColour(*args, **kwargs): return _gizmos.TreeListCtrl_SetItemBackgroundColour(*args, **kwargs)
    def SetItemFont(*args, **kwargs): return _gizmos.TreeListCtrl_SetItemFont(*args, **kwargs)
    def GetItemBold(*args, **kwargs): return _gizmos.TreeListCtrl_GetItemBold(*args, **kwargs)
    def GetItemTextColour(*args, **kwargs): return _gizmos.TreeListCtrl_GetItemTextColour(*args, **kwargs)
    def GetItemBackgroundColour(*args, **kwargs): return _gizmos.TreeListCtrl_GetItemBackgroundColour(*args, **kwargs)
    def GetItemFont(*args, **kwargs): return _gizmos.TreeListCtrl_GetItemFont(*args, **kwargs)
    def IsVisible(*args, **kwargs): return _gizmos.TreeListCtrl_IsVisible(*args, **kwargs)
    def ItemHasChildren(*args, **kwargs): return _gizmos.TreeListCtrl_ItemHasChildren(*args, **kwargs)
    def IsExpanded(*args, **kwargs): return _gizmos.TreeListCtrl_IsExpanded(*args, **kwargs)
    def IsSelected(*args, **kwargs): return _gizmos.TreeListCtrl_IsSelected(*args, **kwargs)
    def IsBold(*args, **kwargs): return _gizmos.TreeListCtrl_IsBold(*args, **kwargs)
    def GetChildrenCount(*args, **kwargs): return _gizmos.TreeListCtrl_GetChildrenCount(*args, **kwargs)
    def GetRootItem(*args, **kwargs): return _gizmos.TreeListCtrl_GetRootItem(*args, **kwargs)
    def GetSelection(*args, **kwargs): return _gizmos.TreeListCtrl_GetSelection(*args, **kwargs)
    def GetSelections(*args, **kwargs): return _gizmos.TreeListCtrl_GetSelections(*args, **kwargs)
    def GetItemParent(*args, **kwargs): return _gizmos.TreeListCtrl_GetItemParent(*args, **kwargs)
    def GetFirstChild(*args, **kwargs): return _gizmos.TreeListCtrl_GetFirstChild(*args, **kwargs)
    def GetNextChild(*args, **kwargs): return _gizmos.TreeListCtrl_GetNextChild(*args, **kwargs)
    def GetLastChild(*args, **kwargs): return _gizmos.TreeListCtrl_GetLastChild(*args, **kwargs)
    def GetNextSibling(*args, **kwargs): return _gizmos.TreeListCtrl_GetNextSibling(*args, **kwargs)
    def GetPrevSibling(*args, **kwargs): return _gizmos.TreeListCtrl_GetPrevSibling(*args, **kwargs)
    def GetFirstVisibleItem(*args, **kwargs): return _gizmos.TreeListCtrl_GetFirstVisibleItem(*args, **kwargs)
    def GetNextVisible(*args, **kwargs): return _gizmos.TreeListCtrl_GetNextVisible(*args, **kwargs)
    def GetPrevVisible(*args, **kwargs): return _gizmos.TreeListCtrl_GetPrevVisible(*args, **kwargs)
    def GetNext(*args, **kwargs): return _gizmos.TreeListCtrl_GetNext(*args, **kwargs)
    def AddRoot(*args, **kwargs): return _gizmos.TreeListCtrl_AddRoot(*args, **kwargs)
    def PrependItem(*args, **kwargs): return _gizmos.TreeListCtrl_PrependItem(*args, **kwargs)
    def InsertItem(*args, **kwargs): return _gizmos.TreeListCtrl_InsertItem(*args, **kwargs)
    def InsertItemBefore(*args, **kwargs): return _gizmos.TreeListCtrl_InsertItemBefore(*args, **kwargs)
    def AppendItem(*args, **kwargs): return _gizmos.TreeListCtrl_AppendItem(*args, **kwargs)
    def Delete(*args, **kwargs): return _gizmos.TreeListCtrl_Delete(*args, **kwargs)
    def DeleteChildren(*args, **kwargs): return _gizmos.TreeListCtrl_DeleteChildren(*args, **kwargs)
    def DeleteAllItems(*args, **kwargs): return _gizmos.TreeListCtrl_DeleteAllItems(*args, **kwargs)
    def Expand(*args, **kwargs): return _gizmos.TreeListCtrl_Expand(*args, **kwargs)
    def ExpandAll(*args, **kwargs): return _gizmos.TreeListCtrl_ExpandAll(*args, **kwargs)
    def Collapse(*args, **kwargs): return _gizmos.TreeListCtrl_Collapse(*args, **kwargs)
    def CollapseAndReset(*args, **kwargs): return _gizmos.TreeListCtrl_CollapseAndReset(*args, **kwargs)
    def Toggle(*args, **kwargs): return _gizmos.TreeListCtrl_Toggle(*args, **kwargs)
    def Unselect(*args, **kwargs): return _gizmos.TreeListCtrl_Unselect(*args, **kwargs)
    def UnselectAll(*args, **kwargs): return _gizmos.TreeListCtrl_UnselectAll(*args, **kwargs)
    def SelectItem(*args, **kwargs): return _gizmos.TreeListCtrl_SelectItem(*args, **kwargs)
    def EnsureVisible(*args, **kwargs): return _gizmos.TreeListCtrl_EnsureVisible(*args, **kwargs)
    def ScrollTo(*args, **kwargs): return _gizmos.TreeListCtrl_ScrollTo(*args, **kwargs)
    def HitTest(*args, **kwargs): return _gizmos.TreeListCtrl_HitTest(*args, **kwargs)
    def GetBoundingRect(*args, **kwargs): return _gizmos.TreeListCtrl_GetBoundingRect(*args, **kwargs)
    def EditLabel(*args, **kwargs): return _gizmos.TreeListCtrl_EditLabel(*args, **kwargs)
    def Edit(*args, **kwargs): return _gizmos.TreeListCtrl_Edit(*args, **kwargs)
    def SortChildren(*args, **kwargs): return _gizmos.TreeListCtrl_SortChildren(*args, **kwargs)
    def GetItemSelectedImage(*args, **kwargs): return _gizmos.TreeListCtrl_GetItemSelectedImage(*args, **kwargs)
    def SetItemSelectedImage(*args, **kwargs): return _gizmos.TreeListCtrl_SetItemSelectedImage(*args, **kwargs)
    def GetHeaderWindow(*args, **kwargs): return _gizmos.TreeListCtrl_GetHeaderWindow(*args, **kwargs)
    def GetMainWindow(*args, **kwargs): return _gizmos.TreeListCtrl_GetMainWindow(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTreeListCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class TreeListCtrlPtr(TreeListCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = TreeListCtrl
_gizmos.TreeListCtrl_swigregister(TreeListCtrlPtr)

def PreTreeListCtrl(*args, **kwargs):
    val = _gizmos.new_PreTreeListCtrl(*args, **kwargs)
    val.thisown = 1
    return val


