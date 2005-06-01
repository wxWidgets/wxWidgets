# This file was created automatically by SWIG.
import gizmosc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from fonts import *

from clip_dnd import *

from events import *

from streams import *

from utils import *

from mdi import *

from frames import *

from stattool import *

from controls import *

from controls2 import *

from windows2 import *

from cmndlgs import *

from windows3 import *

from image import *

from printfw import *

from sizers import *

from filesys import *

def EVT_DYNAMIC_SASH_SPLIT(win, id, func):
    win.Connect(id, -1, wxEVT_DYNAMIC_SASH_SPLIT, func)

def EVT_DYNAMIC_SASH_UNIFY(win, id, func):
    win.Connect(id, -1, wxEVT_DYNAMIC_SASH_UNIFY, func)

class wxDynamicSashSplitEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxDynamicSashSplitEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxDynamicSashSplitEvent(wxDynamicSashSplitEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxDynamicSashSplitEvent(*_args,**_kwargs)
        self.thisown = 1




class wxDynamicSashUnifyEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxDynamicSashUnifyEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxDynamicSashUnifyEvent(wxDynamicSashUnifyEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxDynamicSashUnifyEvent(*_args,**_kwargs)
        self.thisown = 1




class wxDynamicSashWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = gizmosc.wxDynamicSashWindow_Create(self, *_args, **_kwargs)
        return val
    def GetHScrollBar(self, *_args, **_kwargs):
        val = gizmosc.wxDynamicSashWindow_GetHScrollBar(self, *_args, **_kwargs)
        if val: val = wxScrollBarPtr(val) 
        return val
    def GetVScrollBar(self, *_args, **_kwargs):
        val = gizmosc.wxDynamicSashWindow_GetVScrollBar(self, *_args, **_kwargs)
        if val: val = wxScrollBarPtr(val) 
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxDynamicSashWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxDynamicSashWindow(wxDynamicSashWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxDynamicSashWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreDynamicSashWindow(*_args,**_kwargs):
    val = wxDynamicSashWindowPtr(gizmosc.new_wxPreDynamicSashWindow(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxEditableListBoxPtr(wxPanelPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetStrings(self, *_args, **_kwargs):
        val = gizmosc.wxEditableListBox_SetStrings(self, *_args, **_kwargs)
        return val
    def GetStrings(self, *_args, **_kwargs):
        val = gizmosc.wxEditableListBox_GetStrings(self, *_args, **_kwargs)
        return val
    def GetListCtrl(self, *_args, **_kwargs):
        val = gizmosc.wxEditableListBox_GetListCtrl(self, *_args, **_kwargs)
        return val
    def GetDelButton(self, *_args, **_kwargs):
        val = gizmosc.wxEditableListBox_GetDelButton(self, *_args, **_kwargs)
        return val
    def GetNewButton(self, *_args, **_kwargs):
        val = gizmosc.wxEditableListBox_GetNewButton(self, *_args, **_kwargs)
        return val
    def GetUpButton(self, *_args, **_kwargs):
        val = gizmosc.wxEditableListBox_GetUpButton(self, *_args, **_kwargs)
        return val
    def GetDownButton(self, *_args, **_kwargs):
        val = gizmosc.wxEditableListBox_GetDownButton(self, *_args, **_kwargs)
        return val
    def GetEditButton(self, *_args, **_kwargs):
        val = gizmosc.wxEditableListBox_GetEditButton(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxEditableListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxEditableListBox(wxEditableListBoxPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxEditableListBox(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxRemotelyScrolledTreeCtrlPtr(wxTreeCtrlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def HideVScrollbar(self, *_args, **_kwargs):
        val = gizmosc.wxRemotelyScrolledTreeCtrl_HideVScrollbar(self, *_args, **_kwargs)
        return val
    def AdjustRemoteScrollbars(self, *_args, **_kwargs):
        val = gizmosc.wxRemotelyScrolledTreeCtrl_AdjustRemoteScrollbars(self, *_args, **_kwargs)
        return val
    def GetScrolledWindow(self, *_args, **_kwargs):
        val = gizmosc.wxRemotelyScrolledTreeCtrl_GetScrolledWindow(self, *_args, **_kwargs)
        if val: val = wxScrolledWindowPtr(val) 
        return val
    def ScrollToLine(self, *_args, **_kwargs):
        val = gizmosc.wxRemotelyScrolledTreeCtrl_ScrollToLine(self, *_args, **_kwargs)
        return val
    def SetCompanionWindow(self, *_args, **_kwargs):
        val = gizmosc.wxRemotelyScrolledTreeCtrl_SetCompanionWindow(self, *_args, **_kwargs)
        return val
    def GetCompanionWindow(self, *_args, **_kwargs):
        val = gizmosc.wxRemotelyScrolledTreeCtrl_GetCompanionWindow(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxRemotelyScrolledTreeCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxRemotelyScrolledTreeCtrl(wxRemotelyScrolledTreeCtrlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxRemotelyScrolledTreeCtrl(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxTreeCompanionWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = gizmosc.wxTreeCompanionWindow__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def GetTreeCtrl(self, *_args, **_kwargs):
        val = gizmosc.wxTreeCompanionWindow_GetTreeCtrl(self, *_args, **_kwargs)
        if val: val = wxRemotelyScrolledTreeCtrlPtr(val) 
        return val
    def SetTreeCtrl(self, *_args, **_kwargs):
        val = gizmosc.wxTreeCompanionWindow_SetTreeCtrl(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxTreeCompanionWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxTreeCompanionWindow(wxTreeCompanionWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxTreeCompanionWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxTreeCompanionWindow)
        self._setOORInfo(self)




class wxThinSplitterWindowPtr(wxSplitterWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxThinSplitterWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxThinSplitterWindow(wxThinSplitterWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxThinSplitterWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxSplitterScrolledWindowPtr(wxScrolledWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxSplitterScrolledWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxSplitterScrolledWindow(wxSplitterScrolledWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxSplitterScrolledWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxLEDNumberCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = gizmosc.wxLEDNumberCtrl_Create(self, *_args, **_kwargs)
        return val
    def GetAlignment(self, *_args, **_kwargs):
        val = gizmosc.wxLEDNumberCtrl_GetAlignment(self, *_args, **_kwargs)
        return val
    def GetDrawFaded(self, *_args, **_kwargs):
        val = gizmosc.wxLEDNumberCtrl_GetDrawFaded(self, *_args, **_kwargs)
        return val
    def GetValue(self, *_args, **_kwargs):
        val = gizmosc.wxLEDNumberCtrl_GetValue(self, *_args, **_kwargs)
        return val
    def SetAlignment(self, *_args, **_kwargs):
        val = gizmosc.wxLEDNumberCtrl_SetAlignment(self, *_args, **_kwargs)
        return val
    def SetDrawFaded(self, *_args, **_kwargs):
        val = gizmosc.wxLEDNumberCtrl_SetDrawFaded(self, *_args, **_kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = gizmosc.wxLEDNumberCtrl_SetValue(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxLEDNumberCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxLEDNumberCtrl(wxLEDNumberCtrlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxLEDNumberCtrl(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreLEDNumberCtrl(*_args,**_kwargs):
    val = wxLEDNumberCtrlPtr(gizmosc.new_wxPreLEDNumberCtrl(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxTreeListColumnInfoPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetAlignment(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListColumnInfo_GetAlignment(self, *_args, **_kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListColumnInfo_GetText(self, *_args, **_kwargs)
        return val
    def GetImage(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListColumnInfo_GetImage(self, *_args, **_kwargs)
        return val
    def GetSelectedImage(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListColumnInfo_GetSelectedImage(self, *_args, **_kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListColumnInfo_GetWidth(self, *_args, **_kwargs)
        return val
    def SetAlignment(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListColumnInfo_SetAlignment(self, *_args, **_kwargs)
        return val
    def SetText(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListColumnInfo_SetText(self, *_args, **_kwargs)
        return val
    def SetImage(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListColumnInfo_SetImage(self, *_args, **_kwargs)
        return val
    def SetSelectedImage(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListColumnInfo_SetSelectedImage(self, *_args, **_kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListColumnInfo_SetWidth(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxTreeListColumnInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxTreeListColumnInfo(wxTreeListColumnInfoPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxTreeListColumnInfo(*_args,**_kwargs)
        self.thisown = 1




class wxTreeListCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_Create(self, *_args, **_kwargs)
        return val
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def GetCount(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetCount(self, *_args, **_kwargs)
        return val
    def GetIndent(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetIndent(self, *_args, **_kwargs)
        return val
    def SetIndent(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetIndent(self, *_args, **_kwargs)
        return val
    def GetSpacing(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetSpacing(self, *_args, **_kwargs)
        return val
    def SetSpacing(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetSpacing(self, *_args, **_kwargs)
        return val
    def GetLineSpacing(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetLineSpacing(self, *_args, **_kwargs)
        return val
    def SetLineSpacing(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetLineSpacing(self, *_args, **_kwargs)
        return val
    def GetImageList(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetImageList(self, *_args, **_kwargs)
        return val
    def GetStateImageList(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetStateImageList(self, *_args, **_kwargs)
        return val
    def GetButtonsImageList(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetButtonsImageList(self, *_args, **_kwargs)
        return val
    def SetImageList(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetImageList(self, *_args, **_kwargs)
        return val
    def SetStateImageList(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetStateImageList(self, *_args, **_kwargs)
        return val
    def SetButtonsImageList(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetButtonsImageList(self, *_args, **_kwargs)
        return val
    def AssignImageList(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_AssignImageList(self, *_args, **_kwargs)
        _args[0].thisown = 0
        return val
    def AssignStateImageList(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_AssignStateImageList(self, *_args, **_kwargs)
        _args[0].thisown = 0
        return val
    def AssignButtonsImageList(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_AssignButtonsImageList(self, *_args, **_kwargs)
        _args[0].thisown = 0
        return val
    def AddColumn(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_AddColumn(self, *_args, **_kwargs)
        return val
    def AddColumnInfo(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_AddColumnInfo(self, *_args, **_kwargs)
        return val
    def InsertColumn(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_InsertColumn(self, *_args, **_kwargs)
        return val
    def InsertColumnInfo(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_InsertColumnInfo(self, *_args, **_kwargs)
        return val
    def RemoveColumn(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_RemoveColumn(self, *_args, **_kwargs)
        return val
    def GetColumnCount(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetColumnCount(self, *_args, **_kwargs)
        return val
    def SetColumnWidth(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetColumnWidth(self, *_args, **_kwargs)
        return val
    def GetColumnWidth(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetColumnWidth(self, *_args, **_kwargs)
        return val
    def SetMainColumn(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetMainColumn(self, *_args, **_kwargs)
        return val
    def GetMainColumn(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetMainColumn(self, *_args, **_kwargs)
        return val
    def SetColumnText(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetColumnText(self, *_args, **_kwargs)
        return val
    def GetColumnText(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetColumnText(self, *_args, **_kwargs)
        return val
    def SetColumn(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetColumn(self, *_args, **_kwargs)
        return val
    def GetColumn(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetColumn(self, *_args, **_kwargs)
        if val: val = wxTreeListColumnInfoPtr(val) 
        return val
    def SetColumnAlignment(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetColumnAlignment(self, *_args, **_kwargs)
        return val
    def GetColumnAlignment(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetColumnAlignment(self, *_args, **_kwargs)
        return val
    def SetColumnImage(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetColumnImage(self, *_args, **_kwargs)
        return val
    def GetColumnImage(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetColumnImage(self, *_args, **_kwargs)
        return val
    def GetItemText(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetItemText(self, *_args, **_kwargs)
        return val
    def GetItemImage(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetItemImage(self, *_args, **_kwargs)
        return val
    def SetItemText(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetItemText(self, *_args, **_kwargs)
        return val
    def SetItemImage(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetItemImage(self, *_args, **_kwargs)
        return val
    def GetItemData(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetItemData(self, *_args, **_kwargs)
        if val: val = wxTreeItemDataPtr(val) 
        return val
    def SetItemData(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetItemData(self, *_args, **_kwargs)
        return val
    def GetPyData(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetPyData(self, *_args, **_kwargs)
        return val
    def SetPyData(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetPyData(self, *_args, **_kwargs)
        return val
    def SetItemHasChildren(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetItemHasChildren(self, *_args, **_kwargs)
        return val
    def SetItemBold(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetItemBold(self, *_args, **_kwargs)
        return val
    def SetItemTextColour(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetItemTextColour(self, *_args, **_kwargs)
        return val
    def SetItemBackgroundColour(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetItemBackgroundColour(self, *_args, **_kwargs)
        return val
    def SetItemFont(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetItemFont(self, *_args, **_kwargs)
        return val
    def GetItemBold(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetItemBold(self, *_args, **_kwargs)
        return val
    def GetItemTextColour(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetItemTextColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetItemBackgroundColour(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetItemBackgroundColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetItemFont(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetItemFont(self, *_args, **_kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def IsVisible(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_IsVisible(self, *_args, **_kwargs)
        return val
    def ItemHasChildren(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_ItemHasChildren(self, *_args, **_kwargs)
        return val
    def IsExpanded(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_IsExpanded(self, *_args, **_kwargs)
        return val
    def IsSelected(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_IsSelected(self, *_args, **_kwargs)
        return val
    def IsBold(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_IsBold(self, *_args, **_kwargs)
        return val
    def GetChildrenCount(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetChildrenCount(self, *_args, **_kwargs)
        return val
    def GetRootItem(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetRootItem(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetSelection(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def GetSelections(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetSelections(self, *_args, **_kwargs)
        return val
    def GetItemParent(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetItemParent(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def GetFirstChild(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetFirstChild(self, *_args, **_kwargs)
        return val
    def GetNextChild(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetNextChild(self, *_args, **_kwargs)
        return val
    def GetLastChild(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetLastChild(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def GetNextSibling(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetNextSibling(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def GetPrevSibling(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetPrevSibling(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def GetFirstVisibleItem(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetFirstVisibleItem(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def GetNextVisible(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetNextVisible(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def GetPrevVisible(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetPrevVisible(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def GetNext(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetNext(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def AddRoot(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_AddRoot(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def PrependItem(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_PrependItem(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def InsertItem(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_InsertItem(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def InsertItemBefore(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_InsertItemBefore(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def AppendItem(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_AppendItem(self, *_args, **_kwargs)
        if val: val = wxTreeItemIdPtr(val) ; val.thisown = 1
        return val
    def Delete(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_Delete(self, *_args, **_kwargs)
        return val
    def DeleteChildren(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_DeleteChildren(self, *_args, **_kwargs)
        return val
    def DeleteAllItems(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_DeleteAllItems(self, *_args, **_kwargs)
        return val
    def Expand(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_Expand(self, *_args, **_kwargs)
        return val
    def ExpandAll(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_ExpandAll(self, *_args, **_kwargs)
        return val
    def Collapse(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_Collapse(self, *_args, **_kwargs)
        return val
    def CollapseAndReset(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_CollapseAndReset(self, *_args, **_kwargs)
        return val
    def Toggle(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_Toggle(self, *_args, **_kwargs)
        return val
    def Unselect(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_Unselect(self, *_args, **_kwargs)
        return val
    def UnselectAll(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_UnselectAll(self, *_args, **_kwargs)
        return val
    def SelectItem(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SelectItem(self, *_args, **_kwargs)
        return val
    def EnsureVisible(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_EnsureVisible(self, *_args, **_kwargs)
        return val
    def ScrollTo(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_ScrollTo(self, *_args, **_kwargs)
        return val
    def HitTest(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_HitTest(self, *_args, **_kwargs)
        return val
    def GetBoundingRect(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetBoundingRect(self, *_args, **_kwargs)
        return val
    def EditLabel(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_EditLabel(self, *_args, **_kwargs)
        return val
    def Edit(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_Edit(self, *_args, **_kwargs)
        return val
    def SortChildren(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SortChildren(self, *_args, **_kwargs)
        return val
    def GetItemSelectedImage(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetItemSelectedImage(self, *_args, **_kwargs)
        return val
    def SetItemSelectedImage(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_SetItemSelectedImage(self, *_args, **_kwargs)
        return val
    def GetHeaderWindow(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetHeaderWindow(self, *_args, **_kwargs)
        return val
    def GetMainWindow(self, *_args, **_kwargs):
        val = gizmosc.wxTreeListCtrl_GetMainWindow(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxTreeListCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    
    # Redefine some methods that SWIG gets a bit confused on...
    def GetFirstChild(self, *_args, **_kwargs):
        val1,val2 = gizmosc.wxTreeListCtrl_GetFirstChild(self, *_args, **_kwargs)
        val1 = wxTreeItemIdPtr(val1)
        val1.thisown = 1
        return (val1,val2)
    def GetNextChild(self, *_args, **_kwargs):
        val1,val2 = gizmosc.wxTreeListCtrl_GetNextChild(self, *_args, **_kwargs)
        val1 = wxTreeItemIdPtr(val1)
        val1.thisown = 1
        return (val1,val2)
    def HitTest(self, *_args, **_kwargs):
        val1, val2, val3 = gizmosc.wxTreeListCtrl_HitTest(self, *_args, **_kwargs)
        val1 = wxTreeItemIdPtr(val1)
        val1.thisown = 1
        return (val1, val2, val3)

class wxTreeListCtrl(wxTreeListCtrlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gizmosc.new_wxTreeListCtrl(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxTreeListCtrl)
        self._setOORInfo(self)



def wxPreTreeListCtrl(*_args,**_kwargs):
    val = wxTreeListCtrlPtr(gizmosc.new_wxPreTreeListCtrl(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val




#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxEVT_DYNAMIC_SASH_SPLIT = gizmosc.wxEVT_DYNAMIC_SASH_SPLIT
wxEVT_DYNAMIC_SASH_UNIFY = gizmosc.wxEVT_DYNAMIC_SASH_UNIFY
wxDS_MANAGE_SCROLLBARS = gizmosc.wxDS_MANAGE_SCROLLBARS
wxDS_DRAG_CORNER = gizmosc.wxDS_DRAG_CORNER
wxEL_ALLOW_NEW = gizmosc.wxEL_ALLOW_NEW
wxEL_ALLOW_EDIT = gizmosc.wxEL_ALLOW_EDIT
wxEL_ALLOW_DELETE = gizmosc.wxEL_ALLOW_DELETE
wxLED_ALIGN_LEFT = gizmosc.wxLED_ALIGN_LEFT
wxLED_ALIGN_RIGHT = gizmosc.wxLED_ALIGN_RIGHT
wxLED_ALIGN_CENTER = gizmosc.wxLED_ALIGN_CENTER
wxLED_ALIGN_MASK = gizmosc.wxLED_ALIGN_MASK
wxLED_DRAW_FADED = gizmosc.wxLED_DRAW_FADED
wxTL_ALIGN_LEFT = gizmosc.wxTL_ALIGN_LEFT
wxTL_ALIGN_RIGHT = gizmosc.wxTL_ALIGN_RIGHT
wxTL_ALIGN_CENTER = gizmosc.wxTL_ALIGN_CENTER
wxTREE_HITTEST_ONITEMCOLUMN = gizmosc.wxTREE_HITTEST_ONITEMCOLUMN


#-------------- USER INCLUDE -----------------------

# Stuff these names into the wx namespace so wxPyConstructObject can find them

wx.wxDynamicSashSplitEventPtr       = wxDynamicSashSplitEventPtr
wx.wxDynamicSashUnifyEventPtr       = wxDynamicSashUnifyEventPtr
wx.wxDynamicSashWindowPtr           = wxDynamicSashWindowPtr

wx.wxEditableListBoxPtr             = wxEditableListBoxPtr

wx.wxRemotelyScrolledTreeCtrlPtr    = wxRemotelyScrolledTreeCtrlPtr
wx.wxTreeCompanionWindowPtr         = wxTreeCompanionWindowPtr
wx.wxThinSplitterWindowPtr          = wxThinSplitterWindowPtr
wx.wxSplitterScrolledWindowPtr      = wxSplitterScrolledWindowPtr
