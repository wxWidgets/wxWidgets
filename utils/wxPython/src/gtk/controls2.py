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






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

