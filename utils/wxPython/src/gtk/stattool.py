# This file was created automatically by SWIG.
import stattoolc

from misc import *

from windows import *

from gdi import *

from controls import *

from events import *
import wx
class wxStatusBarPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetFieldRect(self, *_args, **_kwargs):
        val = apply(stattoolc.wxStatusBar_GetFieldRect,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def GetFieldsCount(self, *_args, **_kwargs):
        val = apply(stattoolc.wxStatusBar_GetFieldsCount,(self,) + _args, _kwargs)
        return val
    def GetStatusText(self, *_args, **_kwargs):
        val = apply(stattoolc.wxStatusBar_GetStatusText,(self,) + _args, _kwargs)
        return val
    def DrawField(self, *_args, **_kwargs):
        val = apply(stattoolc.wxStatusBar_DrawField,(self,) + _args, _kwargs)
        return val
    def DrawFieldText(self, *_args, **_kwargs):
        val = apply(stattoolc.wxStatusBar_DrawFieldText,(self,) + _args, _kwargs)
        return val
    def InitColours(self, *_args, **_kwargs):
        val = apply(stattoolc.wxStatusBar_InitColours,(self,) + _args, _kwargs)
        return val
    def SetFieldsCount(self, *_args, **_kwargs):
        val = apply(stattoolc.wxStatusBar_SetFieldsCount,(self,) + _args, _kwargs)
        return val
    def SetStatusText(self, *_args, **_kwargs):
        val = apply(stattoolc.wxStatusBar_SetStatusText,(self,) + _args, _kwargs)
        return val
    def SetStatusWidths(self, *_args, **_kwargs):
        val = apply(stattoolc.wxStatusBar_SetStatusWidths,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxStatusBar instance at %s>" % (self.this,)
class wxStatusBar(wxStatusBarPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(stattoolc.new_wxStatusBar,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxToolBarToolPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,stattoolc=stattoolc):
        if self.thisown == 1 :
            stattoolc.delete_wxToolBarTool(self)
    def __setattr__(self,name,value):
        if name == "m_toolStyle" :
            stattoolc.wxToolBarTool_m_toolStyle_set(self,value)
            return
        if name == "m_clientData" :
            stattoolc.wxToolBarTool_m_clientData_set(self,value)
            return
        if name == "m_index" :
            stattoolc.wxToolBarTool_m_index_set(self,value)
            return
        if name == "m_toggleState" :
            stattoolc.wxToolBarTool_m_toggleState_set(self,value)
            return
        if name == "m_isToggle" :
            stattoolc.wxToolBarTool_m_isToggle_set(self,value)
            return
        if name == "m_deleteSecondBitmap" :
            stattoolc.wxToolBarTool_m_deleteSecondBitmap_set(self,value)
            return
        if name == "m_enabled" :
            stattoolc.wxToolBarTool_m_enabled_set(self,value)
            return
        if name == "m_bitmap1" :
            stattoolc.wxToolBarTool_m_bitmap1_set(self,value.this)
            return
        if name == "m_bitmap2" :
            stattoolc.wxToolBarTool_m_bitmap2_set(self,value.this)
            return
        if name == "m_isMenuCommand" :
            stattoolc.wxToolBarTool_m_isMenuCommand_set(self,value)
            return
        if name == "m_shortHelpString" :
            stattoolc.wxToolBarTool_m_shortHelpString_set(self,value)
            return
        if name == "m_longHelpString" :
            stattoolc.wxToolBarTool_m_longHelpString_set(self,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_toolStyle" : 
            return stattoolc.wxToolBarTool_m_toolStyle_get(self)
        if name == "m_clientData" : 
            return stattoolc.wxToolBarTool_m_clientData_get(self)
        if name == "m_index" : 
            return stattoolc.wxToolBarTool_m_index_get(self)
        if name == "m_toggleState" : 
            return stattoolc.wxToolBarTool_m_toggleState_get(self)
        if name == "m_isToggle" : 
            return stattoolc.wxToolBarTool_m_isToggle_get(self)
        if name == "m_deleteSecondBitmap" : 
            return stattoolc.wxToolBarTool_m_deleteSecondBitmap_get(self)
        if name == "m_enabled" : 
            return stattoolc.wxToolBarTool_m_enabled_get(self)
        if name == "m_bitmap1" : 
            return wxBitmapPtr(stattoolc.wxToolBarTool_m_bitmap1_get(self))
        if name == "m_bitmap2" : 
            return wxBitmapPtr(stattoolc.wxToolBarTool_m_bitmap2_get(self))
        if name == "m_isMenuCommand" : 
            return stattoolc.wxToolBarTool_m_isMenuCommand_get(self)
        if name == "m_shortHelpString" : 
            return stattoolc.wxToolBarTool_m_shortHelpString_get(self)
        if name == "m_longHelpString" : 
            return stattoolc.wxToolBarTool_m_longHelpString_get(self)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxToolBarTool instance at %s>" % (self.this,)
class wxToolBarTool(wxToolBarToolPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(stattoolc.new_wxToolBarTool,_args,_kwargs)
        self.thisown = 1




class wxToolBarPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def AddSeparator(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_AddSeparator,(self,) + _args, _kwargs)
        return val
    def AddTool(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_AddTool,(self,) + _args, _kwargs)
        if val: val = wxToolBarToolPtr(val) 
        return val
    def AddSimpleTool(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_AddSimpleTool,(self,) + _args, _kwargs)
        if val: val = wxToolBarToolPtr(val) 
        return val
    def EnableTool(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_EnableTool,(self,) + _args, _kwargs)
        return val
    def GetToolEnabled(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_GetToolEnabled,(self,) + _args, _kwargs)
        return val
    def GetToolLongHelp(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_GetToolLongHelp,(self,) + _args, _kwargs)
        return val
    def GetToolPacking(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_GetToolPacking,(self,) + _args, _kwargs)
        return val
    def GetToolSeparation(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_GetToolSeparation,(self,) + _args, _kwargs)
        return val
    def GetToolShortHelp(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_GetToolShortHelp,(self,) + _args, _kwargs)
        return val
    def GetToolState(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_GetToolState,(self,) + _args, _kwargs)
        return val
    def Realize(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_Realize,(self,) + _args, _kwargs)
        return val
    def SetToolLongHelp(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_SetToolLongHelp,(self,) + _args, _kwargs)
        return val
    def SetToolShortHelp(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_SetToolShortHelp,(self,) + _args, _kwargs)
        return val
    def SetMargins(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_SetMargins,(self,) + _args, _kwargs)
        return val
    def SetToolPacking(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_SetToolPacking,(self,) + _args, _kwargs)
        return val
    def SetToolSeparation(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_SetToolSeparation,(self,) + _args, _kwargs)
        return val
    def ToggleTool(self, *_args, **_kwargs):
        val = apply(stattoolc.wxToolBar_ToggleTool,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxToolBar instance at %s>" % (self.this,)
class wxToolBar(wxToolBarPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(stattoolc.new_wxToolBar,_args,_kwargs)
        self.thisown = 1
        wx._StdWindowCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

