# This file was created automatically by SWIG.
import gdic

from misc import *
class wxBitmapPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            gdic.delete_wxBitmap(self.this)
    def Create(self,arg0,arg1,*args):
        val = apply(gdic.wxBitmap_Create,(self.this,arg0,arg1,)+args)
        return val
    def GetDepth(self):
        val = gdic.wxBitmap_GetDepth(self.this)
        return val
    def GetHeight(self):
        val = gdic.wxBitmap_GetHeight(self.this)
        return val
    def GetPalette(self):
        val = gdic.wxBitmap_GetPalette(self.this)
        return val
    def GetMask(self):
        val = gdic.wxBitmap_GetMask(self.this)
        val = wxMaskPtr(val)
        return val
    def GetWidth(self):
        val = gdic.wxBitmap_GetWidth(self.this)
        return val
    def LoadFile(self,arg0,arg1):
        val = gdic.wxBitmap_LoadFile(self.this,arg0,arg1)
        return val
    def Ok(self):
        val = gdic.wxBitmap_Ok(self.this)
        return val
    def SaveFile(self,arg0,arg1,*args):
        val = apply(gdic.wxBitmap_SaveFile,(self.this,arg0,arg1,)+args)
        return val
    def SetDepth(self,arg0):
        val = gdic.wxBitmap_SetDepth(self.this,arg0)
        return val
    def SetHeight(self,arg0):
        val = gdic.wxBitmap_SetHeight(self.this,arg0)
        return val
    def SetMask(self,arg0):
        val = gdic.wxBitmap_SetMask(self.this,arg0.this)
        return val
    def SetPalette(self,arg0):
        val = gdic.wxBitmap_SetPalette(self.this,arg0)
        return val
    def SetWidth(self,arg0):
        val = gdic.wxBitmap_SetWidth(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxBitmap instance>"
class wxBitmap(wxBitmapPtr):
    def __init__(self,arg0,arg1) :
        self.this = gdic.new_wxBitmap(arg0,arg1)
        self.thisown = 1




class wxMaskPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            gdic.delete_wxMask(self.this)
    def __repr__(self):
        return "<C wxMask instance>"
class wxMask(wxMaskPtr):
    def __init__(self,arg0) :
        self.this = gdic.new_wxMask(arg0.this)
        self.thisown = 1




class wxIconPtr(wxBitmapPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            gdic.delete_wxIcon(self.this)
    def GetDepth(self):
        val = gdic.wxIcon_GetDepth(self.this)
        return val
    def GetHeight(self):
        val = gdic.wxIcon_GetHeight(self.this)
        return val
    def GetWidth(self):
        val = gdic.wxIcon_GetWidth(self.this)
        return val
    def LoadFile(self,arg0,arg1):
        val = gdic.wxIcon_LoadFile(self.this,arg0,arg1)
        return val
    def Ok(self):
        val = gdic.wxIcon_Ok(self.this)
        return val
    def SetDepth(self,arg0):
        val = gdic.wxIcon_SetDepth(self.this,arg0)
        return val
    def SetHeight(self,arg0):
        val = gdic.wxIcon_SetHeight(self.this,arg0)
        return val
    def SetWidth(self,arg0):
        val = gdic.wxIcon_SetWidth(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxIcon instance>"
class wxIcon(wxIconPtr):
    def __init__(self,arg0,arg1,*args) :
        self.this = apply(gdic.new_wxIcon,(arg0,arg1,)+args)
        self.thisown = 1




class wxCursorPtr(wxBitmapPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            gdic.delete_wxCursor(self.this)
    def Ok(self):
        val = gdic.wxCursor_Ok(self.this)
        return val
    def __repr__(self):
        return "<C wxCursor instance>"
class wxCursor(wxCursorPtr):
    def __init__(self,arg0,arg1,*args) :
        self.this = apply(gdic.new_wxCursor,(arg0,arg1,)+args)
        self.thisown = 1




class wxFontPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetFaceName(self):
        val = gdic.wxFont_GetFaceName(self.this)
        return val
    def GetFamily(self):
        val = gdic.wxFont_GetFamily(self.this)
        return val
    def GetFontId(self):
        val = gdic.wxFont_GetFontId(self.this)
        return val
    def GetPointSize(self):
        val = gdic.wxFont_GetPointSize(self.this)
        return val
    def GetStyle(self):
        val = gdic.wxFont_GetStyle(self.this)
        return val
    def GetUnderlined(self):
        val = gdic.wxFont_GetUnderlined(self.this)
        return val
    def GetWeight(self):
        val = gdic.wxFont_GetWeight(self.this)
        return val
    def SetFaceName(self,arg0):
        val = gdic.wxFont_SetFaceName(self.this,arg0)
        return val
    def SetFamily(self,arg0):
        val = gdic.wxFont_SetFamily(self.this,arg0)
        return val
    def SetPointSize(self,arg0):
        val = gdic.wxFont_SetPointSize(self.this,arg0)
        return val
    def SetStyle(self,arg0):
        val = gdic.wxFont_SetStyle(self.this,arg0)
        return val
    def SetUnderlined(self,arg0):
        val = gdic.wxFont_SetUnderlined(self.this,arg0)
        return val
    def SetWeight(self,arg0):
        val = gdic.wxFont_SetWeight(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxFont instance>"
class wxFont(wxFontPtr):
    def __init__(self,arg0,arg1,arg2,arg3,*args) :
        self.this = apply(gdic.new_wxFont,(arg0,arg1,arg2,arg3,)+args)
        self.thisown = 1




class wxColourPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            gdic.delete_wxColour(self.this)
    def Red(self):
        val = gdic.wxColour_Red(self.this)
        return val
    def Green(self):
        val = gdic.wxColour_Green(self.this)
        return val
    def Blue(self):
        val = gdic.wxColour_Blue(self.this)
        return val
    def Ok(self):
        val = gdic.wxColour_Ok(self.this)
        return val
    def Set(self,arg0,arg1,arg2):
        val = gdic.wxColour_Set(self.this,arg0,arg1,arg2)
        return val
    def Get(self):
        val = gdic.wxColour_Get(self.this)
        return val
    def __repr__(self):
        return "<C wxColour instance>"
class wxColour(wxColourPtr):
    def __init__(self,*args) :
        self.this = apply(gdic.new_wxColour,()+args)
        self.thisown = 1




class wxPenPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetCap(self):
        val = gdic.wxPen_GetCap(self.this)
        return val
    def GetColour(self):
        val = gdic.wxPen_GetColour(self.this)
        val = wxColourPtr(val)
        return val
    def GetDashes(self,arg0):
        val = gdic.wxPen_GetDashes(self.this,arg0)
        return val
    def GetStipple(self):
        val = gdic.wxPen_GetStipple(self.this)
        val = wxBitmapPtr(val)
        return val
    def GetJoin(self):
        val = gdic.wxPen_GetJoin(self.this)
        return val
    def GetStyle(self):
        val = gdic.wxPen_GetStyle(self.this)
        return val
    def GetWidth(self):
        val = gdic.wxPen_GetWidth(self.this)
        return val
    def Ok(self):
        val = gdic.wxPen_Ok(self.this)
        return val
    def SetCap(self,arg0):
        val = gdic.wxPen_SetCap(self.this,arg0)
        return val
    def SetColour(self,arg0):
        val = gdic.wxPen_SetColour(self.this,arg0.this)
        return val
    def SetDashes(self,arg0,*args):
        val = apply(gdic.wxPen_SetDashes,(self.this,arg0,)+args)
        return val
    def SetStipple(self,arg0):
        val = gdic.wxPen_SetStipple(self.this,arg0.this)
        return val
    def SetJoin(self,arg0):
        val = gdic.wxPen_SetJoin(self.this,arg0)
        return val
    def SetStyle(self,arg0):
        val = gdic.wxPen_SetStyle(self.this,arg0)
        return val
    def SetWidth(self,arg0):
        val = gdic.wxPen_SetWidth(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxPen instance>"
class wxPen(wxPenPtr):
    def __init__(self,arg0,*args) :
        self.this = apply(gdic.new_wxPen,(arg0.this,)+args)
        self.thisown = 1




class wxBrushPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def GetColour(self):
        val = gdic.wxBrush_GetColour(self.this)
        val = wxColourPtr(val)
        return val
    def GetStipple(self):
        val = gdic.wxBrush_GetStipple(self.this)
        val = wxBitmapPtr(val)
        return val
    def GetStyle(self):
        val = gdic.wxBrush_GetStyle(self.this)
        return val
    def Ok(self):
        val = gdic.wxBrush_Ok(self.this)
        return val
    def SetColour(self,arg0):
        val = gdic.wxBrush_SetColour(self.this,arg0.this)
        return val
    def SetStipple(self,arg0):
        val = gdic.wxBrush_SetStipple(self.this,arg0.this)
        return val
    def SetStyle(self,arg0):
        val = gdic.wxBrush_SetStyle(self.this,arg0)
        return val
    def __repr__(self):
        return "<C wxBrush instance>"
class wxBrush(wxBrushPtr):
    def __init__(self,arg0,*args) :
        self.this = apply(gdic.new_wxBrush,(arg0.this,)+args)
        self.thisown = 1




class wxDCPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self):
        if self.thisown == 1 :
            gdic.delete_wxDC(self.this)
    def BeginDrawing(self):
        val = gdic.wxDC_BeginDrawing(self.this)
        return val
    def Blit(self,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7):
        val = gdic.wxDC_Blit(self.this,arg0,arg1,arg2,arg3,arg4.this,arg5,arg6,arg7)
        return val
    def Clear(self):
        val = gdic.wxDC_Clear(self.this)
        return val
    def CrossHair(self,arg0,arg1):
        val = gdic.wxDC_CrossHair(self.this,arg0,arg1)
        return val
    def DestroyClippingRegion(self):
        val = gdic.wxDC_DestroyClippingRegion(self.this)
        return val
    def DeviceToLogicalX(self,arg0):
        val = gdic.wxDC_DeviceToLogicalX(self.this,arg0)
        return val
    def DeviceToLogicalXRel(self,arg0):
        val = gdic.wxDC_DeviceToLogicalXRel(self.this,arg0)
        return val
    def DeviceToLogicalY(self,arg0):
        val = gdic.wxDC_DeviceToLogicalY(self.this,arg0)
        return val
    def DeviceToLogicalYRel(self,arg0):
        val = gdic.wxDC_DeviceToLogicalYRel(self.this,arg0)
        return val
    def DrawArc(self,arg0,arg1,arg2,arg3,arg4,arg5):
        val = gdic.wxDC_DrawArc(self.this,arg0,arg1,arg2,arg3,arg4,arg5)
        return val
    def DrawEllipse(self,arg0,arg1,arg2,arg3):
        val = gdic.wxDC_DrawEllipse(self.this,arg0,arg1,arg2,arg3)
        return val
    def DrawEllipticArc(self,arg0,arg1,arg2,arg3,arg4,arg5):
        val = gdic.wxDC_DrawEllipticArc(self.this,arg0,arg1,arg2,arg3,arg4,arg5)
        return val
    def DrawIcon(self,arg0,arg1,arg2):
        val = gdic.wxDC_DrawIcon(self.this,arg0.this,arg1,arg2)
        return val
    def DrawLine(self,arg0,arg1,arg2,arg3):
        val = gdic.wxDC_DrawLine(self.this,arg0,arg1,arg2,arg3)
        return val
    def DrawLines(self,arg0,*args):
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        val = apply(gdic.wxDC_DrawLines,(self.this,arg0,)+args)
        return val
    def DrawPolygon(self,arg0,*args):
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        val = apply(gdic.wxDC_DrawPolygon,(self.this,arg0,)+args)
        return val
    def DrawPoint(self,arg0,arg1):
        val = gdic.wxDC_DrawPoint(self.this,arg0,arg1)
        return val
    def DrawRectangle(self,arg0,arg1,arg2,arg3):
        val = gdic.wxDC_DrawRectangle(self.this,arg0,arg1,arg2,arg3)
        return val
    def DrawRoundedRectangle(self,arg0,arg1,arg2,arg3,*args):
        val = apply(gdic.wxDC_DrawRoundedRectangle,(self.this,arg0,arg1,arg2,arg3,)+args)
        return val
    def DrawSpline(self,arg0,*args):
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        val = apply(gdic.wxDC_DrawSpline,(self.this,arg0,)+args)
        return val
    def DrawText(self,arg0,arg1,arg2):
        val = gdic.wxDC_DrawText(self.this,arg0,arg1,arg2)
        return val
    def EndDoc(self):
        val = gdic.wxDC_EndDoc(self.this)
        return val
    def EndDrawing(self):
        val = gdic.wxDC_EndDrawing(self.this)
        return val
    def EndPage(self):
        val = gdic.wxDC_EndPage(self.this)
        return val
    def GetBackground(self):
        val = gdic.wxDC_GetBackground(self.this)
        val = wxBrushPtr(val)
        return val
    def GetBrush(self):
        val = gdic.wxDC_GetBrush(self.this)
        val = wxBrushPtr(val)
        return val
    def GetCharHeight(self):
        val = gdic.wxDC_GetCharHeight(self.this)
        return val
    def GetCharWidth(self):
        val = gdic.wxDC_GetCharWidth(self.this)
        return val
    def GetClippingBox(self):
        val = gdic.wxDC_GetClippingBox(self.this)
        return val
    def GetFont(self):
        val = gdic.wxDC_GetFont(self.this)
        val = wxFontPtr(val)
        return val
    def GetLogicalFunction(self):
        val = gdic.wxDC_GetLogicalFunction(self.this)
        return val
    def GetMapMode(self):
        val = gdic.wxDC_GetMapMode(self.this)
        return val
    def GetOptimization(self):
        val = gdic.wxDC_GetOptimization(self.this)
        return val
    def GetPen(self):
        val = gdic.wxDC_GetPen(self.this)
        val = wxPenPtr(val)
        return val
    def GetPixel(self,arg0,arg1):
        val = gdic.wxDC_GetPixel(self.this,arg0,arg1)
        val = wxColourPtr(val)
        val.thisown = 1
        return val
    def GetSize(self):
        val = gdic.wxDC_GetSize(self.this)
        return val
    def GetTextBackground(self):
        val = gdic.wxDC_GetTextBackground(self.this)
        val = wxColourPtr(val)
        return val
    def GetTextExtent(self,arg0):
        val = gdic.wxDC_GetTextExtent(self.this,arg0)
        return val
    def GetTextForeground(self):
        val = gdic.wxDC_GetTextForeground(self.this)
        val = wxColourPtr(val)
        return val
    def LogicalToDeviceX(self,arg0):
        val = gdic.wxDC_LogicalToDeviceX(self.this,arg0)
        return val
    def LogicalToDeviceXRel(self,arg0):
        val = gdic.wxDC_LogicalToDeviceXRel(self.this,arg0)
        return val
    def LogicalToDeviceY(self,arg0):
        val = gdic.wxDC_LogicalToDeviceY(self.this,arg0)
        return val
    def LogicalToDeviceYRel(self,arg0):
        val = gdic.wxDC_LogicalToDeviceYRel(self.this,arg0)
        return val
    def MaxX(self):
        val = gdic.wxDC_MaxX(self.this)
        return val
    def MaxY(self):
        val = gdic.wxDC_MaxY(self.this)
        return val
    def MinX(self):
        val = gdic.wxDC_MinX(self.this)
        return val
    def MinY(self):
        val = gdic.wxDC_MinY(self.this)
        return val
    def Ok(self):
        val = gdic.wxDC_Ok(self.this)
        return val
    def SetDeviceOrigin(self,arg0,arg1):
        val = gdic.wxDC_SetDeviceOrigin(self.this,arg0,arg1)
        return val
    def SetBackground(self,arg0):
        val = gdic.wxDC_SetBackground(self.this,arg0.this)
        return val
    def SetBackgroundMode(self,arg0):
        val = gdic.wxDC_SetBackgroundMode(self.this,arg0)
        return val
    def SetClippingRegion(self,arg0,arg1,arg2,arg3):
        val = gdic.wxDC_SetClippingRegion(self.this,arg0,arg1,arg2,arg3)
        return val
    def SetPalette(self,arg0):
        val = gdic.wxDC_SetPalette(self.this,arg0)
        return val
    def SetBrush(self,arg0):
        val = gdic.wxDC_SetBrush(self.this,arg0.this)
        return val
    def SetFont(self,arg0):
        val = gdic.wxDC_SetFont(self.this,arg0.this)
        return val
    def SetLogicalFunction(self,arg0):
        val = gdic.wxDC_SetLogicalFunction(self.this,arg0)
        return val
    def SetMapMode(self,arg0):
        val = gdic.wxDC_SetMapMode(self.this,arg0)
        return val
    def SetOptimization(self,arg0):
        val = gdic.wxDC_SetOptimization(self.this,arg0)
        return val
    def SetPen(self,arg0):
        val = gdic.wxDC_SetPen(self.this,arg0.this)
        return val
    def SetTextBackground(self,arg0):
        val = gdic.wxDC_SetTextBackground(self.this,arg0.this)
        return val
    def SetTextForeground(self,arg0):
        val = gdic.wxDC_SetTextForeground(self.this,arg0.this)
        return val
    def SetUserScale(self,arg0,arg1):
        val = gdic.wxDC_SetUserScale(self.this,arg0,arg1)
        return val
    def StartDoc(self,arg0):
        val = gdic.wxDC_StartDoc(self.this,arg0)
        return val
    def StartPage(self):
        val = gdic.wxDC_StartPage(self.this)
        return val
    def DrawBitmap(self,arg0,arg1,arg2,*args):
        val = apply(gdic.wxDC_DrawBitmap,(self.this,arg0.this,arg1,arg2,)+args)
        return val
    def __repr__(self):
        return "<C wxDC instance>"
class wxDC(wxDCPtr):
    def __init__(self,this):
        self.this = this




class wxMemoryDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SelectObject(self,arg0):
        val = gdic.wxMemoryDC_SelectObject(self.this,arg0.this)
        return val
    def __repr__(self):
        return "<C wxMemoryDC instance>"
class wxMemoryDC(wxMemoryDCPtr):
    def __init__(self) :
        self.this = gdic.new_wxMemoryDC()
        self.thisown = 1




class wxScreenDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def StartDrawingOnTop(self,arg0):
        val = gdic.wxScreenDC_StartDrawingOnTop(self.this,arg0.this)
        return val
    def StartDrawingOnTopRect(self,*args):
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        val = apply(gdic.wxScreenDC_StartDrawingOnTopRect,(self.this,)+args)
        return val
    def EndDrawingOnTop(self):
        val = gdic.wxScreenDC_EndDrawingOnTop(self.this)
        return val
    def __repr__(self):
        return "<C wxScreenDC instance>"
class wxScreenDC(wxScreenDCPtr):
    def __init__(self) :
        self.this = gdic.new_wxScreenDC()
        self.thisown = 1




class wxClientDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxClientDC instance>"
class wxClientDC(wxClientDCPtr):
    def __init__(self,arg0) :
        self.this = gdic.new_wxClientDC(arg0.this)
        self.thisown = 1




class wxPaintDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxPaintDC instance>"
class wxPaintDC(wxPaintDCPtr):
    def __init__(self,arg0) :
        self.this = gdic.new_wxPaintDC(arg0.this)
        self.thisown = 1




class wxPrinterDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __repr__(self):
        return "<C wxPrinterDC instance>"
class wxPrinterDC(wxPrinterDCPtr):
    def __init__(self,arg0,arg1,arg2,*args) :
        self.this = apply(gdic.new_wxPrinterDC,(arg0,arg1,arg2,)+args)
        self.thisown = 1




class wxMetaFileDCPtr(wxDCPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def Close(self):
        val = gdic.wxMetaFileDC_Close(self.this)
        return val
    def __repr__(self):
        return "<C wxMetaFileDC instance>"
class wxMetaFileDC(wxMetaFileDCPtr):
    def __init__(self,*args) :
        self.this = apply(gdic.new_wxMetaFileDC,()+args)
        self.thisown = 1






#-------------- FUNCTION WRAPPERS ------------------

def wxEmptyBitmap(arg0,arg1,*args):
    val = apply(gdic.wxEmptyBitmap,(arg0,arg1,)+args)
    val = wxBitmapPtr(val)
    val.thisown = 1
    return val

def wxNoRefBitmap(arg0,arg1):
    val = gdic.wxNoRefBitmap(arg0,arg1)
    val = wxBitmapPtr(val)
    return val

def wxMaskColour(arg0,arg1):
    val = gdic.wxMaskColour(arg0.this,arg1.this)
    val = wxMaskPtr(val)
    val.thisown = 1
    return val

def wxStockCursor(arg0):
    val = gdic.wxStockCursor(arg0)
    val = wxCursorPtr(val)
    val.thisown = 1
    return val

def wxNamedColour(arg0):
    val = gdic.wxNamedColour(arg0)
    val = wxColourPtr(val)
    val.thisown = 1
    return val

def wxMemoryDCFromDC(arg0):
    val = gdic.wxMemoryDCFromDC(arg0.this)
    val = wxMemoryDCPtr(val)
    val.thisown = 1
    return val



#-------------- VARIABLE WRAPPERS ------------------

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
wxNullFont = wxFontPtr(gdic.cvar.wxNullFont)
wxNullColour = wxColourPtr(gdic.cvar.wxNullColour)
