# This file was created automatically by SWIG.
import gdic

from misc import *
class wxBitmapPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxBitmap(self)
    def GetPalette(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetPalette,(self,) + _args, _kwargs)
        if val: val = wxPalettePtr(val) 
        return val
    def GetMask(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetMask,(self,) + _args, _kwargs)
        if val: val = wxMaskPtr(val) 
        return val
    def LoadFile(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_LoadFile,(self,) + _args, _kwargs)
        return val
    def SaveFile(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_SaveFile,(self,) + _args, _kwargs)
        return val
    def SetMask(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_SetMask,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_Ok,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetWidth,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetHeight,(self,) + _args, _kwargs)
        return val
    def GetDepth(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_GetDepth,(self,) + _args, _kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_SetWidth,(self,) + _args, _kwargs)
        return val
    def SetHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_SetHeight,(self,) + _args, _kwargs)
        return val
    def SetDepth(self, *_args, **_kwargs):
        val = apply(gdic.wxBitmap_SetDepth,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxBitmap instance at %s>" % (self.this,)
class wxBitmap(wxBitmapPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxBitmap,_args,_kwargs)
        self.thisown = 1




class wxMaskPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxMask instance at %s>" % (self.this,)
class wxMask(wxMaskPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxMask,_args,_kwargs)
        self.thisown = 1




class wxIconPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxIcon(self)
    def LoadFile(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_LoadFile,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_Ok,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_GetWidth,(self,) + _args, _kwargs)
        return val
    def GetHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_GetHeight,(self,) + _args, _kwargs)
        return val
    def GetDepth(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_GetDepth,(self,) + _args, _kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_SetWidth,(self,) + _args, _kwargs)
        return val
    def SetHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_SetHeight,(self,) + _args, _kwargs)
        return val
    def SetDepth(self, *_args, **_kwargs):
        val = apply(gdic.wxIcon_SetDepth,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxIcon instance at %s>" % (self.this,)
class wxIcon(wxIconPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxIcon,_args,_kwargs)
        self.thisown = 1




class wxCursorPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxCursor(self)
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxCursor_Ok,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxCursor instance at %s>" % (self.this,)
class wxCursor(wxCursorPtr):
    def __init__(self,this):
        self.this = this




class wxFontPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_Ok,(self,) + _args, _kwargs)
        return val
    def GetFaceName(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetFaceName,(self,) + _args, _kwargs)
        return val
    def GetFamily(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetFamily,(self,) + _args, _kwargs)
        return val
    def GetPointSize(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetPointSize,(self,) + _args, _kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetStyle,(self,) + _args, _kwargs)
        return val
    def GetUnderlined(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetUnderlined,(self,) + _args, _kwargs)
        return val
    def GetWeight(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetWeight,(self,) + _args, _kwargs)
        return val
    def GetEncoding(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetEncoding,(self,) + _args, _kwargs)
        return val
    def SetFaceName(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetFaceName,(self,) + _args, _kwargs)
        return val
    def SetFamily(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetFamily,(self,) + _args, _kwargs)
        return val
    def SetPointSize(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetPointSize,(self,) + _args, _kwargs)
        return val
    def SetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetStyle,(self,) + _args, _kwargs)
        return val
    def SetUnderlined(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetUnderlined,(self,) + _args, _kwargs)
        return val
    def SetWeight(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetWeight,(self,) + _args, _kwargs)
        return val
    def SetEncoding(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_SetEncoding,(self,) + _args, _kwargs)
        return val
    def GetFamilyString(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetFamilyString,(self,) + _args, _kwargs)
        return val
    def GetStyleString(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetStyleString,(self,) + _args, _kwargs)
        return val
    def GetWeightString(self, *_args, **_kwargs):
        val = apply(gdic.wxFont_GetWeightString,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxFont instance at %s>" % (self.this,)
class wxFont(wxFontPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxFont,_args,_kwargs)
        self.thisown = 1




class wxColourPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxColour(self)
    def Red(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Red,(self,) + _args, _kwargs)
        return val
    def Green(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Green,(self,) + _args, _kwargs)
        return val
    def Blue(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Blue,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Ok,(self,) + _args, _kwargs)
        return val
    def Set(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Set,(self,) + _args, _kwargs)
        return val
    def Get(self, *_args, **_kwargs):
        val = apply(gdic.wxColour_Get,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxColour instance at %s>" % (self.this,)
    asTuple = Get
    def __str__(self): return str(self.asTuple())
    def __repr__(self): return str(self.asTuple())
class wxColour(wxColourPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxColour,_args,_kwargs)
        self.thisown = 1




class wxPenPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetCap(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetCap,(self,) + _args, _kwargs)
        return val
    def GetColour(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetJoin(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetJoin,(self,) + _args, _kwargs)
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetStyle,(self,) + _args, _kwargs)
        return val
    def GetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetWidth,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_Ok,(self,) + _args, _kwargs)
        return val
    def SetCap(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetCap,(self,) + _args, _kwargs)
        return val
    def SetColour(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetColour,(self,) + _args, _kwargs)
        return val
    def SetJoin(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetJoin,(self,) + _args, _kwargs)
        return val
    def SetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetStyle,(self,) + _args, _kwargs)
        return val
    def SetWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetWidth,(self,) + _args, _kwargs)
        return val
    def GetDashes(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_GetDashes,(self,) + _args, _kwargs)
        return val
    def SetDashes(self, *_args, **_kwargs):
        val = apply(gdic.wxPen_SetDashes,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPen instance at %s>" % (self.this,)
class wxPen(wxPenPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxPen,_args,_kwargs)
        self.thisown = 1




class wxBrushPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetColour(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_GetColour,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetStipple(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_GetStipple,(self,) + _args, _kwargs)
        if val: val = wxBitmapPtr(val) 
        return val
    def GetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_GetStyle,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_Ok,(self,) + _args, _kwargs)
        return val
    def SetColour(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_SetColour,(self,) + _args, _kwargs)
        return val
    def SetStipple(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_SetStipple,(self,) + _args, _kwargs)
        return val
    def SetStyle(self, *_args, **_kwargs):
        val = apply(gdic.wxBrush_SetStyle,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxBrush instance at %s>" % (self.this,)
class wxBrush(wxBrushPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxBrush,_args,_kwargs)
        self.thisown = 1




class wxDCPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxDC(self)
    def BeginDrawing(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_BeginDrawing,(self,) + _args, _kwargs)
        return val
    def Blit(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_Blit,(self,) + _args, _kwargs)
        return val
    def Clear(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_Clear,(self,) + _args, _kwargs)
        return val
    def CrossHair(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_CrossHair,(self,) + _args, _kwargs)
        return val
    def DestroyClippingRegion(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DestroyClippingRegion,(self,) + _args, _kwargs)
        return val
    def DeviceToLogicalX(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DeviceToLogicalX,(self,) + _args, _kwargs)
        return val
    def DeviceToLogicalXRel(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DeviceToLogicalXRel,(self,) + _args, _kwargs)
        return val
    def DeviceToLogicalY(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DeviceToLogicalY,(self,) + _args, _kwargs)
        return val
    def DeviceToLogicalYRel(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DeviceToLogicalYRel,(self,) + _args, _kwargs)
        return val
    def DrawArc(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawArc,(self,) + _args, _kwargs)
        return val
    def DrawCircle(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawCircle,(self,) + _args, _kwargs)
        return val
    def DrawEllipse(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawEllipse,(self,) + _args, _kwargs)
        return val
    def DrawEllipticArc(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawEllipticArc,(self,) + _args, _kwargs)
        return val
    def DrawIcon(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawIcon,(self,) + _args, _kwargs)
        return val
    def DrawLine(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawLine,(self,) + _args, _kwargs)
        return val
    def DrawLines(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawLines,(self,) + _args, _kwargs)
        return val
    def DrawPolygon(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawPolygon,(self,) + _args, _kwargs)
        return val
    def DrawPoint(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawPoint,(self,) + _args, _kwargs)
        return val
    def DrawRectangle(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawRectangle,(self,) + _args, _kwargs)
        return val
    def DrawRotatedText(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawRotatedText,(self,) + _args, _kwargs)
        return val
    def DrawRoundedRectangle(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawRoundedRectangle,(self,) + _args, _kwargs)
        return val
    def DrawSpline(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawSpline,(self,) + _args, _kwargs)
        return val
    def DrawText(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawText,(self,) + _args, _kwargs)
        return val
    def EndDoc(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_EndDoc,(self,) + _args, _kwargs)
        return val
    def EndDrawing(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_EndDrawing,(self,) + _args, _kwargs)
        return val
    def EndPage(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_EndPage,(self,) + _args, _kwargs)
        return val
    def FloodFill(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_FloodFill,(self,) + _args, _kwargs)
        return val
    def GetBackground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetBackground,(self,) + _args, _kwargs)
        if val: val = wxBrushPtr(val) 
        return val
    def GetBrush(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetBrush,(self,) + _args, _kwargs)
        if val: val = wxBrushPtr(val) 
        return val
    def GetCharHeight(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetCharHeight,(self,) + _args, _kwargs)
        return val
    def GetCharWidth(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetCharWidth,(self,) + _args, _kwargs)
        return val
    def GetClippingBox(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetClippingBox,(self,) + _args, _kwargs)
        return val
    def GetFont(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetFont,(self,) + _args, _kwargs)
        if val: val = wxFontPtr(val) 
        return val
    def GetLogicalFunction(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetLogicalFunction,(self,) + _args, _kwargs)
        return val
    def GetMapMode(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetMapMode,(self,) + _args, _kwargs)
        return val
    def GetOptimization(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetOptimization,(self,) + _args, _kwargs)
        return val
    def GetPen(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetPen,(self,) + _args, _kwargs)
        if val: val = wxPenPtr(val) 
        return val
    def GetPixel(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetPixel,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) ; val.thisown = 1
        return val
    def GetSizeTuple(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetSizeTuple,(self,) + _args, _kwargs)
        return val
    def GetSize(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetSize,(self,) + _args, _kwargs)
        if val: val = wxSizePtr(val) ; val.thisown = 1
        return val
    def GetTextBackground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetTextBackground,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def GetTextExtent(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetTextExtent,(self,) + _args, _kwargs)
        return val
    def GetFullTextExtent(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetFullTextExtent,(self,) + _args, _kwargs)
        return val
    def GetTextForeground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_GetTextForeground,(self,) + _args, _kwargs)
        if val: val = wxColourPtr(val) 
        return val
    def LogicalToDeviceX(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_LogicalToDeviceX,(self,) + _args, _kwargs)
        return val
    def LogicalToDeviceXRel(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_LogicalToDeviceXRel,(self,) + _args, _kwargs)
        return val
    def LogicalToDeviceY(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_LogicalToDeviceY,(self,) + _args, _kwargs)
        return val
    def LogicalToDeviceYRel(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_LogicalToDeviceYRel,(self,) + _args, _kwargs)
        return val
    def MaxX(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_MaxX,(self,) + _args, _kwargs)
        return val
    def MaxY(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_MaxY,(self,) + _args, _kwargs)
        return val
    def MinX(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_MinX,(self,) + _args, _kwargs)
        return val
    def MinY(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_MinY,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_Ok,(self,) + _args, _kwargs)
        return val
    def SetDeviceOrigin(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetDeviceOrigin,(self,) + _args, _kwargs)
        return val
    def SetBackground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetBackground,(self,) + _args, _kwargs)
        return val
    def SetBackgroundMode(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetBackgroundMode,(self,) + _args, _kwargs)
        return val
    def SetClippingRegion(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetClippingRegion,(self,) + _args, _kwargs)
        return val
    def SetPalette(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetPalette,(self,) + _args, _kwargs)
        return val
    def SetBrush(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetBrush,(self,) + _args, _kwargs)
        return val
    def SetFont(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetFont,(self,) + _args, _kwargs)
        return val
    def SetLogicalFunction(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetLogicalFunction,(self,) + _args, _kwargs)
        return val
    def SetMapMode(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetMapMode,(self,) + _args, _kwargs)
        return val
    def SetOptimization(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetOptimization,(self,) + _args, _kwargs)
        return val
    def SetPen(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetPen,(self,) + _args, _kwargs)
        return val
    def SetTextBackground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetTextBackground,(self,) + _args, _kwargs)
        return val
    def SetTextForeground(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetTextForeground,(self,) + _args, _kwargs)
        return val
    def SetUserScale(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_SetUserScale,(self,) + _args, _kwargs)
        return val
    def StartDoc(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_StartDoc,(self,) + _args, _kwargs)
        return val
    def StartPage(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_StartPage,(self,) + _args, _kwargs)
        return val
    def DrawBitmap(self, *_args, **_kwargs):
        val = apply(gdic.wxDC_DrawBitmap,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxDC instance at %s>" % (self.this,)
class wxDC(wxDCPtr):
    def __init__(self,this):
        self.this = this




class wxMemoryDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SelectObject(self, *_args, **_kwargs):
        val = apply(gdic.wxMemoryDC_SelectObject,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxMemoryDC instance at %s>" % (self.this,)
class wxMemoryDC(wxMemoryDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxMemoryDC,_args,_kwargs)
        self.thisown = 1




class wxScreenDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def StartDrawingOnTop(self, *_args, **_kwargs):
        val = apply(gdic.wxScreenDC_StartDrawingOnTop,(self,) + _args, _kwargs)
        return val
    def StartDrawingOnTopRect(self, *_args, **_kwargs):
        val = apply(gdic.wxScreenDC_StartDrawingOnTopRect,(self,) + _args, _kwargs)
        return val
    def EndDrawingOnTop(self, *_args, **_kwargs):
        val = apply(gdic.wxScreenDC_EndDrawingOnTop,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxScreenDC instance at %s>" % (self.this,)
class wxScreenDC(wxScreenDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxScreenDC,_args,_kwargs)
        self.thisown = 1




class wxClientDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxClientDC instance at %s>" % (self.this,)
class wxClientDC(wxClientDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxClientDC,_args,_kwargs)
        self.thisown = 1




class wxPaintDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxPaintDC instance at %s>" % (self.this,)
class wxPaintDC(wxPaintDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxPaintDC,_args,_kwargs)
        self.thisown = 1




class wxWindowDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxWindowDC instance at %s>" % (self.this,)
class wxWindowDC(wxWindowDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxWindowDC,_args,_kwargs)
        self.thisown = 1




class wxPostScriptDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxPostScriptDC instance at %s>" % (self.this,)
class wxPostScriptDC(wxPostScriptDCPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxPostScriptDC,_args,_kwargs)
        self.thisown = 1




class wxPalettePtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxPalette(self)
    def GetPixel(self, *_args, **_kwargs):
        val = apply(gdic.wxPalette_GetPixel,(self,) + _args, _kwargs)
        return val
    def GetRGB(self, *_args, **_kwargs):
        val = apply(gdic.wxPalette_GetRGB,(self,) + _args, _kwargs)
        return val
    def Ok(self, *_args, **_kwargs):
        val = apply(gdic.wxPalette_Ok,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxPalette instance at %s>" % (self.this,)
class wxPalette(wxPalettePtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxPalette,_args,_kwargs)
        self.thisown = 1




class wxImageListPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self,gdic=gdic):
        if self.thisown == 1 :
            gdic.delete_wxImageList(self)
    def Add(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_Add,(self,) + _args, _kwargs)
        return val
    def Replace(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_Replace,(self,) + _args, _kwargs)
        return val
    def Draw(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_Draw,(self,) + _args, _kwargs)
        return val
    def GetImageCount(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_GetImageCount,(self,) + _args, _kwargs)
        return val
    def Remove(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_Remove,(self,) + _args, _kwargs)
        return val
    def RemoveAll(self, *_args, **_kwargs):
        val = apply(gdic.wxImageList_RemoveAll,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxImageList instance at %s>" % (self.this,)
class wxImageList(wxImageListPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(gdic.new_wxImageList,_args,_kwargs)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxEmptyBitmap(*_args, **_kwargs):
    val = apply(gdic.wxEmptyBitmap,_args,_kwargs)
    if val: val = wxBitmapPtr(val); val.thisown = 1
    return val

def wxMaskColour(*_args, **_kwargs):
    val = apply(gdic.wxMaskColour,_args,_kwargs)
    if val: val = wxMaskPtr(val); val.thisown = 1
    return val

def wxStockCursor(*_args, **_kwargs):
    val = apply(gdic.wxStockCursor,_args,_kwargs)
    if val: val = wxCursorPtr(val); val.thisown = 1
    return val

wxFont_GetDefaultEncoding = gdic.wxFont_GetDefaultEncoding

wxFont_SetDefaultEncoding = gdic.wxFont_SetDefaultEncoding

def wxNamedColour(*_args, **_kwargs):
    val = apply(gdic.wxNamedColour,_args,_kwargs)
    if val: val = wxColourPtr(val); val.thisown = 1
    return val

def wxMemoryDCFromDC(*_args, **_kwargs):
    val = apply(gdic.wxMemoryDCFromDC,_args,_kwargs)
    if val: val = wxMemoryDCPtr(val); val.thisown = 1
    return val



#-------------- VARIABLE WRAPPERS ------------------

wxFONTENCODING_SYSTEM = gdic.wxFONTENCODING_SYSTEM
wxFONTENCODING_DEFAULT = gdic.wxFONTENCODING_DEFAULT
wxFONTENCODING_ISO8859_1 = gdic.wxFONTENCODING_ISO8859_1
wxFONTENCODING_ISO8859_2 = gdic.wxFONTENCODING_ISO8859_2
wxFONTENCODING_ISO8859_3 = gdic.wxFONTENCODING_ISO8859_3
wxFONTENCODING_ISO8859_4 = gdic.wxFONTENCODING_ISO8859_4
wxFONTENCODING_ISO8859_5 = gdic.wxFONTENCODING_ISO8859_5
wxFONTENCODING_ISO8859_6 = gdic.wxFONTENCODING_ISO8859_6
wxFONTENCODING_ISO8859_7 = gdic.wxFONTENCODING_ISO8859_7
wxFONTENCODING_ISO8859_8 = gdic.wxFONTENCODING_ISO8859_8
wxFONTENCODING_ISO8859_9 = gdic.wxFONTENCODING_ISO8859_9
wxFONTENCODING_ISO8859_10 = gdic.wxFONTENCODING_ISO8859_10
wxFONTENCODING_ISO8859_11 = gdic.wxFONTENCODING_ISO8859_11
wxFONTENCODING_ISO8859_12 = gdic.wxFONTENCODING_ISO8859_12
wxFONTENCODING_ISO8859_13 = gdic.wxFONTENCODING_ISO8859_13
wxFONTENCODING_ISO8859_14 = gdic.wxFONTENCODING_ISO8859_14
wxFONTENCODING_ISO8859_15 = gdic.wxFONTENCODING_ISO8859_15
wxFONTENCODING_KOI8 = gdic.wxFONTENCODING_KOI8
wxFONTENCODING_ALTERNATIVE = gdic.wxFONTENCODING_ALTERNATIVE
wxFONTENCODING_BULGARIAN = gdic.wxFONTENCODING_BULGARIAN
wxFONTENCODING_CP437 = gdic.wxFONTENCODING_CP437
wxFONTENCODING_CP850 = gdic.wxFONTENCODING_CP850
wxFONTENCODING_CP852 = gdic.wxFONTENCODING_CP852
wxFONTENCODING_CP855 = gdic.wxFONTENCODING_CP855
wxFONTENCODING_CP866 = gdic.wxFONTENCODING_CP866
wxFONTENCODING_CP1250 = gdic.wxFONTENCODING_CP1250
wxFONTENCODING_CP1251 = gdic.wxFONTENCODING_CP1251
wxFONTENCODING_CP1252 = gdic.wxFONTENCODING_CP1252
wxFONTENCODING_MAX = gdic.wxFONTENCODING_MAX
cvar = gdic.cvar
wxNORMAL_FONT = wxFontPtr(gdic.cvar.wxNORMAL_FONT)
wxSMALL_FONT = wxFontPtr(gdic.cvar.wxSMALL_FONT)
wxITALIC_FONT = wxFontPtr(gdic.cvar.wxITALIC_FONT)
wxSWISS_FONT = wxFontPtr(gdic.cvar.wxSWISS_FONT)
wxRED_PEN = wxPenPtr(gdic.cvar.wxRED_PEN)
wxCYAN_PEN = wxPenPtr(gdic.cvar.wxCYAN_PEN)
wxGREEN_PEN = wxPenPtr(gdic.cvar.wxGREEN_PEN)
wxBLACK_PEN = wxPenPtr(gdic.cvar.wxBLACK_PEN)
wxWHITE_PEN = wxPenPtr(gdic.cvar.wxWHITE_PEN)
wxTRANSPARENT_PEN = wxPenPtr(gdic.cvar.wxTRANSPARENT_PEN)
wxBLACK_DASHED_PEN = wxPenPtr(gdic.cvar.wxBLACK_DASHED_PEN)
wxGREY_PEN = wxPenPtr(gdic.cvar.wxGREY_PEN)
wxMEDIUM_GREY_PEN = wxPenPtr(gdic.cvar.wxMEDIUM_GREY_PEN)
wxLIGHT_GREY_PEN = wxPenPtr(gdic.cvar.wxLIGHT_GREY_PEN)
wxBLUE_BRUSH = wxBrushPtr(gdic.cvar.wxBLUE_BRUSH)
wxGREEN_BRUSH = wxBrushPtr(gdic.cvar.wxGREEN_BRUSH)
wxWHITE_BRUSH = wxBrushPtr(gdic.cvar.wxWHITE_BRUSH)
wxBLACK_BRUSH = wxBrushPtr(gdic.cvar.wxBLACK_BRUSH)
wxTRANSPARENT_BRUSH = wxBrushPtr(gdic.cvar.wxTRANSPARENT_BRUSH)
wxCYAN_BRUSH = wxBrushPtr(gdic.cvar.wxCYAN_BRUSH)
wxRED_BRUSH = wxBrushPtr(gdic.cvar.wxRED_BRUSH)
wxGREY_BRUSH = wxBrushPtr(gdic.cvar.wxGREY_BRUSH)
wxMEDIUM_GREY_BRUSH = wxBrushPtr(gdic.cvar.wxMEDIUM_GREY_BRUSH)
wxLIGHT_GREY_BRUSH = wxBrushPtr(gdic.cvar.wxLIGHT_GREY_BRUSH)
wxBLACK = wxColourPtr(gdic.cvar.wxBLACK)
wxWHITE = wxColourPtr(gdic.cvar.wxWHITE)
wxRED = wxColourPtr(gdic.cvar.wxRED)
wxBLUE = wxColourPtr(gdic.cvar.wxBLUE)
wxGREEN = wxColourPtr(gdic.cvar.wxGREEN)
wxCYAN = wxColourPtr(gdic.cvar.wxCYAN)
wxLIGHT_GREY = wxColourPtr(gdic.cvar.wxLIGHT_GREY)
wxSTANDARD_CURSOR = wxCursorPtr(gdic.cvar.wxSTANDARD_CURSOR)
wxHOURGLASS_CURSOR = wxCursorPtr(gdic.cvar.wxHOURGLASS_CURSOR)
wxCROSS_CURSOR = wxCursorPtr(gdic.cvar.wxCROSS_CURSOR)
wxNullBitmap = wxBitmapPtr(gdic.cvar.wxNullBitmap)
wxNullIcon = wxIconPtr(gdic.cvar.wxNullIcon)
wxNullCursor = wxCursorPtr(gdic.cvar.wxNullCursor)
wxNullPen = wxPenPtr(gdic.cvar.wxNullPen)
wxNullBrush = wxBrushPtr(gdic.cvar.wxNullBrush)
wxNullPalette = wxPalettePtr(gdic.cvar.wxNullPalette)
wxNullFont = wxFontPtr(gdic.cvar.wxNullFont)
wxNullColour = wxColourPtr(gdic.cvar.wxNullColour)
wxIMAGELIST_DRAW_NORMAL = gdic.wxIMAGELIST_DRAW_NORMAL
wxIMAGELIST_DRAW_TRANSPARENT = gdic.wxIMAGELIST_DRAW_TRANSPARENT
wxIMAGELIST_DRAW_SELECTED = gdic.wxIMAGELIST_DRAW_SELECTED
wxIMAGELIST_DRAW_FOCUSED = gdic.wxIMAGELIST_DRAW_FOCUSED
wxIMAGE_LIST_NORMAL = gdic.wxIMAGE_LIST_NORMAL
wxIMAGE_LIST_SMALL = gdic.wxIMAGE_LIST_SMALL
wxIMAGE_LIST_STATE = gdic.wxIMAGE_LIST_STATE
