
# aliases
wxShapeCanvas =       wxPyShapeCanvas
wxShapeEvtHandler =   wxPyShapeEvtHandler
wxShape =             wxPyShape
wxRectangleShape =    wxPyRectangleShape
wxBitmapShape =       wxPyBitmapShape
wxDrawnShape =        wxPyDrawnShape
wxCompositeShape =    wxPyCompositeShape
wxDividedShape =      wxPyDividedShape
wxDivisionShape =     wxPyDivisionShape
wxEllipseShape =      wxPyEllipseShape
wxCircleShape =       wxPyCircleShape
wxLineShape =         wxPyLineShape
wxPolygonShape =      wxPyPolygonShape
wxTextShape =         wxPyTextShape
wxControlPoint =      wxPyControlPoint

# Stuff these names into the wx namespace so wxPyConstructObject can find them
import wx
wx.wxArrowHeadPtr         = wxArrowHeadPtr
wx.wxControlPointPtr      = wxPyControlPointPtr
wx.wxDiagramPtr           = wxDiagramPtr
wx.wxOGLConstraintPtr     = wxOGLConstraintPtr
wx.wxPseudoMetaFilePtr    = wxPseudoMetaFile
wx.wxPyBitmapShapePtr     = wxPyBitmapShapePtr
wx.wxPyCircleShapePtr     = wxPyCircleShapePtr
wx.wxPyCompositeShapePtr  = wxPyCompositeShapePtr
wx.wxPyControlPointPtr    = wxPyControlPointPtr
wx.wxPyDividedShapePtr    = wxPyDividedShapePtr
wx.wxPyDivisionShapePtr   = wxPyDivisionShapePtr
wx.wxPyDrawnShapePtr      = wxPyDrawnShapePtr
wx.wxPyEllipseShapePtr    = wxPyEllipseShapePtr
wx.wxPyLineShapePtr       = wxPyLineShapePtr
wx.wxPyPolygonShapePtr    = wxPyPolygonShapePtr
wx.wxPyRectangleShapePtr  = wxPyRectangleShapePtr
wx.wxPyShapeCanvasPtr     = wxPyShapeCanvasPtr
wx.wxPyShapeEvtHandlerPtr = wxPyShapeEvtHandlerPtr
wx.wxPyShapePtr           = wxPyShapePtr
wx.wxPyTextShapePtr       = wxPyTextShapePtr
wx.wxShapeRegionPtr       = wxShapeRegionPtr
