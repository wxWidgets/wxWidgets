# This file was created automatically by SWIG.
import wizardc

from windows import *

from misc import *

from gdi import *

from fonts import *

from clip_dnd import *

from frames import *

from stattool import *

from controls import *

from events import *

# wizard events
def EVT_WIZARD_PAGE_CHANGED(win, id, func):
    win.Connect(id, -1, wxEVT_WIZARD_PAGE_CHANGED, func)

def EVT_WIZARD_PAGE_CHANGING(win, id, func):
    win.Connect(id, -1, wxEVT_WIZARD_PAGE_CHANGING, func)

def EVT_WIZARD_CANCEL(win, id, func):
    win.Connect(id, -1, wxEVT_WIZARD_CANCEL, func)

def EVT_WIZARD_HELP(win, id, func):
    win.Connect(id, -1, wxEVT_WIZARD_HELP, func)

def EVT_WIZARD_FINISHED(win, id, func):
    win.Connect(id, -1, wxEVT_WIZARD_FINISHED, func)


class wxWizardEventPtr(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetDirection(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizardEvent_GetDirection,(self,) + _args, _kwargs)
        return val
    def GetPage(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizardEvent_GetPage,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxWizardEvent instance at %s>" % (self.this,)
class wxWizardEvent(wxWizardEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(wizardc.new_wxWizardEvent,_args,_kwargs)
        self.thisown = 1




class wxWizardPagePtr(wxPanelPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizardPage_Create,(self,) + _args, _kwargs)
        return val
    def GetPrev(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizardPage_GetPrev,(self,) + _args, _kwargs)
        return val
    def GetNext(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizardPage_GetNext,(self,) + _args, _kwargs)
        return val
    def GetBitmap(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizardPage_GetBitmap,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxWizardPage instance at %s>" % (self.this,)
class wxWizardPage(wxWizardPagePtr):
    def __init__(self,this):
        self.this = this




class wxPyWizardPagePtr(wxWizardPagePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_Create,(self,) + _args, _kwargs)
        return val
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage__setCallbackInfo,(self,) + _args, _kwargs)
        return val
    def base_DoMoveWindow(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_DoMoveWindow,(self,) + _args, _kwargs)
        return val
    def base_DoSetSize(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_DoSetSize,(self,) + _args, _kwargs)
        return val
    def base_DoSetClientSize(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_DoSetClientSize,(self,) + _args, _kwargs)
        return val
    def base_DoSetVirtualSize(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_DoSetVirtualSize,(self,) + _args, _kwargs)
        return val
    def base_DoGetSize(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_DoGetSize,(self,) + _args, _kwargs)
        return val
    def base_DoGetClientSize(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_DoGetClientSize,(self,) + _args, _kwargs)
        return val
    def base_DoGetPosition(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_DoGetPosition,(self,) + _args, _kwargs)
        return val
    def base_DoGetVirtualSize(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_DoGetVirtualSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_DoGetBestSize(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_DoGetBestSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_InitDialog(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_InitDialog,(self,) + _args, _kwargs)
        return val
    def base_TransferDataToWindow(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_TransferDataToWindow,(self,) + _args, _kwargs)
        return val
    def base_TransferDataFromWindow(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_TransferDataFromWindow,(self,) + _args, _kwargs)
        return val
    def base_Validate(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_Validate,(self,) + _args, _kwargs)
        return val
    def base_AcceptsFocus(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_AcceptsFocus,(self,) + _args, _kwargs)
        return val
    def base_AcceptsFocusFromKeyboard(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_AcceptsFocusFromKeyboard,(self,) + _args, _kwargs)
        return val
    def base_GetMaxSize(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_GetMaxSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def base_AddChild(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_AddChild,(self,) + _args, _kwargs)
        return val
    def base_RemoveChild(self, *_args, **_kwargs):
        val = apply(wizardc.wxPyWizardPage_base_RemoveChild,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyWizardPage instance at %s>" % (self.this,)
class wxPyWizardPage(wxPyWizardPagePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(wizardc.new_wxPyWizardPage,_args,_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyWizardPage)
        self._setOORInfo(self)



def wxPrePyWizardPage(*_args,**_kwargs):
    val = wxPyWizardPagePtr(apply(wizardc.new_wxPrePyWizardPage,_args,_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxWizardPageSimplePtr(wxWizardPagePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizardPageSimple_Create,(self,) + _args, _kwargs)
        return val
    def SetPrev(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizardPageSimple_SetPrev,(self,) + _args, _kwargs)
        return val
    def SetNext(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizardPageSimple_SetNext,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxWizardPageSimple instance at %s>" % (self.this,)
class wxWizardPageSimple(wxWizardPageSimplePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(wizardc.new_wxWizardPageSimple,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreWizardPageSimple(*_args,**_kwargs):
    val = wxWizardPageSimplePtr(apply(wizardc.new_wxPreWizardPageSimple,_args,_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxWizardPtr(wxDialogPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_Create,(self,) + _args, _kwargs)
        return val
    def Init(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_Init,(self,) + _args, _kwargs)
        return val
    def RunWizard(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_RunWizard,(self,) + _args, _kwargs)
        return val
    def GetCurrentPage(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_GetCurrentPage,(self,) + _args, _kwargs)
        return val
    def SetPageSize(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_SetPageSize,(self,) + _args, _kwargs)
        return val
    def GetPageSize(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_GetPageSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def FitToPage(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_FitToPage,(self,) + _args, _kwargs)
        return val
    def IsRunning(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_IsRunning,(self,) + _args, _kwargs)
        return val
    def ShowPage(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_ShowPage,(self,) + _args, _kwargs)
        return val
    def HasNextPage(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_HasNextPage,(self,) + _args, _kwargs)
        return val
    def HasPrevPage(self, *_args, **_kwargs):
        val = apply(wizardc.wxWizard_HasPrevPage,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxWizard instance at %s>" % (self.this,)
class wxWizard(wxWizardPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(wizardc.new_wxWizard,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreWizard(*_args,**_kwargs):
    val = wxWizardPtr(apply(wizardc.new_wxPreWizard,_args,_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val




#-------------- FUNCTION WRAPPERS ------------------

wxWizardPageSimple_Chain = wizardc.wxWizardPageSimple_Chain



#-------------- VARIABLE WRAPPERS ------------------

wxWIZARD_EX_HELPBUTTON = wizardc.wxWIZARD_EX_HELPBUTTON
wxEVT_WIZARD_PAGE_CHANGED = wizardc.wxEVT_WIZARD_PAGE_CHANGED
wxEVT_WIZARD_PAGE_CHANGING = wizardc.wxEVT_WIZARD_PAGE_CHANGING
wxEVT_WIZARD_CANCEL = wizardc.wxEVT_WIZARD_CANCEL
wxEVT_WIZARD_HELP = wizardc.wxEVT_WIZARD_HELP
wxEVT_WIZARD_FINISHED = wizardc.wxEVT_WIZARD_FINISHED


#-------------- USER INCLUDE -----------------------

# Stuff these names into the wx namespace so wxPyConstructObject can find them
import wx
wx.wxWizardEventPtr         = wxWizardEventPtr
wx.wxWizardPagePtr          = wxWizardPagePtr
wx.wxPyWizardPagePtr        = wxPyWizardPagePtr
wx.wxWizardPageSimplePtr    = wxWizardPageSimplePtr
wx.wxWizardPtr              = wxWizardPtr
