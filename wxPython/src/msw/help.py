# This file was created automatically by SWIG.
import helpc

from windows import *

from misc import *

from gdi import *

from fonts import *

from clip_dnd import *

from controls import *

from events import *

# Help events
def EVT_HELP(win, id, func):
    win.Connect(id, -1, wxEVT_HELP, func)

def EVT_HELP_RANGE(win, id, id2, func):
    win.Connect(id, id2, wxEVT_HELP, func)

def EVT_DETAILED_HELP(win, id, func):
    win.Connect(id, -1, wxEVT_DETAILED_HELP, func)

def EVT_DETAILED_HELP_RANGE(win, id, id2, func):
    win.Connect(id, id2, wxEVT_DETAILED_HELP, func)

class wxHelpEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPosition(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpEvent_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) 
        return val
    def SetPosition(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpEvent_SetPosition,(self,) + _args, _kwargs)
        return val
    def GetLink(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpEvent_GetLink,(self,) + _args, _kwargs)
        return val
    def SetLink(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpEvent_SetLink,(self,) + _args, _kwargs)
        return val
    def GetTarget(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpEvent_GetTarget,(self,) + _args, _kwargs)
        return val
    def SetTarget(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpEvent_SetTarget,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHelpEvent instance at %s>" % (self.this,)
class wxHelpEvent(wxHelpEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(helpc.new_wxHelpEvent,_args,_kwargs)
        self.thisown = 1




class wxContextHelpPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=helpc.delete_wxContextHelp):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def BeginContextHelp(self, *_args, **_kwargs):
        val = apply(helpc.wxContextHelp_BeginContextHelp,(self,) + _args, _kwargs)
        return val
    def EndContextHelp(self, *_args, **_kwargs):
        val = apply(helpc.wxContextHelp_EndContextHelp,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxContextHelp instance at %s>" % (self.this,)
class wxContextHelp(wxContextHelpPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(helpc.new_wxContextHelp,_args,_kwargs)
        self.thisown = 1




class wxContextHelpButtonPtr(wxBitmapButtonPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxContextHelpButton instance at %s>" % (self.this,)
class wxContextHelpButton(wxContextHelpButtonPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(helpc.new_wxContextHelpButton,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxHelpProviderPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetHelp(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpProvider_GetHelp,(self,) + _args, _kwargs)
        return val
    def ShowHelp(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpProvider_ShowHelp,(self,) + _args, _kwargs)
        return val
    def AddHelp(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpProvider_AddHelp,(self,) + _args, _kwargs)
        return val
    def AddHelpById(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpProvider_AddHelpById,(self,) + _args, _kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = apply(helpc.wxHelpProvider_Destroy,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxHelpProvider instance at %s>" % (self.this,)
class wxHelpProvider(wxHelpProviderPtr):
    def __init__(self,this):
        self.this = this




class wxSimpleHelpProviderPtr(wxHelpProviderPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxSimpleHelpProvider instance at %s>" % (self.this,)
class wxSimpleHelpProvider(wxSimpleHelpProviderPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(helpc.new_wxSimpleHelpProvider,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxHelpProvider_Set(*_args, **_kwargs):
    val = apply(helpc.wxHelpProvider_Set,_args,_kwargs)
    if val: val = wxHelpProviderPtr(val)
    return val

def wxHelpProvider_Get(*_args, **_kwargs):
    val = apply(helpc.wxHelpProvider_Get,_args,_kwargs)
    if val: val = wxHelpProviderPtr(val)
    return val



#-------------- VARIABLE WRAPPERS ------------------

wxFRAME_EX_CONTEXTHELP = helpc.wxFRAME_EX_CONTEXTHELP
wxDIALOG_EX_CONTEXTHELP = helpc.wxDIALOG_EX_CONTEXTHELP
wxID_CONTEXT_HELP = helpc.wxID_CONTEXT_HELP
wxEVT_HELP = helpc.wxEVT_HELP
wxEVT_DETAILED_HELP = helpc.wxEVT_DETAILED_HELP


#-------------- USER INCLUDE -----------------------

# Stuff these names into the wx namespace so wxPyConstructObject can find them
import wx
wx.wxHelpEventPtr         = wxHelpEventPtr
wx.wxContextHelpButtonPtr = wxContextHelpButtonPtr
