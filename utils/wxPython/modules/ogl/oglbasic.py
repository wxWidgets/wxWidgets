# This file was created automatically by SWIG.
import oglbasicc

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

from sizers import *
import wx
from oglcanvas import wxPyShapeCanvasPtr
class wxShapeRegionPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetText(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetText,(self,) + _args, _kwargs)
        return val
    def SetFont(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetFont,(self,) + _args, _kwargs)
        return val
    def SetMinSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetMinSize,(self,) + _args, _kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetSize,(self,) + _args, _kwargs)
        return val
    def SetPosition(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetPosition,(self,) + _args, _kwargs)
        return val
    def SetProportions(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetProportions,(self,) + _args, _kwargs)
        return val
    def SetFormatMode(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetFormatMode,(self,) + _args, _kwargs)
        return val
    def SetName(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetName,(self,) + _args, _kwargs)
        return val
    def SetColour(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetColour,(self,) + _args, _kwargs)
        return val
    def GetText(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetText,(self,) + _args, _kwargs)
        return val
    def GetFont(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def GetMinSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetMinSize,(self,) + _args, _kwargs)
        return val
    def GetProportion(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetProportion,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetSize,(self,) + _args, _kwargs)
        return val
    def GetPosition(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetPosition,(self,) + _args, _kwargs)
        return val
    def GetFormatMode(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetFormatMode,(self,) + _args, _kwargs)
        return val
    def GetName(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetName,(self,) + _args, _kwargs)
        return val
    def GetColour(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetColour,(self,) + _args, _kwargs)
        return val
    def GetActualColourObject(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetActualColourObject,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetFormattedText(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetFormattedText,(self,) + _args, _kwargs)
        return val
    def GetPenColour(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetPenColour,(self,) + _args, _kwargs)
        return val
    def GetPenStyle(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetPenStyle,(self,) + _args, _kwargs)
        return val
    def SetPenStyle(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetPenStyle,(self,) + _args, _kwargs)
        return val
    def SetPenColour(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_SetPenColour,(self,) + _args, _kwargs)
        return val
    def GetActualPen(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetActualPen,(self,) + _args, _kwargs)
        if val: val = wxPenPtr(val) 
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetWidth,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_GetHeight,(self,) + _args, _kwargs)
        return val
    def ClearText(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxShapeRegion_ClearText,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxShapeRegion instance at %s>" % (self.this,)
class wxShapeRegion(wxShapeRegionPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(oglbasicc.new_wxShapeRegion,_args,_kwargs)
        self.thisown = 1




class wxPyShapeEvtHandlerPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler__setSelf,(self,) + _args, _kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_Destroy,(self,) + _args, _kwargs)
        return val
    def SetShape(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_SetShape,(self,) + _args, _kwargs)
        return val
    def GetShape(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_GetShape,(self,) + _args, _kwargs)
        if val: val = wxPyShapePtr(val) 
        return val
    def SetPreviousHandler(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_SetPreviousHandler,(self,) + _args, _kwargs)
        return val
    def GetPreviousHandler(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_GetPreviousHandler,(self,) + _args, _kwargs)
        if val: val = wxPyShapeEvtHandlerPtr(val) 
        return val
    def CreateNewCopy(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_CreateNewCopy,(self,) + _args, _kwargs)
        if val: val = wxPyShapeEvtHandlerPtr(val) 
        return val
    def base_OnDelete(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnDelete,(self,) + _args, _kwargs)
        return val
    def base_OnDraw(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnDraw,(self,) + _args, _kwargs)
        return val
    def base_OnDrawContents(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnDrawContents,(self,) + _args, _kwargs)
        return val
    def base_OnDrawBranches(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnDrawBranches,(self,) + _args, _kwargs)
        return val
    def base_OnMoveLinks(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnMoveLinks,(self,) + _args, _kwargs)
        return val
    def base_OnErase(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnErase,(self,) + _args, _kwargs)
        return val
    def base_OnEraseContents(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnEraseContents,(self,) + _args, _kwargs)
        return val
    def base_OnHighlight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnHighlight,(self,) + _args, _kwargs)
        return val
    def base_OnLeftClick(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnLeftClick,(self,) + _args, _kwargs)
        return val
    def base_OnLeftDoubleClick(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnLeftDoubleClick,(self,) + _args, _kwargs)
        return val
    def base_OnRightClick(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnRightClick,(self,) + _args, _kwargs)
        return val
    def base_OnSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnSize,(self,) + _args, _kwargs)
        return val
    def base_OnMovePre(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnMovePre,(self,) + _args, _kwargs)
        return val
    def base_OnMovePost(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnMovePost,(self,) + _args, _kwargs)
        return val
    def base_OnDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnBeginDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnBeginDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnEndDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnEndDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnDragRight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnDragRight,(self,) + _args, _kwargs)
        return val
    def base_OnBeginDragRight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnBeginDragRight,(self,) + _args, _kwargs)
        return val
    def base_OnEndDragRight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnEndDragRight,(self,) + _args, _kwargs)
        return val
    def base_OnDrawOutline(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnDrawOutline,(self,) + _args, _kwargs)
        return val
    def base_OnDrawControlPoints(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnDrawControlPoints,(self,) + _args, _kwargs)
        return val
    def base_OnEraseControlPoints(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnEraseControlPoints,(self,) + _args, _kwargs)
        return val
    def base_OnMoveLink(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnMoveLink,(self,) + _args, _kwargs)
        return val
    def base_OnSizingDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnSizingDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnSizingBeginDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnSizingBeginDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnSizingEndDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnSizingEndDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnBeginSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnBeginSize,(self,) + _args, _kwargs)
        return val
    def base_OnEndSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShapeEvtHandler_base_OnEndSize,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyShapeEvtHandler instance at %s>" % (self.this,)
class wxPyShapeEvtHandler(wxPyShapeEvtHandlerPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(oglbasicc.new_wxPyShapeEvtHandler,_args,_kwargs)
        self.thisown = 1
        self._setSelf(self)




class wxPyShapePtr(wxPyShapeEvtHandlerPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def _setSelf(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape__setSelf,(self,) + _args, _kwargs)
        return val
    def Destroy(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Destroy,(self,) + _args, _kwargs)
        return val
    def GetBoundingBoxMax(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetBoundingBoxMax,(self,) + _args, _kwargs)
        return val
    def GetBoundingBoxMin(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetBoundingBoxMin,(self,) + _args, _kwargs)
        return val
    def GetPerimeterPoint(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetPerimeterPoint,(self,) + _args, _kwargs)
        return val
    def GetCanvas(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetCanvas,(self,) + _args, _kwargs)
        if val: val = wxPyShapeCanvasPtr(val) 
        return val
    def SetCanvas(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetCanvas,(self,) + _args, _kwargs)
        return val
    def AddToCanvas(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_AddToCanvas,(self,) + _args, _kwargs)
        return val
    def InsertInCanvas(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_InsertInCanvas,(self,) + _args, _kwargs)
        return val
    def RemoveFromCanvas(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_RemoveFromCanvas,(self,) + _args, _kwargs)
        return val
    def GetX(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetX,(self,) + _args, _kwargs)
        return val
    def GetY(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetY,(self,) + _args, _kwargs)
        return val
    def SetX(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetX,(self,) + _args, _kwargs)
        return val
    def SetY(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetY,(self,) + _args, _kwargs)
        return val
    def GetParent(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetParent,(self,) + _args, _kwargs)
        if val: val = wxPyShapePtr(val) 
        return val
    def SetParent(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetParent,(self,) + _args, _kwargs)
        return val
    def GetTopAncestor(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetTopAncestor,(self,) + _args, _kwargs)
        if val: val = wxPyShapePtr(val) 
        return val
    def GetChildren(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetChildren,(self,) + _args, _kwargs)
        return val
    def Unlink(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Unlink,(self,) + _args, _kwargs)
        return val
    def SetDrawHandles(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetDrawHandles,(self,) + _args, _kwargs)
        return val
    def GetDrawHandles(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetDrawHandles,(self,) + _args, _kwargs)
        return val
    def MakeControlPoints(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_MakeControlPoints,(self,) + _args, _kwargs)
        return val
    def DeleteControlPoints(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_DeleteControlPoints,(self,) + _args, _kwargs)
        return val
    def ResetControlPoints(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_ResetControlPoints,(self,) + _args, _kwargs)
        return val
    def GetEventHandler(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetEventHandler,(self,) + _args, _kwargs)
        if val: val = wxPyShapeEvtHandlerPtr(val) 
        return val
    def SetEventHandler(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetEventHandler,(self,) + _args, _kwargs)
        return val
    def MakeMandatoryControlPoints(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_MakeMandatoryControlPoints,(self,) + _args, _kwargs)
        return val
    def ResetMandatoryControlPoints(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_ResetMandatoryControlPoints,(self,) + _args, _kwargs)
        return val
    def Recompute(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Recompute,(self,) + _args, _kwargs)
        return val
    def CalculateSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_CalculateSize,(self,) + _args, _kwargs)
        return val
    def Select(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Select,(self,) + _args, _kwargs)
        return val
    def SetHighlight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetHighlight,(self,) + _args, _kwargs)
        return val
    def IsHighlighted(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_IsHighlighted,(self,) + _args, _kwargs)
        return val
    def Selected(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Selected,(self,) + _args, _kwargs)
        return val
    def AncestorSelected(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_AncestorSelected,(self,) + _args, _kwargs)
        return val
    def SetSensitivityFilter(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetSensitivityFilter,(self,) + _args, _kwargs)
        return val
    def GetSensitivityFilter(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetSensitivityFilter,(self,) + _args, _kwargs)
        return val
    def SetDraggable(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetDraggable,(self,) + _args, _kwargs)
        return val
    def SetFixedSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetFixedSize,(self,) + _args, _kwargs)
        return val
    def GetFixedSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetFixedSize,(self,) + _args, _kwargs)
        return val
    def GetFixedWidth(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetFixedWidth,(self,) + _args, _kwargs)
        return val
    def GetFixedHeight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetFixedHeight,(self,) + _args, _kwargs)
        return val
    def SetSpaceAttachments(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetSpaceAttachments,(self,) + _args, _kwargs)
        return val
    def GetSpaceAttachments(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetSpaceAttachments,(self,) + _args, _kwargs)
        return val
    def SetShadowMode(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetShadowMode,(self,) + _args, _kwargs)
        return val
    def GetShadowMode(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetShadowMode,(self,) + _args, _kwargs)
        return val
    def HitTest(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_HitTest,(self,) + _args, _kwargs)
        return val
    def SetCentreResize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetCentreResize,(self,) + _args, _kwargs)
        return val
    def GetCentreResize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetCentreResize,(self,) + _args, _kwargs)
        return val
    def SetMaintainAspectRatio(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetMaintainAspectRatio,(self,) + _args, _kwargs)
        return val
    def GetMaintainAspectRatio(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetMaintainAspectRatio,(self,) + _args, _kwargs)
        return val
    def GetLines(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetLines,(self,) + _args, _kwargs)
        return val
    def SetDisableLabel(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetDisableLabel,(self,) + _args, _kwargs)
        return val
    def GetDisableLabel(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetDisableLabel,(self,) + _args, _kwargs)
        return val
    def SetAttachmentMode(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetAttachmentMode,(self,) + _args, _kwargs)
        return val
    def GetAttachmentMode(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetAttachmentMode,(self,) + _args, _kwargs)
        return val
    def SetId(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetId,(self,) + _args, _kwargs)
        return val
    def GetId(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetId,(self,) + _args, _kwargs)
        return val
    def SetPen(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetPen,(self,) + _args, _kwargs)
        return val
    def SetBrush(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetBrush,(self,) + _args, _kwargs)
        return val
    def SetClientData(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetClientData,(self,) + _args, _kwargs)
        return val
    def GetClientData(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetClientData,(self,) + _args, _kwargs)
        return val
    def Show(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Show,(self,) + _args, _kwargs)
        return val
    def IsShown(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_IsShown,(self,) + _args, _kwargs)
        return val
    def Move(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Move,(self,) + _args, _kwargs)
        return val
    def Erase(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Erase,(self,) + _args, _kwargs)
        return val
    def EraseContents(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_EraseContents,(self,) + _args, _kwargs)
        return val
    def Draw(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Draw,(self,) + _args, _kwargs)
        return val
    def Flash(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Flash,(self,) + _args, _kwargs)
        return val
    def MoveLinks(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_MoveLinks,(self,) + _args, _kwargs)
        return val
    def DrawContents(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_DrawContents,(self,) + _args, _kwargs)
        return val
    def SetSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetSize,(self,) + _args, _kwargs)
        return val
    def SetAttachmentSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetAttachmentSize,(self,) + _args, _kwargs)
        return val
    def Attach(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Attach,(self,) + _args, _kwargs)
        return val
    def Detach(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Detach,(self,) + _args, _kwargs)
        return val
    def Constrain(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Constrain,(self,) + _args, _kwargs)
        return val
    def AddLine(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_AddLine,(self,) + _args, _kwargs)
        return val
    def GetLinePosition(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetLinePosition,(self,) + _args, _kwargs)
        return val
    def AddText(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_AddText,(self,) + _args, _kwargs)
        return val
    def GetPen(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetPen,(self,) + _args, _kwargs)
        if val: val = wxPenPtr(val) 
        return val
    def GetBrush(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetBrush,(self,) + _args, _kwargs)
        if val: val = wxBrushPtr(val) 
        return val
    def SetDefaultRegionSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetDefaultRegionSize,(self,) + _args, _kwargs)
        return val
    def FormatText(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_FormatText,(self,) + _args, _kwargs)
        return val
    def SetFormatMode(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetFormatMode,(self,) + _args, _kwargs)
        return val
    def GetFormatMode(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetFormatMode,(self,) + _args, _kwargs)
        return val
    def SetFont(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetFont,(self,) + _args, _kwargs)
        return val
    def GetFont(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def SetTextColour(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetTextColour,(self,) + _args, _kwargs)
        return val
    def GetTextColour(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetTextColour,(self,) + _args, _kwargs)
        return val
    def GetNumberOfTextRegions(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetNumberOfTextRegions,(self,) + _args, _kwargs)
        return val
    def SetRegionName(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetRegionName,(self,) + _args, _kwargs)
        return val
    def GetRegionName(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetRegionName,(self,) + _args, _kwargs)
        return val
    def GetRegionId(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetRegionId,(self,) + _args, _kwargs)
        return val
    def NameRegions(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_NameRegions,(self,) + _args, _kwargs)
        return val
    def GetRegions(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetRegions,(self,) + _args, _kwargs)
        return val
    def AddRegion(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_AddRegion,(self,) + _args, _kwargs)
        return val
    def ClearRegions(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_ClearRegions,(self,) + _args, _kwargs)
        return val
    def AssignNewIds(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_AssignNewIds,(self,) + _args, _kwargs)
        return val
    def FindRegion(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_FindRegion,(self,) + _args, _kwargs)
        if val: val = wxPyShapePtr(val) 
        return val
    def FindRegionNames(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_FindRegionNames,(self,) + _args, _kwargs)
        return val
    def ClearText(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_ClearText,(self,) + _args, _kwargs)
        return val
    def RemoveLine(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_RemoveLine,(self,) + _args, _kwargs)
        return val
    def GetAttachmentPosition(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetAttachmentPosition,(self,) + _args, _kwargs)
        return val
    def GetNumberOfAttachments(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetNumberOfAttachments,(self,) + _args, _kwargs)
        return val
    def AttachmentIsValid(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_AttachmentIsValid,(self,) + _args, _kwargs)
        return val
    def GetAttachmentPositionEdge(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetAttachmentPositionEdge,(self,) + _args, _kwargs)
        return val
    def CalcSimpleAttachment(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_CalcSimpleAttachment,(self,) + _args, _kwargs)
        if val: val = wxRealPointPtr(val) ; val.thisown = 1
        return val
    def AttachmentSortTest(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_AttachmentSortTest,(self,) + _args, _kwargs)
        return val
    def EraseLinks(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_EraseLinks,(self,) + _args, _kwargs)
        return val
    def DrawLinks(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_DrawLinks,(self,) + _args, _kwargs)
        return val
    def MoveLineToNewAttachment(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_MoveLineToNewAttachment,(self,) + _args, _kwargs)
        return val
    def ApplyAttachmentOrdering(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_ApplyAttachmentOrdering,(self,) + _args, _kwargs)
        return val
    def GetBranchingAttachmentRoot(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetBranchingAttachmentRoot,(self,) + _args, _kwargs)
        if val: val = wxRealPointPtr(val) ; val.thisown = 1
        return val
    def GetBranchingAttachmentInfo(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetBranchingAttachmentInfo,(self,) + _args, _kwargs)
        return val
    def GetBranchingAttachmentPoint(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetBranchingAttachmentPoint,(self,) + _args, _kwargs)
        return val
    def GetAttachmentLineCount(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetAttachmentLineCount,(self,) + _args, _kwargs)
        return val
    def SetBranchNeckLength(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetBranchNeckLength,(self,) + _args, _kwargs)
        return val
    def GetBranchNeckLength(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetBranchNeckLength,(self,) + _args, _kwargs)
        return val
    def SetBranchStemLength(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetBranchStemLength,(self,) + _args, _kwargs)
        return val
    def GetBranchStemLength(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetBranchStemLength,(self,) + _args, _kwargs)
        return val
    def SetBranchSpacing(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetBranchSpacing,(self,) + _args, _kwargs)
        return val
    def GetBranchSpacing(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetBranchSpacing,(self,) + _args, _kwargs)
        return val
    def SetBranchStyle(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_SetBranchStyle,(self,) + _args, _kwargs)
        return val
    def GetBranchStyle(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetBranchStyle,(self,) + _args, _kwargs)
        return val
    def PhysicalToLogicalAttachment(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_PhysicalToLogicalAttachment,(self,) + _args, _kwargs)
        return val
    def LogicalToPhysicalAttachment(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_LogicalToPhysicalAttachment,(self,) + _args, _kwargs)
        return val
    def Draggable(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Draggable,(self,) + _args, _kwargs)
        return val
    def HasDescendant(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_HasDescendant,(self,) + _args, _kwargs)
        return val
    def CreateNewCopy(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_CreateNewCopy,(self,) + _args, _kwargs)
        if val: val = wxPyShapePtr(val) 
        return val
    def Copy(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Copy,(self,) + _args, _kwargs)
        return val
    def CopyWithHandler(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_CopyWithHandler,(self,) + _args, _kwargs)
        return val
    def Rotate(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Rotate,(self,) + _args, _kwargs)
        return val
    def GetRotation(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_GetRotation,(self,) + _args, _kwargs)
        return val
    def ClearAttachments(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_ClearAttachments,(self,) + _args, _kwargs)
        return val
    def Recentre(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_Recentre,(self,) + _args, _kwargs)
        return val
    def ClearPointList(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_ClearPointList,(self,) + _args, _kwargs)
        return val
    def base_OnDelete(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnDelete,(self,) + _args, _kwargs)
        return val
    def base_OnDraw(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnDraw,(self,) + _args, _kwargs)
        return val
    def base_OnDrawContents(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnDrawContents,(self,) + _args, _kwargs)
        return val
    def base_OnDrawBranches(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnDrawBranches,(self,) + _args, _kwargs)
        return val
    def base_OnMoveLinks(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnMoveLinks,(self,) + _args, _kwargs)
        return val
    def base_OnErase(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnErase,(self,) + _args, _kwargs)
        return val
    def base_OnEraseContents(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnEraseContents,(self,) + _args, _kwargs)
        return val
    def base_OnHighlight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnHighlight,(self,) + _args, _kwargs)
        return val
    def base_OnLeftClick(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnLeftClick,(self,) + _args, _kwargs)
        return val
    def base_OnLeftDoubleClick(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnLeftDoubleClick,(self,) + _args, _kwargs)
        return val
    def base_OnRightClick(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnRightClick,(self,) + _args, _kwargs)
        return val
    def base_OnSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnSize,(self,) + _args, _kwargs)
        return val
    def base_OnMovePre(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnMovePre,(self,) + _args, _kwargs)
        return val
    def base_OnMovePost(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnMovePost,(self,) + _args, _kwargs)
        return val
    def base_OnDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnBeginDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnBeginDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnEndDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnEndDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnDragRight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnDragRight,(self,) + _args, _kwargs)
        return val
    def base_OnBeginDragRight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnBeginDragRight,(self,) + _args, _kwargs)
        return val
    def base_OnEndDragRight(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnEndDragRight,(self,) + _args, _kwargs)
        return val
    def base_OnDrawOutline(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnDrawOutline,(self,) + _args, _kwargs)
        return val
    def base_OnDrawControlPoints(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnDrawControlPoints,(self,) + _args, _kwargs)
        return val
    def base_OnEraseControlPoints(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnEraseControlPoints,(self,) + _args, _kwargs)
        return val
    def base_OnMoveLink(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnMoveLink,(self,) + _args, _kwargs)
        return val
    def base_OnSizingDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnSizingDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnSizingBeginDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnSizingBeginDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnSizingEndDragLeft(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnSizingEndDragLeft,(self,) + _args, _kwargs)
        return val
    def base_OnBeginSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnBeginSize,(self,) + _args, _kwargs)
        return val
    def base_OnEndSize(self, *_args, **_kwargs):
        val = apply(oglbasicc.wxPyShape_base_OnEndSize,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPyShape instance at %s>" % (self.this,)
class wxPyShape(wxPyShapePtr):
    def __init__(self,this):
        self.this = this






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

