# This file was created automatically by SWIG.
import windows3c

from misc import *

from gdi import *

from fonts import *

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
        val = windows3c.wxSashEvent_SetEdge(self, *_args, **_kwargs)
        return val
    def GetEdge(self, *_args, **_kwargs):
        val = windows3c.wxSashEvent_GetEdge(self, *_args, **_kwargs)
        return val
    def SetDragRect(self, *_args, **_kwargs):
        val = windows3c.wxSashEvent_SetDragRect(self, *_args, **_kwargs)
        return val
    def GetDragRect(self, *_args, **_kwargs):
        val = windows3c.wxSashEvent_GetDragRect(self, *_args, **_kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def SetDragStatus(self, *_args, **_kwargs):
        val = windows3c.wxSashEvent_SetDragStatus(self, *_args, **_kwargs)
        return val
    def GetDragStatus(self, *_args, **_kwargs):
        val = windows3c.wxSashEvent_GetDragStatus(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxSashEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxSashEvent(wxSashEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows3c.new_wxSashEvent(*_args,**_kwargs)
        self.thisown = 1




class wxSashWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_Create(self, *_args, **_kwargs)
        return val
    def GetSashVisible(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_GetSashVisible(self, *_args, **_kwargs)
        return val
    def GetDefaultBorderSize(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_GetDefaultBorderSize(self, *_args, **_kwargs)
        return val
    def GetEdgeMargin(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_GetEdgeMargin(self, *_args, **_kwargs)
        return val
    def GetExtraBorderSize(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_GetExtraBorderSize(self, *_args, **_kwargs)
        return val
    def GetMaximumSizeX(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_GetMaximumSizeX(self, *_args, **_kwargs)
        return val
    def GetMaximumSizeY(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_GetMaximumSizeY(self, *_args, **_kwargs)
        return val
    def GetMinimumSizeX(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_GetMinimumSizeX(self, *_args, **_kwargs)
        return val
    def GetMinimumSizeY(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_GetMinimumSizeY(self, *_args, **_kwargs)
        return val
    def HasBorder(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_HasBorder(self, *_args, **_kwargs)
        return val
    def SetDefaultBorderSize(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_SetDefaultBorderSize(self, *_args, **_kwargs)
        return val
    def SetExtraBorderSize(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_SetExtraBorderSize(self, *_args, **_kwargs)
        return val
    def SetMaximumSizeX(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_SetMaximumSizeX(self, *_args, **_kwargs)
        return val
    def SetMaximumSizeY(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_SetMaximumSizeY(self, *_args, **_kwargs)
        return val
    def SetMinimumSizeX(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_SetMinimumSizeX(self, *_args, **_kwargs)
        return val
    def SetMinimumSizeY(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_SetMinimumSizeY(self, *_args, **_kwargs)
        return val
    def SetSashVisible(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_SetSashVisible(self, *_args, **_kwargs)
        return val
    def SetSashBorder(self, *_args, **_kwargs):
        val = windows3c.wxSashWindow_SetSashBorder(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxSashWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxSashWindow(wxSashWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows3c.new_wxSashWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreSashWindow(*_args,**_kwargs):
    val = wxSashWindowPtr(windows3c.new_wxPreSashWindow(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxQueryLayoutInfoEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetRequestedLength(self, *_args, **_kwargs):
        val = windows3c.wxQueryLayoutInfoEvent_SetRequestedLength(self, *_args, **_kwargs)
        return val
    def GetRequestedLength(self, *_args, **_kwargs):
        val = windows3c.wxQueryLayoutInfoEvent_GetRequestedLength(self, *_args, **_kwargs)
        return val
    def SetFlags(self, *_args, **_kwargs):
        val = windows3c.wxQueryLayoutInfoEvent_SetFlags(self, *_args, **_kwargs)
        return val
    def GetFlags(self, *_args, **_kwargs):
        val = windows3c.wxQueryLayoutInfoEvent_GetFlags(self, *_args, **_kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = windows3c.wxQueryLayoutInfoEvent_SetSize(self, *_args, **_kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = windows3c.wxQueryLayoutInfoEvent_GetSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def SetOrientation(self, *_args, **_kwargs):
        val = windows3c.wxQueryLayoutInfoEvent_SetOrientation(self, *_args, **_kwargs)
        return val
    def GetOrientation(self, *_args, **_kwargs):
        val = windows3c.wxQueryLayoutInfoEvent_GetOrientation(self, *_args, **_kwargs)
        return val
    def SetAlignment(self, *_args, **_kwargs):
        val = windows3c.wxQueryLayoutInfoEvent_SetAlignment(self, *_args, **_kwargs)
        return val
    def GetAlignment(self, *_args, **_kwargs):
        val = windows3c.wxQueryLayoutInfoEvent_GetAlignment(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxQueryLayoutInfoEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxQueryLayoutInfoEvent(wxQueryLayoutInfoEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows3c.new_wxQueryLayoutInfoEvent(*_args,**_kwargs)
        self.thisown = 1




class wxCalculateLayoutEventPtr(wxEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetFlags(self, *_args, **_kwargs):
        val = windows3c.wxCalculateLayoutEvent_SetFlags(self, *_args, **_kwargs)
        return val
    def GetFlags(self, *_args, **_kwargs):
        val = windows3c.wxCalculateLayoutEvent_GetFlags(self, *_args, **_kwargs)
        return val
    def SetRect(self, *_args, **_kwargs):
        val = windows3c.wxCalculateLayoutEvent_SetRect(self, *_args, **_kwargs)
        return val
    def GetRect(self, *_args, **_kwargs):
        val = windows3c.wxCalculateLayoutEvent_GetRect(self, *_args, **_kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxCalculateLayoutEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxCalculateLayoutEvent(wxCalculateLayoutEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows3c.new_wxCalculateLayoutEvent(*_args,**_kwargs)
        self.thisown = 1




class wxSashLayoutWindowPtr(wxSashWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Create(self, *_args, **_kwargs):
        val = windows3c.wxSashLayoutWindow_Create(self, *_args, **_kwargs)
        return val
    def GetAlignment(self, *_args, **_kwargs):
        val = windows3c.wxSashLayoutWindow_GetAlignment(self, *_args, **_kwargs)
        return val
    def GetOrientation(self, *_args, **_kwargs):
        val = windows3c.wxSashLayoutWindow_GetOrientation(self, *_args, **_kwargs)
        return val
    def SetAlignment(self, *_args, **_kwargs):
        val = windows3c.wxSashLayoutWindow_SetAlignment(self, *_args, **_kwargs)
        return val
    def SetDefaultSize(self, *_args, **_kwargs):
        val = windows3c.wxSashLayoutWindow_SetDefaultSize(self, *_args, **_kwargs)
        return val
    def SetOrientation(self, *_args, **_kwargs):
        val = windows3c.wxSashLayoutWindow_SetOrientation(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxSashLayoutWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxSashLayoutWindow(wxSashLayoutWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows3c.new_wxSashLayoutWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)



def wxPreSashLayoutWindow(*_args,**_kwargs):
    val = wxSashLayoutWindowPtr(windows3c.new_wxPreSashLayoutWindow(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxLayoutAlgorithmPtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=windows3c.delete_wxLayoutAlgorithm):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def LayoutMDIFrame(self, *_args, **_kwargs):
        val = windows3c.wxLayoutAlgorithm_LayoutMDIFrame(self, *_args, **_kwargs)
        return val
    def LayoutFrame(self, *_args, **_kwargs):
        val = windows3c.wxLayoutAlgorithm_LayoutFrame(self, *_args, **_kwargs)
        return val
    def LayoutWindow(self, *_args, **_kwargs):
        val = windows3c.wxLayoutAlgorithm_LayoutWindow(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxLayoutAlgorithm instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxLayoutAlgorithm(wxLayoutAlgorithmPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows3c.new_wxLayoutAlgorithm(*_args,**_kwargs)
        self.thisown = 1




class wxTipWindowPtr(wxFramePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetBoundingRect(self, *_args, **_kwargs):
        val = windows3c.wxTipWindow_SetBoundingRect(self, *_args, **_kwargs)
        return val
    def Close(self, *_args, **_kwargs):
        val = windows3c.wxTipWindow_Close(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxTipWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxTipWindow(wxTipWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows3c.new_wxTipWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxVScrolledWindowPtr(wxPanelPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def Create(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_Create(self, *_args, **_kwargs)
        return val
    def SetLineCount(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_SetLineCount(self, *_args, **_kwargs)
        return val
    def ScrollToLine(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_ScrollToLine(self, *_args, **_kwargs)
        return val
    def ScrollLines(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_ScrollLines(self, *_args, **_kwargs)
        return val
    def ScrollPages(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_ScrollPages(self, *_args, **_kwargs)
        return val
    def RefreshLine(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_RefreshLine(self, *_args, **_kwargs)
        return val
    def RefreshLines(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_RefreshLines(self, *_args, **_kwargs)
        return val
    def HitTestXT(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_HitTestXT(self, *_args, **_kwargs)
        return val
    def HitTest(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_HitTest(self, *_args, **_kwargs)
        return val
    def RefreshAll(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_RefreshAll(self, *_args, **_kwargs)
        return val
    def GetLineCount(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_GetLineCount(self, *_args, **_kwargs)
        return val
    def GetFirstVisibleLine(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_GetFirstVisibleLine(self, *_args, **_kwargs)
        return val
    def GetLastVisibleLine(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_GetLastVisibleLine(self, *_args, **_kwargs)
        return val
    def IsVisible(self, *_args, **_kwargs):
        val = windows3c.wxVScrolledWindow_IsVisible(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxVScrolledWindow instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxVScrolledWindow(wxVScrolledWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows3c.new_wxVScrolledWindow(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxVScrolledWindow)
        self._setOORInfo(self)



def wxPreVScrolledWindow(*_args,**_kwargs):
    val = wxVScrolledWindowPtr(windows3c.new_wxPreVScrolledWindow(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxVListBoxPtr(wxVScrolledWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = windows3c.wxVListBox__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def Create(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_Create(self, *_args, **_kwargs)
        return val
    def GetItemCount(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_GetItemCount(self, *_args, **_kwargs)
        return val
    def HasMultipleSelection(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_HasMultipleSelection(self, *_args, **_kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_GetSelection(self, *_args, **_kwargs)
        return val
    def IsCurrent(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_IsCurrent(self, *_args, **_kwargs)
        return val
    def IsSelected(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_IsSelected(self, *_args, **_kwargs)
        return val
    def GetSelectedCount(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_GetSelectedCount(self, *_args, **_kwargs)
        return val
    def GetFirstSelected(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_GetFirstSelected(self, *_args, **_kwargs)
        return val
    def GetNextSelected(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_GetNextSelected(self, *_args, **_kwargs)
        return val
    def GetMargins(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_GetMargins(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def GetSelectionBackground(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_GetSelectionBackground(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def SetItemCount(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_SetItemCount(self, *_args, **_kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_Clear(self, *_args, **_kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_SetSelection(self, *_args, **_kwargs)
        return val
    def Select(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_Select(self, *_args, **_kwargs)
        return val
    def SelectRange(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_SelectRange(self, *_args, **_kwargs)
        return val
    def Toggle(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_Toggle(self, *_args, **_kwargs)
        return val
    def SelectAll(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_SelectAll(self, *_args, **_kwargs)
        return val
    def DeselectAll(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_DeselectAll(self, *_args, **_kwargs)
        return val
    def SetMargins(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_SetMargins(self, *_args, **_kwargs)
        return val
    def SetMarginsXY(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_SetMarginsXY(self, *_args, **_kwargs)
        return val
    def SetSelectionBackground(self, *_args, **_kwargs):
        val = windows3c.wxVListBox_SetSelectionBackground(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxVListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxVListBox(wxVListBoxPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows3c.new_wxVListBox(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxVListBox)
        self._setOORInfo(self)



def wxPreVListBox(*_args,**_kwargs):
    val = wxVListBoxPtr(windows3c.new_wxPreVListBox(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val


class wxHtmlListBoxPtr(wxVListBoxPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = windows3c.wxHtmlListBox__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def Create(self, *_args, **_kwargs):
        val = windows3c.wxHtmlListBox_Create(self, *_args, **_kwargs)
        return val
    def RefreshAll(self, *_args, **_kwargs):
        val = windows3c.wxHtmlListBox_RefreshAll(self, *_args, **_kwargs)
        return val
    def SetItemCount(self, *_args, **_kwargs):
        val = windows3c.wxHtmlListBox_SetItemCount(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxHtmlListBox instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxHtmlListBox(wxHtmlListBoxPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = windows3c.new_wxHtmlListBox(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxHtmlListBox)
        self._setOORInfo(self)



def wxPreHtmlListBox(*_args,**_kwargs):
    val = wxHtmlListBoxPtr(windows3c.new_wxPreHtmlListBox(*_args,**_kwargs))
    val.thisown = 1
    val._setOORInfo(val)
    return val




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
