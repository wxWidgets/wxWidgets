"""Decorator classes for documentation and shell scripting.
"""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]


# These are not the real wxPython classes. These are Python versions
# for documentation purposes. They are also used to apply docstrings
# to the real wxPython classes, which are SWIG-generated wrappers for
# C-language classes.


from Base import Object
import Parameters as wx


class DC(Object):
    """"""

    def BeginDrawing(self):
        """"""
        pass

    def Blit(self):
        """"""
        pass

    def CalcBoundingBox(self):
        """"""
        pass

    def CanDrawBitmap(self):
        """"""
        pass

    def CanGetTextExtent(self):
        """"""
        pass

    def Clear(self):
        """"""
        pass

    def CrossHair(self):
        """"""
        pass

    def DestroyClippingRegion(self):
        """"""
        pass

    def DeviceToLogicalX(self):
        """"""
        pass

    def DeviceToLogicalXRel(self):
        """"""
        pass

    def DeviceToLogicalY(self):
        """"""
        pass

    def DeviceToLogicalYRel(self):
        """"""
        pass

    def DrawArc(self):
        """"""
        pass

    def DrawBitmap(self):
        """"""
        pass

    def DrawCircle(self):
        """"""
        pass

    def DrawEllipse(self):
        """"""
        pass

    def DrawEllipticArc(self):
        """"""
        pass

    def DrawIcon(self):
        """"""
        pass

    def DrawImageLabel(self):
        """"""
        pass

    def DrawLabel(self):
        """"""
        pass

    def DrawLine(self):
        """"""
        pass

    def DrawLineList(self):
        """"""
        pass

    def DrawLines(self):
        """"""
        pass

    def DrawPoint(self):
        """"""
        pass

    def DrawPointList(self):
        """"""
        pass

    def DrawPolygon(self):
        """"""
        pass

    def DrawRectangle(self):
        """"""
        pass

    def DrawRectangleRect(self):
        """"""
        pass

    def DrawRotatedText(self):
        """"""
        pass

    def DrawRoundedRectangle(self):
        """"""
        pass

    def DrawSpline(self):
        """"""
        pass

    def DrawText(self):
        """"""
        pass

    def EndDoc(self):
        """"""
        pass

    def EndDrawing(self):
        """"""
        pass

    def EndPage(self):
        """"""
        pass

    def FloodFill(self):
        """"""
        pass

    def GetBackground(self):
        """"""
        pass

    def GetBoundingBox(self):
        """"""
        pass

    def GetBrush(self):
        """"""
        pass

    def GetCharHeight(self):
        """"""
        pass

    def GetCharWidth(self):
        """"""
        pass

    def GetClippingBox(self):
        """"""
        pass

    def GetDepth(self):
        """"""
        pass

    def GetDeviceOrigin(self):
        """"""
        pass

    def GetFont(self):
        """"""
        pass

    def GetFullTextExtent(self):
        """"""
        pass

    def GetLogicalFunction(self):
        """"""
        pass

    def GetLogicalOrigin(self):
        """"""
        pass

    def GetLogicalScale(self):
        """"""
        pass

    def GetMapMode(self):
        """"""
        pass

    def GetMultiLineTextExtent(self):
        """"""
        pass

    def GetOptimization(self):
        """"""
        pass

    def GetPPI(self):
        """"""
        pass

    def GetPen(self):
        """"""
        pass

    def GetPixel(self):
        """"""
        pass

    def GetSize(self):
        """"""
        pass

    def GetSizeMM(self):
        """"""
        pass

    def GetSizeTuple(self):
        """"""
        pass

    def GetTextBackground(self):
        """"""
        pass

    def GetTextExtent(self):
        """"""
        pass

    def GetTextForeground(self):
        """"""
        pass

    def GetUserScale(self):
        """"""
        pass

    def LogicalToDeviceX(self):
        """"""
        pass

    def LogicalToDeviceXRel(self):
        """"""
        pass

    def LogicalToDeviceY(self):
        """"""
        pass

    def LogicalToDeviceYRel(self):
        """"""
        pass

    def MaxX(self):
        """"""
        pass

    def MaxY(self):
        """"""
        pass

    def MinX(self):
        """"""
        pass

    def MinY(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def ResetBoundingBox(self):
        """"""
        pass

    def SetAxisOrientation(self):
        """"""
        pass

    def SetBackground(self):
        """"""
        pass

    def SetBackgroundMode(self):
        """"""
        pass

    def SetBrush(self):
        """"""
        pass

    def SetClippingRect(self):
        """"""
        pass

    def SetClippingRegion(self):
        """"""
        pass

    def SetClippingRegionAsRegion(self):
        """"""
        pass

    def SetDeviceOrigin(self):
        """"""
        pass

    def SetFont(self):
        """"""
        pass

    def SetLogicalFunction(self):
        """"""
        pass

    def SetLogicalOrigin(self):
        """"""
        pass

    def SetLogicalScale(self):
        """"""
        pass

    def SetMapMode(self):
        """"""
        pass

    def SetOptimization(self):
        """"""
        pass

    def SetPalette(self):
        """"""
        pass

    def SetPen(self):
        """"""
        pass

    def SetTextBackground(self):
        """"""
        pass

    def SetTextForeground(self):
        """"""
        pass

    def SetUserScale(self):
        """"""
        pass

    def StartDoc(self):
        """"""
        pass

    def StartPage(self):
        """"""
        pass

    def _DrawLineList(self):
        """"""
        pass

    def _DrawPointList(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ClientDC(DC):
    """"""

    def __init__(self):
        """"""
        pass


class MemoryDC(DC):
    """"""

    def SelectObject(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class BufferedDC(MemoryDC):
    """"""

    def UnMask(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class BufferedPaintDC(BufferedDC):
    """"""

    def __init__(self):
        """"""
        pass


class PaintDC(DC):
    """"""

    def __init__(self):
        """"""
        pass


class PostScriptDC(DC):
    """"""

    def GetPrintData(self):
        """"""
        pass

    def SetPrintData(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ScreenDC(DC):
    """"""

    def EndDrawingOnTop(self):
        """"""
        pass

    def StartDrawingOnTop(self):
        """"""
        pass

    def StartDrawingOnTopWin(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class WindowDC(DC):
    """"""

    def __init__(self):
        """"""
        pass


class GDIObject(Object):
    """"""

    def GetVisible(self):
        """"""
        pass

    def IsNull(self):
        """"""
        pass

    def SetVisible(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Bitmap(GDIObject):
    """"""

    def CopyFromIcon(self):
        """"""
        pass

    def GetDepth(self):
        """"""
        pass

    def GetHeight(self):
        """"""
        pass

    def GetMask(self):
        """"""
        pass

    def GetPalette(self):
        """"""
        pass

    def GetSubBitmap(self):
        """"""
        pass

    def GetWidth(self):
        """"""
        pass

    def LoadFile(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def SaveFile(self):
        """"""
        pass

    def SetDepth(self):
        """"""
        pass

    def SetHeight(self):
        """"""
        pass

    def SetMask(self):
        """"""
        pass

    def SetWidth(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Brush(GDIObject):
    """"""

    def GetColour(self):
        """"""
        pass

    def GetStipple(self):
        """"""
        pass

    def GetStyle(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def SetColour(self):
        """"""
        pass

    def SetStipple(self):
        """"""
        pass

    def SetStyle(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class BrushList(Object):
    """"""

    def AddBrush(self):
        """"""
        pass

    def FindOrCreateBrush(self):
        """"""
        pass

    def GetCount(self):
        """"""
        pass

    def RemoveBrush(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Colour(Object):
    """"""

    def Blue(self):
        """"""
        pass

    def Get(self):
        """"""
        pass

    def Green(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def Red(self):
        """"""
        pass

    def Set(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __eq__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def __ne__(self):
        """"""
        pass

    def __nonzero__(self):
        """"""
        pass

    def __str__(self):
        """"""
        pass

    def asTuple(self):
        """"""
        pass


class Cursor(GDIObject):
    """"""

    def Ok(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Font(Object):
    """"""

    def GetEncoding(self):
        """"""
        pass

    def GetFaceName(self):
        """"""
        pass

    def GetFamily(self):
        """"""
        pass

    def GetFamilyString(self):
        """"""
        pass

    def GetNativeFontInfo(self):
        """"""
        pass

    def GetNativeFontInfoDesc(self):
        """"""
        pass

    def GetNativeFontInfoUserDesc(self):
        """"""
        pass

    def GetNoAntiAliasing(self):
        """"""
        pass

    def GetPointSize(self):
        """"""
        pass

    def GetStyle(self):
        """"""
        pass

    def GetStyleString(self):
        """"""
        pass

    def GetUnderlined(self):
        """"""
        pass

    def GetWeight(self):
        """"""
        pass

    def GetWeightString(self):
        """"""
        pass

    def IsFixedWidth(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def SetEncoding(self):
        """"""
        pass

    def SetFaceName(self):
        """"""
        pass

    def SetFamily(self):
        """"""
        pass

    def SetNativeFontInfo(self):
        """"""
        pass

    def SetNativeFontInfoUserDesc(self):
        """"""
        pass

    def SetNoAntiAliasing(self):
        """"""
        pass

    def SetPointSize(self):
        """"""
        pass

    def SetStyle(self):
        """"""
        pass

    def SetUnderlined(self):
        """"""
        pass

    def SetWeight(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass

    def wxFontFromNativeInfoString(self):
        """"""
        pass


class FontList(Object):
    """"""

    def AddFont(self):
        """"""
        pass

    def FindOrCreateFont(self):
        """"""
        pass

    def GetCount(self):
        """"""
        pass

    def RemoveFont(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Icon(GDIObject):
    """"""

    def CopyFromBitmap(self):
        """"""
        pass

    def GetDepth(self):
        """"""
        pass

    def GetHeight(self):
        """"""
        pass

    def GetWidth(self):
        """"""
        pass

    def LoadFile(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def SetDepth(self):
        """"""
        pass

    def SetHeight(self):
        """"""
        pass

    def SetWidth(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class IconBundle:
    """"""

    def AddIcon(self):
        """"""
        pass

    def AddIconFromFile(self):
        """"""
        pass

    def GetIcon(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Image(Object):
    """"""

    def ConvertToBitmap(self):
        """"""
        pass

    def ConvertToMono(self):
        """"""
        pass

    def ConvertToMonoBitmap(self):
        """"""
        pass

    def Copy(self):
        """"""
        pass

    def CountColours(self):
        """"""
        pass

    def Create(self):
        """"""
        pass

    def Destroy(self):
        """"""
        pass

    def FindFirstUnusedColour(self):
        """"""
        pass

    def GetBlue(self):
        """"""
        pass

    def GetData(self):
        """"""
        pass

    def GetDataBuffer(self):
        """"""
        pass

    def GetGreen(self):
        """"""
        pass

    def GetHeight(self):
        """"""
        pass

    def GetMaskBlue(self):
        """"""
        pass

    def GetMaskGreen(self):
        """"""
        pass

    def GetMaskRed(self):
        """"""
        pass

    def GetOption(self):
        """"""
        pass

    def GetOptionInt(self):
        """"""
        pass

    def GetRed(self):
        """"""
        pass

    def GetSubImage(self):
        """"""
        pass

    def GetWidth(self):
        """"""
        pass

    def HasMask(self):
        """"""
        pass

    def HasOption(self):
        """"""
        pass

    def LoadFile(self):
        """"""
        pass

    def LoadMimeFile(self):
        """"""
        pass

    def LoadMimeStream(self):
        """"""
        pass

    def LoadStream(self):
        """"""
        pass

    def Mirror(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def Paste(self):
        """"""
        pass

    def Replace(self):
        """"""
        pass

    def Rescale(self):
        """"""
        pass

    def Rotate(self):
        """"""
        pass

    def Rotate90(self):
        """"""
        pass

    def SaveFile(self):
        """"""
        pass

    def SaveMimeFile(self):
        """"""
        pass

    def Scale(self):
        """"""
        pass

    def SetData(self):
        """"""
        pass

    def SetMask(self):
        """"""
        pass

    def SetMaskColour(self):
        """"""
        pass

    def SetMaskFromImage(self):
        """"""
        pass

    def SetOption(self):
        """"""
        pass

    def SetOptionInt(self):
        """"""
        pass

    def SetRGB(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class ImageList(Object):
    """"""

    def Add(self):
        """"""
        pass

    def AddIcon(self):
        """"""
        pass

    def AddWithColourMask(self):
        """"""
        pass

    def Draw(self):
        """"""
        pass

    def GetImageCount(self):
        """"""
        pass

    def GetSize(self):
        """"""
        pass

    def Remove(self):
        """"""
        pass

    def RemoveAll(self):
        """"""
        pass

    def Replace(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Mask(Object):
    """"""

    def Destroy(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Palette(GDIObject):
    """"""

    def GetPixel(self):
        """"""
        pass

    def GetRGB(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Pen(GDIObject):
    """"""

    def GetCap(self):
        """"""
        pass

    def GetColour(self):
        """"""
        pass

    def GetDashes(self):
        """"""
        pass

    def GetJoin(self):
        """"""
        pass

    def GetStyle(self):
        """"""
        pass

    def GetWidth(self):
        """"""
        pass

    def Ok(self):
        """"""
        pass

    def SetCap(self):
        """"""
        pass

    def SetColour(self):
        """"""
        pass

    def SetDashes(self):
        """"""
        pass

    def SetJoin(self):
        """"""
        pass

    def SetStyle(self):
        """"""
        pass

    def SetWidth(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PenList(Object):
    """"""

    def AddPen(self):
        """"""
        pass

    def FindOrCreatePen(self):
        """"""
        pass

    def GetCount(self):
        """"""
        pass

    def RemovePen(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class PyPen(Pen):
    """"""

    def SetDashes(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class Region(GDIObject):
    """"""

    def Clear(self):
        """"""
        pass

    def Contains(self):
        """"""
        pass

    def ContainsPoint(self):
        """"""
        pass

    def ContainsRect(self):
        """"""
        pass

    def ContainsRectDim(self):
        """"""
        pass

    def GetBox(self):
        """"""
        pass

    def Intersect(self):
        """"""
        pass

    def IntersectRect(self):
        """"""
        pass

    def IntersectRegion(self):
        """"""
        pass

    def IsEmpty(self):
        """"""
        pass

    def Offset(self):
        """"""
        pass

    def Subtract(self):
        """"""
        pass

    def SubtractRect(self):
        """"""
        pass

    def SubtractRegion(self):
        """"""
        pass

    def Union(self):
        """"""
        pass

    def UnionRect(self):
        """"""
        pass

    def UnionRegion(self):
        """"""
        pass

    def Xor(self):
        """"""
        pass

    def XorRect(self):
        """"""
        pass

    def XorRegion(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


class RegionIterator(Object):
    """"""

    def GetH(self):
        """"""
        pass

    def GetHeight(self):
        """"""
        pass

    def GetRect(self):
        """"""
        pass

    def GetW(self):
        """"""
        pass

    def GetWidth(self):
        """"""
        pass

    def GetX(self):
        """"""
        pass

    def GetY(self):
        """"""
        pass

    def HaveRects(self):
        """"""
        pass

    def Next(self):
        """"""
        pass

    def Reset(self):
        """"""
        pass

    def __del__(self):
        """"""
        pass

    def __init__(self):
        """"""
        pass


