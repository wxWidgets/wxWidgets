# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

"""
wx.webkit.WebKitCtrl for Mac OSX.
"""

import _webkit

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


import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
class WebKitCtrl(_core.Control):
    """Proxy of C++ WebKitCtrl class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWebKitCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int winID=-1, String strURL=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=WebKitNameStr) -> WebKitCtrl
        """
        newobj = _webkit.new_WebKitCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int winID=-1, String strURL=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=WebKitNameStr) -> bool
        """
        return _webkit.WebKitCtrl_Create(*args, **kwargs)

    def LoadURL(*args, **kwargs):
        """LoadURL(self, String url)"""
        return _webkit.WebKitCtrl_LoadURL(*args, **kwargs)

    def CanGoBack(*args, **kwargs):
        """CanGoBack(self) -> bool"""
        return _webkit.WebKitCtrl_CanGoBack(*args, **kwargs)

    def CanGoForward(*args, **kwargs):
        """CanGoForward(self) -> bool"""
        return _webkit.WebKitCtrl_CanGoForward(*args, **kwargs)

    def GoBack(*args, **kwargs):
        """GoBack(self) -> bool"""
        return _webkit.WebKitCtrl_GoBack(*args, **kwargs)

    def GoForward(*args, **kwargs):
        """GoForward(self) -> bool"""
        return _webkit.WebKitCtrl_GoForward(*args, **kwargs)

    def Reload(*args, **kwargs):
        """Reload(self)"""
        return _webkit.WebKitCtrl_Reload(*args, **kwargs)

    def Stop(*args, **kwargs):
        """Stop(self)"""
        return _webkit.WebKitCtrl_Stop(*args, **kwargs)

    def CanGetPageSource(*args, **kwargs):
        """CanGetPageSource(self) -> bool"""
        return _webkit.WebKitCtrl_CanGetPageSource(*args, **kwargs)

    def GetPageSource(*args, **kwargs):
        """GetPageSource(self) -> String"""
        return _webkit.WebKitCtrl_GetPageSource(*args, **kwargs)

    def SetPageSource(*args, **kwargs):
        """SetPageSource(self, String source, String baseUrl=EmptyString)"""
        return _webkit.WebKitCtrl_SetPageSource(*args, **kwargs)


class WebKitCtrlPtr(WebKitCtrl):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WebKitCtrl
_webkit.WebKitCtrl_swigregister(WebKitCtrlPtr)
cvar = _webkit.cvar
WebKitNameStr = cvar.WebKitNameStr

def PreWebKitCtrl(*args, **kwargs):
    """PreWebKitCtrl() -> WebKitCtrl"""
    val = _webkit.new_PreWebKitCtrl(*args, **kwargs)
    val.thisown = 1
    return val

WEBKIT_STATE_START = _webkit.WEBKIT_STATE_START
WEBKIT_STATE_NEGOTIATING = _webkit.WEBKIT_STATE_NEGOTIATING
WEBKIT_STATE_REDIRECTING = _webkit.WEBKIT_STATE_REDIRECTING
WEBKIT_STATE_TRANSFERRING = _webkit.WEBKIT_STATE_TRANSFERRING
WEBKIT_STATE_STOP = _webkit.WEBKIT_STATE_STOP
WEBKIT_STATE_FAILED = _webkit.WEBKIT_STATE_FAILED
wxEVT_WEBKIT_STATE_CHANGED = _webkit.wxEVT_WEBKIT_STATE_CHANGED
class WebKitStateChangedEvent(_core.CommandEvent):
    """Proxy of C++ WebKitStateChangedEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWebKitStateChangedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Window win=None) -> WebKitStateChangedEvent"""
        newobj = _webkit.new_WebKitStateChangedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetState(*args, **kwargs):
        """GetState(self) -> int"""
        return _webkit.WebKitStateChangedEvent_GetState(*args, **kwargs)

    def SetState(*args, **kwargs):
        """SetState(self, int state)"""
        return _webkit.WebKitStateChangedEvent_SetState(*args, **kwargs)

    def GetURL(*args, **kwargs):
        """GetURL(self) -> String"""
        return _webkit.WebKitStateChangedEvent_GetURL(*args, **kwargs)

    def SetURL(*args, **kwargs):
        """SetURL(self, String url)"""
        return _webkit.WebKitStateChangedEvent_SetURL(*args, **kwargs)


class WebKitStateChangedEventPtr(WebKitStateChangedEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WebKitStateChangedEvent
_webkit.WebKitStateChangedEvent_swigregister(WebKitStateChangedEventPtr)

EVT_WEBKIT_STATE_CHANGED = wx.PyEventBinder(wxEVT_WEBKIT_STATE_CHANGED)


