# This file was created automatically by SWIG.
import sizersc

from misc import *

from windows import *

from gdi import *

from fonts import *

from clip_dnd import *

from controls import *

from events import *
import wx
class wxSizerItemPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def DeleteWindows(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_DeleteWindows(self, *_args, **_kwargs)
        return val
    def DetachSizer(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_DetachSizer(self, *_args, **_kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_CalcMin(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetDimension(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetDimension(self, *_args, **_kwargs)
        return val
    def GetMinSize(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetMinSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetInitSize(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetInitSize(self, *_args, **_kwargs)
        return val
    def SetRatioWH(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetRatioWH(self, *_args, **_kwargs)
        return val
    def SetRatioSize(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetRatioSize(self, *_args, **_kwargs)
        return val
    def SetRatio(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetRatio(self, *_args, **_kwargs)
        return val
    def GetRatio(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetRatio(self, *_args, **_kwargs)
        return val
    def IsWindow(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_IsWindow(self, *_args, **_kwargs)
        return val
    def IsSizer(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_IsSizer(self, *_args, **_kwargs)
        return val
    def IsSpacer(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_IsSpacer(self, *_args, **_kwargs)
        return val
    def SetProportion(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetProportion(self, *_args, **_kwargs)
        return val
    def GetProportion(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetProportion(self, *_args, **_kwargs)
        return val
    def SetFlag(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetFlag(self, *_args, **_kwargs)
        return val
    def GetFlag(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetFlag(self, *_args, **_kwargs)
        return val
    def SetBorder(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetBorder(self, *_args, **_kwargs)
        return val
    def GetBorder(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetBorder(self, *_args, **_kwargs)
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetWindow(self, *_args, **_kwargs)
        return val
    def SetWindow(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetWindow(self, *_args, **_kwargs)
        return val
    def GetSizer(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetSizer(self, *_args, **_kwargs)
        return val
    def SetSizer(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetSizer(self, *_args, **_kwargs)
        return val
    def GetSpacer(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetSpacer(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) 
        return val
    def SetSpacer(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_SetSpacer(self, *_args, **_kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_Show(self, *_args, **_kwargs)
        return val
    def IsShown(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_IsShown(self, *_args, **_kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetPosition(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetUserData(self, *_args, **_kwargs):
        val = sizersc.wxSizerItem_GetUserData(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxSizerItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    SetOption = SetProportion
    GetOption = GetProportion
class wxSizerItem(wxSizerItemPtr):
    def __init__(self,this):
        self.this = this




class wxSizerPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setOORInfo(self, *_args, **_kwargs):
        val = sizersc.wxSizer__setOORInfo(self, *_args, **_kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = sizersc.wxSizer_Destroy(self, *_args, **_kwargs)
        return val
    def _Add(self, *_args, **_kwargs):
        val = sizersc.wxSizer__Add(self, *_args, **_kwargs)
        return val
    def _Insert(self, *_args, **_kwargs):
        val = sizersc.wxSizer__Insert(self, *_args, **_kwargs)
        return val
    def _Prepend(self, *_args, **_kwargs):
        val = sizersc.wxSizer__Prepend(self, *_args, **_kwargs)
        return val
    def Remove(self, *_args, **_kwargs):
        val = sizersc.wxSizer_Remove(self, *_args, **_kwargs)
        return val
    def _SetItemMinSize(self, *_args, **_kwargs):
        val = sizersc.wxSizer__SetItemMinSize(self, *_args, **_kwargs)
        return val
    def SetDimension(self, *_args, **_kwargs):
        val = sizersc.wxSizer_SetDimension(self, *_args, **_kwargs)
        return val
    def SetMinSize(self, *_args, **_kwargs):
        val = sizersc.wxSizer_SetMinSize(self, *_args, **_kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = sizersc.wxSizer_GetSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = sizersc.wxSizer_GetPosition(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetMinSize(self, *_args, **_kwargs):
        val = sizersc.wxSizer_GetMinSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def Layout(self, *_args, **_kwargs):
        val = sizersc.wxSizer_Layout(self, *_args, **_kwargs)
        return val
    def Fit(self, *_args, **_kwargs):
        val = sizersc.wxSizer_Fit(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def FitInside(self, *_args, **_kwargs):
        val = sizersc.wxSizer_FitInside(self, *_args, **_kwargs)
        return val
    def SetSizeHints(self, *_args, **_kwargs):
        val = sizersc.wxSizer_SetSizeHints(self, *_args, **_kwargs)
        return val
    def SetVirtualSizeHints(self, *_args, **_kwargs):
        val = sizersc.wxSizer_SetVirtualSizeHints(self, *_args, **_kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = sizersc.wxSizer_Clear(self, *_args, **_kwargs)
        return val
    def DeleteWindows(self, *_args, **_kwargs):
        val = sizersc.wxSizer_DeleteWindows(self, *_args, **_kwargs)
        return val
    def GetChildren(self, *_args, **_kwargs):
        val = sizersc.wxSizer_GetChildren(self, *_args, **_kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = sizersc.wxSizer_Show(self, *_args, **_kwargs)
        return val
    def Hide(self, *_args, **_kwargs):
        val = sizersc.wxSizer_Hide(self, *_args, **_kwargs)
        return val
    def IsShown(self, *_args, **_kwargs):
        val = sizersc.wxSizer_IsShown(self, *_args, **_kwargs)
        return val
    def ShowItems(self, *_args, **_kwargs):
        val = sizersc.wxSizer_ShowItems(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    
    def Add(self, item, *args, **kw):
        if type(item) == type(1):
            item = (item, args[0])  # backwards compatibility, args are width, height
            args = args[1:]
        self._Add(item, *args, **kw)

    def AddMany(self, widgets):
        for childinfo in widgets:
            if type(childinfo) != type(()):
                childinfo = (childinfo, )
            self.Add(*childinfo)

    def Prepend(self, item, *args, **kw):
        if type(item) == type(1):
            item = (item, args[0])  # backwards compatibility, args are width, height
            args = args[1:]
        self._Prepend(item, *args, **kw)

    def Insert(self, before, item, *args, **kw):
        if type(item) == type(1):
            item = (item, args[0])  # backwards compatibility, args are width, height
            args = args[1:]
        self._Insert(before, item, *args, **kw)


    # for backwards compatibility only
    AddWindow = AddSizer = AddSpacer = Add
    PrependWindow = PrependSizer = PrependSpacer = Prepend
    InsertWindow = InsertSizer = InsertSpacer = Insert
    RemoveWindow = RemoveSizer = RemovePos = Remove


    def SetItemMinSize(self, item, *args):
        if len(args) == 2:
            return self._SetItemMinSize(item, args)
        else:
            return self._SetItemMinSize(item, args[0])


    
    def GetSizeTuple(self):
        return self.GetSize().asTuple()
    def GetPositionTuple(self):
        return self.GetPosition().asTuple()
    def GetMinSizeTuple(self):
        return self.GetMinSize().asTuple()
    
class wxSizer(wxSizerPtr):
    def __init__(self,this):
        self.this = this




class wxPySizerPtr(wxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = sizersc.wxPySizer__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPySizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPySizer(wxPySizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = sizersc.new_wxPySizer(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPySizer)
        self._setOORInfo(self)




class wxBoxSizerPtr(wxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetOrientation(self, *_args, **_kwargs):
        val = sizersc.wxBoxSizer_GetOrientation(self, *_args, **_kwargs)
        return val
    def SetOrientation(self, *_args, **_kwargs):
        val = sizersc.wxBoxSizer_SetOrientation(self, *_args, **_kwargs)
        return val
    def RecalcSizes(self, *_args, **_kwargs):
        val = sizersc.wxBoxSizer_RecalcSizes(self, *_args, **_kwargs)
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = sizersc.wxBoxSizer_CalcMin(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxBoxSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxBoxSizer(wxBoxSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = sizersc.new_wxBoxSizer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxStaticBoxSizerPtr(wxBoxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetStaticBox(self, *_args, **_kwargs):
        val = sizersc.wxStaticBoxSizer_GetStaticBox(self, *_args, **_kwargs)
        return val
    def RecalcSizes(self, *_args, **_kwargs):
        val = sizersc.wxStaticBoxSizer_RecalcSizes(self, *_args, **_kwargs)
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = sizersc.wxStaticBoxSizer_CalcMin(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxStaticBoxSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxStaticBoxSizer(wxStaticBoxSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = sizersc.new_wxStaticBoxSizer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxNotebookSizerPtr(wxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def RecalcSizes(self, *_args, **_kwargs):
        val = sizersc.wxNotebookSizer_RecalcSizes(self, *_args, **_kwargs)
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = sizersc.wxNotebookSizer_CalcMin(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetNotebook(self, *_args, **_kwargs):
        val = sizersc.wxNotebookSizer_GetNotebook(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxNotebookSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxNotebookSizer(wxNotebookSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = sizersc.new_wxNotebookSizer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridSizerPtr(wxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def RecalcSizes(self, *_args, **_kwargs):
        val = sizersc.wxGridSizer_RecalcSizes(self, *_args, **_kwargs)
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = sizersc.wxGridSizer_CalcMin(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetCols(self, *_args, **_kwargs):
        val = sizersc.wxGridSizer_SetCols(self, *_args, **_kwargs)
        return val
    def SetRows(self, *_args, **_kwargs):
        val = sizersc.wxGridSizer_SetRows(self, *_args, **_kwargs)
        return val
    def SetVGap(self, *_args, **_kwargs):
        val = sizersc.wxGridSizer_SetVGap(self, *_args, **_kwargs)
        return val
    def SetHGap(self, *_args, **_kwargs):
        val = sizersc.wxGridSizer_SetHGap(self, *_args, **_kwargs)
        return val
    def GetCols(self, *_args, **_kwargs):
        val = sizersc.wxGridSizer_GetCols(self, *_args, **_kwargs)
        return val
    def GetRows(self, *_args, **_kwargs):
        val = sizersc.wxGridSizer_GetRows(self, *_args, **_kwargs)
        return val
    def GetVGap(self, *_args, **_kwargs):
        val = sizersc.wxGridSizer_GetVGap(self, *_args, **_kwargs)
        return val
    def GetHGap(self, *_args, **_kwargs):
        val = sizersc.wxGridSizer_GetHGap(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridSizer(wxGridSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = sizersc.new_wxGridSizer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxFlexGridSizerPtr(wxGridSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def RecalcSizes(self, *_args, **_kwargs):
        val = sizersc.wxFlexGridSizer_RecalcSizes(self, *_args, **_kwargs)
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = sizersc.wxFlexGridSizer_CalcMin(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def AddGrowableRow(self, *_args, **_kwargs):
        val = sizersc.wxFlexGridSizer_AddGrowableRow(self, *_args, **_kwargs)
        return val
    def RemoveGrowableRow(self, *_args, **_kwargs):
        val = sizersc.wxFlexGridSizer_RemoveGrowableRow(self, *_args, **_kwargs)
        return val
    def AddGrowableCol(self, *_args, **_kwargs):
        val = sizersc.wxFlexGridSizer_AddGrowableCol(self, *_args, **_kwargs)
        return val
    def RemoveGrowableCol(self, *_args, **_kwargs):
        val = sizersc.wxFlexGridSizer_RemoveGrowableCol(self, *_args, **_kwargs)
        return val
    def SetFlexibleDirection(self, *_args, **_kwargs):
        val = sizersc.wxFlexGridSizer_SetFlexibleDirection(self, *_args, **_kwargs)
        return val
    def GetFlexibleDirection(self, *_args, **_kwargs):
        val = sizersc.wxFlexGridSizer_GetFlexibleDirection(self, *_args, **_kwargs)
        return val
    def SetNonFlexibleGrowMode(self, *_args, **_kwargs):
        val = sizersc.wxFlexGridSizer_SetNonFlexibleGrowMode(self, *_args, **_kwargs)
        return val
    def GetNonFlexibleGrowMode(self, *_args, **_kwargs):
        val = sizersc.wxFlexGridSizer_GetNonFlexibleGrowMode(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxFlexGridSizer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxFlexGridSizer(wxFlexGridSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = sizersc.new_wxFlexGridSizer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxFLEX_GROWMODE_NONE = sizersc.wxFLEX_GROWMODE_NONE
wxFLEX_GROWMODE_SPECIFIED = sizersc.wxFLEX_GROWMODE_SPECIFIED
wxFLEX_GROWMODE_ALL = sizersc.wxFLEX_GROWMODE_ALL
