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
        val = apply(sizersc.wxSizerItem_DeleteWindows,(self,) + _args, _kwargs)
        return val
    def DetachSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_DetachSizer,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_CalcMin,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetDimension(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetDimension,(self,) + _args, _kwargs)
        return val
    def GetMinSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetMinSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetInitSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetInitSize,(self,) + _args, _kwargs)
        return val
    def SetRatioWH(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetRatioWH,(self,) + _args, _kwargs)
        return val
    def SetRatioSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetRatioSize,(self,) + _args, _kwargs)
        return val
    def SetRatio(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetRatio,(self,) + _args, _kwargs)
        return val
    def GetRatio(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetRatio,(self,) + _args, _kwargs)
        return val
    def IsWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_IsWindow,(self,) + _args, _kwargs)
        return val
    def IsSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_IsSizer,(self,) + _args, _kwargs)
        return val
    def IsSpacer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_IsSpacer,(self,) + _args, _kwargs)
        return val
    def SetProportion(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetProportion,(self,) + _args, _kwargs)
        return val
    def GetProportion(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetProportion,(self,) + _args, _kwargs)
        return val
    def SetFlag(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetFlag,(self,) + _args, _kwargs)
        return val
    def GetFlag(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetFlag,(self,) + _args, _kwargs)
        return val
    def SetBorder(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetBorder,(self,) + _args, _kwargs)
        return val
    def GetBorder(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetBorder,(self,) + _args, _kwargs)
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetWindow,(self,) + _args, _kwargs)
        return val
    def SetWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetWindow,(self,) + _args, _kwargs)
        return val
    def GetSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetSizer,(self,) + _args, _kwargs)
        return val
    def SetSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetSizer,(self,) + _args, _kwargs)
        return val
    def GetSpacer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetSpacer,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) 
        return val
    def SetSpacer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetSpacer,(self,) + _args, _kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_Show,(self,) + _args, _kwargs)
        return val
    def IsShown(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_IsShown,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetUserData(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetUserData,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSizerItem instance at %s>" % (self.this,)
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
        val = apply(sizersc.wxSizer__setOORInfo,(self,) + _args, _kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_Destroy,(self,) + _args, _kwargs)
        return val
    def AddWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_AddWindow,(self,) + _args, _kwargs)
        return val
    def AddSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_AddSizer,(self,) + _args, _kwargs)
        return val
    def AddSpacer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_AddSpacer,(self,) + _args, _kwargs)
        return val
    def InsertWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_InsertWindow,(self,) + _args, _kwargs)
        return val
    def InsertSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_InsertSizer,(self,) + _args, _kwargs)
        return val
    def InsertSpacer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_InsertSpacer,(self,) + _args, _kwargs)
        return val
    def PrependWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_PrependWindow,(self,) + _args, _kwargs)
        return val
    def PrependSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_PrependSizer,(self,) + _args, _kwargs)
        return val
    def PrependSpacer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_PrependSpacer,(self,) + _args, _kwargs)
        return val
    def RemoveWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_RemoveWindow,(self,) + _args, _kwargs)
        return val
    def RemoveSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_RemoveSizer,(self,) + _args, _kwargs)
        return val
    def RemovePos(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_RemovePos,(self,) + _args, _kwargs)
        return val
    def DetachWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_DetachWindow,(self,) + _args, _kwargs)
        return val
    def DetachSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_DetachSizer,(self,) + _args, _kwargs)
        return val
    def DetachPos(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_DetachPos,(self,) + _args, _kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_Clear,(self,) + _args, _kwargs)
        return val
    def DeleteWindows(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_DeleteWindows,(self,) + _args, _kwargs)
        return val
    def SetMinSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetMinSize,(self,) + _args, _kwargs)
        return val
    def SetItemMinSizeWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetItemMinSizeWindow,(self,) + _args, _kwargs)
        return val
    def SetItemMinSizeSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetItemMinSizeSizer,(self,) + _args, _kwargs)
        return val
    def SetItemMinSizePos(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetItemMinSizePos,(self,) + _args, _kwargs)
        return val
    def SetItemMinSizeWindowWH(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetItemMinSizeWindowWH,(self,) + _args, _kwargs)
        return val
    def SetItemMinSizeSizerWH(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetItemMinSizeSizerWH,(self,) + _args, _kwargs)
        return val
    def SetItemMinSizePosWH(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetItemMinSizePosWH,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetMinSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_GetMinSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def Layout(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_Layout,(self,) + _args, _kwargs)
        return val
    def Fit(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_Fit,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def FitInside(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_FitInside,(self,) + _args, _kwargs)
        return val
    def SetSizeHints(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetSizeHints,(self,) + _args, _kwargs)
        return val
    def SetVirtualSizeHints(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetVirtualSizeHints,(self,) + _args, _kwargs)
        return val
    def GetChildren(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_GetChildren,(self,) + _args, _kwargs)
        return val
    def SetDimension(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetDimension,(self,) + _args, _kwargs)
        return val
    def ShowWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_ShowWindow,(self,) + _args, _kwargs)
        return val
    def ShowSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_ShowSizer,(self,) + _args, _kwargs)
        return val
    def ShowPos(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_ShowPos,(self,) + _args, _kwargs)
        return val
    def HideWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_HideWindow,(self,) + _args, _kwargs)
        return val
    def HideSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_HideSizer,(self,) + _args, _kwargs)
        return val
    def HidePos(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_HidePos,(self,) + _args, _kwargs)
        return val
    def IsShownWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_IsShownWindow,(self,) + _args, _kwargs)
        return val
    def IsShownSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_IsShownSizer,(self,) + _args, _kwargs)
        return val
    def IsShownPos(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_IsShownPos,(self,) + _args, _kwargs)
        return val
    def ShowItems(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_ShowItems,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSizer instance at %s>" % (self.this,)
    
    def Add(self, *args, **kw):
        if type(args[0]) == type(1):
            apply(self.AddSpacer, args, kw)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.AddSizer, args, kw)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.AddWindow, args, kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Insert(self, *args, **kw):
        if type(args[1]) == type(1):
            apply(self.InsertSpacer, args, kw)
        elif isinstance(args[1], wxSizerPtr):
            apply(self.InsertSizer, args, kw)
        elif isinstance(args[1], wxWindowPtr):
            apply(self.InsertWindow, args, kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Prepend(self, *args, **kw):
        if type(args[0]) == type(1):
            apply(self.PrependSpacer, args, kw)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.PrependSizer, args, kw)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.PrependWindow, args, kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Remove(self, *args, **kw):
        if type(args[0]) == type(1):
            return apply(self.RemovePos, args, kw)
        elif isinstance(args[0], wxSizerPtr):
            return apply(self.RemoveSizer, args, kw)
        elif isinstance(args[0], wxWindowPtr):
            return apply(self.RemoveWindow, args, kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Detach(self, *args, **kw):
        if type(args[0]) == type(1):
            return apply(self.DetachPos, args, kw)
        elif isinstance(args[0], wxSizerPtr):
            return apply(self.DetachSizer, args, kw)
        elif isinstance(args[0], wxWindowPtr):
            return apply(self.DetachWindow, args, kw)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def AddMany(self, widgets):
        for childinfo in widgets:
            if type(childinfo) != type(()):
                childinfo = (childinfo, )
            apply(self.Add, childinfo)

    
    def SetItemMinSize(self, *args):
        if type(args[0]) == type(1):
            apply(self.SetItemMinSizePos, args)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.SetItemMinSizeSizer, args)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.SetItemMinSizeWindow, args)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def SetItemMinSizeWH(self, *args):
        if type(args[0]) == type(1):
            apply(self.SetItemMinSizePosWH, args)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.SetItemMinSizeSizerWH, args)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.SetItemMinSizeWindowWH, args)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'
     
    
    def GetSizeTuple(self):
        return self.GetSize().asTuple()
    def GetPositionTuple(self):
        return self.GetPosition().asTuple()
    def GetMinSizeTuple(self):
        return self.GetMinSize().asTuple()
    
    
    def Show(self, *args):
        if type(args[0]) == type(1):
            apply(self.ShowPos, args)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.ShowSizer, args)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.ShowWindow, args)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def Hide(self, *args):
        if type(args[0]) == type(1):
            apply(self.HidePos, args)
        elif isinstance(args[0], wxSizerPtr):
            apply(self.HideSizer, args)
        elif isinstance(args[0], wxWindowPtr):
            apply(self.HideWindow, args)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

    def IsShown(self, *args):
        if type(args[0]) == type(1):
            return apply(self.IsShownPos, args)
        elif isinstance(args[0], wxSizerPtr):
            return apply(self.IsShownSizer, args)
        elif isinstance(args[0], wxWindowPtr):
            return apply(self.IsShownWindow, args)
        else:
            raise TypeError, 'Expected int, wxSizer or wxWindow parameter'

class wxSizer(wxSizerPtr):
    def __init__(self,this):
        self.this = this




class wxPySizerPtr(wxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = apply(sizersc.wxPySizer__setCallbackInfo,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPySizer instance at %s>" % (self.this,)
class wxPySizer(wxPySizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(sizersc.new_wxPySizer,_args,_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPySizer)
        self._setOORInfo(self)




class wxBoxSizerPtr(wxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetOrientation(self, *_args, **_kwargs):
        val = apply(sizersc.wxBoxSizer_GetOrientation,(self,) + _args, _kwargs)
        return val
    def SetOrientation(self, *_args, **_kwargs):
        val = apply(sizersc.wxBoxSizer_SetOrientation,(self,) + _args, _kwargs)
        return val
    def RecalcSizes(self, *_args, **_kwargs):
        val = apply(sizersc.wxBoxSizer_RecalcSizes,(self,) + _args, _kwargs)
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = apply(sizersc.wxBoxSizer_CalcMin,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxBoxSizer instance at %s>" % (self.this,)
class wxBoxSizer(wxBoxSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(sizersc.new_wxBoxSizer,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxStaticBoxSizerPtr(wxBoxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetStaticBox(self, *_args, **_kwargs):
        val = apply(sizersc.wxStaticBoxSizer_GetStaticBox,(self,) + _args, _kwargs)
        return val
    def RecalcSizes(self, *_args, **_kwargs):
        val = apply(sizersc.wxStaticBoxSizer_RecalcSizes,(self,) + _args, _kwargs)
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = apply(sizersc.wxStaticBoxSizer_CalcMin,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxStaticBoxSizer instance at %s>" % (self.this,)
class wxStaticBoxSizer(wxStaticBoxSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(sizersc.new_wxStaticBoxSizer,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxNotebookSizerPtr(wxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def RecalcSizes(self, *_args, **_kwargs):
        val = apply(sizersc.wxNotebookSizer_RecalcSizes,(self,) + _args, _kwargs)
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = apply(sizersc.wxNotebookSizer_CalcMin,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetNotebook(self, *_args, **_kwargs):
        val = apply(sizersc.wxNotebookSizer_GetNotebook,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxNotebookSizer instance at %s>" % (self.this,)
class wxNotebookSizer(wxNotebookSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(sizersc.new_wxNotebookSizer,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridSizerPtr(wxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def RecalcSizes(self, *_args, **_kwargs):
        val = apply(sizersc.wxGridSizer_RecalcSizes,(self,) + _args, _kwargs)
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = apply(sizersc.wxGridSizer_CalcMin,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetCols(self, *_args, **_kwargs):
        val = apply(sizersc.wxGridSizer_SetCols,(self,) + _args, _kwargs)
        return val
    def SetRows(self, *_args, **_kwargs):
        val = apply(sizersc.wxGridSizer_SetRows,(self,) + _args, _kwargs)
        return val
    def SetVGap(self, *_args, **_kwargs):
        val = apply(sizersc.wxGridSizer_SetVGap,(self,) + _args, _kwargs)
        return val
    def SetHGap(self, *_args, **_kwargs):
        val = apply(sizersc.wxGridSizer_SetHGap,(self,) + _args, _kwargs)
        return val
    def GetCols(self, *_args, **_kwargs):
        val = apply(sizersc.wxGridSizer_GetCols,(self,) + _args, _kwargs)
        return val
    def GetRows(self, *_args, **_kwargs):
        val = apply(sizersc.wxGridSizer_GetRows,(self,) + _args, _kwargs)
        return val
    def GetVGap(self, *_args, **_kwargs):
        val = apply(sizersc.wxGridSizer_GetVGap,(self,) + _args, _kwargs)
        return val
    def GetHGap(self, *_args, **_kwargs):
        val = apply(sizersc.wxGridSizer_GetHGap,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxGridSizer instance at %s>" % (self.this,)
class wxGridSizer(wxGridSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(sizersc.new_wxGridSizer,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxFlexGridSizerPtr(wxGridSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def RecalcSizes(self, *_args, **_kwargs):
        val = apply(sizersc.wxFlexGridSizer_RecalcSizes,(self,) + _args, _kwargs)
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = apply(sizersc.wxFlexGridSizer_CalcMin,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def AddGrowableRow(self, *_args, **_kwargs):
        val = apply(sizersc.wxFlexGridSizer_AddGrowableRow,(self,) + _args, _kwargs)
        return val
    def RemoveGrowableRow(self, *_args, **_kwargs):
        val = apply(sizersc.wxFlexGridSizer_RemoveGrowableRow,(self,) + _args, _kwargs)
        return val
    def AddGrowableCol(self, *_args, **_kwargs):
        val = apply(sizersc.wxFlexGridSizer_AddGrowableCol,(self,) + _args, _kwargs)
        return val
    def RemoveGrowableCol(self, *_args, **_kwargs):
        val = apply(sizersc.wxFlexGridSizer_RemoveGrowableCol,(self,) + _args, _kwargs)
        return val
    def SetFlexibleDirection(self, *_args, **_kwargs):
        val = apply(sizersc.wxFlexGridSizer_SetFlexibleDirection,(self,) + _args, _kwargs)
        return val
    def GetFlexibleDirection(self, *_args, **_kwargs):
        val = apply(sizersc.wxFlexGridSizer_GetFlexibleDirection,(self,) + _args, _kwargs)
        return val
    def SetNonFlexibleGrowMode(self, *_args, **_kwargs):
        val = apply(sizersc.wxFlexGridSizer_SetNonFlexibleGrowMode,(self,) + _args, _kwargs)
        return val
    def GetNonFlexibleGrowMode(self, *_args, **_kwargs):
        val = apply(sizersc.wxFlexGridSizer_GetNonFlexibleGrowMode,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFlexGridSizer instance at %s>" % (self.this,)
class wxFlexGridSizer(wxFlexGridSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(sizersc.new_wxFlexGridSizer,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxFLEX_GROWMODE_NONE = sizersc.wxFLEX_GROWMODE_NONE
wxFLEX_GROWMODE_SPECIFIED = sizersc.wxFLEX_GROWMODE_SPECIFIED
wxFLEX_GROWMODE_ALL = sizersc.wxFLEX_GROWMODE_ALL
