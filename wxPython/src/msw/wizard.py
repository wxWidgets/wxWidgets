# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

"""
`Wizard` is a dialog class that guides the user through a sequence of steps,
or pages.
"""

import _wizard

import _windows
import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
WIZARD_EX_HELPBUTTON = _wizard.WIZARD_EX_HELPBUTTON
wxEVT_WIZARD_PAGE_CHANGED = _wizard.wxEVT_WIZARD_PAGE_CHANGED
wxEVT_WIZARD_PAGE_CHANGING = _wizard.wxEVT_WIZARD_PAGE_CHANGING
wxEVT_WIZARD_CANCEL = _wizard.wxEVT_WIZARD_CANCEL
wxEVT_WIZARD_HELP = _wizard.wxEVT_WIZARD_HELP
wxEVT_WIZARD_FINISHED = _wizard.wxEVT_WIZARD_FINISHED
EVT_WIZARD_PAGE_CHANGED  = wx.PyEventBinder( wxEVT_WIZARD_PAGE_CHANGED, 1)
EVT_WIZARD_PAGE_CHANGING = wx.PyEventBinder( wxEVT_WIZARD_PAGE_CHANGING, 1)
EVT_WIZARD_CANCEL        = wx.PyEventBinder( wxEVT_WIZARD_CANCEL, 1)
EVT_WIZARD_HELP          = wx.PyEventBinder( wxEVT_WIZARD_HELP, 1)
EVT_WIZARD_FINISHED      = wx.PyEventBinder( wxEVT_WIZARD_FINISHED, 1)

class WizardEvent(_core.NotifyEvent):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWizardEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxEventType type=wxEVT_NULL, int id=-1, bool direction=True, 
            WizardPage page=None) -> WizardEvent
        """
        newobj = _wizard.new_WizardEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetDirection(*args, **kwargs):
        """GetDirection(self) -> bool"""
        return _wizard.WizardEvent_GetDirection(*args, **kwargs)

    def GetPage(*args, **kwargs):
        """GetPage(self) -> WizardPage"""
        return _wizard.WizardEvent_GetPage(*args, **kwargs)


class WizardEventPtr(WizardEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WizardEvent
_wizard.WizardEvent_swigregister(WizardEventPtr)

class WizardPage(_windows.Panel):
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWizardPage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def Create(*args, **kwargs):
        """Create(self, Wizard parent, Bitmap bitmap=wxNullBitmap, String resource=EmptyString) -> bool"""
        return _wizard.WizardPage_Create(*args, **kwargs)

    def GetPrev(*args, **kwargs):
        """GetPrev(self) -> WizardPage"""
        return _wizard.WizardPage_GetPrev(*args, **kwargs)

    def GetNext(*args, **kwargs):
        """GetNext(self) -> WizardPage"""
        return _wizard.WizardPage_GetNext(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap(self) -> Bitmap"""
        return _wizard.WizardPage_GetBitmap(*args, **kwargs)


class WizardPagePtr(WizardPage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WizardPage
_wizard.WizardPage_swigregister(WizardPagePtr)

class PyWizardPage(WizardPage):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyWizardPage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Wizard parent, Bitmap bitmap=&wxNullBitmap, String resource=&wxPyEmptyString) -> PyWizardPage"""
        newobj = _wizard.new_PyWizardPage(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyWizardPage);self._setOORInfo(self)

    def Create(*args, **kwargs):
        """Create(self, Wizard parent, Bitmap bitmap=wxNullBitmap, String resource=EmptyString) -> bool"""
        return _wizard.PyWizardPage_Create(*args, **kwargs)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _wizard.PyWizardPage__setCallbackInfo(*args, **kwargs)

    def base_DoMoveWindow(*args, **kwargs):
        """base_DoMoveWindow(self, int x, int y, int width, int height)"""
        return _wizard.PyWizardPage_base_DoMoveWindow(*args, **kwargs)

    def base_DoSetSize(*args, **kwargs):
        """base_DoSetSize(self, int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)"""
        return _wizard.PyWizardPage_base_DoSetSize(*args, **kwargs)

    def base_DoSetClientSize(*args, **kwargs):
        """base_DoSetClientSize(self, int width, int height)"""
        return _wizard.PyWizardPage_base_DoSetClientSize(*args, **kwargs)

    def base_DoSetVirtualSize(*args, **kwargs):
        """base_DoSetVirtualSize(self, int x, int y)"""
        return _wizard.PyWizardPage_base_DoSetVirtualSize(*args, **kwargs)

    def base_DoGetSize(*args, **kwargs):
        """base_DoGetSize() -> (width, height)"""
        return _wizard.PyWizardPage_base_DoGetSize(*args, **kwargs)

    def base_DoGetClientSize(*args, **kwargs):
        """base_DoGetClientSize() -> (width, height)"""
        return _wizard.PyWizardPage_base_DoGetClientSize(*args, **kwargs)

    def base_DoGetPosition(*args, **kwargs):
        """base_DoGetPosition() -> (x,y)"""
        return _wizard.PyWizardPage_base_DoGetPosition(*args, **kwargs)

    def base_DoGetVirtualSize(*args, **kwargs):
        """base_DoGetVirtualSize(self) -> Size"""
        return _wizard.PyWizardPage_base_DoGetVirtualSize(*args, **kwargs)

    def base_DoGetBestSize(*args, **kwargs):
        """base_DoGetBestSize(self) -> Size"""
        return _wizard.PyWizardPage_base_DoGetBestSize(*args, **kwargs)

    def base_InitDialog(*args, **kwargs):
        """base_InitDialog(self)"""
        return _wizard.PyWizardPage_base_InitDialog(*args, **kwargs)

    def base_TransferDataToWindow(*args, **kwargs):
        """base_TransferDataToWindow(self) -> bool"""
        return _wizard.PyWizardPage_base_TransferDataToWindow(*args, **kwargs)

    def base_TransferDataFromWindow(*args, **kwargs):
        """base_TransferDataFromWindow(self) -> bool"""
        return _wizard.PyWizardPage_base_TransferDataFromWindow(*args, **kwargs)

    def base_Validate(*args, **kwargs):
        """base_Validate(self) -> bool"""
        return _wizard.PyWizardPage_base_Validate(*args, **kwargs)

    def base_AcceptsFocus(*args, **kwargs):
        """base_AcceptsFocus(self) -> bool"""
        return _wizard.PyWizardPage_base_AcceptsFocus(*args, **kwargs)

    def base_AcceptsFocusFromKeyboard(*args, **kwargs):
        """base_AcceptsFocusFromKeyboard(self) -> bool"""
        return _wizard.PyWizardPage_base_AcceptsFocusFromKeyboard(*args, **kwargs)

    def base_GetMaxSize(*args, **kwargs):
        """base_GetMaxSize(self) -> Size"""
        return _wizard.PyWizardPage_base_GetMaxSize(*args, **kwargs)

    def base_AddChild(*args, **kwargs):
        """base_AddChild(self, Window child)"""
        return _wizard.PyWizardPage_base_AddChild(*args, **kwargs)

    def base_RemoveChild(*args, **kwargs):
        """base_RemoveChild(self, Window child)"""
        return _wizard.PyWizardPage_base_RemoveChild(*args, **kwargs)


class PyWizardPagePtr(PyWizardPage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyWizardPage
_wizard.PyWizardPage_swigregister(PyWizardPagePtr)

def PrePyWizardPage(*args, **kwargs):
    """PrePyWizardPage() -> PyWizardPage"""
    val = _wizard.new_PrePyWizardPage(*args, **kwargs)
    val.thisown = 1
    return val

class WizardPageSimple(WizardPage):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWizardPageSimple instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Wizard parent, WizardPage prev=None, WizardPage next=None, 
            Bitmap bitmap=wxNullBitmap, wxChar resource=None) -> WizardPageSimple
        """
        newobj = _wizard.new_WizardPageSimple(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Wizard parent=None, WizardPage prev=None, WizardPage next=None, 
            Bitmap bitmap=wxNullBitmap, wxChar resource=None) -> bool
        """
        return _wizard.WizardPageSimple_Create(*args, **kwargs)

    def SetPrev(*args, **kwargs):
        """SetPrev(self, WizardPage prev)"""
        return _wizard.WizardPageSimple_SetPrev(*args, **kwargs)

    def SetNext(*args, **kwargs):
        """SetNext(self, WizardPage next)"""
        return _wizard.WizardPageSimple_SetNext(*args, **kwargs)

    def Chain(*args, **kwargs):
        """Chain(WizardPageSimple first, WizardPageSimple second)"""
        return _wizard.WizardPageSimple_Chain(*args, **kwargs)

    Chain = staticmethod(Chain)

class WizardPageSimplePtr(WizardPageSimple):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WizardPageSimple
_wizard.WizardPageSimple_swigregister(WizardPageSimplePtr)

def PreWizardPageSimple(*args, **kwargs):
    """PreWizardPageSimple() -> WizardPageSimple"""
    val = _wizard.new_PreWizardPageSimple(*args, **kwargs)
    val.thisown = 1
    return val

def WizardPageSimple_Chain(*args, **kwargs):
    """WizardPageSimple_Chain(WizardPageSimple first, WizardPageSimple second)"""
    return _wizard.WizardPageSimple_Chain(*args, **kwargs)

class Wizard(_windows.Dialog):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWizard instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, String title=EmptyString, 
            Bitmap bitmap=wxNullBitmap, Point pos=DefaultPosition, 
            long style=DEFAULT_DIALOG_STYLE) -> Wizard
        """
        newobj = _wizard.new_Wizard(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, String title=EmptyString, 
            Bitmap bitmap=wxNullBitmap, Point pos=DefaultPosition) -> bool
        """
        return _wizard.Wizard_Create(*args, **kwargs)

    def Init(*args, **kwargs):
        """Init(self)"""
        return _wizard.Wizard_Init(*args, **kwargs)

    def RunWizard(*args, **kwargs):
        """RunWizard(self, WizardPage firstPage) -> bool"""
        return _wizard.Wizard_RunWizard(*args, **kwargs)

    def GetCurrentPage(*args, **kwargs):
        """GetCurrentPage(self) -> WizardPage"""
        return _wizard.Wizard_GetCurrentPage(*args, **kwargs)

    def SetPageSize(*args, **kwargs):
        """SetPageSize(self, Size size)"""
        return _wizard.Wizard_SetPageSize(*args, **kwargs)

    def GetPageSize(*args, **kwargs):
        """GetPageSize(self) -> Size"""
        return _wizard.Wizard_GetPageSize(*args, **kwargs)

    def FitToPage(*args, **kwargs):
        """FitToPage(self, WizardPage firstPage)"""
        return _wizard.Wizard_FitToPage(*args, **kwargs)

    def GetPageAreaSizer(*args, **kwargs):
        """GetPageAreaSizer(self) -> Sizer"""
        return _wizard.Wizard_GetPageAreaSizer(*args, **kwargs)

    def SetBorder(*args, **kwargs):
        """SetBorder(self, int border)"""
        return _wizard.Wizard_SetBorder(*args, **kwargs)

    def IsRunning(*args, **kwargs):
        """IsRunning(self) -> bool"""
        return _wizard.Wizard_IsRunning(*args, **kwargs)

    def ShowPage(*args, **kwargs):
        """ShowPage(self, WizardPage page, bool goingForward=True) -> bool"""
        return _wizard.Wizard_ShowPage(*args, **kwargs)

    def HasNextPage(*args, **kwargs):
        """HasNextPage(self, WizardPage page) -> bool"""
        return _wizard.Wizard_HasNextPage(*args, **kwargs)

    def HasPrevPage(*args, **kwargs):
        """HasPrevPage(self, WizardPage page) -> bool"""
        return _wizard.Wizard_HasPrevPage(*args, **kwargs)


class WizardPtr(Wizard):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Wizard
_wizard.Wizard_swigregister(WizardPtr)

def PreWizard(*args, **kwargs):
    """PreWizard() -> Wizard"""
    val = _wizard.new_PreWizard(*args, **kwargs)
    val.thisown = 1
    return val


