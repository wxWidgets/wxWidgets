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
    def DeleteAllColumns(self):
        val = controls2c.wxListCtrl_DeleteAllColumns(self.this)
        return val
    def ClearAll(self):
        val = controls2c.wxListCtrl_ClearAll(self.this)
        return val
    def EditLabel(self,arg0):
        val = controls2c.wxListCtrl_EditLabel(self.this,arg0)
        val = wxTextCtrlPtr(val)
        return val
    def EndEditLabel(self,arg0):
        val = controls2c.wxListCtrl_EndEditLabel(self.this,arg0)
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
    def GetEditControl(self):
        val = controls2c.wxListCtrl_GetEditControl(self.this)
        val = wxTextCtrlPtr(val)
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
    def GetTextColour(self):
        val = controls2c.wxListCtrl_GetTextColour(self.this)
        val = wxColourPtr(val)
        val.thisown = 1
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
    def SetTextColour(self,arg0):
        val = controls2c.wxListCtrl_SetTextColour(self.this,arg0.this)
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




class wxTreeItemIdPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            controls2c.delete_wxTreeItemId(self.this)
    def IsOk(self):
        val = controls2c.wxTreeItemId_IsOk(self.this)
        return val
    def GetId(self):
        val = controls2c.wxTreeItemId_GetId(self.this)
        return val
    def __repr__(self):
        return "<C wxTreeItemId instance>"
class wxTreeItemId(wxTreeItemIdPtr):
    def __init__(self) :
        self.this = controls2c.new_wxTreeItemId()
        self.thisown = 1




class wxTreeItemDataPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            controls2c.delete_wxTreeItemData(self.this)
    def GetItemId(self):
        val = controls2c.wxTreeItemData_GetItemId(self.this)
        val = wxTreeItemIdPtr(val)
        return val
    def __repr__(self):
        return "<C wxTreeItemData instance>"
class wxTreeItemData(wxTreeItemDataPtr):
    def __init__(self) :
        self.this = controls2c.new_wxTreeItemData()
        self.thisown = 1




class wxTreeEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetItem(self):
        val = controls2c.wxTreeEvent_GetItem(self.this)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def GetOldItem(self):
        val = controls2c.wxTreeEvent_GetOldItem(self.this)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def GetPoint(self):
        val = controls2c.wxTreeEvent_GetPoint(self.this)
        val = wxPointPtr(val)
        val.thisown = 1
        return val
    def GetCode(self):
        val = controls2c.wxTreeEvent_GetCode(self.this)
        return val
    def Veto(self):
        val = controls2c.wxTreeEvent_Veto(self.this)
        return val
    def __repr__(self):
        return "<C wxTreeEvent instance>"
class wxTreeEvent(wxTreeEventPtr):
    def __init__(self,this):
        self.this = this




class wxTreeCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetCount(self):
        val = controls2c.wxTreeCtrl_GetCount(self.this)
        return val
    def GetIndent(self):
        val = controls2c.wxTreeCtrl_GetIndent(self.this)
        return val
    def SetIndent(self,arg0):
        val = controls2c.wxTreeCtrl_SetIndent(self.this,arg0)
        return val
    def GetImageList(self):
        val = controls2c.wxTreeCtrl_GetImageList(self.this)
        return val
    def GetStateImageList(self):
        val = controls2c.wxTreeCtrl_GetStateImageList(self.this)
        return val
    def SetImageList(self,arg0):
        val = controls2c.wxTreeCtrl_SetImageList(self.this,arg0)
        return val
    def SetStateImageList(self,arg0):
        val = controls2c.wxTreeCtrl_SetStateImageList(self.this,arg0)
        return val
    def GetItemText(self,arg0):
        val = controls2c.wxTreeCtrl_GetItemText(self.this,arg0.this)
        return val
    def GetItemImage(self,arg0):
        val = controls2c.wxTreeCtrl_GetItemImage(self.this,arg0.this)
        return val
    def GetItemSelectedImage(self,arg0):
        val = controls2c.wxTreeCtrl_GetItemSelectedImage(self.this,arg0.this)
        return val
    def GetItemData(self,arg0):
        val = controls2c.wxTreeCtrl_GetItemData(self.this,arg0.this)
        val = wxTreeItemDataPtr(val)
        return val
    def SetItemText(self,arg0,arg1):
        val = controls2c.wxTreeCtrl_SetItemText(self.this,arg0.this,arg1)
        return val
    def SetItemImage(self,arg0,arg1):
        val = controls2c.wxTreeCtrl_SetItemImage(self.this,arg0.this,arg1)
        return val
    def SetItemSelectedImage(self,arg0,arg1):
        val = controls2c.wxTreeCtrl_SetItemSelectedImage(self.this,arg0.this,arg1)
        return val
    def SetItemData(self,arg0,arg1):
        val = controls2c.wxTreeCtrl_SetItemData(self.this,arg0.this,arg1.this)
        return val
    def IsVisible(self,arg0):
        val = controls2c.wxTreeCtrl_IsVisible(self.this,arg0.this)
        return val
    def ItemHasChildren(self,arg0):
        val = controls2c.wxTreeCtrl_ItemHasChildren(self.this,arg0.this)
        return val
    def IsExpanded(self,arg0):
        val = controls2c.wxTreeCtrl_IsExpanded(self.this,arg0.this)
        return val
    def IsSelected(self,arg0):
        val = controls2c.wxTreeCtrl_IsSelected(self.this,arg0.this)
        return val
    def GetRootItem(self):
        val = controls2c.wxTreeCtrl_GetRootItem(self.this)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def GetSelection(self):
        val = controls2c.wxTreeCtrl_GetSelection(self.this)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def GetParent(self,arg0):
        val = controls2c.wxTreeCtrl_GetParent(self.this,arg0.this)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def GetFirstChild(self,arg0,arg1):
        val = controls2c.wxTreeCtrl_GetFirstChild(self.this,arg0.this,arg1)
        return val
    def GetNextChild(self,arg0,arg1):
        val = controls2c.wxTreeCtrl_GetNextChild(self.this,arg0.this,arg1)
        return val
    def GetNextSibling(self,arg0):
        val = controls2c.wxTreeCtrl_GetNextSibling(self.this,arg0.this)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def GetPrevSibling(self,arg0):
        val = controls2c.wxTreeCtrl_GetPrevSibling(self.this,arg0.this)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def GetFirstVisibleItem(self):
        val = controls2c.wxTreeCtrl_GetFirstVisibleItem(self.this)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def GetNextVisible(self,arg0):
        val = controls2c.wxTreeCtrl_GetNextVisible(self.this,arg0.this)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def GetPrevVisible(self,arg0):
        val = controls2c.wxTreeCtrl_GetPrevVisible(self.this,arg0.this)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def AddRoot(self,arg0,*args):
        argl = map(None,args)
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        val = apply(controls2c.wxTreeCtrl_AddRoot,(self.this,arg0,)+args)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def PrependItem(self,arg0,arg1,*args):
        argl = map(None,args)
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        val = apply(controls2c.wxTreeCtrl_PrependItem,(self.this,arg0.this,arg1,)+args)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def InsertItem(self,arg0,arg1,arg2,*args):
        argl = map(None,args)
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        val = apply(controls2c.wxTreeCtrl_InsertItem,(self.this,arg0.this,arg1.this,arg2,)+args)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def AppendItem(self,arg0,arg1,*args):
        argl = map(None,args)
        try: argl[2] = argl[2].this
        except: pass
        args = tuple(argl)
        val = apply(controls2c.wxTreeCtrl_AppendItem,(self.this,arg0.this,arg1,)+args)
        val = wxTreeItemIdPtr(val)
        val.thisown = 1
        return val
    def Delete(self,arg0):
        val = controls2c.wxTreeCtrl_Delete(self.this,arg0.this)
        return val
    def DeleteAllItems(self):
        val = controls2c.wxTreeCtrl_DeleteAllItems(self.this)
        return val
    def Expand(self,arg0):
        val = controls2c.wxTreeCtrl_Expand(self.this,arg0.this)
        return val
    def Collapse(self,arg0):
        val = controls2c.wxTreeCtrl_Collapse(self.this,arg0.this)
        return val
    def CollapseAndReset(self,arg0):
        val = controls2c.wxTreeCtrl_CollapseAndReset(self.this,arg0.this)
        return val
    def Toggle(self,arg0):
        val = controls2c.wxTreeCtrl_Toggle(self.this,arg0.this)
        return val
    def Unselect(self):
        val = controls2c.wxTreeCtrl_Unselect(self.this)
        return val
    def SelectItem(self,arg0):
        val = controls2c.wxTreeCtrl_SelectItem(self.this,arg0.this)
        return val
    def EnsureVisible(self,arg0):
        val = controls2c.wxTreeCtrl_EnsureVisible(self.this,arg0.this)
        return val
    def ScrollTo(self,arg0):
        val = controls2c.wxTreeCtrl_ScrollTo(self.this,arg0.this)
        return val
    def EditLabel(self,arg0):
        val = controls2c.wxTreeCtrl_EditLabel(self.this,arg0.this)
        val = wxTextCtrlPtr(val)
        return val
    def GetEditControl(self):
        val = controls2c.wxTreeCtrl_GetEditControl(self.this)
        val = wxTextCtrlPtr(val)
        return val
    def EndEditLabel(self,arg0,*args):
        val = apply(controls2c.wxTreeCtrl_EndEditLabel,(self.this,arg0.this,)+args)
        return val
    def SortChildren(self,arg0):
        val = controls2c.wxTreeCtrl_SortChildren(self.this,arg0.this)
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

