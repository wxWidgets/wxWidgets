# This file was created automatically by SWIG.
import lseditorc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from events import *

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
import wx
class wxsLSEditorPluginPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,lseditorc=lseditorc):
        if self.thisown == 1 :
            lseditorc.delete_wxsLSEditorPlugin(self)
    def Create(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_Create,(self,) + _args, _kwargs)
        return val
    def OnOpen(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnOpen,(self,) + _args, _kwargs)
        return val
    def OnSave(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnSave,(self,) + _args, _kwargs)
        return val
    def OnCopy(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnCopy,(self,) + _args, _kwargs)
        return val
    def OnCut(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnCut,(self,) + _args, _kwargs)
        return val
    def OnPaste(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnPaste,(self,) + _args, _kwargs)
        return val
    def OnDelete(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnDelete,(self,) + _args, _kwargs)
        return val
    def OnUndo(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnUndo,(self,) + _args, _kwargs)
        return val
    def OnRedo(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnRedo,(self,) + _args, _kwargs)
        return val
    def SelectAll(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_SelectAll,(self,) + _args, _kwargs)
        return val
    def OnGotoLine(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnGotoLine,(self,) + _args, _kwargs)
        return val
    def OnProperties(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnProperties,(self,) + _args, _kwargs)
        return val
    def OnFind(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnFind,(self,) + _args, _kwargs)
        return val
    def OnFindNext(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnFindNext,(self,) + _args, _kwargs)
        return val
    def OnFindPrevious(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnFindPrevious,(self,) + _args, _kwargs)
        return val
    def OnReplace(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnReplace,(self,) + _args, _kwargs)
        return val
    def OnToggleBookmark(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnToggleBookmark,(self,) + _args, _kwargs)
        return val
    def OnNextBookmark(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnNextBookmark,(self,) + _args, _kwargs)
        return val
    def OnPreviousBookmark(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnPreviousBookmark,(self,) + _args, _kwargs)
        return val
    def OnShowBookmarks(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_OnShowBookmarks,(self,) + _args, _kwargs)
        return val
    def SetCheckpoint(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_SetCheckpoint,(self,) + _args, _kwargs)
        return val
    def CheckpointModified(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_CheckpointModified,(self,) + _args, _kwargs)
        return val
    def CanCopy(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_CanCopy,(self,) + _args, _kwargs)
        return val
    def CanCut(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_CanCut,(self,) + _args, _kwargs)
        return val
    def CanPaste(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_CanPaste,(self,) + _args, _kwargs)
        return val
    def CanUndo(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_CanUndo,(self,) + _args, _kwargs)
        return val
    def CanRedo(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_CanRedo,(self,) + _args, _kwargs)
        return val
    def GetName(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_GetName,(self,) + _args, _kwargs)
        return val
    def IsModified(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_IsModified,(self,) + _args, _kwargs)
        return val
    def GetWindow(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_GetWindow,(self,) + _args, _kwargs)
        if val: val = wxWindowPtr(val) 
        return val
    def SetFileName(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_SetFileName,(self,) + _args, _kwargs)
        return val
    def HoldCursor(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_HoldCursor,(self,) + _args, _kwargs)
        return val
    def FindWordAtCursor(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_FindWordAtCursor,(self,) + _args, _kwargs)
        return val
    def GetCursorPos(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_GetCursorPos,(self,) + _args, _kwargs)
        return val
    def SetCursorPos(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_SetCursorPos,(self,) + _args, _kwargs)
        return val
    def GetPagePos(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_GetPagePos,(self,) + _args, _kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_GetText,(self,) + _args, _kwargs)
        return val
    def InsertText(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_InsertText,(self,) + _args, _kwargs)
        return val
    def DeleteText(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_DeleteText,(self,) + _args, _kwargs)
        return val
    def PositionToXY(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_PositionToXY,(self,) + _args, _kwargs)
        return val
    def GetSelectionRange(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_GetSelectionRange,(self,) + _args, _kwargs)
        return val
    def GetCharacterSize(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_GetCharacterSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def IsUnixText(self, *_args, **_kwargs):
        val = apply(lseditorc.wxsLSEditorPlugin_IsUnixText,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxsLSEditorPlugin instance at %s>" % (self.this,)
class wxsLSEditorPlugin(wxsLSEditorPluginPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(lseditorc.new_wxsLSEditorPlugin,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

