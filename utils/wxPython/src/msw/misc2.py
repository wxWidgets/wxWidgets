# This file was created automatically by SWIG.
import misc2c

from windows import *

from misc import *

from gdi import *
class wxToolTipPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetTip(self, *_args, **_kwargs):
        val = apply(misc2c.wxToolTip_SetTip,(self,) + _args, _kwargs)
        return val
    def GetTip(self, *_args, **_kwargs):
        val = apply(misc2c.wxToolTip_GetTip,(self,) + _args, _kwargs)
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = apply(misc2c.wxToolTip_GetWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def __repr__(self):
        return "<C wxToolTip instance at %s>" % (self.this,)
class wxToolTip(wxToolTipPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxToolTip,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxFindWindowByLabel(*_args, **_kwargs):
    val = apply(misc2c.wxFindWindowByLabel,_args,_kwargs)
    if val: val = wxWindowPtr(val)
    return val

def wxFindWindowByName(*_args, **_kwargs):
    val = apply(misc2c.wxFindWindowByName,_args,_kwargs)
    if val: val = wxWindowPtr(val)
    return val

wxToolTip_Enable = misc2c.wxToolTip_Enable

wxToolTip_SetDelay = misc2c.wxToolTip_SetDelay



#-------------- VARIABLE WRAPPERS ------------------

