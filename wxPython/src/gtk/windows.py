# This file was created automatically by SWIG.
import windowsc

from misc import *

from gdi import *

from fonts import *

from clip_dnd import *
import wx

def wxDLG_PNT(win, point_or_x, y=None):
    if y is None:
        return win.ConvertDialogPointToPixels(point_or_x)
    else:
        return win.ConvertDialogPointToPixels(wxPoint(point_or_x, y))

def wxDLG_SZE(win, size_width, height=None):
    if height is None:
        return win.ConvertDialogSizeToPixels(size_width)
    else:
        return win.ConvertDialogSizeToPixels(wxSize(size_width, height))

class wxEvtHandlerPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def ProcessEvent(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler_ProcessEvent(self, *_args, **_kwargs)
        return val
    def AddPendingEvent(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler_AddPendingEvent(self, *_args, **_kwargs)
        return val
    def GetEvtHandlerEnabled(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler_GetEvtHandlerEnabled(self, *_args, **_kwargs)
        return val
    def SetEvtHandlerEnabled(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler_SetEvtHandlerEnabled(self, *_args, **_kwargs)
        return val
    def GetNextHandler(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler_GetNextHandler(self, *_args, **_kwargs)
        return val
    def GetPreviousHandler(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler_GetPreviousHandler(self, *_args, **_kwargs)
        return val
    def SetNextHandler(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler_SetNextHandler(self, *_args, **_kwargs)
        return val
    def SetPreviousHandler(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler_SetPreviousHandler(self, *_args, **_kwargs)
        return val
    def Connect(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler_Connect(self, *_args, **_kwargs)
        return val
    def Disconnect(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler_Disconnect(self, *_args, **_kwargs)
        return val
    def _setOORInfo(self, *_args, **_kwargs):
        val = windowsc.wxEvtHandler__setOORInfo(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxEvtHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxEvtHandler(wxEvtHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windowsc.new_wxEvtHandler(*_args,**_kwargs)
        self.thisown = 1




class wxValidatorPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Clone(self, *_args, **_kwargs):
        val = windowsc.wxValidator_Clone(self, *_args, **_kwargs)
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = windowsc.wxValidator_GetWindow(self, *_args, **_kwargs)
        return val
    def SetWindow(self, *_args, **_kwargs):
        val = windowsc.wxValidator_SetWindow(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxValidator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxValidator(wxValidatorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windowsc.new_wxValidator(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxPyValidatorPtr(wxValidatorPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = windowsc.wxPyValidator__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPyValidator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPyValidator(wxPyValidatorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windowsc.new_wxPyValidator(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyValidator, 1)
        self._setOORInfo(self)




class wxWindowPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Create(self, *_args, **_kwargs)
        return val
    def Center(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Center(self, *_args, **_kwargs)
        return val
    def Centre(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Centre(self, *_args, **_kwargs)
        return val
    def CentreOnParent(self, *_args, **_kwargs):
        val = windowsc.wxWindow_CentreOnParent(self, *_args, **_kwargs)
        return val
    def CenterOnParent(self, *_args, **_kwargs):
        val = windowsc.wxWindow_CenterOnParent(self, *_args, **_kwargs)
        return val
    def CentreOnScreen(self, *_args, **_kwargs):
        val = windowsc.wxWindow_CentreOnScreen(self, *_args, **_kwargs)
        return val
    def CenterOnScreen(self, *_args, **_kwargs):
        val = windowsc.wxWindow_CenterOnScreen(self, *_args, **_kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Clear(self, *_args, **_kwargs)
        return val
    def ClientToScreenXY(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ClientToScreenXY(self, *_args, **_kwargs)
        return val
    def ClientToScreen(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ClientToScreen(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def Close(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Close(self, *_args, **_kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Destroy(self, *_args, **_kwargs)
        return val
    def DestroyChildren(self, *_args, **_kwargs):
        val = windowsc.wxWindow_DestroyChildren(self, *_args, **_kwargs)
        return val
    def IsBeingDeleted(self, *_args, **_kwargs):
        val = windowsc.wxWindow_IsBeingDeleted(self, *_args, **_kwargs)
        return val
    def Enable(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Enable(self, *_args, **_kwargs)
        return val
    def Disable(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Disable(self, *_args, **_kwargs)
        return val
    def FindWindowById(self, *_args, **_kwargs):
        val = windowsc.wxWindow_FindWindowById(self, *_args, **_kwargs)
        return val
    def FindWindowByName(self, *_args, **_kwargs):
        val = windowsc.wxWindow_FindWindowByName(self, *_args, **_kwargs)
        return val
    def Fit(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Fit(self, *_args, **_kwargs)
        return val
    def FitInside(self, *_args, **_kwargs):
        val = windowsc.wxWindow_FitInside(self, *_args, **_kwargs)
        return val
    def GetBackgroundColour(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetBackgroundColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetBorder(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetBorder(self, *_args, **_kwargs)
        return val
    def GetChildren(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetChildren(self, *_args, **_kwargs)
        return val
    def GetCharHeight(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetCharHeight(self, *_args, **_kwargs)
        return val
    def GetCharWidth(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetCharWidth(self, *_args, **_kwargs)
        return val
    def GetClientSizeTuple(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetClientSizeTuple(self, *_args, **_kwargs)
        return val
    def GetClientSize(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetClientSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetClientAreaOrigin(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetClientAreaOrigin(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetClientRect(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetClientRect(self, *_args, **_kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def GetConstraints(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetConstraints(self, *_args, **_kwargs)
        if val: val = wxLayoutConstraintsPtr(val) 
        return val
    def GetEventHandler(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetEventHandler(self, *_args, **_kwargs)
        return val
    def GetFont(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetFont(self, *_args, **_kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def GetForegroundColour(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetForegroundColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetGrandParent(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetGrandParent(self, *_args, **_kwargs)
        return val
    def GetHandle(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetHandle(self, *_args, **_kwargs)
        return val
    def GetId(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetId(self, *_args, **_kwargs)
        return val
    def GetLabel(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetLabel(self, *_args, **_kwargs)
        return val
    def SetLabel(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetLabel(self, *_args, **_kwargs)
        return val
    def GetName(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetName(self, *_args, **_kwargs)
        return val
    def GetParent(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetParent(self, *_args, **_kwargs)
        return val
    def GetPositionTuple(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetPositionTuple(self, *_args, **_kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetPosition(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetRect(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetRect(self, *_args, **_kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def GetScrollThumb(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetScrollThumb(self, *_args, **_kwargs)
        return val
    def GetScrollPos(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetScrollPos(self, *_args, **_kwargs)
        return val
    def GetScrollRange(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetScrollRange(self, *_args, **_kwargs)
        return val
    def GetSizeTuple(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetSizeTuple(self, *_args, **_kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetTextExtent(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetTextExtent(self, *_args, **_kwargs)
        return val
    def GetFullTextExtent(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetFullTextExtent(self, *_args, **_kwargs)
        return val
    def GetTitle(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetTitle(self, *_args, **_kwargs)
        return val
    def GetUpdateRegion(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetUpdateRegion(self, *_args, **_kwargs)
        if val: val = wxRegionPtr(val) ; val.thisown = 1
        return val
    def GetWindowStyleFlag(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetWindowStyleFlag(self, *_args, **_kwargs)
        return val
    def SetWindowStyleFlag(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetWindowStyleFlag(self, *_args, **_kwargs)
        return val
    def SetWindowStyle(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetWindowStyle(self, *_args, **_kwargs)
        return val
    def HasScrollbar(self, *_args, **_kwargs):
        val = windowsc.wxWindow_HasScrollbar(self, *_args, **_kwargs)
        return val
    def Hide(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Hide(self, *_args, **_kwargs)
        return val
    def HitTest(self, *_args, **_kwargs):
        val = windowsc.wxWindow_HitTest(self, *_args, **_kwargs)
        return val
    def InitDialog(self, *_args, **_kwargs):
        val = windowsc.wxWindow_InitDialog(self, *_args, **_kwargs)
        return val
    def IsEnabled(self, *_args, **_kwargs):
        val = windowsc.wxWindow_IsEnabled(self, *_args, **_kwargs)
        return val
    def IsExposed(self, *_args, **_kwargs):
        val = windowsc.wxWindow_IsExposed(self, *_args, **_kwargs)
        return val
    def IsExposedPoint(self, *_args, **_kwargs):
        val = windowsc.wxWindow_IsExposedPoint(self, *_args, **_kwargs)
        return val
    def IsExposedRect(self, *_args, **_kwargs):
        val = windowsc.wxWindow_IsExposedRect(self, *_args, **_kwargs)
        return val
    def IsRetained(self, *_args, **_kwargs):
        val = windowsc.wxWindow_IsRetained(self, *_args, **_kwargs)
        return val
    def IsShown(self, *_args, **_kwargs):
        val = windowsc.wxWindow_IsShown(self, *_args, **_kwargs)
        return val
    def IsTopLevel(self, *_args, **_kwargs):
        val = windowsc.wxWindow_IsTopLevel(self, *_args, **_kwargs)
        return val
    def Layout(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Layout(self, *_args, **_kwargs)
        return val
    def Lower(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Lower(self, *_args, **_kwargs)
        return val
    def MakeModal(self, *_args, **_kwargs):
        val = windowsc.wxWindow_MakeModal(self, *_args, **_kwargs)
        return val
    def MoveXY(self, *_args, **_kwargs):
        val = windowsc.wxWindow_MoveXY(self, *_args, **_kwargs)
        return val
    def Move(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Move(self, *_args, **_kwargs)
        return val
    def PopEventHandler(self, *_args, **_kwargs):
        val = windowsc.wxWindow_PopEventHandler(self, *_args, **_kwargs)
        return val
    def PushEventHandler(self, *_args, **_kwargs):
        val = windowsc.wxWindow_PushEventHandler(self, *_args, **_kwargs)
        return val
    def RemoveEventHandler(self, *_args, **_kwargs):
        val = windowsc.wxWindow_RemoveEventHandler(self, *_args, **_kwargs)
        return val
    def PopupMenuXY(self, *_args, **_kwargs):
        val = windowsc.wxWindow_PopupMenuXY(self, *_args, **_kwargs)
        return val
    def PopupMenu(self, *_args, **_kwargs):
        val = windowsc.wxWindow_PopupMenu(self, *_args, **_kwargs)
        return val
    def Raise(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Raise(self, *_args, **_kwargs)
        return val
    def Refresh(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Refresh(self, *_args, **_kwargs)
        return val
    def RefreshRect(self, *_args, **_kwargs):
        val = windowsc.wxWindow_RefreshRect(self, *_args, **_kwargs)
        return val
    def AddChild(self, *_args, **_kwargs):
        val = windowsc.wxWindow_AddChild(self, *_args, **_kwargs)
        return val
    def RemoveChild(self, *_args, **_kwargs):
        val = windowsc.wxWindow_RemoveChild(self, *_args, **_kwargs)
        return val
    def Reparent(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Reparent(self, *_args, **_kwargs)
        return val
    def ScreenToClientXY(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ScreenToClientXY(self, *_args, **_kwargs)
        return val
    def ScreenToClient(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ScreenToClient(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def ScrollWindow(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ScrollWindow(self, *_args, **_kwargs)
        return val
    def SetAutoLayout(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetAutoLayout(self, *_args, **_kwargs)
        return val
    def GetAutoLayout(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetAutoLayout(self, *_args, **_kwargs)
        return val
    def SetBackgroundColour(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetBackgroundColour(self, *_args, **_kwargs)
        return val
    def SetConstraints(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetConstraints(self, *_args, **_kwargs)
        return val
    def UnsetConstraints(self, *_args, **_kwargs):
        val = windowsc.wxWindow_UnsetConstraints(self, *_args, **_kwargs)
        return val
    def SetFocus(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetFocus(self, *_args, **_kwargs)
        return val
    def SetFocusFromKbd(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetFocusFromKbd(self, *_args, **_kwargs)
        return val
    def AcceptsFocus(self, *_args, **_kwargs):
        val = windowsc.wxWindow_AcceptsFocus(self, *_args, **_kwargs)
        return val
    def SetFont(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetFont(self, *_args, **_kwargs)
        return val
    def SetForegroundColour(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetForegroundColour(self, *_args, **_kwargs)
        return val
    def SetId(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetId(self, *_args, **_kwargs)
        return val
    def SetName(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetName(self, *_args, **_kwargs)
        return val
    def SetScrollbar(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetScrollbar(self, *_args, **_kwargs)
        return val
    def SetScrollPos(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetScrollPos(self, *_args, **_kwargs)
        return val
    def SetDimensions(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetDimensions(self, *_args, **_kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetSize(self, *_args, **_kwargs)
        return val
    def SetPosition(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetPosition(self, *_args, **_kwargs)
        return val
    def SetRect(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetRect(self, *_args, **_kwargs)
        return val
    def SetSizeHints(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetSizeHints(self, *_args, **_kwargs)
        return val
    def SetVirtualSizeHints(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetVirtualSizeHints(self, *_args, **_kwargs)
        return val
    def SetVirtualSize(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetVirtualSize(self, *_args, **_kwargs)
        return val
    def SetVirtualSizeWH(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetVirtualSizeWH(self, *_args, **_kwargs)
        return val
    def GetVirtualSize(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetVirtualSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetVirtualSizeTuple(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetVirtualSizeTuple(self, *_args, **_kwargs)
        return val
    def GetBestVirtualSize(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetBestVirtualSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetClientSizeWH(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetClientSizeWH(self, *_args, **_kwargs)
        return val
    def SetClientSize(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetClientSize(self, *_args, **_kwargs)
        return val
    def SetCursor(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetCursor(self, *_args, **_kwargs)
        return val
    def GetCursor(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetCursor(self, *_args, **_kwargs)
        if val: val = wxCursorPtr(val) 
        return val
    def SetEventHandler(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetEventHandler(self, *_args, **_kwargs)
        return val
    def SetExtraStyle(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetExtraStyle(self, *_args, **_kwargs)
        return val
    def SetTitle(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetTitle(self, *_args, **_kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Show(self, *_args, **_kwargs)
        return val
    def TransferDataFromWindow(self, *_args, **_kwargs):
        val = windowsc.wxWindow_TransferDataFromWindow(self, *_args, **_kwargs)
        return val
    def TransferDataToWindow(self, *_args, **_kwargs):
        val = windowsc.wxWindow_TransferDataToWindow(self, *_args, **_kwargs)
        return val
    def UpdateWindowUI(self, *_args, **_kwargs):
        val = windowsc.wxWindow_UpdateWindowUI(self, *_args, **_kwargs)
        return val
    def Validate(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Validate(self, *_args, **_kwargs)
        return val
    def ConvertDialogPointToPixels(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ConvertDialogPointToPixels(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def ConvertDialogSizeToPixels(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ConvertDialogSizeToPixels(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def DLG_PNT(self, *_args, **_kwargs):
        val = windowsc.wxWindow_DLG_PNT(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def DLG_SZE(self, *_args, **_kwargs):
        val = windowsc.wxWindow_DLG_SZE(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def ConvertPixelPointToDialog(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ConvertPixelPointToDialog(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def ConvertPixelSizeToDialog(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ConvertPixelSizeToDialog(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetToolTipString(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetToolTipString(self, *_args, **_kwargs)
        return val
    def SetToolTip(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetToolTip(self, *_args, **_kwargs)
        return val
    def GetToolTip(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetToolTip(self, *_args, **_kwargs)
        return val
    def SetSizer(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetSizer(self, *_args, **_kwargs)
        return val
    def SetSizerAndFit(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetSizerAndFit(self, *_args, **_kwargs)
        return val
    def GetSizer(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetSizer(self, *_args, **_kwargs)
        return val
    def SetContainingSizer(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetContainingSizer(self, *_args, **_kwargs)
        return val
    def GetContainingSizer(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetContainingSizer(self, *_args, **_kwargs)
        return val
    def GetValidator(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetValidator(self, *_args, **_kwargs)
        return val
    def SetValidator(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetValidator(self, *_args, **_kwargs)
        return val
    def SetDropTarget(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetDropTarget(self, *_args, **_kwargs)
        _args[0].thisown = 0
        return val
    def GetDropTarget(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetDropTarget(self, *_args, **_kwargs)
        if val: val = wxDropTargetPtr(val) 
        return val
    def GetBestSize(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetBestSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetMaxSize(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetMaxSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetAdjustedBestSize(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetAdjustedBestSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetCaret(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetCaret(self, *_args, **_kwargs)
        return val
    def GetCaret(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetCaret(self, *_args, **_kwargs)
        if val: val = wxCaretPtr(val) 
        return val
    def Freeze(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Freeze(self, *_args, **_kwargs)
        return val
    def Thaw(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Thaw(self, *_args, **_kwargs)
        return val
    def Update(self, *_args, **_kwargs):
        val = windowsc.wxWindow_Update(self, *_args, **_kwargs)
        return val
    def GetHelpText(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetHelpText(self, *_args, **_kwargs)
        return val
    def SetHelpText(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetHelpText(self, *_args, **_kwargs)
        return val
    def SetHelpTextForId(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetHelpTextForId(self, *_args, **_kwargs)
        return val
    def ScrollLines(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ScrollLines(self, *_args, **_kwargs)
        return val
    def ScrollPages(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ScrollPages(self, *_args, **_kwargs)
        return val
    def LineUp(self, *_args, **_kwargs):
        val = windowsc.wxWindow_LineUp(self, *_args, **_kwargs)
        return val
    def LineDown(self, *_args, **_kwargs):
        val = windowsc.wxWindow_LineDown(self, *_args, **_kwargs)
        return val
    def PageUp(self, *_args, **_kwargs):
        val = windowsc.wxWindow_PageUp(self, *_args, **_kwargs)
        return val
    def PageDown(self, *_args, **_kwargs):
        val = windowsc.wxWindow_PageDown(self, *_args, **_kwargs)
        return val
    def SetAcceleratorTable(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetAcceleratorTable(self, *_args, **_kwargs)
        return val
    def GetAcceleratorTable(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetAcceleratorTable(self, *_args, **_kwargs)
        if val: val = wxAcceleratorTablePtr(val) 
        return val
    def GetDefaultItem(self, *_args, **_kwargs):
        val = windowsc.wxWindow_GetDefaultItem(self, *_args, **_kwargs)
        return val
    def SetDefaultItem(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetDefaultItem(self, *_args, **_kwargs)
        return val
    def SetTmpDefaultItem(self, *_args, **_kwargs):
        val = windowsc.wxWindow_SetTmpDefaultItem(self, *_args, **_kwargs)
        return val
    def WarpPointer(self, *_args, **_kwargs):
        val = windowsc.wxWindow_WarpPointer(self, *_args, **_kwargs)
        return val
    def CaptureMouse(self, *_args, **_kwargs):
        val = windowsc.wxWindow_CaptureMouse(self, *_args, **_kwargs)
        return val
    def ReleaseMouse(self, *_args, **_kwargs):
        val = windowsc.wxWindow_ReleaseMouse(self, *_args, **_kwargs)
        return val
    def HasCapture(self, *_args, **_kwargs):
        val = windowsc.wxWindow_HasCapture(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    # replaces broken shadow method
    def GetCaret(self, *_args, **_kwargs):
        from misc2 import wxCaretPtr
        val = windowsc.wxWindow_GetCaret(self, *_args, **_kwargs)
        if val: val = wxCaretPtr(val)
        return val
    
class wxWindow(wxWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windowsc.new_wxWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreWindow(*_args,**_kwargs):
    val = wxWindowPtr(windowsc.new_wxPreWindow(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxPanelPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = windowsc.wxPanel_Create(self, *_args, **_kwargs)
        return val
    def InitDialog(self, *_args, **_kwargs):
        val = windowsc.wxPanel_InitDialog(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPanel instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPanel(wxPanelPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windowsc.new_wxPanel(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPrePanel(*_args,**_kwargs):
    val = wxPanelPtr(windowsc.new_wxPrePanel(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxScrolledWindowPtr(wxPanelPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_Create(self, *_args, **_kwargs)
        return val
    def EnableScrolling(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_EnableScrolling(self, *_args, **_kwargs)
        return val
    def GetScrollPageSize(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_GetScrollPageSize(self, *_args, **_kwargs)
        return val
    def GetScrollPixelsPerUnit(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_GetScrollPixelsPerUnit(self, *_args, **_kwargs)
        return val
    def GetTargetWindow(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_GetTargetWindow(self, *_args, **_kwargs)
        return val
    def IsRetained(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_IsRetained(self, *_args, **_kwargs)
        return val
    def PrepareDC(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_PrepareDC(self, *_args, **_kwargs)
        return val
    def Scroll(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_Scroll(self, *_args, **_kwargs)
        return val
    def SetScrollbars(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_SetScrollbars(self, *_args, **_kwargs)
        return val
    def SetScrollPageSize(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_SetScrollPageSize(self, *_args, **_kwargs)
        return val
    def SetTargetWindow(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_SetTargetWindow(self, *_args, **_kwargs)
        return val
    def GetViewStart(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_GetViewStart(self, *_args, **_kwargs)
        return val
    def CalcScrolledPosition1(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_CalcScrolledPosition1(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def CalcScrolledPosition2(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_CalcScrolledPosition2(self, *_args, **_kwargs)
        return val
    def CalcUnscrolledPosition1(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_CalcUnscrolledPosition1(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def CalcUnscrolledPosition2(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_CalcUnscrolledPosition2(self, *_args, **_kwargs)
        return val
    def SetScale(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_SetScale(self, *_args, **_kwargs)
        return val
    def GetScaleX(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_GetScaleX(self, *_args, **_kwargs)
        return val
    def GetScaleY(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_GetScaleY(self, *_args, **_kwargs)
        return val
    def AdjustScrollbars(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_AdjustScrollbars(self, *_args, **_kwargs)
        return val
    def Layout(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_Layout(self, *_args, **_kwargs)
        return val
    def SetScrollRate(self, *_args, **_kwargs):
        val = windowsc.wxScrolledWindow_SetScrollRate(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxScrolledWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    ViewStart = GetViewStart
    
    def CalcScrolledPosition(self, *args):
        if len(args) == 1:
            return self.CalcScrolledPosition1(*args)
        elif len(args) == 2:
            return self.CalcScrolledPosition2(*args)
        else:
            raise TypeError, 'Invalid parameters: only (x,y) or (point) allowed'

    def CalcUnscrolledPosition(self, *args):
        if len(args) == 1:
            return self.CalcUnscrolledPosition1(*args)
        elif len(args) == 2:
            return self.CalcUnscrolledPosition2(*args)
        else:
            raise TypeError, 'Invalid parameters: only (x,y) or (point) allowed'

class wxScrolledWindow(wxScrolledWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windowsc.new_wxScrolledWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreScrolledWindow(*_args,**_kwargs):
    val = wxScrolledWindowPtr(windowsc.new_wxPreScrolledWindow(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxMenuPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Append(self, *_args, **_kwargs):
        val = windowsc.wxMenu_Append(self, *_args, **_kwargs)
        return val
    def AppendMenu(self, *_args, **_kwargs):
        val = windowsc.wxMenu_AppendMenu(self, *_args, **_kwargs)
        return val
    def AppendItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_AppendItem(self, *_args, **_kwargs)
        return val
    def AppendCheckItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_AppendCheckItem(self, *_args, **_kwargs)
        return val
    def AppendRadioItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_AppendRadioItem(self, *_args, **_kwargs)
        return val
    def AppendSeparator(self, *_args, **_kwargs):
        val = windowsc.wxMenu_AppendSeparator(self, *_args, **_kwargs)
        return val
    def Insert(self, *_args, **_kwargs):
        val = windowsc.wxMenu_Insert(self, *_args, **_kwargs)
        return val
    def InsertSeparator(self, *_args, **_kwargs):
        val = windowsc.wxMenu_InsertSeparator(self, *_args, **_kwargs)
        return val
    def InsertCheckItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_InsertCheckItem(self, *_args, **_kwargs)
        return val
    def InsertRadioItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_InsertRadioItem(self, *_args, **_kwargs)
        return val
    def InsertMenu(self, *_args, **_kwargs):
        val = windowsc.wxMenu_InsertMenu(self, *_args, **_kwargs)
        return val
    def InsertItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_InsertItem(self, *_args, **_kwargs)
        return val
    def Prepend(self, *_args, **_kwargs):
        val = windowsc.wxMenu_Prepend(self, *_args, **_kwargs)
        return val
    def PrependSeparator(self, *_args, **_kwargs):
        val = windowsc.wxMenu_PrependSeparator(self, *_args, **_kwargs)
        return val
    def PrependCheckItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_PrependCheckItem(self, *_args, **_kwargs)
        return val
    def PrependRadioItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_PrependRadioItem(self, *_args, **_kwargs)
        return val
    def PrependMenu(self, *_args, **_kwargs):
        val = windowsc.wxMenu_PrependMenu(self, *_args, **_kwargs)
        return val
    def PrependItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_PrependItem(self, *_args, **_kwargs)
        return val
    def Break(self, *_args, **_kwargs):
        val = windowsc.wxMenu_Break(self, *_args, **_kwargs)
        return val
    def Check(self, *_args, **_kwargs):
        val = windowsc.wxMenu_Check(self, *_args, **_kwargs)
        return val
    def IsChecked(self, *_args, **_kwargs):
        val = windowsc.wxMenu_IsChecked(self, *_args, **_kwargs)
        return val
    def Enable(self, *_args, **_kwargs):
        val = windowsc.wxMenu_Enable(self, *_args, **_kwargs)
        return val
    def IsEnabled(self, *_args, **_kwargs):
        val = windowsc.wxMenu_IsEnabled(self, *_args, **_kwargs)
        return val
    def FindItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_FindItem(self, *_args, **_kwargs)
        return val
    def FindItemById(self, *_args, **_kwargs):
        val = windowsc.wxMenu_FindItemById(self, *_args, **_kwargs)
        return val
    def GetTitle(self, *_args, **_kwargs):
        val = windowsc.wxMenu_GetTitle(self, *_args, **_kwargs)
        return val
    def SetTitle(self, *_args, **_kwargs):
        val = windowsc.wxMenu_SetTitle(self, *_args, **_kwargs)
        return val
    def GetLabel(self, *_args, **_kwargs):
        val = windowsc.wxMenu_GetLabel(self, *_args, **_kwargs)
        return val
    def SetLabel(self, *_args, **_kwargs):
        val = windowsc.wxMenu_SetLabel(self, *_args, **_kwargs)
        return val
    def GetHelpString(self, *_args, **_kwargs):
        val = windowsc.wxMenu_GetHelpString(self, *_args, **_kwargs)
        return val
    def SetHelpString(self, *_args, **_kwargs):
        val = windowsc.wxMenu_SetHelpString(self, *_args, **_kwargs)
        return val
    def UpdateUI(self, *_args, **_kwargs):
        val = windowsc.wxMenu_UpdateUI(self, *_args, **_kwargs)
        return val
    def Delete(self, *_args, **_kwargs):
        val = windowsc.wxMenu_Delete(self, *_args, **_kwargs)
        return val
    def DeleteItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_DeleteItem(self, *_args, **_kwargs)
        return val
    def Remove(self, *_args, **_kwargs):
        val = windowsc.wxMenu_Remove(self, *_args, **_kwargs)
        return val
    def RemoveItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_RemoveItem(self, *_args, **_kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = windowsc.wxMenu_Destroy(self, *_args, **_kwargs)
        return val
    def DestroyId(self, *_args, **_kwargs):
        val = windowsc.wxMenu_DestroyId(self, *_args, **_kwargs)
        return val
    def DestroyItem(self, *_args, **_kwargs):
        val = windowsc.wxMenu_DestroyItem(self, *_args, **_kwargs)
        return val
    def GetMenuItemCount(self, *_args, **_kwargs):
        val = windowsc.wxMenu_GetMenuItemCount(self, *_args, **_kwargs)
        return val
    def GetMenuItems(self, *_args, **_kwargs):
        val = windowsc.wxMenu_GetMenuItems(self, *_args, **_kwargs)
        return val
    def SetEventHandler(self, *_args, **_kwargs):
        val = windowsc.wxMenu_SetEventHandler(self, *_args, **_kwargs)
        return val
    def GetEventHandler(self, *_args, **_kwargs):
        val = windowsc.wxMenu_GetEventHandler(self, *_args, **_kwargs)
        return val
    def SetInvokingWindow(self, *_args, **_kwargs):
        val = windowsc.wxMenu_SetInvokingWindow(self, *_args, **_kwargs)
        return val
    def GetInvokingWindow(self, *_args, **_kwargs):
        val = windowsc.wxMenu_GetInvokingWindow(self, *_args, **_kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = windowsc.wxMenu_GetStyle(self, *_args, **_kwargs)
        return val
    def IsAttached(self, *_args, **_kwargs):
        val = windowsc.wxMenu_IsAttached(self, *_args, **_kwargs)
        return val
    def SetParent(self, *_args, **_kwargs):
        val = windowsc.wxMenu_SetParent(self, *_args, **_kwargs)
        return val
    def GetParent(self, *_args, **_kwargs):
        val = windowsc.wxMenu_GetParent(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxMenu instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxMenu(wxMenuPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windowsc.new_wxMenu(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxMenuBarPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Append(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_Append(self, *_args, **_kwargs)
        return val
    def Insert(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_Insert(self, *_args, **_kwargs)
        return val
    def GetMenuCount(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_GetMenuCount(self, *_args, **_kwargs)
        return val
    def GetMenu(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_GetMenu(self, *_args, **_kwargs)
        return val
    def Replace(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_Replace(self, *_args, **_kwargs)
        return val
    def Remove(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_Remove(self, *_args, **_kwargs)
        return val
    def EnableTop(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_EnableTop(self, *_args, **_kwargs)
        return val
    def IsEnabledTop(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_IsEnabledTop(self, *_args, **_kwargs)
        return val
    def SetLabelTop(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_SetLabelTop(self, *_args, **_kwargs)
        return val
    def GetLabelTop(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_GetLabelTop(self, *_args, **_kwargs)
        return val
    def FindMenu(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_FindMenu(self, *_args, **_kwargs)
        return val
    def FindMenuItem(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_FindMenuItem(self, *_args, **_kwargs)
        return val
    def FindItemById(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_FindItemById(self, *_args, **_kwargs)
        return val
    def Enable(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_Enable(self, *_args, **_kwargs)
        return val
    def Check(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_Check(self, *_args, **_kwargs)
        return val
    def IsChecked(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_IsChecked(self, *_args, **_kwargs)
        return val
    def IsEnabled(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_IsEnabled(self, *_args, **_kwargs)
        return val
    def SetLabel(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_SetLabel(self, *_args, **_kwargs)
        return val
    def GetLabel(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_GetLabel(self, *_args, **_kwargs)
        return val
    def SetHelpString(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_SetHelpString(self, *_args, **_kwargs)
        return val
    def GetHelpString(self, *_args, **_kwargs):
        val = windowsc.wxMenuBar_GetHelpString(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxMenuBar instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxMenuBar(wxMenuBarPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windowsc.new_wxMenuBar(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxMenuItemPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetMenu(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_GetMenu(self, *_args, **_kwargs)
        return val
    def SetId(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_SetId(self, *_args, **_kwargs)
        return val
    def GetId(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_GetId(self, *_args, **_kwargs)
        return val
    def IsSeparator(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_IsSeparator(self, *_args, **_kwargs)
        return val
    def SetText(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_SetText(self, *_args, **_kwargs)
        return val
    def GetLabel(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_GetLabel(self, *_args, **_kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_GetText(self, *_args, **_kwargs)
        return val
    def GetKind(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_GetKind(self, *_args, **_kwargs)
        return val
    def SetCheckable(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_SetCheckable(self, *_args, **_kwargs)
        return val
    def IsCheckable(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_IsCheckable(self, *_args, **_kwargs)
        return val
    def IsSubMenu(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_IsSubMenu(self, *_args, **_kwargs)
        return val
    def SetSubMenu(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_SetSubMenu(self, *_args, **_kwargs)
        return val
    def GetSubMenu(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_GetSubMenu(self, *_args, **_kwargs)
        return val
    def Enable(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_Enable(self, *_args, **_kwargs)
        return val
    def IsEnabled(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_IsEnabled(self, *_args, **_kwargs)
        return val
    def Check(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_Check(self, *_args, **_kwargs)
        return val
    def IsChecked(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_IsChecked(self, *_args, **_kwargs)
        return val
    def Toggle(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_Toggle(self, *_args, **_kwargs)
        return val
    def SetHelp(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_SetHelp(self, *_args, **_kwargs)
        return val
    def GetHelp(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_GetHelp(self, *_args, **_kwargs)
        return val
    def GetAccel(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_GetAccel(self, *_args, **_kwargs)
        if val: val = wxAcceleratorEntryPtr(val) 
        return val
    def SetAccel(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_SetAccel(self, *_args, **_kwargs)
        return val
    def SetBitmap(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_SetBitmap(self, *_args, **_kwargs)
        return val
    def GetBitmap(self, *_args, **_kwargs):
        val = windowsc.wxMenuItem_GetBitmap(self, *_args, **_kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxMenuItem instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxMenuItem(wxMenuItemPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windowsc.new_wxMenuItem(*_args,**_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxFindWindowById(*_args, **_kwargs):
    val = windowsc.wxFindWindowById(*_args,**_kwargs)
    return val

def wxFindWindowByName(*_args, **_kwargs):
    val = windowsc.wxFindWindowByName(*_args,**_kwargs)
    return val

def wxFindWindowByLabel(*_args, **_kwargs):
    val = windowsc.wxFindWindowByLabel(*_args,**_kwargs)
    return val

wxValidator_IsSilent = windowsc.wxValidator_IsSilent

wxValidator_SetBellOnError = windowsc.wxValidator_SetBellOnError

def wxWindow_FindFocus(*_args, **_kwargs):
    val = windowsc.wxWindow_FindFocus(*_args,**_kwargs)
    return val

wxWindow_NewControlId = windowsc.wxWindow_NewControlId

wxWindow_NextControlId = windowsc.wxWindow_NextControlId

wxWindow_PrevControlId = windowsc.wxWindow_PrevControlId

def wxWindow_GetCapture(*_args, **_kwargs):
    val = windowsc.wxWindow_GetCapture(*_args,**_kwargs)
    return val

wxMenuItem_GetLabelFromText = windowsc.wxMenuItem_GetLabelFromText



#-------------- VARIABLE WRAPPERS ------------------

