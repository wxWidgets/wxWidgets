# This file was created automatically by SWIG.
import windows2c

from misc import *

from gdi import *

from windows import *

from clip_dnd import *

from controls import *

from events import *
import wx
class wxNotebookEventPtr(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebookEvent_GetSelection,(self,) + _args, _kwargs)
        return val
    def GetOldSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebookEvent_GetOldSelection,(self,) + _args, _kwargs)
        return val
    def SetOldSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebookEvent_SetOldSelection,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebookEvent_SetSelection,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxNotebookEvent instance at %s>" % (self.this,)
class wxNotebookEvent(wxNotebookEventPtr):
    def __init__(self,this):
        self.this = this




class wxNotebookPtr(wxControlPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetPageCount(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetPageCount,(self,) + _args, _kwargs)
        return val
    def SetSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_SetSelection,(self,) + _args, _kwargs)
        return val
    def AdvanceSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_AdvanceSelection,(self,) + _args, _kwargs)
        return val
    def GetSelection(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetSelection,(self,) + _args, _kwargs)
        return val
    def SetPageText(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_SetPageText,(self,) + _args, _kwargs)
        return val
    def GetPageText(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetPageText,(self,) + _args, _kwargs)
        return val
    def SetImageList(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_SetImageList,(self,) + _args, _kwargs)
        return val
    def GetImageList(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetImageList,(self,) + _args, _kwargs)
        if val: val = wxImageListPtr(val) 
        return val
    def GetPageImage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetPageImage,(self,) + _args, _kwargs)
        return val
    def SetPageImage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_SetPageImage,(self,) + _args, _kwargs)
        return val
    def GetRowCount(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetRowCount,(self,) + _args, _kwargs)
        return val
    def DeletePage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_DeletePage,(self,) + _args, _kwargs)
        return val
    def RemovePage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_RemovePage,(self,) + _args, _kwargs)
        return val
    def DeleteAllPages(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_DeleteAllPages,(self,) + _args, _kwargs)
        return val
    def AddPage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_AddPage,(self,) + _args, _kwargs)
        return val
    def InsertPage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_InsertPage,(self,) + _args, _kwargs)
        return val
    def GetPage(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_GetPage,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def ResizeChildren(self, *_args, **_kwargs):
        val = apply(windows2c.wxNotebook_ResizeChildren,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxNotebook instance at %s>" % (self.this,)
class wxNotebook(wxNotebookPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows2c.new_wxNotebook,_args,_kwargs)
        self.thisown = 1
        #wx._StdWindowCallbacks(self)




class wxSplitterEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetSashPosition(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterEvent_GetSashPosition,(self,) + _args, _kwargs)
        return val
    def GetX(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterEvent_GetX,(self,) + _args, _kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterEvent_GetY,(self,) + _args, _kwargs)
        return val
    def GetWindowBeingRemoved(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterEvent_GetWindowBeingRemoved,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def SetSashPosition(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterEvent_SetSashPosition,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSplitterEvent instance at %s>" % (self.this,)
class wxSplitterEvent(wxSplitterEventPtr):
    def __init__(self,this):
        self.this = this




class wxSplitterWindowPtr(wxWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetBorderSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetBorderSize,(self,) + _args, _kwargs)
        return val
    def GetMinimumPaneSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetMinimumPaneSize,(self,) + _args, _kwargs)
        return val
    def GetSashPosition(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetSashPosition,(self,) + _args, _kwargs)
        return val
    def GetSashSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetSashSize,(self,) + _args, _kwargs)
        return val
    def GetSplitMode(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetSplitMode,(self,) + _args, _kwargs)
        return val
    def GetWindow1(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetWindow1,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def GetWindow2(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_GetWindow2,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def Initialize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_Initialize,(self,) + _args, _kwargs)
        return val
    def IsSplit(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_IsSplit,(self,) + _args, _kwargs)
        return val
    def ReplaceWindow(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_ReplaceWindow,(self,) + _args, _kwargs)
        return val
    def SetBorderSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SetBorderSize,(self,) + _args, _kwargs)
        return val
    def SetSashPosition(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SetSashPosition,(self,) + _args, _kwargs)
        return val
    def SetSashSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SetSashSize,(self,) + _args, _kwargs)
        return val
    def SetMinimumPaneSize(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SetMinimumPaneSize,(self,) + _args, _kwargs)
        return val
    def SetSplitMode(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SetSplitMode,(self,) + _args, _kwargs)
        return val
    def SplitHorizontally(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SplitHorizontally,(self,) + _args, _kwargs)
        return val
    def SplitVertically(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_SplitVertically,(self,) + _args, _kwargs)
        return val
    def Unsplit(self, *_args, **_kwargs):
        val = apply(windows2c.wxSplitterWindow_Unsplit,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxSplitterWindow instance at %s>" % (self.this,)
class wxSplitterWindow(wxSplitterWindowPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows2c.new_wxSplitterWindow,_args,_kwargs)
        self.thisown = 1
        #wx._StdWindowCallbacks(self)




class wxTaskBarIconPtr(wxEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,windows2c=windows2c):
        if self.thisown == 1 :
            windows2c.delete_wxTaskBarIcon(self)
    def SetIcon(self, *_args, **_kwargs):
        val = apply(windows2c.wxTaskBarIcon_SetIcon,(self,) + _args, _kwargs)
        return val
    def RemoveIcon(self, *_args, **_kwargs):
        val = apply(windows2c.wxTaskBarIcon_RemoveIcon,(self,) + _args, _kwargs)
        return val
    def PopupMenu(self, *_args, **_kwargs):
        val = apply(windows2c.wxTaskBarIcon_PopupMenu,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxTaskBarIcon instance at %s>" % (self.this,)
class wxTaskBarIcon(wxTaskBarIconPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(windows2c.new_wxTaskBarIcon,_args,_kwargs)
        self.thisown = 1
        wx._checkForCallback(self, 'OnMouseMove',    wxEVT_TASKBAR_MOVE)
        wx._checkForCallback(self, 'OnLButtonDown',  wxEVT_TASKBAR_LEFT_DOWN)
        wx._checkForCallback(self, 'OnLButtonUp',    wxEVT_TASKBAR_LEFT_UP)
        wx._checkForCallback(self, 'OnRButtonDown',  wxEVT_TASKBAR_RIGHT_DOWN)
        wx._checkForCallback(self, 'OnRButtonUp',    wxEVT_TASKBAR_RIGHT_UP)
        wx._checkForCallback(self, 'OnLButtonDClick',wxEVT_TASKBAR_LEFT_DCLICK)
        wx._checkForCallback(self, 'OnRButtonDClick',wxEVT_TASKBAR_RIGHT_DCLICK)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxSPLIT_HORIZONTAL = windows2c.wxSPLIT_HORIZONTAL
wxSPLIT_VERTICAL = windows2c.wxSPLIT_VERTICAL
wxSPLIT_DRAG_NONE = windows2c.wxSPLIT_DRAG_NONE
wxSPLIT_DRAG_DRAGGING = windows2c.wxSPLIT_DRAG_DRAGGING
wxSPLIT_DRAG_LEFT_DOWN = windows2c.wxSPLIT_DRAG_LEFT_DOWN
wxEVT_TASKBAR_MOVE = windows2c.wxEVT_TASKBAR_MOVE
wxEVT_TASKBAR_LEFT_DOWN = windows2c.wxEVT_TASKBAR_LEFT_DOWN
wxEVT_TASKBAR_LEFT_UP = windows2c.wxEVT_TASKBAR_LEFT_UP
wxEVT_TASKBAR_RIGHT_DOWN = windows2c.wxEVT_TASKBAR_RIGHT_DOWN
wxEVT_TASKBAR_RIGHT_UP = windows2c.wxEVT_TASKBAR_RIGHT_UP
wxEVT_TASKBAR_LEFT_DCLICK = windows2c.wxEVT_TASKBAR_LEFT_DCLICK
wxEVT_TASKBAR_RIGHT_DCLICK = windows2c.wxEVT_TASKBAR_RIGHT_DCLICK
