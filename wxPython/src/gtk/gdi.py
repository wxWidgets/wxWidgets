# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _gdi

import core
wx = core 
#---------------------------------------------------------------------------

class GDIObject(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGDIObject instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> GDIObject"""
        newobj = _gdi.new_GDIObject(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_GDIObject):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetVisible(*args, **kwargs):
        """GetVisible() -> bool"""
        return _gdi.GDIObject_GetVisible(*args, **kwargs)

    def SetVisible(*args, **kwargs):
        """SetVisible(bool visible)"""
        return _gdi.GDIObject_SetVisible(*args, **kwargs)

    def IsNull(*args, **kwargs):
        """IsNull() -> bool"""
        return _gdi.GDIObject_IsNull(*args, **kwargs)


class GDIObjectPtr(GDIObject):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GDIObject
_gdi.GDIObject_swigregister(GDIObjectPtr)

#---------------------------------------------------------------------------

class Colour(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxColour instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(unsigned char red=0, unsigned char green=0, unsigned char blue=0) -> Colour"""
        newobj = _gdi.new_Colour(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Colour):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Red(*args, **kwargs):
        """Red() -> unsigned char"""
        return _gdi.Colour_Red(*args, **kwargs)

    def Green(*args, **kwargs):
        """Green() -> unsigned char"""
        return _gdi.Colour_Green(*args, **kwargs)

    def Blue(*args, **kwargs):
        """Blue() -> unsigned char"""
        return _gdi.Colour_Blue(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _gdi.Colour_Ok(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(unsigned char red, unsigned char green, unsigned char blue)"""
        return _gdi.Colour_Set(*args, **kwargs)

    def SetRBG(*args, **kwargs):
        """SetRBG(unsigned long colRGB)"""
        return _gdi.Colour_SetRBG(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(Colour colour) -> bool"""
        return _gdi.Colour___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(Colour colour) -> bool"""
        return _gdi.Colour___ne__(*args, **kwargs)

    def Get(*args, **kwargs):
        """Get() -> PyObject"""
        return _gdi.Colour_Get(*args, **kwargs)

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
    """NamedColour(wxString colorName) -> Colour"""
    val = _gdi.new_NamedColour(*args, **kwargs)
    val.thisown = 1
    return val

def ColourRGB(*args, **kwargs):
    """ColourRGB(unsigned long colRGB) -> Colour"""
    val = _gdi.new_ColourRGB(*args, **kwargs)
    val.thisown = 1
    return val

class Palette(GDIObject):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPalette instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int n, unsigned char red, unsigned char green, unsigned char blue) -> Palette"""
        newobj = _gdi.new_Palette(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Palette):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetPixel(*args, **kwargs):
        """GetPixel(byte red, byte green, byte blue) -> int"""
        return _gdi.Palette_GetPixel(*args, **kwargs)

    def GetRGB(*args, **kwargs):
        """GetRGB(int pixel) -> (R,G,B)"""
        return _gdi.Palette_GetRGB(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _gdi.Palette_Ok(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class PalettePtr(Palette):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Palette
_gdi.Palette_swigregister(PalettePtr)

#---------------------------------------------------------------------------

class Pen(GDIObject):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPen instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Colour colour, int width=1, int style=SOLID) -> Pen"""
        newobj = _gdi.new_Pen(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Pen):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetCap(*args, **kwargs):
        """GetCap() -> int"""
        return _gdi.Pen_GetCap(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour() -> Colour"""
        return _gdi.Pen_GetColour(*args, **kwargs)

    def GetJoin(*args, **kwargs):
        """GetJoin() -> int"""
        return _gdi.Pen_GetJoin(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle() -> int"""
        return _gdi.Pen_GetStyle(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth() -> int"""
        return _gdi.Pen_GetWidth(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _gdi.Pen_Ok(*args, **kwargs)

    def SetCap(*args, **kwargs):
        """SetCap(int cap_style)"""
        return _gdi.Pen_SetCap(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """SetColour(Colour colour)"""
        return _gdi.Pen_SetColour(*args, **kwargs)

    def SetJoin(*args, **kwargs):
        """SetJoin(int join_style)"""
        return _gdi.Pen_SetJoin(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(int style)"""
        return _gdi.Pen_SetStyle(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(int width)"""
        return _gdi.Pen_SetWidth(*args, **kwargs)

    def SetDashes(*args, **kwargs):
        """SetDashes(int dashes, wxDash dashes_array)"""
        return _gdi.Pen_SetDashes(*args, **kwargs)

    def GetDashes(*args, **kwargs):
        """GetDashes() -> PyObject"""
        return _gdi.Pen_GetDashes(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class PenPtr(Pen):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Pen
_gdi.Pen_swigregister(PenPtr)

class PyPen(Pen):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyPen instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Colour colour, int width=1, int style=SOLID) -> PyPen"""
        newobj = _gdi.new_PyPen(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_PyPen):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetDashes(*args, **kwargs):
        """SetDashes(int dashes, wxDash dashes_array)"""
        return _gdi.PyPen_SetDashes(*args, **kwargs)


class PyPenPtr(PyPen):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PyPen
_gdi.PyPen_swigregister(PyPenPtr)

Pen = PyPen 
#---------------------------------------------------------------------------

class Brush(GDIObject):
    """A brush is a drawing tool for filling in areas. It is used for painting the
background of rectangles, ellipses, etc. It has a colour and a style."""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBrush instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Colour colour, int style=SOLID) -> Brush

Constructs a brush from a colour object and style."""
        newobj = _gdi.new_Brush(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Brush):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetColour(*args, **kwargs):
        """SetColour(Colour col)"""
        return _gdi.Brush_SetColour(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(int style)"""
        return _gdi.Brush_SetStyle(*args, **kwargs)

    def SetStipple(*args, **kwargs):
        """SetStipple(Bitmap stipple)"""
        return _gdi.Brush_SetStipple(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour() -> Colour"""
        return _gdi.Brush_GetColour(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle() -> int"""
        return _gdi.Brush_GetStyle(*args, **kwargs)

    def GetStipple(*args, **kwargs):
        """GetStipple() -> Bitmap"""
        return _gdi.Brush_GetStipple(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _gdi.Brush_Ok(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class BrushPtr(Brush):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Brush
_gdi.Brush_swigregister(BrushPtr)

class Bitmap(GDIObject):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBitmap instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString name, wxBitmapType type=BITMAP_TYPE_ANY) -> Bitmap

Loads a bitmap from a file."""
        newobj = _gdi.new_Bitmap(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Bitmap):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _gdi.Bitmap_Ok(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth() -> int

Gets the width of the bitmap in pixels."""
        return _gdi.Bitmap_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight() -> int

Gets the height of the bitmap in pixels."""
        return _gdi.Bitmap_GetHeight(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """GetDepth() -> int

Gets the colour depth of the bitmap. A value of 1 indicates a
monochrome bitmap."""
        return _gdi.Bitmap_GetDepth(*args, **kwargs)

    def ConvertToImage(*args, **kwargs):
        """ConvertToImage() -> Image

Creates a platform-independent image from a platform-dependent bitmap. This
preserves mask information so that bitmaps and images can be converted back
and forth without loss in that respect."""
        return _gdi.Bitmap_ConvertToImage(*args, **kwargs)

    def GetMask(*args, **kwargs):
        """GetMask() -> Mask

Gets the associated mask (if any) which may have been loaded from a file
or explpicitly set for the bitmap."""
        return _gdi.Bitmap_GetMask(*args, **kwargs)

    def SetMask(*args, **kwargs):
        """SetMask(Mask mask)

Sets the mask for this bitmap."""
        return _gdi.Bitmap_SetMask(*args, **kwargs)

    def SetMaskColour(*args, **kwargs):
        """SetMaskColour(Colour colour)

Create a Mask based on a specified colour in the Bitmap."""
        return _gdi.Bitmap_SetMaskColour(*args, **kwargs)

    def GetSubBitmap(*args, **kwargs):
        """GetSubBitmap(Rect rect) -> Bitmap

Returns a sub bitmap of the current one as long as the rect belongs entirely
to the bitmap. This function preserves bit depth and mask information."""
        return _gdi.Bitmap_GetSubBitmap(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """SaveFile(wxString name, wxBitmapType type, Palette palette=(wxPalette *) NULL) -> bool

Saves a bitmap in the named file."""
        return _gdi.Bitmap_SaveFile(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """LoadFile(wxString name, wxBitmapType type) -> bool

Loads a bitmap from a file"""
        return _gdi.Bitmap_LoadFile(*args, **kwargs)

    def CopyFromIcon(*args, **kwargs):
        """CopyFromIcon(Icon icon) -> bool"""
        return _gdi.Bitmap_CopyFromIcon(*args, **kwargs)

    def SetHeight(*args, **kwargs):
        """SetHeight(int height)

Set the height property (does not affect the bitmap data)."""
        return _gdi.Bitmap_SetHeight(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(int width)

Set the width property (does not affect the bitmap data)."""
        return _gdi.Bitmap_SetWidth(*args, **kwargs)

    def SetDepth(*args, **kwargs):
        """SetDepth(int depth)

Set the depth property (does not affect the bitmap data)."""
        return _gdi.Bitmap_SetDepth(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class BitmapPtr(Bitmap):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Bitmap
_gdi.Bitmap_swigregister(BitmapPtr)

def EmptyBitmap(*args, **kwargs):
    """EmptyBitmap(int width, int height, int depth=-1) -> Bitmap

Creates a new bitmap of the given size.  A depth of -1 indicates the depth of
the current screen or visual. Some platforms only support 1 for monochrome and
-1 for the current colour setting."""
    val = _gdi.new_EmptyBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromIcon(*args, **kwargs):
    """BitmapFromIcon(Icon icon) -> Bitmap

Create a new bitmap from an Icon object."""
    val = _gdi.new_BitmapFromIcon(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromImage(*args, **kwargs):
    """BitmapFromImage(Image image, int depth=-1) -> Bitmap

Creates bitmap object from the image. This has to be done to actually display
an image as you cannot draw an image directly on a window. The resulting
bitmap will use the provided colour depth (or that of the current system if
depth is -1) which entails that a colour reduction has to take place."""
    val = _gdi.new_BitmapFromImage(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromXPMData(*args, **kwargs):
    """BitmapFromXPMData(PyObject listOfStrings) -> Bitmap

Construct a Bitmap from a list of strings formatted as XPM data."""
    val = _gdi.new_BitmapFromXPMData(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromBits(*args, **kwargs):
    """BitmapFromBits(PyObject bits, int width, int height, int depth=1) -> Bitmap

Creates a bitmap from an array of bits.  You should only use this function for
monochrome bitmaps (depth 1) in portable programs: in this case the bits
parameter should contain an XBM image.  For other bit depths, the behaviour is
platform dependent."""
    val = _gdi.new_BitmapFromBits(*args, **kwargs)
    val.thisown = 1
    return val

class Mask(core.Object):
    """This class encapsulates a monochrome mask bitmap, where the masked area is
black and the unmasked area is white. When associated with a bitmap and drawn
in a device context, the unmasked area of the bitmap will be drawn, and the
masked area will not be drawn."""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMask instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Bitmap bitmap) -> Mask

Constructs a mask from a monochrome bitmap."""
        newobj = _gdi.new_Mask(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class MaskPtr(Mask):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Mask
_gdi.Mask_swigregister(MaskPtr)

def MaskColour(*args, **kwargs):
    """MaskColour(Bitmap bitmap, Colour colour) -> Mask

Constructs a mask from a bitmap and a colour in that bitmap that indicates the
background."""
    val = _gdi.new_MaskColour(*args, **kwargs)
    val.thisown = 1
    return val

class Icon(GDIObject):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIcon instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString name, long flags, int desiredWidth=-1, int desiredHeight=-1) -> Icon"""
        newobj = _gdi.new_Icon(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Icon):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def LoadFile(*args, **kwargs):
        """LoadFile(wxString name, long flags) -> bool"""
        return _gdi.Icon_LoadFile(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _gdi.Icon_Ok(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth() -> int"""
        return _gdi.Icon_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight() -> int"""
        return _gdi.Icon_GetHeight(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """GetDepth() -> int"""
        return _gdi.Icon_GetDepth(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(int w)"""
        return _gdi.Icon_SetWidth(*args, **kwargs)

    def SetHeight(*args, **kwargs):
        """SetHeight(int h)"""
        return _gdi.Icon_SetHeight(*args, **kwargs)

    def SetDepth(*args, **kwargs):
        """SetDepth(int d)"""
        return _gdi.Icon_SetDepth(*args, **kwargs)

    def CopyFromBitmap(*args, **kwargs):
        """CopyFromBitmap(Bitmap bmp)"""
        return _gdi.Icon_CopyFromBitmap(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class IconPtr(Icon):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Icon
_gdi.Icon_swigregister(IconPtr)

def EmptyIcon(*args, **kwargs):
    """EmptyIcon() -> Icon"""
    val = _gdi.new_EmptyIcon(*args, **kwargs)
    val.thisown = 1
    return val

def IconFromLocation(*args, **kwargs):
    """IconFromLocation(IconLocation loc) -> Icon"""
    val = _gdi.new_IconFromLocation(*args, **kwargs)
    val.thisown = 1
    return val

def IconFromBitmap(*args, **kwargs):
    """IconFromBitmap(Bitmap bmp) -> Icon"""
    val = _gdi.new_IconFromBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def IconFromXPMData(*args, **kwargs):
    """IconFromXPMData(PyObject listOfStrings) -> Icon"""
    val = _gdi.new_IconFromXPMData(*args, **kwargs)
    val.thisown = 1
    return val

class IconLocation(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIconLocation instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString filename=&wxPyEmptyString, int num=0) -> IconLocation"""
        newobj = _gdi.new_IconLocation(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_IconLocation):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def IsOk(*args, **kwargs):
        """IsOk() -> bool"""
        return _gdi.IconLocation_IsOk(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
    def SetFileName(*args, **kwargs):
        """SetFileName(wxString filename)"""
        return _gdi.IconLocation_SetFileName(*args, **kwargs)

    def GetFileName(*args, **kwargs):
        """GetFileName() -> wxString"""
        return _gdi.IconLocation_GetFileName(*args, **kwargs)

    def SetIndex(*args, **kwargs):
        """SetIndex(int num)"""
        return _gdi.IconLocation_SetIndex(*args, **kwargs)

    def GetIndex(*args, **kwargs):
        """GetIndex() -> int"""
        return _gdi.IconLocation_GetIndex(*args, **kwargs)


class IconLocationPtr(IconLocation):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IconLocation
_gdi.IconLocation_swigregister(IconLocationPtr)

class IconBundle(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIconBundle instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> IconBundle"""
        newobj = _gdi.new_IconBundle(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_IconBundle):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def AddIcon(*args, **kwargs):
        """AddIcon(Icon icon)"""
        return _gdi.IconBundle_AddIcon(*args, **kwargs)

    def AddIconFromFile(*args, **kwargs):
        """AddIconFromFile(wxString file, long type)"""
        return _gdi.IconBundle_AddIconFromFile(*args, **kwargs)

    def GetIcon(*args, **kwargs):
        """GetIcon(Size size) -> Icon"""
        return _gdi.IconBundle_GetIcon(*args, **kwargs)


class IconBundlePtr(IconBundle):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IconBundle
_gdi.IconBundle_swigregister(IconBundlePtr)

def IconBundleFromFile(*args, **kwargs):
    """IconBundleFromFile(wxString file, long type) -> IconBundle"""
    val = _gdi.new_IconBundleFromFile(*args, **kwargs)
    val.thisown = 1
    return val

def IconBundleFromIcon(*args, **kwargs):
    """IconBundleFromIcon(Icon icon) -> IconBundle"""
    val = _gdi.new_IconBundleFromIcon(*args, **kwargs)
    val.thisown = 1
    return val

class Cursor(GDIObject):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCursor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString cursorName, long flags, int hotSpotX=0, int hotSpotY=0) -> Cursor"""
        newobj = _gdi.new_Cursor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Cursor):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _gdi.Cursor_Ok(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class CursorPtr(Cursor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Cursor
_gdi.Cursor_swigregister(CursorPtr)

def StockCursor(*args, **kwargs):
    """StockCursor(int id) -> Cursor"""
    val = _gdi.new_StockCursor(*args, **kwargs)
    val.thisown = 1
    return val

def CursorFromImage(*args, **kwargs):
    """CursorFromImage(Image image) -> Cursor"""
    val = _gdi.new_CursorFromImage(*args, **kwargs)
    val.thisown = 1
    return val

def CursorFromBits(*args, **kwargs):
    """CursorFromBits(PyObject bits, int width, int height, int hotSpotX=-1, 
    int hotSpotY=-1, PyObject maskBits=0) -> Cursor"""
    val = _gdi.new_CursorFromBits(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

OutRegion = _gdi.OutRegion
PartRegion = _gdi.PartRegion
InRegion = _gdi.InRegion
class Region(GDIObject):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRegion instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int x=0, int y=0, int width=0, int height=0) -> Region"""
        newobj = _gdi.new_Region(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Region):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Clear(*args, **kwargs):
        """Clear()"""
        return _gdi.Region_Clear(*args, **kwargs)

    def Offset(*args, **kwargs):
        """Offset(int x, int y) -> bool"""
        return _gdi.Region_Offset(*args, **kwargs)

    def Contains(*args, **kwargs):
        """Contains(int x, int y) -> wxRegionContain"""
        return _gdi.Region_Contains(*args, **kwargs)

    def ContainsPoint(*args, **kwargs):
        """ContainsPoint(Point pt) -> wxRegionContain"""
        return _gdi.Region_ContainsPoint(*args, **kwargs)

    def ContainsRect(*args, **kwargs):
        """ContainsRect(Rect rect) -> wxRegionContain"""
        return _gdi.Region_ContainsRect(*args, **kwargs)

    def ContainsRectDim(*args, **kwargs):
        """ContainsRectDim(int x, int y, int w, int h) -> wxRegionContain"""
        return _gdi.Region_ContainsRectDim(*args, **kwargs)

    def GetBox(*args, **kwargs):
        """GetBox() -> Rect"""
        return _gdi.Region_GetBox(*args, **kwargs)

    def Intersect(*args, **kwargs):
        """Intersect(int x, int y, int width, int height) -> bool"""
        return _gdi.Region_Intersect(*args, **kwargs)

    def IntersectRect(*args, **kwargs):
        """IntersectRect(Rect rect) -> bool"""
        return _gdi.Region_IntersectRect(*args, **kwargs)

    def IntersectRegion(*args, **kwargs):
        """IntersectRegion(Region region) -> bool"""
        return _gdi.Region_IntersectRegion(*args, **kwargs)

    def IsEmpty(*args, **kwargs):
        """IsEmpty() -> bool"""
        return _gdi.Region_IsEmpty(*args, **kwargs)

    def Union(*args, **kwargs):
        """Union(int x, int y, int width, int height) -> bool"""
        return _gdi.Region_Union(*args, **kwargs)

    def UnionRect(*args, **kwargs):
        """UnionRect(Rect rect) -> bool"""
        return _gdi.Region_UnionRect(*args, **kwargs)

    def UnionRegion(*args, **kwargs):
        """UnionRegion(Region region) -> bool"""
        return _gdi.Region_UnionRegion(*args, **kwargs)

    def Subtract(*args, **kwargs):
        """Subtract(int x, int y, int width, int height) -> bool"""
        return _gdi.Region_Subtract(*args, **kwargs)

    def SubtractRect(*args, **kwargs):
        """SubtractRect(Rect rect) -> bool"""
        return _gdi.Region_SubtractRect(*args, **kwargs)

    def SubtractRegion(*args, **kwargs):
        """SubtractRegion(Region region) -> bool"""
        return _gdi.Region_SubtractRegion(*args, **kwargs)

    def Xor(*args, **kwargs):
        """Xor(int x, int y, int width, int height) -> bool"""
        return _gdi.Region_Xor(*args, **kwargs)

    def XorRect(*args, **kwargs):
        """XorRect(Rect rect) -> bool"""
        return _gdi.Region_XorRect(*args, **kwargs)

    def XorRegion(*args, **kwargs):
        """XorRegion(Region region) -> bool"""
        return _gdi.Region_XorRegion(*args, **kwargs)

    def ConvertToBitmap(*args, **kwargs):
        """ConvertToBitmap() -> Bitmap"""
        return _gdi.Region_ConvertToBitmap(*args, **kwargs)

    def UnionBitmap(*args, **kwargs):
        """UnionBitmap(Bitmap bmp, Colour transColour=NullColour, int tolerance=0) -> bool"""
        return _gdi.Region_UnionBitmap(*args, **kwargs)


class RegionPtr(Region):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Region
_gdi.Region_swigregister(RegionPtr)

def RegionFromBitmap(*args, **kwargs):
    """RegionFromBitmap(Bitmap bmp, Colour transColour=NullColour, int tolerance=0) -> Region"""
    val = _gdi.new_RegionFromBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def RegionFromPoints(*args, **kwargs):
    """RegionFromPoints(int points, Point points_array, int fillStyle=WINDING_RULE) -> Region"""
    val = _gdi.new_RegionFromPoints(*args, **kwargs)
    val.thisown = 1
    return val

class RegionIterator(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRegionIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Region region) -> RegionIterator"""
        newobj = _gdi.new_RegionIterator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_RegionIterator):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetX(*args, **kwargs):
        """GetX() -> int"""
        return _gdi.RegionIterator_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY() -> int"""
        return _gdi.RegionIterator_GetY(*args, **kwargs)

    def GetW(*args, **kwargs):
        """GetW() -> int"""
        return _gdi.RegionIterator_GetW(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth() -> int"""
        return _gdi.RegionIterator_GetWidth(*args, **kwargs)

    def GetH(*args, **kwargs):
        """GetH() -> int"""
        return _gdi.RegionIterator_GetH(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight() -> int"""
        return _gdi.RegionIterator_GetHeight(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """GetRect() -> Rect"""
        return _gdi.RegionIterator_GetRect(*args, **kwargs)

    def HaveRects(*args, **kwargs):
        """HaveRects() -> bool"""
        return _gdi.RegionIterator_HaveRects(*args, **kwargs)

    def Reset(*args, **kwargs):
        """Reset()"""
        return _gdi.RegionIterator_Reset(*args, **kwargs)

    def Next(*args, **kwargs):
        """Next()"""
        return _gdi.RegionIterator_Next(*args, **kwargs)

    def __nonzero__(*args, **kwargs):
        """__nonzero__() -> bool"""
        return _gdi.RegionIterator___nonzero__(*args, **kwargs)


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
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNativeFontInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> NativeFontInfo"""
        newobj = _gdi.new_NativeFontInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_NativeFontInfo):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Init(*args, **kwargs):
        """Init()"""
        return _gdi.NativeFontInfo_Init(*args, **kwargs)

    def InitFromFont(*args, **kwargs):
        """InitFromFont(Font font)"""
        return _gdi.NativeFontInfo_InitFromFont(*args, **kwargs)

    def GetPointSize(*args, **kwargs):
        """GetPointSize() -> int"""
        return _gdi.NativeFontInfo_GetPointSize(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle() -> wxFontStyle"""
        return _gdi.NativeFontInfo_GetStyle(*args, **kwargs)

    def GetWeight(*args, **kwargs):
        """GetWeight() -> wxFontWeight"""
        return _gdi.NativeFontInfo_GetWeight(*args, **kwargs)

    def GetUnderlined(*args, **kwargs):
        """GetUnderlined() -> bool"""
        return _gdi.NativeFontInfo_GetUnderlined(*args, **kwargs)

    def GetFaceName(*args, **kwargs):
        """GetFaceName() -> wxString"""
        return _gdi.NativeFontInfo_GetFaceName(*args, **kwargs)

    def GetFamily(*args, **kwargs):
        """GetFamily() -> wxFontFamily"""
        return _gdi.NativeFontInfo_GetFamily(*args, **kwargs)

    def GetEncoding(*args, **kwargs):
        """GetEncoding() -> wxFontEncoding"""
        return _gdi.NativeFontInfo_GetEncoding(*args, **kwargs)

    def SetPointSize(*args, **kwargs):
        """SetPointSize(int pointsize)"""
        return _gdi.NativeFontInfo_SetPointSize(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(wxFontStyle style)"""
        return _gdi.NativeFontInfo_SetStyle(*args, **kwargs)

    def SetWeight(*args, **kwargs):
        """SetWeight(wxFontWeight weight)"""
        return _gdi.NativeFontInfo_SetWeight(*args, **kwargs)

    def SetUnderlined(*args, **kwargs):
        """SetUnderlined(bool underlined)"""
        return _gdi.NativeFontInfo_SetUnderlined(*args, **kwargs)

    def SetFaceName(*args, **kwargs):
        """SetFaceName(wxString facename)"""
        return _gdi.NativeFontInfo_SetFaceName(*args, **kwargs)

    def SetFamily(*args, **kwargs):
        """SetFamily(wxFontFamily family)"""
        return _gdi.NativeFontInfo_SetFamily(*args, **kwargs)

    def SetEncoding(*args, **kwargs):
        """SetEncoding(wxFontEncoding encoding)"""
        return _gdi.NativeFontInfo_SetEncoding(*args, **kwargs)

    def FromString(*args, **kwargs):
        """FromString(wxString s) -> bool"""
        return _gdi.NativeFontInfo_FromString(*args, **kwargs)

    def ToString(*args, **kwargs):
        """ToString() -> wxString"""
        return _gdi.NativeFontInfo_ToString(*args, **kwargs)

    def __str__(*args, **kwargs):
        """__str__() -> wxString"""
        return _gdi.NativeFontInfo___str__(*args, **kwargs)

    def FromUserString(*args, **kwargs):
        """FromUserString(wxString s) -> bool"""
        return _gdi.NativeFontInfo_FromUserString(*args, **kwargs)

    def ToUserString(*args, **kwargs):
        """ToUserString() -> wxString"""
        return _gdi.NativeFontInfo_ToUserString(*args, **kwargs)


class NativeFontInfoPtr(NativeFontInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NativeFontInfo
_gdi.NativeFontInfo_swigregister(NativeFontInfoPtr)

class NativeEncodingInfo(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNativeEncodingInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    facename = property(_gdi.NativeEncodingInfo_facename_get, _gdi.NativeEncodingInfo_facename_set)
    encoding = property(_gdi.NativeEncodingInfo_encoding_get, _gdi.NativeEncodingInfo_encoding_set)
    def __init__(self, *args, **kwargs):
        """__init__() -> NativeEncodingInfo"""
        newobj = _gdi.new_NativeEncodingInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_NativeEncodingInfo):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def FromString(*args, **kwargs):
        """FromString(wxString s) -> bool"""
        return _gdi.NativeEncodingInfo_FromString(*args, **kwargs)

    def ToString(*args, **kwargs):
        """ToString() -> wxString"""
        return _gdi.NativeEncodingInfo_ToString(*args, **kwargs)


class NativeEncodingInfoPtr(NativeEncodingInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NativeEncodingInfo
_gdi.NativeEncodingInfo_swigregister(NativeEncodingInfoPtr)


def GetNativeFontEncoding(*args, **kwargs):
    """GetNativeFontEncoding(wxFontEncoding encoding) -> NativeEncodingInfo"""
    return _gdi.GetNativeFontEncoding(*args, **kwargs)

def TestFontEncoding(*args, **kwargs):
    """TestFontEncoding(NativeEncodingInfo info) -> bool"""
    return _gdi.TestFontEncoding(*args, **kwargs)
#---------------------------------------------------------------------------

class FontMapper(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFontMapper instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> FontMapper"""
        newobj = _gdi.new_FontMapper(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_FontMapper):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Get(*args, **kwargs):
        """FontMapper.Get() -> FontMapper"""
        return _gdi.FontMapper_Get(*args, **kwargs)

    Get = staticmethod(Get)
    def Set(*args, **kwargs):
        """FontMapper.Set(FontMapper mapper) -> FontMapper"""
        return _gdi.FontMapper_Set(*args, **kwargs)

    Set = staticmethod(Set)
    def CharsetToEncoding(*args, **kwargs):
        """CharsetToEncoding(wxString charset, bool interactive=True) -> wxFontEncoding"""
        return _gdi.FontMapper_CharsetToEncoding(*args, **kwargs)

    def GetSupportedEncodingsCount(*args, **kwargs):
        """FontMapper.GetSupportedEncodingsCount() -> size_t"""
        return _gdi.FontMapper_GetSupportedEncodingsCount(*args, **kwargs)

    GetSupportedEncodingsCount = staticmethod(GetSupportedEncodingsCount)
    def GetEncoding(*args, **kwargs):
        """FontMapper.GetEncoding(size_t n) -> wxFontEncoding"""
        return _gdi.FontMapper_GetEncoding(*args, **kwargs)

    GetEncoding = staticmethod(GetEncoding)
    def GetEncodingName(*args, **kwargs):
        """FontMapper.GetEncodingName(wxFontEncoding encoding) -> wxString"""
        return _gdi.FontMapper_GetEncodingName(*args, **kwargs)

    GetEncodingName = staticmethod(GetEncodingName)
    def GetEncodingDescription(*args, **kwargs):
        """FontMapper.GetEncodingDescription(wxFontEncoding encoding) -> wxString"""
        return _gdi.FontMapper_GetEncodingDescription(*args, **kwargs)

    GetEncodingDescription = staticmethod(GetEncodingDescription)
    def SetConfig(*args, **kwargs):
        """SetConfig(wxConfigBase config)"""
        return _gdi.FontMapper_SetConfig(*args, **kwargs)

    def SetConfigPath(*args, **kwargs):
        """SetConfigPath(wxString prefix)"""
        return _gdi.FontMapper_SetConfigPath(*args, **kwargs)

    def GetDefaultConfigPath(*args, **kwargs):
        """FontMapper.GetDefaultConfigPath() -> wxString"""
        return _gdi.FontMapper_GetDefaultConfigPath(*args, **kwargs)

    GetDefaultConfigPath = staticmethod(GetDefaultConfigPath)
    def GetAltForEncoding(*args, **kwargs):
        """GetAltForEncoding(wxFontEncoding encoding, wxString facename=wxPyEmptyString, 
    bool interactive=True) -> PyObject"""
        return _gdi.FontMapper_GetAltForEncoding(*args, **kwargs)

    def IsEncodingAvailable(*args, **kwargs):
        """IsEncodingAvailable(wxFontEncoding encoding, wxString facename=wxPyEmptyString) -> bool"""
        return _gdi.FontMapper_IsEncodingAvailable(*args, **kwargs)

    def SetDialogParent(*args, **kwargs):
        """SetDialogParent(Window parent)"""
        return _gdi.FontMapper_SetDialogParent(*args, **kwargs)

    def SetDialogTitle(*args, **kwargs):
        """SetDialogTitle(wxString title)"""
        return _gdi.FontMapper_SetDialogTitle(*args, **kwargs)


class FontMapperPtr(FontMapper):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FontMapper
_gdi.FontMapper_swigregister(FontMapperPtr)

def FontMapper_Get(*args, **kwargs):
    """FontMapper_Get() -> FontMapper"""
    return _gdi.FontMapper_Get(*args, **kwargs)

def FontMapper_Set(*args, **kwargs):
    """FontMapper_Set(FontMapper mapper) -> FontMapper"""
    return _gdi.FontMapper_Set(*args, **kwargs)

def FontMapper_GetSupportedEncodingsCount(*args, **kwargs):
    """FontMapper_GetSupportedEncodingsCount() -> size_t"""
    return _gdi.FontMapper_GetSupportedEncodingsCount(*args, **kwargs)

def FontMapper_GetEncoding(*args, **kwargs):
    """FontMapper_GetEncoding(size_t n) -> wxFontEncoding"""
    return _gdi.FontMapper_GetEncoding(*args, **kwargs)

def FontMapper_GetEncodingName(*args, **kwargs):
    """FontMapper_GetEncodingName(wxFontEncoding encoding) -> wxString"""
    return _gdi.FontMapper_GetEncodingName(*args, **kwargs)

def FontMapper_GetEncodingDescription(*args, **kwargs):
    """FontMapper_GetEncodingDescription(wxFontEncoding encoding) -> wxString"""
    return _gdi.FontMapper_GetEncodingDescription(*args, **kwargs)

def FontMapper_GetDefaultConfigPath(*args, **kwargs):
    """FontMapper_GetDefaultConfigPath() -> wxString"""
    return _gdi.FontMapper_GetDefaultConfigPath(*args, **kwargs)

#---------------------------------------------------------------------------

class Font(GDIObject):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFont instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int pointSize, int family, int style, int weight, bool underline=False, 
    wxString face=wxPyEmptyString, 
    wxFontEncoding encoding=FONTENCODING_DEFAULT) -> Font"""
        newobj = _gdi.new_Font(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Font):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _gdi.Font_Ok(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
    def __eq__(*args, **kwargs):
        """__eq__(Font font) -> bool"""
        return _gdi.Font___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(Font font) -> bool"""
        return _gdi.Font___ne__(*args, **kwargs)

    def GetPointSize(*args, **kwargs):
        """GetPointSize() -> int"""
        return _gdi.Font_GetPointSize(*args, **kwargs)

    def GetFamily(*args, **kwargs):
        """GetFamily() -> int"""
        return _gdi.Font_GetFamily(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle() -> int"""
        return _gdi.Font_GetStyle(*args, **kwargs)

    def GetWeight(*args, **kwargs):
        """GetWeight() -> int"""
        return _gdi.Font_GetWeight(*args, **kwargs)

    def GetUnderlined(*args, **kwargs):
        """GetUnderlined() -> bool"""
        return _gdi.Font_GetUnderlined(*args, **kwargs)

    def GetFaceName(*args, **kwargs):
        """GetFaceName() -> wxString"""
        return _gdi.Font_GetFaceName(*args, **kwargs)

    def GetEncoding(*args, **kwargs):
        """GetEncoding() -> wxFontEncoding"""
        return _gdi.Font_GetEncoding(*args, **kwargs)

    def GetNativeFontInfo(*args, **kwargs):
        """GetNativeFontInfo() -> NativeFontInfo"""
        return _gdi.Font_GetNativeFontInfo(*args, **kwargs)

    def IsFixedWidth(*args, **kwargs):
        """IsFixedWidth() -> bool"""
        return _gdi.Font_IsFixedWidth(*args, **kwargs)

    def GetNativeFontInfoDesc(*args, **kwargs):
        """GetNativeFontInfoDesc() -> wxString"""
        return _gdi.Font_GetNativeFontInfoDesc(*args, **kwargs)

    def GetNativeFontInfoUserDesc(*args, **kwargs):
        """GetNativeFontInfoUserDesc() -> wxString"""
        return _gdi.Font_GetNativeFontInfoUserDesc(*args, **kwargs)

    def SetPointSize(*args, **kwargs):
        """SetPointSize(int pointSize)"""
        return _gdi.Font_SetPointSize(*args, **kwargs)

    def SetFamily(*args, **kwargs):
        """SetFamily(int family)"""
        return _gdi.Font_SetFamily(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(int style)"""
        return _gdi.Font_SetStyle(*args, **kwargs)

    def SetWeight(*args, **kwargs):
        """SetWeight(int weight)"""
        return _gdi.Font_SetWeight(*args, **kwargs)

    def SetFaceName(*args, **kwargs):
        """SetFaceName(wxString faceName)"""
        return _gdi.Font_SetFaceName(*args, **kwargs)

    def SetUnderlined(*args, **kwargs):
        """SetUnderlined(bool underlined)"""
        return _gdi.Font_SetUnderlined(*args, **kwargs)

    def SetEncoding(*args, **kwargs):
        """SetEncoding(wxFontEncoding encoding)"""
        return _gdi.Font_SetEncoding(*args, **kwargs)

    def SetNativeFontInfo(*args, **kwargs):
        """SetNativeFontInfo(NativeFontInfo info)"""
        return _gdi.Font_SetNativeFontInfo(*args, **kwargs)

    def SetNativeFontInfoFromString(*args, **kwargs):
        """SetNativeFontInfoFromString(wxString info)"""
        return _gdi.Font_SetNativeFontInfoFromString(*args, **kwargs)

    def SetNativeFontInfoUserDesc(*args, **kwargs):
        """SetNativeFontInfoUserDesc(wxString info)"""
        return _gdi.Font_SetNativeFontInfoUserDesc(*args, **kwargs)

    def GetFamilyString(*args, **kwargs):
        """GetFamilyString() -> wxString"""
        return _gdi.Font_GetFamilyString(*args, **kwargs)

    def GetStyleString(*args, **kwargs):
        """GetStyleString() -> wxString"""
        return _gdi.Font_GetStyleString(*args, **kwargs)

    def GetWeightString(*args, **kwargs):
        """GetWeightString() -> wxString"""
        return _gdi.Font_GetWeightString(*args, **kwargs)

    def SetNoAntiAliasing(*args, **kwargs):
        """SetNoAntiAliasing(bool no=True)"""
        return _gdi.Font_SetNoAntiAliasing(*args, **kwargs)

    def GetNoAntiAliasing(*args, **kwargs):
        """GetNoAntiAliasing() -> bool"""
        return _gdi.Font_GetNoAntiAliasing(*args, **kwargs)

    def GetDefaultEncoding(*args, **kwargs):
        """Font.GetDefaultEncoding() -> wxFontEncoding"""
        return _gdi.Font_GetDefaultEncoding(*args, **kwargs)

    GetDefaultEncoding = staticmethod(GetDefaultEncoding)
    def SetDefaultEncoding(*args, **kwargs):
        """Font.SetDefaultEncoding(wxFontEncoding encoding)"""
        return _gdi.Font_SetDefaultEncoding(*args, **kwargs)

    SetDefaultEncoding = staticmethod(SetDefaultEncoding)

class FontPtr(Font):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Font
_gdi.Font_swigregister(FontPtr)

def FontFromNativeInfo(*args, **kwargs):
    """FontFromNativeInfo(NativeFontInfo info) -> Font"""
    val = _gdi.new_FontFromNativeInfo(*args, **kwargs)
    val.thisown = 1
    return val

def FontFromNativeInfoString(*args, **kwargs):
    """FontFromNativeInfoString(wxString info) -> Font"""
    val = _gdi.new_FontFromNativeInfoString(*args, **kwargs)
    val.thisown = 1
    return val

def Font2(*args, **kwargs):
    """Font2(int pointSize, wxFontFamily family, int flags=FONTFLAG_DEFAULT, 
    wxString face=wxPyEmptyString, wxFontEncoding encoding=FONTENCODING_DEFAULT) -> Font"""
    val = _gdi.new_Font2(*args, **kwargs)
    val.thisown = 1
    return val

def Font_GetDefaultEncoding(*args, **kwargs):
    """Font_GetDefaultEncoding() -> wxFontEncoding"""
    return _gdi.Font_GetDefaultEncoding(*args, **kwargs)

def Font_SetDefaultEncoding(*args, **kwargs):
    """Font_SetDefaultEncoding(wxFontEncoding encoding)"""
    return _gdi.Font_SetDefaultEncoding(*args, **kwargs)

#---------------------------------------------------------------------------

class FontEnumerator(object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyFontEnumerator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> FontEnumerator"""
        newobj = _gdi.new_FontEnumerator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, FontEnumerator, 0)

    def __del__(self, destroy=_gdi.delete_FontEnumerator):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(PyObject self, PyObject _class, bool incref)"""
        return _gdi.FontEnumerator__setCallbackInfo(*args, **kwargs)

    def EnumerateFacenames(*args, **kwargs):
        """EnumerateFacenames(wxFontEncoding encoding=FONTENCODING_SYSTEM, bool fixedWidthOnly=False) -> bool"""
        return _gdi.FontEnumerator_EnumerateFacenames(*args, **kwargs)

    def EnumerateEncodings(*args, **kwargs):
        """EnumerateEncodings(wxString facename=wxPyEmptyString) -> bool"""
        return _gdi.FontEnumerator_EnumerateEncodings(*args, **kwargs)

    def GetEncodings(*args, **kwargs):
        """GetEncodings() -> PyObject"""
        return _gdi.FontEnumerator_GetEncodings(*args, **kwargs)

    def GetFacenames(*args, **kwargs):
        """GetFacenames() -> PyObject"""
        return _gdi.FontEnumerator_GetFacenames(*args, **kwargs)


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
    """"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxLanguageInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    Language = property(_gdi.LanguageInfo_Language_get, _gdi.LanguageInfo_Language_set)
    CanonicalName = property(_gdi.LanguageInfo_CanonicalName_get, _gdi.LanguageInfo_CanonicalName_set)
    Description = property(_gdi.LanguageInfo_Description_get, _gdi.LanguageInfo_Description_set)

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
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxLocale instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int language=LANGUAGE_DEFAULT, int flags=wxLOCALE_LOAD_DEFAULT|wxLOCALE_CONV_ENCODING) -> Locale"""
        newobj = _gdi.new_Locale(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_Locale):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Init1(*args, **kwargs):
        """Init1(wxString szName, wxString szShort=wxPyEmptyString, 
    wxString szLocale=wxPyEmptyString, bool bLoadDefault=True, 
    bool bConvertEncoding=False) -> bool"""
        return _gdi.Locale_Init1(*args, **kwargs)

    def Init2(*args, **kwargs):
        """Init2(int language=LANGUAGE_DEFAULT, int flags=wxLOCALE_LOAD_DEFAULT|wxLOCALE_CONV_ENCODING) -> bool"""
        return _gdi.Locale_Init2(*args, **kwargs)

    def Init(self, *_args, **_kwargs):
        if type(_args[0]) in [type(''), type(u'')]:
            val = self.Init1(*_args, **_kwargs)
        else:
            val = self.Init2(*_args, **_kwargs)
        return val

    def GetSystemLanguage(*args, **kwargs):
        """Locale.GetSystemLanguage() -> int"""
        return _gdi.Locale_GetSystemLanguage(*args, **kwargs)

    GetSystemLanguage = staticmethod(GetSystemLanguage)
    def GetSystemEncoding(*args, **kwargs):
        """Locale.GetSystemEncoding() -> wxFontEncoding"""
        return _gdi.Locale_GetSystemEncoding(*args, **kwargs)

    GetSystemEncoding = staticmethod(GetSystemEncoding)
    def GetSystemEncodingName(*args, **kwargs):
        """Locale.GetSystemEncodingName() -> wxString"""
        return _gdi.Locale_GetSystemEncodingName(*args, **kwargs)

    GetSystemEncodingName = staticmethod(GetSystemEncodingName)
    def IsOk(*args, **kwargs):
        """IsOk() -> bool"""
        return _gdi.Locale_IsOk(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    def GetLocale(*args, **kwargs):
        """GetLocale() -> wxString"""
        return _gdi.Locale_GetLocale(*args, **kwargs)

    def GetLanguage(*args, **kwargs):
        """GetLanguage() -> int"""
        return _gdi.Locale_GetLanguage(*args, **kwargs)

    def GetSysName(*args, **kwargs):
        """GetSysName() -> wxString"""
        return _gdi.Locale_GetSysName(*args, **kwargs)

    def GetCanonicalName(*args, **kwargs):
        """GetCanonicalName() -> wxString"""
        return _gdi.Locale_GetCanonicalName(*args, **kwargs)

    def AddCatalogLookupPathPrefix(*args, **kwargs):
        """Locale.AddCatalogLookupPathPrefix(wxString prefix)"""
        return _gdi.Locale_AddCatalogLookupPathPrefix(*args, **kwargs)

    AddCatalogLookupPathPrefix = staticmethod(AddCatalogLookupPathPrefix)
    def AddCatalog(*args, **kwargs):
        """AddCatalog(wxString szDomain) -> bool"""
        return _gdi.Locale_AddCatalog(*args, **kwargs)

    def IsLoaded(*args, **kwargs):
        """IsLoaded(wxString szDomain) -> bool"""
        return _gdi.Locale_IsLoaded(*args, **kwargs)

    def GetLanguageInfo(*args, **kwargs):
        """Locale.GetLanguageInfo(int lang) -> LanguageInfo"""
        return _gdi.Locale_GetLanguageInfo(*args, **kwargs)

    GetLanguageInfo = staticmethod(GetLanguageInfo)
    def GetLanguageName(*args, **kwargs):
        """Locale.GetLanguageName(int lang) -> wxString"""
        return _gdi.Locale_GetLanguageName(*args, **kwargs)

    GetLanguageName = staticmethod(GetLanguageName)
    def FindLanguageInfo(*args, **kwargs):
        """Locale.FindLanguageInfo(wxString locale) -> LanguageInfo"""
        return _gdi.Locale_FindLanguageInfo(*args, **kwargs)

    FindLanguageInfo = staticmethod(FindLanguageInfo)
    def AddLanguage(*args, **kwargs):
        """Locale.AddLanguage(LanguageInfo info)"""
        return _gdi.Locale_AddLanguage(*args, **kwargs)

    AddLanguage = staticmethod(AddLanguage)
    def GetString(*args, **kwargs):
        """GetString(wxString szOrigString, wxString szDomain=wxPyEmptyString) -> wxString"""
        return _gdi.Locale_GetString(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName() -> wxString"""
        return _gdi.Locale_GetName(*args, **kwargs)


class LocalePtr(Locale):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Locale
_gdi.Locale_swigregister(LocalePtr)

def Locale_GetSystemLanguage(*args, **kwargs):
    """Locale_GetSystemLanguage() -> int"""
    return _gdi.Locale_GetSystemLanguage(*args, **kwargs)

def Locale_GetSystemEncoding(*args, **kwargs):
    """Locale_GetSystemEncoding() -> wxFontEncoding"""
    return _gdi.Locale_GetSystemEncoding(*args, **kwargs)

def Locale_GetSystemEncodingName(*args, **kwargs):
    """Locale_GetSystemEncodingName() -> wxString"""
    return _gdi.Locale_GetSystemEncodingName(*args, **kwargs)

def Locale_AddCatalogLookupPathPrefix(*args, **kwargs):
    """Locale_AddCatalogLookupPathPrefix(wxString prefix)"""
    return _gdi.Locale_AddCatalogLookupPathPrefix(*args, **kwargs)

def Locale_GetLanguageInfo(*args, **kwargs):
    """Locale_GetLanguageInfo(int lang) -> LanguageInfo"""
    return _gdi.Locale_GetLanguageInfo(*args, **kwargs)

def Locale_GetLanguageName(*args, **kwargs):
    """Locale_GetLanguageName(int lang) -> wxString"""
    return _gdi.Locale_GetLanguageName(*args, **kwargs)

def Locale_FindLanguageInfo(*args, **kwargs):
    """Locale_FindLanguageInfo(wxString locale) -> LanguageInfo"""
    return _gdi.Locale_FindLanguageInfo(*args, **kwargs)

def Locale_AddLanguage(*args, **kwargs):
    """Locale_AddLanguage(LanguageInfo info)"""
    return _gdi.Locale_AddLanguage(*args, **kwargs)


def GetLocale(*args, **kwargs):
    """GetLocale() -> Locale"""
    return _gdi.GetLocale(*args, **kwargs)
#---------------------------------------------------------------------------

CONVERT_STRICT = _gdi.CONVERT_STRICT
CONVERT_SUBSTITUTE = _gdi.CONVERT_SUBSTITUTE
PLATFORM_CURRENT = _gdi.PLATFORM_CURRENT
PLATFORM_UNIX = _gdi.PLATFORM_UNIX
PLATFORM_WINDOWS = _gdi.PLATFORM_WINDOWS
PLATFORM_OS2 = _gdi.PLATFORM_OS2
PLATFORM_MAC = _gdi.PLATFORM_MAC
class EncodingConverter(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEncodingConverter instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> EncodingConverter"""
        newobj = _gdi.new_EncodingConverter(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_EncodingConverter):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Init(*args, **kwargs):
        """Init(wxFontEncoding input_enc, wxFontEncoding output_enc, 
    int method=CONVERT_STRICT) -> bool"""
        return _gdi.EncodingConverter_Init(*args, **kwargs)

    def Convert(*args, **kwargs):
        """Convert(wxString input) -> wxString"""
        return _gdi.EncodingConverter_Convert(*args, **kwargs)

    def GetPlatformEquivalents(*args, **kwargs):
        """EncodingConverter.GetPlatformEquivalents(wxFontEncoding enc, int platform=PLATFORM_CURRENT) -> wxFontEncodingArray"""
        return _gdi.EncodingConverter_GetPlatformEquivalents(*args, **kwargs)

    GetPlatformEquivalents = staticmethod(GetPlatformEquivalents)
    def GetAllEquivalents(*args, **kwargs):
        """EncodingConverter.GetAllEquivalents(wxFontEncoding enc) -> wxFontEncodingArray"""
        return _gdi.EncodingConverter_GetAllEquivalents(*args, **kwargs)

    GetAllEquivalents = staticmethod(GetAllEquivalents)
    def CanConvert(*args, **kwargs):
        """EncodingConverter.CanConvert(wxFontEncoding encIn, wxFontEncoding encOut) -> bool"""
        return _gdi.EncodingConverter_CanConvert(*args, **kwargs)

    CanConvert = staticmethod(CanConvert)
    def __nonzero__(self): return self.IsOk() 

class EncodingConverterPtr(EncodingConverter):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EncodingConverter
_gdi.EncodingConverter_swigregister(EncodingConverterPtr)

def GetTranslation(*args):
    """GetTranslation(wxString sz) -> wxString
GetTranslation(wxString sz1, wxString sz2, size_t n) -> wxString"""
    return _gdi.GetTranslation(*args)

def EncodingConverter_GetPlatformEquivalents(*args, **kwargs):
    """EncodingConverter_GetPlatformEquivalents(wxFontEncoding enc, int platform=PLATFORM_CURRENT) -> wxFontEncodingArray"""
    return _gdi.EncodingConverter_GetPlatformEquivalents(*args, **kwargs)

def EncodingConverter_GetAllEquivalents(*args, **kwargs):
    """EncodingConverter_GetAllEquivalents(wxFontEncoding enc) -> wxFontEncodingArray"""
    return _gdi.EncodingConverter_GetAllEquivalents(*args, **kwargs)

def EncodingConverter_CanConvert(*args, **kwargs):
    """EncodingConverter_CanConvert(wxFontEncoding encIn, wxFontEncoding encOut) -> bool"""
    return _gdi.EncodingConverter_CanConvert(*args, **kwargs)

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
    """"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_gdi.delete_DC):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def BeginDrawing(*args, **kwargs):
        """BeginDrawing()"""
        return _gdi.DC_BeginDrawing(*args, **kwargs)

    def EndDrawing(*args, **kwargs):
        """EndDrawing()"""
        return _gdi.DC_EndDrawing(*args, **kwargs)

    def FloodFillXY(*args, **kwargs):
        """FloodFillXY(int x, int y, Colour col, int style=FLOOD_SURFACE) -> bool"""
        return _gdi.DC_FloodFillXY(*args, **kwargs)

    def FloodFill(*args, **kwargs):
        """FloodFill(Point pt, Colour col, int style=FLOOD_SURFACE) -> bool"""
        return _gdi.DC_FloodFill(*args, **kwargs)

    def GetPixelXY(*args, **kwargs):
        """GetPixelXY(int x, int y) -> Colour"""
        return _gdi.DC_GetPixelXY(*args, **kwargs)

    def GetPixel(*args, **kwargs):
        """GetPixel(Point pt) -> Colour"""
        return _gdi.DC_GetPixel(*args, **kwargs)

    def DrawLineXY(*args, **kwargs):
        """DrawLineXY(int x1, int y1, int x2, int y2)"""
        return _gdi.DC_DrawLineXY(*args, **kwargs)

    def DrawLine(*args, **kwargs):
        """DrawLine(Point pt1, Point pt2)"""
        return _gdi.DC_DrawLine(*args, **kwargs)

    def CrossHairXY(*args, **kwargs):
        """CrossHairXY(int x, int y)"""
        return _gdi.DC_CrossHairXY(*args, **kwargs)

    def CrossHair(*args, **kwargs):
        """CrossHair(Point pt)"""
        return _gdi.DC_CrossHair(*args, **kwargs)

    def DrawArcXY(*args, **kwargs):
        """DrawArcXY(int x1, int y1, int x2, int y2, int xc, int yc)"""
        return _gdi.DC_DrawArcXY(*args, **kwargs)

    def DrawArc(*args, **kwargs):
        """DrawArc(Point pt1, Point pt2, Point centre)"""
        return _gdi.DC_DrawArc(*args, **kwargs)

    def DrawCheckMarkXY(*args, **kwargs):
        """DrawCheckMarkXY(int x, int y, int width, int height)"""
        return _gdi.DC_DrawCheckMarkXY(*args, **kwargs)

    def DrawCheckMark(*args, **kwargs):
        """DrawCheckMark(Rect rect)"""
        return _gdi.DC_DrawCheckMark(*args, **kwargs)

    def DrawEllipticArcXY(*args, **kwargs):
        """DrawEllipticArcXY(int x, int y, int w, int h, double sa, double ea)"""
        return _gdi.DC_DrawEllipticArcXY(*args, **kwargs)

    def DrawEllipticArc(*args, **kwargs):
        """DrawEllipticArc(Point pt, Size sz, double sa, double ea)"""
        return _gdi.DC_DrawEllipticArc(*args, **kwargs)

    def DrawPointXY(*args, **kwargs):
        """DrawPointXY(int x, int y)"""
        return _gdi.DC_DrawPointXY(*args, **kwargs)

    def DrawPoint(*args, **kwargs):
        """DrawPoint(Point pt)"""
        return _gdi.DC_DrawPoint(*args, **kwargs)

    def DrawRectangleXY(*args, **kwargs):
        """DrawRectangleXY(int x, int y, int width, int height)"""
        return _gdi.DC_DrawRectangleXY(*args, **kwargs)

    def DrawRectangle(*args, **kwargs):
        """DrawRectangle(Point pt, Size sz)"""
        return _gdi.DC_DrawRectangle(*args, **kwargs)

    def DrawRectangleRect(*args, **kwargs):
        """DrawRectangleRect(Rect rect)"""
        return _gdi.DC_DrawRectangleRect(*args, **kwargs)

    def DrawRoundedRectangleXY(*args, **kwargs):
        """DrawRoundedRectangleXY(int x, int y, int width, int height, double radius)"""
        return _gdi.DC_DrawRoundedRectangleXY(*args, **kwargs)

    def DrawRoundedRectangle(*args, **kwargs):
        """DrawRoundedRectangle(Point pt, Size sz, double radius)"""
        return _gdi.DC_DrawRoundedRectangle(*args, **kwargs)

    def DrawRoundedRectangleRect(*args, **kwargs):
        """DrawRoundedRectangleRect(Rect r, double radius)"""
        return _gdi.DC_DrawRoundedRectangleRect(*args, **kwargs)

    def DrawCircleXY(*args, **kwargs):
        """DrawCircleXY(int x, int y, int radius)"""
        return _gdi.DC_DrawCircleXY(*args, **kwargs)

    def DrawCircle(*args, **kwargs):
        """DrawCircle(Point pt, int radius)"""
        return _gdi.DC_DrawCircle(*args, **kwargs)

    def DrawEllipseXY(*args, **kwargs):
        """DrawEllipseXY(int x, int y, int width, int height)"""
        return _gdi.DC_DrawEllipseXY(*args, **kwargs)

    def DrawEllipse(*args, **kwargs):
        """DrawEllipse(Point pt, Size sz)"""
        return _gdi.DC_DrawEllipse(*args, **kwargs)

    def DrawEllipseRect(*args, **kwargs):
        """DrawEllipseRect(Rect rect)"""
        return _gdi.DC_DrawEllipseRect(*args, **kwargs)

    def DrawIconXY(*args, **kwargs):
        """DrawIconXY(Icon icon, int x, int y)"""
        return _gdi.DC_DrawIconXY(*args, **kwargs)

    def DrawIcon(*args, **kwargs):
        """DrawIcon(Icon icon, Point pt)"""
        return _gdi.DC_DrawIcon(*args, **kwargs)

    def DrawBitmapXY(*args, **kwargs):
        """DrawBitmapXY(Bitmap bmp, int x, int y, bool useMask=False)"""
        return _gdi.DC_DrawBitmapXY(*args, **kwargs)

    def DrawBitmap(*args, **kwargs):
        """DrawBitmap(Bitmap bmp, Point pt, bool useMask=False)"""
        return _gdi.DC_DrawBitmap(*args, **kwargs)

    def DrawTextXY(*args, **kwargs):
        """DrawTextXY(wxString text, int x, int y)"""
        return _gdi.DC_DrawTextXY(*args, **kwargs)

    def DrawText(*args, **kwargs):
        """DrawText(wxString text, Point pt)"""
        return _gdi.DC_DrawText(*args, **kwargs)

    def DrawRotatedTextXY(*args, **kwargs):
        """DrawRotatedTextXY(wxString text, int x, int y, double angle)"""
        return _gdi.DC_DrawRotatedTextXY(*args, **kwargs)

    def DrawRotatedText(*args, **kwargs):
        """DrawRotatedText(wxString text, Point pt, double angle)"""
        return _gdi.DC_DrawRotatedText(*args, **kwargs)

    def BlitXY(*args, **kwargs):
        """BlitXY(int xdest, int ydest, int width, int height, DC source, 
    int xsrc, int ysrc, int rop=COPY, bool useMask=False, 
    int xsrcMask=-1, int ysrcMask=-1) -> bool"""
        return _gdi.DC_BlitXY(*args, **kwargs)

    def Blit(*args, **kwargs):
        """Blit(Point destPt, Size sz, DC source, Point srcPt, int rop=COPY, 
    bool useMask=False, Point srcPtMask=DefaultPosition) -> bool"""
        return _gdi.DC_Blit(*args, **kwargs)

    def DrawLines(*args, **kwargs):
        """DrawLines(int points, Point points_array, int xoffset=0, int yoffset=0)"""
        return _gdi.DC_DrawLines(*args, **kwargs)

    def DrawPolygon(*args, **kwargs):
        """DrawPolygon(int points, Point points_array, int xoffset=0, int yoffset=0, 
    int fillStyle=ODDEVEN_RULE)"""
        return _gdi.DC_DrawPolygon(*args, **kwargs)

    def DrawLabel(*args, **kwargs):
        """DrawLabel(wxString text, Rect rect, int alignment=wxALIGN_LEFT|wxALIGN_TOP, 
    int indexAccel=-1)"""
        return _gdi.DC_DrawLabel(*args, **kwargs)

    def DrawImageLabel(*args, **kwargs):
        """DrawImageLabel(wxString text, Bitmap image, Rect rect, int alignment=wxALIGN_LEFT|wxALIGN_TOP, 
    int indexAccel=-1) -> Rect"""
        return _gdi.DC_DrawImageLabel(*args, **kwargs)

    def DrawSpline(*args, **kwargs):
        """DrawSpline(int points, Point points_array)"""
        return _gdi.DC_DrawSpline(*args, **kwargs)

    def Clear(*args, **kwargs):
        """Clear()"""
        return _gdi.DC_Clear(*args, **kwargs)

    def StartDoc(*args, **kwargs):
        """StartDoc(wxString message) -> bool"""
        return _gdi.DC_StartDoc(*args, **kwargs)

    def EndDoc(*args, **kwargs):
        """EndDoc()"""
        return _gdi.DC_EndDoc(*args, **kwargs)

    def StartPage(*args, **kwargs):
        """StartPage()"""
        return _gdi.DC_StartPage(*args, **kwargs)

    def EndPage(*args, **kwargs):
        """EndPage()"""
        return _gdi.DC_EndPage(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """SetFont(Font font)"""
        return _gdi.DC_SetFont(*args, **kwargs)

    def SetPen(*args, **kwargs):
        """SetPen(Pen pen)"""
        return _gdi.DC_SetPen(*args, **kwargs)

    def SetBrush(*args, **kwargs):
        """SetBrush(Brush brush)"""
        return _gdi.DC_SetBrush(*args, **kwargs)

    def SetBackground(*args, **kwargs):
        """SetBackground(Brush brush)"""
        return _gdi.DC_SetBackground(*args, **kwargs)

    def SetBackgroundMode(*args, **kwargs):
        """SetBackgroundMode(int mode)"""
        return _gdi.DC_SetBackgroundMode(*args, **kwargs)

    def SetPalette(*args, **kwargs):
        """SetPalette(Palette palette)"""
        return _gdi.DC_SetPalette(*args, **kwargs)

    def SetClippingRegion(*args, **kwargs):
        """SetClippingRegion(int x, int y, int width, int height)"""
        return _gdi.DC_SetClippingRegion(*args, **kwargs)

    def SetClippingRect(*args, **kwargs):
        """SetClippingRect(Rect rect)"""
        return _gdi.DC_SetClippingRect(*args, **kwargs)

    def SetClippingRegionAsRegion(*args, **kwargs):
        """SetClippingRegionAsRegion(Region region)"""
        return _gdi.DC_SetClippingRegionAsRegion(*args, **kwargs)

    def DestroyClippingRegion(*args, **kwargs):
        """DestroyClippingRegion()"""
        return _gdi.DC_DestroyClippingRegion(*args, **kwargs)

    def GetClippingBox(*args, **kwargs):
        """GetClippingBox() -> (x, y, width, height)"""
        return _gdi.DC_GetClippingBox(*args, **kwargs)

    def GetClippingRect(*args, **kwargs):
        """GetClippingRect() -> Rect"""
        return _gdi.DC_GetClippingRect(*args, **kwargs)

    def GetCharHeight(*args, **kwargs):
        """GetCharHeight() -> int"""
        return _gdi.DC_GetCharHeight(*args, **kwargs)

    def GetCharWidth(*args, **kwargs):
        """GetCharWidth() -> int"""
        return _gdi.DC_GetCharWidth(*args, **kwargs)

    def GetTextExtent(*args, **kwargs):
        """GetTextExtent(wxString string) -> (width, height)

Get the width and height of the text using the current font.
Only works for single line strings."""
        return _gdi.DC_GetTextExtent(*args, **kwargs)

    def GetFullTextExtent(*args, **kwargs):
        """GetFullTextExtent(wxString string, Font font=None) ->
   (width, height, descent, externalLeading)

Get the width, height, decent and leading of the text using the current or specified font.
Only works for single line strings."""
        return _gdi.DC_GetFullTextExtent(*args, **kwargs)

    def GetMultiLineTextExtent(*args, **kwargs):
        """GetMultiLineTextExtent(wxString string, Font font=None) ->
   (width, height, descent, externalLeading)

Get the width, height, decent and leading of the text using the current or specified font.
Works for single as well as multi-line strings."""
        return _gdi.DC_GetMultiLineTextExtent(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize() -> Size

Get the DC size in device units."""
        return _gdi.DC_GetSize(*args, **kwargs)

    def GetSizeTuple(*args, **kwargs):
        """GetSizeTuple() -> (width, height)

Get the DC size in device units."""
        return _gdi.DC_GetSizeTuple(*args, **kwargs)

    def GetSizeMM(*args, **kwargs):
        """GetSizeMM() -> Size

Get the DC size in milimeters."""
        return _gdi.DC_GetSizeMM(*args, **kwargs)

    def GetSizeMMTuple(*args, **kwargs):
        """GetSizeMMTuple() -> (width, height)

Get the DC size in milimeters."""
        return _gdi.DC_GetSizeMMTuple(*args, **kwargs)

    def DeviceToLogicalX(*args, **kwargs):
        """DeviceToLogicalX(int x) -> int"""
        return _gdi.DC_DeviceToLogicalX(*args, **kwargs)

    def DeviceToLogicalY(*args, **kwargs):
        """DeviceToLogicalY(int y) -> int"""
        return _gdi.DC_DeviceToLogicalY(*args, **kwargs)

    def DeviceToLogicalXRel(*args, **kwargs):
        """DeviceToLogicalXRel(int x) -> int"""
        return _gdi.DC_DeviceToLogicalXRel(*args, **kwargs)

    def DeviceToLogicalYRel(*args, **kwargs):
        """DeviceToLogicalYRel(int y) -> int"""
        return _gdi.DC_DeviceToLogicalYRel(*args, **kwargs)

    def LogicalToDeviceX(*args, **kwargs):
        """LogicalToDeviceX(int x) -> int"""
        return _gdi.DC_LogicalToDeviceX(*args, **kwargs)

    def LogicalToDeviceY(*args, **kwargs):
        """LogicalToDeviceY(int y) -> int"""
        return _gdi.DC_LogicalToDeviceY(*args, **kwargs)

    def LogicalToDeviceXRel(*args, **kwargs):
        """LogicalToDeviceXRel(int x) -> int"""
        return _gdi.DC_LogicalToDeviceXRel(*args, **kwargs)

    def LogicalToDeviceYRel(*args, **kwargs):
        """LogicalToDeviceYRel(int y) -> int"""
        return _gdi.DC_LogicalToDeviceYRel(*args, **kwargs)

    def CanDrawBitmap(*args, **kwargs):
        """CanDrawBitmap() -> bool"""
        return _gdi.DC_CanDrawBitmap(*args, **kwargs)

    def CanGetTextExtent(*args, **kwargs):
        """CanGetTextExtent() -> bool"""
        return _gdi.DC_CanGetTextExtent(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """GetDepth() -> int"""
        return _gdi.DC_GetDepth(*args, **kwargs)

    def GetPPI(*args, **kwargs):
        """GetPPI() -> Size"""
        return _gdi.DC_GetPPI(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok() -> bool"""
        return _gdi.DC_Ok(*args, **kwargs)

    def GetBackgroundMode(*args, **kwargs):
        """GetBackgroundMode() -> int"""
        return _gdi.DC_GetBackgroundMode(*args, **kwargs)

    def GetBackground(*args, **kwargs):
        """GetBackground() -> Brush"""
        return _gdi.DC_GetBackground(*args, **kwargs)

    def GetBrush(*args, **kwargs):
        """GetBrush() -> Brush"""
        return _gdi.DC_GetBrush(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """GetFont() -> Font"""
        return _gdi.DC_GetFont(*args, **kwargs)

    def GetPen(*args, **kwargs):
        """GetPen() -> Pen"""
        return _gdi.DC_GetPen(*args, **kwargs)

    def GetTextBackground(*args, **kwargs):
        """GetTextBackground() -> Colour"""
        return _gdi.DC_GetTextBackground(*args, **kwargs)

    def GetTextForeground(*args, **kwargs):
        """GetTextForeground() -> Colour"""
        return _gdi.DC_GetTextForeground(*args, **kwargs)

    def SetTextForeground(*args, **kwargs):
        """SetTextForeground(Colour colour)"""
        return _gdi.DC_SetTextForeground(*args, **kwargs)

    def SetTextBackground(*args, **kwargs):
        """SetTextBackground(Colour colour)"""
        return _gdi.DC_SetTextBackground(*args, **kwargs)

    def GetMapMode(*args, **kwargs):
        """GetMapMode() -> int"""
        return _gdi.DC_GetMapMode(*args, **kwargs)

    def SetMapMode(*args, **kwargs):
        """SetMapMode(int mode)"""
        return _gdi.DC_SetMapMode(*args, **kwargs)

    def GetUserScale(*args, **kwargs):
        """GetUserScale() -> (xScale, yScale)"""
        return _gdi.DC_GetUserScale(*args, **kwargs)

    def SetUserScale(*args, **kwargs):
        """SetUserScale(double x, double y)"""
        return _gdi.DC_SetUserScale(*args, **kwargs)

    def GetLogicalScale(*args, **kwargs):
        """GetLogicalScale() -> (xScale, yScale)"""
        return _gdi.DC_GetLogicalScale(*args, **kwargs)

    def SetLogicalScale(*args, **kwargs):
        """SetLogicalScale(double x, double y)"""
        return _gdi.DC_SetLogicalScale(*args, **kwargs)

    def GetLogicalOrigin(*args, **kwargs):
        """GetLogicalOrigin() -> Point"""
        return _gdi.DC_GetLogicalOrigin(*args, **kwargs)

    def GetLogicalOriginTuple(*args, **kwargs):
        """GetLogicalOriginTuple() -> (x,y)"""
        return _gdi.DC_GetLogicalOriginTuple(*args, **kwargs)

    def SetLogicalOrigin(*args, **kwargs):
        """SetLogicalOrigin(int x, int y)"""
        return _gdi.DC_SetLogicalOrigin(*args, **kwargs)

    def GetDeviceOrigin(*args, **kwargs):
        """GetDeviceOrigin() -> Point"""
        return _gdi.DC_GetDeviceOrigin(*args, **kwargs)

    def GetDeviceOriginTuple(*args, **kwargs):
        """GetDeviceOriginTuple() -> (x,y)"""
        return _gdi.DC_GetDeviceOriginTuple(*args, **kwargs)

    def SetDeviceOrigin(*args, **kwargs):
        """SetDeviceOrigin(int x, int y)"""
        return _gdi.DC_SetDeviceOrigin(*args, **kwargs)

    def SetAxisOrientation(*args, **kwargs):
        """SetAxisOrientation(bool xLeftRight, bool yBottomUp)"""
        return _gdi.DC_SetAxisOrientation(*args, **kwargs)

    def GetLogicalFunction(*args, **kwargs):
        """GetLogicalFunction() -> int"""
        return _gdi.DC_GetLogicalFunction(*args, **kwargs)

    def SetLogicalFunction(*args, **kwargs):
        """SetLogicalFunction(int function)"""
        return _gdi.DC_SetLogicalFunction(*args, **kwargs)

    def SetOptimization(*args, **kwargs):
        """SetOptimization(bool opt)"""
        return _gdi.DC_SetOptimization(*args, **kwargs)

    def GetOptimization(*args, **kwargs):
        """GetOptimization() -> bool"""
        return _gdi.DC_GetOptimization(*args, **kwargs)

    def CalcBoundingBox(*args, **kwargs):
        """CalcBoundingBox(int x, int y)"""
        return _gdi.DC_CalcBoundingBox(*args, **kwargs)

    def ResetBoundingBox(*args, **kwargs):
        """ResetBoundingBox()"""
        return _gdi.DC_ResetBoundingBox(*args, **kwargs)

    def MinX(*args, **kwargs):
        """MinX() -> int"""
        return _gdi.DC_MinX(*args, **kwargs)

    def MaxX(*args, **kwargs):
        """MaxX() -> int"""
        return _gdi.DC_MaxX(*args, **kwargs)

    def MinY(*args, **kwargs):
        """MinY() -> int"""
        return _gdi.DC_MinY(*args, **kwargs)

    def MaxY(*args, **kwargs):
        """MaxY() -> int"""
        return _gdi.DC_MaxY(*args, **kwargs)

    def GetBoundingBox(*args, **kwargs):
        """GetBoundingBox() -> (x1,y1, x2,y2)"""
        return _gdi.DC_GetBoundingBox(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
    def _DrawPointList(*args, **kwargs):
        """_DrawPointList(PyObject pyCoords, PyObject pyPens, PyObject pyBrushes) -> PyObject"""
        return _gdi.DC__DrawPointList(*args, **kwargs)

    def _DrawLineList(*args, **kwargs):
        """_DrawLineList(PyObject pyCoords, PyObject pyPens, PyObject pyBrushes) -> PyObject"""
        return _gdi.DC__DrawLineList(*args, **kwargs)

    def _DrawRectangleList(*args, **kwargs):
        """_DrawRectangleList(PyObject pyCoords, PyObject pyPens, PyObject pyBrushes) -> PyObject"""
        return _gdi.DC__DrawRectangleList(*args, **kwargs)

    def _DrawEllipseList(*args, **kwargs):
        """_DrawEllipseList(PyObject pyCoords, PyObject pyPens, PyObject pyBrushes) -> PyObject"""
        return _gdi.DC__DrawEllipseList(*args, **kwargs)

    def _DrawPolygonList(*args, **kwargs):
        """_DrawPolygonList(PyObject pyCoords, PyObject pyPens, PyObject pyBrushes) -> PyObject"""
        return _gdi.DC__DrawPolygonList(*args, **kwargs)

    def _DrawTextList(*args, **kwargs):
        """_DrawTextList(PyObject textList, PyObject pyPoints, PyObject foregroundList, 
    PyObject backgroundList) -> PyObject"""
        return _gdi.DC__DrawTextList(*args, **kwargs)

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
        elif isinstance(foregrounds, wx.Colour):
           foregrounds = [foregrounds]
        elif len(foregrounds) != len(coords):
           raise ValueError('foregrounds and coords must have same length')
        if backgrounds is None:
           backgrounds = []
        elif isinstance(backgrounds, wx.Colour):
           backgrounds = [backgrounds]
        elif len(backgrounds) != len(coords):
           raise ValueError('backgrounds and coords must have same length')
        return  self._DrawTextList(textList, coords, foregrounds, backgrounds)


class DCPtr(DC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = DC
_gdi.DC_swigregister(DCPtr)

#---------------------------------------------------------------------------

class MemoryDC(DC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMemoryDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> MemoryDC"""
        newobj = _gdi.new_MemoryDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SelectObject(*args, **kwargs):
        """SelectObject(Bitmap bitmap)"""
        return _gdi.MemoryDC_SelectObject(*args, **kwargs)


class MemoryDCPtr(MemoryDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MemoryDC
_gdi.MemoryDC_swigregister(MemoryDCPtr)

def MemoryDCFromDC(*args, **kwargs):
    """MemoryDCFromDC(DC oldDC) -> MemoryDC"""
    val = _gdi.new_MemoryDCFromDC(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class BufferedDC(MemoryDC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBufferedDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(DC dc, Bitmap buffer) -> BufferedDC"""
        newobj = _gdi.new_BufferedDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._dc = args[0] # save a ref so the other dc will not be deleted before self

    def UnMask(*args, **kwargs):
        """UnMask()"""
        return _gdi.BufferedDC_UnMask(*args, **kwargs)


class BufferedDCPtr(BufferedDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BufferedDC
_gdi.BufferedDC_swigregister(BufferedDCPtr)

def BufferedDCInternalBuffer(*args, **kwargs):
    """BufferedDCInternalBuffer(DC dc, Size area) -> BufferedDC"""
    val = _gdi.new_BufferedDCInternalBuffer(*args, **kwargs)
    val.thisown = 1
    val._dc = args[0] # save a ref so the other dc will not be deleted before self
    return val

class BufferedPaintDC(BufferedDC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBufferedPaintDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window window, Bitmap buffer=NullBitmap) -> BufferedPaintDC"""
        newobj = _gdi.new_BufferedPaintDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class BufferedPaintDCPtr(BufferedPaintDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BufferedPaintDC
_gdi.BufferedPaintDC_swigregister(BufferedPaintDCPtr)

#---------------------------------------------------------------------------

class ScreenDC(DC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScreenDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> ScreenDC"""
        newobj = _gdi.new_ScreenDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def StartDrawingOnTopWin(*args, **kwargs):
        """StartDrawingOnTopWin(Window window) -> bool"""
        return _gdi.ScreenDC_StartDrawingOnTopWin(*args, **kwargs)

    def StartDrawingOnTop(*args, **kwargs):
        """StartDrawingOnTop(Rect rect=None) -> bool"""
        return _gdi.ScreenDC_StartDrawingOnTop(*args, **kwargs)

    def EndDrawingOnTop(*args, **kwargs):
        """EndDrawingOnTop() -> bool"""
        return _gdi.ScreenDC_EndDrawingOnTop(*args, **kwargs)


class ScreenDCPtr(ScreenDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ScreenDC
_gdi.ScreenDC_swigregister(ScreenDCPtr)

#---------------------------------------------------------------------------

class ClientDC(DC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxClientDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window win) -> ClientDC"""
        newobj = _gdi.new_ClientDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class ClientDCPtr(ClientDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ClientDC
_gdi.ClientDC_swigregister(ClientDCPtr)

#---------------------------------------------------------------------------

class PaintDC(DC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPaintDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window win) -> PaintDC"""
        newobj = _gdi.new_PaintDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class PaintDCPtr(PaintDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PaintDC
_gdi.PaintDC_swigregister(PaintDCPtr)

#---------------------------------------------------------------------------

class WindowDC(DC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWindowDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(Window win) -> WindowDC"""
        newobj = _gdi.new_WindowDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class WindowDCPtr(WindowDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WindowDC
_gdi.WindowDC_swigregister(WindowDCPtr)

#---------------------------------------------------------------------------

class MirrorDC(DC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMirrorDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(DC dc, bool mirror) -> MirrorDC"""
        newobj = _gdi.new_MirrorDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class MirrorDCPtr(MirrorDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MirrorDC
_gdi.MirrorDC_swigregister(MirrorDCPtr)

#---------------------------------------------------------------------------

class PostScriptDC(DC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPostScriptDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxPrintData printData) -> PostScriptDC"""
        newobj = _gdi.new_PostScriptDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPrintData(*args, **kwargs):
        """GetPrintData() -> wxPrintData"""
        return _gdi.PostScriptDC_GetPrintData(*args, **kwargs)

    def SetPrintData(*args, **kwargs):
        """SetPrintData(wxPrintData data)"""
        return _gdi.PostScriptDC_SetPrintData(*args, **kwargs)

    def SetResolution(*args, **kwargs):
        """PostScriptDC.SetResolution(int ppi)"""
        return _gdi.PostScriptDC_SetResolution(*args, **kwargs)

    SetResolution = staticmethod(SetResolution)
    def GetResolution(*args, **kwargs):
        """PostScriptDC.GetResolution() -> int"""
        return _gdi.PostScriptDC_GetResolution(*args, **kwargs)

    GetResolution = staticmethod(GetResolution)

class PostScriptDCPtr(PostScriptDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PostScriptDC
_gdi.PostScriptDC_swigregister(PostScriptDCPtr)

def PostScriptDC_SetResolution(*args, **kwargs):
    """PostScriptDC_SetResolution(int ppi)"""
    return _gdi.PostScriptDC_SetResolution(*args, **kwargs)

def PostScriptDC_GetResolution(*args, **kwargs):
    """PostScriptDC_GetResolution() -> int"""
    return _gdi.PostScriptDC_GetResolution(*args, **kwargs)

#---------------------------------------------------------------------------

class MetaFile(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMetaFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString filename=wxPyEmptyString) -> MetaFile"""
        newobj = _gdi.new_MetaFile(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class MetaFilePtr(MetaFile):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MetaFile
_gdi.MetaFile_swigregister(MetaFilePtr)

class MetaFileDC(DC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMetaFileDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxString filename=wxPyEmptyString, int width=0, int height=0, 
    wxString description=wxPyEmptyString) -> MetaFileDC"""
        newobj = _gdi.new_MetaFileDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class MetaFileDCPtr(MetaFileDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MetaFileDC
_gdi.MetaFileDC_swigregister(MetaFileDCPtr)

class PrinterDC(DC):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPrinterDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(wxPrintData printData) -> PrinterDC"""
        newobj = _gdi.new_PrinterDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

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
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxImageList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(int width, int height, int mask=True, int initialCount=1) -> ImageList"""
        newobj = _gdi.new_ImageList(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_ImageList):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Add(*args, **kwargs):
        """Add(Bitmap bitmap, Bitmap mask=NullBitmap) -> int"""
        return _gdi.ImageList_Add(*args, **kwargs)

    def AddWithColourMask(*args, **kwargs):
        """AddWithColourMask(Bitmap bitmap, Colour maskColour) -> int"""
        return _gdi.ImageList_AddWithColourMask(*args, **kwargs)

    def AddIcon(*args, **kwargs):
        """AddIcon(Icon icon) -> int"""
        return _gdi.ImageList_AddIcon(*args, **kwargs)

    def Replace(*args, **kwargs):
        """Replace(int index, Bitmap bitmap) -> bool"""
        return _gdi.ImageList_Replace(*args, **kwargs)

    def Draw(*args, **kwargs):
        """Draw(int index, DC dc, int x, int x, int flags=IMAGELIST_DRAW_NORMAL, 
    bool solidBackground=False) -> bool"""
        return _gdi.ImageList_Draw(*args, **kwargs)

    def GetImageCount(*args, **kwargs):
        """GetImageCount() -> int"""
        return _gdi.ImageList_GetImageCount(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(int index) -> bool"""
        return _gdi.ImageList_Remove(*args, **kwargs)

    def RemoveAll(*args, **kwargs):
        """RemoveAll() -> bool"""
        return _gdi.ImageList_RemoveAll(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize() -> (width,height)"""
        return _gdi.ImageList_GetSize(*args, **kwargs)


class ImageListPtr(ImageList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ImageList
_gdi.ImageList_swigregister(ImageListPtr)

#---------------------------------------------------------------------------

class PenList(core.Object):
    """"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPenList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def AddPen(*args, **kwargs):
        """AddPen(Pen pen)"""
        return _gdi.PenList_AddPen(*args, **kwargs)

    def FindOrCreatePen(*args, **kwargs):
        """FindOrCreatePen(Colour colour, int width, int style) -> Pen"""
        return _gdi.PenList_FindOrCreatePen(*args, **kwargs)

    def RemovePen(*args, **kwargs):
        """RemovePen(Pen pen)"""
        return _gdi.PenList_RemovePen(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount() -> int"""
        return _gdi.PenList_GetCount(*args, **kwargs)


class PenListPtr(PenList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PenList
_gdi.PenList_swigregister(PenListPtr)

class BrushList(core.Object):
    """"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBrushList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def AddBrush(*args, **kwargs):
        """AddBrush(Brush brush)"""
        return _gdi.BrushList_AddBrush(*args, **kwargs)

    def FindOrCreateBrush(*args, **kwargs):
        """FindOrCreateBrush(Colour colour, int style) -> Brush"""
        return _gdi.BrushList_FindOrCreateBrush(*args, **kwargs)

    def RemoveBrush(*args, **kwargs):
        """RemoveBrush(Brush brush)"""
        return _gdi.BrushList_RemoveBrush(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount() -> int"""
        return _gdi.BrushList_GetCount(*args, **kwargs)


class BrushListPtr(BrushList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BrushList
_gdi.BrushList_swigregister(BrushListPtr)

class ColourDatabase(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxColourDatabase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> ColourDatabase"""
        newobj = _gdi.new_ColourDatabase(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi.delete_ColourDatabase):
        """__del__()"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Find(*args, **kwargs):
        """Find(wxString name) -> Colour"""
        return _gdi.ColourDatabase_Find(*args, **kwargs)

    def FindName(*args, **kwargs):
        """FindName(Colour colour) -> wxString"""
        return _gdi.ColourDatabase_FindName(*args, **kwargs)

    FindColour = Find 
    def AddColour(*args, **kwargs):
        """AddColour(wxString name, Colour colour)"""
        return _gdi.ColourDatabase_AddColour(*args, **kwargs)

    def Append(*args, **kwargs):
        """Append(wxString name, int red, int green, int blue)"""
        return _gdi.ColourDatabase_Append(*args, **kwargs)


class ColourDatabasePtr(ColourDatabase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ColourDatabase
_gdi.ColourDatabase_swigregister(ColourDatabasePtr)

class FontList(core.Object):
    """"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFontList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def AddFont(*args, **kwargs):
        """AddFont(Font font)"""
        return _gdi.FontList_AddFont(*args, **kwargs)

    def FindOrCreateFont(*args, **kwargs):
        """FindOrCreateFont(int point_size, int family, int style, int weight, 
    bool underline=False, wxString facename=wxPyEmptyString, 
    wxFontEncoding encoding=FONTENCODING_DEFAULT) -> Font"""
        return _gdi.FontList_FindOrCreateFont(*args, **kwargs)

    def RemoveFont(*args, **kwargs):
        """RemoveFont(Font font)"""
        return _gdi.FontList_RemoveFont(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount() -> int"""
        return _gdi.FontList_GetCount(*args, **kwargs)


class FontListPtr(FontList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FontList
_gdi.FontList_swigregister(FontListPtr)

#---------------------------------------------------------------------------

#---------------------------------------------------------------------------

class Effects(core.Object):
    """"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEffects instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__() -> Effects"""
        newobj = _gdi.new_Effects(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetHighlightColour(*args, **kwargs):
        """GetHighlightColour() -> Colour"""
        return _gdi.Effects_GetHighlightColour(*args, **kwargs)

    def GetLightShadow(*args, **kwargs):
        """GetLightShadow() -> Colour"""
        return _gdi.Effects_GetLightShadow(*args, **kwargs)

    def GetFaceColour(*args, **kwargs):
        """GetFaceColour() -> Colour"""
        return _gdi.Effects_GetFaceColour(*args, **kwargs)

    def GetMediumShadow(*args, **kwargs):
        """GetMediumShadow() -> Colour"""
        return _gdi.Effects_GetMediumShadow(*args, **kwargs)

    def GetDarkShadow(*args, **kwargs):
        """GetDarkShadow() -> Colour"""
        return _gdi.Effects_GetDarkShadow(*args, **kwargs)

    def SetHighlightColour(*args, **kwargs):
        """SetHighlightColour(Colour c)"""
        return _gdi.Effects_SetHighlightColour(*args, **kwargs)

    def SetLightShadow(*args, **kwargs):
        """SetLightShadow(Colour c)"""
        return _gdi.Effects_SetLightShadow(*args, **kwargs)

    def SetFaceColour(*args, **kwargs):
        """SetFaceColour(Colour c)"""
        return _gdi.Effects_SetFaceColour(*args, **kwargs)

    def SetMediumShadow(*args, **kwargs):
        """SetMediumShadow(Colour c)"""
        return _gdi.Effects_SetMediumShadow(*args, **kwargs)

    def SetDarkShadow(*args, **kwargs):
        """SetDarkShadow(Colour c)"""
        return _gdi.Effects_SetDarkShadow(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(Colour highlightColour, Colour lightShadow, Colour faceColour, 
    Colour mediumShadow, Colour darkShadow)"""
        return _gdi.Effects_Set(*args, **kwargs)

    def DrawSunkenEdge(*args, **kwargs):
        """DrawSunkenEdge(DC dc, Rect rect, int borderSize=1)"""
        return _gdi.Effects_DrawSunkenEdge(*args, **kwargs)

    def TileBitmap(*args, **kwargs):
        """TileBitmap(Rect rect, DC dc, Bitmap bitmap) -> bool"""
        return _gdi.Effects_TileBitmap(*args, **kwargs)


class EffectsPtr(Effects):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Effects
_gdi.Effects_swigregister(EffectsPtr)
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


