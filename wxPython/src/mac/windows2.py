# This file was created automatically by SWIG.
import windows2c

from misc import *

from gdi import *

from fonts import *

from windows import *

from clip_dnd import *

from controls import *

from events import *
import wx

# wxNotebook events
def EVT_NOTEBOOK_PAGE_CHANGED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, func)

def EVT_NOTEBOOK_PAGE_CHANGING(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, func)


#wxListbook events
def EVT_LISTBOOK_PAGE_CHANGED(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, func)

def EVT_LISTBOOK_PAGE_CHANGING(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING, func)

class wxBookCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPageCount(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_GetPageCount(self, *_args, **_kwargs)
        return val
    def GetPage(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_GetPage(self, *_args, **_kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_GetSelection(self, *_args, **_kwargs)
        return val
    def SetPageText(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_SetPageText(self, *_args, **_kwargs)
        return val
    def GetPageText(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_GetPageText(self, *_args, **_kwargs)
        return val
    def SetImageList(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_SetImageList(self, *_args, **_kwargs)
        return val
    def AssignImageList(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_AssignImageList(self, *_args, **_kwargs)
        _args[0].thisown = 0
        return val
    def GetImageList(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_GetImageList(self, *_args, **_kwargs)
        return val
    def GetPageImage(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_GetPageImage(self, *_args, **_kwargs)
        return val
    def SetPageImage(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_SetPageImage(self, *_args, **_kwargs)
        return val
    def SetPageSize(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_SetPageSize(self, *_args, **_kwargs)
        return val
    def CalcSizeFromPage(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_CalcSizeFromPage(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def DeletePage(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_DeletePage(self, *_args, **_kwargs)
        return val
    def RemovePage(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_RemovePage(self, *_args, **_kwargs)
        return val
    def DeleteAllPages(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_DeleteAllPages(self, *_args, **_kwargs)
        return val
    def AddPage(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_AddPage(self, *_args, **_kwargs)
        return val
    def InsertPage(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_InsertPage(self, *_args, **_kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_SetSelection(self, *_args, **_kwargs)
        return val
    def AdvanceSelection(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrl_AdvanceSelection(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxBookCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxBookCtrl(wxBookCtrlPtr):
    def __init__(self,this):
        self.this = this




class wxBookCtrlEventPtr(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSelection(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrlEvent_GetSelection(self, *_args, **_kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrlEvent_SetSelection(self, *_args, **_kwargs)
        return val
    def GetOldSelection(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrlEvent_GetOldSelection(self, *_args, **_kwargs)
        return val
    def SetOldSelection(self, *_args, **_kwargs):
        val = windows2c.wxBookCtrlEvent_SetOldSelection(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxBookCtrlEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxBookCtrlEvent(wxBookCtrlEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows2c.new_wxBookCtrlEvent(*_args,**_kwargs)
        self.thisown = 1




class wxNotebookPtr(wxBookCtrlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = windows2c.wxNotebook_Create(self, *_args, **_kwargs)
        return val
    def GetRowCount(self, *_args, **_kwargs):
        val = windows2c.wxNotebook_GetRowCount(self, *_args, **_kwargs)
        return val
    def SetPadding(self, *_args, **_kwargs):
        val = windows2c.wxNotebook_SetPadding(self, *_args, **_kwargs)
        return val
    def SetTabSize(self, *_args, **_kwargs):
        val = windows2c.wxNotebook_SetTabSize(self, *_args, **_kwargs)
        return val
    def HitTest(self, *_args, **_kwargs):
        val = windows2c.wxNotebook_HitTest(self, *_args, **_kwargs)
        return val
    def CalcSizeFromPage(self, *_args, **_kwargs):
        val = windows2c.wxNotebook_CalcSizeFromPage(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxNotebook instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxNotebook(wxNotebookPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows2c.new_wxNotebook(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreNotebook(*_args,**_kwargs):
    val = wxNotebookPtr(windows2c.new_wxPreNotebook(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxNotebookEventPtr(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSelection(self, *_args, **_kwargs):
        val = windows2c.wxNotebookEvent_GetSelection(self, *_args, **_kwargs)
        return val
    def GetOldSelection(self, *_args, **_kwargs):
        val = windows2c.wxNotebookEvent_GetOldSelection(self, *_args, **_kwargs)
        return val
    def SetOldSelection(self, *_args, **_kwargs):
        val = windows2c.wxNotebookEvent_SetOldSelection(self, *_args, **_kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = windows2c.wxNotebookEvent_SetSelection(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxNotebookEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxNotebookEvent(wxNotebookEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows2c.new_wxNotebookEvent(*_args,**_kwargs)
        self.thisown = 1




class wxListbookPtr(wxBookCtrlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = windows2c.wxListbook_Create(self, *_args, **_kwargs)
        return val
    def IsVertical(self, *_args, **_kwargs):
        val = windows2c.wxListbook_IsVertical(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxListbook instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxListbook(wxListbookPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows2c.new_wxListbook(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreListbook(*_args,**_kwargs):
    val = wxListbookPtr(windows2c.new_wxPreListbook(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxListbookEventPtr(wxBookCtrlEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxListbookEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxListbookEvent(wxListbookEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows2c.new_wxListbookEvent(*_args,**_kwargs)
        self.thisown = 1




class wxSplitterEventPtr(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSashPosition(self, *_args, **_kwargs):
        val = windows2c.wxSplitterEvent_GetSashPosition(self, *_args, **_kwargs)
        return val
    def GetX(self, *_args, **_kwargs):
        val = windows2c.wxSplitterEvent_GetX(self, *_args, **_kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = windows2c.wxSplitterEvent_GetY(self, *_args, **_kwargs)
        return val
    def GetWindowBeingRemoved(self, *_args, **_kwargs):
        val = windows2c.wxSplitterEvent_GetWindowBeingRemoved(self, *_args, **_kwargs)
        return val
    def SetSashPosition(self, *_args, **_kwargs):
        val = windows2c.wxSplitterEvent_SetSashPosition(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxSplitterEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxSplitterEvent(wxSplitterEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows2c.new_wxSplitterEvent(*_args,**_kwargs)
        self.thisown = 1




class wxSplitterWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_Create(self, *_args, **_kwargs)
        return val
    def GetWindow1(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_GetWindow1(self, *_args, **_kwargs)
        return val
    def GetWindow2(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_GetWindow2(self, *_args, **_kwargs)
        return val
    def SetSplitMode(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_SetSplitMode(self, *_args, **_kwargs)
        return val
    def GetSplitMode(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_GetSplitMode(self, *_args, **_kwargs)
        return val
    def Initialize(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_Initialize(self, *_args, **_kwargs)
        return val
    def SplitVertically(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_SplitVertically(self, *_args, **_kwargs)
        return val
    def SplitHorizontally(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_SplitHorizontally(self, *_args, **_kwargs)
        return val
    def Unsplit(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_Unsplit(self, *_args, **_kwargs)
        return val
    def ReplaceWindow(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_ReplaceWindow(self, *_args, **_kwargs)
        return val
    def IsSplit(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_IsSplit(self, *_args, **_kwargs)
        return val
    def SetSashSize(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_SetSashSize(self, *_args, **_kwargs)
        return val
    def SetBorderSize(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_SetBorderSize(self, *_args, **_kwargs)
        return val
    def GetSashSize(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_GetSashSize(self, *_args, **_kwargs)
        return val
    def GetBorderSize(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_GetBorderSize(self, *_args, **_kwargs)
        return val
    def SetSashPosition(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_SetSashPosition(self, *_args, **_kwargs)
        return val
    def GetSashPosition(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_GetSashPosition(self, *_args, **_kwargs)
        return val
    def SetMinimumPaneSize(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_SetMinimumPaneSize(self, *_args, **_kwargs)
        return val
    def GetMinimumPaneSize(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_GetMinimumPaneSize(self, *_args, **_kwargs)
        return val
    def SizeWindows(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_SizeWindows(self, *_args, **_kwargs)
        return val
    def SetNeedUpdating(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_SetNeedUpdating(self, *_args, **_kwargs)
        return val
    def GetNeedUpdating(self, *_args, **_kwargs):
        val = windows2c.wxSplitterWindow_GetNeedUpdating(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxSplitterWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxSplitterWindow(wxSplitterWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows2c.new_wxSplitterWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreSplitterWindow(*_args,**_kwargs):
    val = wxSplitterWindowPtr(windows2c.new_wxPreSplitterWindow(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxPyWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def base_DoMoveWindow(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_DoMoveWindow(self, *_args, **_kwargs)
        return val
    def base_DoSetSize(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_DoSetSize(self, *_args, **_kwargs)
        return val
    def base_DoSetClientSize(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_DoSetClientSize(self, *_args, **_kwargs)
        return val
    def base_DoSetVirtualSize(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_DoSetVirtualSize(self, *_args, **_kwargs)
        return val
    def base_DoGetSize(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_DoGetSize(self, *_args, **_kwargs)
        return val
    def base_DoGetClientSize(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_DoGetClientSize(self, *_args, **_kwargs)
        return val
    def base_DoGetPosition(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_DoGetPosition(self, *_args, **_kwargs)
        return val
    def base_DoGetVirtualSize(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_DoGetVirtualSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_DoGetBestSize(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_DoGetBestSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_InitDialog(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_InitDialog(self, *_args, **_kwargs)
        return val
    def base_TransferDataToWindow(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_TransferDataToWindow(self, *_args, **_kwargs)
        return val
    def base_TransferDataFromWindow(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_TransferDataFromWindow(self, *_args, **_kwargs)
        return val
    def base_Validate(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_Validate(self, *_args, **_kwargs)
        return val
    def base_AcceptsFocus(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_AcceptsFocus(self, *_args, **_kwargs)
        return val
    def base_AcceptsFocusFromKeyboard(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_AcceptsFocusFromKeyboard(self, *_args, **_kwargs)
        return val
    def base_GetMaxSize(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_GetMaxSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_AddChild(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_AddChild(self, *_args, **_kwargs)
        return val
    def base_RemoveChild(self, *_args, **_kwargs):
        val = windows2c.wxPyWindow_base_RemoveChild(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPyWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPyWindow(wxPyWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows2c.new_wxPyWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyWindow)
        self._setOORInfo(self)




class wxPyControlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = windows2c.wxPyControl__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def base_DoMoveWindow(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_DoMoveWindow(self, *_args, **_kwargs)
        return val
    def base_DoSetSize(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_DoSetSize(self, *_args, **_kwargs)
        return val
    def base_DoSetClientSize(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_DoSetClientSize(self, *_args, **_kwargs)
        return val
    def base_DoSetVirtualSize(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_DoSetVirtualSize(self, *_args, **_kwargs)
        return val
    def base_DoGetSize(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_DoGetSize(self, *_args, **_kwargs)
        return val
    def base_DoGetClientSize(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_DoGetClientSize(self, *_args, **_kwargs)
        return val
    def base_DoGetPosition(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_DoGetPosition(self, *_args, **_kwargs)
        return val
    def base_DoGetVirtualSize(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_DoGetVirtualSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_DoGetBestSize(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_DoGetBestSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_InitDialog(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_InitDialog(self, *_args, **_kwargs)
        return val
    def base_TransferDataToWindow(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_TransferDataToWindow(self, *_args, **_kwargs)
        return val
    def base_TransferDataFromWindow(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_TransferDataFromWindow(self, *_args, **_kwargs)
        return val
    def base_Validate(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_Validate(self, *_args, **_kwargs)
        return val
    def base_AcceptsFocus(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_AcceptsFocus(self, *_args, **_kwargs)
        return val
    def base_AcceptsFocusFromKeyboard(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_AcceptsFocusFromKeyboard(self, *_args, **_kwargs)
        return val
    def base_GetMaxSize(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_GetMaxSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_AddChild(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_AddChild(self, *_args, **_kwargs)
        return val
    def base_RemoveChild(self, *_args, **_kwargs):
        val = windows2c.wxPyControl_base_RemoveChild(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPyControl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPyControl(wxPyControlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows2c.new_wxPyControl(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyControl)
        self._setOORInfo(self)




class wxPyPanelPtr(wxPanelPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def base_DoMoveWindow(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_DoMoveWindow(self, *_args, **_kwargs)
        return val
    def base_DoSetSize(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_DoSetSize(self, *_args, **_kwargs)
        return val
    def base_DoSetClientSize(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_DoSetClientSize(self, *_args, **_kwargs)
        return val
    def base_DoSetVirtualSize(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_DoSetVirtualSize(self, *_args, **_kwargs)
        return val
    def base_DoGetSize(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_DoGetSize(self, *_args, **_kwargs)
        return val
    def base_DoGetClientSize(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_DoGetClientSize(self, *_args, **_kwargs)
        return val
    def base_DoGetPosition(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_DoGetPosition(self, *_args, **_kwargs)
        return val
    def base_DoGetVirtualSize(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_DoGetVirtualSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_DoGetBestSize(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_DoGetBestSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_InitDialog(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_InitDialog(self, *_args, **_kwargs)
        return val
    def base_TransferDataToWindow(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_TransferDataToWindow(self, *_args, **_kwargs)
        return val
    def base_TransferDataFromWindow(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_TransferDataFromWindow(self, *_args, **_kwargs)
        return val
    def base_Validate(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_Validate(self, *_args, **_kwargs)
        return val
    def base_AcceptsFocus(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_AcceptsFocus(self, *_args, **_kwargs)
        return val
    def base_AcceptsFocusFromKeyboard(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_AcceptsFocusFromKeyboard(self, *_args, **_kwargs)
        return val
    def base_GetMaxSize(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_GetMaxSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_AddChild(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_AddChild(self, *_args, **_kwargs)
        return val
    def base_RemoveChild(self, *_args, **_kwargs):
        val = windows2c.wxPyPanel_base_RemoveChild(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPyPanel instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPyPanel(wxPyPanelPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows2c.new_wxPyPanel(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyPanel)
        self._setOORInfo(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED = windows2c.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED
wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING = windows2c.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING
wxNB_FIXEDWIDTH = windows2c.wxNB_FIXEDWIDTH
wxNB_TOP = windows2c.wxNB_TOP
wxNB_LEFT = windows2c.wxNB_LEFT
wxNB_RIGHT = windows2c.wxNB_RIGHT
wxNB_BOTTOM = windows2c.wxNB_BOTTOM
wxNB_MULTILINE = windows2c.wxNB_MULTILINE
wxNB_HITTEST_NOWHERE = windows2c.wxNB_HITTEST_NOWHERE
wxNB_HITTEST_ONICON = windows2c.wxNB_HITTEST_ONICON
wxNB_HITTEST_ONLABEL = windows2c.wxNB_HITTEST_ONLABEL
wxNB_HITTEST_ONITEM = windows2c.wxNB_HITTEST_ONITEM
wxLB_DEFAULT = windows2c.wxLB_DEFAULT
wxLB_TOP = windows2c.wxLB_TOP
wxLB_BOTTOM = windows2c.wxLB_BOTTOM
wxLB_LEFT = windows2c.wxLB_LEFT
wxLB_RIGHT = windows2c.wxLB_RIGHT
wxLB_ALIGN_MASK = windows2c.wxLB_ALIGN_MASK
wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED = windows2c.wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED
wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING = windows2c.wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING
wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING = windows2c.wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING
wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED = windows2c.wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED
wxEVT_COMMAND_SPLITTER_UNSPLIT = windows2c.wxEVT_COMMAND_SPLITTER_UNSPLIT
wxEVT_COMMAND_SPLITTER_DOUBLECLICKED = windows2c.wxEVT_COMMAND_SPLITTER_DOUBLECLICKED
wxSPLIT_HORIZONTAL = windows2c.wxSPLIT_HORIZONTAL
wxSPLIT_VERTICAL = windows2c.wxSPLIT_VERTICAL
wxSPLIT_DRAG_NONE = windows2c.wxSPLIT_DRAG_NONE
wxSPLIT_DRAG_DRAGGING = windows2c.wxSPLIT_DRAG_DRAGGING
wxSPLIT_DRAG_LEFT_DOWN = windows2c.wxSPLIT_DRAG_LEFT_DOWN
wxSP_VERTICAL = windows2c.wxSP_VERTICAL
wxSP_HORIZONTAL = windows2c.wxSP_HORIZONTAL
wxSP_ARROW_KEYS = windows2c.wxSP_ARROW_KEYS
wxSP_WRAP = windows2c.wxSP_WRAP
wxSP_NOBORDER = windows2c.wxSP_NOBORDER
wxSP_3D = windows2c.wxSP_3D
wxSP_3DSASH = windows2c.wxSP_3DSASH
wxSP_3DBORDER = windows2c.wxSP_3DBORDER
wxSP_FULLSASH = windows2c.wxSP_FULLSASH
wxSP_BORDER = windows2c.wxSP_BORDER
wxSP_LIVE_UPDATE = windows2c.wxSP_LIVE_UPDATE
wxSP_PERMIT_UNSPLIT = windows2c.wxSP_PERMIT_UNSPLIT
