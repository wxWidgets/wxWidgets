# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _wizard

import windows
import core
wx = core 
WIZARD_EX_HELPBUTTON = _wizard.WIZARD_EX_HELPBUTTON
wxEVT_WIZARD_PAGE_CHANGED = _wizard.wxEVT_WIZARD_PAGE_CHANGED
wxEVT_WIZARD_PAGE_CHANGING = _wizard.wxEVT_WIZARD_PAGE_CHANGING
wxEVT_WIZARD_CANCEL = _wizard.wxEVT_WIZARD_CANCEL
wxEVT_WIZARD_HELP = _wizard.wxEVT_WIZARD_HELP
wxEVT_WIZARD_FINISHED = _wizard.wxEVT_WIZARD_FINISHED
def EVT_WIZARD_PAGE_CHANGED = wx.PyEventBinder( wxEVT_WIZARD_PAGE_CHANGED, 1)
def EVT_WIZARD_PAGE_CHANGING = wx.PyEventBinder( wxEVT_WIZARD_PAGE_CHANGING, 1)
def EVT_WIZARD_CANCEL = wx.PyEventBinder( wxEVT_WIZARD_CANCEL, 1)
def EVT_WIZARD_HELP = wx.PyEventBinder( wxEVT_WIZARD_HELP, 1)
def EVT_WIZARD_FINISHED = wx.PyEventBinder( wxEVT_WIZARD_FINISHED, 1)

class WizardEvent(core.NotifyEvent):
    def __init__(self, *args, **kwargs):
        newobj = _wizard.new_WizardEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetDirection(*args, **kwargs): return _wizard.WizardEvent_GetDirection(*args, **kwargs)
    def GetPage(*args, **kwargs): return _wizard.WizardEvent_GetPage(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWizardEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class WizardEventPtr(WizardEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WizardEvent
_wizard.WizardEvent_swigregister(WizardEventPtr)

class WizardPage(windows.Panel):
    def Create(*args, **kwargs): return _wizard.WizardPage_Create(*args, **kwargs)
    def GetPrev(*args, **kwargs): return _wizard.WizardPage_GetPrev(*args, **kwargs)
    def GetNext(*args, **kwargs): return _wizard.WizardPage_GetNext(*args, **kwargs)
    def GetBitmap(*args, **kwargs): return _wizard.WizardPage_GetBitmap(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWizardPage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class WizardPagePtr(WizardPage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WizardPage
_wizard.WizardPage_swigregister(WizardPagePtr)

class PyWizardPage(WizardPage):
    def __init__(self, *args, **kwargs):
        newobj = _wizard.new_PyWizardPage(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyWizardPage);self._setOORInfo(self)
    def Create(*args, **kwargs): return _wizard.PyWizardPage_Create(*args, **kwargs)
    def _setCallbackInfo(*args, **kwargs): return _wizard.PyWizardPage__setCallbackInfo(*args, **kwargs)
    def base_DoMoveWindow(*args, **kwargs): return _wizard.PyWizardPage_base_DoMoveWindow(*args, **kwargs)
    def base_DoSetSize(*args, **kwargs): return _wizard.PyWizardPage_base_DoSetSize(*args, **kwargs)
    def base_DoSetClientSize(*args, **kwargs): return _wizard.PyWizardPage_base_DoSetClientSize(*args, **kwargs)
    def base_DoSetVirtualSize(*args, **kwargs): return _wizard.PyWizardPage_base_DoSetVirtualSize(*args, **kwargs)
    def base_DoGetSize(*args, **kwargs): return _wizard.PyWizardPage_base_DoGetSize(*args, **kwargs)
    def base_DoGetClientSize(*args, **kwargs): return _wizard.PyWizardPage_base_DoGetClientSize(*args, **kwargs)
    def base_DoGetPosition(*args, **kwargs): return _wizard.PyWizardPage_base_DoGetPosition(*args, **kwargs)
    def base_DoGetVirtualSize(*args, **kwargs): return _wizard.PyWizardPage_base_DoGetVirtualSize(*args, **kwargs)
    def base_DoGetBestSize(*args, **kwargs): return _wizard.PyWizardPage_base_DoGetBestSize(*args, **kwargs)
    def base_InitDialog(*args, **kwargs): return _wizard.PyWizardPage_base_InitDialog(*args, **kwargs)
    def base_TransferDataToWindow(*args, **kwargs): return _wizard.PyWizardPage_base_TransferDataToWindow(*args, **kwargs)
    def base_TransferDataFromWindow(*args, **kwargs): return _wizard.PyWizardPage_base_TransferDataFromWindow(*args, **kwargs)
    def base_Validate(*args, **kwargs): return _wizard.PyWizardPage_base_Validate(*args, **kwargs)
    def base_AcceptsFocus(*args, **kwargs): return _wizard.PyWizardPage_base_AcceptsFocus(*args, **kwargs)
    def base_AcceptsFocusFromKeyboard(*args, **kwargs): return _wizard.PyWizardPage_base_AcceptsFocusFromKeyboard(*args, **kwargs)
    def base_GetMaxSize(*args, **kwargs): return _wizard.PyWizardPage_base_GetMaxSize(*args, **kwargs)
    def base_AddChild(*args, **kwargs): return _wizard.PyWizardPage_base_AddChild(*args, **kwargs)
    def base_RemoveChild(*args, **kwargs): return _wizard.PyWizardPage_base_RemoveChild(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyWizardPage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PyWizardPagePtr(PyWizardPage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyWizardPage
_wizard.PyWizardPage_swigregister(PyWizardPagePtr)

def PrePyWizardPage(*args, **kwargs):
    val = _wizard.new_PrePyWizardPage(*args, **kwargs)
    val.thisown = 1
    val._setOORInfo(val)
    return val

class WizardPageSimple(WizardPage):
    def __init__(self, *args, **kwargs):
        newobj = _wizard.new_WizardPageSimple(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _wizard.WizardPageSimple_Create(*args, **kwargs)
    def SetPrev(*args, **kwargs): return _wizard.WizardPageSimple_SetPrev(*args, **kwargs)
    def SetNext(*args, **kwargs): return _wizard.WizardPageSimple_SetNext(*args, **kwargs)
    Chain = staticmethod(_wizard.WizardPageSimple_Chain)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWizardPageSimple instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class WizardPageSimplePtr(WizardPageSimple):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WizardPageSimple
_wizard.WizardPageSimple_swigregister(WizardPageSimplePtr)

def PreWizardPageSimple(*args, **kwargs):
    val = _wizard.new_PreWizardPageSimple(*args, **kwargs)
    val.thisown = 1
    val._setOORInfo(val)
    return val

WizardPageSimple_Chain = _wizard.WizardPageSimple_Chain

class Wizard(windows.Dialog):
    def __init__(self, *args, **kwargs):
        newobj = _wizard.new_Wizard(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Create(*args, **kwargs): return _wizard.Wizard_Create(*args, **kwargs)
    def Init(*args, **kwargs): return _wizard.Wizard_Init(*args, **kwargs)
    def RunWizard(*args, **kwargs): return _wizard.Wizard_RunWizard(*args, **kwargs)
    def GetCurrentPage(*args, **kwargs): return _wizard.Wizard_GetCurrentPage(*args, **kwargs)
    def SetPageSize(*args, **kwargs): return _wizard.Wizard_SetPageSize(*args, **kwargs)
    def GetPageSize(*args, **kwargs): return _wizard.Wizard_GetPageSize(*args, **kwargs)
    def FitToPage(*args, **kwargs): return _wizard.Wizard_FitToPage(*args, **kwargs)
    def GetPageAreaSizer(*args, **kwargs): return _wizard.Wizard_GetPageAreaSizer(*args, **kwargs)
    def SetBorder(*args, **kwargs): return _wizard.Wizard_SetBorder(*args, **kwargs)
    def IsRunning(*args, **kwargs): return _wizard.Wizard_IsRunning(*args, **kwargs)
    def ShowPage(*args, **kwargs): return _wizard.Wizard_ShowPage(*args, **kwargs)
    def HasNextPage(*args, **kwargs): return _wizard.Wizard_HasNextPage(*args, **kwargs)
    def HasPrevPage(*args, **kwargs): return _wizard.Wizard_HasPrevPage(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWizard instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class WizardPtr(Wizard):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Wizard
_wizard.Wizard_swigregister(WizardPtr)

def PreWizard(*args, **kwargs):
    val = _wizard.new_PreWizard(*args, **kwargs)
    val.thisown = 1
    val._setOORInfo(val)
    return val


