# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _ogl

import windows
import core
wx = core 
#---------------------------------------------------------------------------

class ShapeRegion(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxShapeRegion instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> ShapeRegion"""
        newobj = _ogl.new_ShapeRegion(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SetText(*args, **kwargs):
        """SetText(wxString s)"""
        return _ogl.ShapeRegion_SetText(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(wxFont f)"""
        return _ogl.ShapeRegion_SetFont(*args, **kwargs)

    def SetMinSize(*args, **kwargs):
        """SetMinSize(double w, double h)"""
        return _ogl.ShapeRegion_SetMinSize(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(double w, double h)"""
        return _ogl.ShapeRegion_SetSize(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(double x, double y)"""
        return _ogl.ShapeRegion_SetPosition(*args, **kwargs)

    def SetProportions(*args, **kwargs):
        """SetProportions(double x, double y)"""
        return _ogl.ShapeRegion_SetProportions(*args, **kwargs)

    def SetFormatMode(*args, **kwargs):
        """SetFormatMode(int mode)"""
        return _ogl.ShapeRegion_SetFormatMode(*args, **kwargs)

    def SetName(*args, **kwargs):
        """SetName(wxString s)"""
        return _ogl.ShapeRegion_SetName(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """SetColour(wxString col)"""
        return _ogl.ShapeRegion_SetColour(*args, **kwargs)

    def GetText(*args, **kwargs):
        """GetText() -> wxString"""
        return _ogl.ShapeRegion_GetText(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont() -> wxFont"""
        return _ogl.ShapeRegion_GetFont(*args, **kwargs)

    def GetMinSize(*args, **kwargs):
        """GetMinSize(double OUTPUT, double OUTPUT)"""
        return _ogl.ShapeRegion_GetMinSize(*args, **kwargs)

    def GetProportion(*args, **kwargs):
        """GetProportion(double OUTPUT, double OUTPUT)"""
        return _ogl.ShapeRegion_GetProportion(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(double OUTPUT, double OUTPUT)"""
        return _ogl.ShapeRegion_GetSize(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition(double OUTPUT, double OUTPUT)"""
        return _ogl.ShapeRegion_GetPosition(*args, **kwargs)

    def GetFormatMode(*args, **kwargs):
        """GetFormatMode() -> int"""
        return _ogl.ShapeRegion_GetFormatMode(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName() -> wxString"""
        return _ogl.ShapeRegion_GetName(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour() -> wxString"""
        return _ogl.ShapeRegion_GetColour(*args, **kwargs)

    def GetActualColourObject(*args, **kwargs):
        """GetActualColourObject() -> wxColour"""
        return _ogl.ShapeRegion_GetActualColourObject(*args, **kwargs)

    def GetFormattedText(*args, **kwargs):
        """GetFormattedText() -> wxList"""
        return _ogl.ShapeRegion_GetFormattedText(*args, **kwargs)

    def GetPenColour(*args, **kwargs):
        """GetPenColour() -> wxString"""
        return _ogl.ShapeRegion_GetPenColour(*args, **kwargs)

    def GetPenStyle(*args, **kwargs):
        """GetPenStyle() -> int"""
        return _ogl.ShapeRegion_GetPenStyle(*args, **kwargs)

    def SetPenStyle(*args, **kwargs):
        """SetPenStyle(int style)"""
        return _ogl.ShapeRegion_SetPenStyle(*args, **kwargs)

    def SetPenColour(*args, **kwargs):
        """SetPenColour(wxString col)"""
        return _ogl.ShapeRegion_SetPenColour(*args, **kwargs)

    def GetActualPen(*args, **kwargs):
        """GetActualPen() -> wxPen"""
        return _ogl.ShapeRegion_GetActualPen(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth() -> double"""
        return _ogl.ShapeRegion_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight() -> double"""
        return _ogl.ShapeRegion_GetHeight(*args, **kwargs)

    def ClearText(*args, **kwargs):
        """ClearText()"""
        return _ogl.ShapeRegion_ClearText(*args, **kwargs)


class ShapeRegionPtr(ShapeRegion):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ShapeRegion
_ogl.ShapeRegion_swigregister(ShapeRegionPtr)

class PyShapeEvtHandler(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyShapeEvtHandler instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(PyShapeEvtHandler prev=None, PyShape shape=None) -> PyShapeEvtHandler"""
        newobj = _ogl.new_PyShapeEvtHandler(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyShapeEvtHandler)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyShapeEvtHandler__setCallbackInfo(*args, **kwargs)

    def _setOORInfo(*args, **kwargs):
        """_setOORInfo(PyObject _self)"""
        return _ogl.PyShapeEvtHandler__setOORInfo(*args, **kwargs)

    def _setOORandCallbackInfo(self, _class):
        self._setOORInfo(self)
        self._setCallbackInfo(self, _class)

    def SetShape(*args, **kwargs):
        """SetShape(PyShape sh)"""
        return _ogl.PyShapeEvtHandler_SetShape(*args, **kwargs)

    def GetShape(*args, **kwargs):
        """GetShape() -> PyShape"""
        return _ogl.PyShapeEvtHandler_GetShape(*args, **kwargs)

    def SetPreviousHandler(*args, **kwargs):
        """SetPreviousHandler(PyShapeEvtHandler handler)"""
        return _ogl.PyShapeEvtHandler_SetPreviousHandler(*args, **kwargs)

    def GetPreviousHandler(*args, **kwargs):
        """GetPreviousHandler() -> PyShapeEvtHandler"""
        return _ogl.PyShapeEvtHandler_GetPreviousHandler(*args, **kwargs)

    def CreateNewCopy(*args, **kwargs):
        """CreateNewCopy() -> PyShapeEvtHandler"""
        return _ogl.PyShapeEvtHandler_CreateNewCopy(*args, **kwargs)

    def base_OnDelete(*args, **kwargs):
        """base_OnDelete()"""
        return _ogl.PyShapeEvtHandler_base_OnDelete(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyShapeEvtHandler_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyShapeEvtHandler_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=False)"""
        return _ogl.PyShapeEvtHandler_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyShapeEvtHandler_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyShapeEvtHandler_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyShapeEvtHandler_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyShapeEvtHandler_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyShapeEvtHandler_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyShapeEvtHandler_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyShapeEvtHandler_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyShapeEvtHandler_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyShapeEvtHandler_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyShapeEvtHandler_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyShapeEvtHandler_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyShapeEvtHandler_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyShapeEvtHandler_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyShapeEvtHandler_base_OnEndSize(*args, **kwargs)


class PyShapeEvtHandlerPtr(PyShapeEvtHandler):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyShapeEvtHandler
_ogl.PyShapeEvtHandler_swigregister(PyShapeEvtHandlerPtr)

class PyShape(PyShapeEvtHandler):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(PyShapeCanvas can=None) -> PyShape"""
        newobj = _ogl.new_PyShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyShapeEvtHandler)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyShape__setCallbackInfo(*args, **kwargs)

    def GetBoundingBoxMax(*args, **kwargs):
        """GetBoundingBoxMax(double OUTPUT, double OUTPUT)"""
        return _ogl.PyShape_GetBoundingBoxMax(*args, **kwargs)

    def GetBoundingBoxMin(*args, **kwargs):
        """GetBoundingBoxMin(double OUTPUT, double OUTPUT)"""
        return _ogl.PyShape_GetBoundingBoxMin(*args, **kwargs)

    def GetPerimeterPoint(*args, **kwargs):
        """GetPerimeterPoint(double x1, double y1, double x2, double y2, double OUTPUT, 
    double OUTPUT) -> bool"""
        return _ogl.PyShape_GetPerimeterPoint(*args, **kwargs)

    def GetCanvas(*args, **kwargs):
        """GetCanvas() -> PyShapeCanvas"""
        return _ogl.PyShape_GetCanvas(*args, **kwargs)

    def SetCanvas(*args, **kwargs):
        """SetCanvas(PyShapeCanvas the_canvas)"""
        return _ogl.PyShape_SetCanvas(*args, **kwargs)

    def AddToCanvas(*args, **kwargs):
        """AddToCanvas(PyShapeCanvas the_canvas, PyShape addAfter=None)"""
        return _ogl.PyShape_AddToCanvas(*args, **kwargs)

    def InsertInCanvas(*args, **kwargs):
        """InsertInCanvas(PyShapeCanvas the_canvas)"""
        return _ogl.PyShape_InsertInCanvas(*args, **kwargs)

    def RemoveFromCanvas(*args, **kwargs):
        """RemoveFromCanvas(PyShapeCanvas the_canvas)"""
        return _ogl.PyShape_RemoveFromCanvas(*args, **kwargs)

    def GetX(*args, **kwargs):
        """GetX() -> double"""
        return _ogl.PyShape_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY() -> double"""
        return _ogl.PyShape_GetY(*args, **kwargs)

    def SetX(*args, **kwargs):
        """SetX(double x)"""
        return _ogl.PyShape_SetX(*args, **kwargs)

    def SetY(*args, **kwargs):
        """SetY(double y)"""
        return _ogl.PyShape_SetY(*args, **kwargs)

    def GetParent(*args, **kwargs):
        """GetParent() -> PyShape"""
        return _ogl.PyShape_GetParent(*args, **kwargs)

    def SetParent(*args, **kwargs):
        """SetParent(PyShape p)"""
        return _ogl.PyShape_SetParent(*args, **kwargs)

    def GetTopAncestor(*args, **kwargs):
        """GetTopAncestor() -> PyShape"""
        return _ogl.PyShape_GetTopAncestor(*args, **kwargs)

    def GetChildren(*args, **kwargs):
        """GetChildren() -> PyObject"""
        return _ogl.PyShape_GetChildren(*args, **kwargs)

    def Unlink(*args, **kwargs):
        """Unlink()"""
        return _ogl.PyShape_Unlink(*args, **kwargs)

    def SetDrawHandles(*args, **kwargs):
        """SetDrawHandles(bool drawH)"""
        return _ogl.PyShape_SetDrawHandles(*args, **kwargs)

    def GetDrawHandles(*args, **kwargs):
        """GetDrawHandles() -> bool"""
        return _ogl.PyShape_GetDrawHandles(*args, **kwargs)

    def MakeControlPoints(*args, **kwargs):
        """MakeControlPoints()"""
        return _ogl.PyShape_MakeControlPoints(*args, **kwargs)

    def DeleteControlPoints(*args, **kwargs):
        """DeleteControlPoints(wxDC dc=None)"""
        return _ogl.PyShape_DeleteControlPoints(*args, **kwargs)

    def ResetControlPoints(*args, **kwargs):
        """ResetControlPoints()"""
        return _ogl.PyShape_ResetControlPoints(*args, **kwargs)

    def GetEventHandler(*args, **kwargs):
        """GetEventHandler() -> PyShapeEvtHandler"""
        return _ogl.PyShape_GetEventHandler(*args, **kwargs)

    def SetEventHandler(*args, **kwargs):
        """SetEventHandler(PyShapeEvtHandler handler)"""
        return _ogl.PyShape_SetEventHandler(*args, **kwargs)

    def MakeMandatoryControlPoints(*args, **kwargs):
        """MakeMandatoryControlPoints()"""
        return _ogl.PyShape_MakeMandatoryControlPoints(*args, **kwargs)

    def ResetMandatoryControlPoints(*args, **kwargs):
        """ResetMandatoryControlPoints()"""
        return _ogl.PyShape_ResetMandatoryControlPoints(*args, **kwargs)

    def Recompute(*args, **kwargs):
        """Recompute() -> bool"""
        return _ogl.PyShape_Recompute(*args, **kwargs)

    def CalculateSize(*args, **kwargs):
        """CalculateSize()"""
        return _ogl.PyShape_CalculateSize(*args, **kwargs)

    def Select(*args, **kwargs):
        """Select(bool select=True, wxDC dc=None)"""
        return _ogl.PyShape_Select(*args, **kwargs)

    def SetHighlight(*args, **kwargs):
        """SetHighlight(bool hi=True, bool recurse=False)"""
        return _ogl.PyShape_SetHighlight(*args, **kwargs)

    def IsHighlighted(*args, **kwargs):
        """IsHighlighted() -> bool"""
        return _ogl.PyShape_IsHighlighted(*args, **kwargs)

    def Selected(*args, **kwargs):
        """Selected() -> bool"""
        return _ogl.PyShape_Selected(*args, **kwargs)

    def AncestorSelected(*args, **kwargs):
        """AncestorSelected() -> bool"""
        return _ogl.PyShape_AncestorSelected(*args, **kwargs)

    def SetSensitivityFilter(*args, **kwargs):
        """SetSensitivityFilter(int sens=OP_ALL, bool recursive=False)"""
        return _ogl.PyShape_SetSensitivityFilter(*args, **kwargs)

    def GetSensitivityFilter(*args, **kwargs):
        """GetSensitivityFilter() -> int"""
        return _ogl.PyShape_GetSensitivityFilter(*args, **kwargs)

    def SetDraggable(*args, **kwargs):
        """SetDraggable(bool drag, bool recursive=False)"""
        return _ogl.PyShape_SetDraggable(*args, **kwargs)

    def SetFixedSize(*args, **kwargs):
        """SetFixedSize(bool x, bool y)"""
        return _ogl.PyShape_SetFixedSize(*args, **kwargs)

    def GetFixedSize(*args, **kwargs):
        """GetFixedSize(bool OUTPUT, bool OUTPUT)"""
        return _ogl.PyShape_GetFixedSize(*args, **kwargs)

    def GetFixedWidth(*args, **kwargs):
        """GetFixedWidth() -> bool"""
        return _ogl.PyShape_GetFixedWidth(*args, **kwargs)

    def GetFixedHeight(*args, **kwargs):
        """GetFixedHeight() -> bool"""
        return _ogl.PyShape_GetFixedHeight(*args, **kwargs)

    def SetSpaceAttachments(*args, **kwargs):
        """SetSpaceAttachments(bool sp)"""
        return _ogl.PyShape_SetSpaceAttachments(*args, **kwargs)

    def GetSpaceAttachments(*args, **kwargs):
        """GetSpaceAttachments() -> bool"""
        return _ogl.PyShape_GetSpaceAttachments(*args, **kwargs)

    def SetShadowMode(*args, **kwargs):
        """SetShadowMode(int mode, bool redraw=False)"""
        return _ogl.PyShape_SetShadowMode(*args, **kwargs)

    def GetShadowMode(*args, **kwargs):
        """GetShadowMode() -> int"""
        return _ogl.PyShape_GetShadowMode(*args, **kwargs)

    def HitTest(*args, **kwargs):
        """HitTest(double x, double y, int OUTPUT, double OUTPUT) -> bool"""
        return _ogl.PyShape_HitTest(*args, **kwargs)

    def SetCentreResize(*args, **kwargs):
        """SetCentreResize(bool cr)"""
        return _ogl.PyShape_SetCentreResize(*args, **kwargs)

    def GetCentreResize(*args, **kwargs):
        """GetCentreResize() -> bool"""
        return _ogl.PyShape_GetCentreResize(*args, **kwargs)

    def SetMaintainAspectRatio(*args, **kwargs):
        """SetMaintainAspectRatio(bool ar)"""
        return _ogl.PyShape_SetMaintainAspectRatio(*args, **kwargs)

    def GetMaintainAspectRatio(*args, **kwargs):
        """GetMaintainAspectRatio() -> bool"""
        return _ogl.PyShape_GetMaintainAspectRatio(*args, **kwargs)

    def GetLines(*args, **kwargs):
        """GetLines() -> PyObject"""
        return _ogl.PyShape_GetLines(*args, **kwargs)

    def SetDisableLabel(*args, **kwargs):
        """SetDisableLabel(bool flag)"""
        return _ogl.PyShape_SetDisableLabel(*args, **kwargs)

    def GetDisableLabel(*args, **kwargs):
        """GetDisableLabel() -> bool"""
        return _ogl.PyShape_GetDisableLabel(*args, **kwargs)

    def SetAttachmentMode(*args, **kwargs):
        """SetAttachmentMode(int mode)"""
        return _ogl.PyShape_SetAttachmentMode(*args, **kwargs)

    def GetAttachmentMode(*args, **kwargs):
        """GetAttachmentMode() -> int"""
        return _ogl.PyShape_GetAttachmentMode(*args, **kwargs)

    def SetId(*args, **kwargs):
        """SetId(long i)"""
        return _ogl.PyShape_SetId(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId() -> long"""
        return _ogl.PyShape_GetId(*args, **kwargs)

    def SetPen(*args, **kwargs):
        """SetPen(wxPen pen)"""
        return _ogl.PyShape_SetPen(*args, **kwargs)

    def SetBrush(*args, **kwargs):
        """SetBrush(wxBrush brush)"""
        return _ogl.PyShape_SetBrush(*args, **kwargs)

    def SetClientData(self, data):
        self.clientData = data
    def GetClientData(self):
        if hasattr(self, 'clientData'):
            return self.clientData
        else:
            return None

    def Show(*args, **kwargs):
        """Show(bool show)"""
        return _ogl.PyShape_Show(*args, **kwargs)

    def IsShown(*args, **kwargs):
        """IsShown() -> bool"""
        return _ogl.PyShape_IsShown(*args, **kwargs)

    def Move(*args, **kwargs):
        """Move(wxDC dc, double x1, double y1, bool display=True)"""
        return _ogl.PyShape_Move(*args, **kwargs)

    def Erase(*args, **kwargs):
        """Erase(wxDC dc)"""
        return _ogl.PyShape_Erase(*args, **kwargs)

    def EraseContents(*args, **kwargs):
        """EraseContents(wxDC dc)"""
        return _ogl.PyShape_EraseContents(*args, **kwargs)

    def Draw(*args, **kwargs):
        """Draw(wxDC dc)"""
        return _ogl.PyShape_Draw(*args, **kwargs)

    def Flash(*args, **kwargs):
        """Flash()"""
        return _ogl.PyShape_Flash(*args, **kwargs)

    def MoveLinks(*args, **kwargs):
        """MoveLinks(wxDC dc)"""
        return _ogl.PyShape_MoveLinks(*args, **kwargs)

    def DrawContents(*args, **kwargs):
        """DrawContents(wxDC dc)"""
        return _ogl.PyShape_DrawContents(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(double x, double y, bool recursive=True)"""
        return _ogl.PyShape_SetSize(*args, **kwargs)

    def SetAttachmentSize(*args, **kwargs):
        """SetAttachmentSize(double x, double y)"""
        return _ogl.PyShape_SetAttachmentSize(*args, **kwargs)

    def Attach(*args, **kwargs):
        """Attach(PyShapeCanvas can)"""
        return _ogl.PyShape_Attach(*args, **kwargs)

    def Detach(*args, **kwargs):
        """Detach()"""
        return _ogl.PyShape_Detach(*args, **kwargs)

    def Constrain(*args, **kwargs):
        """Constrain() -> bool"""
        return _ogl.PyShape_Constrain(*args, **kwargs)

    def AddLine(*args, **kwargs):
        """AddLine(PyLineShape line, PyShape other, int attachFrom=0, 
    int attachTo=0, int positionFrom=-1, int positionTo=-1)"""
        return _ogl.PyShape_AddLine(*args, **kwargs)

    def GetLinePosition(*args, **kwargs):
        """GetLinePosition(PyLineShape line) -> int"""
        return _ogl.PyShape_GetLinePosition(*args, **kwargs)

    def AddText(*args, **kwargs):
        """AddText(wxString string)"""
        return _ogl.PyShape_AddText(*args, **kwargs)

    def GetPen(*args, **kwargs):
        """GetPen() -> wxPen"""
        return _ogl.PyShape_GetPen(*args, **kwargs)

    def GetBrush(*args, **kwargs):
        """GetBrush() -> wxBrush"""
        return _ogl.PyShape_GetBrush(*args, **kwargs)

    def SetDefaultRegionSize(*args, **kwargs):
        """SetDefaultRegionSize()"""
        return _ogl.PyShape_SetDefaultRegionSize(*args, **kwargs)

    def FormatText(*args, **kwargs):
        """FormatText(wxDC dc, wxString s, int regionId=0)"""
        return _ogl.PyShape_FormatText(*args, **kwargs)

    def SetFormatMode(*args, **kwargs):
        """SetFormatMode(int mode, int regionId=0)"""
        return _ogl.PyShape_SetFormatMode(*args, **kwargs)

    def GetFormatMode(*args, **kwargs):
        """GetFormatMode(int regionId=0) -> int"""
        return _ogl.PyShape_GetFormatMode(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(wxFont font, int regionId=0)"""
        return _ogl.PyShape_SetFont(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont(int regionId=0) -> wxFont"""
        return _ogl.PyShape_GetFont(*args, **kwargs)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(wxString colour, int regionId=0)"""
        return _ogl.PyShape_SetTextColour(*args, **kwargs)

    def GetTextColour(*args, **kwargs):
        """GetTextColour(int regionId=0) -> wxString"""
        return _ogl.PyShape_GetTextColour(*args, **kwargs)

    def GetNumberOfTextRegions(*args, **kwargs):
        """GetNumberOfTextRegions() -> int"""
        return _ogl.PyShape_GetNumberOfTextRegions(*args, **kwargs)

    def SetRegionName(*args, **kwargs):
        """SetRegionName(wxString name, int regionId=0)"""
        return _ogl.PyShape_SetRegionName(*args, **kwargs)

    def GetRegionName(*args, **kwargs):
        """GetRegionName(int regionId) -> wxString"""
        return _ogl.PyShape_GetRegionName(*args, **kwargs)

    def GetRegionId(*args, **kwargs):
        """GetRegionId(wxString name) -> int"""
        return _ogl.PyShape_GetRegionId(*args, **kwargs)

    def NameRegions(*args, **kwargs):
        """NameRegions(wxString parentName="")"""
        return _ogl.PyShape_NameRegions(*args, **kwargs)

    def GetRegions(*args, **kwargs):
        """GetRegions() -> PyObject"""
        return _ogl.PyShape_GetRegions(*args, **kwargs)

    def AddRegion(*args, **kwargs):
        """AddRegion(ShapeRegion region)"""
        return _ogl.PyShape_AddRegion(*args, **kwargs)

    def ClearRegions(*args, **kwargs):
        """ClearRegions()"""
        return _ogl.PyShape_ClearRegions(*args, **kwargs)

    def AssignNewIds(*args, **kwargs):
        """AssignNewIds()"""
        return _ogl.PyShape_AssignNewIds(*args, **kwargs)

    def FindRegion(*args, **kwargs):
        """FindRegion(wxString regionName, int OUTPUT) -> PyShape"""
        return _ogl.PyShape_FindRegion(*args, **kwargs)

    def FindRegionNames(*args, **kwargs):
        """FindRegionNames(wxStringList list)"""
        return _ogl.PyShape_FindRegionNames(*args, **kwargs)

    def ClearText(*args, **kwargs):
        """ClearText(int regionId=0)"""
        return _ogl.PyShape_ClearText(*args, **kwargs)

    def RemoveLine(*args, **kwargs):
        """RemoveLine(PyLineShape line)"""
        return _ogl.PyShape_RemoveLine(*args, **kwargs)

    def GetAttachmentPosition(*args, **kwargs):
        """GetAttachmentPosition(int attachment, double OUTPUT, double OUTPUT, int nth=0, 
    int no_arcs=1, PyLineShape line=None) -> bool"""
        return _ogl.PyShape_GetAttachmentPosition(*args, **kwargs)

    def GetNumberOfAttachments(*args, **kwargs):
        """GetNumberOfAttachments() -> int"""
        return _ogl.PyShape_GetNumberOfAttachments(*args, **kwargs)

    def AttachmentIsValid(*args, **kwargs):
        """AttachmentIsValid(int attachment) -> bool"""
        return _ogl.PyShape_AttachmentIsValid(*args, **kwargs)

    def GetAttachmentPositionEdge(*args, **kwargs):
        """GetAttachmentPositionEdge(int attachment, double OUTPUT, double OUTPUT, int nth=0, 
    int no_arcs=1, PyLineShape line=None) -> bool"""
        return _ogl.PyShape_GetAttachmentPositionEdge(*args, **kwargs)

    def CalcSimpleAttachment(*args, **kwargs):
        """CalcSimpleAttachment(RealPoint pt1, RealPoint pt2, int nth, int noArcs, 
    PyLineShape line) -> RealPoint"""
        return _ogl.PyShape_CalcSimpleAttachment(*args, **kwargs)

    def AttachmentSortTest(*args, **kwargs):
        """AttachmentSortTest(int attachmentPoint, RealPoint pt1, RealPoint pt2) -> bool"""
        return _ogl.PyShape_AttachmentSortTest(*args, **kwargs)

    def EraseLinks(*args, **kwargs):
        """EraseLinks(wxDC dc, int attachment=-1, bool recurse=False)"""
        return _ogl.PyShape_EraseLinks(*args, **kwargs)

    def DrawLinks(*args, **kwargs):
        """DrawLinks(wxDC dc, int attachment=-1, bool recurse=False)"""
        return _ogl.PyShape_DrawLinks(*args, **kwargs)

    def MoveLineToNewAttachment(*args, **kwargs):
        """MoveLineToNewAttachment(wxDC dc, PyLineShape to_move, double x, double y) -> bool"""
        return _ogl.PyShape_MoveLineToNewAttachment(*args, **kwargs)

    def ApplyAttachmentOrdering(*args, **kwargs):
        """ApplyAttachmentOrdering(PyObject linesToSort)"""
        return _ogl.PyShape_ApplyAttachmentOrdering(*args, **kwargs)

    def GetBranchingAttachmentRoot(*args, **kwargs):
        """GetBranchingAttachmentRoot(int attachment) -> RealPoint"""
        return _ogl.PyShape_GetBranchingAttachmentRoot(*args, **kwargs)

    def GetBranchingAttachmentInfo(*args, **kwargs):
        """GetBranchingAttachmentInfo(int attachment, RealPoint root, RealPoint neck, RealPoint shoulder1, 
    RealPoint shoulder2) -> bool"""
        return _ogl.PyShape_GetBranchingAttachmentInfo(*args, **kwargs)

    def GetBranchingAttachmentPoint(*args, **kwargs):
        """GetBranchingAttachmentPoint(int attachment, int n, RealPoint attachmentPoint, RealPoint stemPoint) -> bool"""
        return _ogl.PyShape_GetBranchingAttachmentPoint(*args, **kwargs)

    def GetAttachmentLineCount(*args, **kwargs):
        """GetAttachmentLineCount(int attachment) -> int"""
        return _ogl.PyShape_GetAttachmentLineCount(*args, **kwargs)

    def SetBranchNeckLength(*args, **kwargs):
        """SetBranchNeckLength(int len)"""
        return _ogl.PyShape_SetBranchNeckLength(*args, **kwargs)

    def GetBranchNeckLength(*args, **kwargs):
        """GetBranchNeckLength() -> int"""
        return _ogl.PyShape_GetBranchNeckLength(*args, **kwargs)

    def SetBranchStemLength(*args, **kwargs):
        """SetBranchStemLength(int len)"""
        return _ogl.PyShape_SetBranchStemLength(*args, **kwargs)

    def GetBranchStemLength(*args, **kwargs):
        """GetBranchStemLength() -> int"""
        return _ogl.PyShape_GetBranchStemLength(*args, **kwargs)

    def SetBranchSpacing(*args, **kwargs):
        """SetBranchSpacing(int len)"""
        return _ogl.PyShape_SetBranchSpacing(*args, **kwargs)

    def GetBranchSpacing(*args, **kwargs):
        """GetBranchSpacing() -> int"""
        return _ogl.PyShape_GetBranchSpacing(*args, **kwargs)

    def SetBranchStyle(*args, **kwargs):
        """SetBranchStyle(long style)"""
        return _ogl.PyShape_SetBranchStyle(*args, **kwargs)

    def GetBranchStyle(*args, **kwargs):
        """GetBranchStyle() -> long"""
        return _ogl.PyShape_GetBranchStyle(*args, **kwargs)

    def PhysicalToLogicalAttachment(*args, **kwargs):
        """PhysicalToLogicalAttachment(int physicalAttachment) -> int"""
        return _ogl.PyShape_PhysicalToLogicalAttachment(*args, **kwargs)

    def LogicalToPhysicalAttachment(*args, **kwargs):
        """LogicalToPhysicalAttachment(int logicalAttachment) -> int"""
        return _ogl.PyShape_LogicalToPhysicalAttachment(*args, **kwargs)

    def Draggable(*args, **kwargs):
        """Draggable() -> bool"""
        return _ogl.PyShape_Draggable(*args, **kwargs)

    def HasDescendant(*args, **kwargs):
        """HasDescendant(PyShape image) -> bool"""
        return _ogl.PyShape_HasDescendant(*args, **kwargs)

    def CreateNewCopy(*args, **kwargs):
        """CreateNewCopy(bool resetMapping=True, bool recompute=True) -> PyShape"""
        return _ogl.PyShape_CreateNewCopy(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(PyShape copy)"""
        return _ogl.PyShape_Copy(*args, **kwargs)

    def CopyWithHandler(*args, **kwargs):
        """CopyWithHandler(PyShape copy)"""
        return _ogl.PyShape_CopyWithHandler(*args, **kwargs)

    def Rotate(*args, **kwargs):
        """Rotate(double x, double y, double theta)"""
        return _ogl.PyShape_Rotate(*args, **kwargs)

    def GetRotation(*args, **kwargs):
        """GetRotation() -> double"""
        return _ogl.PyShape_GetRotation(*args, **kwargs)

    def ClearAttachments(*args, **kwargs):
        """ClearAttachments()"""
        return _ogl.PyShape_ClearAttachments(*args, **kwargs)

    def Recentre(*args, **kwargs):
        """Recentre(wxDC dc)"""
        return _ogl.PyShape_Recentre(*args, **kwargs)

    def ClearPointList(*args, **kwargs):
        """ClearPointList(wxList list)"""
        return _ogl.PyShape_ClearPointList(*args, **kwargs)

    def GetBackgroundPen(*args, **kwargs):
        """GetBackgroundPen() -> wxPen"""
        return _ogl.PyShape_GetBackgroundPen(*args, **kwargs)

    def GetBackgroundBrush(*args, **kwargs):
        """GetBackgroundBrush() -> wxBrush"""
        return _ogl.PyShape_GetBackgroundBrush(*args, **kwargs)

    def base_OnDelete(*args, **kwargs):
        """base_OnDelete()"""
        return _ogl.PyShape_base_OnDelete(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=False)"""
        return _ogl.PyShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyShape_base_OnEndSize(*args, **kwargs)


class PyShapePtr(PyShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyShape
_ogl.PyShape_swigregister(PyShapePtr)

oglMETAFLAGS_OUTLINE = _ogl.oglMETAFLAGS_OUTLINE
oglMETAFLAGS_ATTACHMENTS = _ogl.oglMETAFLAGS_ATTACHMENTS
class PseudoMetaFile(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPseudoMetaFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> PseudoMetaFile"""
        newobj = _ogl.new_PseudoMetaFile(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_ogl.delete_PseudoMetaFile):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Draw(*args, **kwargs):
        """Draw(wxDC dc, double xoffset, double yoffset)"""
        return _ogl.PseudoMetaFile_Draw(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear()"""
        return _ogl.PseudoMetaFile_Clear(*args, **kwargs)

    def Copy(*args, **kwargs):
        """Copy(PseudoMetaFile copy)"""
        return _ogl.PseudoMetaFile_Copy(*args, **kwargs)

    def Scale(*args, **kwargs):
        """Scale(double sx, double sy)"""
        return _ogl.PseudoMetaFile_Scale(*args, **kwargs)

    def ScaleTo(*args, **kwargs):
        """ScaleTo(double w, double h)"""
        return _ogl.PseudoMetaFile_ScaleTo(*args, **kwargs)

    def Translate(*args, **kwargs):
        """Translate(double x, double y)"""
        return _ogl.PseudoMetaFile_Translate(*args, **kwargs)

    def Rotate(*args, **kwargs):
        """Rotate(double x, double y, double theta)"""
        return _ogl.PseudoMetaFile_Rotate(*args, **kwargs)

    def LoadFromMetaFile(*args, **kwargs):
        """LoadFromMetaFile(wxString filename, double width, double height) -> bool"""
        return _ogl.PseudoMetaFile_LoadFromMetaFile(*args, **kwargs)

    def GetBounds(*args, **kwargs):
        """GetBounds(double minX, double minY, double maxX, double maxY)"""
        return _ogl.PseudoMetaFile_GetBounds(*args, **kwargs)

    def CalculateSize(*args, **kwargs):
        """CalculateSize(PyDrawnShape shape)"""
        return _ogl.PseudoMetaFile_CalculateSize(*args, **kwargs)

    def SetRotateable(*args, **kwargs):
        """SetRotateable(bool rot)"""
        return _ogl.PseudoMetaFile_SetRotateable(*args, **kwargs)

    def GetRotateable(*args, **kwargs):
        """GetRotateable() -> bool"""
        return _ogl.PseudoMetaFile_GetRotateable(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(double w, double h)"""
        return _ogl.PseudoMetaFile_SetSize(*args, **kwargs)

    def SetFillBrush(*args, **kwargs):
        """SetFillBrush(wxBrush brush)"""
        return _ogl.PseudoMetaFile_SetFillBrush(*args, **kwargs)

    def GetFillBrush(*args, **kwargs):
        """GetFillBrush() -> wxBrush"""
        return _ogl.PseudoMetaFile_GetFillBrush(*args, **kwargs)

    def SetOutlinePen(*args, **kwargs):
        """SetOutlinePen(wxPen pen)"""
        return _ogl.PseudoMetaFile_SetOutlinePen(*args, **kwargs)

    def GetOutlinePen(*args, **kwargs):
        """GetOutlinePen() -> wxPen"""
        return _ogl.PseudoMetaFile_GetOutlinePen(*args, **kwargs)

    def SetOutlineOp(*args, **kwargs):
        """SetOutlineOp(int op)"""
        return _ogl.PseudoMetaFile_SetOutlineOp(*args, **kwargs)

    def GetOutlineOp(*args, **kwargs):
        """GetOutlineOp() -> int"""
        return _ogl.PseudoMetaFile_GetOutlineOp(*args, **kwargs)

    def IsValid(*args, **kwargs):
        """IsValid() -> bool"""
        return _ogl.PseudoMetaFile_IsValid(*args, **kwargs)

    def DrawLine(*args, **kwargs):
        """DrawLine(Point pt1, Point pt2)"""
        return _ogl.PseudoMetaFile_DrawLine(*args, **kwargs)

    def DrawRectangle(*args, **kwargs):
        """DrawRectangle(Rect rect)"""
        return _ogl.PseudoMetaFile_DrawRectangle(*args, **kwargs)

    def DrawRoundedRectangle(*args, **kwargs):
        """DrawRoundedRectangle(Rect rect, double radius)"""
        return _ogl.PseudoMetaFile_DrawRoundedRectangle(*args, **kwargs)

    def DrawArc(*args, **kwargs):
        """DrawArc(Point centrePt, Point startPt, Point endPt)"""
        return _ogl.PseudoMetaFile_DrawArc(*args, **kwargs)

    def DrawEllipticArc(*args, **kwargs):
        """DrawEllipticArc(Rect rect, double startAngle, double endAngle)"""
        return _ogl.PseudoMetaFile_DrawEllipticArc(*args, **kwargs)

    def DrawEllipse(*args, **kwargs):
        """DrawEllipse(Rect rect)"""
        return _ogl.PseudoMetaFile_DrawEllipse(*args, **kwargs)

    def DrawPoint(*args, **kwargs):
        """DrawPoint(Point pt)"""
        return _ogl.PseudoMetaFile_DrawPoint(*args, **kwargs)

    def DrawText(*args, **kwargs):
        """DrawText(wxString text, Point pt)"""
        return _ogl.PseudoMetaFile_DrawText(*args, **kwargs)

    def DrawLines(*args, **kwargs):
        """DrawLines(int PCOUNT, Point points)"""
        return _ogl.PseudoMetaFile_DrawLines(*args, **kwargs)

    def DrawPolygon(*args, **kwargs):
        """DrawPolygon(int PCOUNT, Point points, int flags=0)"""
        return _ogl.PseudoMetaFile_DrawPolygon(*args, **kwargs)

    def DrawSpline(*args, **kwargs):
        """DrawSpline(int PCOUNT, Point points)"""
        return _ogl.PseudoMetaFile_DrawSpline(*args, **kwargs)

    def SetClippingRect(*args, **kwargs):
        """SetClippingRect(Rect rect)"""
        return _ogl.PseudoMetaFile_SetClippingRect(*args, **kwargs)

    def DestroyClippingRect(*args, **kwargs):
        """DestroyClippingRect()"""
        return _ogl.PseudoMetaFile_DestroyClippingRect(*args, **kwargs)

    def SetPen(*args, **kwargs):
        """SetPen(wxPen pen, bool isOutline=FALSE)"""
        return _ogl.PseudoMetaFile_SetPen(*args, **kwargs)

    def SetBrush(*args, **kwargs):
        """SetBrush(wxBrush brush, bool isFill=FALSE)"""
        return _ogl.PseudoMetaFile_SetBrush(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(wxFont font)"""
        return _ogl.PseudoMetaFile_SetFont(*args, **kwargs)

    def SetTextColour(*args, **kwargs):
        """SetTextColour(wxColour colour)"""
        return _ogl.PseudoMetaFile_SetTextColour(*args, **kwargs)

    def SetBackgroundColour(*args, **kwargs):
        """SetBackgroundColour(wxColour colour)"""
        return _ogl.PseudoMetaFile_SetBackgroundColour(*args, **kwargs)

    def SetBackgroundMode(*args, **kwargs):
        """SetBackgroundMode(int mode)"""
        return _ogl.PseudoMetaFile_SetBackgroundMode(*args, **kwargs)


class PseudoMetaFilePtr(PseudoMetaFile):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PseudoMetaFile
_ogl.PseudoMetaFile_swigregister(PseudoMetaFilePtr)

class PyRectangleShape(PyShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyRectangleShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(double width=0.0, double height=0.0) -> PyRectangleShape"""
        newobj = _ogl.new_PyRectangleShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyRectangleShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyRectangleShape__setCallbackInfo(*args, **kwargs)

    def SetCornerRadius(*args, **kwargs):
        """SetCornerRadius(double radius)"""
        return _ogl.PyRectangleShape_SetCornerRadius(*args, **kwargs)

    def base_OnDelete(*args, **kwargs):
        """base_OnDelete()"""
        return _ogl.PyRectangleShape_base_OnDelete(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyRectangleShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyRectangleShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyRectangleShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyRectangleShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyRectangleShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyRectangleShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyRectangleShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyRectangleShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyRectangleShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyRectangleShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyRectangleShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyRectangleShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyRectangleShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyRectangleShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyRectangleShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyRectangleShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyRectangleShape_base_OnEndSize(*args, **kwargs)


class PyRectangleShapePtr(PyRectangleShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyRectangleShape
_ogl.PyRectangleShape_swigregister(PyRectangleShapePtr)

class PyControlPoint(PyRectangleShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyControlPoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(PyShapeCanvas the_canvas=None, PyShape object=None, 
    double size=0.0, double the_xoffset=0.0, double the_yoffset=0.0, 
    int the_type=0) -> PyControlPoint"""
        newobj = _ogl.new_PyControlPoint(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyControlPoint)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyControlPoint__setCallbackInfo(*args, **kwargs)

    def SetCornerRadius(*args, **kwargs):
        """SetCornerRadius(double radius)"""
        return _ogl.PyControlPoint_SetCornerRadius(*args, **kwargs)

    def base_OnDelete(*args, **kwargs):
        """base_OnDelete()"""
        return _ogl.PyControlPoint_base_OnDelete(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyControlPoint_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyControlPoint_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyControlPoint_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyControlPoint_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyControlPoint_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyControlPoint_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyControlPoint_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyControlPoint_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyControlPoint_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyControlPoint_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyControlPoint_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyControlPoint_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyControlPoint_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyControlPoint_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyControlPoint_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyControlPoint_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyControlPoint_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyControlPoint_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyControlPoint_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyControlPoint_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyControlPoint_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyControlPoint_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyControlPoint_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyControlPoint_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyControlPoint_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyControlPoint_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyControlPoint_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyControlPoint_base_OnEndSize(*args, **kwargs)


class PyControlPointPtr(PyControlPoint):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyControlPoint
_ogl.PyControlPoint_swigregister(PyControlPointPtr)

class PyBitmapShape(PyRectangleShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyBitmapShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> PyBitmapShape"""
        newobj = _ogl.new_PyBitmapShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyBitmapShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyBitmapShape__setCallbackInfo(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap() -> wxBitmap"""
        return _ogl.PyBitmapShape_GetBitmap(*args, **kwargs)

    def GetFilename(*args, **kwargs):
        """GetFilename() -> wxString"""
        return _ogl.PyBitmapShape_GetFilename(*args, **kwargs)

    def SetBitmap(*args, **kwargs):
        """SetBitmap(wxBitmap bitmap)"""
        return _ogl.PyBitmapShape_SetBitmap(*args, **kwargs)

    def SetFilename(*args, **kwargs):
        """SetFilename(wxString filename)"""
        return _ogl.PyBitmapShape_SetFilename(*args, **kwargs)

    def base_OnDelete(*args, **kwargs):
        """base_OnDelete()"""
        return _ogl.PyBitmapShape_base_OnDelete(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyBitmapShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyBitmapShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyBitmapShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyBitmapShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyBitmapShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyBitmapShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyBitmapShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyBitmapShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyBitmapShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyBitmapShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyBitmapShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyBitmapShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyBitmapShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyBitmapShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyBitmapShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyBitmapShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyBitmapShape_base_OnEndSize(*args, **kwargs)


class PyBitmapShapePtr(PyBitmapShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyBitmapShape
_ogl.PyBitmapShape_swigregister(PyBitmapShapePtr)

class PyDrawnShape(PyRectangleShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyDrawnShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> PyDrawnShape"""
        newobj = _ogl.new_PyDrawnShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyDrawnShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyDrawnShape__setCallbackInfo(*args, **kwargs)

    def CalculateSize(*args, **kwargs):
        """CalculateSize()"""
        return _ogl.PyDrawnShape_CalculateSize(*args, **kwargs)

    def DestroyClippingRect(*args, **kwargs):
        """DestroyClippingRect()"""
        return _ogl.PyDrawnShape_DestroyClippingRect(*args, **kwargs)

    def DrawArc(*args, **kwargs):
        """DrawArc(Point centrePoint, Point startPoint, Point endPoint)"""
        return _ogl.PyDrawnShape_DrawArc(*args, **kwargs)

    def DrawAtAngle(*args, **kwargs):
        """DrawAtAngle(int angle)"""
        return _ogl.PyDrawnShape_DrawAtAngle(*args, **kwargs)

    def DrawEllipticArc(*args, **kwargs):
        """DrawEllipticArc(Rect rect, double startAngle, double endAngle)"""
        return _ogl.PyDrawnShape_DrawEllipticArc(*args, **kwargs)

    def DrawLine(*args, **kwargs):
        """DrawLine(Point point1, Point point2)"""
        return _ogl.PyDrawnShape_DrawLine(*args, **kwargs)

    def DrawLines(*args, **kwargs):
        """DrawLines(int PCOUNT, Point points)"""
        return _ogl.PyDrawnShape_DrawLines(*args, **kwargs)

    def DrawPoint(*args, **kwargs):
        """DrawPoint(Point point)"""
        return _ogl.PyDrawnShape_DrawPoint(*args, **kwargs)

    def DrawPolygon(*args, **kwargs):
        """DrawPolygon(int PCOUNT, Point points, int flags=0)"""
        return _ogl.PyDrawnShape_DrawPolygon(*args, **kwargs)

    def DrawRectangle(*args, **kwargs):
        """DrawRectangle(Rect rect)"""
        return _ogl.PyDrawnShape_DrawRectangle(*args, **kwargs)

    def DrawRoundedRectangle(*args, **kwargs):
        """DrawRoundedRectangle(Rect rect, double radius)"""
        return _ogl.PyDrawnShape_DrawRoundedRectangle(*args, **kwargs)

    def DrawSpline(*args, **kwargs):
        """DrawSpline(int PCOUNT, Point points)"""
        return _ogl.PyDrawnShape_DrawSpline(*args, **kwargs)

    def DrawText(*args, **kwargs):
        """DrawText(wxString text, Point point)"""
        return _ogl.PyDrawnShape_DrawText(*args, **kwargs)

    def GetAngle(*args, **kwargs):
        """GetAngle() -> int"""
        return _ogl.PyDrawnShape_GetAngle(*args, **kwargs)

    def GetMetaFile(*args, **kwargs):
        """GetMetaFile() -> PseudoMetaFile"""
        return _ogl.PyDrawnShape_GetMetaFile(*args, **kwargs)

    def GetRotation(*args, **kwargs):
        """GetRotation() -> double"""
        return _ogl.PyDrawnShape_GetRotation(*args, **kwargs)

    def LoadFromMetaFile(*args, **kwargs):
        """LoadFromMetaFile(wxString filename) -> bool"""
        return _ogl.PyDrawnShape_LoadFromMetaFile(*args, **kwargs)

    def Rotate(*args, **kwargs):
        """Rotate(double x, double y, double theta)"""
        return _ogl.PyDrawnShape_Rotate(*args, **kwargs)

    def SetClippingRect(*args, **kwargs):
        """SetClippingRect(Rect rect)"""
        return _ogl.PyDrawnShape_SetClippingRect(*args, **kwargs)

    def SetDrawnBackgroundColour(*args, **kwargs):
        """SetDrawnBackgroundColour(wxColour colour)"""
        return _ogl.PyDrawnShape_SetDrawnBackgroundColour(*args, **kwargs)

    def SetDrawnBackgroundMode(*args, **kwargs):
        """SetDrawnBackgroundMode(int mode)"""
        return _ogl.PyDrawnShape_SetDrawnBackgroundMode(*args, **kwargs)

    def SetDrawnBrush(*args, **kwargs):
        """SetDrawnBrush(wxBrush pen, bool isOutline=FALSE)"""
        return _ogl.PyDrawnShape_SetDrawnBrush(*args, **kwargs)

    def SetDrawnFont(*args, **kwargs):
        """SetDrawnFont(wxFont font)"""
        return _ogl.PyDrawnShape_SetDrawnFont(*args, **kwargs)

    def SetDrawnPen(*args, **kwargs):
        """SetDrawnPen(wxPen pen, bool isOutline=FALSE)"""
        return _ogl.PyDrawnShape_SetDrawnPen(*args, **kwargs)

    def SetDrawnTextColour(*args, **kwargs):
        """SetDrawnTextColour(wxColour colour)"""
        return _ogl.PyDrawnShape_SetDrawnTextColour(*args, **kwargs)

    def Scale(*args, **kwargs):
        """Scale(double sx, double sy)"""
        return _ogl.PyDrawnShape_Scale(*args, **kwargs)

    def SetSaveToFile(*args, **kwargs):
        """SetSaveToFile(bool save)"""
        return _ogl.PyDrawnShape_SetSaveToFile(*args, **kwargs)

    def Translate(*args, **kwargs):
        """Translate(double x, double y)"""
        return _ogl.PyDrawnShape_Translate(*args, **kwargs)

    def base_OnDelete(*args, **kwargs):
        """base_OnDelete()"""
        return _ogl.PyDrawnShape_base_OnDelete(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyDrawnShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyDrawnShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyDrawnShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyDrawnShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyDrawnShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyDrawnShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyDrawnShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyDrawnShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyDrawnShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyDrawnShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyDrawnShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyDrawnShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyDrawnShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyDrawnShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyDrawnShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyDrawnShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyDrawnShape_base_OnEndSize(*args, **kwargs)


class PyDrawnShapePtr(PyDrawnShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyDrawnShape
_ogl.PyDrawnShape_swigregister(PyDrawnShapePtr)

class OGLConstraint(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxOGLConstraint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int type, PyShape constraining, PyObject constrained) -> OGLConstraint"""
        newobj = _ogl.new_OGLConstraint(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Evaluate(*args, **kwargs):
        """Evaluate() -> bool"""
        return _ogl.OGLConstraint_Evaluate(*args, **kwargs)

    def SetSpacing(*args, **kwargs):
        """SetSpacing(double x, double y)"""
        return _ogl.OGLConstraint_SetSpacing(*args, **kwargs)

    def Equals(*args, **kwargs):
        """Equals(double a, double b) -> bool"""
        return _ogl.OGLConstraint_Equals(*args, **kwargs)


class OGLConstraintPtr(OGLConstraint):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = OGLConstraint
_ogl.OGLConstraint_swigregister(OGLConstraintPtr)

class PyCompositeShape(PyRectangleShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyCompositeShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> PyCompositeShape"""
        newobj = _ogl.new_PyCompositeShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyCompositeShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyCompositeShape__setCallbackInfo(*args, **kwargs)

    def AddChild(*args, **kwargs):
        """AddChild(PyShape child, PyShape addAfter=None)"""
        return _ogl.PyCompositeShape_AddChild(*args, **kwargs)

    def AddConstraint(*args, **kwargs):
        """AddConstraint(OGLConstraint constraint) -> OGLConstraint"""
        return _ogl.PyCompositeShape_AddConstraint(*args, **kwargs)

    def AddConstrainedShapes(*args, **kwargs):
        """AddConstrainedShapes(int type, PyShape constraining, PyObject constrained) -> OGLConstraint"""
        return _ogl.PyCompositeShape_AddConstrainedShapes(*args, **kwargs)

    def AddSimpleConstraint(*args, **kwargs):
        """AddSimpleConstraint(int type, PyShape constraining, PyShape constrained) -> OGLConstraint"""
        return _ogl.PyCompositeShape_AddSimpleConstraint(*args, **kwargs)

    def CalculateSize(*args, **kwargs):
        """CalculateSize()"""
        return _ogl.PyCompositeShape_CalculateSize(*args, **kwargs)

    def ContainsDivision(*args, **kwargs):
        """ContainsDivision(PyDivisionShape division) -> bool"""
        return _ogl.PyCompositeShape_ContainsDivision(*args, **kwargs)

    def DeleteConstraint(*args, **kwargs):
        """DeleteConstraint(OGLConstraint constraint)"""
        return _ogl.PyCompositeShape_DeleteConstraint(*args, **kwargs)

    def DeleteConstraintsInvolvingChild(*args, **kwargs):
        """DeleteConstraintsInvolvingChild(PyShape child)"""
        return _ogl.PyCompositeShape_DeleteConstraintsInvolvingChild(*args, **kwargs)

    def FindContainerImage(*args, **kwargs):
        """FindContainerImage() -> PyShape"""
        return _ogl.PyCompositeShape_FindContainerImage(*args, **kwargs)

    def GetConstraints(*args, **kwargs):
        """GetConstraints() -> PyObject"""
        return _ogl.PyCompositeShape_GetConstraints(*args, **kwargs)

    def GetDivisions(*args, **kwargs):
        """GetDivisions() -> PyObject"""
        return _ogl.PyCompositeShape_GetDivisions(*args, **kwargs)

    def MakeContainer(*args, **kwargs):
        """MakeContainer()"""
        return _ogl.PyCompositeShape_MakeContainer(*args, **kwargs)

    def Recompute(*args, **kwargs):
        """Recompute() -> bool"""
        return _ogl.PyCompositeShape_Recompute(*args, **kwargs)

    def RemoveChild(*args, **kwargs):
        """RemoveChild(PyShape child)"""
        return _ogl.PyCompositeShape_RemoveChild(*args, **kwargs)

    def base_OnDelete(*args, **kwargs):
        """base_OnDelete()"""
        return _ogl.PyCompositeShape_base_OnDelete(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyCompositeShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyCompositeShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyCompositeShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyCompositeShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyCompositeShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyCompositeShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyCompositeShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyCompositeShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyCompositeShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyCompositeShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyCompositeShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyCompositeShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyCompositeShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyCompositeShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyCompositeShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyCompositeShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyCompositeShape_base_OnEndSize(*args, **kwargs)


class PyCompositeShapePtr(PyCompositeShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyCompositeShape
_ogl.PyCompositeShape_swigregister(PyCompositeShapePtr)

class PyDividedShape(PyRectangleShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyDividedShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(double width=0.0, double height=0.0) -> PyDividedShape"""
        newobj = _ogl.new_PyDividedShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyDividedShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyDividedShape__setCallbackInfo(*args, **kwargs)

    def EditRegions(*args, **kwargs):
        """EditRegions()"""
        return _ogl.PyDividedShape_EditRegions(*args, **kwargs)

    def SetRegionSizes(*args, **kwargs):
        """SetRegionSizes()"""
        return _ogl.PyDividedShape_SetRegionSizes(*args, **kwargs)

    def base_OnDelete(*args, **kwargs):
        """base_OnDelete()"""
        return _ogl.PyDividedShape_base_OnDelete(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyDividedShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyDividedShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyDividedShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyDividedShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyDividedShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyDividedShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyDividedShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDividedShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDividedShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDividedShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyDividedShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyDividedShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyDividedShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDividedShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDividedShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDividedShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDividedShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDividedShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDividedShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyDividedShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyDividedShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyDividedShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyDividedShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyDividedShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyDividedShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyDividedShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyDividedShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyDividedShape_base_OnEndSize(*args, **kwargs)


class PyDividedShapePtr(PyDividedShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyDividedShape
_ogl.PyDividedShape_swigregister(PyDividedShapePtr)

class PyDivisionShape(PyCompositeShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyDivisionShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> PyDivisionShape"""
        newobj = _ogl.new_PyDivisionShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyDivisionShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyDivisionShape__setCallbackInfo(*args, **kwargs)

    def AdjustBottom(*args, **kwargs):
        """AdjustBottom(double bottom, bool test)"""
        return _ogl.PyDivisionShape_AdjustBottom(*args, **kwargs)

    def AdjustLeft(*args, **kwargs):
        """AdjustLeft(double left, bool test)"""
        return _ogl.PyDivisionShape_AdjustLeft(*args, **kwargs)

    def AdjustRight(*args, **kwargs):
        """AdjustRight(double right, bool test)"""
        return _ogl.PyDivisionShape_AdjustRight(*args, **kwargs)

    def AdjustTop(*args, **kwargs):
        """AdjustTop(double top, bool test)"""
        return _ogl.PyDivisionShape_AdjustTop(*args, **kwargs)

    def Divide(*args, **kwargs):
        """Divide(int direction)"""
        return _ogl.PyDivisionShape_Divide(*args, **kwargs)

    def EditEdge(*args, **kwargs):
        """EditEdge(int side)"""
        return _ogl.PyDivisionShape_EditEdge(*args, **kwargs)

    def GetBottomSide(*args, **kwargs):
        """GetBottomSide() -> PyDivisionShape"""
        return _ogl.PyDivisionShape_GetBottomSide(*args, **kwargs)

    def GetHandleSide(*args, **kwargs):
        """GetHandleSide() -> int"""
        return _ogl.PyDivisionShape_GetHandleSide(*args, **kwargs)

    def GetLeftSide(*args, **kwargs):
        """GetLeftSide() -> PyDivisionShape"""
        return _ogl.PyDivisionShape_GetLeftSide(*args, **kwargs)

    def GetLeftSideColour(*args, **kwargs):
        """GetLeftSideColour() -> wxString"""
        return _ogl.PyDivisionShape_GetLeftSideColour(*args, **kwargs)

    def GetLeftSidePen(*args, **kwargs):
        """GetLeftSidePen() -> wxPen"""
        return _ogl.PyDivisionShape_GetLeftSidePen(*args, **kwargs)

    def GetRightSide(*args, **kwargs):
        """GetRightSide() -> PyDivisionShape"""
        return _ogl.PyDivisionShape_GetRightSide(*args, **kwargs)

    def GetTopSide(*args, **kwargs):
        """GetTopSide() -> PyDivisionShape"""
        return _ogl.PyDivisionShape_GetTopSide(*args, **kwargs)

    def GetTopSidePen(*args, **kwargs):
        """GetTopSidePen() -> wxPen"""
        return _ogl.PyDivisionShape_GetTopSidePen(*args, **kwargs)

    def ResizeAdjoining(*args, **kwargs):
        """ResizeAdjoining(int side, double newPos, bool test)"""
        return _ogl.PyDivisionShape_ResizeAdjoining(*args, **kwargs)

    def PopupMenu(*args, **kwargs):
        """PopupMenu(double x, double y)"""
        return _ogl.PyDivisionShape_PopupMenu(*args, **kwargs)

    def SetBottomSide(*args, **kwargs):
        """SetBottomSide(PyDivisionShape shape)"""
        return _ogl.PyDivisionShape_SetBottomSide(*args, **kwargs)

    def SetHandleSide(*args, **kwargs):
        """SetHandleSide(int side)"""
        return _ogl.PyDivisionShape_SetHandleSide(*args, **kwargs)

    def SetLeftSide(*args, **kwargs):
        """SetLeftSide(PyDivisionShape shape)"""
        return _ogl.PyDivisionShape_SetLeftSide(*args, **kwargs)

    def SetLeftSideColour(*args, **kwargs):
        """SetLeftSideColour(wxString colour)"""
        return _ogl.PyDivisionShape_SetLeftSideColour(*args, **kwargs)

    def SetLeftSidePen(*args, **kwargs):
        """SetLeftSidePen(wxPen pen)"""
        return _ogl.PyDivisionShape_SetLeftSidePen(*args, **kwargs)

    def SetRightSide(*args, **kwargs):
        """SetRightSide(PyDivisionShape shape)"""
        return _ogl.PyDivisionShape_SetRightSide(*args, **kwargs)

    def SetTopSide(*args, **kwargs):
        """SetTopSide(PyDivisionShape shape)"""
        return _ogl.PyDivisionShape_SetTopSide(*args, **kwargs)

    def SetTopSideColour(*args, **kwargs):
        """SetTopSideColour(wxString colour)"""
        return _ogl.PyDivisionShape_SetTopSideColour(*args, **kwargs)

    def SetTopSidePen(*args, **kwargs):
        """SetTopSidePen(wxPen pen)"""
        return _ogl.PyDivisionShape_SetTopSidePen(*args, **kwargs)

    def base_OnDelete(*args, **kwargs):
        """base_OnDelete()"""
        return _ogl.PyDivisionShape_base_OnDelete(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyDivisionShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyDivisionShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyDivisionShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyDivisionShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyDivisionShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyDivisionShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyDivisionShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyDivisionShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyDivisionShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyDivisionShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyDivisionShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyDivisionShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyDivisionShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyDivisionShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyDivisionShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyDivisionShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyDivisionShape_base_OnEndSize(*args, **kwargs)


class PyDivisionShapePtr(PyDivisionShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyDivisionShape
_ogl.PyDivisionShape_swigregister(PyDivisionShapePtr)

class PyEllipseShape(PyShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyEllipseShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(double width=0.0, double height=0.0) -> PyEllipseShape"""
        newobj = _ogl.new_PyEllipseShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyEllipseShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyEllipseShape__setCallbackInfo(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyEllipseShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyEllipseShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyEllipseShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyEllipseShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyEllipseShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyEllipseShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyEllipseShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyEllipseShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyEllipseShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyEllipseShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyEllipseShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyEllipseShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyEllipseShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyEllipseShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyEllipseShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyEllipseShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyEllipseShape_base_OnEndSize(*args, **kwargs)


class PyEllipseShapePtr(PyEllipseShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyEllipseShape
_ogl.PyEllipseShape_swigregister(PyEllipseShapePtr)

class PyCircleShape(PyEllipseShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyCircleShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(double width=0.0) -> PyCircleShape"""
        newobj = _ogl.new_PyCircleShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyCircleShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyCircleShape__setCallbackInfo(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyCircleShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyCircleShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyCircleShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyCircleShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyCircleShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyCircleShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyCircleShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCircleShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCircleShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCircleShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyCircleShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyCircleShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyCircleShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCircleShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCircleShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCircleShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCircleShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCircleShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyCircleShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyCircleShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyCircleShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyCircleShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyCircleShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyCircleShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyCircleShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyCircleShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyCircleShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyCircleShape_base_OnEndSize(*args, **kwargs)


class PyCircleShapePtr(PyCircleShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyCircleShape
_ogl.PyCircleShape_swigregister(PyCircleShapePtr)

class ArrowHead(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxArrowHead instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int type=0, int end=0, double size=0.0, double dist=0.0, 
    wxString name=wxPyEmptyString, PseudoMetaFile mf=None, 
    long arrowId=-1) -> ArrowHead"""
        newobj = _ogl.new_ArrowHead(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_ogl.delete_ArrowHead):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def _GetType(*args, **kwargs):
        """_GetType() -> int"""
        return _ogl.ArrowHead__GetType(*args, **kwargs)

    def GetPosition(*args, **kwargs):
        """GetPosition() -> int"""
        return _ogl.ArrowHead_GetPosition(*args, **kwargs)

    def SetPosition(*args, **kwargs):
        """SetPosition(int pos)"""
        return _ogl.ArrowHead_SetPosition(*args, **kwargs)

    def GetXOffset(*args, **kwargs):
        """GetXOffset() -> double"""
        return _ogl.ArrowHead_GetXOffset(*args, **kwargs)

    def GetYOffset(*args, **kwargs):
        """GetYOffset() -> double"""
        return _ogl.ArrowHead_GetYOffset(*args, **kwargs)

    def GetSpacing(*args, **kwargs):
        """GetSpacing() -> double"""
        return _ogl.ArrowHead_GetSpacing(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize() -> double"""
        return _ogl.ArrowHead_GetSize(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName() -> wxString"""
        return _ogl.ArrowHead_GetName(*args, **kwargs)

    def SetXOffset(*args, **kwargs):
        """SetXOffset(double x)"""
        return _ogl.ArrowHead_SetXOffset(*args, **kwargs)

    def SetYOffset(*args, **kwargs):
        """SetYOffset(double y)"""
        return _ogl.ArrowHead_SetYOffset(*args, **kwargs)

    def GetMetaFile(*args, **kwargs):
        """GetMetaFile() -> PseudoMetaFile"""
        return _ogl.ArrowHead_GetMetaFile(*args, **kwargs)

    def GetId(*args, **kwargs):
        """GetId() -> long"""
        return _ogl.ArrowHead_GetId(*args, **kwargs)

    def GetArrowEnd(*args, **kwargs):
        """GetArrowEnd() -> int"""
        return _ogl.ArrowHead_GetArrowEnd(*args, **kwargs)

    def GetArrowSize(*args, **kwargs):
        """GetArrowSize() -> double"""
        return _ogl.ArrowHead_GetArrowSize(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(double size)"""
        return _ogl.ArrowHead_SetSize(*args, **kwargs)

    def SetSpacing(*args, **kwargs):
        """SetSpacing(double sp)"""
        return _ogl.ArrowHead_SetSpacing(*args, **kwargs)


class ArrowHeadPtr(ArrowHead):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ArrowHead
_ogl.ArrowHead_swigregister(ArrowHeadPtr)

class PyLineShape(PyShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyLineShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> PyLineShape"""
        newobj = _ogl.new_PyLineShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyLineShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyLineShape__setCallbackInfo(*args, **kwargs)

    def AddArrow(*args, **kwargs):
        """AddArrow(int type, int end=ARROW_POSITION_END, double arrowSize=10.0, 
    double xOffset=0.0, wxString name=wxPyEmptyString, 
    PseudoMetaFile mf=None, long arrowId=-1)"""
        return _ogl.PyLineShape_AddArrow(*args, **kwargs)

    def AddArrowOrdered(*args, **kwargs):
        """AddArrowOrdered(ArrowHead arrow, PyObject referenceList, int end)"""
        return _ogl.PyLineShape_AddArrowOrdered(*args, **kwargs)

    def ClearArrow(*args, **kwargs):
        """ClearArrow(wxString name) -> bool"""
        return _ogl.PyLineShape_ClearArrow(*args, **kwargs)

    def ClearArrowsAtPosition(*args, **kwargs):
        """ClearArrowsAtPosition(int position=-1)"""
        return _ogl.PyLineShape_ClearArrowsAtPosition(*args, **kwargs)

    def DrawArrow(*args, **kwargs):
        """DrawArrow(wxDC dc, ArrowHead arrow, double xOffset, bool proportionalOffset)"""
        return _ogl.PyLineShape_DrawArrow(*args, **kwargs)

    def DeleteArrowHeadId(*args, **kwargs):
        """DeleteArrowHeadId(long arrowId) -> bool"""
        return _ogl.PyLineShape_DeleteArrowHeadId(*args, **kwargs)

    def DeleteArrowHead(*args, **kwargs):
        """DeleteArrowHead(int position, wxString name) -> bool"""
        return _ogl.PyLineShape_DeleteArrowHead(*args, **kwargs)

    def DeleteLineControlPoint(*args, **kwargs):
        """DeleteLineControlPoint() -> bool"""
        return _ogl.PyLineShape_DeleteLineControlPoint(*args, **kwargs)

    def DrawArrows(*args, **kwargs):
        """DrawArrows(wxDC dc)"""
        return _ogl.PyLineShape_DrawArrows(*args, **kwargs)

    def DrawRegion(*args, **kwargs):
        """DrawRegion(wxDC dc, ShapeRegion region, double x, double y)"""
        return _ogl.PyLineShape_DrawRegion(*args, **kwargs)

    def EraseRegion(*args, **kwargs):
        """EraseRegion(wxDC dc, ShapeRegion region, double x, double y)"""
        return _ogl.PyLineShape_EraseRegion(*args, **kwargs)

    def FindArrowHeadId(*args, **kwargs):
        """FindArrowHeadId(long arrowId) -> ArrowHead"""
        return _ogl.PyLineShape_FindArrowHeadId(*args, **kwargs)

    def FindArrowHead(*args, **kwargs):
        """FindArrowHead(int position, wxString name) -> ArrowHead"""
        return _ogl.PyLineShape_FindArrowHead(*args, **kwargs)

    def FindLineEndPoints(*args, **kwargs):
        """FindLineEndPoints(double OUTPUT, double OUTPUT, double OUTPUT, double OUTPUT)"""
        return _ogl.PyLineShape_FindLineEndPoints(*args, **kwargs)

    def FindLinePosition(*args, **kwargs):
        """FindLinePosition(double x, double y) -> int"""
        return _ogl.PyLineShape_FindLinePosition(*args, **kwargs)

    def FindMinimumWidth(*args, **kwargs):
        """FindMinimumWidth() -> double"""
        return _ogl.PyLineShape_FindMinimumWidth(*args, **kwargs)

    def FindNth(*args, **kwargs):
        """FindNth(PyShape image, int OUTPUT, int OUTPUT, bool incoming)"""
        return _ogl.PyLineShape_FindNth(*args, **kwargs)

    def GetAttachmentFrom(*args, **kwargs):
        """GetAttachmentFrom() -> int"""
        return _ogl.PyLineShape_GetAttachmentFrom(*args, **kwargs)

    def GetAttachmentTo(*args, **kwargs):
        """GetAttachmentTo() -> int"""
        return _ogl.PyLineShape_GetAttachmentTo(*args, **kwargs)

    def GetEnds(*args, **kwargs):
        """GetEnds(double OUTPUT, double OUTPUT, double OUTPUT, double OUTPUT)"""
        return _ogl.PyLineShape_GetEnds(*args, **kwargs)

    def GetFrom(*args, **kwargs):
        """GetFrom() -> PyShape"""
        return _ogl.PyLineShape_GetFrom(*args, **kwargs)

    def GetLabelPosition(*args, **kwargs):
        """GetLabelPosition(int position, double OUTPUT, double OUTPUT)"""
        return _ogl.PyLineShape_GetLabelPosition(*args, **kwargs)

    def GetNextControlPoint(*args, **kwargs):
        """GetNextControlPoint(PyShape shape) -> RealPoint"""
        return _ogl.PyLineShape_GetNextControlPoint(*args, **kwargs)

    def GetTo(*args, **kwargs):
        """GetTo() -> PyShape"""
        return _ogl.PyLineShape_GetTo(*args, **kwargs)

    def Initialise(*args, **kwargs):
        """Initialise()"""
        return _ogl.PyLineShape_Initialise(*args, **kwargs)

    def InsertLineControlPoint(*args, **kwargs):
        """InsertLineControlPoint(wxDC dc)"""
        return _ogl.PyLineShape_InsertLineControlPoint(*args, **kwargs)

    def IsEnd(*args, **kwargs):
        """IsEnd(PyShape shape) -> bool"""
        return _ogl.PyLineShape_IsEnd(*args, **kwargs)

    def IsSpline(*args, **kwargs):
        """IsSpline() -> bool"""
        return _ogl.PyLineShape_IsSpline(*args, **kwargs)

    def MakeLineControlPoints(*args, **kwargs):
        """MakeLineControlPoints(int n)"""
        return _ogl.PyLineShape_MakeLineControlPoints(*args, **kwargs)

    def GetLineControlPoints(*args, **kwargs):
        """GetLineControlPoints() -> PyObject"""
        return _ogl.PyLineShape_GetLineControlPoints(*args, **kwargs)

    def SetAttachmentFrom(*args, **kwargs):
        """SetAttachmentFrom(int fromAttach)"""
        return _ogl.PyLineShape_SetAttachmentFrom(*args, **kwargs)

    def SetAttachments(*args, **kwargs):
        """SetAttachments(int fromAttach, int toAttach)"""
        return _ogl.PyLineShape_SetAttachments(*args, **kwargs)

    def SetAttachmentTo(*args, **kwargs):
        """SetAttachmentTo(int toAttach)"""
        return _ogl.PyLineShape_SetAttachmentTo(*args, **kwargs)

    def SetEnds(*args, **kwargs):
        """SetEnds(double x1, double y1, double x2, double y2)"""
        return _ogl.PyLineShape_SetEnds(*args, **kwargs)

    def SetFrom(*args, **kwargs):
        """SetFrom(PyShape object)"""
        return _ogl.PyLineShape_SetFrom(*args, **kwargs)

    def SetIgnoreOffsets(*args, **kwargs):
        """SetIgnoreOffsets(bool ignore)"""
        return _ogl.PyLineShape_SetIgnoreOffsets(*args, **kwargs)

    def SetSpline(*args, **kwargs):
        """SetSpline(bool spline)"""
        return _ogl.PyLineShape_SetSpline(*args, **kwargs)

    def SetTo(*args, **kwargs):
        """SetTo(PyShape object)"""
        return _ogl.PyLineShape_SetTo(*args, **kwargs)

    def Straighten(*args, **kwargs):
        """Straighten(wxDC dc=None)"""
        return _ogl.PyLineShape_Straighten(*args, **kwargs)

    def Unlink(*args, **kwargs):
        """Unlink()"""
        return _ogl.PyLineShape_Unlink(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyLineShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyLineShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyLineShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyLineShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyLineShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyLineShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyLineShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyLineShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyLineShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyLineShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyLineShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyLineShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyLineShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyLineShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyLineShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyLineShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyLineShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyLineShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyLineShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyLineShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyLineShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyLineShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyLineShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyLineShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyLineShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyLineShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyLineShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyLineShape_base_OnEndSize(*args, **kwargs)


class PyLineShapePtr(PyLineShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyLineShape
_ogl.PyLineShape_swigregister(PyLineShapePtr)

class PyPolygonShape(PyShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyPolygonShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> PyPolygonShape"""
        newobj = _ogl.new_PyPolygonShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyPolygonShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyPolygonShape__setCallbackInfo(*args, **kwargs)

    def Create(*args, **kwargs):
        """Create(PyObject points) -> PyObject"""
        return _ogl.PyPolygonShape_Create(*args, **kwargs)

    def AddPolygonPoint(*args, **kwargs):
        """AddPolygonPoint(int pos=0)"""
        return _ogl.PyPolygonShape_AddPolygonPoint(*args, **kwargs)

    def CalculatePolygonCentre(*args, **kwargs):
        """CalculatePolygonCentre()"""
        return _ogl.PyPolygonShape_CalculatePolygonCentre(*args, **kwargs)

    def DeletePolygonPoint(*args, **kwargs):
        """DeletePolygonPoint(int pos=0)"""
        return _ogl.PyPolygonShape_DeletePolygonPoint(*args, **kwargs)

    def GetPoints(*args, **kwargs):
        """GetPoints() -> PyObject"""
        return _ogl.PyPolygonShape_GetPoints(*args, **kwargs)

    def UpdateOriginalPoints(*args, **kwargs):
        """UpdateOriginalPoints()"""
        return _ogl.PyPolygonShape_UpdateOriginalPoints(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyPolygonShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyPolygonShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyPolygonShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyPolygonShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyPolygonShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyPolygonShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyPolygonShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyPolygonShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyPolygonShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyPolygonShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyPolygonShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyPolygonShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyPolygonShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyPolygonShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyPolygonShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyPolygonShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyPolygonShape_base_OnEndSize(*args, **kwargs)


class PyPolygonShapePtr(PyPolygonShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyPolygonShape
_ogl.PyPolygonShape_swigregister(PyPolygonShapePtr)

class PyTextShape(PyRectangleShape):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyTextShape instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(double width=0.0, double height=0.0) -> PyTextShape"""
        newobj = _ogl.new_PyTextShape(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setOORandCallbackInfo(PyTextShape)

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyTextShape__setCallbackInfo(*args, **kwargs)

    def base_OnDelete(*args, **kwargs):
        """base_OnDelete()"""
        return _ogl.PyTextShape_base_OnDelete(*args, **kwargs)

    def base_OnDraw(*args, **kwargs):
        """base_OnDraw(wxDC dc)"""
        return _ogl.PyTextShape_base_OnDraw(*args, **kwargs)

    def base_OnDrawContents(*args, **kwargs):
        """base_OnDrawContents(wxDC dc)"""
        return _ogl.PyTextShape_base_OnDrawContents(*args, **kwargs)

    def base_OnDrawBranches(*args, **kwargs):
        """base_OnDrawBranches(wxDC dc, bool erase=FALSE)"""
        return _ogl.PyTextShape_base_OnDrawBranches(*args, **kwargs)

    def base_OnMoveLinks(*args, **kwargs):
        """base_OnMoveLinks(wxDC dc)"""
        return _ogl.PyTextShape_base_OnMoveLinks(*args, **kwargs)

    def base_OnErase(*args, **kwargs):
        """base_OnErase(wxDC dc)"""
        return _ogl.PyTextShape_base_OnErase(*args, **kwargs)

    def base_OnEraseContents(*args, **kwargs):
        """base_OnEraseContents(wxDC dc)"""
        return _ogl.PyTextShape_base_OnEraseContents(*args, **kwargs)

    def base_OnHighlight(*args, **kwargs):
        """base_OnHighlight(wxDC dc)"""
        return _ogl.PyTextShape_base_OnHighlight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyTextShape_base_OnLeftClick(*args, **kwargs)

    def base_OnLeftDoubleClick(*args, **kwargs):
        """base_OnLeftDoubleClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyTextShape_base_OnLeftDoubleClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyTextShape_base_OnRightClick(*args, **kwargs)

    def base_OnSize(*args, **kwargs):
        """base_OnSize(double x, double y)"""
        return _ogl.PyTextShape_base_OnSize(*args, **kwargs)

    def base_OnMovePre(*args, **kwargs):
        """base_OnMovePre(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True) -> bool"""
        return _ogl.PyTextShape_base_OnMovePre(*args, **kwargs)

    def base_OnMovePost(*args, **kwargs):
        """base_OnMovePost(wxDC dc, double x, double y, double old_x, double old_y, 
    bool display=True)"""
        return _ogl.PyTextShape_base_OnMovePost(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyTextShape_base_OnDragLeft(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyTextShape_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyTextShape_base_OnEndDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyTextShape_base_OnDragRight(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyTextShape_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0, int attachment=0)"""
        return _ogl.PyTextShape_base_OnEndDragRight(*args, **kwargs)

    def base_OnDrawOutline(*args, **kwargs):
        """base_OnDrawOutline(wxDC dc, double x, double y, double w, double h)"""
        return _ogl.PyTextShape_base_OnDrawOutline(*args, **kwargs)

    def base_OnDrawControlPoints(*args, **kwargs):
        """base_OnDrawControlPoints(wxDC dc)"""
        return _ogl.PyTextShape_base_OnDrawControlPoints(*args, **kwargs)

    def base_OnEraseControlPoints(*args, **kwargs):
        """base_OnEraseControlPoints(wxDC dc)"""
        return _ogl.PyTextShape_base_OnEraseControlPoints(*args, **kwargs)

    def base_OnMoveLink(*args, **kwargs):
        """base_OnMoveLink(wxDC dc, bool moveControlPoints=True)"""
        return _ogl.PyTextShape_base_OnMoveLink(*args, **kwargs)

    def base_OnSizingDragLeft(*args, **kwargs):
        """base_OnSizingDragLeft(PyControlPoint pt, bool draw, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyTextShape_base_OnSizingDragLeft(*args, **kwargs)

    def base_OnSizingBeginDragLeft(*args, **kwargs):
        """base_OnSizingBeginDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyTextShape_base_OnSizingBeginDragLeft(*args, **kwargs)

    def base_OnSizingEndDragLeft(*args, **kwargs):
        """base_OnSizingEndDragLeft(PyControlPoint pt, double x, double y, int keys=0, 
    int attachment=0)"""
        return _ogl.PyTextShape_base_OnSizingEndDragLeft(*args, **kwargs)

    def base_OnBeginSize(*args, **kwargs):
        """base_OnBeginSize(double w, double h)"""
        return _ogl.PyTextShape_base_OnBeginSize(*args, **kwargs)

    def base_OnEndSize(*args, **kwargs):
        """base_OnEndSize(double w, double h)"""
        return _ogl.PyTextShape_base_OnEndSize(*args, **kwargs)


class PyTextShapePtr(PyTextShape):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyTextShape
_ogl.PyTextShape_swigregister(PyTextShapePtr)

class Diagram(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDiagram instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> Diagram"""
        newobj = _ogl.new_Diagram(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def AddShape(*args, **kwargs):
        """AddShape(PyShape shape, PyShape addAfter=None)"""
        return _ogl.Diagram_AddShape(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear(wxDC dc)"""
        return _ogl.Diagram_Clear(*args, **kwargs)

    def DeleteAllShapes(*args, **kwargs):
        """DeleteAllShapes()"""
        return _ogl.Diagram_DeleteAllShapes(*args, **kwargs)

    def DrawOutline(*args, **kwargs):
        """DrawOutline(wxDC dc, double x1, double y1, double x2, double y2)"""
        return _ogl.Diagram_DrawOutline(*args, **kwargs)

    def FindShape(*args, **kwargs):
        """FindShape(long id) -> PyShape"""
        return _ogl.Diagram_FindShape(*args, **kwargs)

    def GetCanvas(*args, **kwargs):
        """GetCanvas() -> PyShapeCanvas"""
        return _ogl.Diagram_GetCanvas(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount() -> int"""
        return _ogl.Diagram_GetCount(*args, **kwargs)

    def GetGridSpacing(*args, **kwargs):
        """GetGridSpacing() -> double"""
        return _ogl.Diagram_GetGridSpacing(*args, **kwargs)

    def GetMouseTolerance(*args, **kwargs):
        """GetMouseTolerance() -> int"""
        return _ogl.Diagram_GetMouseTolerance(*args, **kwargs)

    def GetShapeList(*args, **kwargs):
        """GetShapeList() -> PyObject"""
        return _ogl.Diagram_GetShapeList(*args, **kwargs)

    def GetQuickEditMode(*args, **kwargs):
        """GetQuickEditMode() -> bool"""
        return _ogl.Diagram_GetQuickEditMode(*args, **kwargs)

    def GetSnapToGrid(*args, **kwargs):
        """GetSnapToGrid() -> bool"""
        return _ogl.Diagram_GetSnapToGrid(*args, **kwargs)

    def InsertShape(*args, **kwargs):
        """InsertShape(PyShape shape)"""
        return _ogl.Diagram_InsertShape(*args, **kwargs)

    def RecentreAll(*args, **kwargs):
        """RecentreAll(wxDC dc)"""
        return _ogl.Diagram_RecentreAll(*args, **kwargs)

    def Redraw(*args, **kwargs):
        """Redraw(wxDC dc)"""
        return _ogl.Diagram_Redraw(*args, **kwargs)

    def RemoveAllShapes(*args, **kwargs):
        """RemoveAllShapes()"""
        return _ogl.Diagram_RemoveAllShapes(*args, **kwargs)

    def RemoveShape(*args, **kwargs):
        """RemoveShape(PyShape shape)"""
        return _ogl.Diagram_RemoveShape(*args, **kwargs)

    def SetCanvas(*args, **kwargs):
        """SetCanvas(PyShapeCanvas canvas)"""
        return _ogl.Diagram_SetCanvas(*args, **kwargs)

    def SetGridSpacing(*args, **kwargs):
        """SetGridSpacing(double spacing)"""
        return _ogl.Diagram_SetGridSpacing(*args, **kwargs)

    def SetMouseTolerance(*args, **kwargs):
        """SetMouseTolerance(int tolerance)"""
        return _ogl.Diagram_SetMouseTolerance(*args, **kwargs)

    def SetQuickEditMode(*args, **kwargs):
        """SetQuickEditMode(bool mode)"""
        return _ogl.Diagram_SetQuickEditMode(*args, **kwargs)

    def SetSnapToGrid(*args, **kwargs):
        """SetSnapToGrid(bool snap)"""
        return _ogl.Diagram_SetSnapToGrid(*args, **kwargs)

    def ShowAll(*args, **kwargs):
        """ShowAll(bool show)"""
        return _ogl.Diagram_ShowAll(*args, **kwargs)

    def Snap(*args, **kwargs):
        """Snap(double INOUT, double INOUT)"""
        return _ogl.Diagram_Snap(*args, **kwargs)


class DiagramPtr(Diagram):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Diagram
_ogl.Diagram_swigregister(DiagramPtr)

class PyShapeCanvas(windows.ScrolledWindow):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyShapeCanvas instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window parent=None, int id=-1, Point pos=DefaultPosition, 
    Size size=DefaultSize, long style=BORDER, 
    wxString name=wxPyShapeCanvasNameStr) -> PyShapeCanvas"""
        newobj = _ogl.new_PyShapeCanvas(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class)"""
        return _ogl.PyShapeCanvas__setCallbackInfo(*args, **kwargs)

    def _setOORandCallbackInfo(self, _class):
        self._setOORInfo(self)
        self._setCallbackInfo(self, _class)

    def AddShape(*args, **kwargs):
        """AddShape(PyShape shape, PyShape addAfter=None)"""
        return _ogl.PyShapeCanvas_AddShape(*args, **kwargs)

    def FindShape(*args, **kwargs):
        """FindShape(double x1, double y, int OUTPUT, wxClassInfo info=None, 
    PyShape notImage=None) -> PyShape"""
        return _ogl.PyShapeCanvas_FindShape(*args, **kwargs)

    def FindFirstSensitiveShape(*args, **kwargs):
        """FindFirstSensitiveShape(double x1, double y, int OUTPUT, int op) -> PyShape"""
        return _ogl.PyShapeCanvas_FindFirstSensitiveShape(*args, **kwargs)

    def GetDiagram(*args, **kwargs):
        """GetDiagram() -> Diagram"""
        return _ogl.PyShapeCanvas_GetDiagram(*args, **kwargs)

    def GetQuickEditMode(*args, **kwargs):
        """GetQuickEditMode() -> bool"""
        return _ogl.PyShapeCanvas_GetQuickEditMode(*args, **kwargs)

    def InsertShape(*args, **kwargs):
        """InsertShape(PyShape shape)"""
        return _ogl.PyShapeCanvas_InsertShape(*args, **kwargs)

    def base_OnBeginDragLeft(*args, **kwargs):
        """base_OnBeginDragLeft(double x, double y, int keys=0)"""
        return _ogl.PyShapeCanvas_base_OnBeginDragLeft(*args, **kwargs)

    def base_OnBeginDragRight(*args, **kwargs):
        """base_OnBeginDragRight(double x, double y, int keys=0)"""
        return _ogl.PyShapeCanvas_base_OnBeginDragRight(*args, **kwargs)

    def base_OnEndDragLeft(*args, **kwargs):
        """base_OnEndDragLeft(double x, double y, int keys=0)"""
        return _ogl.PyShapeCanvas_base_OnEndDragLeft(*args, **kwargs)

    def base_OnEndDragRight(*args, **kwargs):
        """base_OnEndDragRight(double x, double y, int keys=0)"""
        return _ogl.PyShapeCanvas_base_OnEndDragRight(*args, **kwargs)

    def base_OnDragLeft(*args, **kwargs):
        """base_OnDragLeft(bool draw, double x, double y, int keys=0)"""
        return _ogl.PyShapeCanvas_base_OnDragLeft(*args, **kwargs)

    def base_OnDragRight(*args, **kwargs):
        """base_OnDragRight(bool draw, double x, double y, int keys=0)"""
        return _ogl.PyShapeCanvas_base_OnDragRight(*args, **kwargs)

    def base_OnLeftClick(*args, **kwargs):
        """base_OnLeftClick(double x, double y, int keys=0)"""
        return _ogl.PyShapeCanvas_base_OnLeftClick(*args, **kwargs)

    def base_OnRightClick(*args, **kwargs):
        """base_OnRightClick(double x, double y, int keys=0)"""
        return _ogl.PyShapeCanvas_base_OnRightClick(*args, **kwargs)

    def Redraw(*args, **kwargs):
        """Redraw(wxDC dc)"""
        return _ogl.PyShapeCanvas_Redraw(*args, **kwargs)

    def RemoveShape(*args, **kwargs):
        """RemoveShape(PyShape shape)"""
        return _ogl.PyShapeCanvas_RemoveShape(*args, **kwargs)

    def SetDiagram(*args, **kwargs):
        """SetDiagram(Diagram diagram)"""
        return _ogl.PyShapeCanvas_SetDiagram(*args, **kwargs)

    def Snap(*args, **kwargs):
        """Snap(double INOUT, double INOUT)"""
        return _ogl.PyShapeCanvas_Snap(*args, **kwargs)

    def GetShapeList(self):
        return self.GetDiagram().GetShapeList()


class PyShapeCanvasPtr(PyShapeCanvas):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyShapeCanvas
_ogl.PyShapeCanvas_swigregister(PyShapeCanvasPtr)

# Aliases    
ShapeCanvas =       PyShapeCanvas
ShapeEvtHandler =   PyShapeEvtHandler
Shape =             PyShape
RectangleShape =    PyRectangleShape
BitmapShape =       PyBitmapShape
DrawnShape =        PyDrawnShape
CompositeShape =    PyCompositeShape
DividedShape =      PyDividedShape
DivisionShape =     PyDivisionShape
EllipseShape =      PyEllipseShape
CircleShape =       PyCircleShape
LineShape =         PyLineShape
PolygonShape =      PyPolygonShape
TextShape =         PyTextShape
ControlPoint =      PyControlPoint

KEY_SHIFT = _ogl.KEY_SHIFT
KEY_CTRL = _ogl.KEY_CTRL
ARROW_NONE = _ogl.ARROW_NONE
ARROW_END = _ogl.ARROW_END
ARROW_BOTH = _ogl.ARROW_BOTH
ARROW_MIDDLE = _ogl.ARROW_MIDDLE
ARROW_START = _ogl.ARROW_START
ARROW_HOLLOW_CIRCLE = _ogl.ARROW_HOLLOW_CIRCLE
ARROW_FILLED_CIRCLE = _ogl.ARROW_FILLED_CIRCLE
ARROW_ARROW = _ogl.ARROW_ARROW
ARROW_SINGLE_OBLIQUE = _ogl.ARROW_SINGLE_OBLIQUE
ARROW_DOUBLE_OBLIQUE = _ogl.ARROW_DOUBLE_OBLIQUE
ARROW_METAFILE = _ogl.ARROW_METAFILE
ARROW_POSITION_END = _ogl.ARROW_POSITION_END
ARROW_POSITION_START = _ogl.ARROW_POSITION_START
CONTROL_POINT_VERTICAL = _ogl.CONTROL_POINT_VERTICAL
CONTROL_POINT_HORIZONTAL = _ogl.CONTROL_POINT_HORIZONTAL
CONTROL_POINT_DIAGONAL = _ogl.CONTROL_POINT_DIAGONAL
CONTROL_POINT_ENDPOINT_TO = _ogl.CONTROL_POINT_ENDPOINT_TO
CONTROL_POINT_ENDPOINT_FROM = _ogl.CONTROL_POINT_ENDPOINT_FROM
CONTROL_POINT_LINE = _ogl.CONTROL_POINT_LINE
FORMAT_NONE = _ogl.FORMAT_NONE
FORMAT_CENTRE_HORIZ = _ogl.FORMAT_CENTRE_HORIZ
FORMAT_CENTRE_VERT = _ogl.FORMAT_CENTRE_VERT
FORMAT_SIZE_TO_CONTENTS = _ogl.FORMAT_SIZE_TO_CONTENTS
LINE_ALIGNMENT_HORIZ = _ogl.LINE_ALIGNMENT_HORIZ
LINE_ALIGNMENT_VERT = _ogl.LINE_ALIGNMENT_VERT
LINE_ALIGNMENT_TO_NEXT_HANDLE = _ogl.LINE_ALIGNMENT_TO_NEXT_HANDLE
LINE_ALIGNMENT_NONE = _ogl.LINE_ALIGNMENT_NONE
SHADOW_NONE = _ogl.SHADOW_NONE
SHADOW_LEFT = _ogl.SHADOW_LEFT
SHADOW_RIGHT = _ogl.SHADOW_RIGHT
OP_CLICK_LEFT = _ogl.OP_CLICK_LEFT
OP_CLICK_RIGHT = _ogl.OP_CLICK_RIGHT
OP_DRAG_LEFT = _ogl.OP_DRAG_LEFT
OP_DRAG_RIGHT = _ogl.OP_DRAG_RIGHT
OP_ALL = _ogl.OP_ALL
ATTACHMENT_MODE_NONE = _ogl.ATTACHMENT_MODE_NONE
ATTACHMENT_MODE_EDGE = _ogl.ATTACHMENT_MODE_EDGE
ATTACHMENT_MODE_BRANCHING = _ogl.ATTACHMENT_MODE_BRANCHING
BRANCHING_ATTACHMENT_NORMAL = _ogl.BRANCHING_ATTACHMENT_NORMAL
BRANCHING_ATTACHMENT_BLOB = _ogl.BRANCHING_ATTACHMENT_BLOB
gyCONSTRAINT_CENTRED_VERTICALLY = _ogl.gyCONSTRAINT_CENTRED_VERTICALLY
gyCONSTRAINT_CENTRED_HORIZONTALLY = _ogl.gyCONSTRAINT_CENTRED_HORIZONTALLY
gyCONSTRAINT_CENTRED_BOTH = _ogl.gyCONSTRAINT_CENTRED_BOTH
gyCONSTRAINT_LEFT_OF = _ogl.gyCONSTRAINT_LEFT_OF
gyCONSTRAINT_RIGHT_OF = _ogl.gyCONSTRAINT_RIGHT_OF
gyCONSTRAINT_ABOVE = _ogl.gyCONSTRAINT_ABOVE
gyCONSTRAINT_BELOW = _ogl.gyCONSTRAINT_BELOW
gyCONSTRAINT_ALIGNED_TOP = _ogl.gyCONSTRAINT_ALIGNED_TOP
gyCONSTRAINT_ALIGNED_BOTTOM = _ogl.gyCONSTRAINT_ALIGNED_BOTTOM
gyCONSTRAINT_ALIGNED_LEFT = _ogl.gyCONSTRAINT_ALIGNED_LEFT
gyCONSTRAINT_ALIGNED_RIGHT = _ogl.gyCONSTRAINT_ALIGNED_RIGHT
gyCONSTRAINT_MIDALIGNED_TOP = _ogl.gyCONSTRAINT_MIDALIGNED_TOP
gyCONSTRAINT_MIDALIGNED_BOTTOM = _ogl.gyCONSTRAINT_MIDALIGNED_BOTTOM
gyCONSTRAINT_MIDALIGNED_LEFT = _ogl.gyCONSTRAINT_MIDALIGNED_LEFT
gyCONSTRAINT_MIDALIGNED_RIGHT = _ogl.gyCONSTRAINT_MIDALIGNED_RIGHT
DIVISION_SIDE_NONE = _ogl.DIVISION_SIDE_NONE
DIVISION_SIDE_LEFT = _ogl.DIVISION_SIDE_LEFT
DIVISION_SIDE_TOP = _ogl.DIVISION_SIDE_TOP
DIVISION_SIDE_RIGHT = _ogl.DIVISION_SIDE_RIGHT
DIVISION_SIDE_BOTTOM = _ogl.DIVISION_SIDE_BOTTOM

def OGLInitialize(*args, **kwargs):
    """OGLInitialize()"""
    return _ogl.OGLInitialize(*args, **kwargs)

def OGLCleanUp(*args, **kwargs):
    """OGLCleanUp()"""
    return _ogl.OGLCleanUp(*args, **kwargs)

