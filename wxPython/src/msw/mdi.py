# This file was created automatically by SWIG.
import mdic

from misc import *

from windows import *

from gdi import *

from fonts import *

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
    def Create(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_Create(self, *_args, **_kwargs)
        return val
    def ActivateNext(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_ActivateNext(self, *_args, **_kwargs)
        return val
    def ActivatePrevious(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_ActivatePrevious(self, *_args, **_kwargs)
        return val
    def ArrangeIcons(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_ArrangeIcons(self, *_args, **_kwargs)
        return val
    def Cascade(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_Cascade(self, *_args, **_kwargs)
        return val
    def GetActiveChild(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_GetActiveChild(self, *_args, **_kwargs)
        return val
    def GetClientWindow(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_GetClientWindow(self, *_args, **_kwargs)
        return val
    def GetToolBar(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_GetToolBar(self, *_args, **_kwargs)
        return val
    def GetWindowMenu(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_GetWindowMenu(self, *_args, **_kwargs)
        return val
    def SetWindowMenu(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_SetWindowMenu(self, *_args, **_kwargs)
        return val
    def SetToolBar(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_SetToolBar(self, *_args, **_kwargs)
        return val
    def Tile(self, *_args, **_kwargs):
        val = mdic.wxMDIParentFrame_Tile(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxMDIParentFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxMDIParentFrame(wxMDIParentFramePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = mdic.new_wxMDIParentFrame(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreMDIParentFrame(*_args,**_kwargs):
    val = wxMDIParentFramePtr(mdic.new_wxPreMDIParentFrame(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxMDIChildFramePtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = mdic.wxMDIChildFrame_Create(self, *_args, **_kwargs)
        return val
    def Activate(self, *_args, **_kwargs):
        val = mdic.wxMDIChildFrame_Activate(self, *_args, **_kwargs)
        return val
    def Maximize(self, *_args, **_kwargs):
        val = mdic.wxMDIChildFrame_Maximize(self, *_args, **_kwargs)
        return val
    def Restore(self, *_args, **_kwargs):
        val = mdic.wxMDIChildFrame_Restore(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxMDIChildFrame instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxMDIChildFrame(wxMDIChildFramePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = mdic.new_wxMDIChildFrame(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreMDIChildFrame(*_args,**_kwargs):
    val = wxMDIChildFramePtr(mdic.new_wxPreMDIChildFrame(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxMDIClientWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = mdic.wxMDIClientWindow_Create(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxMDIClientWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxMDIClientWindow(wxMDIClientWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = mdic.new_wxMDIClientWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreMDIClientWindow(*_args,**_kwargs):
    val = wxMDIClientWindowPtr(mdic.new_wxPreMDIClientWindow(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val




#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

IDM_WINDOWTILE = mdic.IDM_WINDOWTILE
IDM_WINDOWTILEHOR = mdic.IDM_WINDOWTILEHOR
IDM_WINDOWCASCADE = mdic.IDM_WINDOWCASCADE
IDM_WINDOWICONS = mdic.IDM_WINDOWICONS
IDM_WINDOWNEXT = mdic.IDM_WINDOWNEXT
IDM_WINDOWTILEVERT = mdic.IDM_WINDOWTILEVERT
wxFIRST_MDI_CHILD = mdic.wxFIRST_MDI_CHILD
wxLAST_MDI_CHILD = mdic.wxLAST_MDI_CHILD
