# This file was created automatically by SWIG.
import gizmosc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from fonts import *

from clip_dnd import *

from events import *

from streams import *

from utils import *

from mdi import *

from frames import *

from stattool import *

from controls import *

from controls2 import *

from windows2 import *

from cmndlgs import *

from windows3 import *

from image import *

from printfw import *

from sizers import *

from filesys import *

def EVT_DYNAMIC_SASH_SPLIT(win, id, func):
    win.Connect(id, -1, wxEVT_DYNAMIC_SASH_SPLIT, func)

def EVT_DYNAMIC_SASH_UNIFY(win, id, func):
    win.Connect(id, -1, wxEVT_DYNAMIC_SASH_UNIFY, func)

class wxDynamicSashSplitEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxDynamicSashSplitEvent instance at %s>" % (self.this,)
class wxDynamicSashSplitEvent(wxDynamicSashSplitEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gizmosc.new_wxDynamicSashSplitEvent,_args,_kwargs)
        self.thisown = 1




class wxDynamicSashUnifyEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxDynamicSashUnifyEvent instance at %s>" % (self.this,)
class wxDynamicSashUnifyEvent(wxDynamicSashUnifyEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gizmosc.new_wxDynamicSashUnifyEvent,_args,_kwargs)
        self.thisown = 1




class wxDynamicSashWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = apply(gizmosc.wxDynamicSashWindow_Create,(self,) + _args, _kwargs)
        return val
    def GetHScrollBar(self, *_args, **_kwargs):
        val = apply(gizmosc.wxDynamicSashWindow_GetHScrollBar,(self,) + _args, _kwargs)
        if val: val = wxScrollBarPtr(val) 
        return val
    def GetVScrollBar(self, *_args, **_kwargs):
        val = apply(gizmosc.wxDynamicSashWindow_GetVScrollBar,(self,) + _args, _kwargs)
        if val: val = wxScrollBarPtr(val) 
        return val
    def __repr__(self):
        return "<C wxDynamicSashWindow instance at %s>" % (self.this,)
class wxDynamicSashWindow(wxDynamicSashWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gizmosc.new_wxDynamicSashWindow,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreDynamicSashWindow(*_args,**_kwargs):
    val = wxDynamicSashWindowPtr(apply(gizmosc.new_wxPreDynamicSashWindow,_args,_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxEditableListBoxPtr(wxPanelPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetStrings(self, *_args, **_kwargs):
        val = apply(gizmosc.wxEditableListBox_SetStrings,(self,) + _args, _kwargs)
        return val
    def GetStrings(self, *_args, **_kwargs):
        val = apply(gizmosc.wxEditableListBox_GetStrings,(self,) + _args, _kwargs)
        return val
    def GetListCtrl(self, *_args, **_kwargs):
        val = apply(gizmosc.wxEditableListBox_GetListCtrl,(self,) + _args, _kwargs)
        return val
    def GetDelButton(self, *_args, **_kwargs):
        val = apply(gizmosc.wxEditableListBox_GetDelButton,(self,) + _args, _kwargs)
        return val
    def GetNewButton(self, *_args, **_kwargs):
        val = apply(gizmosc.wxEditableListBox_GetNewButton,(self,) + _args, _kwargs)
        return val
    def GetUpButton(self, *_args, **_kwargs):
        val = apply(gizmosc.wxEditableListBox_GetUpButton,(self,) + _args, _kwargs)
        return val
    def GetDownButton(self, *_args, **_kwargs):
        val = apply(gizmosc.wxEditableListBox_GetDownButton,(self,) + _args, _kwargs)
        return val
    def GetEditButton(self, *_args, **_kwargs):
        val = apply(gizmosc.wxEditableListBox_GetEditButton,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxEditableListBox instance at %s>" % (self.this,)
class wxEditableListBox(wxEditableListBoxPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gizmosc.new_wxEditableListBox,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxRemotelyScrolledTreeCtrlPtr(wxTreeCtrlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def HideVScrollbar(self, *_args, **_kwargs):
        val = apply(gizmosc.wxRemotelyScrolledTreeCtrl_HideVScrollbar,(self,) + _args, _kwargs)
        return val
    def AdjustRemoteScrollbars(self, *_args, **_kwargs):
        val = apply(gizmosc.wxRemotelyScrolledTreeCtrl_AdjustRemoteScrollbars,(self,) + _args, _kwargs)
        return val
    def GetScrolledWindow(self, *_args, **_kwargs):
        val = apply(gizmosc.wxRemotelyScrolledTreeCtrl_GetScrolledWindow,(self,) + _args, _kwargs)
        if val: val = wxScrolledWindowPtr(val) 
        return val
    def ScrollToLine(self, *_args, **_kwargs):
        val = apply(gizmosc.wxRemotelyScrolledTreeCtrl_ScrollToLine,(self,) + _args, _kwargs)
        return val
    def SetCompanionWindow(self, *_args, **_kwargs):
        val = apply(gizmosc.wxRemotelyScrolledTreeCtrl_SetCompanionWindow,(self,) + _args, _kwargs)
        return val
    def GetCompanionWindow(self, *_args, **_kwargs):
        val = apply(gizmosc.wxRemotelyScrolledTreeCtrl_GetCompanionWindow,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxRemotelyScrolledTreeCtrl instance at %s>" % (self.this,)
class wxRemotelyScrolledTreeCtrl(wxRemotelyScrolledTreeCtrlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gizmosc.new_wxRemotelyScrolledTreeCtrl,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxTreeCompanionWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = apply(gizmosc.wxTreeCompanionWindow__setCallbackInfo,(self,) + _args, _kwargs)
        return val
    def GetTreeCtrl(self, *_args, **_kwargs):
        val = apply(gizmosc.wxTreeCompanionWindow_GetTreeCtrl,(self,) + _args, _kwargs)
        if val: val = wxRemotelyScrolledTreeCtrlPtr(val) 
        return val
    def SetTreeCtrl(self, *_args, **_kwargs):
        val = apply(gizmosc.wxTreeCompanionWindow_SetTreeCtrl,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxTreeCompanionWindow instance at %s>" % (self.this,)
class wxTreeCompanionWindow(wxTreeCompanionWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gizmosc.new_wxTreeCompanionWindow,_args,_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxTreeCompanionWindow)
        self._setOORInfo(self)




class wxThinSplitterWindowPtr(wxSplitterWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxThinSplitterWindow instance at %s>" % (self.this,)
class wxThinSplitterWindow(wxThinSplitterWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gizmosc.new_wxThinSplitterWindow,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxSplitterScrolledWindowPtr(wxScrolledWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxSplitterScrolledWindow instance at %s>" % (self.this,)
class wxSplitterScrolledWindow(wxSplitterScrolledWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gizmosc.new_wxSplitterScrolledWindow,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxLEDNumberCtrlPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = apply(gizmosc.wxLEDNumberCtrl_Create,(self,) + _args, _kwargs)
        return val
    def GetAlignment(self, *_args, **_kwargs):
        val = apply(gizmosc.wxLEDNumberCtrl_GetAlignment,(self,) + _args, _kwargs)
        return val
    def GetDrawFaded(self, *_args, **_kwargs):
        val = apply(gizmosc.wxLEDNumberCtrl_GetDrawFaded,(self,) + _args, _kwargs)
        return val
    def GetValue(self, *_args, **_kwargs):
        val = apply(gizmosc.wxLEDNumberCtrl_GetValue,(self,) + _args, _kwargs)
        return val
    def SetAlignment(self, *_args, **_kwargs):
        val = apply(gizmosc.wxLEDNumberCtrl_SetAlignment,(self,) + _args, _kwargs)
        return val
    def SetDrawFaded(self, *_args, **_kwargs):
        val = apply(gizmosc.wxLEDNumberCtrl_SetDrawFaded,(self,) + _args, _kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = apply(gizmosc.wxLEDNumberCtrl_SetValue,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxLEDNumberCtrl instance at %s>" % (self.this,)
class wxLEDNumberCtrl(wxLEDNumberCtrlPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gizmosc.new_wxLEDNumberCtrl,_args,_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreLEDNumberCtrl(*_args,**_kwargs):
    val = wxLEDNumberCtrlPtr(apply(gizmosc.new_wxPreLEDNumberCtrl,_args,_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val




#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxEVT_DYNAMIC_SASH_SPLIT = gizmosc.wxEVT_DYNAMIC_SASH_SPLIT
wxEVT_DYNAMIC_SASH_UNIFY = gizmosc.wxEVT_DYNAMIC_SASH_UNIFY
wxDS_MANAGE_SCROLLBARS = gizmosc.wxDS_MANAGE_SCROLLBARS
wxDS_DRAG_CORNER = gizmosc.wxDS_DRAG_CORNER
wxEL_ALLOW_NEW = gizmosc.wxEL_ALLOW_NEW
wxEL_ALLOW_EDIT = gizmosc.wxEL_ALLOW_EDIT
wxEL_ALLOW_DELETE = gizmosc.wxEL_ALLOW_DELETE
wxLED_ALIGN_LEFT = gizmosc.wxLED_ALIGN_LEFT
wxLED_ALIGN_RIGHT = gizmosc.wxLED_ALIGN_RIGHT
wxLED_ALIGN_CENTER = gizmosc.wxLED_ALIGN_CENTER
wxLED_ALIGN_MASK = gizmosc.wxLED_ALIGN_MASK
wxLED_DRAW_FADED = gizmosc.wxLED_DRAW_FADED


#-------------- USER INCLUDE -----------------------

# Stuff these names into the wx namespace so wxPyConstructObject can find them

wx.wxDynamicSashSplitEventPtr       = wxDynamicSashSplitEventPtr
wx.wxDynamicSashUnifyEventPtr       = wxDynamicSashUnifyEventPtr
wx.wxDynamicSashWindowPtr           = wxDynamicSashWindowPtr

wx.wxEditableListBoxPtr             = wxEditableListBoxPtr

wx.wxRemotelyScrolledTreeCtrlPtr    = wxRemotelyScrolledTreeCtrlPtr
wx.wxTreeCompanionWindowPtr         = wxTreeCompanionWindowPtr
wx.wxThinSplitterWindowPtr          = wxThinSplitterWindowPtr
wx.wxSplitterScrolledWindowPtr      = wxSplitterScrolledWindowPtr
