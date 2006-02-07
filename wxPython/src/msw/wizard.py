# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.

"""
`Wizard` is a dialog class that guides the user through a sequence of steps,
or pages.
"""

import _wizard

def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if hasattr(self,name) or (name in ("this", "thisown")):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


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
    """Proxy of C++ WizardEvent class"""
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
    """Proxy of C++ WizardPage class"""
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
    """Proxy of C++ PyWizardPage class"""
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

    def DoMoveWindow(*args, **kwargs):
        """DoMoveWindow(self, int x, int y, int width, int height)"""
        return _wizard.PyWizardPage_DoMoveWindow(*args, **kwargs)

    def DoSetSize(*args, **kwargs):
        """DoSetSize(self, int x, int y, int width, int height, int sizeFlags=SIZE_AUTO)"""
        return _wizard.PyWizardPage_DoSetSize(*args, **kwargs)

    def DoSetClientSize(*args, **kwargs):
        """DoSetClientSize(self, int width, int height)"""
        return _wizard.PyWizardPage_DoSetClientSize(*args, **kwargs)

    def DoSetVirtualSize(*args, **kwargs):
        """DoSetVirtualSize(self, int x, int y)"""
        return _wizard.PyWizardPage_DoSetVirtualSize(*args, **kwargs)

    def DoGetSize(*args, **kwargs):
        """DoGetSize() -> (width, height)"""
        return _wizard.PyWizardPage_DoGetSize(*args, **kwargs)

    def DoGetClientSize(*args, **kwargs):
        """DoGetClientSize() -> (width, height)"""
        return _wizard.PyWizardPage_DoGetClientSize(*args, **kwargs)

    def DoGetPosition(*args, **kwargs):
        """DoGetPosition() -> (x,y)"""
        return _wizard.PyWizardPage_DoGetPosition(*args, **kwargs)

    def DoGetVirtualSize(*args, **kwargs):
        """DoGetVirtualSize(self) -> Size"""
        return _wizard.PyWizardPage_DoGetVirtualSize(*args, **kwargs)

    def DoGetBestSize(*args, **kwargs):
        """DoGetBestSize(self) -> Size"""
        return _wizard.PyWizardPage_DoGetBestSize(*args, **kwargs)

    def InitDialog(*args, **kwargs):
        """
        InitDialog(self)

        Sends an EVT_INIT_DIALOG event, whose handler usually transfers data
        to the dialog via validators.
        """
        return _wizard.PyWizardPage_InitDialog(*args, **kwargs)

    def TransferDataToWindow(*args, **kwargs):
        """
        TransferDataToWindow(self) -> bool

        Transfers values to child controls from data areas specified by their
        validators.  If the window has wx.WS_EX_VALIDATE_RECURSIVELY extra
        style flag set, the method will also call TransferDataToWindow() of
        all child windows.
        """
        return _wizard.PyWizardPage_TransferDataToWindow(*args, **kwargs)

    def TransferDataFromWindow(*args, **kwargs):
        """
        TransferDataFromWindow(self) -> bool

        Transfers values from child controls to data areas specified by their
        validators. Returns false if a transfer failed.  If the window has
        wx.WS_EX_VALIDATE_RECURSIVELY extra style flag set, the method will
        also call TransferDataFromWindow() of all child windows.
        """
        return _wizard.PyWizardPage_TransferDataFromWindow(*args, **kwargs)

    def Validate(*args, **kwargs):
        """
        Validate(self) -> bool

        Validates the current values of the child controls using their
        validators.  If the window has wx.WS_EX_VALIDATE_RECURSIVELY extra
        style flag set, the method will also call Validate() of all child
        windows.  Returns false if any of the validations failed.
        """
        return _wizard.PyWizardPage_Validate(*args, **kwargs)

    def AcceptsFocus(*args, **kwargs):
        """
        AcceptsFocus(self) -> bool

        Can this window have focus?
        """
        return _wizard.PyWizardPage_AcceptsFocus(*args, **kwargs)

    def AcceptsFocusFromKeyboard(*args, **kwargs):
        """
        AcceptsFocusFromKeyboard(self) -> bool

        Can this window be given focus by keyboard navigation? if not, the
        only way to give it focus (provided it accepts it at all) is to click
        it.
        """
        return _wizard.PyWizardPage_AcceptsFocusFromKeyboard(*args, **kwargs)

    def GetMaxSize(*args, **kwargs):
        """GetMaxSize(self) -> Size"""
        return _wizard.PyWizardPage_GetMaxSize(*args, **kwargs)

    def AddChild(*args, **kwargs):
        """
        AddChild(self, Window child)

        Adds a child window. This is called automatically by window creation
        functions so should not be required by the application programmer.
        """
        return _wizard.PyWizardPage_AddChild(*args, **kwargs)

    def RemoveChild(*args, **kwargs):
        """
        RemoveChild(self, Window child)

        Removes a child window. This is called automatically by window
        deletion functions so should not be required by the application
        programmer.
        """
        return _wizard.PyWizardPage_RemoveChild(*args, **kwargs)

    def ShouldInheritColours(*args, **kwargs):
        """
        ShouldInheritColours(self) -> bool

        Return true from here to allow the colours of this window to be
        changed by InheritAttributes, returning false forbids inheriting them
        from the parent window.

        The base class version returns false, but this method is overridden in
        wxControl where it returns true.
        """
        return _wizard.PyWizardPage_ShouldInheritColours(*args, **kwargs)

    def GetDefaultAttributes(*args, **kwargs):
        """GetDefaultAttributes(self) -> VisualAttributes"""
        return _wizard.PyWizardPage_GetDefaultAttributes(*args, **kwargs)

    def OnInternalIdle(*args, **kwargs):
        """OnInternalIdle(self)"""
        return _wizard.PyWizardPage_OnInternalIdle(*args, **kwargs)

    def base_DoMoveWindow(*args, **kw):
        return PyWizardPage.DoMoveWindow(*args, **kw)
    base_DoMoveWindow = wx._deprecated(base_DoMoveWindow,
                                   "Please use PyWizardPage.DoMoveWindow instead.")

    def base_DoSetSize(*args, **kw):
        return PyWizardPage.DoSetSize(*args, **kw)
    base_DoSetSize = wx._deprecated(base_DoSetSize,
                                   "Please use PyWizardPage.DoSetSize instead.")

    def base_DoSetClientSize(*args, **kw):
        return PyWizardPage.DoSetClientSize(*args, **kw)
    base_DoSetClientSize = wx._deprecated(base_DoSetClientSize,
                                   "Please use PyWizardPage.DoSetClientSize instead.")

    def base_DoSetVirtualSize(*args, **kw):
        return PyWizardPage.DoSetVirtualSize(*args, **kw)
    base_DoSetVirtualSize = wx._deprecated(base_DoSetVirtualSize,
                                   "Please use PyWizardPage.DoSetVirtualSize instead.")

    def base_DoGetSize(*args, **kw):
        return PyWizardPage.DoGetSize(*args, **kw)
    base_DoGetSize = wx._deprecated(base_DoGetSize,
                                   "Please use PyWizardPage.DoGetSize instead.")

    def base_DoGetClientSize(*args, **kw):
        return PyWizardPage.DoGetClientSize(*args, **kw)
    base_DoGetClientSize = wx._deprecated(base_DoGetClientSize,
                                   "Please use PyWizardPage.DoGetClientSize instead.")

    def base_DoGetPosition(*args, **kw):
        return PyWizardPage.DoGetPosition(*args, **kw)
    base_DoGetPosition = wx._deprecated(base_DoGetPosition,
                                   "Please use PyWizardPage.DoGetPosition instead.")

    def base_DoGetVirtualSize(*args, **kw):
        return PyWizardPage.DoGetVirtualSize(*args, **kw)
    base_DoGetVirtualSize = wx._deprecated(base_DoGetVirtualSize,
                                   "Please use PyWizardPage.DoGetVirtualSize instead.")

    def base_DoGetBestSize(*args, **kw):
        return PyWizardPage.DoGetBestSize(*args, **kw)
    base_DoGetBestSize = wx._deprecated(base_DoGetBestSize,
                                   "Please use PyWizardPage.DoGetBestSize instead.")

    def base_InitDialog(*args, **kw):
        return PyWizardPage.InitDialog(*args, **kw)
    base_InitDialog = wx._deprecated(base_InitDialog,
                                   "Please use PyWizardPage.InitDialog instead.")

    def base_TransferDataToWindow(*args, **kw):
        return PyWizardPage.TransferDataToWindow(*args, **kw)
    base_TransferDataToWindow = wx._deprecated(base_TransferDataToWindow,
                                   "Please use PyWizardPage.TransferDataToWindow instead.")

    def base_TransferDataFromWindow(*args, **kw):
        return PyWizardPage.TransferDataFromWindow(*args, **kw)
    base_TransferDataFromWindow = wx._deprecated(base_TransferDataFromWindow,
                                   "Please use PyWizardPage.TransferDataFromWindow instead.")

    def base_Validate(*args, **kw):
        return PyWizardPage.Validate(*args, **kw)
    base_Validate = wx._deprecated(base_Validate,
                                   "Please use PyWizardPage.Validate instead.")

    def base_AcceptsFocus(*args, **kw):
        return PyWizardPage.AcceptsFocus(*args, **kw)
    base_AcceptsFocus = wx._deprecated(base_AcceptsFocus,
                                   "Please use PyWizardPage.AcceptsFocus instead.")

    def base_AcceptsFocusFromKeyboard(*args, **kw):
        return PyWizardPage.AcceptsFocusFromKeyboard(*args, **kw)
    base_AcceptsFocusFromKeyboard = wx._deprecated(base_AcceptsFocusFromKeyboard,
                                   "Please use PyWizardPage.AcceptsFocusFromKeyboard instead.")

    def base_GetMaxSize(*args, **kw):
        return PyWizardPage.GetMaxSize(*args, **kw)
    base_GetMaxSize = wx._deprecated(base_GetMaxSize,
                                   "Please use PyWizardPage.GetMaxSize instead.")

    def base_AddChild(*args, **kw):
        return PyWizardPage.AddChild(*args, **kw)
    base_AddChild = wx._deprecated(base_AddChild,
                                   "Please use PyWizardPage.AddChild instead.")

    def base_RemoveChild(*args, **kw):
        return PyWizardPage.RemoveChild(*args, **kw)
    base_RemoveChild = wx._deprecated(base_RemoveChild,
                                   "Please use PyWizardPage.RemoveChild instead.")

    def base_ShouldInheritColours(*args, **kw):
        return PyWizardPage.ShouldInheritColours(*args, **kw)
    base_ShouldInheritColours = wx._deprecated(base_ShouldInheritColours,
                                   "Please use PyWizardPage.ShouldInheritColours instead.")

    def base_GetDefaultAttributes(*args, **kw):
        return PyWizardPage.GetDefaultAttributes(*args, **kw)
    base_GetDefaultAttributes = wx._deprecated(base_GetDefaultAttributes,
                                   "Please use PyWizardPage.GetDefaultAttributes instead.")

    def base_OnInternalIdle(*args, **kw):
        return PyWizardPage.OnInternalIdle(*args, **kw)
    base_OnInternalIdle = wx._deprecated(base_OnInternalIdle,
                                   "Please use PyWizardPage.OnInternalIdle instead.")


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
    """Proxy of C++ WizardPageSimple class"""
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
    """Proxy of C++ Wizard class"""
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



