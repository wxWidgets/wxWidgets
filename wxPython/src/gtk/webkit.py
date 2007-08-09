# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

"""
wx.webkit.WebKitCtrl for Mac OSX.
"""

import _webkit
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

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
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
class WebKitCtrl(_core.Control):
    """Proxy of C++ WebKitCtrl class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window parent, int winID=-1, String strURL=EmptyString, 
            Point pos=DefaultPosition, Size size=DefaultSize, 
            long style=0, Validator validator=DefaultValidator, 
            String name=WebKitNameStr) -> WebKitCtrl
        """
        _webkit.WebKitCtrl_swiginit(self,_webkit.new_WebKitCtrl(*args, **kwargs))
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

    def GetPageURL(*args, **kwargs):
        """GetPageURL(self) -> String"""
        return _webkit.WebKitCtrl_GetPageURL(*args, **kwargs)

    def GetPageTitle(*args, **kwargs):
        """GetPageTitle(self) -> String"""
        return _webkit.WebKitCtrl_GetPageTitle(*args, **kwargs)

    def GetSelection(*args, **kwargs):
        """GetSelection(self) -> String"""
        return _webkit.WebKitCtrl_GetSelection(*args, **kwargs)

    def CanIncreaseTextSize(*args, **kwargs):
        """CanIncreaseTextSize(self) -> bool"""
        return _webkit.WebKitCtrl_CanIncreaseTextSize(*args, **kwargs)

    def IncreaseTextSize(*args, **kwargs):
        """IncreaseTextSize(self)"""
        return _webkit.WebKitCtrl_IncreaseTextSize(*args, **kwargs)

    def CanDecreaseTextSize(*args, **kwargs):
        """CanDecreaseTextSize(self) -> bool"""
        return _webkit.WebKitCtrl_CanDecreaseTextSize(*args, **kwargs)

    def DecreaseTextSize(*args, **kwargs):
        """DecreaseTextSize(self)"""
        return _webkit.WebKitCtrl_DecreaseTextSize(*args, **kwargs)

    def Print(*args, **kwargs):
        """Print(self, bool showPrompt=False)"""
        return _webkit.WebKitCtrl_Print(*args, **kwargs)

    def MakeEditable(*args, **kwargs):
        """MakeEditable(self, bool enable=True)"""
        return _webkit.WebKitCtrl_MakeEditable(*args, **kwargs)

    def IsEditable(*args, **kwargs):
        """IsEditable(self) -> bool"""
        return _webkit.WebKitCtrl_IsEditable(*args, **kwargs)

    def RunScript(*args, **kwargs):
        """RunScript(self, String javascript) -> String"""
        return _webkit.WebKitCtrl_RunScript(*args, **kwargs)

    def SetScrollPos(*args, **kwargs):
        """SetScrollPos(self, int pos)"""
        return _webkit.WebKitCtrl_SetScrollPos(*args, **kwargs)

    def GetScrollPos(*args, **kwargs):
        """GetScrollPos(self) -> int"""
        return _webkit.WebKitCtrl_GetScrollPos(*args, **kwargs)

    PageSource = property(GetPageSource,SetPageSource,doc="See `GetPageSource` and `SetPageSource`") 
    PageTitle = property(GetPageTitle,doc="See `GetPageTitle`") 
    PageURL = property(GetPageURL,doc="See `GetPageURL`") 
    ScrollPos = property(GetScrollPos,SetScrollPos,doc="See `GetScrollPos and SetScrollPos`") 
    Selection = property(GetSelection,doc="See `GetSelection`") 
_webkit.WebKitCtrl_swigregister(WebKitCtrl)
cvar = _webkit.cvar
WebKitNameStr = cvar.WebKitNameStr

def PreWebKitCtrl(*args, **kwargs):
    """PreWebKitCtrl() -> WebKitCtrl"""
    val = _webkit.new_PreWebKitCtrl(*args, **kwargs)
    return val

WEBKIT_STATE_START = _webkit.WEBKIT_STATE_START
WEBKIT_STATE_NEGOTIATING = _webkit.WEBKIT_STATE_NEGOTIATING
WEBKIT_STATE_REDIRECTING = _webkit.WEBKIT_STATE_REDIRECTING
WEBKIT_STATE_TRANSFERRING = _webkit.WEBKIT_STATE_TRANSFERRING
WEBKIT_STATE_STOP = _webkit.WEBKIT_STATE_STOP
WEBKIT_STATE_FAILED = _webkit.WEBKIT_STATE_FAILED
WEBKIT_NAV_LINK_CLICKED = _webkit.WEBKIT_NAV_LINK_CLICKED
WEBKIT_NAV_BACK_NEXT = _webkit.WEBKIT_NAV_BACK_NEXT
WEBKIT_NAV_FORM_SUBMITTED = _webkit.WEBKIT_NAV_FORM_SUBMITTED
WEBKIT_NAV_RELOAD = _webkit.WEBKIT_NAV_RELOAD
WEBKIT_NAV_FORM_RESUBMITTED = _webkit.WEBKIT_NAV_FORM_RESUBMITTED
WEBKIT_NAV_OTHER = _webkit.WEBKIT_NAV_OTHER
wxEVT_WEBKIT_STATE_CHANGED = _webkit.wxEVT_WEBKIT_STATE_CHANGED
wxEVT_WEBKIT_BEFORE_LOAD = _webkit.wxEVT_WEBKIT_BEFORE_LOAD
class WebKitBeforeLoadEvent(_core.CommandEvent):
    """Proxy of C++ WebKitBeforeLoadEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def IsCancelled(*args, **kwargs):
        """IsCancelled(self) -> bool"""
        return _webkit.WebKitBeforeLoadEvent_IsCancelled(*args, **kwargs)

    def Cancel(*args, **kwargs):
        """Cancel(self, bool cancel=True)"""
        return _webkit.WebKitBeforeLoadEvent_Cancel(*args, **kwargs)

    def GetURL(*args, **kwargs):
        """GetURL(self) -> String"""
        return _webkit.WebKitBeforeLoadEvent_GetURL(*args, **kwargs)

    def SetURL(*args, **kwargs):
        """SetURL(self, String url)"""
        return _webkit.WebKitBeforeLoadEvent_SetURL(*args, **kwargs)

    def SetNavigationType(*args, **kwargs):
        """SetNavigationType(self, int navType)"""
        return _webkit.WebKitBeforeLoadEvent_SetNavigationType(*args, **kwargs)

    def GetNavigationType(*args, **kwargs):
        """GetNavigationType(self) -> int"""
        return _webkit.WebKitBeforeLoadEvent_GetNavigationType(*args, **kwargs)

    def __init__(self, *args, **kwargs): 
        """__init__(self, Window win=(wxWindow *) NULL) -> WebKitBeforeLoadEvent"""
        _webkit.WebKitBeforeLoadEvent_swiginit(self,_webkit.new_WebKitBeforeLoadEvent(*args, **kwargs))
    NavigationType = property(GetNavigationType,SetNavigationType,doc="See `GetNavigationType` and `SetNavigationType`") 
    URL = property(GetURL,SetURL,doc="See `GetURL` and `SetURL`") 
_webkit.WebKitBeforeLoadEvent_swigregister(WebKitBeforeLoadEvent)

class WebKitStateChangedEvent(_core.CommandEvent):
    """Proxy of C++ WebKitStateChangedEvent class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Window win=None) -> WebKitStateChangedEvent"""
        _webkit.WebKitStateChangedEvent_swiginit(self,_webkit.new_WebKitStateChangedEvent(*args, **kwargs))
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

    State = property(GetState,SetState,doc="See `GetState` and `SetState`") 
    URL = property(GetURL,SetURL,doc="See `GetURL` and `SetURL`") 
_webkit.WebKitStateChangedEvent_swigregister(WebKitStateChangedEvent)

EVT_WEBKIT_STATE_CHANGED = wx.PyEventBinder(wxEVT_WEBKIT_STATE_CHANGED)
EVT_WEBKIT_BEFORE_LOAD = wx.PyEventBinder(wxEVT_WEBKIT_BEFORE_LOAD)



