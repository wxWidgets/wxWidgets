# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _webkit

import wx._core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
class WebKitCtrl(wx._core.Control):
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWebKitCtrl instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int winID, String strURL, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, 
            Validator validator=DefaultValidator, 
            String name=WebKitNameStr) -> WebKitCtrl
        """
        newobj = _webkit.new_WebKitCtrl(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int winID, String strURL, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=0, 
            Validator validator=DefaultValidator, 
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


