# This file was created automatically by SWIG.
import windows3c

from misc import *

from gdi import *

from windows import *

from windows2 import *

from controls import *

from events import *

from mdi import *

from frames import *

from stattool import *
import wx
class wxSashEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetEdge(self,arg0):
        val = windows3c.wxSashEvent_SetEdge(self.this,arg0)
        return val
    def GetEdge(self):
        val = windows3c.wxSashEvent_GetEdge(self.this)
        return val
    def SetDragRect(self,arg0):
        val = windows3c.wxSashEvent_SetDragRect(self.this,arg0.this)
        return val
    def GetDragRect(self):
        val = windows3c.wxSashEvent_GetDragRect(self.this)
        val = wxRectPtr(val)
        val.thisown = 1
        return val
    def SetDragStatus(self,arg0):
        val = windows3c.wxSashEvent_SetDragStatus(self.this,arg0)
        return val
    def GetDragStatus(self):
        val = windows3c.wxSashEvent_GetDragStatus(self.this)
        return val
    def __repr__(self):
        return "<C wxSashEvent instance>"
class wxSashEvent(wxSashEventPtr):
    def __init__(self,this):
        self.this = this




class wxSashWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSashVisible(self,arg0):
        val = windows3c.wxSashWindow_GetSashVisible(self.this,arg0)
        return val
    def GetDefaultBorderSize(self):
        val = windows3c.wxSashWindow_GetDefaultBorderSize(self.this)
        return val
    def GetEdgeMargin(self,arg0):
        val = windows3c.wxSashWindow_GetEdgeMargin(self.this,arg0)
        return val
    def GetExtraBorderSize(self):
        val = windows3c.wxSashWindow_GetExtraBorderSize(self.this)
        return val
    def GetMaximumSizeX(self):
        val = windows3c.wxSashWindow_GetMaximumSizeX(self.this)
        return val
    def GetMaximumSizeY(self):
        val = windows3c.wxSashWindow_GetMaximumSizeY(self.this)
        return val
    def GetMinimumSizeX(self):
        val = windows3c.wxSashWindow_GetMinimumSizeX(self.this)
        return val
    def GetMinimumSizeY(self):
        val = windows3c.wxSashWindow_GetMinimumSizeY(self.this)
        return val
    def HasBorder(self,arg0):
        val = windows3c.wxSashWindow_HasBorder(self.this,arg0)
        return val
    def SetDefaultBorderSize(self,arg0):
        val = windows3c.wxSashWindow_SetDefaultBorderSize(self.this,arg0)
        return val
    def SetExtraBorderSize(self,arg0):
        val = windows3c.wxSashWindow_SetExtraBorderSize(self.this,arg0)
        return val
    def SetMaximumSizeX(self,arg0):
        val = windows3c.wxSashWindow_SetMaximumSizeX(self.this,arg0)
        return val
    def SetMaximumSizeY(self,arg0):
        val = windows3c.wxSashWindow_SetMaximumSizeY(self.this,arg0)
        return val
    def SetMinimumSizeX(self,arg0):
        val = windows3c.wxSashWindow_SetMinimumSizeX(self.this,arg0)
        return val
    def SetMinimumSizeY(self,arg0):
        val = windows3c.wxSashWindow_SetMinimumSizeY(self.this,arg0)
        return val
    def SetSashVisible(self,arg0,arg1):
        val = windows3c.wxSashWindow_SetSashVisible(self.this,arg0,arg1)
        return val
    def SetSashBorder(self,arg0,arg1):
        val = windows3c.wxSashWindow_SetSashBorder(self.this,arg0,arg1)
        return val
    def __repr__(self):
        return "<C wxSashWindow instance>"
class wxSashWindow(wxSashWindowPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(windows3c.new_wxSashWindow,(arg0.this,arg1,)+args)
        self.thisown = 1
        wx._StdWindowCallbacks(self)




class wxQueryLayoutInfoEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetRequestedLength(self,arg0):
        val = windows3c.wxQueryLayoutInfoEvent_SetRequestedLength(self.this,arg0)
        return val
    def GetRequestedLength(self):
        val = windows3c.wxQueryLayoutInfoEvent_GetRequestedLength(self.this)
        return val
    def SetFlags(self,arg0):
        val = windows3c.wxQueryLayoutInfoEvent_SetFlags(self.this,arg0)
        return val
    def GetFlags(self):
        val = windows3c.wxQueryLayoutInfoEvent_GetFlags(self.this)
        return val
    def SetSize(self,arg0):
        val = windows3c.wxQueryLayoutInfoEvent_SetSize(self.this,arg0.this)
        return val
    def GetSize(self):
        val = windows3c.wxQueryLayoutInfoEvent_GetSize(self.this)
        val = wxSizePtr(val)
        val.thisown = 1
        return val
    def SetOrientation(self,arg0):
        val = windows3c.wxQueryLayoutInfoEvent_SetOrientation(self.this,arg0)
        return val
    def GetOrientation(self):
        val = windows3c.wxQueryLayoutInfoEvent_GetOrientation(self.this)
        return val
    def SetAlignment(self,arg0):
        val = windows3c.wxQueryLayoutInfoEvent_SetAlignment(self.this,arg0)
        return val
    def GetAlignment(self):
        val = windows3c.wxQueryLayoutInfoEvent_GetAlignment(self.this)
        return val
    def __repr__(self):
        return "<C wxQueryLayoutInfoEvent instance>"
class wxQueryLayoutInfoEvent(wxQueryLayoutInfoEventPtr):
    def __init__(self,this):
        self.this = this




class wxCalculateLayoutEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetFlags(self,arg0):
        val = windows3c.wxCalculateLayoutEvent_SetFlags(self.this,arg0)
        return val
    def GetFlags(self):
        val = windows3c.wxCalculateLayoutEvent_GetFlags(self.this)
        return val
    def SetRect(self,arg0):
        val = windows3c.wxCalculateLayoutEvent_SetRect(self.this,arg0.this)
        return val
    def GetRect(self):
        val = windows3c.wxCalculateLayoutEvent_GetRect(self.this)
        val = wxRectPtr(val)
        val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxCalculateLayoutEvent instance>"
class wxCalculateLayoutEvent(wxCalculateLayoutEventPtr):
    def __init__(self,this):
        self.this = this




class wxSashLayoutWindowPtr(wxSashWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetAlignment(self):
        val = windows3c.wxSashLayoutWindow_GetAlignment(self.this)
        return val
    def GetOrientation(self):
        val = windows3c.wxSashLayoutWindow_GetOrientation(self.this)
        return val
    def SetAlignment(self,arg0):
        val = windows3c.wxSashLayoutWindow_SetAlignment(self.this,arg0)
        return val
    def SetDefaultSize(self,arg0):
        val = windows3c.wxSashLayoutWindow_SetDefaultSize(self.this,arg0.this)
        return val
    def SetOrientation(self,arg0):
        val = windows3c.wxSashLayoutWindow_SetOrientation(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxSashLayoutWindow instance>"
class wxSashLayoutWindow(wxSashLayoutWindowPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        try: argl[1] = argl[1].this
        except: pass
        args = tuple(argl)
        self.this = apply(windows3c.new_wxSashLayoutWindow,(arg0.this,arg1,)+args)
        self.thisown = 1
        wx._StdWindowCallbacks(self)
        wx._checkForCallback(self, 'OnCalculateLayout',    wxEVT_CALCULATE_LAYOUT)
        wx._checkForCallback(self, 'OnQueryLayoutInfo',    wxEVT_QUERY_LAYOUT_INFO)




class wxLayoutAlgorithmPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            windows3c.delete_wxLayoutAlgorithm(self.this)
    def LayoutMDIFrame(self,arg0,*args):
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        val = apply(windows3c.wxLayoutAlgorithm_LayoutMDIFrame,(self.this,arg0.this,)+args)
        return val
    def LayoutFrame(self,arg0,*args):
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        val = apply(windows3c.wxLayoutAlgorithm_LayoutFrame,(self.this,arg0.this,)+args)
        return val
    def LayoutWindow(self,arg0,*args):
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        val = apply(windows3c.wxLayoutAlgorithm_LayoutWindow,(self.this,arg0.this,)+args)
        return val
    def __repr__(self):
        return "<C wxLayoutAlgorithm instance>"
class wxLayoutAlgorithm(wxLayoutAlgorithmPtr):
    def __init__(self) :
        self.this = windows3c.new_wxLayoutAlgorithm()
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxSASH_TOP = windows3c.wxSASH_TOP
wxSASH_RIGHT = windows3c.wxSASH_RIGHT
wxSASH_BOTTOM = windows3c.wxSASH_BOTTOM
wxSASH_LEFT = windows3c.wxSASH_LEFT
wxSASH_NONE = windows3c.wxSASH_NONE
wxEVT_SASH_DRAGGED = windows3c.wxEVT_SASH_DRAGGED
wxSW_3D = windows3c.wxSW_3D
wxSASH_STATUS_OK = windows3c.wxSASH_STATUS_OK
wxSASH_STATUS_OUT_OF_RANGE = windows3c.wxSASH_STATUS_OUT_OF_RANGE
wxLAYOUT_HORIZONTAL = windows3c.wxLAYOUT_HORIZONTAL
wxLAYOUT_VERTICAL = windows3c.wxLAYOUT_VERTICAL
wxLAYOUT_NONE = windows3c.wxLAYOUT_NONE
wxLAYOUT_TOP = windows3c.wxLAYOUT_TOP
wxLAYOUT_LEFT = windows3c.wxLAYOUT_LEFT
wxLAYOUT_RIGHT = windows3c.wxLAYOUT_RIGHT
wxLAYOUT_BOTTOM = windows3c.wxLAYOUT_BOTTOM
wxEVT_QUERY_LAYOUT_INFO = windows3c.wxEVT_QUERY_LAYOUT_INFO
wxEVT_CALCULATE_LAYOUT = windows3c.wxEVT_CALCULATE_LAYOUT
