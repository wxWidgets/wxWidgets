# This file was created automatically by SWIG.
import gizmosc

from misc import *

from misc2 import *

from windows import *

from gdi import *

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




#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxEVT_DYNAMIC_SASH_SPLIT = gizmosc.wxEVT_DYNAMIC_SASH_SPLIT
wxEVT_DYNAMIC_SASH_UNIFY = gizmosc.wxEVT_DYNAMIC_SASH_UNIFY
wxDS_MANAGE_SCROLLBARS = gizmosc.wxDS_MANAGE_SCROLLBARS
wxDS_DRAG_CORNER = gizmosc.wxDS_DRAG_CORNER


#-------------- USER INCLUDE -----------------------

# Stuff these names into the wx namespace so wxPyConstructObject can find them

wx.wxDynamicSashSplitEventPtr       = wxDynamicSashSplitEventPtr
wx.wxDynamicSashUnifyEventPtr       = wxDynamicSashUnifyEventPtr
wx.wxDynamicSashWindowPtr           = wxDynamicSashWindowPtr
