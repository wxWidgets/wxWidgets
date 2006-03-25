# This file was created automatically by SWIG.
import framesc

from misc import *

from gdi import *

from windows import *

from clip_dnd import *

from stattool import *

from controls import *

from events import *
import wx
class wxFramePtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Centre(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_Centre,(self,) + _args, _kwargs)
        return val
    def CreateStatusBar(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_CreateStatusBar,(self,) + _args, _kwargs)
        if val: val = wxStatusBarPtr(val) 
        return val
    def CreateToolBar(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_CreateToolBar,(self,) + _args, _kwargs)
        if val: val = wxToolBarPtr(val) 
        return val
    def GetIcon(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_GetIcon,(self,) + _args, _kwargs)
        if val: val = wxIconPtr(val) 
        return val
    def GetMenuBar(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_GetMenuBar,(self,) + _args, _kwargs)
        if val: val = wxMenuBarPtr(val) 
        return val
    def GetStatusBar(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_GetStatusBar,(self,) + _args, _kwargs)
        if val: val = wxStatusBarPtr(val) 
        return val
    def GetTitle(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_GetTitle,(self,) + _args, _kwargs)
        return val
    def GetToolBar(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_GetToolBar,(self,) + _args, _kwargs)
        if val: val = wxToolBarPtr(val) 
        return val
    def Iconize(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_Iconize,(self,) + _args, _kwargs)
        return val
    def IsIconized(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_IsIconized,(self,) + _args, _kwargs)
        return val
    def Maximize(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_Maximize,(self,) + _args, _kwargs)
        return val
    def IsMaximized(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_IsMaximized,(self,) + _args, _kwargs)
        return val
    def Restore(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_Restore,(self,) + _args, _kwargs)
        return val
    def SetAcceleratorTable(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_SetAcceleratorTable,(self,) + _args, _kwargs)
        return val
    def SetIcon(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_SetIcon,(self,) + _args, _kwargs)
        return val
    def SetMenuBar(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_SetMenuBar,(self,) + _args, _kwargs)
        return val
    def SetStatusBar(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_SetStatusBar,(self,) + _args, _kwargs)
        return val
    def SetStatusText(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_SetStatusText,(self,) + _args, _kwargs)
        return val
    def SetStatusWidths(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_SetStatusWidths,(self,) + _args, _kwargs)
        return val
    def SetTitle(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_SetTitle,(self,) + _args, _kwargs)
        return val
    def SetToolBar(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_SetToolBar,(self,) + _args, _kwargs)
        return val
    def MakeModal(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_MakeModal,(self,) + _args, _kwargs)
        return val
    def GetClientAreaOrigin(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_GetClientAreaOrigin,(self,) + _args, _kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def Command(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_Command,(self,) + _args, _kwargs)
        return val
    def ProcessCommand(self, *_args, **_kwargs):
        val = apply(framesc.wxFrame_ProcessCommand,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFrame instance at %s>" % (self.this,)
class wxFrame(wxFramePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(framesc.new_wxFrame,_args,_kwargs)
        self.thisown = 1
        #wx._StdFrameCallbacks(self)




class wxMiniFramePtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxMiniFrame instance at %s>" % (self.this,)
class wxMiniFrame(wxMiniFramePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(framesc.new_wxMiniFrame,_args,_kwargs)
        self.thisown = 1
        #wx._StdFrameCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxFULLSCREEN_NOMENUBAR = framesc.wxFULLSCREEN_NOMENUBAR
wxFULLSCREEN_NOTOOLBAR = framesc.wxFULLSCREEN_NOTOOLBAR
wxFULLSCREEN_NOSTATUSBAR = framesc.wxFULLSCREEN_NOSTATUSBAR
wxFULLSCREEN_NOBORDER = framesc.wxFULLSCREEN_NOBORDER
wxFULLSCREEN_NOCAPTION = framesc.wxFULLSCREEN_NOCAPTION
wxFULLSCREEN_ALL = framesc.wxFULLSCREEN_ALL
