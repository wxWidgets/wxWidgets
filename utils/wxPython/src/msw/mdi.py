# This file was created automatically by SWIG.
import mdic

from misc import *

from windows import *

from gdi import *

from clip_dnd import *

from frames import *

from stattool import *

from controls import *

from events import *
import wx
class wxMDIParentFramePtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def ActivateNext(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIParentFrame_ActivateNext,(self,) + _args, _kwargs)
        return val
    def ActivatePrevious(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIParentFrame_ActivatePrevious,(self,) + _args, _kwargs)
        return val
    def ArrangeIcons(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIParentFrame_ArrangeIcons,(self,) + _args, _kwargs)
        return val
    def Cascade(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIParentFrame_Cascade,(self,) + _args, _kwargs)
        return val
    def GetActiveChild(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIParentFrame_GetActiveChild,(self,) + _args, _kwargs)
        if val: val = wxMDIChildFramePtr(val) 
        return val
    def GetClientWindow(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIParentFrame_GetClientWindow,(self,) + _args, _kwargs)
        if val: val = wxMDIClientWindowPtr(val) 
        return val
    def GetToolBar(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIParentFrame_GetToolBar,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def SetToolBar(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIParentFrame_SetToolBar,(self,) + _args, _kwargs)
        return val
    def Tile(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIParentFrame_Tile,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxMDIParentFrame instance at %s>" % (self.this,)
class wxMDIParentFrame(wxMDIParentFramePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(mdic.new_wxMDIParentFrame,_args,_kwargs)
        self.thisown = 1
        wx._StdFrameCallbacks(self)




class wxMDIChildFramePtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Activate(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIChildFrame_Activate,(self,) + _args, _kwargs)
        return val
    def Maximize(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIChildFrame_Maximize,(self,) + _args, _kwargs)
        return val
    def Restore(self, *_args, **_kwargs):
        val = apply(mdic.wxMDIChildFrame_Restore,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxMDIChildFrame instance at %s>" % (self.this,)
class wxMDIChildFrame(wxMDIChildFramePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(mdic.new_wxMDIChildFrame,_args,_kwargs)
        self.thisown = 1
        wx._StdFrameCallbacks(self)




class wxMDIClientWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxMDIClientWindow instance at %s>" % (self.this,)
class wxMDIClientWindow(wxMDIClientWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(mdic.new_wxMDIClientWindow,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)
        wx._StdOnScrollCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

