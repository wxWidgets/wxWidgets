# This file was created automatically by SWIG.
import controls2c

from misc import *

from windows import *

from gdi import *

from events import *

from controls import *
import wxp
class wxListItemPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            controls2c.delete_wxListItem(self.this)
    def __setattr__(self,name,value):
        if name == "m_mask" :
            controls2c.wxListItem_m_mask_set(self.this,value)
            return
        if name == "m_itemId" :
            controls2c.wxListItem_m_itemId_set(self.this,value)
            return
        if name == "m_col" :
            controls2c.wxListItem_m_col_set(self.this,value)
            return
        if name == "m_state" :
            controls2c.wxListItem_m_state_set(self.this,value)
            return
        if name == "m_stateMask" :
            controls2c.wxListItem_m_stateMask_set(self.this,value)
            return
        if name == "m_text" :
            controls2c.wxListItem_m_text_set(self.this,value)
            return
        if name == "m_image" :
            controls2c.wxListItem_m_image_set(self.this,value)
            return
        if name == "m_data" :
            controls2c.wxListItem_m_data_set(self.this,value)
            return
        if name == "m_format" :
            controls2c.wxListItem_m_format_set(self.this,value)
            return
        if name == "m_width" :
            controls2c.wxListItem_m_width_set(self.this,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_mask" : 
            return controls2c.wxListItem_m_mask_get(self.this)
        if name == "m_itemId" : 
            return controls2c.wxListItem_m_itemId_get(self.this)
        if name == "m_col" : 
            return controls2c.wxListItem_m_col_get(self.this)
        if name == "m_state" : 
            return controls2c.wxListItem_m_state_get(self.this)
        if name == "m_stateMask" : 
            return controls2c.wxListItem_m_stateMask_get(self.this)
        if name == "m_text" : 
            return controls2c.wxListItem_m_text_get(self.this)
        if name == "m_image" : 
            return controls2c.wxListItem_m_image_get(self.this)
        if name == "m_data" : 
            return controls2c.wxListItem_m_data_get(self.this)
        if name == "m_format" : 
            return controls2c.wxListItem_m_format_get(self.this)
        if name == "m_width" : 
            return controls2c.wxListItem_m_width_get(self.this)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxListItem instance>"
class wxListItem(wxListItemPtr):
    def __init__(self) :
        self.this = controls2c.new_wxListItem()
        self.thisown = 1




class wxListEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "m_code" :
            controls2c.wxListEvent_m_code_set(self.this,value)
            return
        if name == "m_itemIndex" :
            controls2c.wxListEvent_m_itemIndex_set(self.this,value)
            return
        if name == "m_oldItemIndex" :
            controls2c.wxListEvent_m_oldItemIndex_set(self.this,value)
            return
        if name == "m_col" :
            controls2c.wxListEvent_m_col_set(self.this,value)
            return
        if name == "m_cancelled" :
            controls2c.wxListEvent_m_cancelled_set(self.this,value)
            return
        if name == "m_pointDrag" :
            controls2c.wxListEvent_m_pointDrag_set(self.this,value.this)
            return
        if name == "m_item" :
            controls2c.wxListEvent_m_item_set(self.this,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_code" : 
            return controls2c.wxListEvent_m_code_get(self.this)
        if name == "m_itemIndex" : 
            return controls2c.wxListEvent_m_itemIndex_get(self.this)
        if name == "m_oldItemIndex" : 
            return controls2c.wxListEvent_m_oldItemIndex_get(self.this)
        if name == "m_col" : 
            return controls2c.wxListEvent_m_col_get(self.this)
        if name == "m_cancelled" : 
            return controls2c.wxListEvent_m_cancelled_get(self.this)
        if name == "m_pointDrag" : 
            return wxPointPtr(controls2c.wxListEvent_m_pointDrag_get(self.this))
        if name == "m_item" : 
            return wxListItemPtr(controls2c.wxListEvent_m_item_get(self.this))
        raise AttributeError,name
    def __repr__(self):
        return "<C wxListEvent instance>"
class wxListEvent(wxListEventPtr):
    def __init__(self,this):
        self.this = this




class wxListCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Arrange(self,*args):
        val = apply(controls2c.wxListCtrl_Arrange,(self.this,)+args)
        return val
    def DeleteItem(self,arg0):
        val = controls2c.wxListCtrl_DeleteItem(self.this,arg0)
        return val
    def DeleteAllItems(self):
        val = controls2c.wxListCtrl_DeleteAllItems(self.this)
        return val
    def DeleteColumn(self,arg0):
        val = controls2c.wxListCtrl_DeleteColumn(self.this,arg0)
        return val
    def EnsureVisible(self,arg0):
        val = controls2c.wxListCtrl_EnsureVisible(self.this,arg0)
        return val
    def FindItem(self,arg0,arg1,*args):
        val = apply(controls2c.wxListCtrl_FindItem,(self.this,arg0,arg1,)+args)
        return val
    def FindItemData(self,arg0,arg1):
        val = controls2c.wxListCtrl_FindItemData(self.this,arg0,arg1)
        return val
    def FindItemAtPos(self,arg0,arg1,arg2):
        val = controls2c.wxListCtrl_FindItemAtPos(self.this,arg0,arg1.this,arg2)
        return val
    def GetColumn(self,arg0,arg1):
        val = controls2c.wxListCtrl_GetColumn(self.this,arg0,arg1.this)
        return val
    def GetColumnWidth(self,arg0):
        val = controls2c.wxListCtrl_GetColumnWidth(self.this,arg0)
        return val
    def GetCountPerPage(self):
        val = controls2c.wxListCtrl_GetCountPerPage(self.this)
        return val
    def GetImageList(self,arg0):
        val = controls2c.wxListCtrl_GetImageList(self.this,arg0)
        return val
    def GetItemData(self,arg0):
        val = controls2c.wxListCtrl_GetItemData(self.this,arg0)
        return val
    def GetItem(self):
        val = controls2c.wxListCtrl_GetItem(self.this)
        val = wxListItemPtr(val)
        val.thisown = 1
        return val
    def GetItemPosition(self,arg0):
        val = controls2c.wxListCtrl_GetItemPosition(self.this,arg0)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def GetItemRect(self,arg0,*args):
        val = apply(controls2c.wxListCtrl_GetItemRect,(self.this,arg0,)+args)
        val = wxRectPtr(val)
        val.thisown = 1
        return val
    def GetItemState(self,arg0,arg1):
        val = controls2c.wxListCtrl_GetItemState(self.this,arg0,arg1)
        return val
    def GetItemCount(self):
        val = controls2c.wxListCtrl_GetItemCount(self.this)
        return val
    def GetItemSpacing(self,arg0):
        val = controls2c.wxListCtrl_GetItemSpacing(self.this,arg0)
        return val
    def GetItemText(self,arg0):
        val = controls2c.wxListCtrl_GetItemText(self.this,arg0)
        return val
    def GetNextItem(self,arg0,*args):
        val = apply(controls2c.wxListCtrl_GetNextItem,(self.this,arg0,)+args)
        return val
    def GetSelectedItemCount(self):
        val = controls2c.wxListCtrl_GetSelectedItemCount(self.this)
        return val
    def GetTopItem(self):
        val = controls2c.wxListCtrl_GetTopItem(self.this)
        return val
    def HitTest(self,arg0):
        val = controls2c.wxListCtrl_HitTest(self.this,arg0.this)
        return val
    def InsertColumnWithInfo(self,arg0,arg1):
        val = controls2c.wxListCtrl_InsertColumnWithInfo(self.this,arg0,arg1.this)
        return val
    def InsertColumn(self,arg0,arg1,*args):
        val = apply(controls2c.wxListCtrl_InsertColumn,(self.this,arg0,arg1,)+args)
        return val
    def InsertItem(self,arg0):
        val = controls2c.wxListCtrl_InsertItem(self.this,arg0.this)
        return val
    def InsertStringItem(self,arg0,arg1):
        val = controls2c.wxListCtrl_InsertStringItem(self.this,arg0,arg1)
        return val
    def InsertImageItem(self,arg0,arg1):
        val = controls2c.wxListCtrl_InsertImageItem(self.this,arg0,arg1)
        return val
    def InsertImageStringItem(self,arg0,arg1,arg2):
        val = controls2c.wxListCtrl_InsertImageStringItem(self.this,arg0,arg1,arg2)
        return val
    def ScrollList(self,arg0,arg1):
        val = controls2c.wxListCtrl_ScrollList(self.this,arg0,arg1)
        return val
    def SetBackgroundColour(self,arg0):
        val = controls2c.wxListCtrl_SetBackgroundColour(self.this,arg0.this)
        return val
    def SetColumn(self,arg0,arg1):
        val = controls2c.wxListCtrl_SetColumn(self.this,arg0,arg1.this)
        return val
    def SetColumnWidth(self,arg0,arg1):
        val = controls2c.wxListCtrl_SetColumnWidth(self.this,arg0,arg1)
        return val
    def SetImageList(self,arg0,arg1):
        val = controls2c.wxListCtrl_SetImageList(self.this,arg0,arg1)
        return val
    def SetItem(self,arg0):
        val = controls2c.wxListCtrl_SetItem(self.this,arg0.this)
        return val
    def SetItemString(self,arg0,arg1,arg2,*args):
        val = apply(controls2c.wxListCtrl_SetItemString,(self.this,arg0,arg1,arg2,)+args)
        return val
    def SetItemData(self,arg0,arg1):
        val = controls2c.wxListCtrl_SetItemData(self.this,arg0,arg1)
        return val
    def SetItemImage(self,arg0,arg1,arg2):
        val = controls2c.wxListCtrl_SetItemImage(self.this,arg0,arg1,arg2)
        return val
    def SetItemPosition(self,arg0,arg1):
        val = controls2c.wxListCtrl_SetItemPosition(self.this,arg0,arg1.this)
        return val
    def SetItemState(self,arg0,arg1,arg2):
        val = controls2c.wxListCtrl_SetItemState(self.this,arg0,arg1,arg2)
        return val
    def SetItemText(self,arg0,arg1):
        val = controls2c.wxListCtrl_SetItemText(self.this,arg0,arg1)
        return val
    def SetSingleStyle(self,arg0,*args):
        val = apply(controls2c.wxListCtrl_SetSingleStyle,(self.this,arg0,)+args)
        return val
    def SetWindowStyleFlag(self,arg0):
        val = controls2c.wxListCtrl_SetWindowStyleFlag(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxListCtrl instance>"
class wxListCtrl(wxListCtrlPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(controls2c.new_wxListCtrl,(arg0.this,arg1,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxTreeItemPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            controls2c.delete_wxTreeItem(self.this)
    def __setattr__(self,name,value):
        if name == "m_mask" :
            controls2c.wxTreeItem_m_mask_set(self.this,value)
            return
        if name == "m_itemId" :
            controls2c.wxTreeItem_m_itemId_set(self.this,value)
            return
        if name == "m_state" :
            controls2c.wxTreeItem_m_state_set(self.this,value)
            return
        if name == "m_stateMask" :
            controls2c.wxTreeItem_m_stateMask_set(self.this,value)
            return
        if name == "m_text" :
            controls2c.wxTreeItem_m_text_set(self.this,value)
            return
        if name == "m_image" :
            controls2c.wxTreeItem_m_image_set(self.this,value)
            return
        if name == "m_selectedImage" :
            controls2c.wxTreeItem_m_selectedImage_set(self.this,value)
            return
        if name == "m_children" :
            controls2c.wxTreeItem_m_children_set(self.this,value)
            return
        if name == "m_data" :
            controls2c.wxTreeItem_m_data_set(self.this,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_mask" : 
            return controls2c.wxTreeItem_m_mask_get(self.this)
        if name == "m_itemId" : 
            return controls2c.wxTreeItem_m_itemId_get(self.this)
        if name == "m_state" : 
            return controls2c.wxTreeItem_m_state_get(self.this)
        if name == "m_stateMask" : 
            return controls2c.wxTreeItem_m_stateMask_get(self.this)
        if name == "m_text" : 
            return controls2c.wxTreeItem_m_text_get(self.this)
        if name == "m_image" : 
            return controls2c.wxTreeItem_m_image_get(self.this)
        if name == "m_selectedImage" : 
            return controls2c.wxTreeItem_m_selectedImage_get(self.this)
        if name == "m_children" : 
            return controls2c.wxTreeItem_m_children_get(self.this)
        if name == "m_data" : 
            return controls2c.wxTreeItem_m_data_get(self.this)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxTreeItem instance>"
class wxTreeItem(wxTreeItemPtr):
    def __init__(self) :
        self.this = controls2c.new_wxTreeItem()
        self.thisown = 1




class wxTreeEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __setattr__(self,name,value):
        if name == "m_code" :
            controls2c.wxTreeEvent_m_code_set(self.this,value)
            return
        if name == "m_item" :
            controls2c.wxTreeEvent_m_item_set(self.this,value.this)
            return
        if name == "m_oldItem" :
            controls2c.wxTreeEvent_m_oldItem_set(self.this,value)
            return
        if name == "m_pointDrag" :
            controls2c.wxTreeEvent_m_pointDrag_set(self.this,value.this)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_code" : 
            return controls2c.wxTreeEvent_m_code_get(self.this)
        if name == "m_item" : 
            return wxTreeItemPtr(controls2c.wxTreeEvent_m_item_get(self.this))
        if name == "m_oldItem" : 
            return controls2c.wxTreeEvent_m_oldItem_get(self.this)
        if name == "m_pointDrag" : 
            return wxPointPtr(controls2c.wxTreeEvent_m_pointDrag_get(self.this))
        raise AttributeError,name
    def __repr__(self):
        return "<C wxTreeEvent instance>"
class wxTreeEvent(wxTreeEventPtr):
    def __init__(self,this):
        self.this = this




class wxTreeCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def DeleteAllItems(self):
        val = controls2c.wxTreeCtrl_DeleteAllItems(self.this)
        return val
    def DeleteItem(self,arg0):
        val = controls2c.wxTreeCtrl_DeleteItem(self.this,arg0)
        return val
    def GetCount(self):
        val = controls2c.wxTreeCtrl_GetCount(self.this)
        return val
    def GetImageList(self,*args):
        val = apply(controls2c.wxTreeCtrl_GetImageList,(self.this,)+args)
        return val
    def GetIndent(self):
        val = controls2c.wxTreeCtrl_GetIndent(self.this)
        return val
    def GetItemData(self,arg0):
        val = controls2c.wxTreeCtrl_GetItemData(self.this,arg0)
        return val
    def GetItem(self):
        val = controls2c.wxTreeCtrl_GetItem(self.this)
        val = wxTreeItemPtr(val)
        val.thisown = 1
        return val
    def GetItemText(self,arg0):
        val = controls2c.wxTreeCtrl_GetItemText(self.this,arg0)
        return val
    def GetParent(self,arg0):
        val = controls2c.wxTreeCtrl_GetParent(self.this,arg0)
        return val
    def GetRootItem(self):
        val = controls2c.wxTreeCtrl_GetRootItem(self.this)
        return val
    def GetSelection(self):
        val = controls2c.wxTreeCtrl_GetSelection(self.this)
        return val
    def HitTest(self,arg0):
        val = controls2c.wxTreeCtrl_HitTest(self.this,arg0.this)
        return val
    def InsertItem(self,arg0,arg1,*args):
        val = apply(controls2c.wxTreeCtrl_InsertItem,(self.this,arg0,arg1.this,)+args)
        return val
    def InsertItemString(self,arg0,arg1,*args):
        val = apply(controls2c.wxTreeCtrl_InsertItemString,(self.this,arg0,arg1,)+args)
        return val
    def ItemHasChildren(self,arg0):
        val = controls2c.wxTreeCtrl_ItemHasChildren(self.this,arg0)
        return val
    def SelectItem(self,arg0):
        val = controls2c.wxTreeCtrl_SelectItem(self.this,arg0)
        return val
    def SetIndent(self,arg0):
        val = controls2c.wxTreeCtrl_SetIndent(self.this,arg0)
        return val
    def SetImageList(self,arg0,*args):
        val = apply(controls2c.wxTreeCtrl_SetImageList,(self.this,arg0,)+args)
        return val
    def SetItem(self,arg0):
        val = controls2c.wxTreeCtrl_SetItem(self.this,arg0.this)
        return val
    def SetItemImage(self,arg0,arg1,arg2):
        val = controls2c.wxTreeCtrl_SetItemImage(self.this,arg0,arg1,arg2)
        return val
    def SetItemText(self,arg0,arg1):
        val = controls2c.wxTreeCtrl_SetItemText(self.this,arg0,arg1)
        return val
    def SetItemData(self,arg0,arg1):
        val = controls2c.wxTreeCtrl_SetItemData(self.this,arg0,arg1)
        return val
    def __repr__(self):
        return "<C wxTreeCtrl instance>"
class wxTreeCtrl(wxTreeCtrlPtr):
    def __init__(self,arg0,*args) :
        argl = map(None,args)
        try: argl[1] = argl[1].this
        except: pass
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        self.this = apply(controls2c.new_wxTreeCtrl,(arg0.this,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxTREE_MASK_HANDLE = controls2c.wxTREE_MASK_HANDLE
wxTREE_MASK_STATE = controls2c.wxTREE_MASK_STATE
wxTREE_MASK_TEXT = controls2c.wxTREE_MASK_TEXT
wxTREE_MASK_IMAGE = controls2c.wxTREE_MASK_IMAGE
wxTREE_MASK_SELECTED_IMAGE = controls2c.wxTREE_MASK_SELECTED_IMAGE
wxTREE_MASK_CHILDREN = controls2c.wxTREE_MASK_CHILDREN
wxTREE_MASK_DATA = controls2c.wxTREE_MASK_DATA
wxTREE_STATE_BOLD = controls2c.wxTREE_STATE_BOLD
wxTREE_STATE_DROPHILITED = controls2c.wxTREE_STATE_DROPHILITED
wxTREE_STATE_EXPANDED = controls2c.wxTREE_STATE_EXPANDED
wxTREE_STATE_EXPANDEDONCE = controls2c.wxTREE_STATE_EXPANDEDONCE
wxTREE_STATE_FOCUSED = controls2c.wxTREE_STATE_FOCUSED
wxTREE_STATE_SELECTED = controls2c.wxTREE_STATE_SELECTED
wxTREE_STATE_CUT = controls2c.wxTREE_STATE_CUT
wxTREE_HITTEST_ABOVE = controls2c.wxTREE_HITTEST_ABOVE
wxTREE_HITTEST_BELOW = controls2c.wxTREE_HITTEST_BELOW
wxTREE_HITTEST_NOWHERE = controls2c.wxTREE_HITTEST_NOWHERE
wxTREE_HITTEST_ONITEMBUTTON = controls2c.wxTREE_HITTEST_ONITEMBUTTON
wxTREE_HITTEST_ONITEMICON = controls2c.wxTREE_HITTEST_ONITEMICON
wxTREE_HITTEST_ONITEMINDENT = controls2c.wxTREE_HITTEST_ONITEMINDENT
wxTREE_HITTEST_ONITEMLABEL = controls2c.wxTREE_HITTEST_ONITEMLABEL
wxTREE_HITTEST_ONITEMRIGHT = controls2c.wxTREE_HITTEST_ONITEMRIGHT
wxTREE_HITTEST_ONITEMSTATEICON = controls2c.wxTREE_HITTEST_ONITEMSTATEICON
wxTREE_HITTEST_TOLEFT = controls2c.wxTREE_HITTEST_TOLEFT
wxTREE_HITTEST_TORIGHT = controls2c.wxTREE_HITTEST_TORIGHT
wxTREE_HITTEST_ONITEM = controls2c.wxTREE_HITTEST_ONITEM
wxTREE_NEXT_CARET = controls2c.wxTREE_NEXT_CARET
wxTREE_NEXT_CHILD = controls2c.wxTREE_NEXT_CHILD
wxTREE_NEXT_DROPHILITE = controls2c.wxTREE_NEXT_DROPHILITE
wxTREE_NEXT_FIRSTVISIBLE = controls2c.wxTREE_NEXT_FIRSTVISIBLE
wxTREE_NEXT_NEXT = controls2c.wxTREE_NEXT_NEXT
wxTREE_NEXT_NEXTVISIBLE = controls2c.wxTREE_NEXT_NEXTVISIBLE
wxTREE_NEXT_PARENT = controls2c.wxTREE_NEXT_PARENT
wxTREE_NEXT_PREVIOUS = controls2c.wxTREE_NEXT_PREVIOUS
wxTREE_NEXT_PREVIOUSVISIBLE = controls2c.wxTREE_NEXT_PREVIOUSVISIBLE
wxTREE_NEXT_ROOT = controls2c.wxTREE_NEXT_ROOT
wxTREE_EXPAND_EXPAND = controls2c.wxTREE_EXPAND_EXPAND
wxTREE_EXPAND_COLLAPSE = controls2c.wxTREE_EXPAND_COLLAPSE
wxTREE_EXPAND_COLLAPSE_RESET = controls2c.wxTREE_EXPAND_COLLAPSE_RESET
wxTREE_EXPAND_TOGGLE = controls2c.wxTREE_EXPAND_TOGGLE
wxTREE_INSERT_LAST = controls2c.wxTREE_INSERT_LAST
wxTREE_INSERT_FIRST = controls2c.wxTREE_INSERT_FIRST
wxTREE_INSERT_SORT = controls2c.wxTREE_INSERT_SORT
