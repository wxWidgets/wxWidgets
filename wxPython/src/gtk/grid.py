# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

"""
Classes for implementing a spreadsheet-like control.
"""

import _grid

def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


def _swig_setattr_nondynamic_method(set):
    def set_attr(self,name,value):
        if hasattr(self,name) or (name in ("this", "thisown")):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import _windows
import _core
wx = _core 
__docfilter__ = wx.__DocFilter(globals()) 
GRID_VALUE_STRING = _grid.GRID_VALUE_STRING
GRID_VALUE_BOOL = _grid.GRID_VALUE_BOOL
GRID_VALUE_NUMBER = _grid.GRID_VALUE_NUMBER
GRID_VALUE_FLOAT = _grid.GRID_VALUE_FLOAT
GRID_VALUE_CHOICE = _grid.GRID_VALUE_CHOICE
GRID_VALUE_TEXT = _grid.GRID_VALUE_TEXT
GRID_VALUE_LONG = _grid.GRID_VALUE_LONG
GRID_VALUE_CHOICEINT = _grid.GRID_VALUE_CHOICEINT
GRID_VALUE_DATETIME = _grid.GRID_VALUE_DATETIME
GRID_DEFAULT_NUMBER_ROWS = _grid.GRID_DEFAULT_NUMBER_ROWS
GRID_DEFAULT_NUMBER_COLS = _grid.GRID_DEFAULT_NUMBER_COLS
GRID_DEFAULT_ROW_HEIGHT = _grid.GRID_DEFAULT_ROW_HEIGHT
GRID_DEFAULT_COL_WIDTH = _grid.GRID_DEFAULT_COL_WIDTH
GRID_DEFAULT_COL_LABEL_HEIGHT = _grid.GRID_DEFAULT_COL_LABEL_HEIGHT
GRID_DEFAULT_ROW_LABEL_WIDTH = _grid.GRID_DEFAULT_ROW_LABEL_WIDTH
GRID_LABEL_EDGE_ZONE = _grid.GRID_LABEL_EDGE_ZONE
GRID_MIN_ROW_HEIGHT = _grid.GRID_MIN_ROW_HEIGHT
GRID_MIN_COL_WIDTH = _grid.GRID_MIN_COL_WIDTH
GRID_DEFAULT_SCROLLBAR_WIDTH = _grid.GRID_DEFAULT_SCROLLBAR_WIDTH
class GridCellRenderer(object):
    """Proxy of C++ GridCellRenderer class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def _setOORInfo(*args, **kwargs):
        """_setOORInfo(self, PyObject _self)"""
        return _grid.GridCellRenderer__setOORInfo(*args, **kwargs)

    def SetParameters(*args, **kwargs):
        """SetParameters(self, String params)"""
        return _grid.GridCellRenderer_SetParameters(*args, **kwargs)

    def IncRef(*args, **kwargs):
        """IncRef(self)"""
        return _grid.GridCellRenderer_IncRef(*args, **kwargs)

    def DecRef(*args, **kwargs):
        """DecRef(self)"""
        return _grid.GridCellRenderer_DecRef(*args, **kwargs)

    def Draw(*args, **kwargs):
        """
        Draw(self, Grid grid, GridCellAttr attr, DC dc, Rect rect, int row, 
            int col, bool isSelected)
        """
        return _grid.GridCellRenderer_Draw(*args, **kwargs)

    def GetBestSize(*args, **kwargs):
        """GetBestSize(self, Grid grid, GridCellAttr attr, DC dc, int row, int col) -> Size"""
        return _grid.GridCellRenderer_GetBestSize(*args, **kwargs)

    def Clone(*args, **kwargs):
        """Clone(self) -> GridCellRenderer"""
        return _grid.GridCellRenderer_Clone(*args, **kwargs)


class GridCellRendererPtr(GridCellRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellRenderer
_grid.GridCellRenderer_swigregister(GridCellRendererPtr)
cvar = _grid.cvar
DateTimeFormatStr = cvar.DateTimeFormatStr
GridNoCellCoords = cvar.GridNoCellCoords
GridNoCellRect = cvar.GridNoCellRect

class PyGridCellRenderer(GridCellRenderer):
    """Proxy of C++ PyGridCellRenderer class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyGridCellRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> PyGridCellRenderer"""
        newobj = _grid.new_PyGridCellRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyGridCellRenderer);self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _grid.PyGridCellRenderer__setCallbackInfo(*args, **kwargs)

    def base_SetParameters(*args, **kwargs):
        """base_SetParameters(self, String params)"""
        return _grid.PyGridCellRenderer_base_SetParameters(*args, **kwargs)


class PyGridCellRendererPtr(PyGridCellRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyGridCellRenderer
_grid.PyGridCellRenderer_swigregister(PyGridCellRendererPtr)

class GridCellStringRenderer(GridCellRenderer):
    """Proxy of C++ GridCellStringRenderer class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellStringRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GridCellStringRenderer"""
        newobj = _grid.new_GridCellStringRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class GridCellStringRendererPtr(GridCellStringRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellStringRenderer
_grid.GridCellStringRenderer_swigregister(GridCellStringRendererPtr)

class GridCellNumberRenderer(GridCellStringRenderer):
    """Proxy of C++ GridCellNumberRenderer class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellNumberRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GridCellNumberRenderer"""
        newobj = _grid.new_GridCellNumberRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class GridCellNumberRendererPtr(GridCellNumberRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellNumberRenderer
_grid.GridCellNumberRenderer_swigregister(GridCellNumberRendererPtr)

class GridCellFloatRenderer(GridCellStringRenderer):
    """Proxy of C++ GridCellFloatRenderer class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellFloatRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int width=-1, int precision=-1) -> GridCellFloatRenderer"""
        newobj = _grid.new_GridCellFloatRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _grid.GridCellFloatRenderer_GetWidth(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(self, int width)"""
        return _grid.GridCellFloatRenderer_SetWidth(*args, **kwargs)

    def GetPrecision(*args, **kwargs):
        """GetPrecision(self) -> int"""
        return _grid.GridCellFloatRenderer_GetPrecision(*args, **kwargs)

    def SetPrecision(*args, **kwargs):
        """SetPrecision(self, int precision)"""
        return _grid.GridCellFloatRenderer_SetPrecision(*args, **kwargs)


class GridCellFloatRendererPtr(GridCellFloatRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellFloatRenderer
_grid.GridCellFloatRenderer_swigregister(GridCellFloatRendererPtr)

class GridCellBoolRenderer(GridCellRenderer):
    """Proxy of C++ GridCellBoolRenderer class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellBoolRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GridCellBoolRenderer"""
        newobj = _grid.new_GridCellBoolRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class GridCellBoolRendererPtr(GridCellBoolRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellBoolRenderer
_grid.GridCellBoolRenderer_swigregister(GridCellBoolRendererPtr)

class GridCellDateTimeRenderer(GridCellStringRenderer):
    """Proxy of C++ GridCellDateTimeRenderer class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellDateTimeRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String outformat=DateTimeFormatStr, String informat=DateTimeFormatStr) -> GridCellDateTimeRenderer"""
        newobj = _grid.new_GridCellDateTimeRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class GridCellDateTimeRendererPtr(GridCellDateTimeRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellDateTimeRenderer
_grid.GridCellDateTimeRenderer_swigregister(GridCellDateTimeRendererPtr)

class GridCellEnumRenderer(GridCellStringRenderer):
    """Proxy of C++ GridCellEnumRenderer class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellEnumRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String choices=EmptyString) -> GridCellEnumRenderer"""
        newobj = _grid.new_GridCellEnumRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class GridCellEnumRendererPtr(GridCellEnumRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellEnumRenderer
_grid.GridCellEnumRenderer_swigregister(GridCellEnumRendererPtr)

class GridCellAutoWrapStringRenderer(GridCellStringRenderer):
    """Proxy of C++ GridCellAutoWrapStringRenderer class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellAutoWrapStringRenderer instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GridCellAutoWrapStringRenderer"""
        newobj = _grid.new_GridCellAutoWrapStringRenderer(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


class GridCellAutoWrapStringRendererPtr(GridCellAutoWrapStringRenderer):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellAutoWrapStringRenderer
_grid.GridCellAutoWrapStringRenderer_swigregister(GridCellAutoWrapStringRendererPtr)

class GridCellEditor(object):
    """Proxy of C++ GridCellEditor class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def _setOORInfo(*args, **kwargs):
        """_setOORInfo(self, PyObject _self)"""
        return _grid.GridCellEditor__setOORInfo(*args, **kwargs)

    def IsCreated(*args, **kwargs):
        """IsCreated(self) -> bool"""
        return _grid.GridCellEditor_IsCreated(*args, **kwargs)

    def GetControl(*args, **kwargs):
        """GetControl(self) -> Control"""
        return _grid.GridCellEditor_GetControl(*args, **kwargs)

    def SetControl(*args, **kwargs):
        """SetControl(self, Control control)"""
        return _grid.GridCellEditor_SetControl(*args, **kwargs)

    def GetCellAttr(*args, **kwargs):
        """GetCellAttr(self) -> GridCellAttr"""
        return _grid.GridCellEditor_GetCellAttr(*args, **kwargs)

    def SetCellAttr(*args, **kwargs):
        """SetCellAttr(self, GridCellAttr attr)"""
        return _grid.GridCellEditor_SetCellAttr(*args, **kwargs)

    def SetParameters(*args, **kwargs):
        """SetParameters(self, String params)"""
        return _grid.GridCellEditor_SetParameters(*args, **kwargs)

    def IncRef(*args, **kwargs):
        """IncRef(self)"""
        return _grid.GridCellEditor_IncRef(*args, **kwargs)

    def DecRef(*args, **kwargs):
        """DecRef(self)"""
        return _grid.GridCellEditor_DecRef(*args, **kwargs)

    def Create(*args, **kwargs):
        """Create(self, Window parent, int id, EvtHandler evtHandler)"""
        return _grid.GridCellEditor_Create(*args, **kwargs)

    def BeginEdit(*args, **kwargs):
        """BeginEdit(self, int row, int col, Grid grid)"""
        return _grid.GridCellEditor_BeginEdit(*args, **kwargs)

    def EndEdit(*args, **kwargs):
        """EndEdit(self, int row, int col, Grid grid) -> bool"""
        return _grid.GridCellEditor_EndEdit(*args, **kwargs)

    def Reset(*args, **kwargs):
        """Reset(self)"""
        return _grid.GridCellEditor_Reset(*args, **kwargs)

    def Clone(*args, **kwargs):
        """Clone(self) -> GridCellEditor"""
        return _grid.GridCellEditor_Clone(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, Rect rect)"""
        return _grid.GridCellEditor_SetSize(*args, **kwargs)

    def Show(*args, **kwargs):
        """Show(self, bool show, GridCellAttr attr=None)"""
        return _grid.GridCellEditor_Show(*args, **kwargs)

    def PaintBackground(*args, **kwargs):
        """PaintBackground(self, Rect rectCell, GridCellAttr attr)"""
        return _grid.GridCellEditor_PaintBackground(*args, **kwargs)

    def IsAcceptedKey(*args, **kwargs):
        """IsAcceptedKey(self, KeyEvent event) -> bool"""
        return _grid.GridCellEditor_IsAcceptedKey(*args, **kwargs)

    def StartingKey(*args, **kwargs):
        """StartingKey(self, KeyEvent event)"""
        return _grid.GridCellEditor_StartingKey(*args, **kwargs)

    def StartingClick(*args, **kwargs):
        """StartingClick(self)"""
        return _grid.GridCellEditor_StartingClick(*args, **kwargs)

    def HandleReturn(*args, **kwargs):
        """HandleReturn(self, KeyEvent event)"""
        return _grid.GridCellEditor_HandleReturn(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """Destroy(self)"""
        return _grid.GridCellEditor_Destroy(*args, **kwargs)


class GridCellEditorPtr(GridCellEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellEditor
_grid.GridCellEditor_swigregister(GridCellEditorPtr)

class PyGridCellEditor(GridCellEditor):
    """Proxy of C++ PyGridCellEditor class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyGridCellEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> PyGridCellEditor"""
        newobj = _grid.new_PyGridCellEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyGridCellEditor);self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _grid.PyGridCellEditor__setCallbackInfo(*args, **kwargs)

    def base_SetSize(*args, **kwargs):
        """base_SetSize(self, Rect rect)"""
        return _grid.PyGridCellEditor_base_SetSize(*args, **kwargs)

    def base_Show(*args, **kwargs):
        """base_Show(self, bool show, GridCellAttr attr=None)"""
        return _grid.PyGridCellEditor_base_Show(*args, **kwargs)

    def base_PaintBackground(*args, **kwargs):
        """base_PaintBackground(self, Rect rectCell, GridCellAttr attr)"""
        return _grid.PyGridCellEditor_base_PaintBackground(*args, **kwargs)

    def base_IsAcceptedKey(*args, **kwargs):
        """base_IsAcceptedKey(self, KeyEvent event) -> bool"""
        return _grid.PyGridCellEditor_base_IsAcceptedKey(*args, **kwargs)

    def base_StartingKey(*args, **kwargs):
        """base_StartingKey(self, KeyEvent event)"""
        return _grid.PyGridCellEditor_base_StartingKey(*args, **kwargs)

    def base_StartingClick(*args, **kwargs):
        """base_StartingClick(self)"""
        return _grid.PyGridCellEditor_base_StartingClick(*args, **kwargs)

    def base_HandleReturn(*args, **kwargs):
        """base_HandleReturn(self, KeyEvent event)"""
        return _grid.PyGridCellEditor_base_HandleReturn(*args, **kwargs)

    def base_Destroy(*args, **kwargs):
        """base_Destroy(self)"""
        return _grid.PyGridCellEditor_base_Destroy(*args, **kwargs)

    def base_SetParameters(*args, **kwargs):
        """base_SetParameters(self, String params)"""
        return _grid.PyGridCellEditor_base_SetParameters(*args, **kwargs)


class PyGridCellEditorPtr(PyGridCellEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyGridCellEditor
_grid.PyGridCellEditor_swigregister(PyGridCellEditorPtr)

class GridCellTextEditor(GridCellEditor):
    """Proxy of C++ GridCellTextEditor class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellTextEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GridCellTextEditor"""
        newobj = _grid.new_GridCellTextEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _grid.GridCellTextEditor_GetValue(*args, **kwargs)


class GridCellTextEditorPtr(GridCellTextEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellTextEditor
_grid.GridCellTextEditor_swigregister(GridCellTextEditorPtr)

class GridCellNumberEditor(GridCellTextEditor):
    """Proxy of C++ GridCellNumberEditor class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellNumberEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int min=-1, int max=-1) -> GridCellNumberEditor"""
        newobj = _grid.new_GridCellNumberEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _grid.GridCellNumberEditor_GetValue(*args, **kwargs)


class GridCellNumberEditorPtr(GridCellNumberEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellNumberEditor
_grid.GridCellNumberEditor_swigregister(GridCellNumberEditorPtr)

class GridCellFloatEditor(GridCellTextEditor):
    """Proxy of C++ GridCellFloatEditor class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellFloatEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int width=-1, int precision=-1) -> GridCellFloatEditor"""
        newobj = _grid.new_GridCellFloatEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _grid.GridCellFloatEditor_GetValue(*args, **kwargs)


class GridCellFloatEditorPtr(GridCellFloatEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellFloatEditor
_grid.GridCellFloatEditor_swigregister(GridCellFloatEditorPtr)

class GridCellBoolEditor(GridCellEditor):
    """Proxy of C++ GridCellBoolEditor class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellBoolEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GridCellBoolEditor"""
        newobj = _grid.new_GridCellBoolEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _grid.GridCellBoolEditor_GetValue(*args, **kwargs)


class GridCellBoolEditorPtr(GridCellBoolEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellBoolEditor
_grid.GridCellBoolEditor_swigregister(GridCellBoolEditorPtr)

class GridCellChoiceEditor(GridCellEditor):
    """Proxy of C++ GridCellChoiceEditor class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellChoiceEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int choices=0, bool allowOthers=False) -> GridCellChoiceEditor"""
        newobj = _grid.new_GridCellChoiceEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _grid.GridCellChoiceEditor_GetValue(*args, **kwargs)


class GridCellChoiceEditorPtr(GridCellChoiceEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellChoiceEditor
_grid.GridCellChoiceEditor_swigregister(GridCellChoiceEditorPtr)

class GridCellEnumEditor(GridCellChoiceEditor):
    """Proxy of C++ GridCellEnumEditor class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellEnumEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String choices=EmptyString) -> GridCellEnumEditor"""
        newobj = _grid.new_GridCellEnumEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _grid.GridCellEnumEditor_GetValue(*args, **kwargs)


class GridCellEnumEditorPtr(GridCellEnumEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellEnumEditor
_grid.GridCellEnumEditor_swigregister(GridCellEnumEditorPtr)

class GridCellAutoWrapStringEditor(GridCellTextEditor):
    """Proxy of C++ GridCellAutoWrapStringEditor class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellAutoWrapStringEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GridCellAutoWrapStringEditor"""
        newobj = _grid.new_GridCellAutoWrapStringEditor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def GetValue(*args, **kwargs):
        """GetValue(self) -> String"""
        return _grid.GridCellAutoWrapStringEditor_GetValue(*args, **kwargs)


class GridCellAutoWrapStringEditorPtr(GridCellAutoWrapStringEditor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellAutoWrapStringEditor
_grid.GridCellAutoWrapStringEditor_swigregister(GridCellAutoWrapStringEditorPtr)

class GridCellAttr(object):
    """Proxy of C++ GridCellAttr class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellAttr instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    Any = _grid.GridCellAttr_Any
    Default = _grid.GridCellAttr_Default
    Cell = _grid.GridCellAttr_Cell
    Row = _grid.GridCellAttr_Row
    Col = _grid.GridCellAttr_Col
    Merged = _grid.GridCellAttr_Merged
    def _setOORInfo(*args, **kwargs):
        """_setOORInfo(self, PyObject _self)"""
        return _grid.GridCellAttr__setOORInfo(*args, **kwargs)

    def __init__(self, *args, **kwargs):
        """__init__(self, GridCellAttr attrDefault=None) -> GridCellAttr"""
        newobj = _grid.new_GridCellAttr(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Clone(*args, **kwargs):
        """Clone(self) -> GridCellAttr"""
        return _grid.GridCellAttr_Clone(*args, **kwargs)

    def MergeWith(*args, **kwargs):
        """MergeWith(self, GridCellAttr mergefrom)"""
        return _grid.GridCellAttr_MergeWith(*args, **kwargs)

    def IncRef(*args, **kwargs):
        """IncRef(self)"""
        return _grid.GridCellAttr_IncRef(*args, **kwargs)

    def DecRef(*args, **kwargs):
        """DecRef(self)"""
        return _grid.GridCellAttr_DecRef(*args, **kwargs)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(self, Colour colText)"""
        return _grid.GridCellAttr_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(self, Colour colBack)"""
        return _grid.GridCellAttr_SetBackgroundColour(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(self, Font font)"""
        return _grid.GridCellAttr_SetFont(*args, **kwargs)

    def SetAlignment(*args, **kwargs):
        """SetAlignment(self, int hAlign, int vAlign)"""
        return _grid.GridCellAttr_SetAlignment(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, int num_rows, int num_cols)"""
        return _grid.GridCellAttr_SetSize(*args, **kwargs)

    def SetOverflow(*args, **kwargs):
        """SetOverflow(self, bool allow=True)"""
        return _grid.GridCellAttr_SetOverflow(*args, **kwargs)

    def SetReadOnly(*args, **kwargs):
        """SetReadOnly(self, bool isReadOnly=True)"""
        return _grid.GridCellAttr_SetReadOnly(*args, **kwargs)

    def SetRenderer(*args, **kwargs):
        """SetRenderer(self, GridCellRenderer renderer)"""
        return _grid.GridCellAttr_SetRenderer(*args, **kwargs)

    def SetEditor(*args, **kwargs):
        """SetEditor(self, GridCellEditor editor)"""
        return _grid.GridCellAttr_SetEditor(*args, **kwargs)

    def SetKind(*args, **kwargs):
        """SetKind(self, int kind)"""
        return _grid.GridCellAttr_SetKind(*args, **kwargs)

    def HasTextColour(*args, **kwargs):
        """HasTextColour(self) -> bool"""
        return _grid.GridCellAttr_HasTextColour(*args, **kwargs)

    def HasBackgroundColour(*args, **kwargs):
        """HasBackgroundColour(self) -> bool"""
        return _grid.GridCellAttr_HasBackgroundColour(*args, **kwargs)

    def HasFont(*args, **kwargs):
        """HasFont(self) -> bool"""
        return _grid.GridCellAttr_HasFont(*args, **kwargs)

    def HasAlignment(*args, **kwargs):
        """HasAlignment(self) -> bool"""
        return _grid.GridCellAttr_HasAlignment(*args, **kwargs)

    def HasRenderer(*args, **kwargs):
        """HasRenderer(self) -> bool"""
        return _grid.GridCellAttr_HasRenderer(*args, **kwargs)

    def HasEditor(*args, **kwargs):
        """HasEditor(self) -> bool"""
        return _grid.GridCellAttr_HasEditor(*args, **kwargs)

    def HasReadWriteMode(*args, **kwargs):
        """HasReadWriteMode(self) -> bool"""
        return _grid.GridCellAttr_HasReadWriteMode(*args, **kwargs)

    def HasOverflowMode(*args, **kwargs):
        """HasOverflowMode(self) -> bool"""
        return _grid.GridCellAttr_HasOverflowMode(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour(self) -> Colour"""
        return _grid.GridCellAttr_GetTextColour(*args, **kwargs)

    def GetBackgroundColour(*args, **kwargs):
        """GetBackgroundColour(self) -> Colour"""
        return _grid.GridCellAttr_GetBackgroundColour(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(self) -> Font"""
        return _grid.GridCellAttr_GetFont(*args, **kwargs)

    def GetAlignment(*args, **kwargs):
        """GetAlignment() -> (hAlign, vAlign)"""
        return _grid.GridCellAttr_GetAlignment(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize() -> (num_rows, num_cols)"""
        return _grid.GridCellAttr_GetSize(*args, **kwargs)

    def GetOverflow(*args, **kwargs):
        """GetOverflow(self) -> bool"""
        return _grid.GridCellAttr_GetOverflow(*args, **kwargs)

    def GetRenderer(*args, **kwargs):
        """GetRenderer(self, Grid grid, int row, int col) -> GridCellRenderer"""
        return _grid.GridCellAttr_GetRenderer(*args, **kwargs)

    def GetEditor(*args, **kwargs):
        """GetEditor(self, Grid grid, int row, int col) -> GridCellEditor"""
        return _grid.GridCellAttr_GetEditor(*args, **kwargs)

    def IsReadOnly(*args, **kwargs):
        """IsReadOnly(self) -> bool"""
        return _grid.GridCellAttr_IsReadOnly(*args, **kwargs)

    def GetKind(*args, **kwargs):
        """GetKind(self) -> int"""
        return _grid.GridCellAttr_GetKind(*args, **kwargs)

    def SetDefAttr(*args, **kwargs):
        """SetDefAttr(self, GridCellAttr defAttr)"""
        return _grid.GridCellAttr_SetDefAttr(*args, **kwargs)


class GridCellAttrPtr(GridCellAttr):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellAttr
_grid.GridCellAttr_swigregister(GridCellAttrPtr)

class GridCellAttrProvider(object):
    """Proxy of C++ GridCellAttrProvider class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellAttrProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GridCellAttrProvider"""
        newobj = _grid.new_GridCellAttrProvider(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def _setOORInfo(*args, **kwargs):
        """_setOORInfo(self, PyObject _self)"""
        return _grid.GridCellAttrProvider__setOORInfo(*args, **kwargs)

    def GetAttr(*args, **kwargs):
        """GetAttr(self, int row, int col, int kind) -> GridCellAttr"""
        return _grid.GridCellAttrProvider_GetAttr(*args, **kwargs)

    def SetAttr(*args, **kwargs):
        """SetAttr(self, GridCellAttr attr, int row, int col)"""
        return _grid.GridCellAttrProvider_SetAttr(*args, **kwargs)

    def SetRowAttr(*args, **kwargs):
        """SetRowAttr(self, GridCellAttr attr, int row)"""
        return _grid.GridCellAttrProvider_SetRowAttr(*args, **kwargs)

    def SetColAttr(*args, **kwargs):
        """SetColAttr(self, GridCellAttr attr, int col)"""
        return _grid.GridCellAttrProvider_SetColAttr(*args, **kwargs)

    def UpdateAttrRows(*args, **kwargs):
        """UpdateAttrRows(self, size_t pos, int numRows)"""
        return _grid.GridCellAttrProvider_UpdateAttrRows(*args, **kwargs)

    def UpdateAttrCols(*args, **kwargs):
        """UpdateAttrCols(self, size_t pos, int numCols)"""
        return _grid.GridCellAttrProvider_UpdateAttrCols(*args, **kwargs)


class GridCellAttrProviderPtr(GridCellAttrProvider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridCellAttrProvider
_grid.GridCellAttrProvider_swigregister(GridCellAttrProviderPtr)

class PyGridCellAttrProvider(GridCellAttrProvider):
    """Proxy of C++ PyGridCellAttrProvider class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyGridCellAttrProvider instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> PyGridCellAttrProvider"""
        newobj = _grid.new_PyGridCellAttrProvider(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyGridCellAttrProvider)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _grid.PyGridCellAttrProvider__setCallbackInfo(*args, **kwargs)

    def base_GetAttr(*args, **kwargs):
        """base_GetAttr(self, int row, int col, int kind) -> GridCellAttr"""
        return _grid.PyGridCellAttrProvider_base_GetAttr(*args, **kwargs)

    def base_SetAttr(*args, **kwargs):
        """base_SetAttr(self, GridCellAttr attr, int row, int col)"""
        return _grid.PyGridCellAttrProvider_base_SetAttr(*args, **kwargs)

    def base_SetRowAttr(*args, **kwargs):
        """base_SetRowAttr(self, GridCellAttr attr, int row)"""
        return _grid.PyGridCellAttrProvider_base_SetRowAttr(*args, **kwargs)

    def base_SetColAttr(*args, **kwargs):
        """base_SetColAttr(self, GridCellAttr attr, int col)"""
        return _grid.PyGridCellAttrProvider_base_SetColAttr(*args, **kwargs)


class PyGridCellAttrProviderPtr(PyGridCellAttrProvider):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyGridCellAttrProvider
_grid.PyGridCellAttrProvider_swigregister(PyGridCellAttrProviderPtr)

class GridTableBase(_core.Object):
    """Proxy of C++ GridTableBase class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridTableBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def _setOORInfo(*args, **kwargs):
        """_setOORInfo(self, PyObject _self)"""
        return _grid.GridTableBase__setOORInfo(*args, **kwargs)

    def SetAttrProvider(*args, **kwargs):
        """SetAttrProvider(self, GridCellAttrProvider attrProvider)"""
        return _grid.GridTableBase_SetAttrProvider(*args, **kwargs)

    def GetAttrProvider(*args, **kwargs):
        """GetAttrProvider(self) -> GridCellAttrProvider"""
        return _grid.GridTableBase_GetAttrProvider(*args, **kwargs)

    def SetView(*args, **kwargs):
        """SetView(self, Grid grid)"""
        return _grid.GridTableBase_SetView(*args, **kwargs)

    def GetView(*args, **kwargs):
        """GetView(self) -> Grid"""
        return _grid.GridTableBase_GetView(*args, **kwargs)

    def GetNumberRows(*args, **kwargs):
        """GetNumberRows(self) -> int"""
        return _grid.GridTableBase_GetNumberRows(*args, **kwargs)

    def GetNumberCols(*args, **kwargs):
        """GetNumberCols(self) -> int"""
        return _grid.GridTableBase_GetNumberCols(*args, **kwargs)

    def IsEmptyCell(*args, **kwargs):
        """IsEmptyCell(self, int row, int col) -> bool"""
        return _grid.GridTableBase_IsEmptyCell(*args, **kwargs)

    def GetValue(*args, **kwargs):
        """GetValue(self, int row, int col) -> String"""
        return _grid.GridTableBase_GetValue(*args, **kwargs)

    def SetValue(*args, **kwargs):
        """SetValue(self, int row, int col, String value)"""
        return _grid.GridTableBase_SetValue(*args, **kwargs)

    def GetTypeName(*args, **kwargs):
        """GetTypeName(self, int row, int col) -> String"""
        return _grid.GridTableBase_GetTypeName(*args, **kwargs)

    def CanGetValueAs(*args, **kwargs):
        """CanGetValueAs(self, int row, int col, String typeName) -> bool"""
        return _grid.GridTableBase_CanGetValueAs(*args, **kwargs)

    def CanSetValueAs(*args, **kwargs):
        """CanSetValueAs(self, int row, int col, String typeName) -> bool"""
        return _grid.GridTableBase_CanSetValueAs(*args, **kwargs)

    def GetValueAsLong(*args, **kwargs):
        """GetValueAsLong(self, int row, int col) -> long"""
        return _grid.GridTableBase_GetValueAsLong(*args, **kwargs)

    def GetValueAsDouble(*args, **kwargs):
        """GetValueAsDouble(self, int row, int col) -> double"""
        return _grid.GridTableBase_GetValueAsDouble(*args, **kwargs)

    def GetValueAsBool(*args, **kwargs):
        """GetValueAsBool(self, int row, int col) -> bool"""
        return _grid.GridTableBase_GetValueAsBool(*args, **kwargs)

    def SetValueAsLong(*args, **kwargs):
        """SetValueAsLong(self, int row, int col, long value)"""
        return _grid.GridTableBase_SetValueAsLong(*args, **kwargs)

    def SetValueAsDouble(*args, **kwargs):
        """SetValueAsDouble(self, int row, int col, double value)"""
        return _grid.GridTableBase_SetValueAsDouble(*args, **kwargs)

    def SetValueAsBool(*args, **kwargs):
        """SetValueAsBool(self, int row, int col, bool value)"""
        return _grid.GridTableBase_SetValueAsBool(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear(self)"""
        return _grid.GridTableBase_Clear(*args, **kwargs)

    def InsertRows(*args, **kwargs):
        """InsertRows(self, size_t pos=0, size_t numRows=1) -> bool"""
        return _grid.GridTableBase_InsertRows(*args, **kwargs)

    def AppendRows(*args, **kwargs):
        """AppendRows(self, size_t numRows=1) -> bool"""
        return _grid.GridTableBase_AppendRows(*args, **kwargs)

    def DeleteRows(*args, **kwargs):
        """DeleteRows(self, size_t pos=0, size_t numRows=1) -> bool"""
        return _grid.GridTableBase_DeleteRows(*args, **kwargs)

    def InsertCols(*args, **kwargs):
        """InsertCols(self, size_t pos=0, size_t numCols=1) -> bool"""
        return _grid.GridTableBase_InsertCols(*args, **kwargs)

    def AppendCols(*args, **kwargs):
        """AppendCols(self, size_t numCols=1) -> bool"""
        return _grid.GridTableBase_AppendCols(*args, **kwargs)

    def DeleteCols(*args, **kwargs):
        """DeleteCols(self, size_t pos=0, size_t numCols=1) -> bool"""
        return _grid.GridTableBase_DeleteCols(*args, **kwargs)

    def GetRowLabelValue(*args, **kwargs):
        """GetRowLabelValue(self, int row) -> String"""
        return _grid.GridTableBase_GetRowLabelValue(*args, **kwargs)

    def GetColLabelValue(*args, **kwargs):
        """GetColLabelValue(self, int col) -> String"""
        return _grid.GridTableBase_GetColLabelValue(*args, **kwargs)

    def SetRowLabelValue(*args, **kwargs):
        """SetRowLabelValue(self, int row, String value)"""
        return _grid.GridTableBase_SetRowLabelValue(*args, **kwargs)

    def SetColLabelValue(*args, **kwargs):
        """SetColLabelValue(self, int col, String value)"""
        return _grid.GridTableBase_SetColLabelValue(*args, **kwargs)

    def CanHaveAttributes(*args, **kwargs):
        """CanHaveAttributes(self) -> bool"""
        return _grid.GridTableBase_CanHaveAttributes(*args, **kwargs)

    def GetAttr(*args, **kwargs):
        """GetAttr(self, int row, int col, int kind) -> GridCellAttr"""
        return _grid.GridTableBase_GetAttr(*args, **kwargs)

    def SetAttr(*args, **kwargs):
        """SetAttr(self, GridCellAttr attr, int row, int col)"""
        return _grid.GridTableBase_SetAttr(*args, **kwargs)

    def SetRowAttr(*args, **kwargs):
        """SetRowAttr(self, GridCellAttr attr, int row)"""
        return _grid.GridTableBase_SetRowAttr(*args, **kwargs)

    def SetColAttr(*args, **kwargs):
        """SetColAttr(self, GridCellAttr attr, int col)"""
        return _grid.GridTableBase_SetColAttr(*args, **kwargs)


class GridTableBasePtr(GridTableBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridTableBase
_grid.GridTableBase_swigregister(GridTableBasePtr)

class PyGridTableBase(GridTableBase):
    """Proxy of C++ PyGridTableBase class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyGridTableBase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> PyGridTableBase"""
        newobj = _grid.new_PyGridTableBase(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, PyGridTableBase);self._setOORInfo(self)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _grid.PyGridTableBase__setCallbackInfo(*args, **kwargs)

    def Destroy(*args, **kwargs):
        """
        Destroy(self)

        Deletes the C++ object this Python object is a proxy for.
        """
        return _grid.PyGridTableBase_Destroy(*args, **kwargs)

    def base_GetTypeName(*args, **kwargs):
        """base_GetTypeName(self, int row, int col) -> String"""
        return _grid.PyGridTableBase_base_GetTypeName(*args, **kwargs)

    def base_CanGetValueAs(*args, **kwargs):
        """base_CanGetValueAs(self, int row, int col, String typeName) -> bool"""
        return _grid.PyGridTableBase_base_CanGetValueAs(*args, **kwargs)

    def base_CanSetValueAs(*args, **kwargs):
        """base_CanSetValueAs(self, int row, int col, String typeName) -> bool"""
        return _grid.PyGridTableBase_base_CanSetValueAs(*args, **kwargs)

    def base_Clear(*args, **kwargs):
        """base_Clear(self)"""
        return _grid.PyGridTableBase_base_Clear(*args, **kwargs)

    def base_InsertRows(*args, **kwargs):
        """base_InsertRows(self, size_t pos=0, size_t numRows=1) -> bool"""
        return _grid.PyGridTableBase_base_InsertRows(*args, **kwargs)

    def base_AppendRows(*args, **kwargs):
        """base_AppendRows(self, size_t numRows=1) -> bool"""
        return _grid.PyGridTableBase_base_AppendRows(*args, **kwargs)

    def base_DeleteRows(*args, **kwargs):
        """base_DeleteRows(self, size_t pos=0, size_t numRows=1) -> bool"""
        return _grid.PyGridTableBase_base_DeleteRows(*args, **kwargs)

    def base_InsertCols(*args, **kwargs):
        """base_InsertCols(self, size_t pos=0, size_t numCols=1) -> bool"""
        return _grid.PyGridTableBase_base_InsertCols(*args, **kwargs)

    def base_AppendCols(*args, **kwargs):
        """base_AppendCols(self, size_t numCols=1) -> bool"""
        return _grid.PyGridTableBase_base_AppendCols(*args, **kwargs)

    def base_DeleteCols(*args, **kwargs):
        """base_DeleteCols(self, size_t pos=0, size_t numCols=1) -> bool"""
        return _grid.PyGridTableBase_base_DeleteCols(*args, **kwargs)

    def base_GetRowLabelValue(*args, **kwargs):
        """base_GetRowLabelValue(self, int row) -> String"""
        return _grid.PyGridTableBase_base_GetRowLabelValue(*args, **kwargs)

    def base_GetColLabelValue(*args, **kwargs):
        """base_GetColLabelValue(self, int col) -> String"""
        return _grid.PyGridTableBase_base_GetColLabelValue(*args, **kwargs)

    def base_SetRowLabelValue(*args, **kwargs):
        """base_SetRowLabelValue(self, int row, String value)"""
        return _grid.PyGridTableBase_base_SetRowLabelValue(*args, **kwargs)

    def base_SetColLabelValue(*args, **kwargs):
        """base_SetColLabelValue(self, int col, String value)"""
        return _grid.PyGridTableBase_base_SetColLabelValue(*args, **kwargs)

    def base_CanHaveAttributes(*args, **kwargs):
        """base_CanHaveAttributes(self) -> bool"""
        return _grid.PyGridTableBase_base_CanHaveAttributes(*args, **kwargs)

    def base_GetAttr(*args, **kwargs):
        """base_GetAttr(self, int row, int col, int kind) -> GridCellAttr"""
        return _grid.PyGridTableBase_base_GetAttr(*args, **kwargs)

    def base_SetAttr(*args, **kwargs):
        """base_SetAttr(self, GridCellAttr attr, int row, int col)"""
        return _grid.PyGridTableBase_base_SetAttr(*args, **kwargs)

    def base_SetRowAttr(*args, **kwargs):
        """base_SetRowAttr(self, GridCellAttr attr, int row)"""
        return _grid.PyGridTableBase_base_SetRowAttr(*args, **kwargs)

    def base_SetColAttr(*args, **kwargs):
        """base_SetColAttr(self, GridCellAttr attr, int col)"""
        return _grid.PyGridTableBase_base_SetColAttr(*args, **kwargs)


class PyGridTableBasePtr(PyGridTableBase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyGridTableBase
_grid.PyGridTableBase_swigregister(PyGridTableBasePtr)

class GridStringTable(GridTableBase):
    """Proxy of C++ GridStringTable class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridStringTable instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int numRows=0, int numCols=0) -> GridStringTable"""
        newobj = _grid.new_GridStringTable(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)


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
    """Proxy of C++ GridTableMessage class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridTableMessage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, GridTableBase table, int id, int comInt1=-1, int comInt2=-1) -> GridTableMessage"""
        newobj = _grid.new_GridTableMessage(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_grid.delete_GridTableMessage):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetTableObject(*args, **kwargs):
        """SetTableObject(self, GridTableBase table)"""
        return _grid.GridTableMessage_SetTableObject(*args, **kwargs)

    def GetTableObject(*args, **kwargs):
        """GetTableObject(self) -> GridTableBase"""
        return _grid.GridTableMessage_GetTableObject(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(self, int id)"""
        return _grid.GridTableMessage_SetId(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId(self) -> int"""
        return _grid.GridTableMessage_GetId(*args, **kwargs)

    def SetCommandInt(*args, **kwargs):
        """SetCommandInt(self, int comInt1)"""
        return _grid.GridTableMessage_SetCommandInt(*args, **kwargs)

    def GetCommandInt(*args, **kwargs):
        """GetCommandInt(self) -> int"""
        return _grid.GridTableMessage_GetCommandInt(*args, **kwargs)

    def SetCommandInt2(*args, **kwargs):
        """SetCommandInt2(self, int comInt2)"""
        return _grid.GridTableMessage_SetCommandInt2(*args, **kwargs)

    def GetCommandInt2(*args, **kwargs):
        """GetCommandInt2(self) -> int"""
        return _grid.GridTableMessage_GetCommandInt2(*args, **kwargs)


class GridTableMessagePtr(GridTableMessage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridTableMessage
_grid.GridTableMessage_swigregister(GridTableMessagePtr)

class GridCellCoords(object):
    """Proxy of C++ GridCellCoords class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridCellCoords instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int r=-1, int c=-1) -> GridCellCoords"""
        newobj = _grid.new_GridCellCoords(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_grid.delete_GridCellCoords):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetRow(*args, **kwargs):
        """GetRow(self) -> int"""
        return _grid.GridCellCoords_GetRow(*args, **kwargs)

    def SetRow(*args, **kwargs):
        """SetRow(self, int n)"""
        return _grid.GridCellCoords_SetRow(*args, **kwargs)

    def GetCol(*args, **kwargs):
        """GetCol(self) -> int"""
        return _grid.GridCellCoords_GetCol(*args, **kwargs)

    def SetCol(*args, **kwargs):
        """SetCol(self, int n)"""
        return _grid.GridCellCoords_SetCol(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(self, int row, int col)"""
        return _grid.GridCellCoords_Set(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, GridCellCoords other) -> bool"""
        return _grid.GridCellCoords___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, GridCellCoords other) -> bool"""
        return _grid.GridCellCoords___ne__(*args, **kwargs)

    def Get(*args, **kwargs):
        """Get(self) -> PyObject"""
        return _grid.GridCellCoords_Get(*args, **kwargs)

    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wxGridCellCoords'+str(self.Get())
    def __len__(self):                   return len(self.Get())
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

class Grid(_windows.ScrolledWindow):
    """Proxy of C++ Grid class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGrid instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=WANTS_CHARS, 
            String name=PanelNameStr) -> Grid
        """
        newobj = _grid.new_Grid(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORInfo(self)

    def Create(*args, **kwargs):
        """
        Create(self, Window parent, int id=-1, Point pos=DefaultPosition, 
            Size size=DefaultSize, long style=WANTS_CHARS, 
            String name=PanelNameStr) -> bool
        """
        return _grid.Grid_Create(*args, **kwargs)

    wxGridSelectCells = _grid.Grid_wxGridSelectCells
    wxGridSelectRows = _grid.Grid_wxGridSelectRows
    wxGridSelectColumns = _grid.Grid_wxGridSelectColumns
    SelectCells =   wxGridSelectCells
    SelectRows =    wxGridSelectRows
    SelectColumns = wxGridSelectColumns

    def CreateGrid(*args, **kwargs):
        """CreateGrid(self, int numRows, int numCols, WXGRIDSELECTIONMODES selmode=wxGridSelectCells) -> bool"""
        return _grid.Grid_CreateGrid(*args, **kwargs)

    def SetSelectionMode(*args, **kwargs):
        """SetSelectionMode(self, WXGRIDSELECTIONMODES selmode)"""
        return _grid.Grid_SetSelectionMode(*args, **kwargs)

    def GetSelectionMode(*args, **kwargs):
        """GetSelectionMode(self) -> WXGRIDSELECTIONMODES"""
        return _grid.Grid_GetSelectionMode(*args, **kwargs)

    def GetNumberRows(*args, **kwargs):
        """GetNumberRows(self) -> int"""
        return _grid.Grid_GetNumberRows(*args, **kwargs)

    def GetNumberCols(*args, **kwargs):
        """GetNumberCols(self) -> int"""
        return _grid.Grid_GetNumberCols(*args, **kwargs)

    def ProcessTableMessage(*args, **kwargs):
        """ProcessTableMessage(self, GridTableMessage ??) -> bool"""
        return _grid.Grid_ProcessTableMessage(*args, **kwargs)

    def GetTable(*args, **kwargs):
        """GetTable(self) -> GridTableBase"""
        return _grid.Grid_GetTable(*args, **kwargs)

    def SetTable(*args, **kwargs):
        """SetTable(self, GridTableBase table, bool takeOwnership=False, WXGRIDSELECTIONMODES selmode=wxGridSelectCells) -> bool"""
        return _grid.Grid_SetTable(*args, **kwargs)

    def ClearGrid(*args, **kwargs):
        """ClearGrid(self)"""
        return _grid.Grid_ClearGrid(*args, **kwargs)

    def InsertRows(*args, **kwargs):
        """InsertRows(self, int pos=0, int numRows=1, bool updateLabels=True) -> bool"""
        return _grid.Grid_InsertRows(*args, **kwargs)

    def AppendRows(*args, **kwargs):
        """AppendRows(self, int numRows=1, bool updateLabels=True) -> bool"""
        return _grid.Grid_AppendRows(*args, **kwargs)

    def DeleteRows(*args, **kwargs):
        """DeleteRows(self, int pos=0, int numRows=1, bool updateLabels=True) -> bool"""
        return _grid.Grid_DeleteRows(*args, **kwargs)

    def InsertCols(*args, **kwargs):
        """InsertCols(self, int pos=0, int numCols=1, bool updateLabels=True) -> bool"""
        return _grid.Grid_InsertCols(*args, **kwargs)

    def AppendCols(*args, **kwargs):
        """AppendCols(self, int numCols=1, bool updateLabels=True) -> bool"""
        return _grid.Grid_AppendCols(*args, **kwargs)

    def DeleteCols(*args, **kwargs):
        """DeleteCols(self, int pos=0, int numCols=1, bool updateLabels=True) -> bool"""
        return _grid.Grid_DeleteCols(*args, **kwargs)

    def DrawCellHighlight(*args, **kwargs):
        """DrawCellHighlight(self, DC dc, GridCellAttr attr)"""
        return _grid.Grid_DrawCellHighlight(*args, **kwargs)

    def DrawTextRectangle(*args, **kwargs):
        """
        DrawTextRectangle(self, DC dc, String ??, Rect ??, int horizontalAlignment=LEFT, 
            int verticalAlignment=TOP, int textOrientation=HORIZONTAL)
        """
        return _grid.Grid_DrawTextRectangle(*args, **kwargs)

    def GetTextBoxSize(*args, **kwargs):
        """GetTextBoxSize(DC dc, list lines) -> (width, height)"""
        return _grid.Grid_GetTextBoxSize(*args, **kwargs)

    def BeginBatch(*args, **kwargs):
        """BeginBatch(self)"""
        return _grid.Grid_BeginBatch(*args, **kwargs)

    def EndBatch(*args, **kwargs):
        """EndBatch(self)"""
        return _grid.Grid_EndBatch(*args, **kwargs)

    def GetBatchCount(*args, **kwargs):
        """GetBatchCount(self) -> int"""
        return _grid.Grid_GetBatchCount(*args, **kwargs)

    def ForceRefresh(*args, **kwargs):
        """ForceRefresh(self)"""
        return _grid.Grid_ForceRefresh(*args, **kwargs)

    def IsEditable(*args, **kwargs):
        """IsEditable(self) -> bool"""
        return _grid.Grid_IsEditable(*args, **kwargs)

    def EnableEditing(*args, **kwargs):
        """EnableEditing(self, bool edit)"""
        return _grid.Grid_EnableEditing(*args, **kwargs)

    def EnableCellEditControl(*args, **kwargs):
        """EnableCellEditControl(self, bool enable=True)"""
        return _grid.Grid_EnableCellEditControl(*args, **kwargs)

    def DisableCellEditControl(*args, **kwargs):
        """DisableCellEditControl(self)"""
        return _grid.Grid_DisableCellEditControl(*args, **kwargs)

    def CanEnableCellControl(*args, **kwargs):
        """CanEnableCellControl(self) -> bool"""
        return _grid.Grid_CanEnableCellControl(*args, **kwargs)

    def IsCellEditControlEnabled(*args, **kwargs):
        """IsCellEditControlEnabled(self) -> bool"""
        return _grid.Grid_IsCellEditControlEnabled(*args, **kwargs)

    def IsCellEditControlShown(*args, **kwargs):
        """IsCellEditControlShown(self) -> bool"""
        return _grid.Grid_IsCellEditControlShown(*args, **kwargs)

    def IsCurrentCellReadOnly(*args, **kwargs):
        """IsCurrentCellReadOnly(self) -> bool"""
        return _grid.Grid_IsCurrentCellReadOnly(*args, **kwargs)

    def ShowCellEditControl(*args, **kwargs):
        """ShowCellEditControl(self)"""
        return _grid.Grid_ShowCellEditControl(*args, **kwargs)

    def HideCellEditControl(*args, **kwargs):
        """HideCellEditControl(self)"""
        return _grid.Grid_HideCellEditControl(*args, **kwargs)

    def SaveEditControlValue(*args, **kwargs):
        """SaveEditControlValue(self)"""
        return _grid.Grid_SaveEditControlValue(*args, **kwargs)

    def XYToCell(*args, **kwargs):
        """XYToCell(self, int x, int y) -> GridCellCoords"""
        return _grid.Grid_XYToCell(*args, **kwargs)

    def YToRow(*args, **kwargs):
        """YToRow(self, int y) -> int"""
        return _grid.Grid_YToRow(*args, **kwargs)

    def XToCol(*args, **kwargs):
        """XToCol(self, int x) -> int"""
        return _grid.Grid_XToCol(*args, **kwargs)

    def YToEdgeOfRow(*args, **kwargs):
        """YToEdgeOfRow(self, int y) -> int"""
        return _grid.Grid_YToEdgeOfRow(*args, **kwargs)

    def XToEdgeOfCol(*args, **kwargs):
        """XToEdgeOfCol(self, int x) -> int"""
        return _grid.Grid_XToEdgeOfCol(*args, **kwargs)

    def CellToRect(*args, **kwargs):
        """CellToRect(self, int row, int col) -> Rect"""
        return _grid.Grid_CellToRect(*args, **kwargs)

    def GetGridCursorRow(*args, **kwargs):
        """GetGridCursorRow(self) -> int"""
        return _grid.Grid_GetGridCursorRow(*args, **kwargs)

    def GetGridCursorCol(*args, **kwargs):
        """GetGridCursorCol(self) -> int"""
        return _grid.Grid_GetGridCursorCol(*args, **kwargs)

    def IsVisible(*args, **kwargs):
        """IsVisible(self, int row, int col, bool wholeCellVisible=True) -> bool"""
        return _grid.Grid_IsVisible(*args, **kwargs)

    def MakeCellVisible(*args, **kwargs):
        """MakeCellVisible(self, int row, int col)"""
        return _grid.Grid_MakeCellVisible(*args, **kwargs)

    def SetGridCursor(*args, **kwargs):
        """SetGridCursor(self, int row, int col)"""
        return _grid.Grid_SetGridCursor(*args, **kwargs)

    def MoveCursorUp(*args, **kwargs):
        """MoveCursorUp(self, bool expandSelection) -> bool"""
        return _grid.Grid_MoveCursorUp(*args, **kwargs)

    def MoveCursorDown(*args, **kwargs):
        """MoveCursorDown(self, bool expandSelection) -> bool"""
        return _grid.Grid_MoveCursorDown(*args, **kwargs)

    def MoveCursorLeft(*args, **kwargs):
        """MoveCursorLeft(self, bool expandSelection) -> bool"""
        return _grid.Grid_MoveCursorLeft(*args, **kwargs)

    def MoveCursorRight(*args, **kwargs):
        """MoveCursorRight(self, bool expandSelection) -> bool"""
        return _grid.Grid_MoveCursorRight(*args, **kwargs)

    def MovePageDown(*args, **kwargs):
        """MovePageDown(self) -> bool"""
        return _grid.Grid_MovePageDown(*args, **kwargs)

    def MovePageUp(*args, **kwargs):
        """MovePageUp(self) -> bool"""
        return _grid.Grid_MovePageUp(*args, **kwargs)

    def MoveCursorUpBlock(*args, **kwargs):
        """MoveCursorUpBlock(self, bool expandSelection) -> bool"""
        return _grid.Grid_MoveCursorUpBlock(*args, **kwargs)

    def MoveCursorDownBlock(*args, **kwargs):
        """MoveCursorDownBlock(self, bool expandSelection) -> bool"""
        return _grid.Grid_MoveCursorDownBlock(*args, **kwargs)

    def MoveCursorLeftBlock(*args, **kwargs):
        """MoveCursorLeftBlock(self, bool expandSelection) -> bool"""
        return _grid.Grid_MoveCursorLeftBlock(*args, **kwargs)

    def MoveCursorRightBlock(*args, **kwargs):
        """MoveCursorRightBlock(self, bool expandSelection) -> bool"""
        return _grid.Grid_MoveCursorRightBlock(*args, **kwargs)

    def GetDefaultRowLabelSize(*args, **kwargs):
        """GetDefaultRowLabelSize(self) -> int"""
        return _grid.Grid_GetDefaultRowLabelSize(*args, **kwargs)

    def GetRowLabelSize(*args, **kwargs):
        """GetRowLabelSize(self) -> int"""
        return _grid.Grid_GetRowLabelSize(*args, **kwargs)

    def GetDefaultColLabelSize(*args, **kwargs):
        """GetDefaultColLabelSize(self) -> int"""
        return _grid.Grid_GetDefaultColLabelSize(*args, **kwargs)

    def GetColLabelSize(*args, **kwargs):
        """GetColLabelSize(self) -> int"""
        return _grid.Grid_GetColLabelSize(*args, **kwargs)

    def GetLabelBackgroundColour(*args, **kwargs):
        """GetLabelBackgroundColour(self) -> Colour"""
        return _grid.Grid_GetLabelBackgroundColour(*args, **kwargs)

    def GetLabelTextColour(*args, **kwargs):
        """GetLabelTextColour(self) -> Colour"""
        return _grid.Grid_GetLabelTextColour(*args, **kwargs)

    def GetLabelFont(*args, **kwargs):
        """GetLabelFont(self) -> Font"""
        return _grid.Grid_GetLabelFont(*args, **kwargs)

    def GetRowLabelAlignment(*args, **kwargs):
        """GetRowLabelAlignment() -> (horiz, vert)"""
        return _grid.Grid_GetRowLabelAlignment(*args, **kwargs)

    def GetColLabelAlignment(*args, **kwargs):
        """GetColLabelAlignment() -> (horiz, vert)"""
        return _grid.Grid_GetColLabelAlignment(*args, **kwargs)

    def GetColLabelTextOrientation(*args, **kwargs):
        """GetColLabelTextOrientation(self) -> int"""
        return _grid.Grid_GetColLabelTextOrientation(*args, **kwargs)

    def GetRowLabelValue(*args, **kwargs):
        """GetRowLabelValue(self, int row) -> String"""
        return _grid.Grid_GetRowLabelValue(*args, **kwargs)

    def GetColLabelValue(*args, **kwargs):
        """GetColLabelValue(self, int col) -> String"""
        return _grid.Grid_GetColLabelValue(*args, **kwargs)

    def GetGridLineColour(*args, **kwargs):
        """GetGridLineColour(self) -> Colour"""
        return _grid.Grid_GetGridLineColour(*args, **kwargs)

    def GetCellHighlightColour(*args, **kwargs):
        """GetCellHighlightColour(self) -> Colour"""
        return _grid.Grid_GetCellHighlightColour(*args, **kwargs)

    def GetCellHighlightPenWidth(*args, **kwargs):
        """GetCellHighlightPenWidth(self) -> int"""
        return _grid.Grid_GetCellHighlightPenWidth(*args, **kwargs)

    def GetCellHighlightROPenWidth(*args, **kwargs):
        """GetCellHighlightROPenWidth(self) -> int"""
        return _grid.Grid_GetCellHighlightROPenWidth(*args, **kwargs)

    def SetRowLabelSize(*args, **kwargs):
        """SetRowLabelSize(self, int width)"""
        return _grid.Grid_SetRowLabelSize(*args, **kwargs)

    def SetColLabelSize(*args, **kwargs):
        """SetColLabelSize(self, int height)"""
        return _grid.Grid_SetColLabelSize(*args, **kwargs)

    def SetLabelBackgroundColour(*args, **kwargs):
        """SetLabelBackgroundColour(self, Colour ??)"""
        return _grid.Grid_SetLabelBackgroundColour(*args, **kwargs)

    def SetLabelTextColour(*args, **kwargs):
        """SetLabelTextColour(self, Colour ??)"""
        return _grid.Grid_SetLabelTextColour(*args, **kwargs)

    def SetLabelFont(*args, **kwargs):
        """SetLabelFont(self, Font ??)"""
        return _grid.Grid_SetLabelFont(*args, **kwargs)

    def SetRowLabelAlignment(*args, **kwargs):
        """SetRowLabelAlignment(self, int horiz, int vert)"""
        return _grid.Grid_SetRowLabelAlignment(*args, **kwargs)

    def SetColLabelAlignment(*args, **kwargs):
        """SetColLabelAlignment(self, int horiz, int vert)"""
        return _grid.Grid_SetColLabelAlignment(*args, **kwargs)

    def SetColLabelTextOrientation(*args, **kwargs):
        """SetColLabelTextOrientation(self, int textOrientation)"""
        return _grid.Grid_SetColLabelTextOrientation(*args, **kwargs)

    def SetRowLabelValue(*args, **kwargs):
        """SetRowLabelValue(self, int row, String ??)"""
        return _grid.Grid_SetRowLabelValue(*args, **kwargs)

    def SetColLabelValue(*args, **kwargs):
        """SetColLabelValue(self, int col, String ??)"""
        return _grid.Grid_SetColLabelValue(*args, **kwargs)

    def SetGridLineColour(*args, **kwargs):
        """SetGridLineColour(self, Colour ??)"""
        return _grid.Grid_SetGridLineColour(*args, **kwargs)

    def SetCellHighlightColour(*args, **kwargs):
        """SetCellHighlightColour(self, Colour ??)"""
        return _grid.Grid_SetCellHighlightColour(*args, **kwargs)

    def SetCellHighlightPenWidth(*args, **kwargs):
        """SetCellHighlightPenWidth(self, int width)"""
        return _grid.Grid_SetCellHighlightPenWidth(*args, **kwargs)

    def SetCellHighlightROPenWidth(*args, **kwargs):
        """SetCellHighlightROPenWidth(self, int width)"""
        return _grid.Grid_SetCellHighlightROPenWidth(*args, **kwargs)

    def EnableDragRowSize(*args, **kwargs):
        """EnableDragRowSize(self, bool enable=True)"""
        return _grid.Grid_EnableDragRowSize(*args, **kwargs)

    def DisableDragRowSize(*args, **kwargs):
        """DisableDragRowSize(self)"""
        return _grid.Grid_DisableDragRowSize(*args, **kwargs)

    def CanDragRowSize(*args, **kwargs):
        """CanDragRowSize(self) -> bool"""
        return _grid.Grid_CanDragRowSize(*args, **kwargs)

    def EnableDragColSize(*args, **kwargs):
        """EnableDragColSize(self, bool enable=True)"""
        return _grid.Grid_EnableDragColSize(*args, **kwargs)

    def DisableDragColSize(*args, **kwargs):
        """DisableDragColSize(self)"""
        return _grid.Grid_DisableDragColSize(*args, **kwargs)

    def CanDragColSize(*args, **kwargs):
        """CanDragColSize(self) -> bool"""
        return _grid.Grid_CanDragColSize(*args, **kwargs)

    def EnableDragGridSize(*args, **kwargs):
        """EnableDragGridSize(self, bool enable=True)"""
        return _grid.Grid_EnableDragGridSize(*args, **kwargs)

    def DisableDragGridSize(*args, **kwargs):
        """DisableDragGridSize(self)"""
        return _grid.Grid_DisableDragGridSize(*args, **kwargs)

    def CanDragGridSize(*args, **kwargs):
        """CanDragGridSize(self) -> bool"""
        return _grid.Grid_CanDragGridSize(*args, **kwargs)

    def EnableDragCell(*args, **kwargs):
        """EnableDragCell(self, bool enable=True)"""
        return _grid.Grid_EnableDragCell(*args, **kwargs)

    def DisableDragCell(*args, **kwargs):
        """DisableDragCell(self)"""
        return _grid.Grid_DisableDragCell(*args, **kwargs)

    def CanDragCell(*args, **kwargs):
        """CanDragCell(self) -> bool"""
        return _grid.Grid_CanDragCell(*args, **kwargs)

    def SetAttr(*args, **kwargs):
        """SetAttr(self, int row, int col, GridCellAttr attr)"""
        return _grid.Grid_SetAttr(*args, **kwargs)

    def SetRowAttr(*args, **kwargs):
        """SetRowAttr(self, int row, GridCellAttr attr)"""
        return _grid.Grid_SetRowAttr(*args, **kwargs)

    def SetColAttr(*args, **kwargs):
        """SetColAttr(self, int col, GridCellAttr attr)"""
        return _grid.Grid_SetColAttr(*args, **kwargs)

    def GetOrCreateCellAttr(*args, **kwargs):
        """GetOrCreateCellAttr(self, int row, int col) -> GridCellAttr"""
        return _grid.Grid_GetOrCreateCellAttr(*args, **kwargs)

    def SetColFormatBool(*args, **kwargs):
        """SetColFormatBool(self, int col)"""
        return _grid.Grid_SetColFormatBool(*args, **kwargs)

    def SetColFormatNumber(*args, **kwargs):
        """SetColFormatNumber(self, int col)"""
        return _grid.Grid_SetColFormatNumber(*args, **kwargs)

    def SetColFormatFloat(*args, **kwargs):
        """SetColFormatFloat(self, int col, int width=-1, int precision=-1)"""
        return _grid.Grid_SetColFormatFloat(*args, **kwargs)

    def SetColFormatCustom(*args, **kwargs):
        """SetColFormatCustom(self, int col, String typeName)"""
        return _grid.Grid_SetColFormatCustom(*args, **kwargs)

    def EnableGridLines(*args, **kwargs):
        """EnableGridLines(self, bool enable=True)"""
        return _grid.Grid_EnableGridLines(*args, **kwargs)

    def GridLinesEnabled(*args, **kwargs):
        """GridLinesEnabled(self) -> bool"""
        return _grid.Grid_GridLinesEnabled(*args, **kwargs)

    def GetDefaultRowSize(*args, **kwargs):
        """GetDefaultRowSize(self) -> int"""
        return _grid.Grid_GetDefaultRowSize(*args, **kwargs)

    def GetRowSize(*args, **kwargs):
        """GetRowSize(self, int row) -> int"""
        return _grid.Grid_GetRowSize(*args, **kwargs)

    def GetDefaultColSize(*args, **kwargs):
        """GetDefaultColSize(self) -> int"""
        return _grid.Grid_GetDefaultColSize(*args, **kwargs)

    def GetColSize(*args, **kwargs):
        """GetColSize(self, int col) -> int"""
        return _grid.Grid_GetColSize(*args, **kwargs)

    def GetDefaultCellBackgroundColour(*args, **kwargs):
        """GetDefaultCellBackgroundColour(self) -> Colour"""
        return _grid.Grid_GetDefaultCellBackgroundColour(*args, **kwargs)

    def GetCellBackgroundColour(*args, **kwargs):
        """GetCellBackgroundColour(self, int row, int col) -> Colour"""
        return _grid.Grid_GetCellBackgroundColour(*args, **kwargs)

    def GetDefaultCellTextColour(*args, **kwargs):
        """GetDefaultCellTextColour(self) -> Colour"""
        return _grid.Grid_GetDefaultCellTextColour(*args, **kwargs)

    def GetCellTextColour(*args, **kwargs):
        """GetCellTextColour(self, int row, int col) -> Colour"""
        return _grid.Grid_GetCellTextColour(*args, **kwargs)

    def GetDefaultCellFont(*args, **kwargs):
        """GetDefaultCellFont(self) -> Font"""
        return _grid.Grid_GetDefaultCellFont(*args, **kwargs)

    def GetCellFont(*args, **kwargs):
        """GetCellFont(self, int row, int col) -> Font"""
        return _grid.Grid_GetCellFont(*args, **kwargs)

    def GetDefaultCellAlignment(*args, **kwargs):
        """GetDefaultCellAlignment() -> (horiz, vert)"""
        return _grid.Grid_GetDefaultCellAlignment(*args, **kwargs)

    def GetCellAlignment(*args, **kwargs):
        """GetCellAlignment() -> (horiz, vert)"""
        return _grid.Grid_GetCellAlignment(*args, **kwargs)

    def GetDefaultCellOverflow(*args, **kwargs):
        """GetDefaultCellOverflow(self) -> bool"""
        return _grid.Grid_GetDefaultCellOverflow(*args, **kwargs)

    def GetCellOverflow(*args, **kwargs):
        """GetCellOverflow(self, int row, int col) -> bool"""
        return _grid.Grid_GetCellOverflow(*args, **kwargs)

    def GetCellSize(*args, **kwargs):
        """GetCellSize(int row, int col) -> (num_rows, num_cols)"""
        return _grid.Grid_GetCellSize(*args, **kwargs)

    def SetDefaultRowSize(*args, **kwargs):
        """SetDefaultRowSize(self, int height, bool resizeExistingRows=False)"""
        return _grid.Grid_SetDefaultRowSize(*args, **kwargs)

    def SetRowSize(*args, **kwargs):
        """SetRowSize(self, int row, int height)"""
        return _grid.Grid_SetRowSize(*args, **kwargs)

    def SetDefaultColSize(*args, **kwargs):
        """SetDefaultColSize(self, int width, bool resizeExistingCols=False)"""
        return _grid.Grid_SetDefaultColSize(*args, **kwargs)

    def SetColSize(*args, **kwargs):
        """SetColSize(self, int col, int width)"""
        return _grid.Grid_SetColSize(*args, **kwargs)

    def AutoSizeColumn(*args, **kwargs):
        """AutoSizeColumn(self, int col, bool setAsMin=True)"""
        return _grid.Grid_AutoSizeColumn(*args, **kwargs)

    def AutoSizeRow(*args, **kwargs):
        """AutoSizeRow(self, int row, bool setAsMin=True)"""
        return _grid.Grid_AutoSizeRow(*args, **kwargs)

    def AutoSizeColumns(*args, **kwargs):
        """AutoSizeColumns(self, bool setAsMin=True)"""
        return _grid.Grid_AutoSizeColumns(*args, **kwargs)

    def AutoSizeRows(*args, **kwargs):
        """AutoSizeRows(self, bool setAsMin=True)"""
        return _grid.Grid_AutoSizeRows(*args, **kwargs)

    def AutoSize(*args, **kwargs):
        """AutoSize(self)"""
        return _grid.Grid_AutoSize(*args, **kwargs)

    def AutoSizeRowLabelSize(*args, **kwargs):
        """AutoSizeRowLabelSize(self, int row)"""
        return _grid.Grid_AutoSizeRowLabelSize(*args, **kwargs)

    def AutoSizeColLabelSize(*args, **kwargs):
        """AutoSizeColLabelSize(self, int col)"""
        return _grid.Grid_AutoSizeColLabelSize(*args, **kwargs)

    def SetColMinimalWidth(*args, **kwargs):
        """SetColMinimalWidth(self, int col, int width)"""
        return _grid.Grid_SetColMinimalWidth(*args, **kwargs)

    def SetRowMinimalHeight(*args, **kwargs):
        """SetRowMinimalHeight(self, int row, int width)"""
        return _grid.Grid_SetRowMinimalHeight(*args, **kwargs)

    def SetColMinimalAcceptableWidth(*args, **kwargs):
        """SetColMinimalAcceptableWidth(self, int width)"""
        return _grid.Grid_SetColMinimalAcceptableWidth(*args, **kwargs)

    def SetRowMinimalAcceptableHeight(*args, **kwargs):
        """SetRowMinimalAcceptableHeight(self, int width)"""
        return _grid.Grid_SetRowMinimalAcceptableHeight(*args, **kwargs)

    def GetColMinimalAcceptableWidth(*args, **kwargs):
        """GetColMinimalAcceptableWidth(self) -> int"""
        return _grid.Grid_GetColMinimalAcceptableWidth(*args, **kwargs)

    def GetRowMinimalAcceptableHeight(*args, **kwargs):
        """GetRowMinimalAcceptableHeight(self) -> int"""
        return _grid.Grid_GetRowMinimalAcceptableHeight(*args, **kwargs)

    def SetDefaultCellBackgroundColour(*args, **kwargs):
        """SetDefaultCellBackgroundColour(self, Colour ??)"""
        return _grid.Grid_SetDefaultCellBackgroundColour(*args, **kwargs)

    def SetCellBackgroundColour(*args, **kwargs):
        """SetCellBackgroundColour(self, int row, int col, Colour ??)"""
        return _grid.Grid_SetCellBackgroundColour(*args, **kwargs)

    def SetDefaultCellTextColour(*args, **kwargs):
        """SetDefaultCellTextColour(self, Colour ??)"""
        return _grid.Grid_SetDefaultCellTextColour(*args, **kwargs)

    def SetCellTextColour(*args, **kwargs):
        """SetCellTextColour(self, int row, int col, Colour ??)"""
        return _grid.Grid_SetCellTextColour(*args, **kwargs)

    def SetDefaultCellFont(*args, **kwargs):
        """SetDefaultCellFont(self, Font ??)"""
        return _grid.Grid_SetDefaultCellFont(*args, **kwargs)

    def SetCellFont(*args, **kwargs):
        """SetCellFont(self, int row, int col, Font ??)"""
        return _grid.Grid_SetCellFont(*args, **kwargs)

    def SetDefaultCellAlignment(*args, **kwargs):
        """SetDefaultCellAlignment(self, int horiz, int vert)"""
        return _grid.Grid_SetDefaultCellAlignment(*args, **kwargs)

    def SetCellAlignment(*args, **kwargs):
        """SetCellAlignment(self, int row, int col, int horiz, int vert)"""
        return _grid.Grid_SetCellAlignment(*args, **kwargs)

    def SetDefaultCellOverflow(*args, **kwargs):
        """SetDefaultCellOverflow(self, bool allow)"""
        return _grid.Grid_SetDefaultCellOverflow(*args, **kwargs)

    def SetCellOverflow(*args, **kwargs):
        """SetCellOverflow(self, int row, int col, bool allow)"""
        return _grid.Grid_SetCellOverflow(*args, **kwargs)

    def SetCellSize(*args, **kwargs):
        """SetCellSize(self, int row, int col, int num_rows, int num_cols)"""
        return _grid.Grid_SetCellSize(*args, **kwargs)

    def SetDefaultRenderer(*args, **kwargs):
        """SetDefaultRenderer(self, GridCellRenderer renderer)"""
        return _grid.Grid_SetDefaultRenderer(*args, **kwargs)

    def SetCellRenderer(*args, **kwargs):
        """SetCellRenderer(self, int row, int col, GridCellRenderer renderer)"""
        return _grid.Grid_SetCellRenderer(*args, **kwargs)

    def GetDefaultRenderer(*args, **kwargs):
        """GetDefaultRenderer(self) -> GridCellRenderer"""
        return _grid.Grid_GetDefaultRenderer(*args, **kwargs)

    def GetCellRenderer(*args, **kwargs):
        """GetCellRenderer(self, int row, int col) -> GridCellRenderer"""
        return _grid.Grid_GetCellRenderer(*args, **kwargs)

    def SetDefaultEditor(*args, **kwargs):
        """SetDefaultEditor(self, GridCellEditor editor)"""
        return _grid.Grid_SetDefaultEditor(*args, **kwargs)

    def SetCellEditor(*args, **kwargs):
        """SetCellEditor(self, int row, int col, GridCellEditor editor)"""
        return _grid.Grid_SetCellEditor(*args, **kwargs)

    def GetDefaultEditor(*args, **kwargs):
        """GetDefaultEditor(self) -> GridCellEditor"""
        return _grid.Grid_GetDefaultEditor(*args, **kwargs)

    def GetCellEditor(*args, **kwargs):
        """GetCellEditor(self, int row, int col) -> GridCellEditor"""
        return _grid.Grid_GetCellEditor(*args, **kwargs)

    def GetCellValue(*args, **kwargs):
        """GetCellValue(self, int row, int col) -> String"""
        return _grid.Grid_GetCellValue(*args, **kwargs)

    def SetCellValue(*args, **kwargs):
        """SetCellValue(self, int row, int col, String s)"""
        return _grid.Grid_SetCellValue(*args, **kwargs)

    def IsReadOnly(*args, **kwargs):
        """IsReadOnly(self, int row, int col) -> bool"""
        return _grid.Grid_IsReadOnly(*args, **kwargs)

    def SetReadOnly(*args, **kwargs):
        """SetReadOnly(self, int row, int col, bool isReadOnly=True)"""
        return _grid.Grid_SetReadOnly(*args, **kwargs)

    def SelectRow(*args, **kwargs):
        """SelectRow(self, int row, bool addToSelected=False)"""
        return _grid.Grid_SelectRow(*args, **kwargs)

    def SelectCol(*args, **kwargs):
        """SelectCol(self, int col, bool addToSelected=False)"""
        return _grid.Grid_SelectCol(*args, **kwargs)

    def SelectBlock(*args, **kwargs):
        """
        SelectBlock(self, int topRow, int leftCol, int bottomRow, int rightCol, 
            bool addToSelected=False)
        """
        return _grid.Grid_SelectBlock(*args, **kwargs)

    def SelectAll(*args, **kwargs):
        """SelectAll(self)"""
        return _grid.Grid_SelectAll(*args, **kwargs)

    def IsSelection(*args, **kwargs):
        """IsSelection(self) -> bool"""
        return _grid.Grid_IsSelection(*args, **kwargs)

    def ClearSelection(*args, **kwargs):
        """ClearSelection(self)"""
        return _grid.Grid_ClearSelection(*args, **kwargs)

    def IsInSelection(*args, **kwargs):
        """IsInSelection(self, int row, int col) -> bool"""
        return _grid.Grid_IsInSelection(*args, **kwargs)

    def GetSelectedCells(*args, **kwargs):
        """GetSelectedCells(self) -> wxGridCellCoordsArray"""
        return _grid.Grid_GetSelectedCells(*args, **kwargs)

    def GetSelectionBlockTopLeft(*args, **kwargs):
        """GetSelectionBlockTopLeft(self) -> wxGridCellCoordsArray"""
        return _grid.Grid_GetSelectionBlockTopLeft(*args, **kwargs)

    def GetSelectionBlockBottomRight(*args, **kwargs):
        """GetSelectionBlockBottomRight(self) -> wxGridCellCoordsArray"""
        return _grid.Grid_GetSelectionBlockBottomRight(*args, **kwargs)

    def GetSelectedRows(*args, **kwargs):
        """GetSelectedRows(self) -> wxArrayInt"""
        return _grid.Grid_GetSelectedRows(*args, **kwargs)

    def GetSelectedCols(*args, **kwargs):
        """GetSelectedCols(self) -> wxArrayInt"""
        return _grid.Grid_GetSelectedCols(*args, **kwargs)

    def DeselectRow(*args, **kwargs):
        """DeselectRow(self, int row)"""
        return _grid.Grid_DeselectRow(*args, **kwargs)

    def DeselectCol(*args, **kwargs):
        """DeselectCol(self, int col)"""
        return _grid.Grid_DeselectCol(*args, **kwargs)

    def DeselectCell(*args, **kwargs):
        """DeselectCell(self, int row, int col)"""
        return _grid.Grid_DeselectCell(*args, **kwargs)

    def BlockToDeviceRect(*args, **kwargs):
        """BlockToDeviceRect(self, GridCellCoords topLeft, GridCellCoords bottomRight) -> Rect"""
        return _grid.Grid_BlockToDeviceRect(*args, **kwargs)

    def GetSelectionBackground(*args, **kwargs):
        """GetSelectionBackground(self) -> Colour"""
        return _grid.Grid_GetSelectionBackground(*args, **kwargs)

    def GetSelectionForeground(*args, **kwargs):
        """GetSelectionForeground(self) -> Colour"""
        return _grid.Grid_GetSelectionForeground(*args, **kwargs)

    def SetSelectionBackground(*args, **kwargs):
        """SetSelectionBackground(self, Colour c)"""
        return _grid.Grid_SetSelectionBackground(*args, **kwargs)

    def SetSelectionForeground(*args, **kwargs):
        """SetSelectionForeground(self, Colour c)"""
        return _grid.Grid_SetSelectionForeground(*args, **kwargs)

    def RegisterDataType(*args, **kwargs):
        """RegisterDataType(self, String typeName, GridCellRenderer renderer, GridCellEditor editor)"""
        return _grid.Grid_RegisterDataType(*args, **kwargs)

    def GetDefaultEditorForCell(*args, **kwargs):
        """GetDefaultEditorForCell(self, int row, int col) -> GridCellEditor"""
        return _grid.Grid_GetDefaultEditorForCell(*args, **kwargs)

    def GetDefaultRendererForCell(*args, **kwargs):
        """GetDefaultRendererForCell(self, int row, int col) -> GridCellRenderer"""
        return _grid.Grid_GetDefaultRendererForCell(*args, **kwargs)

    def GetDefaultEditorForType(*args, **kwargs):
        """GetDefaultEditorForType(self, String typeName) -> GridCellEditor"""
        return _grid.Grid_GetDefaultEditorForType(*args, **kwargs)

    def GetDefaultRendererForType(*args, **kwargs):
        """GetDefaultRendererForType(self, String typeName) -> GridCellRenderer"""
        return _grid.Grid_GetDefaultRendererForType(*args, **kwargs)

    def SetMargins(*args, **kwargs):
        """SetMargins(self, int extraWidth, int extraHeight)"""
        return _grid.Grid_SetMargins(*args, **kwargs)

    def GetGridWindow(*args, **kwargs):
        """GetGridWindow(self) -> Window"""
        return _grid.Grid_GetGridWindow(*args, **kwargs)

    def GetGridRowLabelWindow(*args, **kwargs):
        """GetGridRowLabelWindow(self) -> Window"""
        return _grid.Grid_GetGridRowLabelWindow(*args, **kwargs)

    def GetGridColLabelWindow(*args, **kwargs):
        """GetGridColLabelWindow(self) -> Window"""
        return _grid.Grid_GetGridColLabelWindow(*args, **kwargs)

    def GetGridCornerLabelWindow(*args, **kwargs):
        """GetGridCornerLabelWindow(self) -> Window"""
        return _grid.Grid_GetGridCornerLabelWindow(*args, **kwargs)

    def SetScrollLineX(*args, **kwargs):
        """SetScrollLineX(self, int x)"""
        return _grid.Grid_SetScrollLineX(*args, **kwargs)

    def SetScrollLineY(*args, **kwargs):
        """SetScrollLineY(self, int y)"""
        return _grid.Grid_SetScrollLineY(*args, **kwargs)

    def GetScrollLineX(*args, **kwargs):
        """GetScrollLineX(self) -> int"""
        return _grid.Grid_GetScrollLineX(*args, **kwargs)

    def GetScrollLineY(*args, **kwargs):
        """GetScrollLineY(self) -> int"""
        return _grid.Grid_GetScrollLineY(*args, **kwargs)

    def GetScrollX(*args, **kwargs):
        """GetScrollX(self, int x) -> int"""
        return _grid.Grid_GetScrollX(*args, **kwargs)

    def GetScrollY(*args, **kwargs):
        """GetScrollY(self, int y) -> int"""
        return _grid.Grid_GetScrollY(*args, **kwargs)

    def GetClassDefaultAttributes(*args, **kwargs):
        """
        GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

        Get the default attributes for this class.  This is useful if you want
        to use the same font or colour in your own control as in a standard
        control -- which is a much better idea than hard coding specific
        colours or fonts which might look completely out of place on the
        user's system, especially if it uses themes.

        The variant parameter is only relevant under Mac currently and is
        ignore under other platforms. Under Mac, it will change the size of
        the returned font. See `wx.Window.SetWindowVariant` for more about
        this.
        """
        return _grid.Grid_GetClassDefaultAttributes(*args, **kwargs)

    GetClassDefaultAttributes = staticmethod(GetClassDefaultAttributes)

class GridPtr(Grid):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Grid
_grid.Grid_swigregister(GridPtr)

def PreGrid(*args, **kwargs):
    """PreGrid() -> Grid"""
    val = _grid.new_PreGrid(*args, **kwargs)
    val.thisown = 1
    return val

def Grid_GetClassDefaultAttributes(*args, **kwargs):
    """
    Grid_GetClassDefaultAttributes(int variant=WINDOW_VARIANT_NORMAL) -> VisualAttributes

    Get the default attributes for this class.  This is useful if you want
    to use the same font or colour in your own control as in a standard
    control -- which is a much better idea than hard coding specific
    colours or fonts which might look completely out of place on the
    user's system, especially if it uses themes.

    The variant parameter is only relevant under Mac currently and is
    ignore under other platforms. Under Mac, it will change the size of
    the returned font. See `wx.Window.SetWindowVariant` for more about
    this.
    """
    return _grid.Grid_GetClassDefaultAttributes(*args, **kwargs)

class GridEvent(_core.NotifyEvent):
    """Proxy of C++ GridEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int id, wxEventType type, Grid obj, int row=-1, int col=-1, 
            int x=-1, int y=-1, bool sel=True, bool control=False, 
            bool shift=False, bool alt=False, 
            bool meta=False) -> GridEvent
        """
        newobj = _grid.new_GridEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRow(*args, **kwargs):
        """GetRow(self) -> int"""
        return _grid.GridEvent_GetRow(*args, **kwargs)

    def GetCol(*args, **kwargs):
        """GetCol(self) -> int"""
        return _grid.GridEvent_GetCol(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _grid.GridEvent_GetPosition(*args, **kwargs)

    def Selecting(*args, **kwargs):
        """Selecting(self) -> bool"""
        return _grid.GridEvent_Selecting(*args, **kwargs)

    def ControlDown(*args, **kwargs):
        """ControlDown(self) -> bool"""
        return _grid.GridEvent_ControlDown(*args, **kwargs)

    def MetaDown(*args, **kwargs):
        """MetaDown(self) -> bool"""
        return _grid.GridEvent_MetaDown(*args, **kwargs)

    def ShiftDown(*args, **kwargs):
        """ShiftDown(self) -> bool"""
        return _grid.GridEvent_ShiftDown(*args, **kwargs)

    def AltDown(*args, **kwargs):
        """AltDown(self) -> bool"""
        return _grid.GridEvent_AltDown(*args, **kwargs)


class GridEventPtr(GridEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridEvent
_grid.GridEvent_swigregister(GridEventPtr)

class GridSizeEvent(_core.NotifyEvent):
    """Proxy of C++ GridSizeEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridSizeEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int id, wxEventType type, Grid obj, int rowOrCol=-1, 
            int x=-1, int y=-1, bool control=False, bool shift=False, 
            bool alt=False, bool meta=False) -> GridSizeEvent
        """
        newobj = _grid.new_GridSizeEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRowOrCol(*args, **kwargs):
        """GetRowOrCol(self) -> int"""
        return _grid.GridSizeEvent_GetRowOrCol(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(self) -> Point"""
        return _grid.GridSizeEvent_GetPosition(*args, **kwargs)

    def ControlDown(*args, **kwargs):
        """ControlDown(self) -> bool"""
        return _grid.GridSizeEvent_ControlDown(*args, **kwargs)

    def MetaDown(*args, **kwargs):
        """MetaDown(self) -> bool"""
        return _grid.GridSizeEvent_MetaDown(*args, **kwargs)

    def ShiftDown(*args, **kwargs):
        """ShiftDown(self) -> bool"""
        return _grid.GridSizeEvent_ShiftDown(*args, **kwargs)

    def AltDown(*args, **kwargs):
        """AltDown(self) -> bool"""
        return _grid.GridSizeEvent_AltDown(*args, **kwargs)


class GridSizeEventPtr(GridSizeEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridSizeEvent
_grid.GridSizeEvent_swigregister(GridSizeEventPtr)

class GridRangeSelectEvent(_core.NotifyEvent):
    """Proxy of C++ GridRangeSelectEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridRangeSelectEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int id, wxEventType type, Grid obj, GridCellCoords topLeft, 
            GridCellCoords bottomRight, bool sel=True, 
            bool control=False, bool shift=False, 
            bool alt=False, bool meta=False) -> GridRangeSelectEvent
        """
        newobj = _grid.new_GridRangeSelectEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetTopLeftCoords(*args, **kwargs):
        """GetTopLeftCoords(self) -> GridCellCoords"""
        return _grid.GridRangeSelectEvent_GetTopLeftCoords(*args, **kwargs)

    def GetBottomRightCoords(*args, **kwargs):
        """GetBottomRightCoords(self) -> GridCellCoords"""
        return _grid.GridRangeSelectEvent_GetBottomRightCoords(*args, **kwargs)

    def GetTopRow(*args, **kwargs):
        """GetTopRow(self) -> int"""
        return _grid.GridRangeSelectEvent_GetTopRow(*args, **kwargs)

    def GetBottomRow(*args, **kwargs):
        """GetBottomRow(self) -> int"""
        return _grid.GridRangeSelectEvent_GetBottomRow(*args, **kwargs)

    def GetLeftCol(*args, **kwargs):
        """GetLeftCol(self) -> int"""
        return _grid.GridRangeSelectEvent_GetLeftCol(*args, **kwargs)

    def GetRightCol(*args, **kwargs):
        """GetRightCol(self) -> int"""
        return _grid.GridRangeSelectEvent_GetRightCol(*args, **kwargs)

    def Selecting(*args, **kwargs):
        """Selecting(self) -> bool"""
        return _grid.GridRangeSelectEvent_Selecting(*args, **kwargs)

    def ControlDown(*args, **kwargs):
        """ControlDown(self) -> bool"""
        return _grid.GridRangeSelectEvent_ControlDown(*args, **kwargs)

    def MetaDown(*args, **kwargs):
        """MetaDown(self) -> bool"""
        return _grid.GridRangeSelectEvent_MetaDown(*args, **kwargs)

    def ShiftDown(*args, **kwargs):
        """ShiftDown(self) -> bool"""
        return _grid.GridRangeSelectEvent_ShiftDown(*args, **kwargs)

    def AltDown(*args, **kwargs):
        """AltDown(self) -> bool"""
        return _grid.GridRangeSelectEvent_AltDown(*args, **kwargs)


class GridRangeSelectEventPtr(GridRangeSelectEvent):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GridRangeSelectEvent
_grid.GridRangeSelectEvent_swigregister(GridRangeSelectEventPtr)

class GridEditorCreatedEvent(_core.CommandEvent):
    """Proxy of C++ GridEditorCreatedEvent class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGridEditorCreatedEvent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int id, wxEventType type, Object obj, int row, int col, 
            Control ctrl) -> GridEditorCreatedEvent
        """
        newobj = _grid.new_GridEditorCreatedEvent(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetRow(*args, **kwargs):
        """GetRow(self) -> int"""
        return _grid.GridEditorCreatedEvent_GetRow(*args, **kwargs)

    def GetCol(*args, **kwargs):
        """GetCol(self) -> int"""
        return _grid.GridEditorCreatedEvent_GetCol(*args, **kwargs)

    def GetControl(*args, **kwargs):
        """GetControl(self) -> Control"""
        return _grid.GridEditorCreatedEvent_GetControl(*args, **kwargs)

    def SetRow(*args, **kwargs):
        """SetRow(self, int row)"""
        return _grid.GridEditorCreatedEvent_SetRow(*args, **kwargs)

    def SetCol(*args, **kwargs):
        """SetCol(self, int col)"""
        return _grid.GridEditorCreatedEvent_SetCol(*args, **kwargs)

    def SetControl(*args, **kwargs):
        """SetControl(self, Control ctrl)"""
        return _grid.GridEditorCreatedEvent_SetControl(*args, **kwargs)


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
wxEVT_GRID_CELL_BEGIN_DRAG = _grid.wxEVT_GRID_CELL_BEGIN_DRAG
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
EVT_GRID_CELL_BEGIN_DRAG = wx.PyEventBinder( wxEVT_GRID_CELL_BEGIN_DRAG )


# The same as above but with the ability to specify an identifier
EVT_GRID_CMD_CELL_LEFT_CLICK =     wx.PyEventBinder( wxEVT_GRID_CELL_LEFT_CLICK,    1 )
EVT_GRID_CMD_CELL_RIGHT_CLICK =    wx.PyEventBinder( wxEVT_GRID_CELL_RIGHT_CLICK,   1 )
EVT_GRID_CMD_CELL_LEFT_DCLICK =    wx.PyEventBinder( wxEVT_GRID_CELL_LEFT_DCLICK,   1 )
EVT_GRID_CMD_CELL_RIGHT_DCLICK =   wx.PyEventBinder( wxEVT_GRID_CELL_RIGHT_DCLICK,  1 )
EVT_GRID_CMD_LABEL_LEFT_CLICK =    wx.PyEventBinder( wxEVT_GRID_LABEL_LEFT_CLICK,   1 )
EVT_GRID_CMD_LABEL_RIGHT_CLICK =   wx.PyEventBinder( wxEVT_GRID_LABEL_RIGHT_CLICK,  1 )
EVT_GRID_CMD_LABEL_LEFT_DCLICK =   wx.PyEventBinder( wxEVT_GRID_LABEL_LEFT_DCLICK,  1 )
EVT_GRID_CMD_LABEL_RIGHT_DCLICK =  wx.PyEventBinder( wxEVT_GRID_LABEL_RIGHT_DCLICK, 1 )
EVT_GRID_CMD_ROW_SIZE =            wx.PyEventBinder( wxEVT_GRID_ROW_SIZE,           1 )
EVT_GRID_CMD_COL_SIZE =            wx.PyEventBinder( wxEVT_GRID_COL_SIZE,           1 )
EVT_GRID_CMD_RANGE_SELECT =        wx.PyEventBinder( wxEVT_GRID_RANGE_SELECT,       1 )
EVT_GRID_CMD_CELL_CHANGE =         wx.PyEventBinder( wxEVT_GRID_CELL_CHANGE,        1 )
EVT_GRID_CMD_SELECT_CELL =         wx.PyEventBinder( wxEVT_GRID_SELECT_CELL,        1 )
EVT_GRID_CMD_EDITOR_SHOWN =        wx.PyEventBinder( wxEVT_GRID_EDITOR_SHOWN,       1 )
EVT_GRID_CMD_EDITOR_HIDDEN =       wx.PyEventBinder( wxEVT_GRID_EDITOR_HIDDEN,      1 )
EVT_GRID_CMD_EDITOR_CREATED =      wx.PyEventBinder( wxEVT_GRID_EDITOR_CREATED,     1 )
EVT_GRID_CMD_CELL_BEGIN_DRAG =     wx.PyEventBinder( wxEVT_GRID_CELL_BEGIN_DRAG,    1 )
    


