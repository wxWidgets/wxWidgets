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




class wxCaretPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,misc2c=misc2c):
        if self.thisown == 1 :
            misc2c.delete_wxCaret(self)
    def IsOk(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_IsOk,(self,) + _args, _kwargs)
        return val
    def IsVisible(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_IsVisible,(self,) + _args, _kwargs)
        return val
    def GetPositionTuple(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_GetPositionTuple,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetSizeTuple(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_GetSizeTuple,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_GetWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def MoveXY(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_MoveXY,(self,) + _args, _kwargs)
        return val
    def Move(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_Move,(self,) + _args, _kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_Show,(self,) + _args, _kwargs)
        return val
    def Hide(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_Hide,(self,) + _args, _kwargs)
        return val
    def OnSetFocus(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_OnSetFocus,(self,) + _args, _kwargs)
        return val
    def OnKillFocus(self, *_args, **_kwargs):
        val = apply(misc2c.wxCaret_OnKillFocus,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCaret instance at %s>" % (self.this,)
class wxCaret(wxCaretPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(misc2c.new_wxCaret,_args,_kwargs)
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

wxCaret_GetBlinkTime = misc2c.wxCaret_GetBlinkTime

wxCaret_SetBlinkTime = misc2c.wxCaret_SetBlinkTime



#-------------- VARIABLE WRAPPERS ------------------

