# This file was created automatically by SWIG.
import windows3c

from misc import *

from gdi import *

from windows import *

from clip_dnd import *

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
    def SetEdge(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashEvent_SetEdge,(self,) + _args, _kwargs)
        return val
    def GetEdge(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashEvent_GetEdge,(self,) + _args, _kwargs)
        return val
    def SetDragRect(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashEvent_SetDragRect,(self,) + _args, _kwargs)
        return val
    def GetDragRect(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashEvent_GetDragRect,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def SetDragStatus(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashEvent_SetDragStatus,(self,) + _args, _kwargs)
        return val
    def GetDragStatus(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashEvent_GetDragStatus,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSashEvent instance at %s>" % (self.this,)
class wxSashEvent(wxSashEventPtr):
    def __init__(self,this):
        self.this = this




class wxSashWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSashVisible(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_GetSashVisible,(self,) + _args, _kwargs)
        return val
    def GetDefaultBorderSize(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_GetDefaultBorderSize,(self,) + _args, _kwargs)
        return val
    def GetEdgeMargin(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_GetEdgeMargin,(self,) + _args, _kwargs)
        return val
    def GetExtraBorderSize(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_GetExtraBorderSize,(self,) + _args, _kwargs)
        return val
    def GetMaximumSizeX(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_GetMaximumSizeX,(self,) + _args, _kwargs)
        return val
    def GetMaximumSizeY(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_GetMaximumSizeY,(self,) + _args, _kwargs)
        return val
    def GetMinimumSizeX(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_GetMinimumSizeX,(self,) + _args, _kwargs)
        return val
    def GetMinimumSizeY(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_GetMinimumSizeY,(self,) + _args, _kwargs)
        return val
    def HasBorder(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_HasBorder,(self,) + _args, _kwargs)
        return val
    def SetDefaultBorderSize(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_SetDefaultBorderSize,(self,) + _args, _kwargs)
        return val
    def SetExtraBorderSize(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_SetExtraBorderSize,(self,) + _args, _kwargs)
        return val
    def SetMaximumSizeX(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_SetMaximumSizeX,(self,) + _args, _kwargs)
        return val
    def SetMaximumSizeY(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_SetMaximumSizeY,(self,) + _args, _kwargs)
        return val
    def SetMinimumSizeX(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_SetMinimumSizeX,(self,) + _args, _kwargs)
        return val
    def SetMinimumSizeY(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_SetMinimumSizeY,(self,) + _args, _kwargs)
        return val
    def SetSashVisible(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_SetSashVisible,(self,) + _args, _kwargs)
        return val
    def SetSashBorder(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashWindow_SetSashBorder,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSashWindow instance at %s>" % (self.this,)
class wxSashWindow(wxSashWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows3c.new_wxSashWindow,_args,_kwargs)
        self.thisown = 1
        #wx._StdWindowCallbacks(self)




class wxQueryLayoutInfoEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetRequestedLength(self, *_args, **_kwargs):
        val = apply(windows3c.wxQueryLayoutInfoEvent_SetRequestedLength,(self,) + _args, _kwargs)
        return val
    def GetRequestedLength(self, *_args, **_kwargs):
        val = apply(windows3c.wxQueryLayoutInfoEvent_GetRequestedLength,(self,) + _args, _kwargs)
        return val
    def SetFlags(self, *_args, **_kwargs):
        val = apply(windows3c.wxQueryLayoutInfoEvent_SetFlags,(self,) + _args, _kwargs)
        return val
    def GetFlags(self, *_args, **_kwargs):
        val = apply(windows3c.wxQueryLayoutInfoEvent_GetFlags,(self,) + _args, _kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = apply(windows3c.wxQueryLayoutInfoEvent_SetSize,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(windows3c.wxQueryLayoutInfoEvent_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetOrientation(self, *_args, **_kwargs):
        val = apply(windows3c.wxQueryLayoutInfoEvent_SetOrientation,(self,) + _args, _kwargs)
        return val
    def GetOrientation(self, *_args, **_kwargs):
        val = apply(windows3c.wxQueryLayoutInfoEvent_GetOrientation,(self,) + _args, _kwargs)
        return val
    def SetAlignment(self, *_args, **_kwargs):
        val = apply(windows3c.wxQueryLayoutInfoEvent_SetAlignment,(self,) + _args, _kwargs)
        return val
    def GetAlignment(self, *_args, **_kwargs):
        val = apply(windows3c.wxQueryLayoutInfoEvent_GetAlignment,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxQueryLayoutInfoEvent instance at %s>" % (self.this,)
class wxQueryLayoutInfoEvent(wxQueryLayoutInfoEventPtr):
    def __init__(self,this):
        self.this = this




class wxCalculateLayoutEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetFlags(self, *_args, **_kwargs):
        val = apply(windows3c.wxCalculateLayoutEvent_SetFlags,(self,) + _args, _kwargs)
        return val
    def GetFlags(self, *_args, **_kwargs):
        val = apply(windows3c.wxCalculateLayoutEvent_GetFlags,(self,) + _args, _kwargs)
        return val
    def SetRect(self, *_args, **_kwargs):
        val = apply(windows3c.wxCalculateLayoutEvent_SetRect,(self,) + _args, _kwargs)
        return val
    def GetRect(self, *_args, **_kwargs):
        val = apply(windows3c.wxCalculateLayoutEvent_GetRect,(self,) + _args, _kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<C wxCalculateLayoutEvent instance at %s>" % (self.this,)
class wxCalculateLayoutEvent(wxCalculateLayoutEventPtr):
    def __init__(self,this):
        self.this = this




class wxSashLayoutWindowPtr(wxSashWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetAlignment(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashLayoutWindow_GetAlignment,(self,) + _args, _kwargs)
        return val
    def GetOrientation(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashLayoutWindow_GetOrientation,(self,) + _args, _kwargs)
        return val
    def SetAlignment(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashLayoutWindow_SetAlignment,(self,) + _args, _kwargs)
        return val
    def SetDefaultSize(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashLayoutWindow_SetDefaultSize,(self,) + _args, _kwargs)
        return val
    def SetOrientation(self, *_args, **_kwargs):
        val = apply(windows3c.wxSashLayoutWindow_SetOrientation,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSashLayoutWindow instance at %s>" % (self.this,)
class wxSashLayoutWindow(wxSashLayoutWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows3c.new_wxSashLayoutWindow,_args,_kwargs)
        self.thisown = 1
        #wx._StdWindowCallbacks(self)
        #wx._checkForCallback(self, 'OnCalculateLayout',    wxEVT_CALCULATE_LAYOUT)
        #wx._checkForCallback(self, 'OnQueryLayoutInfo',    wxEVT_QUERY_LAYOUT_INFO)




class wxLayoutAlgorithmPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,windows3c=windows3c):
        if self.thisown == 1 :
            windows3c.delete_wxLayoutAlgorithm(self)
    def LayoutMDIFrame(self, *_args, **_kwargs):
        val = apply(windows3c.wxLayoutAlgorithm_LayoutMDIFrame,(self,) + _args, _kwargs)
        return val
    def LayoutFrame(self, *_args, **_kwargs):
        val = apply(windows3c.wxLayoutAlgorithm_LayoutFrame,(self,) + _args, _kwargs)
        return val
    def LayoutWindow(self, *_args, **_kwargs):
        val = apply(windows3c.wxLayoutAlgorithm_LayoutWindow,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxLayoutAlgorithm instance at %s>" % (self.this,)
class wxLayoutAlgorithm(wxLayoutAlgorithmPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows3c.new_wxLayoutAlgorithm,_args,_kwargs)
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
wxSW_3DSASH = windows3c.wxSW_3DSASH
wxSW_3DBORDER = windows3c.wxSW_3DBORDER
wxSW_BORDER = windows3c.wxSW_BORDER
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
