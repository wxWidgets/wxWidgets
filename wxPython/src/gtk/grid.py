# This file was created automatically by SWIG.
import gridc

from misc import *

from gdi import *

from fonts import *

from windows import *

from clip_dnd import *

from controls import *

from events import *
import wx

def EVT_GRID_CELL_LEFT_CLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_LEFT_CLICK, fn)

def EVT_GRID_CELL_RIGHT_CLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_RIGHT_CLICK, fn)

def EVT_GRID_CELL_LEFT_DCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_LEFT_DCLICK, fn)

def EVT_GRID_CELL_RIGHT_DCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_RIGHT_DCLICK, fn)

def EVT_GRID_LABEL_LEFT_CLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_LABEL_LEFT_CLICK, fn)

def EVT_GRID_LABEL_RIGHT_CLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_LABEL_RIGHT_CLICK, fn)

def EVT_GRID_LABEL_LEFT_DCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_LABEL_LEFT_DCLICK, fn)

def EVT_GRID_LABEL_RIGHT_DCLICK(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_LABEL_RIGHT_DCLICK, fn)

def EVT_GRID_ROW_SIZE(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_ROW_SIZE, fn)

def EVT_GRID_COL_SIZE(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_COL_SIZE, fn)

def EVT_GRID_RANGE_SELECT(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_RANGE_SELECT, fn)

def EVT_GRID_CELL_CHANGE(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_CELL_CHANGE, fn)

def EVT_GRID_SELECT_CELL(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_SELECT_CELL, fn)

def EVT_GRID_EDITOR_SHOWN(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_EDITOR_SHOWN, fn)

def EVT_GRID_EDITOR_HIDDEN(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_EDITOR_HIDDEN, fn)

def EVT_GRID_EDITOR_CREATED(win, fn):
    win.Connect(-1, -1, wxEVT_GRID_EDITOR_CREATED, fn)


class wxGridCellRendererPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setOORInfo(self, *_args, **_kwargs):
        val = gridc.wxGridCellRenderer__setOORInfo(self, *_args, **_kwargs)
        return val
    def SetParameters(self, *_args, **_kwargs):
        val = gridc.wxGridCellRenderer_SetParameters(self, *_args, **_kwargs)
        return val
    def IncRef(self, *_args, **_kwargs):
        val = gridc.wxGridCellRenderer_IncRef(self, *_args, **_kwargs)
        return val
    def DecRef(self, *_args, **_kwargs):
        val = gridc.wxGridCellRenderer_DecRef(self, *_args, **_kwargs)
        return val
    def Draw(self, *_args, **_kwargs):
        val = gridc.wxGridCellRenderer_Draw(self, *_args, **_kwargs)
        return val
    def GetBestSize(self, *_args, **_kwargs):
        val = gridc.wxGridCellRenderer_GetBestSize(self, *_args, **_kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def Clone(self, *_args, **_kwargs):
        val = gridc.wxGridCellRenderer_Clone(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellRenderer(wxGridCellRendererPtr):
    def __init__(self,this):
        self.this = this




class wxPyGridCellRendererPtr(wxGridCellRendererPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellRenderer__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def base_SetParameters(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellRenderer_base_SetParameters(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPyGridCellRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPyGridCellRenderer(wxPyGridCellRendererPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxPyGridCellRenderer(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyGridCellRenderer)
        self._setOORInfo(self)




class wxGridCellStringRendererPtr(wxGridCellRendererPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellStringRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellStringRenderer(wxGridCellStringRendererPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellStringRenderer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellNumberRendererPtr(wxGridCellStringRendererPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellNumberRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellNumberRenderer(wxGridCellNumberRendererPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellNumberRenderer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellFloatRendererPtr(wxGridCellStringRendererPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetWidth(self, *_args, **_kwargs):
        val = gridc.wxGridCellFloatRenderer_GetWidth(self, *_args, **_kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = gridc.wxGridCellFloatRenderer_SetWidth(self, *_args, **_kwargs)
        return val
    def GetPrecision(self, *_args, **_kwargs):
        val = gridc.wxGridCellFloatRenderer_GetPrecision(self, *_args, **_kwargs)
        return val
    def SetPrecision(self, *_args, **_kwargs):
        val = gridc.wxGridCellFloatRenderer_SetPrecision(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellFloatRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellFloatRenderer(wxGridCellFloatRendererPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellFloatRenderer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellBoolRendererPtr(wxGridCellRendererPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellBoolRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellBoolRenderer(wxGridCellBoolRendererPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellBoolRenderer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellDateTimeRendererPtr(wxGridCellStringRendererPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellDateTimeRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellDateTimeRenderer(wxGridCellDateTimeRendererPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellDateTimeRenderer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellEnumRendererPtr(wxGridCellStringRendererPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellEnumRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellEnumRenderer(wxGridCellEnumRendererPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellEnumRenderer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellAutoWrapStringRendererPtr(wxGridCellStringRendererPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellAutoWrapStringRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellAutoWrapStringRenderer(wxGridCellAutoWrapStringRendererPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellAutoWrapStringRenderer(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellEditorPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setOORInfo(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor__setOORInfo(self, *_args, **_kwargs)
        return val
    def IsCreated(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_IsCreated(self, *_args, **_kwargs)
        return val
    def GetControl(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_GetControl(self, *_args, **_kwargs)
        return val
    def SetControl(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_SetControl(self, *_args, **_kwargs)
        return val
    def GetCellAttr(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_GetCellAttr(self, *_args, **_kwargs)
        return val
    def SetCellAttr(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_SetCellAttr(self, *_args, **_kwargs)
        return val
    def SetParameters(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_SetParameters(self, *_args, **_kwargs)
        return val
    def IncRef(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_IncRef(self, *_args, **_kwargs)
        return val
    def DecRef(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_DecRef(self, *_args, **_kwargs)
        return val
    def Create(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_Create(self, *_args, **_kwargs)
        return val
    def BeginEdit(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_BeginEdit(self, *_args, **_kwargs)
        return val
    def EndEdit(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_EndEdit(self, *_args, **_kwargs)
        return val
    def Reset(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_Reset(self, *_args, **_kwargs)
        return val
    def Clone(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_Clone(self, *_args, **_kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_SetSize(self, *_args, **_kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_Show(self, *_args, **_kwargs)
        return val
    def PaintBackground(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_PaintBackground(self, *_args, **_kwargs)
        return val
    def IsAcceptedKey(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_IsAcceptedKey(self, *_args, **_kwargs)
        return val
    def StartingKey(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_StartingKey(self, *_args, **_kwargs)
        return val
    def StartingClick(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_StartingClick(self, *_args, **_kwargs)
        return val
    def HandleReturn(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_HandleReturn(self, *_args, **_kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = gridc.wxGridCellEditor_Destroy(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellEditor(wxGridCellEditorPtr):
    def __init__(self,this):
        self.this = this




class wxPyGridCellEditorPtr(wxGridCellEditorPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellEditor__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def base_SetSize(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellEditor_base_SetSize(self, *_args, **_kwargs)
        return val
    def base_Show(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellEditor_base_Show(self, *_args, **_kwargs)
        return val
    def base_PaintBackground(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellEditor_base_PaintBackground(self, *_args, **_kwargs)
        return val
    def base_IsAcceptedKey(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellEditor_base_IsAcceptedKey(self, *_args, **_kwargs)
        return val
    def base_StartingKey(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellEditor_base_StartingKey(self, *_args, **_kwargs)
        return val
    def base_StartingClick(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellEditor_base_StartingClick(self, *_args, **_kwargs)
        return val
    def base_HandleReturn(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellEditor_base_HandleReturn(self, *_args, **_kwargs)
        return val
    def base_Destroy(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellEditor_base_Destroy(self, *_args, **_kwargs)
        return val
    def base_SetParameters(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellEditor_base_SetParameters(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPyGridCellEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPyGridCellEditor(wxPyGridCellEditorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxPyGridCellEditor(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyGridCellEditor)
        self._setOORInfo(self)




class wxGridCellTextEditorPtr(wxGridCellEditorPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellTextEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellTextEditor(wxGridCellTextEditorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellTextEditor(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellNumberEditorPtr(wxGridCellTextEditorPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellNumberEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellNumberEditor(wxGridCellNumberEditorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellNumberEditor(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellFloatEditorPtr(wxGridCellTextEditorPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellFloatEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellFloatEditor(wxGridCellFloatEditorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellFloatEditor(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellBoolEditorPtr(wxGridCellEditorPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellBoolEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellBoolEditor(wxGridCellBoolEditorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellBoolEditor(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellChoiceEditorPtr(wxGridCellEditorPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellChoiceEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellChoiceEditor(wxGridCellChoiceEditorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellChoiceEditor(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellEnumEditorPtr(wxGridCellChoiceEditorPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellEnumEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellEnumEditor(wxGridCellEnumEditorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellEnumEditor(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellAutoWrapStringEditorPtr(wxGridCellTextEditorPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellAutoWrapStringEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellAutoWrapStringEditor(wxGridCellAutoWrapStringEditorPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellAutoWrapStringEditor(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellAttrPtr :
    Any = gridc.wxGridCellAttr_Any
    Default = gridc.wxGridCellAttr_Default
    Cell = gridc.wxGridCellAttr_Cell
    Row = gridc.wxGridCellAttr_Row
    Col = gridc.wxGridCellAttr_Col
    Merged = gridc.wxGridCellAttr_Merged
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setOORInfo(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr__setOORInfo(self, *_args, **_kwargs)
        return val
    def Clone(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_Clone(self, *_args, **_kwargs)
        return val
    def MergeWith(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_MergeWith(self, *_args, **_kwargs)
        return val
    def IncRef(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_IncRef(self, *_args, **_kwargs)
        return val
    def DecRef(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_DecRef(self, *_args, **_kwargs)
        return val
    def SetTextColour(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetTextColour(self, *_args, **_kwargs)
        return val
    def SetBackgroundColour(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetBackgroundColour(self, *_args, **_kwargs)
        return val
    def SetFont(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetFont(self, *_args, **_kwargs)
        return val
    def SetAlignment(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetAlignment(self, *_args, **_kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetSize(self, *_args, **_kwargs)
        return val
    def SetOverflow(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetOverflow(self, *_args, **_kwargs)
        return val
    def SetReadOnly(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetReadOnly(self, *_args, **_kwargs)
        return val
    def SetRenderer(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetRenderer(self, *_args, **_kwargs)
        return val
    def SetEditor(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetEditor(self, *_args, **_kwargs)
        return val
    def SetKind(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetKind(self, *_args, **_kwargs)
        return val
    def HasTextColour(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_HasTextColour(self, *_args, **_kwargs)
        return val
    def HasBackgroundColour(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_HasBackgroundColour(self, *_args, **_kwargs)
        return val
    def HasFont(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_HasFont(self, *_args, **_kwargs)
        return val
    def HasAlignment(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_HasAlignment(self, *_args, **_kwargs)
        return val
    def HasRenderer(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_HasRenderer(self, *_args, **_kwargs)
        return val
    def HasEditor(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_HasEditor(self, *_args, **_kwargs)
        return val
    def HasReadWriteMode(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_HasReadWriteMode(self, *_args, **_kwargs)
        return val
    def GetTextColour(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_GetTextColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetBackgroundColour(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_GetBackgroundColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetFont(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_GetFont(self, *_args, **_kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def GetAlignment(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_GetAlignment(self, *_args, **_kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_GetSize(self, *_args, **_kwargs)
        return val
    def GetOverflow(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_GetOverflow(self, *_args, **_kwargs)
        return val
    def GetRenderer(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_GetRenderer(self, *_args, **_kwargs)
        return val
    def GetEditor(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_GetEditor(self, *_args, **_kwargs)
        return val
    def IsReadOnly(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_IsReadOnly(self, *_args, **_kwargs)
        return val
    def SetDefAttr(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttr_SetDefAttr(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellAttr(wxGridCellAttrPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellAttr(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridCellAttrProviderPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setOORInfo(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttrProvider__setOORInfo(self, *_args, **_kwargs)
        return val
    def GetAttr(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttrProvider_GetAttr(self, *_args, **_kwargs)
        return val
    def SetAttr(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttrProvider_SetAttr(self, *_args, **_kwargs)
        return val
    def SetRowAttr(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttrProvider_SetRowAttr(self, *_args, **_kwargs)
        return val
    def SetColAttr(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttrProvider_SetColAttr(self, *_args, **_kwargs)
        return val
    def UpdateAttrRows(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttrProvider_UpdateAttrRows(self, *_args, **_kwargs)
        return val
    def UpdateAttrCols(self, *_args, **_kwargs):
        val = gridc.wxGridCellAttrProvider_UpdateAttrCols(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellAttrProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridCellAttrProvider(wxGridCellAttrProviderPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellAttrProvider(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxPyGridCellAttrProviderPtr(wxGridCellAttrProviderPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellAttrProvider__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def base_GetAttr(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellAttrProvider_base_GetAttr(self, *_args, **_kwargs)
        return val
    def base_SetAttr(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellAttrProvider_base_SetAttr(self, *_args, **_kwargs)
        return val
    def base_SetRowAttr(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellAttrProvider_base_SetRowAttr(self, *_args, **_kwargs)
        return val
    def base_SetColAttr(self, *_args, **_kwargs):
        val = gridc.wxPyGridCellAttrProvider_base_SetColAttr(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPyGridCellAttrProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPyGridCellAttrProvider(wxPyGridCellAttrProviderPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxPyGridCellAttrProvider(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyGridCellAttrProvider)




class wxGridTableBasePtr(wxObjectPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setOORInfo(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase__setOORInfo(self, *_args, **_kwargs)
        return val
    def SetAttrProvider(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetAttrProvider(self, *_args, **_kwargs)
        return val
    def GetAttrProvider(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetAttrProvider(self, *_args, **_kwargs)
        return val
    def SetView(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetView(self, *_args, **_kwargs)
        return val
    def GetView(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetView(self, *_args, **_kwargs)
        return val
    def GetNumberRows(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetNumberRows(self, *_args, **_kwargs)
        return val
    def GetNumberCols(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetNumberCols(self, *_args, **_kwargs)
        return val
    def IsEmptyCell(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_IsEmptyCell(self, *_args, **_kwargs)
        return val
    def GetValue(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetValue(self, *_args, **_kwargs)
        return val
    def SetValue(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetValue(self, *_args, **_kwargs)
        return val
    def GetTypeName(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetTypeName(self, *_args, **_kwargs)
        return val
    def CanGetValueAs(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_CanGetValueAs(self, *_args, **_kwargs)
        return val
    def CanSetValueAs(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_CanSetValueAs(self, *_args, **_kwargs)
        return val
    def GetValueAsLong(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetValueAsLong(self, *_args, **_kwargs)
        return val
    def GetValueAsDouble(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetValueAsDouble(self, *_args, **_kwargs)
        return val
    def GetValueAsBool(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetValueAsBool(self, *_args, **_kwargs)
        return val
    def SetValueAsLong(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetValueAsLong(self, *_args, **_kwargs)
        return val
    def SetValueAsDouble(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetValueAsDouble(self, *_args, **_kwargs)
        return val
    def SetValueAsBool(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetValueAsBool(self, *_args, **_kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_Clear(self, *_args, **_kwargs)
        return val
    def InsertRows(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_InsertRows(self, *_args, **_kwargs)
        return val
    def AppendRows(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_AppendRows(self, *_args, **_kwargs)
        return val
    def DeleteRows(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_DeleteRows(self, *_args, **_kwargs)
        return val
    def InsertCols(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_InsertCols(self, *_args, **_kwargs)
        return val
    def AppendCols(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_AppendCols(self, *_args, **_kwargs)
        return val
    def DeleteCols(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_DeleteCols(self, *_args, **_kwargs)
        return val
    def GetRowLabelValue(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetRowLabelValue(self, *_args, **_kwargs)
        return val
    def GetColLabelValue(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetColLabelValue(self, *_args, **_kwargs)
        return val
    def SetRowLabelValue(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetRowLabelValue(self, *_args, **_kwargs)
        return val
    def SetColLabelValue(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetColLabelValue(self, *_args, **_kwargs)
        return val
    def CanHaveAttributes(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_CanHaveAttributes(self, *_args, **_kwargs)
        return val
    def GetAttr(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_GetAttr(self, *_args, **_kwargs)
        return val
    def SetAttr(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetAttr(self, *_args, **_kwargs)
        return val
    def SetRowAttr(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetRowAttr(self, *_args, **_kwargs)
        return val
    def SetColAttr(self, *_args, **_kwargs):
        val = gridc.wxGridTableBase_SetColAttr(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridTableBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridTableBase(wxGridTableBasePtr):
    def __init__(self,this):
        self.this = this




class wxPyGridTableBasePtr(wxGridTableBasePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setCallbackInfo(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase__setCallbackInfo(self, *_args, **_kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_Destroy(self, *_args, **_kwargs)
        return val
    def base_GetTypeName(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_GetTypeName(self, *_args, **_kwargs)
        return val
    def base_CanGetValueAs(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_CanGetValueAs(self, *_args, **_kwargs)
        return val
    def base_CanSetValueAs(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_CanSetValueAs(self, *_args, **_kwargs)
        return val
    def base_Clear(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_Clear(self, *_args, **_kwargs)
        return val
    def base_InsertRows(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_InsertRows(self, *_args, **_kwargs)
        return val
    def base_AppendRows(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_AppendRows(self, *_args, **_kwargs)
        return val
    def base_DeleteRows(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_DeleteRows(self, *_args, **_kwargs)
        return val
    def base_InsertCols(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_InsertCols(self, *_args, **_kwargs)
        return val
    def base_AppendCols(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_AppendCols(self, *_args, **_kwargs)
        return val
    def base_DeleteCols(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_DeleteCols(self, *_args, **_kwargs)
        return val
    def base_GetRowLabelValue(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_GetRowLabelValue(self, *_args, **_kwargs)
        return val
    def base_GetColLabelValue(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_GetColLabelValue(self, *_args, **_kwargs)
        return val
    def base_SetRowLabelValue(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_SetRowLabelValue(self, *_args, **_kwargs)
        return val
    def base_SetColLabelValue(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_SetColLabelValue(self, *_args, **_kwargs)
        return val
    def base_CanHaveAttributes(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_CanHaveAttributes(self, *_args, **_kwargs)
        return val
    def base_GetAttr(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_GetAttr(self, *_args, **_kwargs)
        return val
    def base_SetAttr(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_SetAttr(self, *_args, **_kwargs)
        return val
    def base_SetRowAttr(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_SetRowAttr(self, *_args, **_kwargs)
        return val
    def base_SetColAttr(self, *_args, **_kwargs):
        val = gridc.wxPyGridTableBase_base_SetColAttr(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxPyGridTableBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxPyGridTableBase(wxPyGridTableBasePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxPyGridTableBase(*_args,**_kwargs)
        self.thisown = 1
        self._setCallbackInfo(self, wxPyGridTableBase)
        self._setOORInfo(self)




class wxGridStringTablePtr(wxGridTableBasePtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridStringTable instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridStringTable(wxGridStringTablePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridStringTable(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridTableMessagePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=gridc.delete_wxGridTableMessage):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def SetTableObject(self, *_args, **_kwargs):
        val = gridc.wxGridTableMessage_SetTableObject(self, *_args, **_kwargs)
        return val
    def GetTableObject(self, *_args, **_kwargs):
        val = gridc.wxGridTableMessage_GetTableObject(self, *_args, **_kwargs)
        return val
    def SetId(self, *_args, **_kwargs):
        val = gridc.wxGridTableMessage_SetId(self, *_args, **_kwargs)
        return val
    def GetId(self, *_args, **_kwargs):
        val = gridc.wxGridTableMessage_GetId(self, *_args, **_kwargs)
        return val
    def SetCommandInt(self, *_args, **_kwargs):
        val = gridc.wxGridTableMessage_SetCommandInt(self, *_args, **_kwargs)
        return val
    def GetCommandInt(self, *_args, **_kwargs):
        val = gridc.wxGridTableMessage_GetCommandInt(self, *_args, **_kwargs)
        return val
    def SetCommandInt2(self, *_args, **_kwargs):
        val = gridc.wxGridTableMessage_SetCommandInt2(self, *_args, **_kwargs)
        return val
    def GetCommandInt2(self, *_args, **_kwargs):
        val = gridc.wxGridTableMessage_GetCommandInt2(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridTableMessage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridTableMessage(wxGridTableMessagePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridTableMessage(*_args,**_kwargs)
        self.thisown = 1




class wxGridCellCoordsPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, delfunc=gridc.delete_wxGridCellCoords):
        if self.thisown == 1:
            try:
                delfunc(self)
            except:
                pass
    def GetRow(self, *_args, **_kwargs):
        val = gridc.wxGridCellCoords_GetRow(self, *_args, **_kwargs)
        return val
    def SetRow(self, *_args, **_kwargs):
        val = gridc.wxGridCellCoords_SetRow(self, *_args, **_kwargs)
        return val
    def GetCol(self, *_args, **_kwargs):
        val = gridc.wxGridCellCoords_GetCol(self, *_args, **_kwargs)
        return val
    def SetCol(self, *_args, **_kwargs):
        val = gridc.wxGridCellCoords_SetCol(self, *_args, **_kwargs)
        return val
    def Set(self, *_args, **_kwargs):
        val = gridc.wxGridCellCoords_Set(self, *_args, **_kwargs)
        return val
    def asTuple(self, *_args, **_kwargs):
        val = gridc.wxGridCellCoords_asTuple(self, *_args, **_kwargs)
        return val
    def __cmp__(self, *_args, **_kwargs):
        val = gridc.wxGridCellCoords___cmp__(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridCellCoords instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
    
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxGridCellCoords'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.SetRow(val)
        elif index == 1: self.SetCol(val)
        else: raise IndexError
    
class wxGridCellCoords(wxGridCellCoordsPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridCellCoords(*_args,**_kwargs)
        self.thisown = 1




class wxGridPtr(wxScrolledWindowPtr):
    wxGridSelectCells = gridc.wxGrid_wxGridSelectCells
    wxGridSelectRows = gridc.wxGrid_wxGridSelectRows
    wxGridSelectColumns = gridc.wxGrid_wxGridSelectColumns
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def CreateGrid(self, *_args, **_kwargs):
        val = gridc.wxGrid_CreateGrid(self, *_args, **_kwargs)
        return val
    def SetSelectionMode(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetSelectionMode(self, *_args, **_kwargs)
        return val
    def GetSelectionMode(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetSelectionMode(self, *_args, **_kwargs)
        return val
    def GetNumberRows(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetNumberRows(self, *_args, **_kwargs)
        return val
    def GetNumberCols(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetNumberCols(self, *_args, **_kwargs)
        return val
    def ProcessTableMessage(self, *_args, **_kwargs):
        val = gridc.wxGrid_ProcessTableMessage(self, *_args, **_kwargs)
        return val
    def GetTable(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetTable(self, *_args, **_kwargs)
        return val
    def SetTable(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetTable(self, *_args, **_kwargs)
        return val
    def ClearGrid(self, *_args, **_kwargs):
        val = gridc.wxGrid_ClearGrid(self, *_args, **_kwargs)
        return val
    def InsertRows(self, *_args, **_kwargs):
        val = gridc.wxGrid_InsertRows(self, *_args, **_kwargs)
        return val
    def AppendRows(self, *_args, **_kwargs):
        val = gridc.wxGrid_AppendRows(self, *_args, **_kwargs)
        return val
    def DeleteRows(self, *_args, **_kwargs):
        val = gridc.wxGrid_DeleteRows(self, *_args, **_kwargs)
        return val
    def InsertCols(self, *_args, **_kwargs):
        val = gridc.wxGrid_InsertCols(self, *_args, **_kwargs)
        return val
    def AppendCols(self, *_args, **_kwargs):
        val = gridc.wxGrid_AppendCols(self, *_args, **_kwargs)
        return val
    def DeleteCols(self, *_args, **_kwargs):
        val = gridc.wxGrid_DeleteCols(self, *_args, **_kwargs)
        return val
    def DrawCellHighlight(self, *_args, **_kwargs):
        val = gridc.wxGrid_DrawCellHighlight(self, *_args, **_kwargs)
        return val
    def DrawTextRectangle(self, *_args, **_kwargs):
        val = gridc.wxGrid_DrawTextRectangle(self, *_args, **_kwargs)
        return val
    def GetTextBoxSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetTextBoxSize(self, *_args, **_kwargs)
        return val
    def BeginBatch(self, *_args, **_kwargs):
        val = gridc.wxGrid_BeginBatch(self, *_args, **_kwargs)
        return val
    def EndBatch(self, *_args, **_kwargs):
        val = gridc.wxGrid_EndBatch(self, *_args, **_kwargs)
        return val
    def GetBatchCount(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetBatchCount(self, *_args, **_kwargs)
        return val
    def ForceRefresh(self, *_args, **_kwargs):
        val = gridc.wxGrid_ForceRefresh(self, *_args, **_kwargs)
        return val
    def Refresh(self, *_args, **_kwargs):
        val = gridc.wxGrid_Refresh(self, *_args, **_kwargs)
        return val
    def IsEditable(self, *_args, **_kwargs):
        val = gridc.wxGrid_IsEditable(self, *_args, **_kwargs)
        return val
    def EnableEditing(self, *_args, **_kwargs):
        val = gridc.wxGrid_EnableEditing(self, *_args, **_kwargs)
        return val
    def EnableCellEditControl(self, *_args, **_kwargs):
        val = gridc.wxGrid_EnableCellEditControl(self, *_args, **_kwargs)
        return val
    def DisableCellEditControl(self, *_args, **_kwargs):
        val = gridc.wxGrid_DisableCellEditControl(self, *_args, **_kwargs)
        return val
    def CanEnableCellControl(self, *_args, **_kwargs):
        val = gridc.wxGrid_CanEnableCellControl(self, *_args, **_kwargs)
        return val
    def IsCellEditControlEnabled(self, *_args, **_kwargs):
        val = gridc.wxGrid_IsCellEditControlEnabled(self, *_args, **_kwargs)
        return val
    def IsCellEditControlShown(self, *_args, **_kwargs):
        val = gridc.wxGrid_IsCellEditControlShown(self, *_args, **_kwargs)
        return val
    def IsCurrentCellReadOnly(self, *_args, **_kwargs):
        val = gridc.wxGrid_IsCurrentCellReadOnly(self, *_args, **_kwargs)
        return val
    def ShowCellEditControl(self, *_args, **_kwargs):
        val = gridc.wxGrid_ShowCellEditControl(self, *_args, **_kwargs)
        return val
    def HideCellEditControl(self, *_args, **_kwargs):
        val = gridc.wxGrid_HideCellEditControl(self, *_args, **_kwargs)
        return val
    def SaveEditControlValue(self, *_args, **_kwargs):
        val = gridc.wxGrid_SaveEditControlValue(self, *_args, **_kwargs)
        return val
    def XYToCell(self, *_args, **_kwargs):
        val = gridc.wxGrid_XYToCell(self, *_args, **_kwargs)
        if val: val = wxGridCellCoordsPtr(val) ; val.thisown = 1
        return val
    def YToRow(self, *_args, **_kwargs):
        val = gridc.wxGrid_YToRow(self, *_args, **_kwargs)
        return val
    def XToCol(self, *_args, **_kwargs):
        val = gridc.wxGrid_XToCol(self, *_args, **_kwargs)
        return val
    def YToEdgeOfRow(self, *_args, **_kwargs):
        val = gridc.wxGrid_YToEdgeOfRow(self, *_args, **_kwargs)
        return val
    def XToEdgeOfCol(self, *_args, **_kwargs):
        val = gridc.wxGrid_XToEdgeOfCol(self, *_args, **_kwargs)
        return val
    def CellToRect(self, *_args, **_kwargs):
        val = gridc.wxGrid_CellToRect(self, *_args, **_kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def GetGridCursorRow(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetGridCursorRow(self, *_args, **_kwargs)
        return val
    def GetGridCursorCol(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetGridCursorCol(self, *_args, **_kwargs)
        return val
    def IsVisible(self, *_args, **_kwargs):
        val = gridc.wxGrid_IsVisible(self, *_args, **_kwargs)
        return val
    def MakeCellVisible(self, *_args, **_kwargs):
        val = gridc.wxGrid_MakeCellVisible(self, *_args, **_kwargs)
        return val
    def SetGridCursor(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetGridCursor(self, *_args, **_kwargs)
        return val
    def MoveCursorUp(self, *_args, **_kwargs):
        val = gridc.wxGrid_MoveCursorUp(self, *_args, **_kwargs)
        return val
    def MoveCursorDown(self, *_args, **_kwargs):
        val = gridc.wxGrid_MoveCursorDown(self, *_args, **_kwargs)
        return val
    def MoveCursorLeft(self, *_args, **_kwargs):
        val = gridc.wxGrid_MoveCursorLeft(self, *_args, **_kwargs)
        return val
    def MoveCursorRight(self, *_args, **_kwargs):
        val = gridc.wxGrid_MoveCursorRight(self, *_args, **_kwargs)
        return val
    def MovePageDown(self, *_args, **_kwargs):
        val = gridc.wxGrid_MovePageDown(self, *_args, **_kwargs)
        return val
    def MovePageUp(self, *_args, **_kwargs):
        val = gridc.wxGrid_MovePageUp(self, *_args, **_kwargs)
        return val
    def MoveCursorUpBlock(self, *_args, **_kwargs):
        val = gridc.wxGrid_MoveCursorUpBlock(self, *_args, **_kwargs)
        return val
    def MoveCursorDownBlock(self, *_args, **_kwargs):
        val = gridc.wxGrid_MoveCursorDownBlock(self, *_args, **_kwargs)
        return val
    def MoveCursorLeftBlock(self, *_args, **_kwargs):
        val = gridc.wxGrid_MoveCursorLeftBlock(self, *_args, **_kwargs)
        return val
    def MoveCursorRightBlock(self, *_args, **_kwargs):
        val = gridc.wxGrid_MoveCursorRightBlock(self, *_args, **_kwargs)
        return val
    def GetDefaultRowLabelSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultRowLabelSize(self, *_args, **_kwargs)
        return val
    def GetRowLabelSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetRowLabelSize(self, *_args, **_kwargs)
        return val
    def GetDefaultColLabelSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultColLabelSize(self, *_args, **_kwargs)
        return val
    def GetColLabelSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetColLabelSize(self, *_args, **_kwargs)
        return val
    def GetLabelBackgroundColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetLabelBackgroundColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetLabelTextColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetLabelTextColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetLabelFont(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetLabelFont(self, *_args, **_kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def GetRowLabelAlignment(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetRowLabelAlignment(self, *_args, **_kwargs)
        return val
    def GetColLabelAlignment(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetColLabelAlignment(self, *_args, **_kwargs)
        return val
    def GetRowLabelValue(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetRowLabelValue(self, *_args, **_kwargs)
        return val
    def GetColLabelValue(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetColLabelValue(self, *_args, **_kwargs)
        return val
    def GetGridLineColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetGridLineColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetCellHighlightColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellHighlightColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetCellHighlightPenWidth(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellHighlightPenWidth(self, *_args, **_kwargs)
        return val
    def GetCellHighlightROPenWidth(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellHighlightROPenWidth(self, *_args, **_kwargs)
        return val
    def SetRowLabelSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetRowLabelSize(self, *_args, **_kwargs)
        return val
    def SetColLabelSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetColLabelSize(self, *_args, **_kwargs)
        return val
    def SetLabelBackgroundColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetLabelBackgroundColour(self, *_args, **_kwargs)
        return val
    def SetLabelTextColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetLabelTextColour(self, *_args, **_kwargs)
        return val
    def SetLabelFont(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetLabelFont(self, *_args, **_kwargs)
        return val
    def SetRowLabelAlignment(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetRowLabelAlignment(self, *_args, **_kwargs)
        return val
    def SetColLabelAlignment(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetColLabelAlignment(self, *_args, **_kwargs)
        return val
    def SetRowLabelValue(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetRowLabelValue(self, *_args, **_kwargs)
        return val
    def SetColLabelValue(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetColLabelValue(self, *_args, **_kwargs)
        return val
    def SetGridLineColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetGridLineColour(self, *_args, **_kwargs)
        return val
    def SetCellHighlightColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellHighlightColour(self, *_args, **_kwargs)
        return val
    def SetCellHighlightPenWidth(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellHighlightPenWidth(self, *_args, **_kwargs)
        return val
    def SetCellHighlightROPenWidth(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellHighlightROPenWidth(self, *_args, **_kwargs)
        return val
    def EnableDragRowSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_EnableDragRowSize(self, *_args, **_kwargs)
        return val
    def DisableDragRowSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_DisableDragRowSize(self, *_args, **_kwargs)
        return val
    def CanDragRowSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_CanDragRowSize(self, *_args, **_kwargs)
        return val
    def EnableDragColSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_EnableDragColSize(self, *_args, **_kwargs)
        return val
    def DisableDragColSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_DisableDragColSize(self, *_args, **_kwargs)
        return val
    def CanDragColSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_CanDragColSize(self, *_args, **_kwargs)
        return val
    def EnableDragGridSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_EnableDragGridSize(self, *_args, **_kwargs)
        return val
    def DisableDragGridSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_DisableDragGridSize(self, *_args, **_kwargs)
        return val
    def CanDragGridSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_CanDragGridSize(self, *_args, **_kwargs)
        return val
    def SetAttr(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetAttr(self, *_args, **_kwargs)
        return val
    def SetRowAttr(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetRowAttr(self, *_args, **_kwargs)
        return val
    def SetColAttr(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetColAttr(self, *_args, **_kwargs)
        return val
    def SetColFormatBool(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetColFormatBool(self, *_args, **_kwargs)
        return val
    def SetColFormatNumber(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetColFormatNumber(self, *_args, **_kwargs)
        return val
    def SetColFormatFloat(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetColFormatFloat(self, *_args, **_kwargs)
        return val
    def SetColFormatCustom(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetColFormatCustom(self, *_args, **_kwargs)
        return val
    def EnableGridLines(self, *_args, **_kwargs):
        val = gridc.wxGrid_EnableGridLines(self, *_args, **_kwargs)
        return val
    def GridLinesEnabled(self, *_args, **_kwargs):
        val = gridc.wxGrid_GridLinesEnabled(self, *_args, **_kwargs)
        return val
    def GetDefaultRowSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultRowSize(self, *_args, **_kwargs)
        return val
    def GetRowSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetRowSize(self, *_args, **_kwargs)
        return val
    def GetDefaultColSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultColSize(self, *_args, **_kwargs)
        return val
    def GetColSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetColSize(self, *_args, **_kwargs)
        return val
    def GetDefaultCellBackgroundColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultCellBackgroundColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetCellBackgroundColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellBackgroundColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetDefaultCellTextColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultCellTextColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetCellTextColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellTextColour(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetDefaultCellFont(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultCellFont(self, *_args, **_kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def GetCellFont(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellFont(self, *_args, **_kwargs)
        if val: val = wxFontPtr(val) ; val.thisown = 1
        return val
    def GetDefaultCellAlignment(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultCellAlignment(self, *_args, **_kwargs)
        return val
    def GetCellAlignment(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellAlignment(self, *_args, **_kwargs)
        return val
    def GetDefaultCellOverflow(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultCellOverflow(self, *_args, **_kwargs)
        return val
    def GetCellOverflow(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellOverflow(self, *_args, **_kwargs)
        return val
    def GetCellSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellSize(self, *_args, **_kwargs)
        return val
    def SetDefaultRowSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetDefaultRowSize(self, *_args, **_kwargs)
        return val
    def SetRowSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetRowSize(self, *_args, **_kwargs)
        return val
    def SetDefaultColSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetDefaultColSize(self, *_args, **_kwargs)
        return val
    def SetColSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetColSize(self, *_args, **_kwargs)
        return val
    def AutoSizeColumn(self, *_args, **_kwargs):
        val = gridc.wxGrid_AutoSizeColumn(self, *_args, **_kwargs)
        return val
    def AutoSizeRow(self, *_args, **_kwargs):
        val = gridc.wxGrid_AutoSizeRow(self, *_args, **_kwargs)
        return val
    def AutoSizeColumns(self, *_args, **_kwargs):
        val = gridc.wxGrid_AutoSizeColumns(self, *_args, **_kwargs)
        return val
    def AutoSizeRows(self, *_args, **_kwargs):
        val = gridc.wxGrid_AutoSizeRows(self, *_args, **_kwargs)
        return val
    def AutoSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_AutoSize(self, *_args, **_kwargs)
        return val
    def SetColMinimalWidth(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetColMinimalWidth(self, *_args, **_kwargs)
        return val
    def SetRowMinimalHeight(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetRowMinimalHeight(self, *_args, **_kwargs)
        return val
    def SetDefaultCellBackgroundColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetDefaultCellBackgroundColour(self, *_args, **_kwargs)
        return val
    def SetCellBackgroundColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellBackgroundColour(self, *_args, **_kwargs)
        return val
    def SetDefaultCellTextColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetDefaultCellTextColour(self, *_args, **_kwargs)
        return val
    def SetCellTextColour(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellTextColour(self, *_args, **_kwargs)
        return val
    def SetDefaultCellFont(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetDefaultCellFont(self, *_args, **_kwargs)
        return val
    def SetCellFont(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellFont(self, *_args, **_kwargs)
        return val
    def SetDefaultCellAlignment(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetDefaultCellAlignment(self, *_args, **_kwargs)
        return val
    def SetCellAlignment(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellAlignment(self, *_args, **_kwargs)
        return val
    def SetDefaultCellOverflow(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetDefaultCellOverflow(self, *_args, **_kwargs)
        return val
    def SetCellOverflow(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellOverflow(self, *_args, **_kwargs)
        return val
    def SetCellSize(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellSize(self, *_args, **_kwargs)
        return val
    def SetDefaultRenderer(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetDefaultRenderer(self, *_args, **_kwargs)
        return val
    def SetCellRenderer(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellRenderer(self, *_args, **_kwargs)
        return val
    def GetDefaultRenderer(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultRenderer(self, *_args, **_kwargs)
        return val
    def GetCellRenderer(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellRenderer(self, *_args, **_kwargs)
        return val
    def SetDefaultEditor(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetDefaultEditor(self, *_args, **_kwargs)
        return val
    def SetCellEditor(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellEditor(self, *_args, **_kwargs)
        return val
    def GetDefaultEditor(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultEditor(self, *_args, **_kwargs)
        return val
    def GetCellEditor(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellEditor(self, *_args, **_kwargs)
        return val
    def GetCellValue(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetCellValue(self, *_args, **_kwargs)
        return val
    def SetCellValue(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetCellValue(self, *_args, **_kwargs)
        return val
    def IsReadOnly(self, *_args, **_kwargs):
        val = gridc.wxGrid_IsReadOnly(self, *_args, **_kwargs)
        return val
    def SetReadOnly(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetReadOnly(self, *_args, **_kwargs)
        return val
    def SelectRow(self, *_args, **_kwargs):
        val = gridc.wxGrid_SelectRow(self, *_args, **_kwargs)
        return val
    def SelectCol(self, *_args, **_kwargs):
        val = gridc.wxGrid_SelectCol(self, *_args, **_kwargs)
        return val
    def SelectBlock(self, *_args, **_kwargs):
        val = gridc.wxGrid_SelectBlock(self, *_args, **_kwargs)
        return val
    def SelectAll(self, *_args, **_kwargs):
        val = gridc.wxGrid_SelectAll(self, *_args, **_kwargs)
        return val
    def IsSelection(self, *_args, **_kwargs):
        val = gridc.wxGrid_IsSelection(self, *_args, **_kwargs)
        return val
    def ClearSelection(self, *_args, **_kwargs):
        val = gridc.wxGrid_ClearSelection(self, *_args, **_kwargs)
        return val
    def IsInSelection(self, *_args, **_kwargs):
        val = gridc.wxGrid_IsInSelection(self, *_args, **_kwargs)
        return val
    def GetSelectedCells(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetSelectedCells(self, *_args, **_kwargs)
        return val
    def GetSelectionBlockTopLeft(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetSelectionBlockTopLeft(self, *_args, **_kwargs)
        return val
    def GetSelectionBlockBottomRight(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetSelectionBlockBottomRight(self, *_args, **_kwargs)
        return val
    def GetSelectedRows(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetSelectedRows(self, *_args, **_kwargs)
        return val
    def GetSelectedCols(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetSelectedCols(self, *_args, **_kwargs)
        return val
    def DeselectRow(self, *_args, **_kwargs):
        val = gridc.wxGrid_DeselectRow(self, *_args, **_kwargs)
        return val
    def DeselectCol(self, *_args, **_kwargs):
        val = gridc.wxGrid_DeselectCol(self, *_args, **_kwargs)
        return val
    def DeselectCell(self, *_args, **_kwargs):
        val = gridc.wxGrid_DeselectCell(self, *_args, **_kwargs)
        return val
    def BlockToDeviceRect(self, *_args, **_kwargs):
        val = gridc.wxGrid_BlockToDeviceRect(self, *_args, **_kwargs)
        if val: val = wxRectPtr(val) ; val.thisown = 1
        return val
    def GetSelectionBackground(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetSelectionBackground(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetSelectionForeground(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetSelectionForeground(self, *_args, **_kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def SetSelectionBackground(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetSelectionBackground(self, *_args, **_kwargs)
        return val
    def SetSelectionForeground(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetSelectionForeground(self, *_args, **_kwargs)
        return val
    def RegisterDataType(self, *_args, **_kwargs):
        val = gridc.wxGrid_RegisterDataType(self, *_args, **_kwargs)
        return val
    def GetDefaultEditorForCell(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultEditorForCell(self, *_args, **_kwargs)
        return val
    def GetDefaultRendererForCell(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultRendererForCell(self, *_args, **_kwargs)
        return val
    def GetDefaultEditorForType(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultEditorForType(self, *_args, **_kwargs)
        return val
    def GetDefaultRendererForType(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetDefaultRendererForType(self, *_args, **_kwargs)
        return val
    def SetMargins(self, *_args, **_kwargs):
        val = gridc.wxGrid_SetMargins(self, *_args, **_kwargs)
        return val
    def GetGridWindow(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetGridWindow(self, *_args, **_kwargs)
        return val
    def GetGridRowLabelWindow(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetGridRowLabelWindow(self, *_args, **_kwargs)
        return val
    def GetGridColLabelWindow(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetGridColLabelWindow(self, *_args, **_kwargs)
        return val
    def GetGridCornerLabelWindow(self, *_args, **_kwargs):
        val = gridc.wxGrid_GetGridCornerLabelWindow(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGrid instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGrid(wxGridPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGrid(*_args,**_kwargs)
        self.thisown = 1
        self._setOORInfo(self)




class wxGridEventPtr(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetRow(self, *_args, **_kwargs):
        val = gridc.wxGridEvent_GetRow(self, *_args, **_kwargs)
        return val
    def GetCol(self, *_args, **_kwargs):
        val = gridc.wxGridEvent_GetCol(self, *_args, **_kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = gridc.wxGridEvent_GetPosition(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def Selecting(self, *_args, **_kwargs):
        val = gridc.wxGridEvent_Selecting(self, *_args, **_kwargs)
        return val
    def ControlDown(self, *_args, **_kwargs):
        val = gridc.wxGridEvent_ControlDown(self, *_args, **_kwargs)
        return val
    def MetaDown(self, *_args, **_kwargs):
        val = gridc.wxGridEvent_MetaDown(self, *_args, **_kwargs)
        return val
    def ShiftDown(self, *_args, **_kwargs):
        val = gridc.wxGridEvent_ShiftDown(self, *_args, **_kwargs)
        return val
    def AltDown(self, *_args, **_kwargs):
        val = gridc.wxGridEvent_AltDown(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridEvent(wxGridEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridEvent(*_args,**_kwargs)
        self.thisown = 1




class wxGridSizeEventPtr(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetRowOrCol(self, *_args, **_kwargs):
        val = gridc.wxGridSizeEvent_GetRowOrCol(self, *_args, **_kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = gridc.wxGridSizeEvent_GetPosition(self, *_args, **_kwargs)
        if val: val = wxPointPtr(val) ; val.thisown = 1
        return val
    def ControlDown(self, *_args, **_kwargs):
        val = gridc.wxGridSizeEvent_ControlDown(self, *_args, **_kwargs)
        return val
    def MetaDown(self, *_args, **_kwargs):
        val = gridc.wxGridSizeEvent_MetaDown(self, *_args, **_kwargs)
        return val
    def ShiftDown(self, *_args, **_kwargs):
        val = gridc.wxGridSizeEvent_ShiftDown(self, *_args, **_kwargs)
        return val
    def AltDown(self, *_args, **_kwargs):
        val = gridc.wxGridSizeEvent_AltDown(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridSizeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridSizeEvent(wxGridSizeEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridSizeEvent(*_args,**_kwargs)
        self.thisown = 1




class wxGridRangeSelectEventPtr(wxNotifyEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetTopLeftCoords(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_GetTopLeftCoords(self, *_args, **_kwargs)
        if val: val = wxGridCellCoordsPtr(val) ; val.thisown = 1
        return val
    def GetBottomRightCoords(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_GetBottomRightCoords(self, *_args, **_kwargs)
        if val: val = wxGridCellCoordsPtr(val) ; val.thisown = 1
        return val
    def GetTopRow(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_GetTopRow(self, *_args, **_kwargs)
        return val
    def GetBottomRow(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_GetBottomRow(self, *_args, **_kwargs)
        return val
    def GetLeftCol(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_GetLeftCol(self, *_args, **_kwargs)
        return val
    def GetRightCol(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_GetRightCol(self, *_args, **_kwargs)
        return val
    def Selecting(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_Selecting(self, *_args, **_kwargs)
        return val
    def ControlDown(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_ControlDown(self, *_args, **_kwargs)
        return val
    def MetaDown(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_MetaDown(self, *_args, **_kwargs)
        return val
    def ShiftDown(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_ShiftDown(self, *_args, **_kwargs)
        return val
    def AltDown(self, *_args, **_kwargs):
        val = gridc.wxGridRangeSelectEvent_AltDown(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridRangeSelectEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridRangeSelectEvent(wxGridRangeSelectEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridRangeSelectEvent(*_args,**_kwargs)
        self.thisown = 1




class wxGridEditorCreatedEventPtr(wxCommandEventPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetRow(self, *_args, **_kwargs):
        val = gridc.wxGridEditorCreatedEvent_GetRow(self, *_args, **_kwargs)
        return val
    def GetCol(self, *_args, **_kwargs):
        val = gridc.wxGridEditorCreatedEvent_GetCol(self, *_args, **_kwargs)
        return val
    def GetControl(self, *_args, **_kwargs):
        val = gridc.wxGridEditorCreatedEvent_GetControl(self, *_args, **_kwargs)
        return val
    def SetRow(self, *_args, **_kwargs):
        val = gridc.wxGridEditorCreatedEvent_SetRow(self, *_args, **_kwargs)
        return val
    def SetCol(self, *_args, **_kwargs):
        val = gridc.wxGridEditorCreatedEvent_SetCol(self, *_args, **_kwargs)
        return val
    def SetControl(self, *_args, **_kwargs):
        val = gridc.wxGridEditorCreatedEvent_SetControl(self, *_args, **_kwargs)
        return val
    def __repr__(self):
        return "<%s.%s instance; proxy of C++ wxGridEditorCreatedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this)
class wxGridEditorCreatedEvent(wxGridEditorCreatedEventPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = gridc.new_wxGridEditorCreatedEvent(*_args,**_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

wxGRID_VALUE_STRING = gridc.wxGRID_VALUE_STRING
wxGRID_VALUE_BOOL = gridc.wxGRID_VALUE_BOOL
wxGRID_VALUE_NUMBER = gridc.wxGRID_VALUE_NUMBER
wxGRID_VALUE_FLOAT = gridc.wxGRID_VALUE_FLOAT
wxGRID_VALUE_CHOICE = gridc.wxGRID_VALUE_CHOICE
wxGRID_VALUE_TEXT = gridc.wxGRID_VALUE_TEXT
wxGRID_VALUE_LONG = gridc.wxGRID_VALUE_LONG
wxGRID_VALUE_CHOICEINT = gridc.wxGRID_VALUE_CHOICEINT
wxGRID_VALUE_DATETIME = gridc.wxGRID_VALUE_DATETIME
cvar = gridc.cvar
wxGridNoCellCoords = wxGridCellCoordsPtr(gridc.cvar.wxGridNoCellCoords)
wxGridNoCellRect = wxRectPtr(gridc.cvar.wxGridNoCellRect)
wxGRIDTABLE_REQUEST_VIEW_GET_VALUES = gridc.wxGRIDTABLE_REQUEST_VIEW_GET_VALUES
wxGRIDTABLE_REQUEST_VIEW_SEND_VALUES = gridc.wxGRIDTABLE_REQUEST_VIEW_SEND_VALUES
wxGRIDTABLE_NOTIFY_ROWS_INSERTED = gridc.wxGRIDTABLE_NOTIFY_ROWS_INSERTED
wxGRIDTABLE_NOTIFY_ROWS_APPENDED = gridc.wxGRIDTABLE_NOTIFY_ROWS_APPENDED
wxGRIDTABLE_NOTIFY_ROWS_DELETED = gridc.wxGRIDTABLE_NOTIFY_ROWS_DELETED
wxGRIDTABLE_NOTIFY_COLS_INSERTED = gridc.wxGRIDTABLE_NOTIFY_COLS_INSERTED
wxGRIDTABLE_NOTIFY_COLS_APPENDED = gridc.wxGRIDTABLE_NOTIFY_COLS_APPENDED
wxGRIDTABLE_NOTIFY_COLS_DELETED = gridc.wxGRIDTABLE_NOTIFY_COLS_DELETED
wxEVT_GRID_CELL_LEFT_CLICK = gridc.wxEVT_GRID_CELL_LEFT_CLICK
wxEVT_GRID_CELL_RIGHT_CLICK = gridc.wxEVT_GRID_CELL_RIGHT_CLICK
wxEVT_GRID_CELL_LEFT_DCLICK = gridc.wxEVT_GRID_CELL_LEFT_DCLICK
wxEVT_GRID_CELL_RIGHT_DCLICK = gridc.wxEVT_GRID_CELL_RIGHT_DCLICK
wxEVT_GRID_LABEL_LEFT_CLICK = gridc.wxEVT_GRID_LABEL_LEFT_CLICK
wxEVT_GRID_LABEL_RIGHT_CLICK = gridc.wxEVT_GRID_LABEL_RIGHT_CLICK
wxEVT_GRID_LABEL_LEFT_DCLICK = gridc.wxEVT_GRID_LABEL_LEFT_DCLICK
wxEVT_GRID_LABEL_RIGHT_DCLICK = gridc.wxEVT_GRID_LABEL_RIGHT_DCLICK
wxEVT_GRID_ROW_SIZE = gridc.wxEVT_GRID_ROW_SIZE
wxEVT_GRID_COL_SIZE = gridc.wxEVT_GRID_COL_SIZE
wxEVT_GRID_RANGE_SELECT = gridc.wxEVT_GRID_RANGE_SELECT
wxEVT_GRID_CELL_CHANGE = gridc.wxEVT_GRID_CELL_CHANGE
wxEVT_GRID_SELECT_CELL = gridc.wxEVT_GRID_SELECT_CELL
wxEVT_GRID_EDITOR_SHOWN = gridc.wxEVT_GRID_EDITOR_SHOWN
wxEVT_GRID_EDITOR_HIDDEN = gridc.wxEVT_GRID_EDITOR_HIDDEN
wxEVT_GRID_EDITOR_CREATED = gridc.wxEVT_GRID_EDITOR_CREATED


#-------------- USER INCLUDE -----------------------



# Stuff these names into the wx namespace so wxPyConstructObject can find them

wx.wxGridEventPtr                  = wxGridEventPtr
wx.wxGridSizeEventPtr              = wxGridSizeEventPtr
wx.wxGridRangeSelectEventPtr       = wxGridRangeSelectEventPtr
wx.wxGridEditorCreatedEventPtr     = wxGridEditorCreatedEventPtr
wx.wxGridCellRendererPtr           = wxGridCellRendererPtr
wx.wxPyGridCellRendererPtr         = wxPyGridCellRendererPtr
wx.wxGridCellEditorPtr             = wxGridCellEditorPtr
wx.wxPyGridCellEditorPtr           = wxPyGridCellEditorPtr
wx.wxGridCellAttrPtr               = wxGridCellAttrPtr
wx.wxGridCellAttrProviderPtr       = wxGridCellAttrProviderPtr
wx.wxPyGridCellAttrProviderPtr     = wxPyGridCellAttrProviderPtr
wx.wxGridTableBasePtr              = wxGridTableBasePtr
wx.wxPyGridTableBasePtr            = wxPyGridTableBasePtr
wx.wxGridTableMessagePtr           = wxGridTableMessagePtr
wx.wxGridCellCoordsPtr             = wxGridCellCoordsPtr
wx.wxGridPtr                       = wxGridPtr
