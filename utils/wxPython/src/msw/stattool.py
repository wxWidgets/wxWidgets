# This file was created automatically by SWIG.
import stattoolc

from misc import *

from windows import *

from gdi import *

from controls import *

from events import *
class wxStatusBarPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetFieldRect(self,arg0):
        val = stattoolc.wxStatusBar_GetFieldRect(self.this,arg0)
        val = wxRectPtr(val)
        val.thisown = 1
        return val
    def GetFieldsCount(self):
        val = stattoolc.wxStatusBar_GetFieldsCount(self.this)
        return val
    def GetStatusText(self,*args):
        val = apply(stattoolc.wxStatusBar_GetStatusText,(self.this,)+args)
        return val
    def DrawField(self,arg0,arg1):
        val = stattoolc.wxStatusBar_DrawField(self.this,arg0.this,arg1)
        return val
    def DrawFieldText(self,arg0,arg1):
        val = stattoolc.wxStatusBar_DrawFieldText(self.this,arg0.this,arg1)
        return val
    def InitColours(self):
        val = stattoolc.wxStatusBar_InitColours(self.this)
        return val
    def SetFieldsCount(self,*args):
        val = apply(stattoolc.wxStatusBar_SetFieldsCount,(self.this,)+args)
        return val
    def SetStatusText(self,arg0,*args):
        val = apply(stattoolc.wxStatusBar_SetStatusText,(self.this,arg0,)+args)
        return val
    def SetStatusWidths(self,arg0,*args):
        val = apply(stattoolc.wxStatusBar_SetStatusWidths,(self.this,arg0,)+args)
        return val
    def __repr__(self):
        return "<C wxStatusBar instance>"
class wxStatusBar(wxStatusBarPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(stattoolc.new_wxStatusBar,(arg0.this,arg1,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxToolBarToolPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            stattoolc.delete_wxToolBarTool(self.this)
    def SetSize(self,arg0,arg1):
        val = stattoolc.wxToolBarTool_SetSize(self.this,arg0,arg1)
        return val
    def GetWidth(self):
        val = stattoolc.wxToolBarTool_GetWidth(self.this)
        return val
    def GetHeight(self):
        val = stattoolc.wxToolBarTool_GetHeight(self.this)
        return val
    def __setattr__(self,name,value):
        if name == "m_toolStyle" :
            stattoolc.wxToolBarTool_m_toolStyle_set(self.this,value)
            return
        if name == "m_clientData" :
            stattoolc.wxToolBarTool_m_clientData_set(self.this,value)
            return
        if name == "m_index" :
            stattoolc.wxToolBarTool_m_index_set(self.this,value)
            return
        if name == "m_x" :
            stattoolc.wxToolBarTool_m_x_set(self.this,value)
            return
        if name == "m_y" :
            stattoolc.wxToolBarTool_m_y_set(self.this,value)
            return
        if name == "m_width" :
            stattoolc.wxToolBarTool_m_width_set(self.this,value)
            return
        if name == "m_height" :
            stattoolc.wxToolBarTool_m_height_set(self.this,value)
            return
        if name == "m_toggleState" :
            stattoolc.wxToolBarTool_m_toggleState_set(self.this,value)
            return
        if name == "m_isToggle" :
            stattoolc.wxToolBarTool_m_isToggle_set(self.this,value)
            return
        if name == "m_deleteSecondBitmap" :
            stattoolc.wxToolBarTool_m_deleteSecondBitmap_set(self.this,value)
            return
        if name == "m_enabled" :
            stattoolc.wxToolBarTool_m_enabled_set(self.this,value)
            return
        if name == "m_bitmap1" :
            stattoolc.wxToolBarTool_m_bitmap1_set(self.this,value.this)
            return
        if name == "m_bitmap2" :
            stattoolc.wxToolBarTool_m_bitmap2_set(self.this,value.this)
            return
        if name == "m_isMenuCommand" :
            stattoolc.wxToolBarTool_m_isMenuCommand_set(self.this,value)
            return
        if name == "m_shortHelpString" :
            stattoolc.wxToolBarTool_m_shortHelpString_set(self.this,value)
            return
        if name == "m_longHelpString" :
            stattoolc.wxToolBarTool_m_longHelpString_set(self.this,value)
            return
        self.__dict__[name] = value
    def __getattr__(self,name):
        if name == "m_toolStyle" : 
            return stattoolc.wxToolBarTool_m_toolStyle_get(self.this)
        if name == "m_clientData" : 
            return stattoolc.wxToolBarTool_m_clientData_get(self.this)
        if name == "m_index" : 
            return stattoolc.wxToolBarTool_m_index_get(self.this)
        if name == "m_x" : 
            return stattoolc.wxToolBarTool_m_x_get(self.this)
        if name == "m_y" : 
            return stattoolc.wxToolBarTool_m_y_get(self.this)
        if name == "m_width" : 
            return stattoolc.wxToolBarTool_m_width_get(self.this)
        if name == "m_height" : 
            return stattoolc.wxToolBarTool_m_height_get(self.this)
        if name == "m_toggleState" : 
            return stattoolc.wxToolBarTool_m_toggleState_get(self.this)
        if name == "m_isToggle" : 
            return stattoolc.wxToolBarTool_m_isToggle_get(self.this)
        if name == "m_deleteSecondBitmap" : 
            return stattoolc.wxToolBarTool_m_deleteSecondBitmap_get(self.this)
        if name == "m_enabled" : 
            return stattoolc.wxToolBarTool_m_enabled_get(self.this)
        if name == "m_bitmap1" : 
            return wxBitmapPtr(stattoolc.wxToolBarTool_m_bitmap1_get(self.this))
        if name == "m_bitmap2" : 
            return wxBitmapPtr(stattoolc.wxToolBarTool_m_bitmap2_get(self.this))
        if name == "m_isMenuCommand" : 
            return stattoolc.wxToolBarTool_m_isMenuCommand_get(self.this)
        if name == "m_shortHelpString" : 
            return stattoolc.wxToolBarTool_m_shortHelpString_get(self.this)
        if name == "m_longHelpString" : 
            return stattoolc.wxToolBarTool_m_longHelpString_get(self.this)
        raise AttributeError,name
    def __repr__(self):
        return "<C wxToolBarTool instance>"
class wxToolBarTool(wxToolBarToolPtr):
    def __init__(self) :
        self.this = stattoolc.new_wxToolBarTool()
        self.thisown = 1




class wxToolBarBasePtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def AddSeparator(self):
        val = stattoolc.wxToolBarBase_AddSeparator(self.this)
        return val
    def AddTool(self,arg0,arg1,*args):
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        val = apply(stattoolc.wxToolBarBase_AddTool,(self.this,arg0,arg1.this,)+args)
        val = wxToolBarToolPtr(val)
        return val
    def EnableTool(self,arg0,arg1):
        val = stattoolc.wxToolBarBase_EnableTool(self.this,arg0,arg1)
        return val
    def FindToolForPosition(self,arg0,arg1):
        val = stattoolc.wxToolBarBase_FindToolForPosition(self.this,arg0,arg1)
        val = wxToolBarToolPtr(val)
        return val
    def GetToolSize(self):
        val = stattoolc.wxToolBarBase_GetToolSize(self.this)
        val = wxSizePtr(val)
        val.thisown = 1
        return val
    def GetToolBitmapSize(self):
        val = stattoolc.wxToolBarBase_GetToolBitmapSize(self.this)
        val = wxSizePtr(val)
        val.thisown = 1
        return val
    def GetMaxSize(self):
        val = stattoolc.wxToolBarBase_GetMaxSize(self.this)
        val = wxSizePtr(val)
        val.thisown = 1
        return val
    def GetToolEnabled(self,arg0):
        val = stattoolc.wxToolBarBase_GetToolEnabled(self.this,arg0)
        return val
    def GetToolLongHelp(self,arg0):
        val = stattoolc.wxToolBarBase_GetToolLongHelp(self.this,arg0)
        return val
    def GetToolPacking(self):
        val = stattoolc.wxToolBarBase_GetToolPacking(self.this)
        return val
    def GetToolSeparation(self):
        val = stattoolc.wxToolBarBase_GetToolSeparation(self.this)
        return val
    def GetToolShortHelp(self,arg0):
        val = stattoolc.wxToolBarBase_GetToolShortHelp(self.this,arg0)
        return val
    def GetToolState(self,arg0):
        val = stattoolc.wxToolBarBase_GetToolState(self.this,arg0)
        return val
    def Realize(self):
        val = stattoolc.wxToolBarBase_Realize(self.this)
        return val
    def SetToolBitmapSize(self,arg0):
        val = stattoolc.wxToolBarBase_SetToolBitmapSize(self.this,arg0.this)
        return val
    def SetMargins(self,arg0):
        val = stattoolc.wxToolBarBase_SetMargins(self.this,arg0.this)
        return val
    def SetToolLongHelp(self,arg0,arg1):
        val = stattoolc.wxToolBarBase_SetToolLongHelp(self.this,arg0,arg1)
        return val
    def SetToolPacking(self,arg0):
        val = stattoolc.wxToolBarBase_SetToolPacking(self.this,arg0)
        return val
    def SetToolShortHelp(self,arg0,arg1):
        val = stattoolc.wxToolBarBase_SetToolShortHelp(self.this,arg0,arg1)
        return val
    def SetToolSeparation(self,arg0):
        val = stattoolc.wxToolBarBase_SetToolSeparation(self.this,arg0)
        return val
    def ToggleTool(self,arg0,arg1):
        val = stattoolc.wxToolBarBase_ToggleTool(self.this,arg0,arg1)
        return val
    def __repr__(self):
        return "<C wxToolBarBase instance>"
class wxToolBarBase(wxToolBarBasePtr):
    def __init__(self,this):
        self.this = this




class wxToolBarPtr(wxToolBarBasePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxToolBar instance>"
class wxToolBar(wxToolBarPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(stattoolc.new_wxToolBar,(arg0.this,arg1,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)




class wxToolBarSimplePtr(wxToolBarBasePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxToolBarSimple instance>"
class wxToolBarSimple(wxToolBarSimplePtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(stattoolc.new_wxToolBarSimple,(arg0.this,arg1,)+args)
        self.thisown = 1
        wxp._StdWindowCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

