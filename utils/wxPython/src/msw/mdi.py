# This file was created automatically by SWIG.
import mdic

from misc import *

from windows import *

from gdi import *

from frames import *

from stattool import *

from controls import *

from events import *
import wx
class wxMDIParentFramePtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def ActivateNext(self):
        val = mdic.wxMDIParentFrame_ActivateNext(self.this)
        return val
    def ActivatePrevious(self):
        val = mdic.wxMDIParentFrame_ActivatePrevious(self.this)
        return val
    def ArrangeIcons(self):
        val = mdic.wxMDIParentFrame_ArrangeIcons(self.this)
        return val
    def Cascade(self):
        val = mdic.wxMDIParentFrame_Cascade(self.this)
        return val
    def GetClientSize(self):
        val = mdic.wxMDIParentFrame_GetClientSize(self.this)
        return val
    def GetActiveChild(self):
        val = mdic.wxMDIParentFrame_GetActiveChild(self.this)
        val = wxMDIChildFramePtr(val)
        return val
    def GetClientWindow(self):
        val = mdic.wxMDIParentFrame_GetClientWindow(self.this)
        val = wxMDIClientWindowPtr(val)
        return val
    def GetToolBar(self):
        val = mdic.wxMDIParentFrame_GetToolBar(self.this)
        val = wxWindowPtr(val)
        return val
    def SetToolBar(self,arg0):
        val = mdic.wxMDIParentFrame_SetToolBar(self.this,arg0.this)
        return val
    def Tile(self):
        val = mdic.wxMDIParentFrame_Tile(self.this)
        return val
    def __repr__(self):
        return "<C wxMDIParentFrame instance>"
class wxMDIParentFrame(wxMDIParentFramePtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(mdic.new_wxMDIParentFrame,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        wx._StdFrameCallbacks(self)




class wxMDIChildFramePtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Activate(self):
        val = mdic.wxMDIChildFrame_Activate(self.this)
        return val
    def Maximize(self):
        val = mdic.wxMDIChildFrame_Maximize(self.this)
        return val
    def Restore(self):
        val = mdic.wxMDIChildFrame_Restore(self.this)
        return val
    def SetMenuBar(self,arg0):
        val = mdic.wxMDIChildFrame_SetMenuBar(self.this,arg0.this)
        return val
    def SetClientSize(self,arg0,arg1):
        val = mdic.wxMDIChildFrame_SetClientSize(self.this,arg0,arg1)
        return val
    def GetPosition(self):
        val = mdic.wxMDIChildFrame_GetPosition(self.this)
        return val
    def __repr__(self):
        return "<C wxMDIChildFrame instance>"
class wxMDIChildFrame(wxMDIChildFramePtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(mdic.new_wxMDIChildFrame,(arg0.this,arg1,arg2,)+args)
        self.thisown = 1
        wx._StdFrameCallbacks(self)




class wxMDIClientWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxMDIClientWindow instance>"
class wxMDIClientWindow(wxMDIClientWindowPtr):
    def __init__(self,arg0,*args) :
        self.this = apply(mdic.new_wxMDIClientWindow,(arg0.this,)+args)
        self.thisown = 1
        wx._StdWindowCallbacks(self)
        wx._StdOnScrollCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

