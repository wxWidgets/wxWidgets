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
    def __del__(self, lseditorc=lseditorc):
        if self.thisown == 1 :
            lseditorc.delete_wxsLSEditorPlugin(self.this)
    def Create(self,arg0,arg1):
        val = lseditorc.wxsLSEditorPlugin_Create(self.this,arg0.this,arg1)
        return val
    def OnOpen(self,arg0):
        val = lseditorc.wxsLSEditorPlugin_OnOpen(self.this,arg0)
        return val
    def OnSave(self,arg0):
        val = lseditorc.wxsLSEditorPlugin_OnSave(self.this,arg0)
        return val
    def OnCopy(self):
        val = lseditorc.wxsLSEditorPlugin_OnCopy(self.this)
        return val
    def OnCut(self):
        val = lseditorc.wxsLSEditorPlugin_OnCut(self.this)
        return val
    def OnPaste(self):
        val = lseditorc.wxsLSEditorPlugin_OnPaste(self.this)
        return val
    def OnDelete(self):
        val = lseditorc.wxsLSEditorPlugin_OnDelete(self.this)
        return val
    def OnUndo(self):
        val = lseditorc.wxsLSEditorPlugin_OnUndo(self.this)
        return val
    def OnRedo(self):
        val = lseditorc.wxsLSEditorPlugin_OnRedo(self.this)
        return val
    def SelectAll(self):
        val = lseditorc.wxsLSEditorPlugin_SelectAll(self.this)
        return val
    def OnGotoLine(self,arg0,*args):
        val = apply(lseditorc.wxsLSEditorPlugin_OnGotoLine,(self.this,arg0,)+args)
        return val
    def OnProperties(self):
        val = lseditorc.wxsLSEditorPlugin_OnProperties(self.this)
        return val
    def OnFind(self):
        val = lseditorc.wxsLSEditorPlugin_OnFind(self.this)
        return val
    def OnFindNext(self):
        val = lseditorc.wxsLSEditorPlugin_OnFindNext(self.this)
        return val
    def OnFindPrevious(self):
        val = lseditorc.wxsLSEditorPlugin_OnFindPrevious(self.this)
        return val
    def OnReplace(self):
        val = lseditorc.wxsLSEditorPlugin_OnReplace(self.this)
        return val
    def OnToggleBookmark(self):
        val = lseditorc.wxsLSEditorPlugin_OnToggleBookmark(self.this)
        return val
    def OnNextBookmark(self):
        val = lseditorc.wxsLSEditorPlugin_OnNextBookmark(self.this)
        return val
    def OnPreviousBookmark(self):
        val = lseditorc.wxsLSEditorPlugin_OnPreviousBookmark(self.this)
        return val
    def OnShowBookmarks(self):
        val = lseditorc.wxsLSEditorPlugin_OnShowBookmarks(self.this)
        return val
    def SetCheckpoint(self):
        val = lseditorc.wxsLSEditorPlugin_SetCheckpoint(self.this)
        return val
    def CheckpointModified(self):
        val = lseditorc.wxsLSEditorPlugin_CheckpointModified(self.this)
        return val
    def CanCopy(self):
        val = lseditorc.wxsLSEditorPlugin_CanCopy(self.this)
        return val
    def CanCut(self):
        val = lseditorc.wxsLSEditorPlugin_CanCut(self.this)
        return val
    def CanPaste(self):
        val = lseditorc.wxsLSEditorPlugin_CanPaste(self.this)
        return val
    def CanUndo(self):
        val = lseditorc.wxsLSEditorPlugin_CanUndo(self.this)
        return val
    def CanRedo(self):
        val = lseditorc.wxsLSEditorPlugin_CanRedo(self.this)
        return val
    def GetName(self):
        val = lseditorc.wxsLSEditorPlugin_GetName(self.this)
        return val
    def IsModified(self):
        val = lseditorc.wxsLSEditorPlugin_IsModified(self.this)
        return val
    def GetWindow(self):
        val = lseditorc.wxsLSEditorPlugin_GetWindow(self.this)
        val = wxWindowPtr(val)
        return val
    def SetFileName(self,arg0):
        val = lseditorc.wxsLSEditorPlugin_SetFileName(self.this,arg0)
        return val
    def HoldCursor(self,arg0):
        val = lseditorc.wxsLSEditorPlugin_HoldCursor(self.this,arg0)
        return val
    def FindWordAtCursor(self):
        val = lseditorc.wxsLSEditorPlugin_FindWordAtCursor(self.this)
        return val
    def GetCursorPos(self,arg0,arg1):
        val = lseditorc.wxsLSEditorPlugin_GetCursorPos(self.this,arg0,arg1)
        return val
    def SetCursorPos(self,arg0,arg1):
        val = lseditorc.wxsLSEditorPlugin_SetCursorPos(self.this,arg0,arg1)
        return val
    def GetPagePos(self,arg0,arg1):
        val = lseditorc.wxsLSEditorPlugin_GetPagePos(self.this,arg0,arg1)
        return val
    def GetText(self,arg0,arg1,arg2,arg3,arg4,arg5):
        val = lseditorc.wxsLSEditorPlugin_GetText(self.this,arg0,arg1,arg2,arg3,arg4,arg5)
        return val
    def InsertText(self,arg0,arg1,arg2,arg3):
        val = lseditorc.wxsLSEditorPlugin_InsertText(self.this,arg0,arg1,arg2,arg3)
        return val
    def DeleteText(self,arg0,arg1,arg2,arg3):
        val = lseditorc.wxsLSEditorPlugin_DeleteText(self.this,arg0,arg1,arg2,arg3)
        return val
    def PositionToXY(self,arg0,arg1,arg2,arg3):
        val = lseditorc.wxsLSEditorPlugin_PositionToXY(self.this,arg0,arg1,arg2,arg3)
        return val
    def GetSelectionRange(self,arg0,arg1,arg2,arg3):
        val = lseditorc.wxsLSEditorPlugin_GetSelectionRange(self.this,arg0,arg1,arg2,arg3)
        return val
    def GetCharacterSize(self):
        val = lseditorc.wxsLSEditorPlugin_GetCharacterSize(self.this)
        val = wxSizePtr(val)
        val.thisown = 1
        return val
    def IsUnixText(self):
        val = lseditorc.wxsLSEditorPlugin_IsUnixText(self.this)
        return val
    def __repr__(self):
        return "<C wxsLSEditorPlugin instance>"
class wxsLSEditorPlugin(wxsLSEditorPluginPtr):
    def __init__(self) :
        self.this = lseditorc.new_wxsLSEditorPlugin()
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

