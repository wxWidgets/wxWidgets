# This file was created automatically by SWIG.
import framesc

from misc import *

from gdi import *

from windows import *

from stattool import *

from controls import *

from events import *
import wxp
class wxFramePtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Centre(self,*args):
        val = apply(framesc.wxFrame_Centre,(self.this,)+args)
        return val
    def CreateStatusBar(self,*args):
        val = apply(framesc.wxFrame_CreateStatusBar,(self.this,)+args)
        val = wxStatusBarPtr(val)
        return val
    def CreateToolBar(self,*args):
        val = apply(framesc.wxFrame_CreateToolBar,(self.this,)+args)
        val = wxToolBarPtr(val)
        return val
    def GetMenuBar(self):
        val = framesc.wxFrame_GetMenuBar(self.this)
        val = wxMenuBarPtr(val)
        return val
    def GetStatusBar(self):
        val = framesc.wxFrame_GetStatusBar(self.this)
        val = wxStatusBarPtr(val)
        return val
    def GetTitle(self):
        val = framesc.wxFrame_GetTitle(self.this)
        return val
    def GetToolBar(self):
        val = framesc.wxFrame_GetToolBar(self.this)
        val = wxToolBarPtr(val)
        return val
    def Iconize(self,arg0):
        val = framesc.wxFrame_Iconize(self.this,arg0)
        return val
    def IsIconized(self):
        val = framesc.wxFrame_IsIconized(self.this)
        return val
    def Maximize(self,arg0):
        val = framesc.wxFrame_Maximize(self.this,arg0)
        return val
    def SetIcon(self,arg0):
        val = framesc.wxFrame_SetIcon(self.this,arg0.this)
        return val
    def SetMenuBar(self,arg0):
        val = framesc.wxFrame_SetMenuBar(self.this,arg0.this)
        return val
    def SetStatusBar(self,arg0):
        val = framesc.wxFrame_SetStatusBar(self.this,arg0.this)
        return val
    def SetStatusText(self,arg0,*args):
        val = apply(framesc.wxFrame_SetStatusText,(self.this,arg0,)+args)
        return val
    def SetStatusWidths(self,arg0,*args):
        val = apply(framesc.wxFrame_SetStatusWidths,(self.this,arg0,)+args)
        return val
    def SetTitle(self,arg0):
        val = framesc.wxFrame_SetTitle(self.this,arg0)
        return val
    def SetToolBar(self,arg0):
        val = framesc.wxFrame_SetToolBar(self.this,arg0.this)
        return val
    def __repr__(self):
        return "<C wxFrame instance>"
class wxFrame(wxFramePtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(framesc.new_wxFrame,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        wxp._StdFrameCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

