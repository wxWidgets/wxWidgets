# This file was created automatically by SWIG.
import oglcanvasc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from clip_dnd import *

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

from sizers import *
import wx
class wxDiagramPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,oglcanvasc=oglcanvasc):
        if self.thisown == 1 :
            oglcanvasc.delete_wxDiagram(self)
    def AddShape(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_AddShape,(self,) + _args, _kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_Clear,(self,) + _args, _kwargs)
        return val
    def DeleteAllShapes(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_DeleteAllShapes,(self,) + _args, _kwargs)
        return val
    def DrawOutline(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_DrawOutline,(self,) + _args, _kwargs)
        return val
    def FindShape(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_FindShape,(self,) + _args, _kwargs)
        if val: val = wxPyShapePtr(val) 
        return val
    def GetCanvas(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_GetCanvas,(self,) + _args, _kwargs)
        if val: val = wxPyShapeCanvasPtr(val) 
        return val
    def GetCount(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_GetCount,(self,) + _args, _kwargs)
        return val
    def GetGridSpacing(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_GetGridSpacing,(self,) + _args, _kwargs)
        return val
    def GetMouseTolerance(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_GetMouseTolerance,(self,) + _args, _kwargs)
        return val
    def GetShapeList(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_GetShapeList,(self,) + _args, _kwargs)
        return val
    def GetQuickEditMode(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_GetQuickEditMode,(self,) + _args, _kwargs)
        return val
    def GetSnapToGrid(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_GetSnapToGrid,(self,) + _args, _kwargs)
        return val
    def InsertShape(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_InsertShape,(self,) + _args, _kwargs)
        return val
    def LoadFile(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_LoadFile,(self,) + _args, _kwargs)
        return val
    def ReadContainerGeometry(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_ReadContainerGeometry,(self,) + _args, _kwargs)
        return val
    def ReadLines(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_ReadLines,(self,) + _args, _kwargs)
        return val
    def ReadNodes(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_ReadNodes,(self,) + _args, _kwargs)
        return val
    def RecentreAll(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_RecentreAll,(self,) + _args, _kwargs)
        return val
    def Redraw(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_Redraw,(self,) + _args, _kwargs)
        return val
    def RemoveAllShapes(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_RemoveAllShapes,(self,) + _args, _kwargs)
        return val
    def RemoveShape(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_RemoveShape,(self,) + _args, _kwargs)
        return val
    def SaveFile(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_SaveFile,(self,) + _args, _kwargs)
        return val
    def SetCanvas(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_SetCanvas,(self,) + _args, _kwargs)
        return val
    def SetGridSpacing(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_SetGridSpacing,(self,) + _args, _kwargs)
        return val
    def SetMouseTolerance(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_SetMouseTolerance,(self,) + _args, _kwargs)
        return val
    def SetQuickEditMode(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_SetQuickEditMode,(self,) + _args, _kwargs)
        return val
    def SetSnapToGrid(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_SetSnapToGrid,(self,) + _args, _kwargs)
        return val
    def ShowAll(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_ShowAll,(self,) + _args, _kwargs)
        return val
    def Snap(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxDiagram_Snap,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDiagram instance at %s>" % (self.this,)
class wxDiagram(wxDiagramPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(oglcanvasc.new_wxDiagram,_args,_kwargs)
        self.thisown = 1




class wxPyShapeCanvasPtr(wxScrolledWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas__setSelf,(self,) + _args, _kwargs)
        return val
    def AddShape(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_AddShape,(self,) + _args, _kwargs)
        return val
    def FindShape(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_FindShape,(self,) + _args, _kwargs)
        return val
    def FindFirstSensitiveShape(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_FindFirstSensitiveShape,(self,) + _args, _kwargs)
        return val
    def GetDiagram(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_GetDiagram,(self,) + _args, _kwargs)
        if val: val = wxDiagramPtr(val) 
        return val
    def GetQuickEditMode(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_GetQuickEditMode,(self,) + _args, _kwargs)
        return val
    def InsertShape(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_InsertShape,(self,) + _args, _kwargs)
        return val
    def base_OnBeginDragLeft(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_base_OnBeginDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnBeginDragRight(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_base_OnBeginDragRight,(self,) + _args, _kwargs)
        return val
    def base_OnEndDragLeft(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_base_OnEndDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnEndDragRight(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_base_OnEndDragRight,(self,) + _args, _kwargs)
        return val
    def base_OnDragLeft(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_base_OnDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnDragRight(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_base_OnDragRight,(self,) + _args, _kwargs)
        return val
    def base_OnLeftClick(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_base_OnLeftClick,(self,) + _args, _kwargs)
        return val
    def base_OnRightClick(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_base_OnRightClick,(self,) + _args, _kwargs)
        return val
    def Redraw(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_Redraw,(self,) + _args, _kwargs)
        return val
    def RemoveShape(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_RemoveShape,(self,) + _args, _kwargs)
        return val
    def SetDiagram(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_SetDiagram,(self,) + _args, _kwargs)
        return val
    def Snap(self, *_args, **_kwargs):
        val = apply(oglcanvasc.wxPyShapeCanvas_Snap,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyShapeCanvas instance at %s>" % (self.this,)
class wxPyShapeCanvas(wxPyShapeCanvasPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(oglcanvasc.new_wxPyShapeCanvas,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self, wxPyShapeCanvas)
        #wx._StdWindowCallbacks(self)
        #wx._StdOnScrollCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

