# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _gdi

import core
wx = core 
#---------------------------------------------------------------------------

class GDIObject(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_GDIObject(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_GDIObject):
        try:
            if self.thisown: destroy(self)
        except: pass
    def GetVisible(*args, **kwargs): return _gdi.GDIObject_GetVisible(*args, **kwargs)
    def SetVisible(*args, **kwargs): return _gdi.GDIObject_SetVisible(*args, **kwargs)
    def IsNull(*args, **kwargs): return _gdi.GDIObject_IsNull(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGDIObject instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class GDIObjectPtr(GDIObject):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GDIObject
_gdi.GDIObject_swigregister(GDIObjectPtr)

#---------------------------------------------------------------------------

class Colour(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Colour(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Colour):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Red(*args, **kwargs): return _gdi.Colour_Red(*args, **kwargs)
    def Green(*args, **kwargs): return _gdi.Colour_Green(*args, **kwargs)
    def Blue(*args, **kwargs): return _gdi.Colour_Blue(*args, **kwargs)
    def Ok(*args, **kwargs): return _gdi.Colour_Ok(*args, **kwargs)
    def Set(*args, **kwargs): return _gdi.Colour_Set(*args, **kwargs)
    def SetRBG(*args, **kwargs): return _gdi.Colour_SetRBG(*args, **kwargs)
    def __eq__(*args, **kwargs): return _gdi.Colour___eq__(*args, **kwargs)
    def __ne__(*args, **kwargs): return _gdi.Colour___ne__(*args, **kwargs)
    def Get(*args, **kwargs): return _gdi.Colour_Get(*args, **kwargs)
    asTuple = Get
    def __str__(self):                  return str(self.asTuple())
    def __repr__(self):                 return 'wxColour' + str(self.asTuple())
    def __nonzero__(self):              return self.Ok()
    def __getinitargs__(self):          return ()
    def __getstate__(self):             return self.asTuple()
    def __setstate__(self, state):      self.Set(*state)


class ColourPtr(Colour):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Colour
_gdi.Colour_swigregister(ColourPtr)

def NamedColour(*args, **kwargs):
    val = _gdi.new_NamedColour(*args, **kwargs)
    val.thisown = 1
    return val

def ColourRGB(*args, **kwargs):
    val = _gdi.new_ColourRGB(*args, **kwargs)
    val.thisown = 1
    return val

class Palette(GDIObject):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Palette(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Palette):
        try:
            if self.thisown: destroy(self)
        except: pass
    def GetPixel(*args, **kwargs): return _gdi.Palette_GetPixel(*args, **kwargs)
    def GetRGB(*args, **kwargs): return _gdi.Palette_GetRGB(*args, **kwargs)
    def Ok(*args, **kwargs): return _gdi.Palette_Ok(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPalette instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PalettePtr(Palette):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Palette
_gdi.Palette_swigregister(PalettePtr)

#---------------------------------------------------------------------------

class Pen(GDIObject):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Pen(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Pen):
        try:
            if self.thisown: destroy(self)
        except: pass
    def GetCap(*args, **kwargs): return _gdi.Pen_GetCap(*args, **kwargs)
    def GetColour(*args, **kwargs): return _gdi.Pen_GetColour(*args, **kwargs)
    def GetJoin(*args, **kwargs): return _gdi.Pen_GetJoin(*args, **kwargs)
    def GetStyle(*args, **kwargs): return _gdi.Pen_GetStyle(*args, **kwargs)
    def GetWidth(*args, **kwargs): return _gdi.Pen_GetWidth(*args, **kwargs)
    def Ok(*args, **kwargs): return _gdi.Pen_Ok(*args, **kwargs)
    def SetCap(*args, **kwargs): return _gdi.Pen_SetCap(*args, **kwargs)
    def SetColour(*args, **kwargs): return _gdi.Pen_SetColour(*args, **kwargs)
    def SetJoin(*args, **kwargs): return _gdi.Pen_SetJoin(*args, **kwargs)
    def SetStyle(*args, **kwargs): return _gdi.Pen_SetStyle(*args, **kwargs)
    def SetWidth(*args, **kwargs): return _gdi.Pen_SetWidth(*args, **kwargs)
    def SetDashes(*args, **kwargs): return _gdi.Pen_SetDashes(*args, **kwargs)
    def GetDashes(*args, **kwargs): return _gdi.Pen_GetDashes(*args, **kwargs)
    def __nonzero__(self): return self.Ok() 
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPen instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PenPtr(Pen):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Pen
_gdi.Pen_swigregister(PenPtr)

class PyPen(Pen):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_PyPen(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_PyPen):
        try:
            if self.thisown: destroy(self)
        except: pass
    def SetDashes(*args, **kwargs): return _gdi.PyPen_SetDashes(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyPen instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PyPenPtr(PyPen):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyPen
_gdi.PyPen_swigregister(PyPenPtr)

Pen = PyPen 
#---------------------------------------------------------------------------

class Brush(GDIObject):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Brush(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Brush):
        try:
            if self.thisown: destroy(self)
        except: pass
    def SetColour(*args, **kwargs): return _gdi.Brush_SetColour(*args, **kwargs)
    def SetStyle(*args, **kwargs): return _gdi.Brush_SetStyle(*args, **kwargs)
    def SetStipple(*args, **kwargs): return _gdi.Brush_SetStipple(*args, **kwargs)
    def GetColour(*args, **kwargs): return _gdi.Brush_GetColour(*args, **kwargs)
    def GetStyle(*args, **kwargs): return _gdi.Brush_GetStyle(*args, **kwargs)
    def GetStipple(*args, **kwargs): return _gdi.Brush_GetStipple(*args, **kwargs)
    def Ok(*args, **kwargs): return _gdi.Brush_Ok(*args, **kwargs)
    def GetMacTheme(*args, **kwargs): return _gdi.Brush_GetMacTheme(*args, **kwargs)
    def SetMacTheme(*args, **kwargs): return _gdi.Brush_SetMacTheme(*args, **kwargs)
    def __nonzero__(self): return self.Ok() 
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBrush instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class BrushPtr(Brush):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Brush
_gdi.Brush_swigregister(BrushPtr)

class Bitmap(GDIObject):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Bitmap(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Bitmap):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Ok(*args, **kwargs): return _gdi.Bitmap_Ok(*args, **kwargs)
    def GetWidth(*args, **kwargs): return _gdi.Bitmap_GetWidth(*args, **kwargs)
    def GetHeight(*args, **kwargs): return _gdi.Bitmap_GetHeight(*args, **kwargs)
    def GetDepth(*args, **kwargs): return _gdi.Bitmap_GetDepth(*args, **kwargs)
    def ConvertToImage(*args, **kwargs): return _gdi.Bitmap_ConvertToImage(*args, **kwargs)
    def GetMask(*args, **kwargs): return _gdi.Bitmap_GetMask(*args, **kwargs)
    def SetMask(*args, **kwargs): return _gdi.Bitmap_SetMask(*args, **kwargs)
    def SetMaskColour(*args, **kwargs): return _gdi.Bitmap_SetMaskColour(*args, **kwargs)
    def GetSubBitmap(*args, **kwargs): return _gdi.Bitmap_GetSubBitmap(*args, **kwargs)
    def SaveFile(*args, **kwargs): return _gdi.Bitmap_SaveFile(*args, **kwargs)
    def LoadFile(*args, **kwargs): return _gdi.Bitmap_LoadFile(*args, **kwargs)
    def CopyFromIcon(*args, **kwargs): return _gdi.Bitmap_CopyFromIcon(*args, **kwargs)
    def SetHeight(*args, **kwargs): return _gdi.Bitmap_SetHeight(*args, **kwargs)
    def SetWidth(*args, **kwargs): return _gdi.Bitmap_SetWidth(*args, **kwargs)
    def SetDepth(*args, **kwargs): return _gdi.Bitmap_SetDepth(*args, **kwargs)
    def __nonzero__(self): return self.Ok() 
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBitmap instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class BitmapPtr(Bitmap):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Bitmap
_gdi.Bitmap_swigregister(BitmapPtr)

def EmptyBitmap(*args, **kwargs):
    val = _gdi.new_EmptyBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromIcon(*args, **kwargs):
    val = _gdi.new_BitmapFromIcon(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromImage(*args, **kwargs):
    val = _gdi.new_BitmapFromImage(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromXPMData(*args, **kwargs):
    val = _gdi.new_BitmapFromXPMData(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromBits(*args, **kwargs):
    val = _gdi.new_BitmapFromBits(*args, **kwargs)
    val.thisown = 1
    return val

class Mask(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Mask(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMask instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class MaskPtr(Mask):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Mask
_gdi.Mask_swigregister(MaskPtr)

def MaskColour(*args, **kwargs):
    val = _gdi.new_MaskColour(*args, **kwargs)
    val.thisown = 1
    return val

class Icon(GDIObject):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Icon(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Icon):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Ok(*args, **kwargs): return _gdi.Icon_Ok(*args, **kwargs)
    def GetWidth(*args, **kwargs): return _gdi.Icon_GetWidth(*args, **kwargs)
    def GetHeight(*args, **kwargs): return _gdi.Icon_GetHeight(*args, **kwargs)
    def GetDepth(*args, **kwargs): return _gdi.Icon_GetDepth(*args, **kwargs)
    def SetWidth(*args, **kwargs): return _gdi.Icon_SetWidth(*args, **kwargs)
    def SetHeight(*args, **kwargs): return _gdi.Icon_SetHeight(*args, **kwargs)
    def SetDepth(*args, **kwargs): return _gdi.Icon_SetDepth(*args, **kwargs)
    def CopyFromBitmap(*args, **kwargs): return _gdi.Icon_CopyFromBitmap(*args, **kwargs)
    def __nonzero__(self): return self.Ok() 
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIcon instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class IconPtr(Icon):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Icon
_gdi.Icon_swigregister(IconPtr)

def EmptyIcon(*args, **kwargs):
    val = _gdi.new_EmptyIcon(*args, **kwargs)
    val.thisown = 1
    return val

def IconFromLocation(*args, **kwargs):
    val = _gdi.new_IconFromLocation(*args, **kwargs)
    val.thisown = 1
    return val

def IconFromBitmap(*args, **kwargs):
    val = _gdi.new_IconFromBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def IconFromXPMData(*args, **kwargs):
    val = _gdi.new_IconFromXPMData(*args, **kwargs)
    val.thisown = 1
    return val

class IconLocation(object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_IconLocation(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_IconLocation):
        try:
            if self.thisown: destroy(self)
        except: pass
    def IsOk(*args, **kwargs): return _gdi.IconLocation_IsOk(*args, **kwargs)
    def __nonzero__(self): return self.Ok() 
    def SetFileName(*args, **kwargs): return _gdi.IconLocation_SetFileName(*args, **kwargs)
    def GetFileName(*args, **kwargs): return _gdi.IconLocation_GetFileName(*args, **kwargs)
    def SetIndex(*args, **kwargs): return _gdi.IconLocation_SetIndex(*args, **kwargs)
    def GetIndex(*args, **kwargs): return _gdi.IconLocation_GetIndex(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIconLocation instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class IconLocationPtr(IconLocation):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IconLocation
_gdi.IconLocation_swigregister(IconLocationPtr)

class IconBundle(object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_IconBundle(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_IconBundle):
        try:
            if self.thisown: destroy(self)
        except: pass
    def AddIcon(*args, **kwargs): return _gdi.IconBundle_AddIcon(*args, **kwargs)
    def AddIconFromFile(*args, **kwargs): return _gdi.IconBundle_AddIconFromFile(*args, **kwargs)
    def GetIcon(*args, **kwargs): return _gdi.IconBundle_GetIcon(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIconBundle instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class IconBundlePtr(IconBundle):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IconBundle
_gdi.IconBundle_swigregister(IconBundlePtr)

def IconBundleFromFile(*args, **kwargs):
    val = _gdi.new_IconBundleFromFile(*args, **kwargs)
    val.thisown = 1
    return val

def IconBundleFromIcon(*args, **kwargs):
    val = _gdi.new_IconBundleFromIcon(*args, **kwargs)
    val.thisown = 1
    return val

class Cursor(GDIObject):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Cursor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Cursor):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Ok(*args, **kwargs): return _gdi.Cursor_Ok(*args, **kwargs)
    def __nonzero__(self): return self.Ok() 
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCursor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class CursorPtr(Cursor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Cursor
_gdi.Cursor_swigregister(CursorPtr)

def StockCursor(*args, **kwargs):
    val = _gdi.new_StockCursor(*args, **kwargs)
    val.thisown = 1
    return val

def CursorFromImage(*args, **kwargs):
    val = _gdi.new_CursorFromImage(*args, **kwargs)
    val.thisown = 1
    return val

def CursorFromBits(*args, **kwargs):
    val = _gdi.new_CursorFromBits(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

OutRegion = _gdi.OutRegion
PartRegion = _gdi.PartRegion
InRegion = _gdi.InRegion
class Region(GDIObject):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Region(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Region):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Clear(*args, **kwargs): return _gdi.Region_Clear(*args, **kwargs)
    def Contains(*args, **kwargs): return _gdi.Region_Contains(*args, **kwargs)
    def ContainsPoint(*args, **kwargs): return _gdi.Region_ContainsPoint(*args, **kwargs)
    def ContainsRect(*args, **kwargs): return _gdi.Region_ContainsRect(*args, **kwargs)
    def ContainsRectDim(*args, **kwargs): return _gdi.Region_ContainsRectDim(*args, **kwargs)
    def GetBox(*args, **kwargs): return _gdi.Region_GetBox(*args, **kwargs)
    def Intersect(*args, **kwargs): return _gdi.Region_Intersect(*args, **kwargs)
    def IntersectRect(*args, **kwargs): return _gdi.Region_IntersectRect(*args, **kwargs)
    def IntersectRegion(*args, **kwargs): return _gdi.Region_IntersectRegion(*args, **kwargs)
    def IsEmpty(*args, **kwargs): return _gdi.Region_IsEmpty(*args, **kwargs)
    def Union(*args, **kwargs): return _gdi.Region_Union(*args, **kwargs)
    def UnionRect(*args, **kwargs): return _gdi.Region_UnionRect(*args, **kwargs)
    def UnionRegion(*args, **kwargs): return _gdi.Region_UnionRegion(*args, **kwargs)
    def Subtract(*args, **kwargs): return _gdi.Region_Subtract(*args, **kwargs)
    def SubtractRect(*args, **kwargs): return _gdi.Region_SubtractRect(*args, **kwargs)
    def SubtractRegion(*args, **kwargs): return _gdi.Region_SubtractRegion(*args, **kwargs)
    def Xor(*args, **kwargs): return _gdi.Region_Xor(*args, **kwargs)
    def XorRect(*args, **kwargs): return _gdi.Region_XorRect(*args, **kwargs)
    def XorRegion(*args, **kwargs): return _gdi.Region_XorRegion(*args, **kwargs)
    def ConvertToBitmap(*args, **kwargs): return _gdi.Region_ConvertToBitmap(*args, **kwargs)
    def UnionBitmap(*args, **kwargs): return _gdi.Region_UnionBitmap(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRegion instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class RegionPtr(Region):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Region
_gdi.Region_swigregister(RegionPtr)

def RegionFromBitmap(*args, **kwargs):
    val = _gdi.new_RegionFromBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def RegionFromPoints(*args, **kwargs):
    val = _gdi.new_RegionFromPoints(*args, **kwargs)
    val.thisown = 1
    return val

class RegionIterator(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_RegionIterator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_RegionIterator):
        try:
            if self.thisown: destroy(self)
        except: pass
    def GetX(*args, **kwargs): return _gdi.RegionIterator_GetX(*args, **kwargs)
    def GetY(*args, **kwargs): return _gdi.RegionIterator_GetY(*args, **kwargs)
    def GetW(*args, **kwargs): return _gdi.RegionIterator_GetW(*args, **kwargs)
    def GetWidth(*args, **kwargs): return _gdi.RegionIterator_GetWidth(*args, **kwargs)
    def GetH(*args, **kwargs): return _gdi.RegionIterator_GetH(*args, **kwargs)
    def GetHeight(*args, **kwargs): return _gdi.RegionIterator_GetHeight(*args, **kwargs)
    def GetRect(*args, **kwargs): return _gdi.RegionIterator_GetRect(*args, **kwargs)
    def HaveRects(*args, **kwargs): return _gdi.RegionIterator_HaveRects(*args, **kwargs)
    def Reset(*args, **kwargs): return _gdi.RegionIterator_Reset(*args, **kwargs)
    def Next(*args, **kwargs): return _gdi.RegionIterator_Next(*args, **kwargs)
    def __nonzero__(*args, **kwargs): return _gdi.RegionIterator___nonzero__(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRegionIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class RegionIteratorPtr(RegionIterator):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RegionIterator
_gdi.RegionIterator_swigregister(RegionIteratorPtr)

#---------------------------------------------------------------------------

FONTFAMILY_DEFAULT = _gdi.FONTFAMILY_DEFAULT
FONTFAMILY_DECORATIVE = _gdi.FONTFAMILY_DECORATIVE
FONTFAMILY_ROMAN = _gdi.FONTFAMILY_ROMAN
FONTFAMILY_SCRIPT = _gdi.FONTFAMILY_SCRIPT
FONTFAMILY_SWISS = _gdi.FONTFAMILY_SWISS
FONTFAMILY_MODERN = _gdi.FONTFAMILY_MODERN
FONTFAMILY_TELETYPE = _gdi.FONTFAMILY_TELETYPE
FONTFAMILY_MAX = _gdi.FONTFAMILY_MAX
FONTFAMILY_UNKNOWN = _gdi.FONTFAMILY_UNKNOWN
FONTSTYLE_NORMAL = _gdi.FONTSTYLE_NORMAL
FONTSTYLE_ITALIC = _gdi.FONTSTYLE_ITALIC
FONTSTYLE_SLANT = _gdi.FONTSTYLE_SLANT
FONTSTYLE_MAX = _gdi.FONTSTYLE_MAX
FONTWEIGHT_NORMAL = _gdi.FONTWEIGHT_NORMAL
FONTWEIGHT_LIGHT = _gdi.FONTWEIGHT_LIGHT
FONTWEIGHT_BOLD = _gdi.FONTWEIGHT_BOLD
FONTWEIGHT_MAX = _gdi.FONTWEIGHT_MAX
FONTFLAG_DEFAULT = _gdi.FONTFLAG_DEFAULT
FONTFLAG_ITALIC = _gdi.FONTFLAG_ITALIC
FONTFLAG_SLANT = _gdi.FONTFLAG_SLANT
FONTFLAG_LIGHT = _gdi.FONTFLAG_LIGHT
FONTFLAG_BOLD = _gdi.FONTFLAG_BOLD
FONTFLAG_ANTIALIASED = _gdi.FONTFLAG_ANTIALIASED
FONTFLAG_NOT_ANTIALIASED = _gdi.FONTFLAG_NOT_ANTIALIASED
FONTFLAG_UNDERLINED = _gdi.FONTFLAG_UNDERLINED
FONTFLAG_STRIKETHROUGH = _gdi.FONTFLAG_STRIKETHROUGH
FONTFLAG_MASK = _gdi.FONTFLAG_MASK
FONTENCODING_SYSTEM = _gdi.FONTENCODING_SYSTEM
FONTENCODING_DEFAULT = _gdi.FONTENCODING_DEFAULT
FONTENCODING_ISO8859_1 = _gdi.FONTENCODING_ISO8859_1
FONTENCODING_ISO8859_2 = _gdi.FONTENCODING_ISO8859_2
FONTENCODING_ISO8859_3 = _gdi.FONTENCODING_ISO8859_3
FONTENCODING_ISO8859_4 = _gdi.FONTENCODING_ISO8859_4
FONTENCODING_ISO8859_5 = _gdi.FONTENCODING_ISO8859_5
FONTENCODING_ISO8859_6 = _gdi.FONTENCODING_ISO8859_6
FONTENCODING_ISO8859_7 = _gdi.FONTENCODING_ISO8859_7
FONTENCODING_ISO8859_8 = _gdi.FONTENCODING_ISO8859_8
FONTENCODING_ISO8859_9 = _gdi.FONTENCODING_ISO8859_9
FONTENCODING_ISO8859_10 = _gdi.FONTENCODING_ISO8859_10
FONTENCODING_ISO8859_11 = _gdi.FONTENCODING_ISO8859_11
FONTENCODING_ISO8859_12 = _gdi.FONTENCODING_ISO8859_12
FONTENCODING_ISO8859_13 = _gdi.FONTENCODING_ISO8859_13
FONTENCODING_ISO8859_14 = _gdi.FONTENCODING_ISO8859_14
FONTENCODING_ISO8859_15 = _gdi.FONTENCODING_ISO8859_15
FONTENCODING_ISO8859_MAX = _gdi.FONTENCODING_ISO8859_MAX
FONTENCODING_KOI8 = _gdi.FONTENCODING_KOI8
FONTENCODING_ALTERNATIVE = _gdi.FONTENCODING_ALTERNATIVE
FONTENCODING_BULGARIAN = _gdi.FONTENCODING_BULGARIAN
FONTENCODING_CP437 = _gdi.FONTENCODING_CP437
FONTENCODING_CP850 = _gdi.FONTENCODING_CP850
FONTENCODING_CP852 = _gdi.FONTENCODING_CP852
FONTENCODING_CP855 = _gdi.FONTENCODING_CP855
FONTENCODING_CP866 = _gdi.FONTENCODING_CP866
FONTENCODING_CP874 = _gdi.FONTENCODING_CP874
FONTENCODING_CP932 = _gdi.FONTENCODING_CP932
FONTENCODING_CP936 = _gdi.FONTENCODING_CP936
FONTENCODING_CP949 = _gdi.FONTENCODING_CP949
FONTENCODING_CP950 = _gdi.FONTENCODING_CP950
FONTENCODING_CP1250 = _gdi.FONTENCODING_CP1250
FONTENCODING_CP1251 = _gdi.FONTENCODING_CP1251
FONTENCODING_CP1252 = _gdi.FONTENCODING_CP1252
FONTENCODING_CP1253 = _gdi.FONTENCODING_CP1253
FONTENCODING_CP1254 = _gdi.FONTENCODING_CP1254
FONTENCODING_CP1255 = _gdi.FONTENCODING_CP1255
FONTENCODING_CP1256 = _gdi.FONTENCODING_CP1256
FONTENCODING_CP1257 = _gdi.FONTENCODING_CP1257
FONTENCODING_CP12_MAX = _gdi.FONTENCODING_CP12_MAX
FONTENCODING_UTF7 = _gdi.FONTENCODING_UTF7
FONTENCODING_UTF8 = _gdi.FONTENCODING_UTF8
FONTENCODING_EUC_JP = _gdi.FONTENCODING_EUC_JP
FONTENCODING_UTF16BE = _gdi.FONTENCODING_UTF16BE
FONTENCODING_UTF16LE = _gdi.FONTENCODING_UTF16LE
FONTENCODING_UTF32BE = _gdi.FONTENCODING_UTF32BE
FONTENCODING_UTF32LE = _gdi.FONTENCODING_UTF32LE
FONTENCODING_MAX = _gdi.FONTENCODING_MAX
FONTENCODING_UTF16 = _gdi.FONTENCODING_UTF16
FONTENCODING_UTF32 = _gdi.FONTENCODING_UTF32
FONTENCODING_UNICODE = _gdi.FONTENCODING_UNICODE
FONTENCODING_GB2312 = _gdi.FONTENCODING_GB2312
FONTENCODING_BIG5 = _gdi.FONTENCODING_BIG5
FONTENCODING_SHIFT_JIS = _gdi.FONTENCODING_SHIFT_JIS
#---------------------------------------------------------------------------

class NativeFontInfo(object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_NativeFontInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_NativeFontInfo):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Init(*args, **kwargs): return _gdi.NativeFontInfo_Init(*args, **kwargs)
    def InitFromFont(*args, **kwargs): return _gdi.NativeFontInfo_InitFromFont(*args, **kwargs)
    def GetPointSize(*args, **kwargs): return _gdi.NativeFontInfo_GetPointSize(*args, **kwargs)
    def GetStyle(*args, **kwargs): return _gdi.NativeFontInfo_GetStyle(*args, **kwargs)
    def GetWeight(*args, **kwargs): return _gdi.NativeFontInfo_GetWeight(*args, **kwargs)
    def GetUnderlined(*args, **kwargs): return _gdi.NativeFontInfo_GetUnderlined(*args, **kwargs)
    def GetFaceName(*args, **kwargs): return _gdi.NativeFontInfo_GetFaceName(*args, **kwargs)
    def GetFamily(*args, **kwargs): return _gdi.NativeFontInfo_GetFamily(*args, **kwargs)
    def GetEncoding(*args, **kwargs): return _gdi.NativeFontInfo_GetEncoding(*args, **kwargs)
    def SetPointSize(*args, **kwargs): return _gdi.NativeFontInfo_SetPointSize(*args, **kwargs)
    def SetStyle(*args, **kwargs): return _gdi.NativeFontInfo_SetStyle(*args, **kwargs)
    def SetWeight(*args, **kwargs): return _gdi.NativeFontInfo_SetWeight(*args, **kwargs)
    def SetUnderlined(*args, **kwargs): return _gdi.NativeFontInfo_SetUnderlined(*args, **kwargs)
    def SetFaceName(*args, **kwargs): return _gdi.NativeFontInfo_SetFaceName(*args, **kwargs)
    def SetFamily(*args, **kwargs): return _gdi.NativeFontInfo_SetFamily(*args, **kwargs)
    def SetEncoding(*args, **kwargs): return _gdi.NativeFontInfo_SetEncoding(*args, **kwargs)
    def FromString(*args, **kwargs): return _gdi.NativeFontInfo_FromString(*args, **kwargs)
    def ToString(*args, **kwargs): return _gdi.NativeFontInfo_ToString(*args, **kwargs)
    def __str__(*args, **kwargs): return _gdi.NativeFontInfo___str__(*args, **kwargs)
    def FromUserString(*args, **kwargs): return _gdi.NativeFontInfo_FromUserString(*args, **kwargs)
    def ToUserString(*args, **kwargs): return _gdi.NativeFontInfo_ToUserString(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNativeFontInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class NativeFontInfoPtr(NativeFontInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NativeFontInfo
_gdi.NativeFontInfo_swigregister(NativeFontInfoPtr)

class NativeEncodingInfo(object):
    facename = property(_gdi.NativeEncodingInfo_facename_get, _gdi.NativeEncodingInfo_facename_set)
    encoding = property(_gdi.NativeEncodingInfo_encoding_get, _gdi.NativeEncodingInfo_encoding_set)
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_NativeEncodingInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_NativeEncodingInfo):
        try:
            if self.thisown: destroy(self)
        except: pass
    def FromString(*args, **kwargs): return _gdi.NativeEncodingInfo_FromString(*args, **kwargs)
    def ToString(*args, **kwargs): return _gdi.NativeEncodingInfo_ToString(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNativeEncodingInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class NativeEncodingInfoPtr(NativeEncodingInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NativeEncodingInfo
_gdi.NativeEncodingInfo_swigregister(NativeEncodingInfoPtr)


GetNativeFontEncoding = _gdi.GetNativeFontEncoding

TestFontEncoding = _gdi.TestFontEncoding
#---------------------------------------------------------------------------

class FontMapper(object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_FontMapper(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_FontMapper):
        try:
            if self.thisown: destroy(self)
        except: pass
    Get = staticmethod(_gdi.FontMapper_Get)
    Set = staticmethod(_gdi.FontMapper_Set)
    def CharsetToEncoding(*args, **kwargs): return _gdi.FontMapper_CharsetToEncoding(*args, **kwargs)
    GetSupportedEncodingsCount = staticmethod(_gdi.FontMapper_GetSupportedEncodingsCount)
    GetEncoding = staticmethod(_gdi.FontMapper_GetEncoding)
    GetEncodingName = staticmethod(_gdi.FontMapper_GetEncodingName)
    GetEncodingDescription = staticmethod(_gdi.FontMapper_GetEncodingDescription)
    def SetConfig(*args, **kwargs): return _gdi.FontMapper_SetConfig(*args, **kwargs)
    def SetConfigPath(*args, **kwargs): return _gdi.FontMapper_SetConfigPath(*args, **kwargs)
    GetDefaultConfigPath = staticmethod(_gdi.FontMapper_GetDefaultConfigPath)
    def GetAltForEncoding(*args, **kwargs): return _gdi.FontMapper_GetAltForEncoding(*args, **kwargs)
    def IsEncodingAvailable(*args, **kwargs): return _gdi.FontMapper_IsEncodingAvailable(*args, **kwargs)
    def SetDialogParent(*args, **kwargs): return _gdi.FontMapper_SetDialogParent(*args, **kwargs)
    def SetDialogTitle(*args, **kwargs): return _gdi.FontMapper_SetDialogTitle(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFontMapper instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class FontMapperPtr(FontMapper):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FontMapper
_gdi.FontMapper_swigregister(FontMapperPtr)

FontMapper_Get = _gdi.FontMapper_Get

FontMapper_Set = _gdi.FontMapper_Set

FontMapper_GetSupportedEncodingsCount = _gdi.FontMapper_GetSupportedEncodingsCount

FontMapper_GetEncoding = _gdi.FontMapper_GetEncoding

FontMapper_GetEncodingName = _gdi.FontMapper_GetEncodingName

FontMapper_GetEncodingDescription = _gdi.FontMapper_GetEncodingDescription

FontMapper_GetDefaultConfigPath = _gdi.FontMapper_GetDefaultConfigPath

#---------------------------------------------------------------------------

class Font(GDIObject):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Font(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Font):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Ok(*args, **kwargs): return _gdi.Font_Ok(*args, **kwargs)
    def __nonzero__(self): return self.Ok() 
    def __eq__(*args, **kwargs): return _gdi.Font___eq__(*args, **kwargs)
    def __ne__(*args, **kwargs): return _gdi.Font___ne__(*args, **kwargs)
    def GetPointSize(*args, **kwargs): return _gdi.Font_GetPointSize(*args, **kwargs)
    def GetFamily(*args, **kwargs): return _gdi.Font_GetFamily(*args, **kwargs)
    def GetStyle(*args, **kwargs): return _gdi.Font_GetStyle(*args, **kwargs)
    def GetWeight(*args, **kwargs): return _gdi.Font_GetWeight(*args, **kwargs)
    def GetUnderlined(*args, **kwargs): return _gdi.Font_GetUnderlined(*args, **kwargs)
    def GetFaceName(*args, **kwargs): return _gdi.Font_GetFaceName(*args, **kwargs)
    def GetEncoding(*args, **kwargs): return _gdi.Font_GetEncoding(*args, **kwargs)
    def GetNativeFontInfo(*args, **kwargs): return _gdi.Font_GetNativeFontInfo(*args, **kwargs)
    def IsFixedWidth(*args, **kwargs): return _gdi.Font_IsFixedWidth(*args, **kwargs)
    def GetNativeFontInfoDesc(*args, **kwargs): return _gdi.Font_GetNativeFontInfoDesc(*args, **kwargs)
    def GetNativeFontInfoUserDesc(*args, **kwargs): return _gdi.Font_GetNativeFontInfoUserDesc(*args, **kwargs)
    def SetPointSize(*args, **kwargs): return _gdi.Font_SetPointSize(*args, **kwargs)
    def SetFamily(*args, **kwargs): return _gdi.Font_SetFamily(*args, **kwargs)
    def SetStyle(*args, **kwargs): return _gdi.Font_SetStyle(*args, **kwargs)
    def SetWeight(*args, **kwargs): return _gdi.Font_SetWeight(*args, **kwargs)
    def SetFaceName(*args, **kwargs): return _gdi.Font_SetFaceName(*args, **kwargs)
    def SetUnderlined(*args, **kwargs): return _gdi.Font_SetUnderlined(*args, **kwargs)
    def SetEncoding(*args, **kwargs): return _gdi.Font_SetEncoding(*args, **kwargs)
    def SetNativeFontInfo(*args, **kwargs): return _gdi.Font_SetNativeFontInfo(*args, **kwargs)
    def SetNativeFontInfoFromString(*args, **kwargs): return _gdi.Font_SetNativeFontInfoFromString(*args, **kwargs)
    def SetNativeFontInfoUserDesc(*args, **kwargs): return _gdi.Font_SetNativeFontInfoUserDesc(*args, **kwargs)
    def GetFamilyString(*args, **kwargs): return _gdi.Font_GetFamilyString(*args, **kwargs)
    def GetStyleString(*args, **kwargs): return _gdi.Font_GetStyleString(*args, **kwargs)
    def GetWeightString(*args, **kwargs): return _gdi.Font_GetWeightString(*args, **kwargs)
    def SetNoAntiAliasing(*args, **kwargs): return _gdi.Font_SetNoAntiAliasing(*args, **kwargs)
    def GetNoAntiAliasing(*args, **kwargs): return _gdi.Font_GetNoAntiAliasing(*args, **kwargs)
    GetDefaultEncoding = staticmethod(_gdi.Font_GetDefaultEncoding)
    SetDefaultEncoding = staticmethod(_gdi.Font_SetDefaultEncoding)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFont instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class FontPtr(Font):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Font
_gdi.Font_swigregister(FontPtr)

def FontFromNativeInfo(*args, **kwargs):
    val = _gdi.new_FontFromNativeInfo(*args, **kwargs)
    val.thisown = 1
    return val

def FontFromNativeInfoString(*args, **kwargs):
    val = _gdi.new_FontFromNativeInfoString(*args, **kwargs)
    val.thisown = 1
    return val

def Font2(*args, **kwargs):
    val = _gdi.new_Font2(*args, **kwargs)
    val.thisown = 1
    return val

Font_GetDefaultEncoding = _gdi.Font_GetDefaultEncoding

Font_SetDefaultEncoding = _gdi.Font_SetDefaultEncoding

#---------------------------------------------------------------------------

class FontEnumerator(object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_FontEnumerator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, FontEnumerator, 0)
    def __del__(self, destroy=_gdi.delete_FontEnumerator):
        try:
            if self.thisown: destroy(self)
        except: pass
    def _setCallbackInfo(*args, **kwargs): return _gdi.FontEnumerator__setCallbackInfo(*args, **kwargs)
    def EnumerateFacenames(*args, **kwargs): return _gdi.FontEnumerator_EnumerateFacenames(*args, **kwargs)
    def EnumerateEncodings(*args, **kwargs): return _gdi.FontEnumerator_EnumerateEncodings(*args, **kwargs)
    def GetEncodings(*args, **kwargs): return _gdi.FontEnumerator_GetEncodings(*args, **kwargs)
    def GetFacenames(*args, **kwargs): return _gdi.FontEnumerator_GetFacenames(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyFontEnumerator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class FontEnumeratorPtr(FontEnumerator):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FontEnumerator
_gdi.FontEnumerator_swigregister(FontEnumeratorPtr)

#---------------------------------------------------------------------------

LANGUAGE_DEFAULT = _gdi.LANGUAGE_DEFAULT
LANGUAGE_UNKNOWN = _gdi.LANGUAGE_UNKNOWN
LANGUAGE_ABKHAZIAN = _gdi.LANGUAGE_ABKHAZIAN
LANGUAGE_AFAR = _gdi.LANGUAGE_AFAR
LANGUAGE_AFRIKAANS = _gdi.LANGUAGE_AFRIKAANS
LANGUAGE_ALBANIAN = _gdi.LANGUAGE_ALBANIAN
LANGUAGE_AMHARIC = _gdi.LANGUAGE_AMHARIC
LANGUAGE_ARABIC = _gdi.LANGUAGE_ARABIC
LANGUAGE_ARABIC_ALGERIA = _gdi.LANGUAGE_ARABIC_ALGERIA
LANGUAGE_ARABIC_BAHRAIN = _gdi.LANGUAGE_ARABIC_BAHRAIN
LANGUAGE_ARABIC_EGYPT = _gdi.LANGUAGE_ARABIC_EGYPT
LANGUAGE_ARABIC_IRAQ = _gdi.LANGUAGE_ARABIC_IRAQ
LANGUAGE_ARABIC_JORDAN = _gdi.LANGUAGE_ARABIC_JORDAN
LANGUAGE_ARABIC_KUWAIT = _gdi.LANGUAGE_ARABIC_KUWAIT
LANGUAGE_ARABIC_LEBANON = _gdi.LANGUAGE_ARABIC_LEBANON
LANGUAGE_ARABIC_LIBYA = _gdi.LANGUAGE_ARABIC_LIBYA
LANGUAGE_ARABIC_MOROCCO = _gdi.LANGUAGE_ARABIC_MOROCCO
LANGUAGE_ARABIC_OMAN = _gdi.LANGUAGE_ARABIC_OMAN
LANGUAGE_ARABIC_QATAR = _gdi.LANGUAGE_ARABIC_QATAR
LANGUAGE_ARABIC_SAUDI_ARABIA = _gdi.LANGUAGE_ARABIC_SAUDI_ARABIA
LANGUAGE_ARABIC_SUDAN = _gdi.LANGUAGE_ARABIC_SUDAN
LANGUAGE_ARABIC_SYRIA = _gdi.LANGUAGE_ARABIC_SYRIA
LANGUAGE_ARABIC_TUNISIA = _gdi.LANGUAGE_ARABIC_TUNISIA
LANGUAGE_ARABIC_UAE = _gdi.LANGUAGE_ARABIC_UAE
LANGUAGE_ARABIC_YEMEN = _gdi.LANGUAGE_ARABIC_YEMEN
LANGUAGE_ARMENIAN = _gdi.LANGUAGE_ARMENIAN
LANGUAGE_ASSAMESE = _gdi.LANGUAGE_ASSAMESE
LANGUAGE_AYMARA = _gdi.LANGUAGE_AYMARA
LANGUAGE_AZERI = _gdi.LANGUAGE_AZERI
LANGUAGE_AZERI_CYRILLIC = _gdi.LANGUAGE_AZERI_CYRILLIC
LANGUAGE_AZERI_LATIN = _gdi.LANGUAGE_AZERI_LATIN
LANGUAGE_BASHKIR = _gdi.LANGUAGE_BASHKIR
LANGUAGE_BASQUE = _gdi.LANGUAGE_BASQUE
LANGUAGE_BELARUSIAN = _gdi.LANGUAGE_BELARUSIAN
LANGUAGE_BENGALI = _gdi.LANGUAGE_BENGALI
LANGUAGE_BHUTANI = _gdi.LANGUAGE_BHUTANI
LANGUAGE_BIHARI = _gdi.LANGUAGE_BIHARI
LANGUAGE_BISLAMA = _gdi.LANGUAGE_BISLAMA
LANGUAGE_BRETON = _gdi.LANGUAGE_BRETON
LANGUAGE_BULGARIAN = _gdi.LANGUAGE_BULGARIAN
LANGUAGE_BURMESE = _gdi.LANGUAGE_BURMESE
LANGUAGE_CAMBODIAN = _gdi.LANGUAGE_CAMBODIAN
LANGUAGE_CATALAN = _gdi.LANGUAGE_CATALAN
LANGUAGE_CHINESE = _gdi.LANGUAGE_CHINESE
LANGUAGE_CHINESE_SIMPLIFIED = _gdi.LANGUAGE_CHINESE_SIMPLIFIED
LANGUAGE_CHINESE_TRADITIONAL = _gdi.LANGUAGE_CHINESE_TRADITIONAL
LANGUAGE_CHINESE_HONGKONG = _gdi.LANGUAGE_CHINESE_HONGKONG
LANGUAGE_CHINESE_MACAU = _gdi.LANGUAGE_CHINESE_MACAU
LANGUAGE_CHINESE_SINGAPORE = _gdi.LANGUAGE_CHINESE_SINGAPORE
LANGUAGE_CHINESE_TAIWAN = _gdi.LANGUAGE_CHINESE_TAIWAN
LANGUAGE_CORSICAN = _gdi.LANGUAGE_CORSICAN
LANGUAGE_CROATIAN = _gdi.LANGUAGE_CROATIAN
LANGUAGE_CZECH = _gdi.LANGUAGE_CZECH
LANGUAGE_DANISH = _gdi.LANGUAGE_DANISH
LANGUAGE_DUTCH = _gdi.LANGUAGE_DUTCH
LANGUAGE_DUTCH_BELGIAN = _gdi.LANGUAGE_DUTCH_BELGIAN
LANGUAGE_ENGLISH = _gdi.LANGUAGE_ENGLISH
LANGUAGE_ENGLISH_UK = _gdi.LANGUAGE_ENGLISH_UK
LANGUAGE_ENGLISH_US = _gdi.LANGUAGE_ENGLISH_US
LANGUAGE_ENGLISH_AUSTRALIA = _gdi.LANGUAGE_ENGLISH_AUSTRALIA
LANGUAGE_ENGLISH_BELIZE = _gdi.LANGUAGE_ENGLISH_BELIZE
LANGUAGE_ENGLISH_BOTSWANA = _gdi.LANGUAGE_ENGLISH_BOTSWANA
LANGUAGE_ENGLISH_CANADA = _gdi.LANGUAGE_ENGLISH_CANADA
LANGUAGE_ENGLISH_CARIBBEAN = _gdi.LANGUAGE_ENGLISH_CARIBBEAN
LANGUAGE_ENGLISH_DENMARK = _gdi.LANGUAGE_ENGLISH_DENMARK
LANGUAGE_ENGLISH_EIRE = _gdi.LANGUAGE_ENGLISH_EIRE
LANGUAGE_ENGLISH_JAMAICA = _gdi.LANGUAGE_ENGLISH_JAMAICA
LANGUAGE_ENGLISH_NEW_ZEALAND = _gdi.LANGUAGE_ENGLISH_NEW_ZEALAND
LANGUAGE_ENGLISH_PHILIPPINES = _gdi.LANGUAGE_ENGLISH_PHILIPPINES
LANGUAGE_ENGLISH_SOUTH_AFRICA = _gdi.LANGUAGE_ENGLISH_SOUTH_AFRICA
LANGUAGE_ENGLISH_TRINIDAD = _gdi.LANGUAGE_ENGLISH_TRINIDAD
LANGUAGE_ENGLISH_ZIMBABWE = _gdi.LANGUAGE_ENGLISH_ZIMBABWE
LANGUAGE_ESPERANTO = _gdi.LANGUAGE_ESPERANTO
LANGUAGE_ESTONIAN = _gdi.LANGUAGE_ESTONIAN
LANGUAGE_FAEROESE = _gdi.LANGUAGE_FAEROESE
LANGUAGE_FARSI = _gdi.LANGUAGE_FARSI
LANGUAGE_FIJI = _gdi.LANGUAGE_FIJI
LANGUAGE_FINNISH = _gdi.LANGUAGE_FINNISH
LANGUAGE_FRENCH = _gdi.LANGUAGE_FRENCH
LANGUAGE_FRENCH_BELGIAN = _gdi.LANGUAGE_FRENCH_BELGIAN
LANGUAGE_FRENCH_CANADIAN = _gdi.LANGUAGE_FRENCH_CANADIAN
LANGUAGE_FRENCH_LUXEMBOURG = _gdi.LANGUAGE_FRENCH_LUXEMBOURG
LANGUAGE_FRENCH_MONACO = _gdi.LANGUAGE_FRENCH_MONACO
LANGUAGE_FRENCH_SWISS = _gdi.LANGUAGE_FRENCH_SWISS
LANGUAGE_FRISIAN = _gdi.LANGUAGE_FRISIAN
LANGUAGE_GALICIAN = _gdi.LANGUAGE_GALICIAN
LANGUAGE_GEORGIAN = _gdi.LANGUAGE_GEORGIAN
LANGUAGE_GERMAN = _gdi.LANGUAGE_GERMAN
LANGUAGE_GERMAN_AUSTRIAN = _gdi.LANGUAGE_GERMAN_AUSTRIAN
LANGUAGE_GERMAN_BELGIUM = _gdi.LANGUAGE_GERMAN_BELGIUM
LANGUAGE_GERMAN_LIECHTENSTEIN = _gdi.LANGUAGE_GERMAN_LIECHTENSTEIN
LANGUAGE_GERMAN_LUXEMBOURG = _gdi.LANGUAGE_GERMAN_LUXEMBOURG
LANGUAGE_GERMAN_SWISS = _gdi.LANGUAGE_GERMAN_SWISS
LANGUAGE_GREEK = _gdi.LANGUAGE_GREEK
LANGUAGE_GREENLANDIC = _gdi.LANGUAGE_GREENLANDIC
LANGUAGE_GUARANI = _gdi.LANGUAGE_GUARANI
LANGUAGE_GUJARATI = _gdi.LANGUAGE_GUJARATI
LANGUAGE_HAUSA = _gdi.LANGUAGE_HAUSA
LANGUAGE_HEBREW = _gdi.LANGUAGE_HEBREW
LANGUAGE_HINDI = _gdi.LANGUAGE_HINDI
LANGUAGE_HUNGARIAN = _gdi.LANGUAGE_HUNGARIAN
LANGUAGE_ICELANDIC = _gdi.LANGUAGE_ICELANDIC
LANGUAGE_INDONESIAN = _gdi.LANGUAGE_INDONESIAN
LANGUAGE_INTERLINGUA = _gdi.LANGUAGE_INTERLINGUA
LANGUAGE_INTERLINGUE = _gdi.LANGUAGE_INTERLINGUE
LANGUAGE_INUKTITUT = _gdi.LANGUAGE_INUKTITUT
LANGUAGE_INUPIAK = _gdi.LANGUAGE_INUPIAK
LANGUAGE_IRISH = _gdi.LANGUAGE_IRISH
LANGUAGE_ITALIAN = _gdi.LANGUAGE_ITALIAN
LANGUAGE_ITALIAN_SWISS = _gdi.LANGUAGE_ITALIAN_SWISS
LANGUAGE_JAPANESE = _gdi.LANGUAGE_JAPANESE
LANGUAGE_JAVANESE = _gdi.LANGUAGE_JAVANESE
LANGUAGE_KANNADA = _gdi.LANGUAGE_KANNADA
LANGUAGE_KASHMIRI = _gdi.LANGUAGE_KASHMIRI
LANGUAGE_KASHMIRI_INDIA = _gdi.LANGUAGE_KASHMIRI_INDIA
LANGUAGE_KAZAKH = _gdi.LANGUAGE_KAZAKH
LANGUAGE_KERNEWEK = _gdi.LANGUAGE_KERNEWEK
LANGUAGE_KINYARWANDA = _gdi.LANGUAGE_KINYARWANDA
LANGUAGE_KIRGHIZ = _gdi.LANGUAGE_KIRGHIZ
LANGUAGE_KIRUNDI = _gdi.LANGUAGE_KIRUNDI
LANGUAGE_KONKANI = _gdi.LANGUAGE_KONKANI
LANGUAGE_KOREAN = _gdi.LANGUAGE_KOREAN
LANGUAGE_KURDISH = _gdi.LANGUAGE_KURDISH
LANGUAGE_LAOTHIAN = _gdi.LANGUAGE_LAOTHIAN
LANGUAGE_LATIN = _gdi.LANGUAGE_LATIN
LANGUAGE_LATVIAN = _gdi.LANGUAGE_LATVIAN
LANGUAGE_LINGALA = _gdi.LANGUAGE_LINGALA
LANGUAGE_LITHUANIAN = _gdi.LANGUAGE_LITHUANIAN
LANGUAGE_MACEDONIAN = _gdi.LANGUAGE_MACEDONIAN
LANGUAGE_MALAGASY = _gdi.LANGUAGE_MALAGASY
LANGUAGE_MALAY = _gdi.LANGUAGE_MALAY
LANGUAGE_MALAYALAM = _gdi.LANGUAGE_MALAYALAM
LANGUAGE_MALAY_BRUNEI_DARUSSALAM = _gdi.LANGUAGE_MALAY_BRUNEI_DARUSSALAM
LANGUAGE_MALAY_MALAYSIA = _gdi.LANGUAGE_MALAY_MALAYSIA
LANGUAGE_MALTESE = _gdi.LANGUAGE_MALTESE
LANGUAGE_MANIPURI = _gdi.LANGUAGE_MANIPURI
LANGUAGE_MAORI = _gdi.LANGUAGE_MAORI
LANGUAGE_MARATHI = _gdi.LANGUAGE_MARATHI
LANGUAGE_MOLDAVIAN = _gdi.LANGUAGE_MOLDAVIAN
LANGUAGE_MONGOLIAN = _gdi.LANGUAGE_MONGOLIAN
LANGUAGE_NAURU = _gdi.LANGUAGE_NAURU
LANGUAGE_NEPALI = _gdi.LANGUAGE_NEPALI
LANGUAGE_NEPALI_INDIA = _gdi.LANGUAGE_NEPALI_INDIA
LANGUAGE_NORWEGIAN_BOKMAL = _gdi.LANGUAGE_NORWEGIAN_BOKMAL
LANGUAGE_NORWEGIAN_NYNORSK = _gdi.LANGUAGE_NORWEGIAN_NYNORSK
LANGUAGE_OCCITAN = _gdi.LANGUAGE_OCCITAN
LANGUAGE_ORIYA = _gdi.LANGUAGE_ORIYA
LANGUAGE_OROMO = _gdi.LANGUAGE_OROMO
LANGUAGE_PASHTO = _gdi.LANGUAGE_PASHTO
LANGUAGE_POLISH = _gdi.LANGUAGE_POLISH
LANGUAGE_PORTUGUESE = _gdi.LANGUAGE_PORTUGUESE
LANGUAGE_PORTUGUESE_BRAZILIAN = _gdi.LANGUAGE_PORTUGUESE_BRAZILIAN
LANGUAGE_PUNJABI = _gdi.LANGUAGE_PUNJABI
LANGUAGE_QUECHUA = _gdi.LANGUAGE_QUECHUA
LANGUAGE_RHAETO_ROMANCE = _gdi.LANGUAGE_RHAETO_ROMANCE
LANGUAGE_ROMANIAN = _gdi.LANGUAGE_ROMANIAN
LANGUAGE_RUSSIAN = _gdi.LANGUAGE_RUSSIAN
LANGUAGE_RUSSIAN_UKRAINE = _gdi.LANGUAGE_RUSSIAN_UKRAINE
LANGUAGE_SAMOAN = _gdi.LANGUAGE_SAMOAN
LANGUAGE_SANGHO = _gdi.LANGUAGE_SANGHO
LANGUAGE_SANSKRIT = _gdi.LANGUAGE_SANSKRIT
LANGUAGE_SCOTS_GAELIC = _gdi.LANGUAGE_SCOTS_GAELIC
LANGUAGE_SERBIAN = _gdi.LANGUAGE_SERBIAN
LANGUAGE_SERBIAN_CYRILLIC = _gdi.LANGUAGE_SERBIAN_CYRILLIC
LANGUAGE_SERBIAN_LATIN = _gdi.LANGUAGE_SERBIAN_LATIN
LANGUAGE_SERBO_CROATIAN = _gdi.LANGUAGE_SERBO_CROATIAN
LANGUAGE_SESOTHO = _gdi.LANGUAGE_SESOTHO
LANGUAGE_SETSWANA = _gdi.LANGUAGE_SETSWANA
LANGUAGE_SHONA = _gdi.LANGUAGE_SHONA
LANGUAGE_SINDHI = _gdi.LANGUAGE_SINDHI
LANGUAGE_SINHALESE = _gdi.LANGUAGE_SINHALESE
LANGUAGE_SISWATI = _gdi.LANGUAGE_SISWATI
LANGUAGE_SLOVAK = _gdi.LANGUAGE_SLOVAK
LANGUAGE_SLOVENIAN = _gdi.LANGUAGE_SLOVENIAN
LANGUAGE_SOMALI = _gdi.LANGUAGE_SOMALI
LANGUAGE_SPANISH = _gdi.LANGUAGE_SPANISH
LANGUAGE_SPANISH_ARGENTINA = _gdi.LANGUAGE_SPANISH_ARGENTINA
LANGUAGE_SPANISH_BOLIVIA = _gdi.LANGUAGE_SPANISH_BOLIVIA
LANGUAGE_SPANISH_CHILE = _gdi.LANGUAGE_SPANISH_CHILE
LANGUAGE_SPANISH_COLOMBIA = _gdi.LANGUAGE_SPANISH_COLOMBIA
LANGUAGE_SPANISH_COSTA_RICA = _gdi.LANGUAGE_SPANISH_COSTA_RICA
LANGUAGE_SPANISH_DOMINICAN_REPUBLIC = _gdi.LANGUAGE_SPANISH_DOMINICAN_REPUBLIC
LANGUAGE_SPANISH_ECUADOR = _gdi.LANGUAGE_SPANISH_ECUADOR
LANGUAGE_SPANISH_EL_SALVADOR = _gdi.LANGUAGE_SPANISH_EL_SALVADOR
LANGUAGE_SPANISH_GUATEMALA = _gdi.LANGUAGE_SPANISH_GUATEMALA
LANGUAGE_SPANISH_HONDURAS = _gdi.LANGUAGE_SPANISH_HONDURAS
LANGUAGE_SPANISH_MEXICAN = _gdi.LANGUAGE_SPANISH_MEXICAN
LANGUAGE_SPANISH_MODERN = _gdi.LANGUAGE_SPANISH_MODERN
LANGUAGE_SPANISH_NICARAGUA = _gdi.LANGUAGE_SPANISH_NICARAGUA
LANGUAGE_SPANISH_PANAMA = _gdi.LANGUAGE_SPANISH_PANAMA
LANGUAGE_SPANISH_PARAGUAY = _gdi.LANGUAGE_SPANISH_PARAGUAY
LANGUAGE_SPANISH_PERU = _gdi.LANGUAGE_SPANISH_PERU
LANGUAGE_SPANISH_PUERTO_RICO = _gdi.LANGUAGE_SPANISH_PUERTO_RICO
LANGUAGE_SPANISH_URUGUAY = _gdi.LANGUAGE_SPANISH_URUGUAY
LANGUAGE_SPANISH_US = _gdi.LANGUAGE_SPANISH_US
LANGUAGE_SPANISH_VENEZUELA = _gdi.LANGUAGE_SPANISH_VENEZUELA
LANGUAGE_SUNDANESE = _gdi.LANGUAGE_SUNDANESE
LANGUAGE_SWAHILI = _gdi.LANGUAGE_SWAHILI
LANGUAGE_SWEDISH = _gdi.LANGUAGE_SWEDISH
LANGUAGE_SWEDISH_FINLAND = _gdi.LANGUAGE_SWEDISH_FINLAND
LANGUAGE_TAGALOG = _gdi.LANGUAGE_TAGALOG
LANGUAGE_TAJIK = _gdi.LANGUAGE_TAJIK
LANGUAGE_TAMIL = _gdi.LANGUAGE_TAMIL
LANGUAGE_TATAR = _gdi.LANGUAGE_TATAR
LANGUAGE_TELUGU = _gdi.LANGUAGE_TELUGU
LANGUAGE_THAI = _gdi.LANGUAGE_THAI
LANGUAGE_TIBETAN = _gdi.LANGUAGE_TIBETAN
LANGUAGE_TIGRINYA = _gdi.LANGUAGE_TIGRINYA
LANGUAGE_TONGA = _gdi.LANGUAGE_TONGA
LANGUAGE_TSONGA = _gdi.LANGUAGE_TSONGA
LANGUAGE_TURKISH = _gdi.LANGUAGE_TURKISH
LANGUAGE_TURKMEN = _gdi.LANGUAGE_TURKMEN
LANGUAGE_TWI = _gdi.LANGUAGE_TWI
LANGUAGE_UIGHUR = _gdi.LANGUAGE_UIGHUR
LANGUAGE_UKRAINIAN = _gdi.LANGUAGE_UKRAINIAN
LANGUAGE_URDU = _gdi.LANGUAGE_URDU
LANGUAGE_URDU_INDIA = _gdi.LANGUAGE_URDU_INDIA
LANGUAGE_URDU_PAKISTAN = _gdi.LANGUAGE_URDU_PAKISTAN
LANGUAGE_UZBEK = _gdi.LANGUAGE_UZBEK
LANGUAGE_UZBEK_CYRILLIC = _gdi.LANGUAGE_UZBEK_CYRILLIC
LANGUAGE_UZBEK_LATIN = _gdi.LANGUAGE_UZBEK_LATIN
LANGUAGE_VIETNAMESE = _gdi.LANGUAGE_VIETNAMESE
LANGUAGE_VOLAPUK = _gdi.LANGUAGE_VOLAPUK
LANGUAGE_WELSH = _gdi.LANGUAGE_WELSH
LANGUAGE_WOLOF = _gdi.LANGUAGE_WOLOF
LANGUAGE_XHOSA = _gdi.LANGUAGE_XHOSA
LANGUAGE_YIDDISH = _gdi.LANGUAGE_YIDDISH
LANGUAGE_YORUBA = _gdi.LANGUAGE_YORUBA
LANGUAGE_ZHUANG = _gdi.LANGUAGE_ZHUANG
LANGUAGE_ZULU = _gdi.LANGUAGE_ZULU
LANGUAGE_USER_DEFINED = _gdi.LANGUAGE_USER_DEFINED
class LanguageInfo(object):
    Language = property(_gdi.LanguageInfo_Language_get, _gdi.LanguageInfo_Language_set)
    CanonicalName = property(_gdi.LanguageInfo_CanonicalName_get, _gdi.LanguageInfo_CanonicalName_set)
    Description = property(_gdi.LanguageInfo_Description_get, _gdi.LanguageInfo_Description_set)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxLanguageInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class LanguageInfoPtr(LanguageInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = LanguageInfo
_gdi.LanguageInfo_swigregister(LanguageInfoPtr)

LOCALE_CAT_NUMBER = _gdi.LOCALE_CAT_NUMBER
LOCALE_CAT_DATE = _gdi.LOCALE_CAT_DATE
LOCALE_CAT_MONEY = _gdi.LOCALE_CAT_MONEY
LOCALE_CAT_MAX = _gdi.LOCALE_CAT_MAX
LOCALE_THOUSANDS_SEP = _gdi.LOCALE_THOUSANDS_SEP
LOCALE_DECIMAL_POINT = _gdi.LOCALE_DECIMAL_POINT
LOCALE_LOAD_DEFAULT = _gdi.LOCALE_LOAD_DEFAULT
LOCALE_CONV_ENCODING = _gdi.LOCALE_CONV_ENCODING
class Locale(object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Locale(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Locale):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Init1(*args, **kwargs): return _gdi.Locale_Init1(*args, **kwargs)
    def Init2(*args, **kwargs): return _gdi.Locale_Init2(*args, **kwargs)
    def Init(self, *_args, **_kwargs):
        if type(_args[0]) in [type(''), type(u'')]:
            val = self.Init1(*_args, **_kwargs)
        else:
            val = self.Init2(*_args, **_kwargs)
        return val

    GetSystemLanguage = staticmethod(_gdi.Locale_GetSystemLanguage)
    GetSystemEncoding = staticmethod(_gdi.Locale_GetSystemEncoding)
    GetSystemEncodingName = staticmethod(_gdi.Locale_GetSystemEncodingName)
    def IsOk(*args, **kwargs): return _gdi.Locale_IsOk(*args, **kwargs)
    def __nonzero__(self): return self.IsOk() 
    def GetLocale(*args, **kwargs): return _gdi.Locale_GetLocale(*args, **kwargs)
    def GetLanguage(*args, **kwargs): return _gdi.Locale_GetLanguage(*args, **kwargs)
    def GetSysName(*args, **kwargs): return _gdi.Locale_GetSysName(*args, **kwargs)
    def GetCanonicalName(*args, **kwargs): return _gdi.Locale_GetCanonicalName(*args, **kwargs)
    AddCatalogLookupPathPrefix = staticmethod(_gdi.Locale_AddCatalogLookupPathPrefix)
    def AddCatalog(*args, **kwargs): return _gdi.Locale_AddCatalog(*args, **kwargs)
    def IsLoaded(*args, **kwargs): return _gdi.Locale_IsLoaded(*args, **kwargs)
    GetLanguageInfo = staticmethod(_gdi.Locale_GetLanguageInfo)
    GetLanguageName = staticmethod(_gdi.Locale_GetLanguageName)
    FindLanguageInfo = staticmethod(_gdi.Locale_FindLanguageInfo)
    AddLanguage = staticmethod(_gdi.Locale_AddLanguage)
    def GetString(*args, **kwargs): return _gdi.Locale_GetString(*args, **kwargs)
    def GetName(*args, **kwargs): return _gdi.Locale_GetName(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxLocale instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class LocalePtr(Locale):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Locale
_gdi.Locale_swigregister(LocalePtr)

Locale_GetSystemLanguage = _gdi.Locale_GetSystemLanguage

Locale_GetSystemEncoding = _gdi.Locale_GetSystemEncoding

Locale_GetSystemEncodingName = _gdi.Locale_GetSystemEncodingName

Locale_AddCatalogLookupPathPrefix = _gdi.Locale_AddCatalogLookupPathPrefix

Locale_GetLanguageInfo = _gdi.Locale_GetLanguageInfo

Locale_GetLanguageName = _gdi.Locale_GetLanguageName

Locale_FindLanguageInfo = _gdi.Locale_FindLanguageInfo

Locale_AddLanguage = _gdi.Locale_AddLanguage


GetLocale = _gdi.GetLocale
#---------------------------------------------------------------------------

CONVERT_STRICT = _gdi.CONVERT_STRICT
CONVERT_SUBSTITUTE = _gdi.CONVERT_SUBSTITUTE
PLATFORM_CURRENT = _gdi.PLATFORM_CURRENT
PLATFORM_UNIX = _gdi.PLATFORM_UNIX
PLATFORM_WINDOWS = _gdi.PLATFORM_WINDOWS
PLATFORM_OS2 = _gdi.PLATFORM_OS2
PLATFORM_MAC = _gdi.PLATFORM_MAC
class EncodingConverter(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_EncodingConverter(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_EncodingConverter):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Init(*args, **kwargs): return _gdi.EncodingConverter_Init(*args, **kwargs)
    def Convert(*args, **kwargs): return _gdi.EncodingConverter_Convert(*args, **kwargs)
    GetPlatformEquivalents = staticmethod(_gdi.EncodingConverter_GetPlatformEquivalents)
    GetAllEquivalents = staticmethod(_gdi.EncodingConverter_GetAllEquivalents)
    CanConvert = staticmethod(_gdi.EncodingConverter_CanConvert)
    def __nonzero__(self): return self.IsOk() 
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEncodingConverter instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class EncodingConverterPtr(EncodingConverter):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EncodingConverter
_gdi.EncodingConverter_swigregister(EncodingConverterPtr)

GetTranslation = _gdi.GetTranslation

EncodingConverter_GetPlatformEquivalents = _gdi.EncodingConverter_GetPlatformEquivalents

EncodingConverter_GetAllEquivalents = _gdi.EncodingConverter_GetAllEquivalents

EncodingConverter_CanConvert = _gdi.EncodingConverter_CanConvert

#----------------------------------------------------------------------------
# wxGTK sets the locale when initialized.  Doing this at the Python
# level should set it up to match what GTK is doing at the C level.
if wx.Platform == "__WXGTK__":
    try:
        import locale
        locale.setlocale(locale.LC_ALL, "")
    except:
        pass

# On MSW add the directory where the wxWindows catalogs were installed
# to the default catalog path.
if wx.Platform == "__WXMSW__":
    import os
    localedir = os.path.join(os.path.split(__file__)[0], "locale")
    Locale_AddCatalogLookupPathPrefix(localedir)
    del os

#----------------------------------------------------------------------------

#---------------------------------------------------------------------------

class DC(core.Object):
    def __del__(self, destroy=_gdi.delete_DC):
        try:
            if self.thisown: destroy(self)
        except: pass
    def BeginDrawing(*args, **kwargs): return _gdi.DC_BeginDrawing(*args, **kwargs)
    def EndDrawing(*args, **kwargs): return _gdi.DC_EndDrawing(*args, **kwargs)
    def FloodFillXY(*args, **kwargs): return _gdi.DC_FloodFillXY(*args, **kwargs)
    def FloodFill(*args, **kwargs): return _gdi.DC_FloodFill(*args, **kwargs)
    def GetPixelXY(*args, **kwargs): return _gdi.DC_GetPixelXY(*args, **kwargs)
    def GetPixel(*args, **kwargs): return _gdi.DC_GetPixel(*args, **kwargs)
    def DrawLineXY(*args, **kwargs): return _gdi.DC_DrawLineXY(*args, **kwargs)
    def DrawLine(*args, **kwargs): return _gdi.DC_DrawLine(*args, **kwargs)
    def CrossHairXY(*args, **kwargs): return _gdi.DC_CrossHairXY(*args, **kwargs)
    def CrossHair(*args, **kwargs): return _gdi.DC_CrossHair(*args, **kwargs)
    def DrawArcXY(*args, **kwargs): return _gdi.DC_DrawArcXY(*args, **kwargs)
    def DrawArc(*args, **kwargs): return _gdi.DC_DrawArc(*args, **kwargs)
    def DrawCheckMarkXY(*args, **kwargs): return _gdi.DC_DrawCheckMarkXY(*args, **kwargs)
    def DrawCheckMark(*args, **kwargs): return _gdi.DC_DrawCheckMark(*args, **kwargs)
    def DrawEllipticArcXY(*args, **kwargs): return _gdi.DC_DrawEllipticArcXY(*args, **kwargs)
    def DrawEllipticArc(*args, **kwargs): return _gdi.DC_DrawEllipticArc(*args, **kwargs)
    def DrawPointXY(*args, **kwargs): return _gdi.DC_DrawPointXY(*args, **kwargs)
    def DrawPoint(*args, **kwargs): return _gdi.DC_DrawPoint(*args, **kwargs)
    def DrawRectangleXY(*args, **kwargs): return _gdi.DC_DrawRectangleXY(*args, **kwargs)
    def DrawRectangle(*args, **kwargs): return _gdi.DC_DrawRectangle(*args, **kwargs)
    def DrawRectangleRect(*args, **kwargs): return _gdi.DC_DrawRectangleRect(*args, **kwargs)
    def DrawRoundedRectangleXY(*args, **kwargs): return _gdi.DC_DrawRoundedRectangleXY(*args, **kwargs)
    def DrawRoundedRectangle(*args, **kwargs): return _gdi.DC_DrawRoundedRectangle(*args, **kwargs)
    def DrawRoundedRectangleRect(*args, **kwargs): return _gdi.DC_DrawRoundedRectangleRect(*args, **kwargs)
    def DrawCircleXY(*args, **kwargs): return _gdi.DC_DrawCircleXY(*args, **kwargs)
    def DrawCircle(*args, **kwargs): return _gdi.DC_DrawCircle(*args, **kwargs)
    def DrawEllipseXY(*args, **kwargs): return _gdi.DC_DrawEllipseXY(*args, **kwargs)
    def DrawEllipse(*args, **kwargs): return _gdi.DC_DrawEllipse(*args, **kwargs)
    def DrawEllipseRect(*args, **kwargs): return _gdi.DC_DrawEllipseRect(*args, **kwargs)
    def DrawIconXY(*args, **kwargs): return _gdi.DC_DrawIconXY(*args, **kwargs)
    def DrawIcon(*args, **kwargs): return _gdi.DC_DrawIcon(*args, **kwargs)
    def DrawBitmapXY(*args, **kwargs): return _gdi.DC_DrawBitmapXY(*args, **kwargs)
    def DrawBitmap(*args, **kwargs): return _gdi.DC_DrawBitmap(*args, **kwargs)
    def DrawTextXY(*args, **kwargs): return _gdi.DC_DrawTextXY(*args, **kwargs)
    def DrawText(*args, **kwargs): return _gdi.DC_DrawText(*args, **kwargs)
    def DrawRotatedTextXY(*args, **kwargs): return _gdi.DC_DrawRotatedTextXY(*args, **kwargs)
    def DrawRotatedText(*args, **kwargs): return _gdi.DC_DrawRotatedText(*args, **kwargs)
    def BlitXY(*args, **kwargs): return _gdi.DC_BlitXY(*args, **kwargs)
    def Blit(*args, **kwargs): return _gdi.DC_Blit(*args, **kwargs)
    def DrawLines(*args, **kwargs): return _gdi.DC_DrawLines(*args, **kwargs)
    def DrawPolygon(*args, **kwargs): return _gdi.DC_DrawPolygon(*args, **kwargs)
    def DrawLabel(*args, **kwargs): return _gdi.DC_DrawLabel(*args, **kwargs)
    def DrawImageLabel(*args, **kwargs): return _gdi.DC_DrawImageLabel(*args, **kwargs)
    def DrawSpline(*args, **kwargs): return _gdi.DC_DrawSpline(*args, **kwargs)
    def Clear(*args, **kwargs): return _gdi.DC_Clear(*args, **kwargs)
    def StartDoc(*args, **kwargs): return _gdi.DC_StartDoc(*args, **kwargs)
    def EndDoc(*args, **kwargs): return _gdi.DC_EndDoc(*args, **kwargs)
    def StartPage(*args, **kwargs): return _gdi.DC_StartPage(*args, **kwargs)
    def EndPage(*args, **kwargs): return _gdi.DC_EndPage(*args, **kwargs)
    def SetFont(*args, **kwargs): return _gdi.DC_SetFont(*args, **kwargs)
    def SetPen(*args, **kwargs): return _gdi.DC_SetPen(*args, **kwargs)
    def SetBrush(*args, **kwargs): return _gdi.DC_SetBrush(*args, **kwargs)
    def SetBackground(*args, **kwargs): return _gdi.DC_SetBackground(*args, **kwargs)
    def SetBackgroundMode(*args, **kwargs): return _gdi.DC_SetBackgroundMode(*args, **kwargs)
    def SetPalette(*args, **kwargs): return _gdi.DC_SetPalette(*args, **kwargs)
    def SetClippingRegion(*args, **kwargs): return _gdi.DC_SetClippingRegion(*args, **kwargs)
    def SetClippingRect(*args, **kwargs): return _gdi.DC_SetClippingRect(*args, **kwargs)
    def SetClippingRegionAsRegion(*args, **kwargs): return _gdi.DC_SetClippingRegionAsRegion(*args, **kwargs)
    def DestroyClippingRegion(*args, **kwargs): return _gdi.DC_DestroyClippingRegion(*args, **kwargs)
    def GetClippingBox(*args, **kwargs): return _gdi.DC_GetClippingBox(*args, **kwargs)
    def GetClippingRect(*args, **kwargs): return _gdi.DC_GetClippingRect(*args, **kwargs)
    def GetCharHeight(*args, **kwargs): return _gdi.DC_GetCharHeight(*args, **kwargs)
    def GetCharWidth(*args, **kwargs): return _gdi.DC_GetCharWidth(*args, **kwargs)
    def GetTextExtent(*args, **kwargs): return _gdi.DC_GetTextExtent(*args, **kwargs)
    def GetFullTextExtent(*args, **kwargs): return _gdi.DC_GetFullTextExtent(*args, **kwargs)
    def GetMultiLineTextExtent(*args, **kwargs): return _gdi.DC_GetMultiLineTextExtent(*args, **kwargs)
    def GetSizeTuple(*args, **kwargs): return _gdi.DC_GetSizeTuple(*args, **kwargs)
    def GetSize(*args, **kwargs): return _gdi.DC_GetSize(*args, **kwargs)
    def GetSizeMMWH(*args, **kwargs): return _gdi.DC_GetSizeMMWH(*args, **kwargs)
    def GetSizeMM(*args, **kwargs): return _gdi.DC_GetSizeMM(*args, **kwargs)
    def DeviceToLogicalX(*args, **kwargs): return _gdi.DC_DeviceToLogicalX(*args, **kwargs)
    def DeviceToLogicalY(*args, **kwargs): return _gdi.DC_DeviceToLogicalY(*args, **kwargs)
    def DeviceToLogicalXRel(*args, **kwargs): return _gdi.DC_DeviceToLogicalXRel(*args, **kwargs)
    def DeviceToLogicalYRel(*args, **kwargs): return _gdi.DC_DeviceToLogicalYRel(*args, **kwargs)
    def LogicalToDeviceX(*args, **kwargs): return _gdi.DC_LogicalToDeviceX(*args, **kwargs)
    def LogicalToDeviceY(*args, **kwargs): return _gdi.DC_LogicalToDeviceY(*args, **kwargs)
    def LogicalToDeviceXRel(*args, **kwargs): return _gdi.DC_LogicalToDeviceXRel(*args, **kwargs)
    def LogicalToDeviceYRel(*args, **kwargs): return _gdi.DC_LogicalToDeviceYRel(*args, **kwargs)
    def CanDrawBitmap(*args, **kwargs): return _gdi.DC_CanDrawBitmap(*args, **kwargs)
    def CanGetTextExtent(*args, **kwargs): return _gdi.DC_CanGetTextExtent(*args, **kwargs)
    def GetDepth(*args, **kwargs): return _gdi.DC_GetDepth(*args, **kwargs)
    def GetPPI(*args, **kwargs): return _gdi.DC_GetPPI(*args, **kwargs)
    def Ok(*args, **kwargs): return _gdi.DC_Ok(*args, **kwargs)
    def GetBackgroundMode(*args, **kwargs): return _gdi.DC_GetBackgroundMode(*args, **kwargs)
    def GetBackground(*args, **kwargs): return _gdi.DC_GetBackground(*args, **kwargs)
    def GetBrush(*args, **kwargs): return _gdi.DC_GetBrush(*args, **kwargs)
    def GetFont(*args, **kwargs): return _gdi.DC_GetFont(*args, **kwargs)
    def GetPen(*args, **kwargs): return _gdi.DC_GetPen(*args, **kwargs)
    def GetTextBackground(*args, **kwargs): return _gdi.DC_GetTextBackground(*args, **kwargs)
    def GetTextForeground(*args, **kwargs): return _gdi.DC_GetTextForeground(*args, **kwargs)
    def SetTextForeground(*args, **kwargs): return _gdi.DC_SetTextForeground(*args, **kwargs)
    def SetTextBackground(*args, **kwargs): return _gdi.DC_SetTextBackground(*args, **kwargs)
    def GetMapMode(*args, **kwargs): return _gdi.DC_GetMapMode(*args, **kwargs)
    def SetMapMode(*args, **kwargs): return _gdi.DC_SetMapMode(*args, **kwargs)
    def GetUserScale(*args, **kwargs): return _gdi.DC_GetUserScale(*args, **kwargs)
    def SetUserScale(*args, **kwargs): return _gdi.DC_SetUserScale(*args, **kwargs)
    def GetLogicalScale(*args, **kwargs): return _gdi.DC_GetLogicalScale(*args, **kwargs)
    def SetLogicalScale(*args, **kwargs): return _gdi.DC_SetLogicalScale(*args, **kwargs)
    def GetLogicalOriginTuple(*args, **kwargs): return _gdi.DC_GetLogicalOriginTuple(*args, **kwargs)
    def GetLogicalOrigin(*args, **kwargs): return _gdi.DC_GetLogicalOrigin(*args, **kwargs)
    def SetLogicalOrigin(*args, **kwargs): return _gdi.DC_SetLogicalOrigin(*args, **kwargs)
    def GetDeviceOriginTuple(*args, **kwargs): return _gdi.DC_GetDeviceOriginTuple(*args, **kwargs)
    def GetDeviceOrigin(*args, **kwargs): return _gdi.DC_GetDeviceOrigin(*args, **kwargs)
    def SetDeviceOrigin(*args, **kwargs): return _gdi.DC_SetDeviceOrigin(*args, **kwargs)
    def SetAxisOrientation(*args, **kwargs): return _gdi.DC_SetAxisOrientation(*args, **kwargs)
    def GetLogicalFunction(*args, **kwargs): return _gdi.DC_GetLogicalFunction(*args, **kwargs)
    def SetLogicalFunction(*args, **kwargs): return _gdi.DC_SetLogicalFunction(*args, **kwargs)
    def SetOptimization(*args, **kwargs): return _gdi.DC_SetOptimization(*args, **kwargs)
    def GetOptimization(*args, **kwargs): return _gdi.DC_GetOptimization(*args, **kwargs)
    def CalcBoundingBox(*args, **kwargs): return _gdi.DC_CalcBoundingBox(*args, **kwargs)
    def ResetBoundingBox(*args, **kwargs): return _gdi.DC_ResetBoundingBox(*args, **kwargs)
    def MinX(*args, **kwargs): return _gdi.DC_MinX(*args, **kwargs)
    def MaxX(*args, **kwargs): return _gdi.DC_MaxX(*args, **kwargs)
    def MinY(*args, **kwargs): return _gdi.DC_MinY(*args, **kwargs)
    def MaxY(*args, **kwargs): return _gdi.DC_MaxY(*args, **kwargs)
    def GetBoundingBox(*args, **kwargs): return _gdi.DC_GetBoundingBox(*args, **kwargs)
    def __nonzero__(self): return self.Ok() 
    def _DrawPointList(*args, **kwargs): return _gdi.DC__DrawPointList(*args, **kwargs)
    def _DrawLineList(*args, **kwargs): return _gdi.DC__DrawLineList(*args, **kwargs)
    def _DrawRectangleList(*args, **kwargs): return _gdi.DC__DrawRectangleList(*args, **kwargs)
    def _DrawEllipseList(*args, **kwargs): return _gdi.DC__DrawEllipseList(*args, **kwargs)
    def _DrawPolygonList(*args, **kwargs): return _gdi.DC__DrawPolygonList(*args, **kwargs)
    def _DrawTextList(*args, **kwargs): return _gdi.DC__DrawTextList(*args, **kwargs)
    def DrawPointList(self, points, pens=None):
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(points):
           raise ValueError('points and pens must have same length')
        return self._DrawPointList(points, pens, [])


    def DrawLineList(self, lines, pens=None):
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(lines):
           raise ValueError('lines and pens must have same length')
        return self._DrawLineList(lines, pens, [])


    def DrawRectangleList(self, rectangles, pens=None, brushes=None):
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(rectangles):
           raise ValueError('rectangles and pens must have same length')
        if brushes is None:
           brushes = []
        elif isinstance(brushes, wx.Brush):
           brushes = [brushes]
        elif len(brushes) != len(rectangles):
           raise ValueError('rectangles and brushes must have same length')
        return self._DrawRectangleList(rectangles, pens, brushes)


    def DrawEllipseList(self, ellipses, pens=None, brushes=None):
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(ellipses):
           raise ValueError('ellipses and pens must have same length')
        if brushes is None:
           brushes = []
        elif isinstance(brushes, wx.Brush):
           brushes = [brushes]
        elif len(brushes) != len(ellipses):
           raise ValueError('ellipses and brushes must have same length')
        return self._DrawEllipseList(ellipses, pens, brushes)


    def DrawPolygonList(self, polygons, pens=None, brushes=None):
        
        
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(polygons):
           raise ValueError('polygons and pens must have same length')
        if brushes is None:
           brushes = []
        elif isinstance(brushes, wx.Brush):
           brushes = [brushes]
        elif len(brushes) != len(polygons):
           raise ValueError('polygons and brushes must have same length')
        return self._DrawPolygonList(polygons, pens, brushes)


    def DrawTextList(self, textList, coords, foregrounds = None, backgrounds = None, fonts = None):
        
        
        
        if type(textList) == type(''):
           textList = [textList]
        elif len(textList) != len(coords):
           raise ValueError('textlist and coords must have same length')
        if foregrounds is None:
           foregrounds = []
        elif isinstance(foregrounds, wxColour):
           foregrounds = [foregrounds]
        elif len(foregrounds) != len(coords):
           raise ValueError('foregrounds and coords must have same length')
        if backgrounds is None:
           backgrounds = []
        elif isinstance(backgrounds, wxColour):
           backgrounds = [backgrounds]
        elif len(backgrounds) != len(coords):
           raise ValueError('backgrounds and coords must have same length')
        return  self._DrawTextList(textList, coords, foregrounds, backgrounds)

    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class DCPtr(DC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DC
_gdi.DC_swigregister(DCPtr)

#---------------------------------------------------------------------------

class MemoryDC(DC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_MemoryDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SelectObject(*args, **kwargs): return _gdi.MemoryDC_SelectObject(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMemoryDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class MemoryDCPtr(MemoryDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MemoryDC
_gdi.MemoryDC_swigregister(MemoryDCPtr)

def MemoryDCFromDC(*args, **kwargs):
    val = _gdi.new_MemoryDCFromDC(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class BufferedDC(MemoryDC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_BufferedDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._dc = args[0] # save a ref so the other dc will not be deleted before self
    def UnMask(*args, **kwargs): return _gdi.BufferedDC_UnMask(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBufferedDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class BufferedDCPtr(BufferedDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BufferedDC
_gdi.BufferedDC_swigregister(BufferedDCPtr)

def BufferedDCInternalBuffer(*args, **kwargs):
    val = _gdi.new_BufferedDCInternalBuffer(*args, **kwargs)
    val.thisown = 1
    val._dc = args[0] # save a ref so the other dc will not be deleted before self
    return val

class BufferedPaintDC(BufferedDC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_BufferedPaintDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBufferedPaintDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class BufferedPaintDCPtr(BufferedPaintDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BufferedPaintDC
_gdi.BufferedPaintDC_swigregister(BufferedPaintDCPtr)

#---------------------------------------------------------------------------

class ScreenDC(DC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_ScreenDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def StartDrawingOnTopWin(*args, **kwargs): return _gdi.ScreenDC_StartDrawingOnTopWin(*args, **kwargs)
    def StartDrawingOnTop(*args, **kwargs): return _gdi.ScreenDC_StartDrawingOnTop(*args, **kwargs)
    def EndDrawingOnTop(*args, **kwargs): return _gdi.ScreenDC_EndDrawingOnTop(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScreenDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ScreenDCPtr(ScreenDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ScreenDC
_gdi.ScreenDC_swigregister(ScreenDCPtr)

#---------------------------------------------------------------------------

class ClientDC(DC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_ClientDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxClientDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ClientDCPtr(ClientDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ClientDC
_gdi.ClientDC_swigregister(ClientDCPtr)

#---------------------------------------------------------------------------

class PaintDC(DC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_PaintDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPaintDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PaintDCPtr(PaintDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PaintDC
_gdi.PaintDC_swigregister(PaintDCPtr)

#---------------------------------------------------------------------------

class WindowDC(DC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_WindowDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWindowDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class WindowDCPtr(WindowDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WindowDC
_gdi.WindowDC_swigregister(WindowDCPtr)

#---------------------------------------------------------------------------

class MirrorDC(DC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_MirrorDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMirrorDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class MirrorDCPtr(MirrorDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MirrorDC
_gdi.MirrorDC_swigregister(MirrorDCPtr)

#---------------------------------------------------------------------------

class PostScriptDC(DC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_PostScriptDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPrintData(*args, **kwargs): return _gdi.PostScriptDC_GetPrintData(*args, **kwargs)
    def SetPrintData(*args, **kwargs): return _gdi.PostScriptDC_SetPrintData(*args, **kwargs)
    SetResolution = staticmethod(_gdi.PostScriptDC_SetResolution)
    GetResolution = staticmethod(_gdi.PostScriptDC_GetResolution)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPostScriptDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PostScriptDCPtr(PostScriptDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PostScriptDC
_gdi.PostScriptDC_swigregister(PostScriptDCPtr)

PostScriptDC_SetResolution = _gdi.PostScriptDC_SetResolution

PostScriptDC_GetResolution = _gdi.PostScriptDC_GetResolution

#---------------------------------------------------------------------------

class MetaFile(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_MetaFile(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMetaFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class MetaFilePtr(MetaFile):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MetaFile
_gdi.MetaFile_swigregister(MetaFilePtr)

class MetaFileDC(DC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_MetaFileDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMetaFileDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class MetaFileDCPtr(MetaFileDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MetaFileDC
_gdi.MetaFileDC_swigregister(MetaFileDCPtr)

class PrinterDC(DC):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_PrinterDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPrinterDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PrinterDCPtr(PrinterDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PrinterDC
_gdi.PrinterDC_swigregister(PrinterDCPtr)

#---------------------------------------------------------------------------

IMAGELIST_DRAW_NORMAL = _gdi.IMAGELIST_DRAW_NORMAL
IMAGELIST_DRAW_TRANSPARENT = _gdi.IMAGELIST_DRAW_TRANSPARENT
IMAGELIST_DRAW_SELECTED = _gdi.IMAGELIST_DRAW_SELECTED
IMAGELIST_DRAW_FOCUSED = _gdi.IMAGELIST_DRAW_FOCUSED
IMAGE_LIST_NORMAL = _gdi.IMAGE_LIST_NORMAL
IMAGE_LIST_SMALL = _gdi.IMAGE_LIST_SMALL
IMAGE_LIST_STATE = _gdi.IMAGE_LIST_STATE
class ImageList(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_ImageList(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_ImageList):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Add(*args, **kwargs): return _gdi.ImageList_Add(*args, **kwargs)
    def AddWithColourMask(*args, **kwargs): return _gdi.ImageList_AddWithColourMask(*args, **kwargs)
    def AddIcon(*args, **kwargs): return _gdi.ImageList_AddIcon(*args, **kwargs)
    def Replace(*args, **kwargs): return _gdi.ImageList_Replace(*args, **kwargs)
    def Draw(*args, **kwargs): return _gdi.ImageList_Draw(*args, **kwargs)
    def GetImageCount(*args, **kwargs): return _gdi.ImageList_GetImageCount(*args, **kwargs)
    def Remove(*args, **kwargs): return _gdi.ImageList_Remove(*args, **kwargs)
    def RemoveAll(*args, **kwargs): return _gdi.ImageList_RemoveAll(*args, **kwargs)
    def GetSize(*args, **kwargs): return _gdi.ImageList_GetSize(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxImageList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ImageListPtr(ImageList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ImageList
_gdi.ImageList_swigregister(ImageListPtr)

#---------------------------------------------------------------------------

class PenList(core.Object):
    def AddPen(*args, **kwargs): return _gdi.PenList_AddPen(*args, **kwargs)
    def FindOrCreatePen(*args, **kwargs): return _gdi.PenList_FindOrCreatePen(*args, **kwargs)
    def RemovePen(*args, **kwargs): return _gdi.PenList_RemovePen(*args, **kwargs)
    def GetCount(*args, **kwargs): return _gdi.PenList_GetCount(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPenList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class PenListPtr(PenList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PenList
_gdi.PenList_swigregister(PenListPtr)

class BrushList(core.Object):
    def AddBrush(*args, **kwargs): return _gdi.BrushList_AddBrush(*args, **kwargs)
    def FindOrCreateBrush(*args, **kwargs): return _gdi.BrushList_FindOrCreateBrush(*args, **kwargs)
    def RemoveBrush(*args, **kwargs): return _gdi.BrushList_RemoveBrush(*args, **kwargs)
    def GetCount(*args, **kwargs): return _gdi.BrushList_GetCount(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBrushList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class BrushListPtr(BrushList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BrushList
_gdi.BrushList_swigregister(BrushListPtr)

class ColourDatabase(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_ColourDatabase(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_ColourDatabase):
        try:
            if self.thisown: destroy(self)
        except: pass
    def Find(*args, **kwargs): return _gdi.ColourDatabase_Find(*args, **kwargs)
    def FindName(*args, **kwargs): return _gdi.ColourDatabase_FindName(*args, **kwargs)
    FindColour = Find 
    def AddColour(*args, **kwargs): return _gdi.ColourDatabase_AddColour(*args, **kwargs)
    def Append(*args, **kwargs): return _gdi.ColourDatabase_Append(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxColourDatabase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ColourDatabasePtr(ColourDatabase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ColourDatabase
_gdi.ColourDatabase_swigregister(ColourDatabasePtr)

class FontList(core.Object):
    def AddFont(*args, **kwargs): return _gdi.FontList_AddFont(*args, **kwargs)
    def FindOrCreateFont(*args, **kwargs): return _gdi.FontList_FindOrCreateFont(*args, **kwargs)
    def RemoveFont(*args, **kwargs): return _gdi.FontList_RemoveFont(*args, **kwargs)
    def GetCount(*args, **kwargs): return _gdi.FontList_GetCount(*args, **kwargs)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFontList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class FontListPtr(FontList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FontList
_gdi.FontList_swigregister(FontListPtr)

#---------------------------------------------------------------------------

#---------------------------------------------------------------------------

class DragImage(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_DragImage(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_DragImage):
        try:
            if self.thisown: destroy(self)
        except: pass
    def SetBackingBitmap(*args, **kwargs): return _gdi.DragImage_SetBackingBitmap(*args, **kwargs)
    def BeginDrag(*args, **kwargs): return _gdi.DragImage_BeginDrag(*args, **kwargs)
    def BeginDragBounded(*args, **kwargs): return _gdi.DragImage_BeginDragBounded(*args, **kwargs)
    def EndDrag(*args, **kwargs): return _gdi.DragImage_EndDrag(*args, **kwargs)
    def Move(*args, **kwargs): return _gdi.DragImage_Move(*args, **kwargs)
    def Show(*args, **kwargs): return _gdi.DragImage_Show(*args, **kwargs)
    def Hide(*args, **kwargs): return _gdi.DragImage_Hide(*args, **kwargs)
    def GetImageRect(*args, **kwargs): return _gdi.DragImage_GetImageRect(*args, **kwargs)
    def DoDrawImage(*args, **kwargs): return _gdi.DragImage_DoDrawImage(*args, **kwargs)
    def UpdateBackingFromWindow(*args, **kwargs): return _gdi.DragImage_UpdateBackingFromWindow(*args, **kwargs)
    def RedrawImage(*args, **kwargs): return _gdi.DragImage_RedrawImage(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGenericDragImage instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class DragImagePtr(DragImage):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DragImage
_gdi.DragImage_swigregister(DragImagePtr)
cvar = _gdi.cvar
NORMAL_FONT = cvar.NORMAL_FONT
SMALL_FONT = cvar.SMALL_FONT
ITALIC_FONT = cvar.ITALIC_FONT
SWISS_FONT = cvar.SWISS_FONT
RED_PEN = cvar.RED_PEN
CYAN_PEN = cvar.CYAN_PEN
GREEN_PEN = cvar.GREEN_PEN
BLACK_PEN = cvar.BLACK_PEN
WHITE_PEN = cvar.WHITE_PEN
TRANSPARENT_PEN = cvar.TRANSPARENT_PEN
BLACK_DASHED_PEN = cvar.BLACK_DASHED_PEN
GREY_PEN = cvar.GREY_PEN
MEDIUM_GREY_PEN = cvar.MEDIUM_GREY_PEN
LIGHT_GREY_PEN = cvar.LIGHT_GREY_PEN
BLUE_BRUSH = cvar.BLUE_BRUSH
GREEN_BRUSH = cvar.GREEN_BRUSH
WHITE_BRUSH = cvar.WHITE_BRUSH
BLACK_BRUSH = cvar.BLACK_BRUSH
TRANSPARENT_BRUSH = cvar.TRANSPARENT_BRUSH
CYAN_BRUSH = cvar.CYAN_BRUSH
RED_BRUSH = cvar.RED_BRUSH
GREY_BRUSH = cvar.GREY_BRUSH
MEDIUM_GREY_BRUSH = cvar.MEDIUM_GREY_BRUSH
LIGHT_GREY_BRUSH = cvar.LIGHT_GREY_BRUSH
BLACK = cvar.BLACK
WHITE = cvar.WHITE
RED = cvar.RED
BLUE = cvar.BLUE
GREEN = cvar.GREEN
CYAN = cvar.CYAN
LIGHT_GREY = cvar.LIGHT_GREY
STANDARD_CURSOR = cvar.STANDARD_CURSOR
HOURGLASS_CURSOR = cvar.HOURGLASS_CURSOR
CROSS_CURSOR = cvar.CROSS_CURSOR
NullBitmap = cvar.NullBitmap
NullIcon = cvar.NullIcon
NullCursor = cvar.NullCursor
NullPen = cvar.NullPen
NullBrush = cvar.NullBrush
NullPalette = cvar.NullPalette
NullFont = cvar.NullFont
NullColour = cvar.NullColour
TheFontList = cvar.TheFontList
ThePenList = cvar.ThePenList
TheBrushList = cvar.TheBrushList
TheColourDatabase = cvar.TheColourDatabase

def DragIcon(*args, **kwargs):
    val = _gdi.new_DragIcon(*args, **kwargs)
    val.thisown = 1
    return val

def DragString(*args, **kwargs):
    val = _gdi.new_DragString(*args, **kwargs)
    val.thisown = 1
    return val

def DragTreeItem(*args, **kwargs):
    val = _gdi.new_DragTreeItem(*args, **kwargs)
    val.thisown = 1
    return val

def DragListItem(*args, **kwargs):
    val = _gdi.new_DragListItem(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class Effects(core.Object):
    def __init__(self, *args, **kwargs):
        newobj = _gdi.new_Effects(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetHighlightColour(*args, **kwargs): return _gdi.Effects_GetHighlightColour(*args, **kwargs)
    def GetLightShadow(*args, **kwargs): return _gdi.Effects_GetLightShadow(*args, **kwargs)
    def GetFaceColour(*args, **kwargs): return _gdi.Effects_GetFaceColour(*args, **kwargs)
    def GetMediumShadow(*args, **kwargs): return _gdi.Effects_GetMediumShadow(*args, **kwargs)
    def GetDarkShadow(*args, **kwargs): return _gdi.Effects_GetDarkShadow(*args, **kwargs)
    def SetHighlightColour(*args, **kwargs): return _gdi.Effects_SetHighlightColour(*args, **kwargs)
    def SetLightShadow(*args, **kwargs): return _gdi.Effects_SetLightShadow(*args, **kwargs)
    def SetFaceColour(*args, **kwargs): return _gdi.Effects_SetFaceColour(*args, **kwargs)
    def SetMediumShadow(*args, **kwargs): return _gdi.Effects_SetMediumShadow(*args, **kwargs)
    def SetDarkShadow(*args, **kwargs): return _gdi.Effects_SetDarkShadow(*args, **kwargs)
    def Set(*args, **kwargs): return _gdi.Effects_Set(*args, **kwargs)
    def DrawSunkenEdge(*args, **kwargs): return _gdi.Effects_DrawSunkenEdge(*args, **kwargs)
    def TileBitmap(*args, **kwargs): return _gdi.Effects_TileBitmap(*args, **kwargs)
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEffects instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class EffectsPtr(Effects):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Effects
_gdi.Effects_swigregister(EffectsPtr)


