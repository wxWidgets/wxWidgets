# This file was created automatically by SWIG.
import sizersc

from misc import *

from windows import *

from gdi import *

from clip_dnd import *

from controls import *

from events import *
import wx
import string
class wxSizerItemPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def CalcMin(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_CalcMin,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetDimension(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetDimension,(self,) + _args, _kwargs)
        return val
    def IsWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_IsWindow,(self,) + _args, _kwargs)
        return val
    def IsSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_IsSizer,(self,) + _args, _kwargs)
        return val
    def IsSpacer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_IsSpacer,(self,) + _args, _kwargs)
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def GetSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetSizer,(self,) + _args, _kwargs)
        if val: val = wxSizerPtr(val) 
        return val
    def GetOption(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetOption,(self,) + _args, _kwargs)
        return val
    def GetFlag(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetFlag,(self,) + _args, _kwargs)
        return val
    def GetBorder(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetBorder,(self,) + _args, _kwargs)
        return val
    def SetInitSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetInitSize,(self,) + _args, _kwargs)
        return val
    def SetOption(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetOption,(self,) + _args, _kwargs)
        return val
    def SetFlag(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetFlag,(self,) + _args, _kwargs)
        return val
    def SetBorder(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_SetBorder,(self,) + _args, _kwargs)
        return val
    def GetUserData(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizerItem_GetUserData,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSizerItem instance at %s>" % (self.this,)
class wxSizerItem(wxSizerItemPtr):
    def __init__(self,this):
        self.this = this




class wxSizerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Destroy(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_Destroy,(self,) + _args, _kwargs)
        return val
    def AddWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_AddWindow,(self,) + _args, _kwargs)
        return val
    def AddSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_AddSizer,(self,) + _args, _kwargs)
        return val
    def AddSpacer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_AddSpacer,(self,) + _args, _kwargs)
        return val
    def PrependWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_PrependWindow,(self,) + _args, _kwargs)
        return val
    def PrependSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_PrependSizer,(self,) + _args, _kwargs)
        return val
    def PrependSpacer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_PrependSpacer,(self,) + _args, _kwargs)
        return val
    def RemoveWindow(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_RemoveWindow,(self,) + _args, _kwargs)
        return val
    def RemoveSizer(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_RemoveSizer,(self,) + _args, _kwargs)
        return val
    def RemovePos(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_RemovePos,(self,) + _args, _kwargs)
        return val
    def SetDimension(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetDimension,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_GetPosition,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetMinSize(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_GetMinSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def Layout(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_Layout,(self,) + _args, _kwargs)
        return val
    def Fit(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_Fit,(self,) + _args, _kwargs)
        return val
    def SetSizeHints(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_SetSizeHints,(self,) + _args, _kwargs)
        return val
    def GetChildren(self, *_args, **_kwargs):
        val = apply(sizersc.wxSizer_GetChildren,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSizer instance at %s>" % (self.this,)
    
    def Add(self, *args):
        if type(args[0]) == type(1):
            apply(self.AddSpacer, args)
        elif string.find(args[0].this, 'Sizer') != -1:
            apply(self.AddSizer, args)
        else:
            apply(self.AddWindow, args)

    def Prepend(self, *args):
        if type(args[0]) == type(1):
            apply(self.PrependSpacer, args)
        elif string.find(args[0].this, 'Sizer') != -1:
            apply(self.PrependSizer, args)
        else:
            apply(self.PrependWindow, args)

    def Remove(self, *args):
        if type(args[0]) == type(1):
            apply(self.RemovePos, args)
        elif string.find(args[0].this, 'Sizer') != -1:
            apply(self.RemoveSizer, args)
        else:
            apply(self.RemoveWindow, args)

    def AddMany(self, widgets):
        for childinfo in widgets:
            if type(childinfo) != type(()):
                childinfo = (childinfo, )
            apply(self.Add, childinfo)

class wxSizer(wxSizerPtr):
    def __init__(self,this):
        self.this = this




class wxPySizerPtr(wxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(sizersc.wxPySizer__setSelf,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPySizer instance at %s>" % (self.this,)
class wxPySizer(wxPySizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(sizersc.new_wxPySizer,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self)




class wxBoxSizerPtr(wxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetOrientation(self, *_args, **_kwargs):
        val = apply(sizersc.wxBoxSizer_GetOrientation,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxBoxSizer instance at %s>" % (self.this,)
class wxBoxSizer(wxBoxSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(sizersc.new_wxBoxSizer,_args,_kwargs)
        self.thisown = 1




class wxStaticBoxSizerPtr(wxBoxSizerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetStaticBox(self, *_args, **_kwargs):
        val = apply(sizersc.wxStaticBoxSizer_GetStaticBox,(self,) + _args, _kwargs)
        if val: val = wxStaticBoxPtr(val) 
        return val
    def __repr__(self):
        return "<C wxStaticBoxSizer instance at %s>" % (self.this,)
class wxStaticBoxSizer(wxStaticBoxSizerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(sizersc.new_wxStaticBoxSizer,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

