# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _grid

import windows
import core
wx = core 
GRID_VALUE_STRING = _grid.GRID_VALUE_STRING
GRID_VALUE_BOOL = _grid.GRID_VALUE_BOOL
GRID_VALUE_NUMBER = _grid.GRID_VALUE_NUMBER
GRID_VALUE_FLOAT = _grid.GRID_VALUE_FLOAT
GRID_VALUE_CHOICE = _grid.GRID_VALUE_CHOICE
GRID_VALUE_TEXT = _grid.GRID_VALUE_TEXT
GRID_VALUE_LONG = _grid.GRID_VALUE_LONG
GRID_VALUE_CHOICEINT = _grid.GRID_VALUE_CHOICEINT
GRID_VALUE_DATETIME = _grid.GRID_VALUE_DATETIME
class GridCellRenderer(object):
    def _setOORInfo(*args, **kwargs): return _grid.GridCellRenderer__setOORInfo(*args, **kwargs)
    def SetParameters(*args, **kwargs): return _grid.GridCellRenderer_SetParameters(*args, **kwargs)
    def IncRef(*args, **kwargs): return _grid.GridCellRenderer_IncRef(*args, **kwargs)
    def DecRef(*args, **kwargs): return _grid.GridCellRenderer_DecRef(*args, **kwargs)
    def Draw(*args, **kwargs): return _grid.GridCellRenderer_Draw(*args, **kwargs)
    def GetBestSize(*args, **kwargs): return _grid.GridCellRenderer_GetBestSize(*args, **kwargs)
    def Clone(*args, **kwargs): return _grid.GridCellRenderer_Clone(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellRendererPtr(GridCellRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellRenderer
_grid.GridCellRenderer_swigregister(GridCellRendererPtr)
cvar = _grid.cvar
GridNoCellCoords = cvar.GridNoCellCoords
GridNoCellRect = cvar.GridNoCellRect

class PyGridCellRenderer(GridCellRenderer):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_PyGridCellRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyGridCellRenderer);self._setOORInfo(self)
    def _setCallbackInfo(*args, **kwargs): return _grid.PyGridCellRenderer__setCallbackInfo(*args, **kwargs)
    def base_SetParameters(*args, **kwargs): return _grid.PyGridCellRenderer_base_SetParameters(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyGridCellRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PyGridCellRendererPtr(PyGridCellRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyGridCellRenderer
_grid.PyGridCellRenderer_swigregister(PyGridCellRendererPtr)

class GridCellStringRenderer(GridCellRenderer):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellStringRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellStringRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellStringRendererPtr(GridCellStringRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellStringRenderer
_grid.GridCellStringRenderer_swigregister(GridCellStringRendererPtr)

class GridCellNumberRenderer(GridCellStringRenderer):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellNumberRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellNumberRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellNumberRendererPtr(GridCellNumberRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellNumberRenderer
_grid.GridCellNumberRenderer_swigregister(GridCellNumberRendererPtr)

class GridCellFloatRenderer(GridCellStringRenderer):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellFloatRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def GetWidth(*args, **kwargs): return _grid.GridCellFloatRenderer_GetWidth(*args, **kwargs)
    def SetWidth(*args, **kwargs): return _grid.GridCellFloatRenderer_SetWidth(*args, **kwargs)
    def GetPrecision(*args, **kwargs): return _grid.GridCellFloatRenderer_GetPrecision(*args, **kwargs)
    def SetPrecision(*args, **kwargs): return _grid.GridCellFloatRenderer_SetPrecision(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellFloatRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellFloatRendererPtr(GridCellFloatRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellFloatRenderer
_grid.GridCellFloatRenderer_swigregister(GridCellFloatRendererPtr)

class GridCellBoolRenderer(GridCellRenderer):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellBoolRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellBoolRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellBoolRendererPtr(GridCellBoolRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellBoolRenderer
_grid.GridCellBoolRenderer_swigregister(GridCellBoolRendererPtr)

class GridCellDateTimeRenderer(GridCellStringRenderer):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellDateTimeRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellDateTimeRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellDateTimeRendererPtr(GridCellDateTimeRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellDateTimeRenderer
_grid.GridCellDateTimeRenderer_swigregister(GridCellDateTimeRendererPtr)

class GridCellEnumRenderer(GridCellStringRenderer):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellEnumRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellEnumRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellEnumRendererPtr(GridCellEnumRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellEnumRenderer
_grid.GridCellEnumRenderer_swigregister(GridCellEnumRendererPtr)

class GridCellAutoWrapStringRenderer(GridCellStringRenderer):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellAutoWrapStringRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellAutoWrapStringRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellAutoWrapStringRendererPtr(GridCellAutoWrapStringRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellAutoWrapStringRenderer
_grid.GridCellAutoWrapStringRenderer_swigregister(GridCellAutoWrapStringRendererPtr)

class GridCellEditor(object):
    def _setOORInfo(*args, **kwargs): return _grid.GridCellEditor__setOORInfo(*args, **kwargs)
    def IsCreated(*args, **kwargs): return _grid.GridCellEditor_IsCreated(*args, **kwargs)
    def GetControl(*args, **kwargs): return _grid.GridCellEditor_GetControl(*args, **kwargs)
    def SetControl(*args, **kwargs): return _grid.GridCellEditor_SetControl(*args, **kwargs)
    def GetCellAttr(*args, **kwargs): return _grid.GridCellEditor_GetCellAttr(*args, **kwargs)
    def SetCellAttr(*args, **kwargs): return _grid.GridCellEditor_SetCellAttr(*args, **kwargs)
    def SetParameters(*args, **kwargs): return _grid.GridCellEditor_SetParameters(*args, **kwargs)
    def IncRef(*args, **kwargs): return _grid.GridCellEditor_IncRef(*args, **kwargs)
    def DecRef(*args, **kwargs): return _grid.GridCellEditor_DecRef(*args, **kwargs)
    def Create(*args, **kwargs): return _grid.GridCellEditor_Create(*args, **kwargs)
    def BeginEdit(*args, **kwargs): return _grid.GridCellEditor_BeginEdit(*args, **kwargs)
    def EndEdit(*args, **kwargs): return _grid.GridCellEditor_EndEdit(*args, **kwargs)
    def Reset(*args, **kwargs): return _grid.GridCellEditor_Reset(*args, **kwargs)
    def Clone(*args, **kwargs): return _grid.GridCellEditor_Clone(*args, **kwargs)
    def SetSize(*args, **kwargs): return _grid.GridCellEditor_SetSize(*args, **kwargs)
    def Show(*args, **kwargs): return _grid.GridCellEditor_Show(*args, **kwargs)
    def PaintBackground(*args, **kwargs): return _grid.GridCellEditor_PaintBackground(*args, **kwargs)
    def IsAcceptedKey(*args, **kwargs): return _grid.GridCellEditor_IsAcceptedKey(*args, **kwargs)
    def StartingKey(*args, **kwargs): return _grid.GridCellEditor_StartingKey(*args, **kwargs)
    def StartingClick(*args, **kwargs): return _grid.GridCellEditor_StartingClick(*args, **kwargs)
    def HandleReturn(*args, **kwargs): return _grid.GridCellEditor_HandleReturn(*args, **kwargs)
    def Destroy(*args, **kwargs): return _grid.GridCellEditor_Destroy(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellEditorPtr(GridCellEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellEditor
_grid.GridCellEditor_swigregister(GridCellEditorPtr)

class PyGridCellEditor(GridCellEditor):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_PyGridCellEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyGridCellEditor);self._setOORInfo(self)
    def _setCallbackInfo(*args, **kwargs): return _grid.PyGridCellEditor__setCallbackInfo(*args, **kwargs)
    def base_SetSize(*args, **kwargs): return _grid.PyGridCellEditor_base_SetSize(*args, **kwargs)
    def base_Show(*args, **kwargs): return _grid.PyGridCellEditor_base_Show(*args, **kwargs)
    def base_PaintBackground(*args, **kwargs): return _grid.PyGridCellEditor_base_PaintBackground(*args, **kwargs)
    def base_IsAcceptedKey(*args, **kwargs): return _grid.PyGridCellEditor_base_IsAcceptedKey(*args, **kwargs)
    def base_StartingKey(*args, **kwargs): return _grid.PyGridCellEditor_base_StartingKey(*args, **kwargs)
    def base_StartingClick(*args, **kwargs): return _grid.PyGridCellEditor_base_StartingClick(*args, **kwargs)
    def base_HandleReturn(*args, **kwargs): return _grid.PyGridCellEditor_base_HandleReturn(*args, **kwargs)
    def base_Destroy(*args, **kwargs): return _grid.PyGridCellEditor_base_Destroy(*args, **kwargs)
    def base_SetParameters(*args, **kwargs): return _grid.PyGridCellEditor_base_SetParameters(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyGridCellEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PyGridCellEditorPtr(PyGridCellEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyGridCellEditor
_grid.PyGridCellEditor_swigregister(PyGridCellEditorPtr)

class GridCellTextEditor(GridCellEditor):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellTextEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def GetValue(*args, **kwargs): return _grid.GridCellTextEditor_GetValue(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellTextEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellTextEditorPtr(GridCellTextEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellTextEditor
_grid.GridCellTextEditor_swigregister(GridCellTextEditorPtr)

class GridCellNumberEditor(GridCellTextEditor):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellNumberEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def GetValue(*args, **kwargs): return _grid.GridCellNumberEditor_GetValue(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellNumberEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellNumberEditorPtr(GridCellNumberEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellNumberEditor
_grid.GridCellNumberEditor_swigregister(GridCellNumberEditorPtr)

class GridCellFloatEditor(GridCellTextEditor):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellFloatEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def GetValue(*args, **kwargs): return _grid.GridCellFloatEditor_GetValue(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellFloatEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellFloatEditorPtr(GridCellFloatEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellFloatEditor
_grid.GridCellFloatEditor_swigregister(GridCellFloatEditorPtr)

class GridCellBoolEditor(GridCellEditor):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellBoolEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def GetValue(*args, **kwargs): return _grid.GridCellBoolEditor_GetValue(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellBoolEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellBoolEditorPtr(GridCellBoolEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellBoolEditor
_grid.GridCellBoolEditor_swigregister(GridCellBoolEditorPtr)

class GridCellChoiceEditor(GridCellEditor):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellChoiceEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def GetValue(*args, **kwargs): return _grid.GridCellChoiceEditor_GetValue(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellChoiceEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellChoiceEditorPtr(GridCellChoiceEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellChoiceEditor
_grid.GridCellChoiceEditor_swigregister(GridCellChoiceEditorPtr)

class GridCellEnumEditor(GridCellChoiceEditor):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellEnumEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def GetValue(*args, **kwargs): return _grid.GridCellEnumEditor_GetValue(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellEnumEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellEnumEditorPtr(GridCellEnumEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellEnumEditor
_grid.GridCellEnumEditor_swigregister(GridCellEnumEditorPtr)

class GridCellAutoWrapStringEditor(GridCellTextEditor):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellAutoWrapStringEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def GetValue(*args, **kwargs): return _grid.GridCellAutoWrapStringEditor_GetValue(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellAutoWrapStringEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellAutoWrapStringEditorPtr(GridCellAutoWrapStringEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellAutoWrapStringEditor
_grid.GridCellAutoWrapStringEditor_swigregister(GridCellAutoWrapStringEditorPtr)

class GridCellAttr(object):
    Any = _grid.GridCellAttr_Any
    Default = _grid.GridCellAttr_Default
    Cell = _grid.GridCellAttr_Cell
    Row = _grid.GridCellAttr_Row
    Col = _grid.GridCellAttr_Col
    Merged = _grid.GridCellAttr_Merged
    def _setOORInfo(*args, **kwargs): return _grid.GridCellAttr__setOORInfo(*args, **kwargs)
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellAttr(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def Clone(*args, **kwargs): return _grid.GridCellAttr_Clone(*args, **kwargs)
    def MergeWith(*args, **kwargs): return _grid.GridCellAttr_MergeWith(*args, **kwargs)
    def IncRef(*args, **kwargs): return _grid.GridCellAttr_IncRef(*args, **kwargs)
    def DecRef(*args, **kwargs): return _grid.GridCellAttr_DecRef(*args, **kwargs)
    def SetTextColour(*args, **kwargs): return _grid.GridCellAttr_SetTextColour(*args, **kwargs)
    def SetBackgroundColour(*args, **kwargs): return _grid.GridCellAttr_SetBackgroundColour(*args, **kwargs)
    def SetFont(*args, **kwargs): return _grid.GridCellAttr_SetFont(*args, **kwargs)
    def SetAlignment(*args, **kwargs): return _grid.GridCellAttr_SetAlignment(*args, **kwargs)
    def SetSize(*args, **kwargs): return _grid.GridCellAttr_SetSize(*args, **kwargs)
    def SetOverflow(*args, **kwargs): return _grid.GridCellAttr_SetOverflow(*args, **kwargs)
    def SetReadOnly(*args, **kwargs): return _grid.GridCellAttr_SetReadOnly(*args, **kwargs)
    def SetRenderer(*args, **kwargs): return _grid.GridCellAttr_SetRenderer(*args, **kwargs)
    def SetEditor(*args, **kwargs): return _grid.GridCellAttr_SetEditor(*args, **kwargs)
    def SetKind(*args, **kwargs): return _grid.GridCellAttr_SetKind(*args, **kwargs)
    def HasTextColour(*args, **kwargs): return _grid.GridCellAttr_HasTextColour(*args, **kwargs)
    def HasBackgroundColour(*args, **kwargs): return _grid.GridCellAttr_HasBackgroundColour(*args, **kwargs)
    def HasFont(*args, **kwargs): return _grid.GridCellAttr_HasFont(*args, **kwargs)
    def HasAlignment(*args, **kwargs): return _grid.GridCellAttr_HasAlignment(*args, **kwargs)
    def HasRenderer(*args, **kwargs): return _grid.GridCellAttr_HasRenderer(*args, **kwargs)
    def HasEditor(*args, **kwargs): return _grid.GridCellAttr_HasEditor(*args, **kwargs)
    def HasReadWriteMode(*args, **kwargs): return _grid.GridCellAttr_HasReadWriteMode(*args, **kwargs)
    def HasOverflowMode(*args, **kwargs): return _grid.GridCellAttr_HasOverflowMode(*args, **kwargs)
    def GetTextColour(*args, **kwargs): return _grid.GridCellAttr_GetTextColour(*args, **kwargs)
    def GetBackgroundColour(*args, **kwargs): return _grid.GridCellAttr_GetBackgroundColour(*args, **kwargs)
    def GetFont(*args, **kwargs): return _grid.GridCellAttr_GetFont(*args, **kwargs)
    def GetAlignment(*args, **kwargs): return _grid.GridCellAttr_GetAlignment(*args, **kwargs)
    def GetSize(*args, **kwargs): return _grid.GridCellAttr_GetSize(*args, **kwargs)
    def GetOverflow(*args, **kwargs): return _grid.GridCellAttr_GetOverflow(*args, **kwargs)
    def GetRenderer(*args, **kwargs): return _grid.GridCellAttr_GetRenderer(*args, **kwargs)
    def GetEditor(*args, **kwargs): return _grid.GridCellAttr_GetEditor(*args, **kwargs)
    def IsReadOnly(*args, **kwargs): return _grid.GridCellAttr_IsReadOnly(*args, **kwargs)
    def SetDefAttr(*args, **kwargs): return _grid.GridCellAttr_SetDefAttr(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellAttrPtr(GridCellAttr):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellAttr
_grid.GridCellAttr_swigregister(GridCellAttrPtr)

class GridCellAttrProvider(object):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellAttrProvider(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def _setOORInfo(*args, **kwargs): return _grid.GridCellAttrProvider__setOORInfo(*args, **kwargs)
    def GetAttr(*args, **kwargs): return _grid.GridCellAttrProvider_GetAttr(*args, **kwargs)
    def SetAttr(*args, **kwargs): return _grid.GridCellAttrProvider_SetAttr(*args, **kwargs)
    def SetRowAttr(*args, **kwargs): return _grid.GridCellAttrProvider_SetRowAttr(*args, **kwargs)
    def SetColAttr(*args, **kwargs): return _grid.GridCellAttrProvider_SetColAttr(*args, **kwargs)
    def UpdateAttrRows(*args, **kwargs): return _grid.GridCellAttrProvider_UpdateAttrRows(*args, **kwargs)
    def UpdateAttrCols(*args, **kwargs): return _grid.GridCellAttrProvider_UpdateAttrCols(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellAttrProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridCellAttrProviderPtr(GridCellAttrProvider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellAttrProvider
_grid.GridCellAttrProvider_swigregister(GridCellAttrProviderPtr)

class PyGridCellAttrProvider(GridCellAttrProvider):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_PyGridCellAttrProvider(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyGridCellAttrProvider)
    def _setCallbackInfo(*args, **kwargs): return _grid.PyGridCellAttrProvider__setCallbackInfo(*args, **kwargs)
    def base_GetAttr(*args, **kwargs): return _grid.PyGridCellAttrProvider_base_GetAttr(*args, **kwargs)
    def base_SetAttr(*args, **kwargs): return _grid.PyGridCellAttrProvider_base_SetAttr(*args, **kwargs)
    def base_SetRowAttr(*args, **kwargs): return _grid.PyGridCellAttrProvider_base_SetRowAttr(*args, **kwargs)
    def base_SetColAttr(*args, **kwargs): return _grid.PyGridCellAttrProvider_base_SetColAttr(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyGridCellAttrProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PyGridCellAttrProviderPtr(PyGridCellAttrProvider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyGridCellAttrProvider
_grid.PyGridCellAttrProvider_swigregister(PyGridCellAttrProviderPtr)

class GridTableBase(core.Object):
    def _setOORInfo(*args, **kwargs): return _grid.GridTableBase__setOORInfo(*args, **kwargs)
    def SetAttrProvider(*args, **kwargs): return _grid.GridTableBase_SetAttrProvider(*args, **kwargs)
    def GetAttrProvider(*args, **kwargs): return _grid.GridTableBase_GetAttrProvider(*args, **kwargs)
    def SetView(*args, **kwargs): return _grid.GridTableBase_SetView(*args, **kwargs)
    def GetView(*args, **kwargs): return _grid.GridTableBase_GetView(*args, **kwargs)
    def GetNumberRows(*args, **kwargs): return _grid.GridTableBase_GetNumberRows(*args, **kwargs)
    def GetNumberCols(*args, **kwargs): return _grid.GridTableBase_GetNumberCols(*args, **kwargs)
    def IsEmptyCell(*args, **kwargs): return _grid.GridTableBase_IsEmptyCell(*args, **kwargs)
    def GetValue(*args, **kwargs): return _grid.GridTableBase_GetValue(*args, **kwargs)
    def SetValue(*args, **kwargs): return _grid.GridTableBase_SetValue(*args, **kwargs)
    def GetTypeName(*args, **kwargs): return _grid.GridTableBase_GetTypeName(*args, **kwargs)
    def CanGetValueAs(*args, **kwargs): return _grid.GridTableBase_CanGetValueAs(*args, **kwargs)
    def CanSetValueAs(*args, **kwargs): return _grid.GridTableBase_CanSetValueAs(*args, **kwargs)
    def GetValueAsLong(*args, **kwargs): return _grid.GridTableBase_GetValueAsLong(*args, **kwargs)
    def GetValueAsDouble(*args, **kwargs): return _grid.GridTableBase_GetValueAsDouble(*args, **kwargs)
    def GetValueAsBool(*args, **kwargs): return _grid.GridTableBase_GetValueAsBool(*args, **kwargs)
    def SetValueAsLong(*args, **kwargs): return _grid.GridTableBase_SetValueAsLong(*args, **kwargs)
    def SetValueAsDouble(*args, **kwargs): return _grid.GridTableBase_SetValueAsDouble(*args, **kwargs)
    def SetValueAsBool(*args, **kwargs): return _grid.GridTableBase_SetValueAsBool(*args, **kwargs)
    def Clear(*args, **kwargs): return _grid.GridTableBase_Clear(*args, **kwargs)
    def InsertRows(*args, **kwargs): return _grid.GridTableBase_InsertRows(*args, **kwargs)
    def AppendRows(*args, **kwargs): return _grid.GridTableBase_AppendRows(*args, **kwargs)
    def DeleteRows(*args, **kwargs): return _grid.GridTableBase_DeleteRows(*args, **kwargs)
    def InsertCols(*args, **kwargs): return _grid.GridTableBase_InsertCols(*args, **kwargs)
    def AppendCols(*args, **kwargs): return _grid.GridTableBase_AppendCols(*args, **kwargs)
    def DeleteCols(*args, **kwargs): return _grid.GridTableBase_DeleteCols(*args, **kwargs)
    def GetRowLabelValue(*args, **kwargs): return _grid.GridTableBase_GetRowLabelValue(*args, **kwargs)
    def GetColLabelValue(*args, **kwargs): return _grid.GridTableBase_GetColLabelValue(*args, **kwargs)
    def SetRowLabelValue(*args, **kwargs): return _grid.GridTableBase_SetRowLabelValue(*args, **kwargs)
    def SetColLabelValue(*args, **kwargs): return _grid.GridTableBase_SetColLabelValue(*args, **kwargs)
    def CanHaveAttributes(*args, **kwargs): return _grid.GridTableBase_CanHaveAttributes(*args, **kwargs)
    def GetAttr(*args, **kwargs): return _grid.GridTableBase_GetAttr(*args, **kwargs)
    def SetAttr(*args, **kwargs): return _grid.GridTableBase_SetAttr(*args, **kwargs)
    def SetRowAttr(*args, **kwargs): return _grid.GridTableBase_SetRowAttr(*args, **kwargs)
    def SetColAttr(*args, **kwargs): return _grid.GridTableBase_SetColAttr(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridTableBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridTableBasePtr(GridTableBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridTableBase
_grid.GridTableBase_swigregister(GridTableBasePtr)

class PyGridTableBase(GridTableBase):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_PyGridTableBase(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyGridTableBase);self._setOORInfo(self)
    def _setCallbackInfo(*args, **kwargs): return _grid.PyGridTableBase__setCallbackInfo(*args, **kwargs)
    def Destroy(*args, **kwargs): return _grid.PyGridTableBase_Destroy(*args, **kwargs)
    def base_GetTypeName(*args, **kwargs): return _grid.PyGridTableBase_base_GetTypeName(*args, **kwargs)
    def base_CanGetValueAs(*args, **kwargs): return _grid.PyGridTableBase_base_CanGetValueAs(*args, **kwargs)
    def base_CanSetValueAs(*args, **kwargs): return _grid.PyGridTableBase_base_CanSetValueAs(*args, **kwargs)
    def base_Clear(*args, **kwargs): return _grid.PyGridTableBase_base_Clear(*args, **kwargs)
    def base_InsertRows(*args, **kwargs): return _grid.PyGridTableBase_base_InsertRows(*args, **kwargs)
    def base_AppendRows(*args, **kwargs): return _grid.PyGridTableBase_base_AppendRows(*args, **kwargs)
    def base_DeleteRows(*args, **kwargs): return _grid.PyGridTableBase_base_DeleteRows(*args, **kwargs)
    def base_InsertCols(*args, **kwargs): return _grid.PyGridTableBase_base_InsertCols(*args, **kwargs)
    def base_AppendCols(*args, **kwargs): return _grid.PyGridTableBase_base_AppendCols(*args, **kwargs)
    def base_DeleteCols(*args, **kwargs): return _grid.PyGridTableBase_base_DeleteCols(*args, **kwargs)
    def base_GetRowLabelValue(*args, **kwargs): return _grid.PyGridTableBase_base_GetRowLabelValue(*args, **kwargs)
    def base_GetColLabelValue(*args, **kwargs): return _grid.PyGridTableBase_base_GetColLabelValue(*args, **kwargs)
    def base_SetRowLabelValue(*args, **kwargs): return _grid.PyGridTableBase_base_SetRowLabelValue(*args, **kwargs)
    def base_SetColLabelValue(*args, **kwargs): return _grid.PyGridTableBase_base_SetColLabelValue(*args, **kwargs)
    def base_CanHaveAttributes(*args, **kwargs): return _grid.PyGridTableBase_base_CanHaveAttributes(*args, **kwargs)
    def base_GetAttr(*args, **kwargs): return _grid.PyGridTableBase_base_GetAttr(*args, **kwargs)
    def base_SetAttr(*args, **kwargs): return _grid.PyGridTableBase_base_SetAttr(*args, **kwargs)
    def base_SetRowAttr(*args, **kwargs): return _grid.PyGridTableBase_base_SetRowAttr(*args, **kwargs)
    def base_SetColAttr(*args, **kwargs): return _grid.PyGridTableBase_base_SetColAttr(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyGridTableBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PyGridTableBasePtr(PyGridTableBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyGridTableBase
_grid.PyGridTableBase_swigregister(PyGridTableBasePtr)

class GridStringTable(GridTableBase):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridStringTable(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridStringTable instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridStringTablePtr(GridStringTable):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridStringTable
_grid.GridStringTable_swigregister(GridStringTablePtr)

GRIDTABLE_REQUEST_VIEW_GET_VALUES = _grid.GRIDTABLE_REQUEST_VIEW_GET_VALUES
GRIDTABLE_REQUEST_VIEW_SEND_VALUES = _grid.GRIDTABLE_REQUEST_VIEW_SEND_VALUES
GRIDTABLE_NOTIFY_ROWS_INSERTED = _grid.GRIDTABLE_NOTIFY_ROWS_INSERTED
GRIDTABLE_NOTIFY_ROWS_APPENDED = _grid.GRIDTABLE_NOTIFY_ROWS_APPENDED
GRIDTABLE_NOTIFY_ROWS_DELETED = _grid.GRIDTABLE_NOTIFY_ROWS_DELETED
GRIDTABLE_NOTIFY_COLS_INSERTED = _grid.GRIDTABLE_NOTIFY_COLS_INSERTED
GRIDTABLE_NOTIFY_COLS_APPENDED = _grid.GRIDTABLE_NOTIFY_COLS_APPENDED
GRIDTABLE_NOTIFY_COLS_DELETED = _grid.GRIDTABLE_NOTIFY_COLS_DELETED
class GridTableMessage(object):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridTableMessage(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_grid.delete_GridTableMessage):
        try:
            if self.thisown: destroy(self)
        except: pass
    def SetTableObject(*args, **kwargs): return _grid.GridTableMessage_SetTableObject(*args, **kwargs)
    def GetTableObject(*args, **kwargs): return _grid.GridTableMessage_GetTableObject(*args, **kwargs)
    def SetId(*args, **kwargs): return _grid.GridTableMessage_SetId(*args, **kwargs)
    def GetId(*args, **kwargs): return _grid.GridTableMessage_GetId(*args, **kwargs)
    def SetCommandInt(*args, **kwargs): return _grid.GridTableMessage_SetCommandInt(*args, **kwargs)
    def GetCommandInt(*args, **kwargs): return _grid.GridTableMessage_GetCommandInt(*args, **kwargs)
    def SetCommandInt2(*args, **kwargs): return _grid.GridTableMessage_SetCommandInt2(*args, **kwargs)
    def GetCommandInt2(*args, **kwargs): return _grid.GridTableMessage_GetCommandInt2(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridTableMessage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridTableMessagePtr(GridTableMessage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridTableMessage
_grid.GridTableMessage_swigregister(GridTableMessagePtr)

class GridCellCoords(object):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridCellCoords(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_grid.delete_GridCellCoords):
        try:
            if self.thisown: destroy(self)
        except: pass
    def GetRow(*args, **kwargs): return _grid.GridCellCoords_GetRow(*args, **kwargs)
    def SetRow(*args, **kwargs): return _grid.GridCellCoords_SetRow(*args, **kwargs)
    def GetCol(*args, **kwargs): return _grid.GridCellCoords_GetCol(*args, **kwargs)
    def SetCol(*args, **kwargs): return _grid.GridCellCoords_SetCol(*args, **kwargs)
    def Set(*args, **kwargs): return _grid.GridCellCoords_Set(*args, **kwargs)
    def __eq__(*args, **kwargs): return _grid.GridCellCoords___eq__(*args, **kwargs)
    def __ne__(*args, **kwargs): return _grid.GridCellCoords___ne__(*args, **kwargs)
    def asTuple(*args, **kwargs): return _grid.GridCellCoords_asTuple(*args, **kwargs)
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxGridCellCoords'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.SetRow(val)
        elif index == 1: self.SetCol(val)
        else: raise IndexError


class GridCellCoordsPtr(GridCellCoords):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellCoords
_grid.GridCellCoords_swigregister(GridCellCoordsPtr)

class Grid(windows.ScrolledWindow):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_Grid(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)
    wxGridSelectCells = _grid.Grid_wxGridSelectCells
    wxGridSelectRows = _grid.Grid_wxGridSelectRows
    wxGridSelectColumns = _grid.Grid_wxGridSelectColumns
    def CreateGrid(*args, **kwargs): return _grid.Grid_CreateGrid(*args, **kwargs)
    def SetSelectionMode(*args, **kwargs): return _grid.Grid_SetSelectionMode(*args, **kwargs)
    def GetSelectionMode(*args, **kwargs): return _grid.Grid_GetSelectionMode(*args, **kwargs)
    def GetNumberRows(*args, **kwargs): return _grid.Grid_GetNumberRows(*args, **kwargs)
    def GetNumberCols(*args, **kwargs): return _grid.Grid_GetNumberCols(*args, **kwargs)
    def ProcessTableMessage(*args, **kwargs): return _grid.Grid_ProcessTableMessage(*args, **kwargs)
    def GetTable(*args, **kwargs): return _grid.Grid_GetTable(*args, **kwargs)
    def SetTable(*args, **kwargs): return _grid.Grid_SetTable(*args, **kwargs)
    def ClearGrid(*args, **kwargs): return _grid.Grid_ClearGrid(*args, **kwargs)
    def InsertRows(*args, **kwargs): return _grid.Grid_InsertRows(*args, **kwargs)
    def AppendRows(*args, **kwargs): return _grid.Grid_AppendRows(*args, **kwargs)
    def DeleteRows(*args, **kwargs): return _grid.Grid_DeleteRows(*args, **kwargs)
    def InsertCols(*args, **kwargs): return _grid.Grid_InsertCols(*args, **kwargs)
    def AppendCols(*args, **kwargs): return _grid.Grid_AppendCols(*args, **kwargs)
    def DeleteCols(*args, **kwargs): return _grid.Grid_DeleteCols(*args, **kwargs)
    def DrawCellHighlight(*args, **kwargs): return _grid.Grid_DrawCellHighlight(*args, **kwargs)
    def DrawTextRectangle(*args, **kwargs): return _grid.Grid_DrawTextRectangle(*args, **kwargs)
    def GetTextBoxSize(*args, **kwargs): return _grid.Grid_GetTextBoxSize(*args, **kwargs)
    def BeginBatch(*args, **kwargs): return _grid.Grid_BeginBatch(*args, **kwargs)
    def EndBatch(*args, **kwargs): return _grid.Grid_EndBatch(*args, **kwargs)
    def GetBatchCount(*args, **kwargs): return _grid.Grid_GetBatchCount(*args, **kwargs)
    def ForceRefresh(*args, **kwargs): return _grid.Grid_ForceRefresh(*args, **kwargs)
    def Refresh(*args, **kwargs): return _grid.Grid_Refresh(*args, **kwargs)
    def IsEditable(*args, **kwargs): return _grid.Grid_IsEditable(*args, **kwargs)
    def EnableEditing(*args, **kwargs): return _grid.Grid_EnableEditing(*args, **kwargs)
    def EnableCellEditControl(*args, **kwargs): return _grid.Grid_EnableCellEditControl(*args, **kwargs)
    def DisableCellEditControl(*args, **kwargs): return _grid.Grid_DisableCellEditControl(*args, **kwargs)
    def CanEnableCellControl(*args, **kwargs): return _grid.Grid_CanEnableCellControl(*args, **kwargs)
    def IsCellEditControlEnabled(*args, **kwargs): return _grid.Grid_IsCellEditControlEnabled(*args, **kwargs)
    def IsCellEditControlShown(*args, **kwargs): return _grid.Grid_IsCellEditControlShown(*args, **kwargs)
    def IsCurrentCellReadOnly(*args, **kwargs): return _grid.Grid_IsCurrentCellReadOnly(*args, **kwargs)
    def ShowCellEditControl(*args, **kwargs): return _grid.Grid_ShowCellEditControl(*args, **kwargs)
    def HideCellEditControl(*args, **kwargs): return _grid.Grid_HideCellEditControl(*args, **kwargs)
    def SaveEditControlValue(*args, **kwargs): return _grid.Grid_SaveEditControlValue(*args, **kwargs)
    def XYToCell(*args, **kwargs): return _grid.Grid_XYToCell(*args, **kwargs)
    def YToRow(*args, **kwargs): return _grid.Grid_YToRow(*args, **kwargs)
    def XToCol(*args, **kwargs): return _grid.Grid_XToCol(*args, **kwargs)
    def YToEdgeOfRow(*args, **kwargs): return _grid.Grid_YToEdgeOfRow(*args, **kwargs)
    def XToEdgeOfCol(*args, **kwargs): return _grid.Grid_XToEdgeOfCol(*args, **kwargs)
    def CellToRect(*args, **kwargs): return _grid.Grid_CellToRect(*args, **kwargs)
    def GetGridCursorRow(*args, **kwargs): return _grid.Grid_GetGridCursorRow(*args, **kwargs)
    def GetGridCursorCol(*args, **kwargs): return _grid.Grid_GetGridCursorCol(*args, **kwargs)
    def IsVisible(*args, **kwargs): return _grid.Grid_IsVisible(*args, **kwargs)
    def MakeCellVisible(*args, **kwargs): return _grid.Grid_MakeCellVisible(*args, **kwargs)
    def SetGridCursor(*args, **kwargs): return _grid.Grid_SetGridCursor(*args, **kwargs)
    def MoveCursorUp(*args, **kwargs): return _grid.Grid_MoveCursorUp(*args, **kwargs)
    def MoveCursorDown(*args, **kwargs): return _grid.Grid_MoveCursorDown(*args, **kwargs)
    def MoveCursorLeft(*args, **kwargs): return _grid.Grid_MoveCursorLeft(*args, **kwargs)
    def MoveCursorRight(*args, **kwargs): return _grid.Grid_MoveCursorRight(*args, **kwargs)
    def MovePageDown(*args, **kwargs): return _grid.Grid_MovePageDown(*args, **kwargs)
    def MovePageUp(*args, **kwargs): return _grid.Grid_MovePageUp(*args, **kwargs)
    def MoveCursorUpBlock(*args, **kwargs): return _grid.Grid_MoveCursorUpBlock(*args, **kwargs)
    def MoveCursorDownBlock(*args, **kwargs): return _grid.Grid_MoveCursorDownBlock(*args, **kwargs)
    def MoveCursorLeftBlock(*args, **kwargs): return _grid.Grid_MoveCursorLeftBlock(*args, **kwargs)
    def MoveCursorRightBlock(*args, **kwargs): return _grid.Grid_MoveCursorRightBlock(*args, **kwargs)
    def GetDefaultRowLabelSize(*args, **kwargs): return _grid.Grid_GetDefaultRowLabelSize(*args, **kwargs)
    def GetRowLabelSize(*args, **kwargs): return _grid.Grid_GetRowLabelSize(*args, **kwargs)
    def GetDefaultColLabelSize(*args, **kwargs): return _grid.Grid_GetDefaultColLabelSize(*args, **kwargs)
    def GetColLabelSize(*args, **kwargs): return _grid.Grid_GetColLabelSize(*args, **kwargs)
    def GetLabelBackgroundColour(*args, **kwargs): return _grid.Grid_GetLabelBackgroundColour(*args, **kwargs)
    def GetLabelTextColour(*args, **kwargs): return _grid.Grid_GetLabelTextColour(*args, **kwargs)
    def GetLabelFont(*args, **kwargs): return _grid.Grid_GetLabelFont(*args, **kwargs)
    def GetRowLabelAlignment(*args, **kwargs): return _grid.Grid_GetRowLabelAlignment(*args, **kwargs)
    def GetColLabelAlignment(*args, **kwargs): return _grid.Grid_GetColLabelAlignment(*args, **kwargs)
    def GetColLabelTextOrientation(*args, **kwargs): return _grid.Grid_GetColLabelTextOrientation(*args, **kwargs)
    def GetRowLabelValue(*args, **kwargs): return _grid.Grid_GetRowLabelValue(*args, **kwargs)
    def GetColLabelValue(*args, **kwargs): return _grid.Grid_GetColLabelValue(*args, **kwargs)
    def GetGridLineColour(*args, **kwargs): return _grid.Grid_GetGridLineColour(*args, **kwargs)
    def GetCellHighlightColour(*args, **kwargs): return _grid.Grid_GetCellHighlightColour(*args, **kwargs)
    def GetCellHighlightPenWidth(*args, **kwargs): return _grid.Grid_GetCellHighlightPenWidth(*args, **kwargs)
    def GetCellHighlightROPenWidth(*args, **kwargs): return _grid.Grid_GetCellHighlightROPenWidth(*args, **kwargs)
    def SetRowLabelSize(*args, **kwargs): return _grid.Grid_SetRowLabelSize(*args, **kwargs)
    def SetColLabelSize(*args, **kwargs): return _grid.Grid_SetColLabelSize(*args, **kwargs)
    def SetLabelBackgroundColour(*args, **kwargs): return _grid.Grid_SetLabelBackgroundColour(*args, **kwargs)
    def SetLabelTextColour(*args, **kwargs): return _grid.Grid_SetLabelTextColour(*args, **kwargs)
    def SetLabelFont(*args, **kwargs): return _grid.Grid_SetLabelFont(*args, **kwargs)
    def SetRowLabelAlignment(*args, **kwargs): return _grid.Grid_SetRowLabelAlignment(*args, **kwargs)
    def SetColLabelAlignment(*args, **kwargs): return _grid.Grid_SetColLabelAlignment(*args, **kwargs)
    def SetColLabelTextOrientation(*args, **kwargs): return _grid.Grid_SetColLabelTextOrientation(*args, **kwargs)
    def SetRowLabelValue(*args, **kwargs): return _grid.Grid_SetRowLabelValue(*args, **kwargs)
    def SetColLabelValue(*args, **kwargs): return _grid.Grid_SetColLabelValue(*args, **kwargs)
    def SetGridLineColour(*args, **kwargs): return _grid.Grid_SetGridLineColour(*args, **kwargs)
    def SetCellHighlightColour(*args, **kwargs): return _grid.Grid_SetCellHighlightColour(*args, **kwargs)
    def SetCellHighlightPenWidth(*args, **kwargs): return _grid.Grid_SetCellHighlightPenWidth(*args, **kwargs)
    def SetCellHighlightROPenWidth(*args, **kwargs): return _grid.Grid_SetCellHighlightROPenWidth(*args, **kwargs)
    def EnableDragRowSize(*args, **kwargs): return _grid.Grid_EnableDragRowSize(*args, **kwargs)
    def DisableDragRowSize(*args, **kwargs): return _grid.Grid_DisableDragRowSize(*args, **kwargs)
    def CanDragRowSize(*args, **kwargs): return _grid.Grid_CanDragRowSize(*args, **kwargs)
    def EnableDragColSize(*args, **kwargs): return _grid.Grid_EnableDragColSize(*args, **kwargs)
    def DisableDragColSize(*args, **kwargs): return _grid.Grid_DisableDragColSize(*args, **kwargs)
    def CanDragColSize(*args, **kwargs): return _grid.Grid_CanDragColSize(*args, **kwargs)
    def EnableDragGridSize(*args, **kwargs): return _grid.Grid_EnableDragGridSize(*args, **kwargs)
    def DisableDragGridSize(*args, **kwargs): return _grid.Grid_DisableDragGridSize(*args, **kwargs)
    def CanDragGridSize(*args, **kwargs): return _grid.Grid_CanDragGridSize(*args, **kwargs)
    def SetAttr(*args, **kwargs): return _grid.Grid_SetAttr(*args, **kwargs)
    def SetRowAttr(*args, **kwargs): return _grid.Grid_SetRowAttr(*args, **kwargs)
    def SetColAttr(*args, **kwargs): return _grid.Grid_SetColAttr(*args, **kwargs)
    def SetColFormatBool(*args, **kwargs): return _grid.Grid_SetColFormatBool(*args, **kwargs)
    def SetColFormatNumber(*args, **kwargs): return _grid.Grid_SetColFormatNumber(*args, **kwargs)
    def SetColFormatFloat(*args, **kwargs): return _grid.Grid_SetColFormatFloat(*args, **kwargs)
    def SetColFormatCustom(*args, **kwargs): return _grid.Grid_SetColFormatCustom(*args, **kwargs)
    def EnableGridLines(*args, **kwargs): return _grid.Grid_EnableGridLines(*args, **kwargs)
    def GridLinesEnabled(*args, **kwargs): return _grid.Grid_GridLinesEnabled(*args, **kwargs)
    def GetDefaultRowSize(*args, **kwargs): return _grid.Grid_GetDefaultRowSize(*args, **kwargs)
    def GetRowSize(*args, **kwargs): return _grid.Grid_GetRowSize(*args, **kwargs)
    def GetDefaultColSize(*args, **kwargs): return _grid.Grid_GetDefaultColSize(*args, **kwargs)
    def GetColSize(*args, **kwargs): return _grid.Grid_GetColSize(*args, **kwargs)
    def GetDefaultCellBackgroundColour(*args, **kwargs): return _grid.Grid_GetDefaultCellBackgroundColour(*args, **kwargs)
    def GetCellBackgroundColour(*args, **kwargs): return _grid.Grid_GetCellBackgroundColour(*args, **kwargs)
    def GetDefaultCellTextColour(*args, **kwargs): return _grid.Grid_GetDefaultCellTextColour(*args, **kwargs)
    def GetCellTextColour(*args, **kwargs): return _grid.Grid_GetCellTextColour(*args, **kwargs)
    def GetDefaultCellFont(*args, **kwargs): return _grid.Grid_GetDefaultCellFont(*args, **kwargs)
    def GetCellFont(*args, **kwargs): return _grid.Grid_GetCellFont(*args, **kwargs)
    def GetDefaultCellAlignment(*args, **kwargs): return _grid.Grid_GetDefaultCellAlignment(*args, **kwargs)
    def GetCellAlignment(*args, **kwargs): return _grid.Grid_GetCellAlignment(*args, **kwargs)
    def GetDefaultCellOverflow(*args, **kwargs): return _grid.Grid_GetDefaultCellOverflow(*args, **kwargs)
    def GetCellOverflow(*args, **kwargs): return _grid.Grid_GetCellOverflow(*args, **kwargs)
    def GetCellSize(*args, **kwargs): return _grid.Grid_GetCellSize(*args, **kwargs)
    def SetDefaultRowSize(*args, **kwargs): return _grid.Grid_SetDefaultRowSize(*args, **kwargs)
    def SetRowSize(*args, **kwargs): return _grid.Grid_SetRowSize(*args, **kwargs)
    def SetDefaultColSize(*args, **kwargs): return _grid.Grid_SetDefaultColSize(*args, **kwargs)
    def SetColSize(*args, **kwargs): return _grid.Grid_SetColSize(*args, **kwargs)
    def AutoSizeColumn(*args, **kwargs): return _grid.Grid_AutoSizeColumn(*args, **kwargs)
    def AutoSizeRow(*args, **kwargs): return _grid.Grid_AutoSizeRow(*args, **kwargs)
    def AutoSizeColumns(*args, **kwargs): return _grid.Grid_AutoSizeColumns(*args, **kwargs)
    def AutoSizeRows(*args, **kwargs): return _grid.Grid_AutoSizeRows(*args, **kwargs)
    def AutoSize(*args, **kwargs): return _grid.Grid_AutoSize(*args, **kwargs)
    def AutoSizeRowLabelSize(*args, **kwargs): return _grid.Grid_AutoSizeRowLabelSize(*args, **kwargs)
    def AutoSizeColLabelSize(*args, **kwargs): return _grid.Grid_AutoSizeColLabelSize(*args, **kwargs)
    def SetColMinimalWidth(*args, **kwargs): return _grid.Grid_SetColMinimalWidth(*args, **kwargs)
    def SetRowMinimalHeight(*args, **kwargs): return _grid.Grid_SetRowMinimalHeight(*args, **kwargs)
    def SetColMinimalAcceptableWidth(*args, **kwargs): return _grid.Grid_SetColMinimalAcceptableWidth(*args, **kwargs)
    def SetRowMinimalAcceptableHeight(*args, **kwargs): return _grid.Grid_SetRowMinimalAcceptableHeight(*args, **kwargs)
    def GetColMinimalAcceptableWidth(*args, **kwargs): return _grid.Grid_GetColMinimalAcceptableWidth(*args, **kwargs)
    def GetRowMinimalAcceptableHeight(*args, **kwargs): return _grid.Grid_GetRowMinimalAcceptableHeight(*args, **kwargs)
    def SetDefaultCellBackgroundColour(*args, **kwargs): return _grid.Grid_SetDefaultCellBackgroundColour(*args, **kwargs)
    def SetCellBackgroundColour(*args, **kwargs): return _grid.Grid_SetCellBackgroundColour(*args, **kwargs)
    def SetDefaultCellTextColour(*args, **kwargs): return _grid.Grid_SetDefaultCellTextColour(*args, **kwargs)
    def SetCellTextColour(*args, **kwargs): return _grid.Grid_SetCellTextColour(*args, **kwargs)
    def SetDefaultCellFont(*args, **kwargs): return _grid.Grid_SetDefaultCellFont(*args, **kwargs)
    def SetCellFont(*args, **kwargs): return _grid.Grid_SetCellFont(*args, **kwargs)
    def SetDefaultCellAlignment(*args, **kwargs): return _grid.Grid_SetDefaultCellAlignment(*args, **kwargs)
    def SetCellAlignment(*args, **kwargs): return _grid.Grid_SetCellAlignment(*args, **kwargs)
    def SetDefaultCellOverflow(*args, **kwargs): return _grid.Grid_SetDefaultCellOverflow(*args, **kwargs)
    def SetCellOverflow(*args, **kwargs): return _grid.Grid_SetCellOverflow(*args, **kwargs)
    def SetCellSize(*args, **kwargs): return _grid.Grid_SetCellSize(*args, **kwargs)
    def SetDefaultRenderer(*args, **kwargs): return _grid.Grid_SetDefaultRenderer(*args, **kwargs)
    def SetCellRenderer(*args, **kwargs): return _grid.Grid_SetCellRenderer(*args, **kwargs)
    def GetDefaultRenderer(*args, **kwargs): return _grid.Grid_GetDefaultRenderer(*args, **kwargs)
    def GetCellRenderer(*args, **kwargs): return _grid.Grid_GetCellRenderer(*args, **kwargs)
    def SetDefaultEditor(*args, **kwargs): return _grid.Grid_SetDefaultEditor(*args, **kwargs)
    def SetCellEditor(*args, **kwargs): return _grid.Grid_SetCellEditor(*args, **kwargs)
    def GetDefaultEditor(*args, **kwargs): return _grid.Grid_GetDefaultEditor(*args, **kwargs)
    def GetCellEditor(*args, **kwargs): return _grid.Grid_GetCellEditor(*args, **kwargs)
    def GetCellValue(*args, **kwargs): return _grid.Grid_GetCellValue(*args, **kwargs)
    def SetCellValue(*args, **kwargs): return _grid.Grid_SetCellValue(*args, **kwargs)
    def IsReadOnly(*args, **kwargs): return _grid.Grid_IsReadOnly(*args, **kwargs)
    def SetReadOnly(*args, **kwargs): return _grid.Grid_SetReadOnly(*args, **kwargs)
    def SelectRow(*args, **kwargs): return _grid.Grid_SelectRow(*args, **kwargs)
    def SelectCol(*args, **kwargs): return _grid.Grid_SelectCol(*args, **kwargs)
    def SelectBlock(*args, **kwargs): return _grid.Grid_SelectBlock(*args, **kwargs)
    def SelectAll(*args, **kwargs): return _grid.Grid_SelectAll(*args, **kwargs)
    def IsSelection(*args, **kwargs): return _grid.Grid_IsSelection(*args, **kwargs)
    def ClearSelection(*args, **kwargs): return _grid.Grid_ClearSelection(*args, **kwargs)
    def IsInSelection(*args, **kwargs): return _grid.Grid_IsInSelection(*args, **kwargs)
    def GetSelectedCells(*args, **kwargs): return _grid.Grid_GetSelectedCells(*args, **kwargs)
    def GetSelectionBlockTopLeft(*args, **kwargs): return _grid.Grid_GetSelectionBlockTopLeft(*args, **kwargs)
    def GetSelectionBlockBottomRight(*args, **kwargs): return _grid.Grid_GetSelectionBlockBottomRight(*args, **kwargs)
    def GetSelectedRows(*args, **kwargs): return _grid.Grid_GetSelectedRows(*args, **kwargs)
    def GetSelectedCols(*args, **kwargs): return _grid.Grid_GetSelectedCols(*args, **kwargs)
    def DeselectRow(*args, **kwargs): return _grid.Grid_DeselectRow(*args, **kwargs)
    def DeselectCol(*args, **kwargs): return _grid.Grid_DeselectCol(*args, **kwargs)
    def DeselectCell(*args, **kwargs): return _grid.Grid_DeselectCell(*args, **kwargs)
    def BlockToDeviceRect(*args, **kwargs): return _grid.Grid_BlockToDeviceRect(*args, **kwargs)
    def GetSelectionBackground(*args, **kwargs): return _grid.Grid_GetSelectionBackground(*args, **kwargs)
    def GetSelectionForeground(*args, **kwargs): return _grid.Grid_GetSelectionForeground(*args, **kwargs)
    def SetSelectionBackground(*args, **kwargs): return _grid.Grid_SetSelectionBackground(*args, **kwargs)
    def SetSelectionForeground(*args, **kwargs): return _grid.Grid_SetSelectionForeground(*args, **kwargs)
    def RegisterDataType(*args, **kwargs): return _grid.Grid_RegisterDataType(*args, **kwargs)
    def GetDefaultEditorForCell(*args, **kwargs): return _grid.Grid_GetDefaultEditorForCell(*args, **kwargs)
    def GetDefaultRendererForCell(*args, **kwargs): return _grid.Grid_GetDefaultRendererForCell(*args, **kwargs)
    def GetDefaultEditorForType(*args, **kwargs): return _grid.Grid_GetDefaultEditorForType(*args, **kwargs)
    def GetDefaultRendererForType(*args, **kwargs): return _grid.Grid_GetDefaultRendererForType(*args, **kwargs)
    def SetMargins(*args, **kwargs): return _grid.Grid_SetMargins(*args, **kwargs)
    def GetGridWindow(*args, **kwargs): return _grid.Grid_GetGridWindow(*args, **kwargs)
    def GetGridRowLabelWindow(*args, **kwargs): return _grid.Grid_GetGridRowLabelWindow(*args, **kwargs)
    def GetGridColLabelWindow(*args, **kwargs): return _grid.Grid_GetGridColLabelWindow(*args, **kwargs)
    def GetGridCornerLabelWindow(*args, **kwargs): return _grid.Grid_GetGridCornerLabelWindow(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGrid instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridPtr(Grid):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Grid
_grid.Grid_swigregister(GridPtr)

class GridEvent(core.NotifyEvent):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRow(*args, **kwargs): return _grid.GridEvent_GetRow(*args, **kwargs)
    def GetCol(*args, **kwargs): return _grid.GridEvent_GetCol(*args, **kwargs)
    def GetPosition(*args, **kwargs): return _grid.GridEvent_GetPosition(*args, **kwargs)
    def Selecting(*args, **kwargs): return _grid.GridEvent_Selecting(*args, **kwargs)
    def ControlDown(*args, **kwargs): return _grid.GridEvent_ControlDown(*args, **kwargs)
    def MetaDown(*args, **kwargs): return _grid.GridEvent_MetaDown(*args, **kwargs)
    def ShiftDown(*args, **kwargs): return _grid.GridEvent_ShiftDown(*args, **kwargs)
    def AltDown(*args, **kwargs): return _grid.GridEvent_AltDown(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridEventPtr(GridEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridEvent
_grid.GridEvent_swigregister(GridEventPtr)

class GridSizeEvent(core.NotifyEvent):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridSizeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRowOrCol(*args, **kwargs): return _grid.GridSizeEvent_GetRowOrCol(*args, **kwargs)
    def GetPosition(*args, **kwargs): return _grid.GridSizeEvent_GetPosition(*args, **kwargs)
    def ControlDown(*args, **kwargs): return _grid.GridSizeEvent_ControlDown(*args, **kwargs)
    def MetaDown(*args, **kwargs): return _grid.GridSizeEvent_MetaDown(*args, **kwargs)
    def ShiftDown(*args, **kwargs): return _grid.GridSizeEvent_ShiftDown(*args, **kwargs)
    def AltDown(*args, **kwargs): return _grid.GridSizeEvent_AltDown(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridSizeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridSizeEventPtr(GridSizeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridSizeEvent
_grid.GridSizeEvent_swigregister(GridSizeEventPtr)

class GridRangeSelectEvent(core.NotifyEvent):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridRangeSelectEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetTopLeftCoords(*args, **kwargs): return _grid.GridRangeSelectEvent_GetTopLeftCoords(*args, **kwargs)
    def GetBottomRightCoords(*args, **kwargs): return _grid.GridRangeSelectEvent_GetBottomRightCoords(*args, **kwargs)
    def GetTopRow(*args, **kwargs): return _grid.GridRangeSelectEvent_GetTopRow(*args, **kwargs)
    def GetBottomRow(*args, **kwargs): return _grid.GridRangeSelectEvent_GetBottomRow(*args, **kwargs)
    def GetLeftCol(*args, **kwargs): return _grid.GridRangeSelectEvent_GetLeftCol(*args, **kwargs)
    def GetRightCol(*args, **kwargs): return _grid.GridRangeSelectEvent_GetRightCol(*args, **kwargs)
    def Selecting(*args, **kwargs): return _grid.GridRangeSelectEvent_Selecting(*args, **kwargs)
    def ControlDown(*args, **kwargs): return _grid.GridRangeSelectEvent_ControlDown(*args, **kwargs)
    def MetaDown(*args, **kwargs): return _grid.GridRangeSelectEvent_MetaDown(*args, **kwargs)
    def ShiftDown(*args, **kwargs): return _grid.GridRangeSelectEvent_ShiftDown(*args, **kwargs)
    def AltDown(*args, **kwargs): return _grid.GridRangeSelectEvent_AltDown(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridRangeSelectEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridRangeSelectEventPtr(GridRangeSelectEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridRangeSelectEvent
_grid.GridRangeSelectEvent_swigregister(GridRangeSelectEventPtr)

class GridEditorCreatedEvent(core.CommandEvent):
    def __init__(self, *args, **kwargs):
        newobj = _grid.new_GridEditorCreatedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRow(*args, **kwargs): return _grid.GridEditorCreatedEvent_GetRow(*args, **kwargs)
    def GetCol(*args, **kwargs): return _grid.GridEditorCreatedEvent_GetCol(*args, **kwargs)
    def GetControl(*args, **kwargs): return _grid.GridEditorCreatedEvent_GetControl(*args, **kwargs)
    def SetRow(*args, **kwargs): return _grid.GridEditorCreatedEvent_SetRow(*args, **kwargs)
    def SetCol(*args, **kwargs): return _grid.GridEditorCreatedEvent_SetCol(*args, **kwargs)
    def SetControl(*args, **kwargs): return _grid.GridEditorCreatedEvent_SetControl(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridEditorCreatedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GridEditorCreatedEventPtr(GridEditorCreatedEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridEditorCreatedEvent
_grid.GridEditorCreatedEvent_swigregister(GridEditorCreatedEventPtr)

wxEVT_GRID_CELL_LEFT_CLICK = _grid.wxEVT_GRID_CELL_LEFT_CLICK
wxEVT_GRID_CELL_RIGHT_CLICK = _grid.wxEVT_GRID_CELL_RIGHT_CLICK
wxEVT_GRID_CELL_LEFT_DCLICK = _grid.wxEVT_GRID_CELL_LEFT_DCLICK
wxEVT_GRID_CELL_RIGHT_DCLICK = _grid.wxEVT_GRID_CELL_RIGHT_DCLICK
wxEVT_GRID_LABEL_LEFT_CLICK = _grid.wxEVT_GRID_LABEL_LEFT_CLICK
wxEVT_GRID_LABEL_RIGHT_CLICK = _grid.wxEVT_GRID_LABEL_RIGHT_CLICK
wxEVT_GRID_LABEL_LEFT_DCLICK = _grid.wxEVT_GRID_LABEL_LEFT_DCLICK
wxEVT_GRID_LABEL_RIGHT_DCLICK = _grid.wxEVT_GRID_LABEL_RIGHT_DCLICK
wxEVT_GRID_ROW_SIZE = _grid.wxEVT_GRID_ROW_SIZE
wxEVT_GRID_COL_SIZE = _grid.wxEVT_GRID_COL_SIZE
wxEVT_GRID_RANGE_SELECT = _grid.wxEVT_GRID_RANGE_SELECT
wxEVT_GRID_CELL_CHANGE = _grid.wxEVT_GRID_CELL_CHANGE
wxEVT_GRID_SELECT_CELL = _grid.wxEVT_GRID_SELECT_CELL
wxEVT_GRID_EDITOR_SHOWN = _grid.wxEVT_GRID_EDITOR_SHOWN
wxEVT_GRID_EDITOR_HIDDEN = _grid.wxEVT_GRID_EDITOR_HIDDEN
wxEVT_GRID_EDITOR_CREATED = _grid.wxEVT_GRID_EDITOR_CREATED
EVT_GRID_CELL_LEFT_CLICK = wx.PyEventBinder( wxEVT_GRID_CELL_LEFT_CLICK )
EVT_GRID_CELL_RIGHT_CLICK = wx.PyEventBinder( wxEVT_GRID_CELL_RIGHT_CLICK )
EVT_GRID_CELL_LEFT_DCLICK = wx.PyEventBinder( wxEVT_GRID_CELL_LEFT_DCLICK )
EVT_GRID_CELL_RIGHT_DCLICK = wx.PyEventBinder( wxEVT_GRID_CELL_RIGHT_DCLICK )
EVT_GRID_LABEL_LEFT_CLICK = wx.PyEventBinder( wxEVT_GRID_LABEL_LEFT_CLICK )
EVT_GRID_LABEL_RIGHT_CLICK = wx.PyEventBinder( wxEVT_GRID_LABEL_RIGHT_CLICK )
EVT_GRID_LABEL_LEFT_DCLICK = wx.PyEventBinder( wxEVT_GRID_LABEL_LEFT_DCLICK )
EVT_GRID_LABEL_RIGHT_DCLICK = wx.PyEventBinder( wxEVT_GRID_LABEL_RIGHT_DCLICK )
EVT_GRID_ROW_SIZE = wx.PyEventBinder( wxEVT_GRID_ROW_SIZE )
EVT_GRID_COL_SIZE = wx.PyEventBinder( wxEVT_GRID_COL_SIZE )
EVT_GRID_RANGE_SELECT = wx.PyEventBinder( wxEVT_GRID_RANGE_SELECT )
EVT_GRID_CELL_CHANGE = wx.PyEventBinder( wxEVT_GRID_CELL_CHANGE )
EVT_GRID_SELECT_CELL = wx.PyEventBinder( wxEVT_GRID_SELECT_CELL )
EVT_GRID_EDITOR_SHOWN = wx.PyEventBinder( wxEVT_GRID_EDITOR_SHOWN )
EVT_GRID_EDITOR_HIDDEN = wx.PyEventBinder( wxEVT_GRID_EDITOR_HIDDEN )
EVT_GRID_EDITOR_CREATED = wx.PyEventBinder( wxEVT_GRID_EDITOR_CREATED )


