# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.

import _gdi_

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


import _core
wx = _core 
#---------------------------------------------------------------------------

class GDIObject(_core.Object):
    """Proxy of C++ GDIObject class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxGDIObject instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> GDIObject"""
        newobj = _gdi_.new_GDIObject(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_GDIObject):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetVisible(*args, **kwargs):
        """GetVisible(self) -> bool"""
        return _gdi_.GDIObject_GetVisible(*args, **kwargs)

    def SetVisible(*args, **kwargs):
        """SetVisible(self, bool visible)"""
        return _gdi_.GDIObject_SetVisible(*args, **kwargs)

    def IsNull(*args, **kwargs):
        """IsNull(self) -> bool"""
        return _gdi_.GDIObject_IsNull(*args, **kwargs)


class GDIObjectPtr(GDIObject):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = GDIObject
_gdi_.GDIObject_swigregister(GDIObjectPtr)

#---------------------------------------------------------------------------

class Colour(_core.Object):
    """
    A colour is an object representing a combination of Red, Green, and
    Blue (RGB) intensity values, and is used to determine drawing colours,
    window colours, etc.  Valid RGB values are in the range 0 to 255.

    In wxPython there are typemaps that will automatically convert from a
    colour name, or from a '#RRGGBB' colour hex value string to a
    wx.Colour object when calling C++ methods that expect a wxColour.
    This means that the following are all equivallent::

        win.SetBackgroundColour(wxColour(0,0,255))
        win.SetBackgroundColour('BLUE')
        win.SetBackgroundColour('#0000FF')

    Additional colour names and their coresponding values can be added
    using `wx.ColourDatabase`.  Various system colours (as set in the
    user's system preferences) can be retrieved with
    `wx.SystemSettings.GetColour`.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxColour instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, byte red=0, byte green=0, byte blue=0) -> Colour

        Constructs a colour from red, green and blue values.

        :see: Alternate constructors `wx.NamedColour` and `wx.ColourRGB`.

        """
        newobj = _gdi_.new_Colour(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_Colour):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Red(*args, **kwargs):
        """
        Red(self) -> byte

        Returns the red intensity.
        """
        return _gdi_.Colour_Red(*args, **kwargs)

    def Green(*args, **kwargs):
        """
        Green(self) -> byte

        Returns the green intensity.
        """
        return _gdi_.Colour_Green(*args, **kwargs)

    def Blue(*args, **kwargs):
        """
        Blue(self) -> byte

        Returns the blue intensity.
        """
        return _gdi_.Colour_Blue(*args, **kwargs)

    def Ok(*args, **kwargs):
        """
        Ok(self) -> bool

        Returns True if the colour object is valid (the colour has been
        initialised with RGB values).
        """
        return _gdi_.Colour_Ok(*args, **kwargs)

    def Set(*args, **kwargs):
        """
        Set(self, byte red, byte green, byte blue)

        Sets the RGB intensity values.
        """
        return _gdi_.Colour_Set(*args, **kwargs)

    def SetRGB(*args, **kwargs):
        """
        SetRGB(self, unsigned long colRGB)

        Sets the RGB intensity values from a packed RGB value.
        """
        return _gdi_.Colour_SetRGB(*args, **kwargs)

    def SetFromName(*args, **kwargs):
        """
        SetFromName(self, String colourName)

        Sets the RGB intensity values using a colour name listed in
        ``wx.TheColourDatabase``.
        """
        return _gdi_.Colour_SetFromName(*args, **kwargs)

    def GetPixel(*args, **kwargs):
        """
        GetPixel(self) -> long

        Returns a pixel value which is platform-dependent. On Windows, a
        COLORREF is returned. On X, an allocated pixel value is returned.  -1
        is returned if the pixel is invalid (on X, unallocated).
        """
        return _gdi_.Colour_GetPixel(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """
        __eq__(self, Colour colour) -> bool

        Compare colours for equality
        """
        return _gdi_.Colour___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, Colour colour) -> bool

        Compare colours for inequality
        """
        return _gdi_.Colour___ne__(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> (r, g, b)

        Returns the RGB intensity values as a tuple.
        """
        return _gdi_.Colour_Get(*args, **kwargs)

    def GetRGB(*args, **kwargs):
        """
        GetRGB(self) -> unsigned long

        Return the colour as a packed RGB value
        """
        return _gdi_.Colour_GetRGB(*args, **kwargs)

    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                  return str(self.Get())
    def __repr__(self):                 return 'wx.Colour' + str(self.Get())
    def __nonzero__(self):              return self.Ok()
    __safe_for_unpickling__ = True
    def __reduce__(self):               return (Colour, self.Get())


class ColourPtr(Colour):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Colour
_gdi_.Colour_swigregister(ColourPtr)

def NamedColour(*args, **kwargs):
    """
    NamedColour(String colorName) -> Colour

    Constructs a colour object using a colour name listed in
    ``wx.TheColourDatabase``.
    """
    val = _gdi_.new_NamedColour(*args, **kwargs)
    val.thisown = 1
    return val

def ColourRGB(*args, **kwargs):
    """
    ColourRGB(unsigned long colRGB) -> Colour

    Constructs a colour from a packed RGB value.
    """
    val = _gdi_.new_ColourRGB(*args, **kwargs)
    val.thisown = 1
    return val

Color = Colour
NamedColor = NamedColour
ColorRGB = ColourRGB

class Palette(GDIObject):
    """Proxy of C++ Palette class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPalette instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int n, unsigned char red, unsigned char green, unsigned char blue) -> Palette"""
        newobj = _gdi_.new_Palette(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_Palette):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetPixel(*args, **kwargs):
        """GetPixel(self, byte red, byte green, byte blue) -> int"""
        return _gdi_.Palette_GetPixel(*args, **kwargs)

    def GetRGB(*args, **kwargs):
        """GetRGB(self, int pixel) -> (R,G,B)"""
        return _gdi_.Palette_GetRGB(*args, **kwargs)

    def GetColoursCount(*args, **kwargs):
        """GetColoursCount(self) -> int"""
        return _gdi_.Palette_GetColoursCount(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _gdi_.Palette_Ok(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class PalettePtr(Palette):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Palette
_gdi_.Palette_swigregister(PalettePtr)

#---------------------------------------------------------------------------

class Pen(GDIObject):
    """Proxy of C++ Pen class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPen instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Colour colour, int width=1, int style=SOLID) -> Pen"""
        newobj = _gdi_.new_Pen(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_Pen):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetCap(*args, **kwargs):
        """GetCap(self) -> int"""
        return _gdi_.Pen_GetCap(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """GetColour(self) -> Colour"""
        return _gdi_.Pen_GetColour(*args, **kwargs)

    def GetJoin(*args, **kwargs):
        """GetJoin(self) -> int"""
        return _gdi_.Pen_GetJoin(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self) -> int"""
        return _gdi_.Pen_GetStyle(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _gdi_.Pen_GetWidth(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _gdi_.Pen_Ok(*args, **kwargs)

    def SetCap(*args, **kwargs):
        """SetCap(self, int cap_style)"""
        return _gdi_.Pen_SetCap(*args, **kwargs)

    def SetColour(*args, **kwargs):
        """SetColour(self, Colour colour)"""
        return _gdi_.Pen_SetColour(*args, **kwargs)

    def SetJoin(*args, **kwargs):
        """SetJoin(self, int join_style)"""
        return _gdi_.Pen_SetJoin(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(self, int style)"""
        return _gdi_.Pen_SetStyle(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(self, int width)"""
        return _gdi_.Pen_SetWidth(*args, **kwargs)

    def SetDashes(*args, **kwargs):
        """SetDashes(self, int dashes)"""
        return _gdi_.Pen_SetDashes(*args, **kwargs)

    def GetDashes(*args, **kwargs):
        """GetDashes(self) -> PyObject"""
        return _gdi_.Pen_GetDashes(*args, **kwargs)

    def _SetDashes(*args, **kwargs):
        """_SetDashes(self, PyObject _self, PyObject pyDashes)"""
        return _gdi_.Pen__SetDashes(*args, **kwargs)

    def SetDashes(self, dashes):
        """
        Associate a list of dash lengths with the Pen.
        """
        self._SetDashes(self, dashes)

    def __eq__(*args, **kwargs):
        """__eq__(self, Pen other) -> bool"""
        return _gdi_.Pen___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, Pen other) -> bool"""
        return _gdi_.Pen___ne__(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class PenPtr(Pen):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Pen
_gdi_.Pen_swigregister(PenPtr)

#---------------------------------------------------------------------------

class Brush(GDIObject):
    """
    A brush is a drawing tool for filling in areas. It is used for
    painting the background of rectangles, ellipses, etc. when drawing on
    a `wx.DC`.  It has a colour and a style.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBrush instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Colour colour, int style=SOLID) -> Brush

        Constructs a brush from a `wx.Colour` object and a style.
        """
        newobj = _gdi_.new_Brush(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_Brush):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def SetColour(*args, **kwargs):
        """
        SetColour(self, Colour col)

        Set the brush's `wx.Colour`.
        """
        return _gdi_.Brush_SetColour(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """
        SetStyle(self, int style)

        Sets the style of the brush. See `__init__` for a listing of styles.
        """
        return _gdi_.Brush_SetStyle(*args, **kwargs)

    def SetStipple(*args, **kwargs):
        """
        SetStipple(self, Bitmap stipple)

        Sets the stipple `wx.Bitmap`.
        """
        return _gdi_.Brush_SetStipple(*args, **kwargs)

    def GetColour(*args, **kwargs):
        """
        GetColour(self) -> Colour

        Returns the `wx.Colour` of the brush.
        """
        return _gdi_.Brush_GetColour(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """
        GetStyle(self) -> int

        Returns the style of the brush.  See `__init__` for a listing of
        styles.
        """
        return _gdi_.Brush_GetStyle(*args, **kwargs)

    def GetStipple(*args, **kwargs):
        """
        GetStipple(self) -> Bitmap

        Returns the stiple `wx.Bitmap` of the brush.  If the brush does not
        have a wx.STIPPLE style, then the return value may be non-None but an
        uninitialised bitmap (`wx.Bitmap.Ok` returns False).
        """
        return _gdi_.Brush_GetStipple(*args, **kwargs)

    def IsHatch(*args, **kwargs):
        """
        IsHatch(self) -> bool

        Is the current style a hatch type?
        """
        return _gdi_.Brush_IsHatch(*args, **kwargs)

    def Ok(*args, **kwargs):
        """
        Ok(self) -> bool

        Returns True if the brush is initialised and valid.
        """
        return _gdi_.Brush_Ok(*args, **kwargs)

    def MacGetTheme(*args, **kwargs):
        """MacGetTheme(self) -> short"""
        return _gdi_.Brush_MacGetTheme(*args, **kwargs)

    def MacSetTheme(*args, **kwargs):
        """MacSetTheme(self, short macThemeBrush)"""
        return _gdi_.Brush_MacSetTheme(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class BrushPtr(Brush):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Brush
_gdi_.Brush_swigregister(BrushPtr)

class Bitmap(GDIObject):
    """
    The wx.Bitmap class encapsulates the concept of a platform-dependent
    bitmap.  It can be either monochrome or colour, and either loaded from
    a file or created dynamically.  A bitmap can be selected into a memory
    device context (instance of `wx.MemoryDC`). This enables the bitmap to
    be copied to a window or memory device context using `wx.DC.Blit`, or
    to be used as a drawing surface.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBitmap instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, String name, int type=BITMAP_TYPE_ANY) -> Bitmap

        Loads a bitmap from a file.
        """
        newobj = _gdi_.new_Bitmap(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_Bitmap):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _gdi_.Bitmap_Ok(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """
        GetWidth(self) -> int

        Gets the width of the bitmap in pixels.
        """
        return _gdi_.Bitmap_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """
        GetHeight(self) -> int

        Gets the height of the bitmap in pixels.
        """
        return _gdi_.Bitmap_GetHeight(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """
        GetDepth(self) -> int

        Gets the colour depth of the bitmap. A value of 1 indicates a
        monochrome bitmap.
        """
        return _gdi_.Bitmap_GetDepth(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> Size

        Get the size of the bitmap.
        """
        return _gdi_.Bitmap_GetSize(*args, **kwargs)

    def ConvertToImage(*args, **kwargs):
        """
        ConvertToImage(self) -> Image

        Creates a platform-independent image from a platform-dependent
        bitmap. This preserves mask information so that bitmaps and images can
        be converted back and forth without loss in that respect.
        """
        return _gdi_.Bitmap_ConvertToImage(*args, **kwargs)

    def GetMask(*args, **kwargs):
        """
        GetMask(self) -> Mask

        Gets the associated mask (if any) which may have been loaded from a
        file or explpicitly set for the bitmap.

        :see: `SetMask`, `wx.Mask`

        """
        return _gdi_.Bitmap_GetMask(*args, **kwargs)

    def SetMask(*args, **kwargs):
        """
        SetMask(self, Mask mask)

        Sets the mask for this bitmap.

        :see: `GetMask`, `wx.Mask`

        """
        return _gdi_.Bitmap_SetMask(*args, **kwargs)

    def SetMaskColour(*args, **kwargs):
        """
        SetMaskColour(self, Colour colour)

        Create a Mask based on a specified colour in the Bitmap.
        """
        return _gdi_.Bitmap_SetMaskColour(*args, **kwargs)

    def GetSubBitmap(*args, **kwargs):
        """
        GetSubBitmap(self, Rect rect) -> Bitmap

        Returns a sub-bitmap of the current one as long as the rect belongs
        entirely to the bitmap. This function preserves bit depth and mask
        information.
        """
        return _gdi_.Bitmap_GetSubBitmap(*args, **kwargs)

    def SaveFile(*args, **kwargs):
        """
        SaveFile(self, String name, int type, Palette palette=None) -> bool

        Saves a bitmap in the named file.  See `__init__` for a description of
        the ``type`` parameter.
        """
        return _gdi_.Bitmap_SaveFile(*args, **kwargs)

    def LoadFile(*args, **kwargs):
        """
        LoadFile(self, String name, int type) -> bool

        Loads a bitmap from a file.  See `__init__` for a description of the
        ``type`` parameter.
        """
        return _gdi_.Bitmap_LoadFile(*args, **kwargs)

    def GetPalette(*args, **kwargs):
        """GetPalette(self) -> Palette"""
        return _gdi_.Bitmap_GetPalette(*args, **kwargs)

    def CopyFromIcon(*args, **kwargs):
        """CopyFromIcon(self, Icon icon) -> bool"""
        return _gdi_.Bitmap_CopyFromIcon(*args, **kwargs)

    def SetHeight(*args, **kwargs):
        """
        SetHeight(self, int height)

        Set the height property (does not affect the existing bitmap data).
        """
        return _gdi_.Bitmap_SetHeight(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """
        SetWidth(self, int width)

        Set the width property (does not affect the existing bitmap data).
        """
        return _gdi_.Bitmap_SetWidth(*args, **kwargs)

    def SetDepth(*args, **kwargs):
        """
        SetDepth(self, int depth)

        Set the depth property (does not affect the existing bitmap data).
        """
        return _gdi_.Bitmap_SetDepth(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """
        SetSize(self, Size size)

        Set the bitmap size (does not affect the existing bitmap data).
        """
        return _gdi_.Bitmap_SetSize(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
    def __eq__(*args, **kwargs):
        """__eq__(self, Bitmap other) -> bool"""
        return _gdi_.Bitmap___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, Bitmap other) -> bool"""
        return _gdi_.Bitmap___ne__(*args, **kwargs)


class BitmapPtr(Bitmap):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Bitmap
_gdi_.Bitmap_swigregister(BitmapPtr)

def EmptyBitmap(*args, **kwargs):
    """
    EmptyBitmap(int width, int height, int depth=-1) -> Bitmap

    Creates a new bitmap of the given size.  A depth of -1 indicates the
    depth of the current screen or visual. Some platforms only support 1
    for monochrome and -1 for the current colour setting.
    """
    val = _gdi_.new_EmptyBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromIcon(*args, **kwargs):
    """
    BitmapFromIcon(Icon icon) -> Bitmap

    Create a new bitmap from a `wx.Icon` object.
    """
    val = _gdi_.new_BitmapFromIcon(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromImage(*args, **kwargs):
    """
    BitmapFromImage(Image image, int depth=-1) -> Bitmap

    Creates bitmap object from a `wx.Image`. This has to be done to
    actually display a `wx.Image` as you cannot draw an image directly on
    a window. The resulting bitmap will use the provided colour depth (or
    that of the current screen colour depth if depth is -1) which entails
    that a colour reduction may have to take place.
    """
    val = _gdi_.new_BitmapFromImage(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromXPMData(*args, **kwargs):
    """
    BitmapFromXPMData(PyObject listOfStrings) -> Bitmap

    Construct a Bitmap from a list of strings formatted as XPM data.
    """
    val = _gdi_.new_BitmapFromXPMData(*args, **kwargs)
    val.thisown = 1
    return val

def BitmapFromBits(*args, **kwargs):
    """
    BitmapFromBits(PyObject bits, int width, int height, int depth=1) -> Bitmap

    Creates a bitmap from an array of bits.  You should only use this
    function for monochrome bitmaps (depth 1) in portable programs: in
    this case the bits parameter should contain an XBM image.  For other
    bit depths, the behaviour is platform dependent.
    """
    val = _gdi_.new_BitmapFromBits(*args, **kwargs)
    val.thisown = 1
    return val

class Mask(_core.Object):
    """
    This class encapsulates a monochrome mask bitmap, where the masked
    area is black and the unmasked area is white. When associated with a
    bitmap and drawn in a device context, the unmasked area of the bitmap
    will be drawn, and the masked area will not be drawn.

    A mask may be associated with a `wx.Bitmap`. It is used in
    `wx.DC.DrawBitmap` or `wx.DC.Blit` when the source device context is a
    `wx.MemoryDC` with a `wx.Bitmap` selected into it that contains a
    mask.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMask instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Bitmap bitmap, Colour colour=NullColour) -> Mask

        Constructs a mask from a `wx.Bitmap` and a `wx.Colour` in that bitmap
        that indicates the transparent portions of the mask.  In other words,
        the pixels in ``bitmap`` that match ``colour`` will be the transparent
        portions of the mask.  If no ``colour`` or an invalid ``colour`` is
        passed then BLACK is used.

        :see: `wx.Bitmap`, `wx.Colour`
        """
        newobj = _gdi_.new_Mask(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class MaskPtr(Mask):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Mask
_gdi_.Mask_swigregister(MaskPtr)

MaskColour = wx._deprecated(Mask, "wx.MaskColour is deprecated, use `wx.Mask` instead.") 
class Icon(GDIObject):
    """Proxy of C++ Icon class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIcon instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String name, int type, int desiredWidth=-1, int desiredHeight=-1) -> Icon"""
        newobj = _gdi_.new_Icon(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_Icon):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _gdi_.Icon_Ok(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _gdi_.Icon_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return _gdi_.Icon_GetHeight(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """GetDepth(self) -> int"""
        return _gdi_.Icon_GetDepth(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(self, int w)"""
        return _gdi_.Icon_SetWidth(*args, **kwargs)

    def SetHeight(*args, **kwargs):
        """SetHeight(self, int h)"""
        return _gdi_.Icon_SetHeight(*args, **kwargs)

    def SetDepth(*args, **kwargs):
        """SetDepth(self, int d)"""
        return _gdi_.Icon_SetDepth(*args, **kwargs)

    def CopyFromBitmap(*args, **kwargs):
        """CopyFromBitmap(self, Bitmap bmp)"""
        return _gdi_.Icon_CopyFromBitmap(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class IconPtr(Icon):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Icon
_gdi_.Icon_swigregister(IconPtr)

def EmptyIcon(*args, **kwargs):
    """EmptyIcon() -> Icon"""
    val = _gdi_.new_EmptyIcon(*args, **kwargs)
    val.thisown = 1
    return val

def IconFromLocation(*args, **kwargs):
    """IconFromLocation(IconLocation loc) -> Icon"""
    val = _gdi_.new_IconFromLocation(*args, **kwargs)
    val.thisown = 1
    return val

def IconFromBitmap(*args, **kwargs):
    """IconFromBitmap(Bitmap bmp) -> Icon"""
    val = _gdi_.new_IconFromBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def IconFromXPMData(*args, **kwargs):
    """IconFromXPMData(PyObject listOfStrings) -> Icon"""
    val = _gdi_.new_IconFromXPMData(*args, **kwargs)
    val.thisown = 1
    return val

class IconLocation(object):
    """Proxy of C++ IconLocation class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIconLocation instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String filename=&wxPyEmptyString, int num=0) -> IconLocation"""
        newobj = _gdi_.new_IconLocation(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_IconLocation):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _gdi_.IconLocation_IsOk(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
    def SetFileName(*args, **kwargs):
        """SetFileName(self, String filename)"""
        return _gdi_.IconLocation_SetFileName(*args, **kwargs)

    def GetFileName(*args, **kwargs):
        """GetFileName(self) -> String"""
        return _gdi_.IconLocation_GetFileName(*args, **kwargs)

    def SetIndex(*args, **kwargs):
        """SetIndex(self, int num)"""
        return _gdi_.IconLocation_SetIndex(*args, **kwargs)

    def GetIndex(*args, **kwargs):
        """GetIndex(self) -> int"""
        return _gdi_.IconLocation_GetIndex(*args, **kwargs)


class IconLocationPtr(IconLocation):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IconLocation
_gdi_.IconLocation_swigregister(IconLocationPtr)

class IconBundle(object):
    """Proxy of C++ IconBundle class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxIconBundle instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> IconBundle"""
        newobj = _gdi_.new_IconBundle(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_IconBundle):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def AddIcon(*args, **kwargs):
        """AddIcon(self, Icon icon)"""
        return _gdi_.IconBundle_AddIcon(*args, **kwargs)

    def AddIconFromFile(*args, **kwargs):
        """AddIconFromFile(self, String file, long type)"""
        return _gdi_.IconBundle_AddIconFromFile(*args, **kwargs)

    def GetIcon(*args, **kwargs):
        """GetIcon(self, Size size) -> Icon"""
        return _gdi_.IconBundle_GetIcon(*args, **kwargs)


class IconBundlePtr(IconBundle):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = IconBundle
_gdi_.IconBundle_swigregister(IconBundlePtr)

def IconBundleFromFile(*args, **kwargs):
    """IconBundleFromFile(String file, long type) -> IconBundle"""
    val = _gdi_.new_IconBundleFromFile(*args, **kwargs)
    val.thisown = 1
    return val

def IconBundleFromIcon(*args, **kwargs):
    """IconBundleFromIcon(Icon icon) -> IconBundle"""
    val = _gdi_.new_IconBundleFromIcon(*args, **kwargs)
    val.thisown = 1
    return val

class Cursor(GDIObject):
    """
    A cursor is a small bitmap usually used for denoting where the mouse
    pointer is, with a picture that might indicate the interpretation of a
    mouse click.

    A single cursor object may be used in many windows (any subwindow
    type). The wxWindows convention is to set the cursor for a window, as
    in X, rather than to set it globally as in MS Windows, although a
    global `wx.SetCursor` function is also available for use on MS Windows.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxCursor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, String cursorName, long type, int hotSpotX=0, int hotSpotY=0) -> Cursor

        Construct a Cursor from a file.  Specify the type of file using
        wx.BITAMP_TYPE* constants, and specify the hotspot if not using a cur
        file.

        This constructor is not available on wxGTK, use ``wx.StockCursor``,
        ``wx.CursorFromImage``, or ``wx.CursorFromBits`` instead.
        """
        newobj = _gdi_.new_Cursor(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_Cursor):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _gdi_.Cursor_Ok(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class CursorPtr(Cursor):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Cursor
_gdi_.Cursor_swigregister(CursorPtr)

def StockCursor(*args, **kwargs):
    """
    StockCursor(int id) -> Cursor

    Create a cursor using one of the stock cursors.  Note that not all
    cursors are available on all platforms.
    """
    val = _gdi_.new_StockCursor(*args, **kwargs)
    val.thisown = 1
    return val

def CursorFromImage(*args, **kwargs):
    """
    CursorFromImage(Image image) -> Cursor

    Constructs a cursor from a wxImage. The cursor is monochrome, colors
    with the RGB elements all greater than 127 will be foreground, colors
    less than this background. The mask (if any) will be used as
    transparent.
    """
    val = _gdi_.new_CursorFromImage(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

OutRegion = _gdi_.OutRegion
PartRegion = _gdi_.PartRegion
InRegion = _gdi_.InRegion
class Region(GDIObject):
    """Proxy of C++ Region class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRegion instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int x=0, int y=0, int width=0, int height=0) -> Region"""
        newobj = _gdi_.new_Region(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_Region):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Clear(*args, **kwargs):
        """Clear(self)"""
        return _gdi_.Region_Clear(*args, **kwargs)

    def Offset(*args, **kwargs):
        """Offset(self, int x, int y) -> bool"""
        return _gdi_.Region_Offset(*args, **kwargs)

    def Contains(*args, **kwargs):
        """Contains(self, int x, int y) -> int"""
        return _gdi_.Region_Contains(*args, **kwargs)

    def ContainsPoint(*args, **kwargs):
        """ContainsPoint(self, Point pt) -> int"""
        return _gdi_.Region_ContainsPoint(*args, **kwargs)

    def ContainsRect(*args, **kwargs):
        """ContainsRect(self, Rect rect) -> int"""
        return _gdi_.Region_ContainsRect(*args, **kwargs)

    def ContainsRectDim(*args, **kwargs):
        """ContainsRectDim(self, int x, int y, int w, int h) -> int"""
        return _gdi_.Region_ContainsRectDim(*args, **kwargs)

    def GetBox(*args, **kwargs):
        """GetBox(self) -> Rect"""
        return _gdi_.Region_GetBox(*args, **kwargs)

    def Intersect(*args, **kwargs):
        """Intersect(self, int x, int y, int width, int height) -> bool"""
        return _gdi_.Region_Intersect(*args, **kwargs)

    def IntersectRect(*args, **kwargs):
        """IntersectRect(self, Rect rect) -> bool"""
        return _gdi_.Region_IntersectRect(*args, **kwargs)

    def IntersectRegion(*args, **kwargs):
        """IntersectRegion(self, Region region) -> bool"""
        return _gdi_.Region_IntersectRegion(*args, **kwargs)

    def IsEmpty(*args, **kwargs):
        """IsEmpty(self) -> bool"""
        return _gdi_.Region_IsEmpty(*args, **kwargs)

    def Union(*args, **kwargs):
        """Union(self, int x, int y, int width, int height) -> bool"""
        return _gdi_.Region_Union(*args, **kwargs)

    def UnionRect(*args, **kwargs):
        """UnionRect(self, Rect rect) -> bool"""
        return _gdi_.Region_UnionRect(*args, **kwargs)

    def UnionRegion(*args, **kwargs):
        """UnionRegion(self, Region region) -> bool"""
        return _gdi_.Region_UnionRegion(*args, **kwargs)

    def Subtract(*args, **kwargs):
        """Subtract(self, int x, int y, int width, int height) -> bool"""
        return _gdi_.Region_Subtract(*args, **kwargs)

    def SubtractRect(*args, **kwargs):
        """SubtractRect(self, Rect rect) -> bool"""
        return _gdi_.Region_SubtractRect(*args, **kwargs)

    def SubtractRegion(*args, **kwargs):
        """SubtractRegion(self, Region region) -> bool"""
        return _gdi_.Region_SubtractRegion(*args, **kwargs)

    def Xor(*args, **kwargs):
        """Xor(self, int x, int y, int width, int height) -> bool"""
        return _gdi_.Region_Xor(*args, **kwargs)

    def XorRect(*args, **kwargs):
        """XorRect(self, Rect rect) -> bool"""
        return _gdi_.Region_XorRect(*args, **kwargs)

    def XorRegion(*args, **kwargs):
        """XorRegion(self, Region region) -> bool"""
        return _gdi_.Region_XorRegion(*args, **kwargs)

    def ConvertToBitmap(*args, **kwargs):
        """ConvertToBitmap(self) -> Bitmap"""
        return _gdi_.Region_ConvertToBitmap(*args, **kwargs)

    def UnionBitmap(*args, **kwargs):
        """UnionBitmap(self, Bitmap bmp) -> bool"""
        return _gdi_.Region_UnionBitmap(*args, **kwargs)

    def UnionBitmapColour(*args, **kwargs):
        """UnionBitmapColour(self, Bitmap bmp, Colour transColour, int tolerance=0) -> bool"""
        return _gdi_.Region_UnionBitmapColour(*args, **kwargs)


class RegionPtr(Region):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Region
_gdi_.Region_swigregister(RegionPtr)

def RegionFromBitmap(*args, **kwargs):
    """RegionFromBitmap(Bitmap bmp) -> Region"""
    val = _gdi_.new_RegionFromBitmap(*args, **kwargs)
    val.thisown = 1
    return val

def RegionFromBitmapColour(*args, **kwargs):
    """RegionFromBitmapColour(Bitmap bmp, Colour transColour, int tolerance=0) -> Region"""
    val = _gdi_.new_RegionFromBitmapColour(*args, **kwargs)
    val.thisown = 1
    return val

def RegionFromPoints(*args, **kwargs):
    """RegionFromPoints(int points, int fillStyle=WINDING_RULE) -> Region"""
    val = _gdi_.new_RegionFromPoints(*args, **kwargs)
    val.thisown = 1
    return val

class RegionIterator(_core.Object):
    """Proxy of C++ RegionIterator class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxRegionIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, Region region) -> RegionIterator"""
        newobj = _gdi_.new_RegionIterator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_RegionIterator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def GetX(*args, **kwargs):
        """GetX(self) -> int"""
        return _gdi_.RegionIterator_GetX(*args, **kwargs)

    def GetY(*args, **kwargs):
        """GetY(self) -> int"""
        return _gdi_.RegionIterator_GetY(*args, **kwargs)

    def GetW(*args, **kwargs):
        """GetW(self) -> int"""
        return _gdi_.RegionIterator_GetW(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _gdi_.RegionIterator_GetWidth(*args, **kwargs)

    def GetH(*args, **kwargs):
        """GetH(self) -> int"""
        return _gdi_.RegionIterator_GetH(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return _gdi_.RegionIterator_GetHeight(*args, **kwargs)

    def GetRect(*args, **kwargs):
        """GetRect(self) -> Rect"""
        return _gdi_.RegionIterator_GetRect(*args, **kwargs)

    def HaveRects(*args, **kwargs):
        """HaveRects(self) -> bool"""
        return _gdi_.RegionIterator_HaveRects(*args, **kwargs)

    def Reset(*args, **kwargs):
        """Reset(self)"""
        return _gdi_.RegionIterator_Reset(*args, **kwargs)

    def Next(*args, **kwargs):
        """Next(self)"""
        return _gdi_.RegionIterator_Next(*args, **kwargs)

    def __nonzero__(*args, **kwargs):
        """__nonzero__(self) -> bool"""
        return _gdi_.RegionIterator___nonzero__(*args, **kwargs)


class RegionIteratorPtr(RegionIterator):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = RegionIterator
_gdi_.RegionIterator_swigregister(RegionIteratorPtr)

#---------------------------------------------------------------------------

FONTFAMILY_DEFAULT = _gdi_.FONTFAMILY_DEFAULT
FONTFAMILY_DECORATIVE = _gdi_.FONTFAMILY_DECORATIVE
FONTFAMILY_ROMAN = _gdi_.FONTFAMILY_ROMAN
FONTFAMILY_SCRIPT = _gdi_.FONTFAMILY_SCRIPT
FONTFAMILY_SWISS = _gdi_.FONTFAMILY_SWISS
FONTFAMILY_MODERN = _gdi_.FONTFAMILY_MODERN
FONTFAMILY_TELETYPE = _gdi_.FONTFAMILY_TELETYPE
FONTFAMILY_MAX = _gdi_.FONTFAMILY_MAX
FONTFAMILY_UNKNOWN = _gdi_.FONTFAMILY_UNKNOWN
FONTSTYLE_NORMAL = _gdi_.FONTSTYLE_NORMAL
FONTSTYLE_ITALIC = _gdi_.FONTSTYLE_ITALIC
FONTSTYLE_SLANT = _gdi_.FONTSTYLE_SLANT
FONTSTYLE_MAX = _gdi_.FONTSTYLE_MAX
FONTWEIGHT_NORMAL = _gdi_.FONTWEIGHT_NORMAL
FONTWEIGHT_LIGHT = _gdi_.FONTWEIGHT_LIGHT
FONTWEIGHT_BOLD = _gdi_.FONTWEIGHT_BOLD
FONTWEIGHT_MAX = _gdi_.FONTWEIGHT_MAX
FONTFLAG_DEFAULT = _gdi_.FONTFLAG_DEFAULT
FONTFLAG_ITALIC = _gdi_.FONTFLAG_ITALIC
FONTFLAG_SLANT = _gdi_.FONTFLAG_SLANT
FONTFLAG_LIGHT = _gdi_.FONTFLAG_LIGHT
FONTFLAG_BOLD = _gdi_.FONTFLAG_BOLD
FONTFLAG_ANTIALIASED = _gdi_.FONTFLAG_ANTIALIASED
FONTFLAG_NOT_ANTIALIASED = _gdi_.FONTFLAG_NOT_ANTIALIASED
FONTFLAG_UNDERLINED = _gdi_.FONTFLAG_UNDERLINED
FONTFLAG_STRIKETHROUGH = _gdi_.FONTFLAG_STRIKETHROUGH
FONTFLAG_MASK = _gdi_.FONTFLAG_MASK
FONTENCODING_SYSTEM = _gdi_.FONTENCODING_SYSTEM
FONTENCODING_DEFAULT = _gdi_.FONTENCODING_DEFAULT
FONTENCODING_ISO8859_1 = _gdi_.FONTENCODING_ISO8859_1
FONTENCODING_ISO8859_2 = _gdi_.FONTENCODING_ISO8859_2
FONTENCODING_ISO8859_3 = _gdi_.FONTENCODING_ISO8859_3
FONTENCODING_ISO8859_4 = _gdi_.FONTENCODING_ISO8859_4
FONTENCODING_ISO8859_5 = _gdi_.FONTENCODING_ISO8859_5
FONTENCODING_ISO8859_6 = _gdi_.FONTENCODING_ISO8859_6
FONTENCODING_ISO8859_7 = _gdi_.FONTENCODING_ISO8859_7
FONTENCODING_ISO8859_8 = _gdi_.FONTENCODING_ISO8859_8
FONTENCODING_ISO8859_9 = _gdi_.FONTENCODING_ISO8859_9
FONTENCODING_ISO8859_10 = _gdi_.FONTENCODING_ISO8859_10
FONTENCODING_ISO8859_11 = _gdi_.FONTENCODING_ISO8859_11
FONTENCODING_ISO8859_12 = _gdi_.FONTENCODING_ISO8859_12
FONTENCODING_ISO8859_13 = _gdi_.FONTENCODING_ISO8859_13
FONTENCODING_ISO8859_14 = _gdi_.FONTENCODING_ISO8859_14
FONTENCODING_ISO8859_15 = _gdi_.FONTENCODING_ISO8859_15
FONTENCODING_ISO8859_MAX = _gdi_.FONTENCODING_ISO8859_MAX
FONTENCODING_KOI8 = _gdi_.FONTENCODING_KOI8
FONTENCODING_KOI8_U = _gdi_.FONTENCODING_KOI8_U
FONTENCODING_ALTERNATIVE = _gdi_.FONTENCODING_ALTERNATIVE
FONTENCODING_BULGARIAN = _gdi_.FONTENCODING_BULGARIAN
FONTENCODING_CP437 = _gdi_.FONTENCODING_CP437
FONTENCODING_CP850 = _gdi_.FONTENCODING_CP850
FONTENCODING_CP852 = _gdi_.FONTENCODING_CP852
FONTENCODING_CP855 = _gdi_.FONTENCODING_CP855
FONTENCODING_CP866 = _gdi_.FONTENCODING_CP866
FONTENCODING_CP874 = _gdi_.FONTENCODING_CP874
FONTENCODING_CP932 = _gdi_.FONTENCODING_CP932
FONTENCODING_CP936 = _gdi_.FONTENCODING_CP936
FONTENCODING_CP949 = _gdi_.FONTENCODING_CP949
FONTENCODING_CP950 = _gdi_.FONTENCODING_CP950
FONTENCODING_CP1250 = _gdi_.FONTENCODING_CP1250
FONTENCODING_CP1251 = _gdi_.FONTENCODING_CP1251
FONTENCODING_CP1252 = _gdi_.FONTENCODING_CP1252
FONTENCODING_CP1253 = _gdi_.FONTENCODING_CP1253
FONTENCODING_CP1254 = _gdi_.FONTENCODING_CP1254
FONTENCODING_CP1255 = _gdi_.FONTENCODING_CP1255
FONTENCODING_CP1256 = _gdi_.FONTENCODING_CP1256
FONTENCODING_CP1257 = _gdi_.FONTENCODING_CP1257
FONTENCODING_CP12_MAX = _gdi_.FONTENCODING_CP12_MAX
FONTENCODING_UTF7 = _gdi_.FONTENCODING_UTF7
FONTENCODING_UTF8 = _gdi_.FONTENCODING_UTF8
FONTENCODING_EUC_JP = _gdi_.FONTENCODING_EUC_JP
FONTENCODING_UTF16BE = _gdi_.FONTENCODING_UTF16BE
FONTENCODING_UTF16LE = _gdi_.FONTENCODING_UTF16LE
FONTENCODING_UTF32BE = _gdi_.FONTENCODING_UTF32BE
FONTENCODING_UTF32LE = _gdi_.FONTENCODING_UTF32LE
FONTENCODING_MACROMAN = _gdi_.FONTENCODING_MACROMAN
FONTENCODING_MACJAPANESE = _gdi_.FONTENCODING_MACJAPANESE
FONTENCODING_MACCHINESETRAD = _gdi_.FONTENCODING_MACCHINESETRAD
FONTENCODING_MACKOREAN = _gdi_.FONTENCODING_MACKOREAN
FONTENCODING_MACARABIC = _gdi_.FONTENCODING_MACARABIC
FONTENCODING_MACHEBREW = _gdi_.FONTENCODING_MACHEBREW
FONTENCODING_MACGREEK = _gdi_.FONTENCODING_MACGREEK
FONTENCODING_MACCYRILLIC = _gdi_.FONTENCODING_MACCYRILLIC
FONTENCODING_MACDEVANAGARI = _gdi_.FONTENCODING_MACDEVANAGARI
FONTENCODING_MACGURMUKHI = _gdi_.FONTENCODING_MACGURMUKHI
FONTENCODING_MACGUJARATI = _gdi_.FONTENCODING_MACGUJARATI
FONTENCODING_MACORIYA = _gdi_.FONTENCODING_MACORIYA
FONTENCODING_MACBENGALI = _gdi_.FONTENCODING_MACBENGALI
FONTENCODING_MACTAMIL = _gdi_.FONTENCODING_MACTAMIL
FONTENCODING_MACTELUGU = _gdi_.FONTENCODING_MACTELUGU
FONTENCODING_MACKANNADA = _gdi_.FONTENCODING_MACKANNADA
FONTENCODING_MACMALAJALAM = _gdi_.FONTENCODING_MACMALAJALAM
FONTENCODING_MACSINHALESE = _gdi_.FONTENCODING_MACSINHALESE
FONTENCODING_MACBURMESE = _gdi_.FONTENCODING_MACBURMESE
FONTENCODING_MACKHMER = _gdi_.FONTENCODING_MACKHMER
FONTENCODING_MACTHAI = _gdi_.FONTENCODING_MACTHAI
FONTENCODING_MACLAOTIAN = _gdi_.FONTENCODING_MACLAOTIAN
FONTENCODING_MACGEORGIAN = _gdi_.FONTENCODING_MACGEORGIAN
FONTENCODING_MACARMENIAN = _gdi_.FONTENCODING_MACARMENIAN
FONTENCODING_MACCHINESESIMP = _gdi_.FONTENCODING_MACCHINESESIMP
FONTENCODING_MACTIBETAN = _gdi_.FONTENCODING_MACTIBETAN
FONTENCODING_MACMONGOLIAN = _gdi_.FONTENCODING_MACMONGOLIAN
FONTENCODING_MACETHIOPIC = _gdi_.FONTENCODING_MACETHIOPIC
FONTENCODING_MACCENTRALEUR = _gdi_.FONTENCODING_MACCENTRALEUR
FONTENCODING_MACVIATNAMESE = _gdi_.FONTENCODING_MACVIATNAMESE
FONTENCODING_MACARABICEXT = _gdi_.FONTENCODING_MACARABICEXT
FONTENCODING_MACSYMBOL = _gdi_.FONTENCODING_MACSYMBOL
FONTENCODING_MACDINGBATS = _gdi_.FONTENCODING_MACDINGBATS
FONTENCODING_MACTURKISH = _gdi_.FONTENCODING_MACTURKISH
FONTENCODING_MACCROATIAN = _gdi_.FONTENCODING_MACCROATIAN
FONTENCODING_MACICELANDIC = _gdi_.FONTENCODING_MACICELANDIC
FONTENCODING_MACROMANIAN = _gdi_.FONTENCODING_MACROMANIAN
FONTENCODING_MACCELTIC = _gdi_.FONTENCODING_MACCELTIC
FONTENCODING_MACGAELIC = _gdi_.FONTENCODING_MACGAELIC
FONTENCODING_MACKEYBOARD = _gdi_.FONTENCODING_MACKEYBOARD
FONTENCODING_MACMIN = _gdi_.FONTENCODING_MACMIN
FONTENCODING_MACMAX = _gdi_.FONTENCODING_MACMAX
FONTENCODING_MAX = _gdi_.FONTENCODING_MAX
FONTENCODING_UTF16 = _gdi_.FONTENCODING_UTF16
FONTENCODING_UTF32 = _gdi_.FONTENCODING_UTF32
FONTENCODING_UNICODE = _gdi_.FONTENCODING_UNICODE
FONTENCODING_GB2312 = _gdi_.FONTENCODING_GB2312
FONTENCODING_BIG5 = _gdi_.FONTENCODING_BIG5
FONTENCODING_SHIFT_JIS = _gdi_.FONTENCODING_SHIFT_JIS
#---------------------------------------------------------------------------

class NativeFontInfo(object):
    """Proxy of C++ NativeFontInfo class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNativeFontInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> NativeFontInfo"""
        newobj = _gdi_.new_NativeFontInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_NativeFontInfo):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Init(*args, **kwargs):
        """Init(self)"""
        return _gdi_.NativeFontInfo_Init(*args, **kwargs)

    def InitFromFont(*args, **kwargs):
        """InitFromFont(self, Font font)"""
        return _gdi_.NativeFontInfo_InitFromFont(*args, **kwargs)

    def GetPointSize(*args, **kwargs):
        """GetPointSize(self) -> int"""
        return _gdi_.NativeFontInfo_GetPointSize(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self) -> int"""
        return _gdi_.NativeFontInfo_GetStyle(*args, **kwargs)

    def GetWeight(*args, **kwargs):
        """GetWeight(self) -> int"""
        return _gdi_.NativeFontInfo_GetWeight(*args, **kwargs)

    def GetUnderlined(*args, **kwargs):
        """GetUnderlined(self) -> bool"""
        return _gdi_.NativeFontInfo_GetUnderlined(*args, **kwargs)

    def GetFaceName(*args, **kwargs):
        """GetFaceName(self) -> String"""
        return _gdi_.NativeFontInfo_GetFaceName(*args, **kwargs)

    def GetFamily(*args, **kwargs):
        """GetFamily(self) -> int"""
        return _gdi_.NativeFontInfo_GetFamily(*args, **kwargs)

    def GetEncoding(*args, **kwargs):
        """GetEncoding(self) -> int"""
        return _gdi_.NativeFontInfo_GetEncoding(*args, **kwargs)

    def SetPointSize(*args, **kwargs):
        """SetPointSize(self, int pointsize)"""
        return _gdi_.NativeFontInfo_SetPointSize(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(self, int style)"""
        return _gdi_.NativeFontInfo_SetStyle(*args, **kwargs)

    def SetWeight(*args, **kwargs):
        """SetWeight(self, int weight)"""
        return _gdi_.NativeFontInfo_SetWeight(*args, **kwargs)

    def SetUnderlined(*args, **kwargs):
        """SetUnderlined(self, bool underlined)"""
        return _gdi_.NativeFontInfo_SetUnderlined(*args, **kwargs)

    def SetFaceName(*args, **kwargs):
        """SetFaceName(self, String facename)"""
        return _gdi_.NativeFontInfo_SetFaceName(*args, **kwargs)

    def SetFamily(*args, **kwargs):
        """SetFamily(self, int family)"""
        return _gdi_.NativeFontInfo_SetFamily(*args, **kwargs)

    def SetEncoding(*args, **kwargs):
        """SetEncoding(self, int encoding)"""
        return _gdi_.NativeFontInfo_SetEncoding(*args, **kwargs)

    def FromString(*args, **kwargs):
        """FromString(self, String s) -> bool"""
        return _gdi_.NativeFontInfo_FromString(*args, **kwargs)

    def ToString(*args, **kwargs):
        """ToString(self) -> String"""
        return _gdi_.NativeFontInfo_ToString(*args, **kwargs)

    def __str__(*args, **kwargs):
        """__str__(self) -> String"""
        return _gdi_.NativeFontInfo___str__(*args, **kwargs)

    def FromUserString(*args, **kwargs):
        """FromUserString(self, String s) -> bool"""
        return _gdi_.NativeFontInfo_FromUserString(*args, **kwargs)

    def ToUserString(*args, **kwargs):
        """ToUserString(self) -> String"""
        return _gdi_.NativeFontInfo_ToUserString(*args, **kwargs)


class NativeFontInfoPtr(NativeFontInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NativeFontInfo
_gdi_.NativeFontInfo_swigregister(NativeFontInfoPtr)

class NativeEncodingInfo(object):
    """Proxy of C++ NativeEncodingInfo class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxNativeEncodingInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    facename = property(_gdi_.NativeEncodingInfo_facename_get, _gdi_.NativeEncodingInfo_facename_set)
    encoding = property(_gdi_.NativeEncodingInfo_encoding_get, _gdi_.NativeEncodingInfo_encoding_set)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> NativeEncodingInfo"""
        newobj = _gdi_.new_NativeEncodingInfo(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_NativeEncodingInfo):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def FromString(*args, **kwargs):
        """FromString(self, String s) -> bool"""
        return _gdi_.NativeEncodingInfo_FromString(*args, **kwargs)

    def ToString(*args, **kwargs):
        """ToString(self) -> String"""
        return _gdi_.NativeEncodingInfo_ToString(*args, **kwargs)


class NativeEncodingInfoPtr(NativeEncodingInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = NativeEncodingInfo
_gdi_.NativeEncodingInfo_swigregister(NativeEncodingInfoPtr)


def GetNativeFontEncoding(*args, **kwargs):
    """GetNativeFontEncoding(int encoding) -> NativeEncodingInfo"""
    return _gdi_.GetNativeFontEncoding(*args, **kwargs)

def TestFontEncoding(*args, **kwargs):
    """TestFontEncoding(NativeEncodingInfo info) -> bool"""
    return _gdi_.TestFontEncoding(*args, **kwargs)
#---------------------------------------------------------------------------

class FontMapper(object):
    """Proxy of C++ FontMapper class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFontMapper instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> FontMapper"""
        newobj = _gdi_.new_FontMapper(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_FontMapper):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Get(*args, **kwargs):
        """Get() -> FontMapper"""
        return _gdi_.FontMapper_Get(*args, **kwargs)

    Get = staticmethod(Get)
    def Set(*args, **kwargs):
        """Set(FontMapper mapper) -> FontMapper"""
        return _gdi_.FontMapper_Set(*args, **kwargs)

    Set = staticmethod(Set)
    def CharsetToEncoding(*args, **kwargs):
        """CharsetToEncoding(self, String charset, bool interactive=True) -> int"""
        return _gdi_.FontMapper_CharsetToEncoding(*args, **kwargs)

    def GetSupportedEncodingsCount(*args, **kwargs):
        """GetSupportedEncodingsCount() -> size_t"""
        return _gdi_.FontMapper_GetSupportedEncodingsCount(*args, **kwargs)

    GetSupportedEncodingsCount = staticmethod(GetSupportedEncodingsCount)
    def GetEncoding(*args, **kwargs):
        """GetEncoding(size_t n) -> int"""
        return _gdi_.FontMapper_GetEncoding(*args, **kwargs)

    GetEncoding = staticmethod(GetEncoding)
    def GetEncodingName(*args, **kwargs):
        """GetEncodingName(int encoding) -> String"""
        return _gdi_.FontMapper_GetEncodingName(*args, **kwargs)

    GetEncodingName = staticmethod(GetEncodingName)
    def GetEncodingDescription(*args, **kwargs):
        """GetEncodingDescription(int encoding) -> String"""
        return _gdi_.FontMapper_GetEncodingDescription(*args, **kwargs)

    GetEncodingDescription = staticmethod(GetEncodingDescription)
    def GetEncodingFromName(*args, **kwargs):
        """GetEncodingFromName(String name) -> int"""
        return _gdi_.FontMapper_GetEncodingFromName(*args, **kwargs)

    GetEncodingFromName = staticmethod(GetEncodingFromName)
    def SetConfig(*args, **kwargs):
        """SetConfig(self, ConfigBase config)"""
        return _gdi_.FontMapper_SetConfig(*args, **kwargs)

    def SetConfigPath(*args, **kwargs):
        """SetConfigPath(self, String prefix)"""
        return _gdi_.FontMapper_SetConfigPath(*args, **kwargs)

    def GetDefaultConfigPath(*args, **kwargs):
        """GetDefaultConfigPath() -> String"""
        return _gdi_.FontMapper_GetDefaultConfigPath(*args, **kwargs)

    GetDefaultConfigPath = staticmethod(GetDefaultConfigPath)
    def GetAltForEncoding(*args, **kwargs):
        """GetAltForEncoding(self, int encoding, String facename=EmptyString, bool interactive=True) -> PyObject"""
        return _gdi_.FontMapper_GetAltForEncoding(*args, **kwargs)

    def IsEncodingAvailable(*args, **kwargs):
        """IsEncodingAvailable(self, int encoding, String facename=EmptyString) -> bool"""
        return _gdi_.FontMapper_IsEncodingAvailable(*args, **kwargs)

    def SetDialogParent(*args, **kwargs):
        """SetDialogParent(self, Window parent)"""
        return _gdi_.FontMapper_SetDialogParent(*args, **kwargs)

    def SetDialogTitle(*args, **kwargs):
        """SetDialogTitle(self, String title)"""
        return _gdi_.FontMapper_SetDialogTitle(*args, **kwargs)


class FontMapperPtr(FontMapper):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FontMapper
_gdi_.FontMapper_swigregister(FontMapperPtr)

def FontMapper_Get(*args, **kwargs):
    """FontMapper_Get() -> FontMapper"""
    return _gdi_.FontMapper_Get(*args, **kwargs)

def FontMapper_Set(*args, **kwargs):
    """FontMapper_Set(FontMapper mapper) -> FontMapper"""
    return _gdi_.FontMapper_Set(*args, **kwargs)

def FontMapper_GetSupportedEncodingsCount(*args, **kwargs):
    """FontMapper_GetSupportedEncodingsCount() -> size_t"""
    return _gdi_.FontMapper_GetSupportedEncodingsCount(*args, **kwargs)

def FontMapper_GetEncoding(*args, **kwargs):
    """FontMapper_GetEncoding(size_t n) -> int"""
    return _gdi_.FontMapper_GetEncoding(*args, **kwargs)

def FontMapper_GetEncodingName(*args, **kwargs):
    """FontMapper_GetEncodingName(int encoding) -> String"""
    return _gdi_.FontMapper_GetEncodingName(*args, **kwargs)

def FontMapper_GetEncodingDescription(*args, **kwargs):
    """FontMapper_GetEncodingDescription(int encoding) -> String"""
    return _gdi_.FontMapper_GetEncodingDescription(*args, **kwargs)

def FontMapper_GetEncodingFromName(*args, **kwargs):
    """FontMapper_GetEncodingFromName(String name) -> int"""
    return _gdi_.FontMapper_GetEncodingFromName(*args, **kwargs)

def FontMapper_GetDefaultConfigPath(*args, **kwargs):
    """FontMapper_GetDefaultConfigPath() -> String"""
    return _gdi_.FontMapper_GetDefaultConfigPath(*args, **kwargs)

#---------------------------------------------------------------------------

class Font(GDIObject):
    """Proxy of C++ Font class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFont instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, int pointSize, int family, int style, int weight, bool underline=False, 
            String face=EmptyString, 
            int encoding=FONTENCODING_DEFAULT) -> Font
        """
        if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
        newobj = _gdi_.new_Font(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_Font):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _gdi_.Font_Ok(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
    def __eq__(*args, **kwargs):
        """__eq__(self, Font other) -> bool"""
        return _gdi_.Font___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, Font other) -> bool"""
        return _gdi_.Font___ne__(*args, **kwargs)

    def GetPointSize(*args, **kwargs):
        """GetPointSize(self) -> int"""
        return _gdi_.Font_GetPointSize(*args, **kwargs)

    def GetPixelSize(*args, **kwargs):
        """GetPixelSize(self) -> Size"""
        return _gdi_.Font_GetPixelSize(*args, **kwargs)

    def IsUsingSizeInPixels(*args, **kwargs):
        """IsUsingSizeInPixels(self) -> bool"""
        return _gdi_.Font_IsUsingSizeInPixels(*args, **kwargs)

    def GetFamily(*args, **kwargs):
        """GetFamily(self) -> int"""
        return _gdi_.Font_GetFamily(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """GetStyle(self) -> int"""
        return _gdi_.Font_GetStyle(*args, **kwargs)

    def GetWeight(*args, **kwargs):
        """GetWeight(self) -> int"""
        return _gdi_.Font_GetWeight(*args, **kwargs)

    def GetUnderlined(*args, **kwargs):
        """GetUnderlined(self) -> bool"""
        return _gdi_.Font_GetUnderlined(*args, **kwargs)

    def GetFaceName(*args, **kwargs):
        """GetFaceName(self) -> String"""
        return _gdi_.Font_GetFaceName(*args, **kwargs)

    def GetEncoding(*args, **kwargs):
        """GetEncoding(self) -> int"""
        return _gdi_.Font_GetEncoding(*args, **kwargs)

    def GetNativeFontInfo(*args, **kwargs):
        """GetNativeFontInfo(self) -> NativeFontInfo"""
        return _gdi_.Font_GetNativeFontInfo(*args, **kwargs)

    def IsFixedWidth(*args, **kwargs):
        """IsFixedWidth(self) -> bool"""
        return _gdi_.Font_IsFixedWidth(*args, **kwargs)

    def GetNativeFontInfoDesc(*args, **kwargs):
        """GetNativeFontInfoDesc(self) -> String"""
        return _gdi_.Font_GetNativeFontInfoDesc(*args, **kwargs)

    def GetNativeFontInfoUserDesc(*args, **kwargs):
        """GetNativeFontInfoUserDesc(self) -> String"""
        return _gdi_.Font_GetNativeFontInfoUserDesc(*args, **kwargs)

    def SetPointSize(*args, **kwargs):
        """SetPointSize(self, int pointSize)"""
        return _gdi_.Font_SetPointSize(*args, **kwargs)

    def SetPixelSize(*args, **kwargs):
        """SetPixelSize(self, Size pixelSize)"""
        return _gdi_.Font_SetPixelSize(*args, **kwargs)

    def SetFamily(*args, **kwargs):
        """SetFamily(self, int family)"""
        return _gdi_.Font_SetFamily(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """SetStyle(self, int style)"""
        return _gdi_.Font_SetStyle(*args, **kwargs)

    def SetWeight(*args, **kwargs):
        """SetWeight(self, int weight)"""
        return _gdi_.Font_SetWeight(*args, **kwargs)

    def SetFaceName(*args, **kwargs):
        """SetFaceName(self, String faceName)"""
        return _gdi_.Font_SetFaceName(*args, **kwargs)

    def SetUnderlined(*args, **kwargs):
        """SetUnderlined(self, bool underlined)"""
        return _gdi_.Font_SetUnderlined(*args, **kwargs)

    def SetEncoding(*args, **kwargs):
        """SetEncoding(self, int encoding)"""
        return _gdi_.Font_SetEncoding(*args, **kwargs)

    def SetNativeFontInfo(*args, **kwargs):
        """SetNativeFontInfo(self, NativeFontInfo info)"""
        return _gdi_.Font_SetNativeFontInfo(*args, **kwargs)

    def SetNativeFontInfoFromString(*args, **kwargs):
        """SetNativeFontInfoFromString(self, String info)"""
        return _gdi_.Font_SetNativeFontInfoFromString(*args, **kwargs)

    def SetNativeFontInfoUserDesc(*args, **kwargs):
        """SetNativeFontInfoUserDesc(self, String info)"""
        return _gdi_.Font_SetNativeFontInfoUserDesc(*args, **kwargs)

    def GetFamilyString(*args, **kwargs):
        """GetFamilyString(self) -> String"""
        return _gdi_.Font_GetFamilyString(*args, **kwargs)

    def GetStyleString(*args, **kwargs):
        """GetStyleString(self) -> String"""
        return _gdi_.Font_GetStyleString(*args, **kwargs)

    def GetWeightString(*args, **kwargs):
        """GetWeightString(self) -> String"""
        return _gdi_.Font_GetWeightString(*args, **kwargs)

    def SetNoAntiAliasing(*args, **kwargs):
        """SetNoAntiAliasing(self, bool no=True)"""
        return _gdi_.Font_SetNoAntiAliasing(*args, **kwargs)

    def GetNoAntiAliasing(*args, **kwargs):
        """GetNoAntiAliasing(self) -> bool"""
        return _gdi_.Font_GetNoAntiAliasing(*args, **kwargs)

    def GetDefaultEncoding(*args, **kwargs):
        """GetDefaultEncoding() -> int"""
        return _gdi_.Font_GetDefaultEncoding(*args, **kwargs)

    GetDefaultEncoding = staticmethod(GetDefaultEncoding)
    def SetDefaultEncoding(*args, **kwargs):
        """SetDefaultEncoding(int encoding)"""
        return _gdi_.Font_SetDefaultEncoding(*args, **kwargs)

    SetDefaultEncoding = staticmethod(SetDefaultEncoding)

class FontPtr(Font):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Font
_gdi_.Font_swigregister(FontPtr)

def FontFromNativeInfo(*args, **kwargs):
    """FontFromNativeInfo(NativeFontInfo info) -> Font"""
    if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
    val = _gdi_.new_FontFromNativeInfo(*args, **kwargs)
    val.thisown = 1
    return val

def FontFromNativeInfoString(*args, **kwargs):
    """FontFromNativeInfoString(String info) -> Font"""
    if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
    val = _gdi_.new_FontFromNativeInfoString(*args, **kwargs)
    val.thisown = 1
    return val

def Font2(*args, **kwargs):
    """
    Font2(int pointSize, int family, int flags=FONTFLAG_DEFAULT, 
        String face=EmptyString, int encoding=FONTENCODING_DEFAULT) -> Font
    """
    if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
    val = _gdi_.new_Font2(*args, **kwargs)
    val.thisown = 1
    return val

def FontFromPixelSize(*args, **kwargs):
    """
    FontFromPixelSize(Size pixelSize, int family, int style, int weight, 
        bool underlined=False, String face=wxEmptyString, 
        int encoding=FONTENCODING_DEFAULT) -> Font
    """
    if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
    val = _gdi_.new_FontFromPixelSize(*args, **kwargs)
    val.thisown = 1
    return val

def Font_GetDefaultEncoding(*args, **kwargs):
    """Font_GetDefaultEncoding() -> int"""
    return _gdi_.Font_GetDefaultEncoding(*args, **kwargs)

def Font_SetDefaultEncoding(*args, **kwargs):
    """Font_SetDefaultEncoding(int encoding)"""
    return _gdi_.Font_SetDefaultEncoding(*args, **kwargs)

#---------------------------------------------------------------------------

class FontEnumerator(object):
    """Proxy of C++ FontEnumerator class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPyFontEnumerator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> FontEnumerator"""
        newobj = _gdi_.new_FontEnumerator(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self._setCallbackInfo(self, FontEnumerator, 0)

    def __del__(self, destroy=_gdi_.delete_FontEnumerator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class, bool incref)"""
        return _gdi_.FontEnumerator__setCallbackInfo(*args, **kwargs)

    def EnumerateFacenames(*args, **kwargs):
        """EnumerateFacenames(self, int encoding=FONTENCODING_SYSTEM, bool fixedWidthOnly=False) -> bool"""
        return _gdi_.FontEnumerator_EnumerateFacenames(*args, **kwargs)

    def EnumerateEncodings(*args, **kwargs):
        """EnumerateEncodings(self, String facename=EmptyString) -> bool"""
        return _gdi_.FontEnumerator_EnumerateEncodings(*args, **kwargs)

    def GetEncodings(*args, **kwargs):
        """GetEncodings(self) -> PyObject"""
        return _gdi_.FontEnumerator_GetEncodings(*args, **kwargs)

    def GetFacenames(*args, **kwargs):
        """GetFacenames(self) -> PyObject"""
        return _gdi_.FontEnumerator_GetFacenames(*args, **kwargs)


class FontEnumeratorPtr(FontEnumerator):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FontEnumerator
_gdi_.FontEnumerator_swigregister(FontEnumeratorPtr)

#---------------------------------------------------------------------------

LANGUAGE_DEFAULT = _gdi_.LANGUAGE_DEFAULT
LANGUAGE_UNKNOWN = _gdi_.LANGUAGE_UNKNOWN
LANGUAGE_ABKHAZIAN = _gdi_.LANGUAGE_ABKHAZIAN
LANGUAGE_AFAR = _gdi_.LANGUAGE_AFAR
LANGUAGE_AFRIKAANS = _gdi_.LANGUAGE_AFRIKAANS
LANGUAGE_ALBANIAN = _gdi_.LANGUAGE_ALBANIAN
LANGUAGE_AMHARIC = _gdi_.LANGUAGE_AMHARIC
LANGUAGE_ARABIC = _gdi_.LANGUAGE_ARABIC
LANGUAGE_ARABIC_ALGERIA = _gdi_.LANGUAGE_ARABIC_ALGERIA
LANGUAGE_ARABIC_BAHRAIN = _gdi_.LANGUAGE_ARABIC_BAHRAIN
LANGUAGE_ARABIC_EGYPT = _gdi_.LANGUAGE_ARABIC_EGYPT
LANGUAGE_ARABIC_IRAQ = _gdi_.LANGUAGE_ARABIC_IRAQ
LANGUAGE_ARABIC_JORDAN = _gdi_.LANGUAGE_ARABIC_JORDAN
LANGUAGE_ARABIC_KUWAIT = _gdi_.LANGUAGE_ARABIC_KUWAIT
LANGUAGE_ARABIC_LEBANON = _gdi_.LANGUAGE_ARABIC_LEBANON
LANGUAGE_ARABIC_LIBYA = _gdi_.LANGUAGE_ARABIC_LIBYA
LANGUAGE_ARABIC_MOROCCO = _gdi_.LANGUAGE_ARABIC_MOROCCO
LANGUAGE_ARABIC_OMAN = _gdi_.LANGUAGE_ARABIC_OMAN
LANGUAGE_ARABIC_QATAR = _gdi_.LANGUAGE_ARABIC_QATAR
LANGUAGE_ARABIC_SAUDI_ARABIA = _gdi_.LANGUAGE_ARABIC_SAUDI_ARABIA
LANGUAGE_ARABIC_SUDAN = _gdi_.LANGUAGE_ARABIC_SUDAN
LANGUAGE_ARABIC_SYRIA = _gdi_.LANGUAGE_ARABIC_SYRIA
LANGUAGE_ARABIC_TUNISIA = _gdi_.LANGUAGE_ARABIC_TUNISIA
LANGUAGE_ARABIC_UAE = _gdi_.LANGUAGE_ARABIC_UAE
LANGUAGE_ARABIC_YEMEN = _gdi_.LANGUAGE_ARABIC_YEMEN
LANGUAGE_ARMENIAN = _gdi_.LANGUAGE_ARMENIAN
LANGUAGE_ASSAMESE = _gdi_.LANGUAGE_ASSAMESE
LANGUAGE_AYMARA = _gdi_.LANGUAGE_AYMARA
LANGUAGE_AZERI = _gdi_.LANGUAGE_AZERI
LANGUAGE_AZERI_CYRILLIC = _gdi_.LANGUAGE_AZERI_CYRILLIC
LANGUAGE_AZERI_LATIN = _gdi_.LANGUAGE_AZERI_LATIN
LANGUAGE_BASHKIR = _gdi_.LANGUAGE_BASHKIR
LANGUAGE_BASQUE = _gdi_.LANGUAGE_BASQUE
LANGUAGE_BELARUSIAN = _gdi_.LANGUAGE_BELARUSIAN
LANGUAGE_BENGALI = _gdi_.LANGUAGE_BENGALI
LANGUAGE_BHUTANI = _gdi_.LANGUAGE_BHUTANI
LANGUAGE_BIHARI = _gdi_.LANGUAGE_BIHARI
LANGUAGE_BISLAMA = _gdi_.LANGUAGE_BISLAMA
LANGUAGE_BRETON = _gdi_.LANGUAGE_BRETON
LANGUAGE_BULGARIAN = _gdi_.LANGUAGE_BULGARIAN
LANGUAGE_BURMESE = _gdi_.LANGUAGE_BURMESE
LANGUAGE_CAMBODIAN = _gdi_.LANGUAGE_CAMBODIAN
LANGUAGE_CATALAN = _gdi_.LANGUAGE_CATALAN
LANGUAGE_CHINESE = _gdi_.LANGUAGE_CHINESE
LANGUAGE_CHINESE_SIMPLIFIED = _gdi_.LANGUAGE_CHINESE_SIMPLIFIED
LANGUAGE_CHINESE_TRADITIONAL = _gdi_.LANGUAGE_CHINESE_TRADITIONAL
LANGUAGE_CHINESE_HONGKONG = _gdi_.LANGUAGE_CHINESE_HONGKONG
LANGUAGE_CHINESE_MACAU = _gdi_.LANGUAGE_CHINESE_MACAU
LANGUAGE_CHINESE_SINGAPORE = _gdi_.LANGUAGE_CHINESE_SINGAPORE
LANGUAGE_CHINESE_TAIWAN = _gdi_.LANGUAGE_CHINESE_TAIWAN
LANGUAGE_CORSICAN = _gdi_.LANGUAGE_CORSICAN
LANGUAGE_CROATIAN = _gdi_.LANGUAGE_CROATIAN
LANGUAGE_CZECH = _gdi_.LANGUAGE_CZECH
LANGUAGE_DANISH = _gdi_.LANGUAGE_DANISH
LANGUAGE_DUTCH = _gdi_.LANGUAGE_DUTCH
LANGUAGE_DUTCH_BELGIAN = _gdi_.LANGUAGE_DUTCH_BELGIAN
LANGUAGE_ENGLISH = _gdi_.LANGUAGE_ENGLISH
LANGUAGE_ENGLISH_UK = _gdi_.LANGUAGE_ENGLISH_UK
LANGUAGE_ENGLISH_US = _gdi_.LANGUAGE_ENGLISH_US
LANGUAGE_ENGLISH_AUSTRALIA = _gdi_.LANGUAGE_ENGLISH_AUSTRALIA
LANGUAGE_ENGLISH_BELIZE = _gdi_.LANGUAGE_ENGLISH_BELIZE
LANGUAGE_ENGLISH_BOTSWANA = _gdi_.LANGUAGE_ENGLISH_BOTSWANA
LANGUAGE_ENGLISH_CANADA = _gdi_.LANGUAGE_ENGLISH_CANADA
LANGUAGE_ENGLISH_CARIBBEAN = _gdi_.LANGUAGE_ENGLISH_CARIBBEAN
LANGUAGE_ENGLISH_DENMARK = _gdi_.LANGUAGE_ENGLISH_DENMARK
LANGUAGE_ENGLISH_EIRE = _gdi_.LANGUAGE_ENGLISH_EIRE
LANGUAGE_ENGLISH_JAMAICA = _gdi_.LANGUAGE_ENGLISH_JAMAICA
LANGUAGE_ENGLISH_NEW_ZEALAND = _gdi_.LANGUAGE_ENGLISH_NEW_ZEALAND
LANGUAGE_ENGLISH_PHILIPPINES = _gdi_.LANGUAGE_ENGLISH_PHILIPPINES
LANGUAGE_ENGLISH_SOUTH_AFRICA = _gdi_.LANGUAGE_ENGLISH_SOUTH_AFRICA
LANGUAGE_ENGLISH_TRINIDAD = _gdi_.LANGUAGE_ENGLISH_TRINIDAD
LANGUAGE_ENGLISH_ZIMBABWE = _gdi_.LANGUAGE_ENGLISH_ZIMBABWE
LANGUAGE_ESPERANTO = _gdi_.LANGUAGE_ESPERANTO
LANGUAGE_ESTONIAN = _gdi_.LANGUAGE_ESTONIAN
LANGUAGE_FAEROESE = _gdi_.LANGUAGE_FAEROESE
LANGUAGE_FARSI = _gdi_.LANGUAGE_FARSI
LANGUAGE_FIJI = _gdi_.LANGUAGE_FIJI
LANGUAGE_FINNISH = _gdi_.LANGUAGE_FINNISH
LANGUAGE_FRENCH = _gdi_.LANGUAGE_FRENCH
LANGUAGE_FRENCH_BELGIAN = _gdi_.LANGUAGE_FRENCH_BELGIAN
LANGUAGE_FRENCH_CANADIAN = _gdi_.LANGUAGE_FRENCH_CANADIAN
LANGUAGE_FRENCH_LUXEMBOURG = _gdi_.LANGUAGE_FRENCH_LUXEMBOURG
LANGUAGE_FRENCH_MONACO = _gdi_.LANGUAGE_FRENCH_MONACO
LANGUAGE_FRENCH_SWISS = _gdi_.LANGUAGE_FRENCH_SWISS
LANGUAGE_FRISIAN = _gdi_.LANGUAGE_FRISIAN
LANGUAGE_GALICIAN = _gdi_.LANGUAGE_GALICIAN
LANGUAGE_GEORGIAN = _gdi_.LANGUAGE_GEORGIAN
LANGUAGE_GERMAN = _gdi_.LANGUAGE_GERMAN
LANGUAGE_GERMAN_AUSTRIAN = _gdi_.LANGUAGE_GERMAN_AUSTRIAN
LANGUAGE_GERMAN_BELGIUM = _gdi_.LANGUAGE_GERMAN_BELGIUM
LANGUAGE_GERMAN_LIECHTENSTEIN = _gdi_.LANGUAGE_GERMAN_LIECHTENSTEIN
LANGUAGE_GERMAN_LUXEMBOURG = _gdi_.LANGUAGE_GERMAN_LUXEMBOURG
LANGUAGE_GERMAN_SWISS = _gdi_.LANGUAGE_GERMAN_SWISS
LANGUAGE_GREEK = _gdi_.LANGUAGE_GREEK
LANGUAGE_GREENLANDIC = _gdi_.LANGUAGE_GREENLANDIC
LANGUAGE_GUARANI = _gdi_.LANGUAGE_GUARANI
LANGUAGE_GUJARATI = _gdi_.LANGUAGE_GUJARATI
LANGUAGE_HAUSA = _gdi_.LANGUAGE_HAUSA
LANGUAGE_HEBREW = _gdi_.LANGUAGE_HEBREW
LANGUAGE_HINDI = _gdi_.LANGUAGE_HINDI
LANGUAGE_HUNGARIAN = _gdi_.LANGUAGE_HUNGARIAN
LANGUAGE_ICELANDIC = _gdi_.LANGUAGE_ICELANDIC
LANGUAGE_INDONESIAN = _gdi_.LANGUAGE_INDONESIAN
LANGUAGE_INTERLINGUA = _gdi_.LANGUAGE_INTERLINGUA
LANGUAGE_INTERLINGUE = _gdi_.LANGUAGE_INTERLINGUE
LANGUAGE_INUKTITUT = _gdi_.LANGUAGE_INUKTITUT
LANGUAGE_INUPIAK = _gdi_.LANGUAGE_INUPIAK
LANGUAGE_IRISH = _gdi_.LANGUAGE_IRISH
LANGUAGE_ITALIAN = _gdi_.LANGUAGE_ITALIAN
LANGUAGE_ITALIAN_SWISS = _gdi_.LANGUAGE_ITALIAN_SWISS
LANGUAGE_JAPANESE = _gdi_.LANGUAGE_JAPANESE
LANGUAGE_JAVANESE = _gdi_.LANGUAGE_JAVANESE
LANGUAGE_KANNADA = _gdi_.LANGUAGE_KANNADA
LANGUAGE_KASHMIRI = _gdi_.LANGUAGE_KASHMIRI
LANGUAGE_KASHMIRI_INDIA = _gdi_.LANGUAGE_KASHMIRI_INDIA
LANGUAGE_KAZAKH = _gdi_.LANGUAGE_KAZAKH
LANGUAGE_KERNEWEK = _gdi_.LANGUAGE_KERNEWEK
LANGUAGE_KINYARWANDA = _gdi_.LANGUAGE_KINYARWANDA
LANGUAGE_KIRGHIZ = _gdi_.LANGUAGE_KIRGHIZ
LANGUAGE_KIRUNDI = _gdi_.LANGUAGE_KIRUNDI
LANGUAGE_KONKANI = _gdi_.LANGUAGE_KONKANI
LANGUAGE_KOREAN = _gdi_.LANGUAGE_KOREAN
LANGUAGE_KURDISH = _gdi_.LANGUAGE_KURDISH
LANGUAGE_LAOTHIAN = _gdi_.LANGUAGE_LAOTHIAN
LANGUAGE_LATIN = _gdi_.LANGUAGE_LATIN
LANGUAGE_LATVIAN = _gdi_.LANGUAGE_LATVIAN
LANGUAGE_LINGALA = _gdi_.LANGUAGE_LINGALA
LANGUAGE_LITHUANIAN = _gdi_.LANGUAGE_LITHUANIAN
LANGUAGE_MACEDONIAN = _gdi_.LANGUAGE_MACEDONIAN
LANGUAGE_MALAGASY = _gdi_.LANGUAGE_MALAGASY
LANGUAGE_MALAY = _gdi_.LANGUAGE_MALAY
LANGUAGE_MALAYALAM = _gdi_.LANGUAGE_MALAYALAM
LANGUAGE_MALAY_BRUNEI_DARUSSALAM = _gdi_.LANGUAGE_MALAY_BRUNEI_DARUSSALAM
LANGUAGE_MALAY_MALAYSIA = _gdi_.LANGUAGE_MALAY_MALAYSIA
LANGUAGE_MALTESE = _gdi_.LANGUAGE_MALTESE
LANGUAGE_MANIPURI = _gdi_.LANGUAGE_MANIPURI
LANGUAGE_MAORI = _gdi_.LANGUAGE_MAORI
LANGUAGE_MARATHI = _gdi_.LANGUAGE_MARATHI
LANGUAGE_MOLDAVIAN = _gdi_.LANGUAGE_MOLDAVIAN
LANGUAGE_MONGOLIAN = _gdi_.LANGUAGE_MONGOLIAN
LANGUAGE_NAURU = _gdi_.LANGUAGE_NAURU
LANGUAGE_NEPALI = _gdi_.LANGUAGE_NEPALI
LANGUAGE_NEPALI_INDIA = _gdi_.LANGUAGE_NEPALI_INDIA
LANGUAGE_NORWEGIAN_BOKMAL = _gdi_.LANGUAGE_NORWEGIAN_BOKMAL
LANGUAGE_NORWEGIAN_NYNORSK = _gdi_.LANGUAGE_NORWEGIAN_NYNORSK
LANGUAGE_OCCITAN = _gdi_.LANGUAGE_OCCITAN
LANGUAGE_ORIYA = _gdi_.LANGUAGE_ORIYA
LANGUAGE_OROMO = _gdi_.LANGUAGE_OROMO
LANGUAGE_PASHTO = _gdi_.LANGUAGE_PASHTO
LANGUAGE_POLISH = _gdi_.LANGUAGE_POLISH
LANGUAGE_PORTUGUESE = _gdi_.LANGUAGE_PORTUGUESE
LANGUAGE_PORTUGUESE_BRAZILIAN = _gdi_.LANGUAGE_PORTUGUESE_BRAZILIAN
LANGUAGE_PUNJABI = _gdi_.LANGUAGE_PUNJABI
LANGUAGE_QUECHUA = _gdi_.LANGUAGE_QUECHUA
LANGUAGE_RHAETO_ROMANCE = _gdi_.LANGUAGE_RHAETO_ROMANCE
LANGUAGE_ROMANIAN = _gdi_.LANGUAGE_ROMANIAN
LANGUAGE_RUSSIAN = _gdi_.LANGUAGE_RUSSIAN
LANGUAGE_RUSSIAN_UKRAINE = _gdi_.LANGUAGE_RUSSIAN_UKRAINE
LANGUAGE_SAMOAN = _gdi_.LANGUAGE_SAMOAN
LANGUAGE_SANGHO = _gdi_.LANGUAGE_SANGHO
LANGUAGE_SANSKRIT = _gdi_.LANGUAGE_SANSKRIT
LANGUAGE_SCOTS_GAELIC = _gdi_.LANGUAGE_SCOTS_GAELIC
LANGUAGE_SERBIAN = _gdi_.LANGUAGE_SERBIAN
LANGUAGE_SERBIAN_CYRILLIC = _gdi_.LANGUAGE_SERBIAN_CYRILLIC
LANGUAGE_SERBIAN_LATIN = _gdi_.LANGUAGE_SERBIAN_LATIN
LANGUAGE_SERBO_CROATIAN = _gdi_.LANGUAGE_SERBO_CROATIAN
LANGUAGE_SESOTHO = _gdi_.LANGUAGE_SESOTHO
LANGUAGE_SETSWANA = _gdi_.LANGUAGE_SETSWANA
LANGUAGE_SHONA = _gdi_.LANGUAGE_SHONA
LANGUAGE_SINDHI = _gdi_.LANGUAGE_SINDHI
LANGUAGE_SINHALESE = _gdi_.LANGUAGE_SINHALESE
LANGUAGE_SISWATI = _gdi_.LANGUAGE_SISWATI
LANGUAGE_SLOVAK = _gdi_.LANGUAGE_SLOVAK
LANGUAGE_SLOVENIAN = _gdi_.LANGUAGE_SLOVENIAN
LANGUAGE_SOMALI = _gdi_.LANGUAGE_SOMALI
LANGUAGE_SPANISH = _gdi_.LANGUAGE_SPANISH
LANGUAGE_SPANISH_ARGENTINA = _gdi_.LANGUAGE_SPANISH_ARGENTINA
LANGUAGE_SPANISH_BOLIVIA = _gdi_.LANGUAGE_SPANISH_BOLIVIA
LANGUAGE_SPANISH_CHILE = _gdi_.LANGUAGE_SPANISH_CHILE
LANGUAGE_SPANISH_COLOMBIA = _gdi_.LANGUAGE_SPANISH_COLOMBIA
LANGUAGE_SPANISH_COSTA_RICA = _gdi_.LANGUAGE_SPANISH_COSTA_RICA
LANGUAGE_SPANISH_DOMINICAN_REPUBLIC = _gdi_.LANGUAGE_SPANISH_DOMINICAN_REPUBLIC
LANGUAGE_SPANISH_ECUADOR = _gdi_.LANGUAGE_SPANISH_ECUADOR
LANGUAGE_SPANISH_EL_SALVADOR = _gdi_.LANGUAGE_SPANISH_EL_SALVADOR
LANGUAGE_SPANISH_GUATEMALA = _gdi_.LANGUAGE_SPANISH_GUATEMALA
LANGUAGE_SPANISH_HONDURAS = _gdi_.LANGUAGE_SPANISH_HONDURAS
LANGUAGE_SPANISH_MEXICAN = _gdi_.LANGUAGE_SPANISH_MEXICAN
LANGUAGE_SPANISH_MODERN = _gdi_.LANGUAGE_SPANISH_MODERN
LANGUAGE_SPANISH_NICARAGUA = _gdi_.LANGUAGE_SPANISH_NICARAGUA
LANGUAGE_SPANISH_PANAMA = _gdi_.LANGUAGE_SPANISH_PANAMA
LANGUAGE_SPANISH_PARAGUAY = _gdi_.LANGUAGE_SPANISH_PARAGUAY
LANGUAGE_SPANISH_PERU = _gdi_.LANGUAGE_SPANISH_PERU
LANGUAGE_SPANISH_PUERTO_RICO = _gdi_.LANGUAGE_SPANISH_PUERTO_RICO
LANGUAGE_SPANISH_URUGUAY = _gdi_.LANGUAGE_SPANISH_URUGUAY
LANGUAGE_SPANISH_US = _gdi_.LANGUAGE_SPANISH_US
LANGUAGE_SPANISH_VENEZUELA = _gdi_.LANGUAGE_SPANISH_VENEZUELA
LANGUAGE_SUNDANESE = _gdi_.LANGUAGE_SUNDANESE
LANGUAGE_SWAHILI = _gdi_.LANGUAGE_SWAHILI
LANGUAGE_SWEDISH = _gdi_.LANGUAGE_SWEDISH
LANGUAGE_SWEDISH_FINLAND = _gdi_.LANGUAGE_SWEDISH_FINLAND
LANGUAGE_TAGALOG = _gdi_.LANGUAGE_TAGALOG
LANGUAGE_TAJIK = _gdi_.LANGUAGE_TAJIK
LANGUAGE_TAMIL = _gdi_.LANGUAGE_TAMIL
LANGUAGE_TATAR = _gdi_.LANGUAGE_TATAR
LANGUAGE_TELUGU = _gdi_.LANGUAGE_TELUGU
LANGUAGE_THAI = _gdi_.LANGUAGE_THAI
LANGUAGE_TIBETAN = _gdi_.LANGUAGE_TIBETAN
LANGUAGE_TIGRINYA = _gdi_.LANGUAGE_TIGRINYA
LANGUAGE_TONGA = _gdi_.LANGUAGE_TONGA
LANGUAGE_TSONGA = _gdi_.LANGUAGE_TSONGA
LANGUAGE_TURKISH = _gdi_.LANGUAGE_TURKISH
LANGUAGE_TURKMEN = _gdi_.LANGUAGE_TURKMEN
LANGUAGE_TWI = _gdi_.LANGUAGE_TWI
LANGUAGE_UIGHUR = _gdi_.LANGUAGE_UIGHUR
LANGUAGE_UKRAINIAN = _gdi_.LANGUAGE_UKRAINIAN
LANGUAGE_URDU = _gdi_.LANGUAGE_URDU
LANGUAGE_URDU_INDIA = _gdi_.LANGUAGE_URDU_INDIA
LANGUAGE_URDU_PAKISTAN = _gdi_.LANGUAGE_URDU_PAKISTAN
LANGUAGE_UZBEK = _gdi_.LANGUAGE_UZBEK
LANGUAGE_UZBEK_CYRILLIC = _gdi_.LANGUAGE_UZBEK_CYRILLIC
LANGUAGE_UZBEK_LATIN = _gdi_.LANGUAGE_UZBEK_LATIN
LANGUAGE_VIETNAMESE = _gdi_.LANGUAGE_VIETNAMESE
LANGUAGE_VOLAPUK = _gdi_.LANGUAGE_VOLAPUK
LANGUAGE_WELSH = _gdi_.LANGUAGE_WELSH
LANGUAGE_WOLOF = _gdi_.LANGUAGE_WOLOF
LANGUAGE_XHOSA = _gdi_.LANGUAGE_XHOSA
LANGUAGE_YIDDISH = _gdi_.LANGUAGE_YIDDISH
LANGUAGE_YORUBA = _gdi_.LANGUAGE_YORUBA
LANGUAGE_ZHUANG = _gdi_.LANGUAGE_ZHUANG
LANGUAGE_ZULU = _gdi_.LANGUAGE_ZULU
LANGUAGE_USER_DEFINED = _gdi_.LANGUAGE_USER_DEFINED
class LanguageInfo(object):
    """Proxy of C++ LanguageInfo class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxLanguageInfo instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    Language = property(_gdi_.LanguageInfo_Language_get, _gdi_.LanguageInfo_Language_set)
    CanonicalName = property(_gdi_.LanguageInfo_CanonicalName_get, _gdi_.LanguageInfo_CanonicalName_set)
    Description = property(_gdi_.LanguageInfo_Description_get, _gdi_.LanguageInfo_Description_set)

class LanguageInfoPtr(LanguageInfo):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = LanguageInfo
_gdi_.LanguageInfo_swigregister(LanguageInfoPtr)

LOCALE_CAT_NUMBER = _gdi_.LOCALE_CAT_NUMBER
LOCALE_CAT_DATE = _gdi_.LOCALE_CAT_DATE
LOCALE_CAT_MONEY = _gdi_.LOCALE_CAT_MONEY
LOCALE_CAT_MAX = _gdi_.LOCALE_CAT_MAX
LOCALE_THOUSANDS_SEP = _gdi_.LOCALE_THOUSANDS_SEP
LOCALE_DECIMAL_POINT = _gdi_.LOCALE_DECIMAL_POINT
LOCALE_LOAD_DEFAULT = _gdi_.LOCALE_LOAD_DEFAULT
LOCALE_CONV_ENCODING = _gdi_.LOCALE_CONV_ENCODING
class Locale(object):
    """Proxy of C++ Locale class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxLocale instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int language=-1, int flags=wxLOCALE_LOAD_DEFAULT|wxLOCALE_CONV_ENCODING) -> Locale"""
        newobj = _gdi_.new_Locale(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_Locale):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Init1(*args, **kwargs):
        """
        Init1(self, String szName, String szShort=EmptyString, String szLocale=EmptyString, 
            bool bLoadDefault=True, 
            bool bConvertEncoding=False) -> bool
        """
        return _gdi_.Locale_Init1(*args, **kwargs)

    def Init2(*args, **kwargs):
        """Init2(self, int language=LANGUAGE_DEFAULT, int flags=wxLOCALE_LOAD_DEFAULT|wxLOCALE_CONV_ENCODING) -> bool"""
        return _gdi_.Locale_Init2(*args, **kwargs)

    def Init(self, *_args, **_kwargs):
        if type(_args[0]) in [type(''), type(u'')]:
            val = self.Init1(*_args, **_kwargs)
        else:
            val = self.Init2(*_args, **_kwargs)
        return val

    def GetSystemLanguage(*args, **kwargs):
        """GetSystemLanguage() -> int"""
        return _gdi_.Locale_GetSystemLanguage(*args, **kwargs)

    GetSystemLanguage = staticmethod(GetSystemLanguage)
    def GetSystemEncoding(*args, **kwargs):
        """GetSystemEncoding() -> int"""
        return _gdi_.Locale_GetSystemEncoding(*args, **kwargs)

    GetSystemEncoding = staticmethod(GetSystemEncoding)
    def GetSystemEncodingName(*args, **kwargs):
        """GetSystemEncodingName() -> String"""
        return _gdi_.Locale_GetSystemEncodingName(*args, **kwargs)

    GetSystemEncodingName = staticmethod(GetSystemEncodingName)
    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _gdi_.Locale_IsOk(*args, **kwargs)

    def __nonzero__(self): return self.IsOk() 
    def GetLocale(*args, **kwargs):
        """GetLocale(self) -> String"""
        return _gdi_.Locale_GetLocale(*args, **kwargs)

    def GetLanguage(*args, **kwargs):
        """GetLanguage(self) -> int"""
        return _gdi_.Locale_GetLanguage(*args, **kwargs)

    def GetSysName(*args, **kwargs):
        """GetSysName(self) -> String"""
        return _gdi_.Locale_GetSysName(*args, **kwargs)

    def GetCanonicalName(*args, **kwargs):
        """GetCanonicalName(self) -> String"""
        return _gdi_.Locale_GetCanonicalName(*args, **kwargs)

    def AddCatalogLookupPathPrefix(*args, **kwargs):
        """AddCatalogLookupPathPrefix(String prefix)"""
        return _gdi_.Locale_AddCatalogLookupPathPrefix(*args, **kwargs)

    AddCatalogLookupPathPrefix = staticmethod(AddCatalogLookupPathPrefix)
    def AddCatalog(*args, **kwargs):
        """AddCatalog(self, String szDomain) -> bool"""
        return _gdi_.Locale_AddCatalog(*args, **kwargs)

    def IsLoaded(*args, **kwargs):
        """IsLoaded(self, String szDomain) -> bool"""
        return _gdi_.Locale_IsLoaded(*args, **kwargs)

    def GetLanguageInfo(*args, **kwargs):
        """GetLanguageInfo(int lang) -> LanguageInfo"""
        return _gdi_.Locale_GetLanguageInfo(*args, **kwargs)

    GetLanguageInfo = staticmethod(GetLanguageInfo)
    def GetLanguageName(*args, **kwargs):
        """GetLanguageName(int lang) -> String"""
        return _gdi_.Locale_GetLanguageName(*args, **kwargs)

    GetLanguageName = staticmethod(GetLanguageName)
    def FindLanguageInfo(*args, **kwargs):
        """FindLanguageInfo(String locale) -> LanguageInfo"""
        return _gdi_.Locale_FindLanguageInfo(*args, **kwargs)

    FindLanguageInfo = staticmethod(FindLanguageInfo)
    def AddLanguage(*args, **kwargs):
        """AddLanguage(LanguageInfo info)"""
        return _gdi_.Locale_AddLanguage(*args, **kwargs)

    AddLanguage = staticmethod(AddLanguage)
    def GetString(*args, **kwargs):
        """GetString(self, String szOrigString, String szDomain=EmptyString) -> String"""
        return _gdi_.Locale_GetString(*args, **kwargs)

    def GetName(*args, **kwargs):
        """GetName(self) -> String"""
        return _gdi_.Locale_GetName(*args, **kwargs)


class LocalePtr(Locale):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Locale
_gdi_.Locale_swigregister(LocalePtr)

def Locale_GetSystemLanguage(*args, **kwargs):
    """Locale_GetSystemLanguage() -> int"""
    return _gdi_.Locale_GetSystemLanguage(*args, **kwargs)

def Locale_GetSystemEncoding(*args, **kwargs):
    """Locale_GetSystemEncoding() -> int"""
    return _gdi_.Locale_GetSystemEncoding(*args, **kwargs)

def Locale_GetSystemEncodingName(*args, **kwargs):
    """Locale_GetSystemEncodingName() -> String"""
    return _gdi_.Locale_GetSystemEncodingName(*args, **kwargs)

def Locale_AddCatalogLookupPathPrefix(*args, **kwargs):
    """Locale_AddCatalogLookupPathPrefix(String prefix)"""
    return _gdi_.Locale_AddCatalogLookupPathPrefix(*args, **kwargs)

def Locale_GetLanguageInfo(*args, **kwargs):
    """Locale_GetLanguageInfo(int lang) -> LanguageInfo"""
    return _gdi_.Locale_GetLanguageInfo(*args, **kwargs)

def Locale_GetLanguageName(*args, **kwargs):
    """Locale_GetLanguageName(int lang) -> String"""
    return _gdi_.Locale_GetLanguageName(*args, **kwargs)

def Locale_FindLanguageInfo(*args, **kwargs):
    """Locale_FindLanguageInfo(String locale) -> LanguageInfo"""
    return _gdi_.Locale_FindLanguageInfo(*args, **kwargs)

def Locale_AddLanguage(*args, **kwargs):
    """Locale_AddLanguage(LanguageInfo info)"""
    return _gdi_.Locale_AddLanguage(*args, **kwargs)


def GetLocale(*args, **kwargs):
    """GetLocale() -> Locale"""
    return _gdi_.GetLocale(*args, **kwargs)
#---------------------------------------------------------------------------

CONVERT_STRICT = _gdi_.CONVERT_STRICT
CONVERT_SUBSTITUTE = _gdi_.CONVERT_SUBSTITUTE
PLATFORM_CURRENT = _gdi_.PLATFORM_CURRENT
PLATFORM_UNIX = _gdi_.PLATFORM_UNIX
PLATFORM_WINDOWS = _gdi_.PLATFORM_WINDOWS
PLATFORM_OS2 = _gdi_.PLATFORM_OS2
PLATFORM_MAC = _gdi_.PLATFORM_MAC
class EncodingConverter(_core.Object):
    """Proxy of C++ EncodingConverter class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEncodingConverter instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> EncodingConverter"""
        newobj = _gdi_.new_EncodingConverter(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_EncodingConverter):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Init(*args, **kwargs):
        """Init(self, int input_enc, int output_enc, int method=CONVERT_STRICT) -> bool"""
        return _gdi_.EncodingConverter_Init(*args, **kwargs)

    def Convert(*args, **kwargs):
        """Convert(self, String input) -> String"""
        return _gdi_.EncodingConverter_Convert(*args, **kwargs)

    def GetPlatformEquivalents(*args, **kwargs):
        """GetPlatformEquivalents(int enc, int platform=PLATFORM_CURRENT) -> wxFontEncodingArray"""
        return _gdi_.EncodingConverter_GetPlatformEquivalents(*args, **kwargs)

    GetPlatformEquivalents = staticmethod(GetPlatformEquivalents)
    def GetAllEquivalents(*args, **kwargs):
        """GetAllEquivalents(int enc) -> wxFontEncodingArray"""
        return _gdi_.EncodingConverter_GetAllEquivalents(*args, **kwargs)

    GetAllEquivalents = staticmethod(GetAllEquivalents)
    def CanConvert(*args, **kwargs):
        """CanConvert(int encIn, int encOut) -> bool"""
        return _gdi_.EncodingConverter_CanConvert(*args, **kwargs)

    CanConvert = staticmethod(CanConvert)
    def __nonzero__(self): return self.IsOk() 

class EncodingConverterPtr(EncodingConverter):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = EncodingConverter
_gdi_.EncodingConverter_swigregister(EncodingConverterPtr)

def GetTranslation(*args):
    """
    GetTranslation(String str) -> String
    GetTranslation(String str, String strPlural, size_t n) -> String
    """
    return _gdi_.GetTranslation(*args)

def EncodingConverter_GetPlatformEquivalents(*args, **kwargs):
    """EncodingConverter_GetPlatformEquivalents(int enc, int platform=PLATFORM_CURRENT) -> wxFontEncodingArray"""
    return _gdi_.EncodingConverter_GetPlatformEquivalents(*args, **kwargs)

def EncodingConverter_GetAllEquivalents(*args, **kwargs):
    """EncodingConverter_GetAllEquivalents(int enc) -> wxFontEncodingArray"""
    return _gdi_.EncodingConverter_GetAllEquivalents(*args, **kwargs)

def EncodingConverter_CanConvert(*args, **kwargs):
    """EncodingConverter_CanConvert(int encIn, int encOut) -> bool"""
    return _gdi_.EncodingConverter_CanConvert(*args, **kwargs)

#----------------------------------------------------------------------------
# On MSW add the directory where the wxWidgets catalogs were installed
# to the default catalog path.
if wx.Platform == "__WXMSW__":
    import os
    _localedir = os.path.join(os.path.split(__file__)[0], "locale")
    Locale.AddCatalogLookupPathPrefix(_localedir)
    del os

#----------------------------------------------------------------------------

#---------------------------------------------------------------------------

class DC(_core.Object):
    """
    A wx.DC is a device context onto which graphics and text can be
    drawn. It is intended to represent a number of output devices in a
    generic way, so a window can have a device context associated with it,
    and a printer also has a device context. In this way, the same piece
    of code may write to a number of different devices, if the device
    context is used as a parameter.

    Derived types of wxDC have documentation for specific features only,
    so refer to this section for most device context information.

    The wx.DC class is abstract and can not be instantiated, you must use
    one of the derived classes instead.  Which one will depend on the
    situation in which it is used.
    """
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_gdi_.delete_DC):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def BeginDrawing(*args, **kwargs):
        """
        BeginDrawing(self)

        Allows for optimization of drawing code on platforms that need it.  On
        other platforms this is just an empty function and is harmless.  To
        take advantage of this postential optimization simply enclose each
        group of calls to the drawing primitives within calls to
        `BeginDrawing` and `EndDrawing`.
        """
        return _gdi_.DC_BeginDrawing(*args, **kwargs)

    def EndDrawing(*args, **kwargs):
        """
        EndDrawing(self)

        Ends the group of drawing primitives started with `BeginDrawing`, and
        invokes whatever optimization is available for this DC type on the
        current platform.
        """
        return _gdi_.DC_EndDrawing(*args, **kwargs)

    def FloodFill(*args, **kwargs):
        """
        FloodFill(self, int x, int y, Colour col, int style=FLOOD_SURFACE) -> bool

        Flood fills the device context starting from the given point, using
        the current brush colour, and using a style:

            - **wxFLOOD_SURFACE**: the flooding occurs until a colour other than
              the given colour is encountered.

            - **wxFLOOD_BORDER**: the area to be flooded is bounded by the given
              colour.

        Returns False if the operation failed.

        Note: The present implementation for non-Windows platforms may fail to
        find colour borders if the pixels do not match the colour
        exactly. However the function will still return true.
        """
        return _gdi_.DC_FloodFill(*args, **kwargs)

    def FloodFillPoint(*args, **kwargs):
        """
        FloodFillPoint(self, Point pt, Colour col, int style=FLOOD_SURFACE) -> bool

        Flood fills the device context starting from the given point, using
        the current brush colour, and using a style:

            - **wxFLOOD_SURFACE**: the flooding occurs until a colour other than
              the given colour is encountered.

            - **wxFLOOD_BORDER**: the area to be flooded is bounded by the given
              colour.

        Returns False if the operation failed.

        Note: The present implementation for non-Windows platforms may fail to
        find colour borders if the pixels do not match the colour
        exactly. However the function will still return true.
        """
        return _gdi_.DC_FloodFillPoint(*args, **kwargs)

    def GetPixel(*args, **kwargs):
        """
        GetPixel(self, int x, int y) -> Colour

        Gets the colour at the specified location on the DC.
        """
        return _gdi_.DC_GetPixel(*args, **kwargs)

    def GetPixelPoint(*args, **kwargs):
        """GetPixelPoint(self, Point pt) -> Colour"""
        return _gdi_.DC_GetPixelPoint(*args, **kwargs)

    def DrawLine(*args, **kwargs):
        """
        DrawLine(self, int x1, int y1, int x2, int y2)

        Draws a line from the first point to the second. The current pen is
        used for drawing the line. Note that the second point is *not* part of
        the line and is not drawn by this function (this is consistent with
        the behaviour of many other toolkits).
        """
        return _gdi_.DC_DrawLine(*args, **kwargs)

    def DrawLinePoint(*args, **kwargs):
        """
        DrawLinePoint(self, Point pt1, Point pt2)

        Draws a line from the first point to the second. The current pen is
        used for drawing the line. Note that the second point is *not* part of
        the line and is not drawn by this function (this is consistent with
        the behaviour of many other toolkits).
        """
        return _gdi_.DC_DrawLinePoint(*args, **kwargs)

    def CrossHair(*args, **kwargs):
        """
        CrossHair(self, int x, int y)

        Displays a cross hair using the current pen. This is a vertical and
        horizontal line the height and width of the window, centred on the
        given point.
        """
        return _gdi_.DC_CrossHair(*args, **kwargs)

    def CrossHairPoint(*args, **kwargs):
        """
        CrossHairPoint(self, Point pt)

        Displays a cross hair using the current pen. This is a vertical and
        horizontal line the height and width of the window, centred on the
        given point.
        """
        return _gdi_.DC_CrossHairPoint(*args, **kwargs)

    def DrawArc(*args, **kwargs):
        """
        DrawArc(self, int x1, int y1, int x2, int y2, int xc, int yc)

        Draws an arc of a circle, centred on the *center* point (xc, yc), from
        the first point to the second. The current pen is used for the outline
        and the current brush for filling the shape.

        The arc is drawn in an anticlockwise direction from the start point to
        the end point.
        """
        return _gdi_.DC_DrawArc(*args, **kwargs)

    def DrawArcPoint(*args, **kwargs):
        """
        DrawArcPoint(self, Point pt1, Point pt2, Point center)

        Draws an arc of a circle, centred on the *center* point (xc, yc), from
        the first point to the second. The current pen is used for the outline
        and the current brush for filling the shape.

        The arc is drawn in an anticlockwise direction from the start point to
        the end point.
        """
        return _gdi_.DC_DrawArcPoint(*args, **kwargs)

    def DrawCheckMark(*args, **kwargs):
        """
        DrawCheckMark(self, int x, int y, int width, int height)

        Draws a check mark inside the given rectangle.
        """
        return _gdi_.DC_DrawCheckMark(*args, **kwargs)

    def DrawCheckMarkRect(*args, **kwargs):
        """
        DrawCheckMarkRect(self, Rect rect)

        Draws a check mark inside the given rectangle.
        """
        return _gdi_.DC_DrawCheckMarkRect(*args, **kwargs)

    def DrawEllipticArc(*args, **kwargs):
        """
        DrawEllipticArc(self, int x, int y, int w, int h, double start, double end)

        Draws an arc of an ellipse, with the given rectangle defining the
        bounds of the ellipse. The current pen is used for drawing the arc and
        the current brush is used for drawing the pie.

        The *start* and *end* parameters specify the start and end of the arc
        relative to the three-o'clock position from the center of the
        rectangle. Angles are specified in degrees (360 is a complete
        circle). Positive values mean counter-clockwise motion. If start is
        equal to end, a complete ellipse will be drawn.
        """
        return _gdi_.DC_DrawEllipticArc(*args, **kwargs)

    def DrawEllipticArcPointSize(*args, **kwargs):
        """
        DrawEllipticArcPointSize(self, Point pt, Size sz, double start, double end)

        Draws an arc of an ellipse, with the given rectangle defining the
        bounds of the ellipse. The current pen is used for drawing the arc and
        the current brush is used for drawing the pie.

        The *start* and *end* parameters specify the start and end of the arc
        relative to the three-o'clock position from the center of the
        rectangle. Angles are specified in degrees (360 is a complete
        circle). Positive values mean counter-clockwise motion. If start is
        equal to end, a complete ellipse will be drawn.
        """
        return _gdi_.DC_DrawEllipticArcPointSize(*args, **kwargs)

    def DrawPoint(*args, **kwargs):
        """
        DrawPoint(self, int x, int y)

        Draws a point using the current pen.
        """
        return _gdi_.DC_DrawPoint(*args, **kwargs)

    def DrawPointPoint(*args, **kwargs):
        """
        DrawPointPoint(self, Point pt)

        Draws a point using the current pen.
        """
        return _gdi_.DC_DrawPointPoint(*args, **kwargs)

    def DrawRectangle(*args, **kwargs):
        """
        DrawRectangle(self, int x, int y, int width, int height)

        Draws a rectangle with the given top left corner, and with the given
        size. The current pen is used for the outline and the current brush
        for filling the shape.
        """
        return _gdi_.DC_DrawRectangle(*args, **kwargs)

    def DrawRectangleRect(*args, **kwargs):
        """
        DrawRectangleRect(self, Rect rect)

        Draws a rectangle with the given top left corner, and with the given
        size. The current pen is used for the outline and the current brush
        for filling the shape.
        """
        return _gdi_.DC_DrawRectangleRect(*args, **kwargs)

    def DrawRectanglePointSize(*args, **kwargs):
        """
        DrawRectanglePointSize(self, Point pt, Size sz)

        Draws a rectangle with the given top left corner, and with the given
        size. The current pen is used for the outline and the current brush
        for filling the shape.
        """
        return _gdi_.DC_DrawRectanglePointSize(*args, **kwargs)

    def DrawRoundedRectangle(*args, **kwargs):
        """
        DrawRoundedRectangle(self, int x, int y, int width, int height, double radius)

        Draws a rectangle with the given top left corner, and with the given
        size. The corners are quarter-circles using the given radius. The
        current pen is used for the outline and the current brush for filling
        the shape.

        If radius is positive, the value is assumed to be the radius of the
        rounded corner. If radius is negative, the absolute value is assumed
        to be the proportion of the smallest dimension of the rectangle. This
        means that the corner can be a sensible size relative to the size of
        the rectangle, and also avoids the strange effects X produces when the
        corners are too big for the rectangle.
        """
        return _gdi_.DC_DrawRoundedRectangle(*args, **kwargs)

    def DrawRoundedRectangleRect(*args, **kwargs):
        """
        DrawRoundedRectangleRect(self, Rect r, double radius)

        Draws a rectangle with the given top left corner, and with the given
        size. The corners are quarter-circles using the given radius. The
        current pen is used for the outline and the current brush for filling
        the shape.

        If radius is positive, the value is assumed to be the radius of the
        rounded corner. If radius is negative, the absolute value is assumed
        to be the proportion of the smallest dimension of the rectangle. This
        means that the corner can be a sensible size relative to the size of
        the rectangle, and also avoids the strange effects X produces when the
        corners are too big for the rectangle.
        """
        return _gdi_.DC_DrawRoundedRectangleRect(*args, **kwargs)

    def DrawRoundedRectanglePointSize(*args, **kwargs):
        """
        DrawRoundedRectanglePointSize(self, Point pt, Size sz, double radius)

        Draws a rectangle with the given top left corner, and with the given
        size. The corners are quarter-circles using the given radius. The
        current pen is used for the outline and the current brush for filling
        the shape.

        If radius is positive, the value is assumed to be the radius of the
        rounded corner. If radius is negative, the absolute value is assumed
        to be the proportion of the smallest dimension of the rectangle. This
        means that the corner can be a sensible size relative to the size of
        the rectangle, and also avoids the strange effects X produces when the
        corners are too big for the rectangle.
        """
        return _gdi_.DC_DrawRoundedRectanglePointSize(*args, **kwargs)

    def DrawCircle(*args, **kwargs):
        """
        DrawCircle(self, int x, int y, int radius)

        Draws a circle with the given center point and radius.  The current
        pen is used for the outline and the current brush for filling the
        shape.
        """
        return _gdi_.DC_DrawCircle(*args, **kwargs)

    def DrawCirclePoint(*args, **kwargs):
        """
        DrawCirclePoint(self, Point pt, int radius)

        Draws a circle with the given center point and radius.  The current
        pen is used for the outline and the current brush for filling the
        shape.
        """
        return _gdi_.DC_DrawCirclePoint(*args, **kwargs)

    def DrawEllipse(*args, **kwargs):
        """
        DrawEllipse(self, int x, int y, int width, int height)

        Draws an ellipse contained in the specified rectangle. The current pen
        is used for the outline and the current brush for filling the shape.
        """
        return _gdi_.DC_DrawEllipse(*args, **kwargs)

    def DrawEllipseRect(*args, **kwargs):
        """
        DrawEllipseRect(self, Rect rect)

        Draws an ellipse contained in the specified rectangle. The current pen
        is used for the outline and the current brush for filling the shape.
        """
        return _gdi_.DC_DrawEllipseRect(*args, **kwargs)

    def DrawEllipsePointSize(*args, **kwargs):
        """
        DrawEllipsePointSize(self, Point pt, Size sz)

        Draws an ellipse contained in the specified rectangle. The current pen
        is used for the outline and the current brush for filling the shape.
        """
        return _gdi_.DC_DrawEllipsePointSize(*args, **kwargs)

    def DrawIcon(*args, **kwargs):
        """
        DrawIcon(self, Icon icon, int x, int y)

        Draw an icon on the display (does nothing if the device context is
        PostScript). This can be the simplest way of drawing bitmaps on a
        window.
        """
        return _gdi_.DC_DrawIcon(*args, **kwargs)

    def DrawIconPoint(*args, **kwargs):
        """
        DrawIconPoint(self, Icon icon, Point pt)

        Draw an icon on the display (does nothing if the device context is
        PostScript). This can be the simplest way of drawing bitmaps on a
        window.
        """
        return _gdi_.DC_DrawIconPoint(*args, **kwargs)

    def DrawBitmap(*args, **kwargs):
        """
        DrawBitmap(self, Bitmap bmp, int x, int y, bool useMask=False)

        Draw a bitmap on the device context at the specified point. If
        *transparent* is true and the bitmap has a transparency mask, (or
        alpha channel on the platforms that support it) then the bitmap will
        be drawn transparently.
        """
        return _gdi_.DC_DrawBitmap(*args, **kwargs)

    def DrawBitmapPoint(*args, **kwargs):
        """
        DrawBitmapPoint(self, Bitmap bmp, Point pt, bool useMask=False)

        Draw a bitmap on the device context at the specified point. If
        *transparent* is true and the bitmap has a transparency mask, (or
        alpha channel on the platforms that support it) then the bitmap will
        be drawn transparently.
        """
        return _gdi_.DC_DrawBitmapPoint(*args, **kwargs)

    def DrawText(*args, **kwargs):
        """
        DrawText(self, String text, int x, int y)

        Draws a text string at the specified point, using the current text
        font, and the current text foreground and background colours.

        The coordinates refer to the top-left corner of the rectangle bounding
        the string. See `GetTextExtent` for how to get the dimensions of a
        text string, which can be used to position the text more precisely.

        **NOTE**: under wxGTK the current logical function is used by this
        function but it is ignored by wxMSW. Thus, you should avoid using
        logical functions with this function in portable programs.
        """
        return _gdi_.DC_DrawText(*args, **kwargs)

    def DrawTextPoint(*args, **kwargs):
        """
        DrawTextPoint(self, String text, Point pt)

        Draws a text string at the specified point, using the current text
        font, and the current text foreground and background colours.

        The coordinates refer to the top-left corner of the rectangle bounding
        the string. See `GetTextExtent` for how to get the dimensions of a
        text string, which can be used to position the text more precisely.

        **NOTE**: under wxGTK the current logical function is used by this
        function but it is ignored by wxMSW. Thus, you should avoid using
        logical functions with this function in portable programs.
        """
        return _gdi_.DC_DrawTextPoint(*args, **kwargs)

    def DrawRotatedText(*args, **kwargs):
        """
        DrawRotatedText(self, String text, int x, int y, double angle)

        Draws the text rotated by *angle* degrees, if supported by the platform.

        **NOTE**: Under Win9x only TrueType fonts can be drawn by this
        function. In particular, a font different from ``wx.NORMAL_FONT``
        should be used as the it is not normally a TrueType
        font. ``wx.SWISS_FONT`` is an example of a font which is.
        """
        return _gdi_.DC_DrawRotatedText(*args, **kwargs)

    def DrawRotatedTextPoint(*args, **kwargs):
        """
        DrawRotatedTextPoint(self, String text, Point pt, double angle)

        Draws the text rotated by *angle* degrees, if supported by the platform.

        **NOTE**: Under Win9x only TrueType fonts can be drawn by this
        function. In particular, a font different from ``wx.NORMAL_FONT``
        should be used as the it is not normally a TrueType
        font. ``wx.SWISS_FONT`` is an example of a font which is.
        """
        return _gdi_.DC_DrawRotatedTextPoint(*args, **kwargs)

    def Blit(*args, **kwargs):
        """
        Blit(self, int xdest, int ydest, int width, int height, DC source, 
            int xsrc, int ysrc, int rop=COPY, bool useMask=False, 
            int xsrcMask=-1, int ysrcMask=-1) -> bool

        Copy from a source DC to this DC.  Parameters specify the destination
        coordinates, size of area to copy, source DC, source coordinates,
        logical function, whether to use a bitmap mask, and mask source
        position.
        """
        return _gdi_.DC_Blit(*args, **kwargs)

    def BlitPointSize(*args, **kwargs):
        """
        BlitPointSize(self, Point destPt, Size sz, DC source, Point srcPt, int rop=COPY, 
            bool useMask=False, Point srcPtMask=DefaultPosition) -> bool

        Copy from a source DC to this DC.  Parameters specify the destination
        coordinates, size of area to copy, source DC, source coordinates,
        logical function, whether to use a bitmap mask, and mask source
        position.
        """
        return _gdi_.DC_BlitPointSize(*args, **kwargs)

    def SetClippingRegion(*args, **kwargs):
        """
        SetClippingRegion(self, int x, int y, int width, int height)

        Sets the clipping region for this device context to the intersection
        of the given region described by the parameters of this method and the
        previously set clipping region. You should call `DestroyClippingRegion`
        if you want to set the clipping region exactly to the region
        specified.

        The clipping region is an area to which drawing is
        restricted. Possible uses for the clipping region are for clipping
        text or for speeding up window redraws when only a known area of the
        screen is damaged.
        """
        return _gdi_.DC_SetClippingRegion(*args, **kwargs)

    def SetClippingRegionPointSize(*args, **kwargs):
        """
        SetClippingRegionPointSize(self, Point pt, Size sz)

        Sets the clipping region for this device context to the intersection
        of the given region described by the parameters of this method and the
        previously set clipping region. You should call `DestroyClippingRegion`
        if you want to set the clipping region exactly to the region
        specified.

        The clipping region is an area to which drawing is
        restricted. Possible uses for the clipping region are for clipping
        text or for speeding up window redraws when only a known area of the
        screen is damaged.
        """
        return _gdi_.DC_SetClippingRegionPointSize(*args, **kwargs)

    def SetClippingRegionAsRegion(*args, **kwargs):
        """
        SetClippingRegionAsRegion(self, Region region)

        Sets the clipping region for this device context to the intersection
        of the given region described by the parameters of this method and the
        previously set clipping region. You should call `DestroyClippingRegion`
        if you want to set the clipping region exactly to the region
        specified.

        The clipping region is an area to which drawing is
        restricted. Possible uses for the clipping region are for clipping
        text or for speeding up window redraws when only a known area of the
        screen is damaged.
        """
        return _gdi_.DC_SetClippingRegionAsRegion(*args, **kwargs)

    def SetClippingRect(*args, **kwargs):
        """
        SetClippingRect(self, Rect rect)

        Sets the clipping region for this device context to the intersection
        of the given region described by the parameters of this method and the
        previously set clipping region. You should call `DestroyClippingRegion`
        if you want to set the clipping region exactly to the region
        specified.

        The clipping region is an area to which drawing is
        restricted. Possible uses for the clipping region are for clipping
        text or for speeding up window redraws when only a known area of the
        screen is damaged.
        """
        return _gdi_.DC_SetClippingRect(*args, **kwargs)

    def DrawLines(*args, **kwargs):
        """
        DrawLines(self, List points, int xoffset=0, int yoffset=0)

        Draws lines using a sequence of `wx.Point` objects, adding the
        optional offset coordinate. The current pen is used for drawing the
        lines.
        """
        return _gdi_.DC_DrawLines(*args, **kwargs)

    def DrawPolygon(*args, **kwargs):
        """
        DrawPolygon(self, List points, int xoffset=0, int yoffset=0,
            int fillStyle=ODDEVEN_RULE)

        Draws a filled polygon using a sequence of `wx.Point` objects, adding
        the optional offset coordinate.  The last argument specifies the fill
        rule: ``wx.ODDEVEN_RULE`` (the default) or ``wx.WINDING_RULE``.

        The current pen is used for drawing the outline, and the current brush
        for filling the shape. Using a transparent brush suppresses
        filling. Note that wxWidgets automatically closes the first and last
        points.
        """
        return _gdi_.DC_DrawPolygon(*args, **kwargs)

    def DrawLabel(*args, **kwargs):
        """
        DrawLabel(self, String text, Rect rect, int alignment=wxALIGN_LEFT|wxALIGN_TOP, 
            int indexAccel=-1)

        Draw *text* within the specified rectangle, abiding by the alignment
        flags.  Will additionally emphasize the character at *indexAccel* if
        it is not -1.
        """
        return _gdi_.DC_DrawLabel(*args, **kwargs)

    def DrawImageLabel(*args, **kwargs):
        """
        DrawImageLabel(self, String text, Bitmap image, Rect rect, int alignment=wxALIGN_LEFT|wxALIGN_TOP, 
            int indexAccel=-1) -> Rect

        Draw *text* and an image (which may be ``wx.NullBitmap`` to skip
        drawing it) within the specified rectangle, abiding by the alignment
        flags.  Will additionally emphasize the character at *indexAccel* if
        it is not -1.  Returns the bounding rectangle.
        """
        return _gdi_.DC_DrawImageLabel(*args, **kwargs)

    def DrawSpline(*args, **kwargs):
        """
        DrawSpline(self, List points)

        Draws a spline between all given control points, (a list of `wx.Point`
        objects) using the current pen. The spline is drawn using a series of
        lines, using an algorithm taken from the X drawing program 'XFIG'.
        """
        return _gdi_.DC_DrawSpline(*args, **kwargs)

    def Clear(*args, **kwargs):
        """
        Clear(self)

        Clears the device context using the current background brush.
        """
        return _gdi_.DC_Clear(*args, **kwargs)

    def StartDoc(*args, **kwargs):
        """
        StartDoc(self, String message) -> bool

        Starts a document (only relevant when outputting to a
        printer). *Message* is a message to show whilst printing.
        """
        return _gdi_.DC_StartDoc(*args, **kwargs)

    def EndDoc(*args, **kwargs):
        """
        EndDoc(self)

        Ends a document (only relevant when outputting to a printer).
        """
        return _gdi_.DC_EndDoc(*args, **kwargs)

    def StartPage(*args, **kwargs):
        """
        StartPage(self)

        Starts a document page (only relevant when outputting to a printer).
        """
        return _gdi_.DC_StartPage(*args, **kwargs)

    def EndPage(*args, **kwargs):
        """
        EndPage(self)

        Ends a document page (only relevant when outputting to a printer).
        """
        return _gdi_.DC_EndPage(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """
        SetFont(self, Font font)

        Sets the current font for the DC. It must be a valid font, in
        particular you should not pass ``wx.NullFont`` to this method.
        """
        return _gdi_.DC_SetFont(*args, **kwargs)

    def SetPen(*args, **kwargs):
        """
        SetPen(self, Pen pen)

        Sets the current pen for the DC.

        If the argument is ``wx.NullPen``, the current pen is selected out of the
        device context, and the original pen restored.
        """
        return _gdi_.DC_SetPen(*args, **kwargs)

    def SetBrush(*args, **kwargs):
        """
        SetBrush(self, Brush brush)

        Sets the current brush for the DC.

        If the argument is ``wx.NullBrush``, the current brush is selected out
        of the device context, and the original brush restored, allowing the
        current brush to be destroyed safely.
        """
        return _gdi_.DC_SetBrush(*args, **kwargs)

    def SetBackground(*args, **kwargs):
        """
        SetBackground(self, Brush brush)

        Sets the current background brush for the DC.
        """
        return _gdi_.DC_SetBackground(*args, **kwargs)

    def SetBackgroundMode(*args, **kwargs):
        """
        SetBackgroundMode(self, int mode)

        *mode* may be one of ``wx.SOLID`` and ``wx.TRANSPARENT``. This setting
        determines whether text will be drawn with a background colour or
        not.
        """
        return _gdi_.DC_SetBackgroundMode(*args, **kwargs)

    def SetPalette(*args, **kwargs):
        """
        SetPalette(self, Palette palette)

        If this is a window DC or memory DC, assigns the given palette to the
        window or bitmap associated with the DC. If the argument is
        ``wx.NullPalette``, the current palette is selected out of the device
        context, and the original palette restored.
        """
        return _gdi_.DC_SetPalette(*args, **kwargs)

    def DestroyClippingRegion(*args, **kwargs):
        """
        DestroyClippingRegion(self)

        Destroys the current clipping region so that none of the DC is
        clipped.
        """
        return _gdi_.DC_DestroyClippingRegion(*args, **kwargs)

    def GetClippingBox(*args, **kwargs):
        """
        GetClippingBox() -> (x, y, width, height)

        Gets the rectangle surrounding the current clipping region.
        """
        return _gdi_.DC_GetClippingBox(*args, **kwargs)

    def GetClippingRect(*args, **kwargs):
        """
        GetClippingRect(self) -> Rect

        Gets the rectangle surrounding the current clipping region.
        """
        return _gdi_.DC_GetClippingRect(*args, **kwargs)

    def GetCharHeight(*args, **kwargs):
        """
        GetCharHeight(self) -> int

        Gets the character height of the currently set font.
        """
        return _gdi_.DC_GetCharHeight(*args, **kwargs)

    def GetCharWidth(*args, **kwargs):
        """
        GetCharWidth(self) -> int

        Gets the average character width of the currently set font.
        """
        return _gdi_.DC_GetCharWidth(*args, **kwargs)

    def GetTextExtent(*args, **kwargs):
        """
        GetTextExtent(wxString string) -> (width, height)

        Get the width and height of the text using the current font. Only
        works for single line strings.
        """
        return _gdi_.DC_GetTextExtent(*args, **kwargs)

    def GetFullTextExtent(*args, **kwargs):
        """
        GetFullTextExtent(wxString string, Font font=None) ->
           (width, height, descent, externalLeading)

        Get the width, height, decent and leading of the text using the
        current or specified font. Only works for single line strings.
        """
        return _gdi_.DC_GetFullTextExtent(*args, **kwargs)

    def GetMultiLineTextExtent(*args, **kwargs):
        """
        GetMultiLineTextExtent(wxString string, Font font=None) ->
           (width, height, descent, externalLeading)

        Get the width, height, decent and leading of the text using the
        current or specified font. Works for single as well as multi-line
        strings.
        """
        return _gdi_.DC_GetMultiLineTextExtent(*args, **kwargs)

    def GetPartialTextExtents(*args, **kwargs):
        """
        GetPartialTextExtents(self, text) -> [widths]

        Returns a list of integers such that each value is the distance in
        pixels from the begining of text to the coresponding character of
        *text*. The generic version simply builds a running total of the widths
        of each character using GetTextExtent, however if the various
        platforms have a native API function that is faster or more accurate
        than the generic implementaiton then it will be used instead.
        """
        return _gdi_.DC_GetPartialTextExtents(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """
        GetSize(self) -> Size

        This gets the horizontal and vertical resolution in device units. It
        can be used to scale graphics to fit the page. For example, if *maxX*
        and *maxY* represent the maximum horizontal and vertical 'pixel' values
        used in your application, the following code will scale the graphic to
        fit on the printer page::

              w, h = dc.GetSize()
              scaleX = maxX*1.0 / w
              scaleY = maxY*1.0 / h
              dc.SetUserScale(min(scaleX,scaleY),min(scaleX,scaleY))

        """
        return _gdi_.DC_GetSize(*args, **kwargs)

    def GetSizeTuple(*args, **kwargs):
        """
        GetSizeTuple() -> (width, height)

        This gets the horizontal and vertical resolution in device units. It
        can be used to scale graphics to fit the page. For example, if *maxX*
        and *maxY* represent the maximum horizontal and vertical 'pixel' values
        used in your application, the following code will scale the graphic to
        fit on the printer page::

              w, h = dc.GetSize()
              scaleX = maxX*1.0 / w
              scaleY = maxY*1.0 / h
              dc.SetUserScale(min(scaleX,scaleY),min(scaleX,scaleY))

        """
        return _gdi_.DC_GetSizeTuple(*args, **kwargs)

    def GetSizeMM(*args, **kwargs):
        """
        GetSizeMM(self) -> Size

        Get the DC size in milimeters.
        """
        return _gdi_.DC_GetSizeMM(*args, **kwargs)

    def GetSizeMMTuple(*args, **kwargs):
        """
        GetSizeMMTuple() -> (width, height)

        Get the DC size in milimeters.
        """
        return _gdi_.DC_GetSizeMMTuple(*args, **kwargs)

    def DeviceToLogicalX(*args, **kwargs):
        """
        DeviceToLogicalX(self, int x) -> int

        Convert device X coordinate to logical coordinate, using the current
        mapping mode.
        """
        return _gdi_.DC_DeviceToLogicalX(*args, **kwargs)

    def DeviceToLogicalY(*args, **kwargs):
        """
        DeviceToLogicalY(self, int y) -> int

        Converts device Y coordinate to logical coordinate, using the current
        mapping mode.
        """
        return _gdi_.DC_DeviceToLogicalY(*args, **kwargs)

    def DeviceToLogicalXRel(*args, **kwargs):
        """
        DeviceToLogicalXRel(self, int x) -> int

        Convert device X coordinate to relative logical coordinate, using the
        current mapping mode but ignoring the x axis orientation. Use this
        function for converting a width, for example.
        """
        return _gdi_.DC_DeviceToLogicalXRel(*args, **kwargs)

    def DeviceToLogicalYRel(*args, **kwargs):
        """
        DeviceToLogicalYRel(self, int y) -> int

        Convert device Y coordinate to relative logical coordinate, using the
        current mapping mode but ignoring the y axis orientation. Use this
        function for converting a height, for example.
        """
        return _gdi_.DC_DeviceToLogicalYRel(*args, **kwargs)

    def LogicalToDeviceX(*args, **kwargs):
        """
        LogicalToDeviceX(self, int x) -> int

        Converts logical X coordinate to device coordinate, using the current
        mapping mode.
        """
        return _gdi_.DC_LogicalToDeviceX(*args, **kwargs)

    def LogicalToDeviceY(*args, **kwargs):
        """
        LogicalToDeviceY(self, int y) -> int

        Converts logical Y coordinate to device coordinate, using the current
        mapping mode.
        """
        return _gdi_.DC_LogicalToDeviceY(*args, **kwargs)

    def LogicalToDeviceXRel(*args, **kwargs):
        """
        LogicalToDeviceXRel(self, int x) -> int

        Converts logical X coordinate to relative device coordinate, using the
        current mapping mode but ignoring the x axis orientation. Use this for
        converting a width, for example.
        """
        return _gdi_.DC_LogicalToDeviceXRel(*args, **kwargs)

    def LogicalToDeviceYRel(*args, **kwargs):
        """
        LogicalToDeviceYRel(self, int y) -> int

        Converts logical Y coordinate to relative device coordinate, using the
        current mapping mode but ignoring the y axis orientation. Use this for
        converting a height, for example.
        """
        return _gdi_.DC_LogicalToDeviceYRel(*args, **kwargs)

    def CanDrawBitmap(*args, **kwargs):
        """CanDrawBitmap(self) -> bool"""
        return _gdi_.DC_CanDrawBitmap(*args, **kwargs)

    def CanGetTextExtent(*args, **kwargs):
        """CanGetTextExtent(self) -> bool"""
        return _gdi_.DC_CanGetTextExtent(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """
        GetDepth(self) -> int

        Returns the colour depth of the DC.
        """
        return _gdi_.DC_GetDepth(*args, **kwargs)

    def GetPPI(*args, **kwargs):
        """
        GetPPI(self) -> Size

        Resolution in Pixels per inch
        """
        return _gdi_.DC_GetPPI(*args, **kwargs)

    def Ok(*args, **kwargs):
        """
        Ok(self) -> bool

        Returns true if the DC is ok to use.
        """
        return _gdi_.DC_Ok(*args, **kwargs)

    def GetBackgroundMode(*args, **kwargs):
        """
        GetBackgroundMode(self) -> int

        Returns the current background mode, either ``wx.SOLID`` or
        ``wx.TRANSPARENT``.
        """
        return _gdi_.DC_GetBackgroundMode(*args, **kwargs)

    def GetBackground(*args, **kwargs):
        """
        GetBackground(self) -> Brush

        Gets the brush used for painting the background.
        """
        return _gdi_.DC_GetBackground(*args, **kwargs)

    def GetBrush(*args, **kwargs):
        """
        GetBrush(self) -> Brush

        Gets the current brush
        """
        return _gdi_.DC_GetBrush(*args, **kwargs)

    def GetFont(*args, **kwargs):
        """
        GetFont(self) -> Font

        Gets the current font
        """
        return _gdi_.DC_GetFont(*args, **kwargs)

    def GetPen(*args, **kwargs):
        """
        GetPen(self) -> Pen

        Gets the current pen
        """
        return _gdi_.DC_GetPen(*args, **kwargs)

    def GetTextBackground(*args, **kwargs):
        """
        GetTextBackground(self) -> Colour

        Gets the current text background colour
        """
        return _gdi_.DC_GetTextBackground(*args, **kwargs)

    def GetTextForeground(*args, **kwargs):
        """
        GetTextForeground(self) -> Colour

        Gets the current text foreground colour
        """
        return _gdi_.DC_GetTextForeground(*args, **kwargs)

    def SetTextForeground(*args, **kwargs):
        """
        SetTextForeground(self, Colour colour)

        Sets the current text foreground colour for the DC.
        """
        return _gdi_.DC_SetTextForeground(*args, **kwargs)

    def SetTextBackground(*args, **kwargs):
        """
        SetTextBackground(self, Colour colour)

        Sets the current text background colour for the DC.
        """
        return _gdi_.DC_SetTextBackground(*args, **kwargs)

    def GetMapMode(*args, **kwargs):
        """
        GetMapMode(self) -> int

        Gets the current *mapping mode* for the device context 
        """
        return _gdi_.DC_GetMapMode(*args, **kwargs)

    def SetMapMode(*args, **kwargs):
        """
        SetMapMode(self, int mode)

        The *mapping mode* of the device context defines the unit of
        measurement used to convert logical units to device units.  The
        mapping mode can be one of the following:

            ================    =============================================
            wx.MM_TWIPS         Each logical unit is 1/20 of a point, or 1/1440
                                of an inch.
            wx.MM_POINTS        Each logical unit is a point, or 1/72 of an inch.
            wx.MM_METRIC        Each logical unit is 1 mm.
            wx.MM_LOMETRIC      Each logical unit is 1/10 of a mm.
            wx.MM_TEXT          Each logical unit is 1 pixel.
            ================    =============================================

        """
        return _gdi_.DC_SetMapMode(*args, **kwargs)

    def GetUserScale(*args, **kwargs):
        """
        GetUserScale(self) -> (xScale, yScale)

        Gets the current user scale factor (set by `SetUserScale`).
        """
        return _gdi_.DC_GetUserScale(*args, **kwargs)

    def SetUserScale(*args, **kwargs):
        """
        SetUserScale(self, double x, double y)

        Sets the user scaling factor, useful for applications which require
        'zooming'.
        """
        return _gdi_.DC_SetUserScale(*args, **kwargs)

    def GetLogicalScale(*args, **kwargs):
        """GetLogicalScale() -> (xScale, yScale)"""
        return _gdi_.DC_GetLogicalScale(*args, **kwargs)

    def SetLogicalScale(*args, **kwargs):
        """SetLogicalScale(self, double x, double y)"""
        return _gdi_.DC_SetLogicalScale(*args, **kwargs)

    def GetLogicalOrigin(*args, **kwargs):
        """GetLogicalOrigin(self) -> Point"""
        return _gdi_.DC_GetLogicalOrigin(*args, **kwargs)

    def GetLogicalOriginTuple(*args, **kwargs):
        """GetLogicalOriginTuple() -> (x,y)"""
        return _gdi_.DC_GetLogicalOriginTuple(*args, **kwargs)

    def SetLogicalOrigin(*args, **kwargs):
        """SetLogicalOrigin(self, int x, int y)"""
        return _gdi_.DC_SetLogicalOrigin(*args, **kwargs)

    def SetLogicalOriginPoint(*args, **kwargs):
        """SetLogicalOriginPoint(self, Point point)"""
        return _gdi_.DC_SetLogicalOriginPoint(*args, **kwargs)

    def GetDeviceOrigin(*args, **kwargs):
        """GetDeviceOrigin(self) -> Point"""
        return _gdi_.DC_GetDeviceOrigin(*args, **kwargs)

    def GetDeviceOriginTuple(*args, **kwargs):
        """GetDeviceOriginTuple() -> (x,y)"""
        return _gdi_.DC_GetDeviceOriginTuple(*args, **kwargs)

    def SetDeviceOrigin(*args, **kwargs):
        """SetDeviceOrigin(self, int x, int y)"""
        return _gdi_.DC_SetDeviceOrigin(*args, **kwargs)

    def SetDeviceOriginPoint(*args, **kwargs):
        """SetDeviceOriginPoint(self, Point point)"""
        return _gdi_.DC_SetDeviceOriginPoint(*args, **kwargs)

    def SetAxisOrientation(*args, **kwargs):
        """
        SetAxisOrientation(self, bool xLeftRight, bool yBottomUp)

        Sets the x and y axis orientation (i.e., the direction from lowest to
        highest values on the axis). The default orientation is the natural
        orientation, e.g. x axis from left to right and y axis from bottom up.
        """
        return _gdi_.DC_SetAxisOrientation(*args, **kwargs)

    def GetLogicalFunction(*args, **kwargs):
        """
        GetLogicalFunction(self) -> int

        Gets the current logical function (set by `SetLogicalFunction`).
        """
        return _gdi_.DC_GetLogicalFunction(*args, **kwargs)

    def SetLogicalFunction(*args, **kwargs):
        """
        SetLogicalFunction(self, int function)

        Sets the current logical function for the device context. This
        determines how a source pixel (from a pen or brush colour, or source
        device context if using `Blit`) combines with a destination pixel in
        the current device context.

        The possible values and their meaning in terms of source and
        destination pixel values are as follows:

            ================       ==========================
            wx.AND                 src AND dst
            wx.AND_INVERT          (NOT src) AND dst
            wx.AND_REVERSE         src AND (NOT dst)
            wx.CLEAR               0
            wx.COPY                src
            wx.EQUIV               (NOT src) XOR dst
            wx.INVERT              NOT dst
            wx.NAND                (NOT src) OR (NOT dst)
            wx.NOR                 (NOT src) AND (NOT dst)
            wx.NO_OP               dst
            wx.OR                  src OR dst
            wx.OR_INVERT           (NOT src) OR dst
            wx.OR_REVERSE          src OR (NOT dst)
            wx.SET                 1
            wx.SRC_INVERT          NOT src
            wx.XOR                 src XOR dst
            ================       ==========================

        The default is wx.COPY, which simply draws with the current
        colour. The others combine the current colour and the background using
        a logical operation. wx.INVERT is commonly used for drawing rubber
        bands or moving outlines, since drawing twice reverts to the original
        colour.

        """
        return _gdi_.DC_SetLogicalFunction(*args, **kwargs)

    def ComputeScaleAndOrigin(*args, **kwargs):
        """
        ComputeScaleAndOrigin(self)

        Performs all necessary computations for given platform and context
        type after each change of scale and origin parameters. Usually called
        automatically internally after such changes.

        """
        return _gdi_.DC_ComputeScaleAndOrigin(*args, **kwargs)

    def SetOptimization(self, optimize):
        pass
    def GetOptimization(self):
        return False

    SetOptimization = wx._deprecated(SetOptimization)
    GetOptimization = wx._deprecated(GetOptimization)

    def CalcBoundingBox(*args, **kwargs):
        """
        CalcBoundingBox(self, int x, int y)

        Adds the specified point to the bounding box which can be retrieved
        with `MinX`, `MaxX` and `MinY`, `MaxY` or `GetBoundingBox` functions.
        """
        return _gdi_.DC_CalcBoundingBox(*args, **kwargs)

    def CalcBoundingBoxPoint(*args, **kwargs):
        """
        CalcBoundingBoxPoint(self, Point point)

        Adds the specified point to the bounding box which can be retrieved
        with `MinX`, `MaxX` and `MinY`, `MaxY` or `GetBoundingBox` functions.
        """
        return _gdi_.DC_CalcBoundingBoxPoint(*args, **kwargs)

    def ResetBoundingBox(*args, **kwargs):
        """
        ResetBoundingBox(self)

        Resets the bounding box: after a call to this function, the bounding
        box doesn't contain anything.
        """
        return _gdi_.DC_ResetBoundingBox(*args, **kwargs)

    def MinX(*args, **kwargs):
        """
        MinX(self) -> int

        Gets the minimum horizontal extent used in drawing commands so far.
        """
        return _gdi_.DC_MinX(*args, **kwargs)

    def MaxX(*args, **kwargs):
        """
        MaxX(self) -> int

        Gets the maximum horizontal extent used in drawing commands so far.
        """
        return _gdi_.DC_MaxX(*args, **kwargs)

    def MinY(*args, **kwargs):
        """
        MinY(self) -> int

        Gets the minimum vertical extent used in drawing commands so far.
        """
        return _gdi_.DC_MinY(*args, **kwargs)

    def MaxY(*args, **kwargs):
        """
        MaxY(self) -> int

        Gets the maximum vertical extent used in drawing commands so far.
        """
        return _gdi_.DC_MaxY(*args, **kwargs)

    def GetBoundingBox(*args, **kwargs):
        """
        GetBoundingBox() -> (x1,y1, x2,y2)

        Returns the min and max points used in drawing commands so far.
        """
        return _gdi_.DC_GetBoundingBox(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
    def _DrawPointList(*args, **kwargs):
        """_DrawPointList(self, PyObject pyCoords, PyObject pyPens, PyObject pyBrushes) -> PyObject"""
        return _gdi_.DC__DrawPointList(*args, **kwargs)

    def _DrawLineList(*args, **kwargs):
        """_DrawLineList(self, PyObject pyCoords, PyObject pyPens, PyObject pyBrushes) -> PyObject"""
        return _gdi_.DC__DrawLineList(*args, **kwargs)

    def _DrawRectangleList(*args, **kwargs):
        """_DrawRectangleList(self, PyObject pyCoords, PyObject pyPens, PyObject pyBrushes) -> PyObject"""
        return _gdi_.DC__DrawRectangleList(*args, **kwargs)

    def _DrawEllipseList(*args, **kwargs):
        """_DrawEllipseList(self, PyObject pyCoords, PyObject pyPens, PyObject pyBrushes) -> PyObject"""
        return _gdi_.DC__DrawEllipseList(*args, **kwargs)

    def _DrawPolygonList(*args, **kwargs):
        """_DrawPolygonList(self, PyObject pyCoords, PyObject pyPens, PyObject pyBrushes) -> PyObject"""
        return _gdi_.DC__DrawPolygonList(*args, **kwargs)

    def _DrawTextList(*args, **kwargs):
        """
        _DrawTextList(self, PyObject textList, PyObject pyPoints, PyObject foregroundList, 
            PyObject backgroundList) -> PyObject
        """
        return _gdi_.DC__DrawTextList(*args, **kwargs)

    def DrawPointList(self, points, pens=None):
        """
        Draw a list of points as quickly as possible.

            :param points:  A sequence of 2-element sequences representing
                            each point to draw, (x,y).
            :param pens:    If None, then the current pen is used.  If a
                            single pen then it will be used for all points.  If
                            a list of pens then there should be one for each point
                            in points.
        """
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(points):
           raise ValueError('points and pens must have same length')
        return self._DrawPointList(points, pens, [])


    def DrawLineList(self, lines, pens=None):
        """
        Draw a list of lines as quickly as possible.

            :param lines:  A sequence of 4-element sequences representing
                            each line to draw, (x1,y1, x2,y2).
            :param pens:    If None, then the current pen is used.  If a
                            single pen then it will be used for all lines.  If
                            a list of pens then there should be one for each line
                            in lines.
        """
        if pens is None:
           pens = []
        elif isinstance(pens, wx.Pen):
           pens = [pens]
        elif len(pens) != len(lines):
           raise ValueError('lines and pens must have same length')
        return self._DrawLineList(lines, pens, [])


    def DrawRectangleList(self, rectangles, pens=None, brushes=None):
        """
        Draw a list of rectangles as quickly as possible.

            :param rectangles:  A sequence of 4-element sequences representing
                            each rectangle to draw, (x,y, w,h).
            :param pens:    If None, then the current pen is used.  If a
                            single pen then it will be used for all rectangles.
                            If a list of pens then there should be one for each 
                            rectangle in rectangles.
            :param brushes: A brush or brushes to be used to fill the rectagles,
                            with similar semantics as the pens parameter.
        """
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
        """
        Draw a list of ellipses as quickly as possible.

            :param ellipses: A sequence of 4-element sequences representing
                            each ellipse to draw, (x,y, w,h).
            :param pens:    If None, then the current pen is used.  If a
                            single pen then it will be used for all ellipses.
                            If a list of pens then there should be one for each 
                            ellipse in ellipses.
            :param brushes: A brush or brushes to be used to fill the ellipses,
                            with similar semantics as the pens parameter.
        """
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
        """
        Draw a list of polygons, each of which is a list of points.

            :param polygons: A sequence of sequences of sequences.
                             [[(x1,y1),(x2,y2),(x3,y3)...],
                             [(x1,y1),(x2,y2),(x3,y3)...]]
                              
            :param pens:    If None, then the current pen is used.  If a
                            single pen then it will be used for all polygons.
                            If a list of pens then there should be one for each 
                            polygon.
            :param brushes: A brush or brushes to be used to fill the polygons,
                            with similar semantics as the pens parameter.
        """
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


    def DrawTextList(self, textList, coords, foregrounds = None, backgrounds = None):
        """
        Draw a list of strings using a list of coordinants for positioning each string.

            :param textList:    A list of strings
            :param coords:      A list of (x,y) positions
            :param foregrounds: A list of `wx.Colour` objects to use for the
                                foregrounds of the strings.
            :param backgrounds: A list of `wx.Colour` objects to use for the
                                backgrounds of the strings.

        NOTE: Make sure you set Background mode to wx.Solid (DC.SetBackgroundMode)
              If you want backgrounds to do anything.
        """
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
_gdi_.DC_swigregister(DCPtr)

#---------------------------------------------------------------------------

class MemoryDC(DC):
    """
    A memory device context provides a means to draw graphics onto a
    bitmap. A bitmap must be selected into the new memory DC before it may
    be used for anything. Typical usage is as follows::

        dc = wx.MemoryDC()
        dc.SelectObject(bitmap)
        # draw on the dc usign any of the Draw methods
        dc.SelectObject(wx.NullBitmap)
        # the bitmap now contains wahtever was drawn upon it

    Note that the memory DC *must* be deleted (or the bitmap selected out
    of it) before a bitmap can be reselected into another memory DC.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMemoryDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> MemoryDC

        Constructs a new memory device context.

        Use the Ok member to test whether the constructor was successful in
        creating a usable device context. Don't forget to select a bitmap into
        the DC before drawing on it.
        """
        newobj = _gdi_.new_MemoryDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def SelectObject(*args, **kwargs):
        """
        SelectObject(self, Bitmap bitmap)

        Selects the bitmap into the device context, to use as the memory
        bitmap. Selecting the bitmap into a memory DC allows you to draw into
        the DC, and therefore the bitmap, and also to use Blit to copy the
        bitmap to a window.

        If the argument is wx.NullBitmap (or some other uninitialised
        `wx.Bitmap`) the current bitmap is selected out of the device context,
        and the original bitmap restored, allowing the current bitmap to be
        destroyed safely.
        """
        return _gdi_.MemoryDC_SelectObject(*args, **kwargs)


class MemoryDCPtr(MemoryDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MemoryDC
_gdi_.MemoryDC_swigregister(MemoryDCPtr)

def MemoryDCFromDC(*args, **kwargs):
    """
    MemoryDCFromDC(DC oldDC) -> MemoryDC

    Creates a DC that is compatible with the oldDC.
    """
    val = _gdi_.new_MemoryDCFromDC(*args, **kwargs)
    val.thisown = 1
    return val

#---------------------------------------------------------------------------

class BufferedDC(MemoryDC):
    """
    This simple class provides a simple way to avoid flicker: when drawing
    on it, everything is in fact first drawn on an in-memory buffer (a
    `wx.Bitmap`) and then copied to the screen only once, when this object
    is destroyed.

    It can be used in the same way as any other device
    context. wx.BufferedDC itself typically replaces `wx.ClientDC`, if you
    want to use it in your EVT_PAINT handler, you should look at
    `wx.BufferedPaintDC`.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBufferedDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, DC dc, Bitmap buffer) -> BufferedDC
        __init__(self, DC dc, Size area) -> BufferedDC

        Constructs a buffered DC.
        """
        newobj = _gdi_.new_BufferedDC(*args)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
        self.__dc = args[0] # save a ref so the other dc will not be deleted before self

    def __del__(self, destroy=_gdi_.delete_BufferedDC):
        """
        __del__(self)

        Copies everything drawn on the DC so far to the underlying DC
        associated with this object, if any.
        """
        try:
            if self.thisown: destroy(self)
        except: pass

    def UnMask(*args, **kwargs):
        """
        UnMask(self)

        Blits the buffer to the dc, and detaches the dc from the buffer (so it
        can be effectively used once only).  This is usually only called in
        the destructor.
        """
        return _gdi_.BufferedDC_UnMask(*args, **kwargs)


class BufferedDCPtr(BufferedDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BufferedDC
_gdi_.BufferedDC_swigregister(BufferedDCPtr)

class BufferedPaintDC(BufferedDC):
    """
    This is a subclass of `wx.BufferedDC` which can be used inside of an
    EVT_PAINT event handler. Just create an object of this class instead
    of `wx.PaintDC` and that's all you have to do to (mostly) avoid
    flicker. The only thing to watch out for is that if you are using this
    class together with `wx.ScrolledWindow`, you probably do **not** want
    to call `wx.Window.PrepareDC` on it as it already does this internally
    for the real underlying `wx.PaintDC`.

    If your window is already fully buffered in a `wx.Bitmap` then your
    EVT_PAINT handler can be as simple as just creating a
    ``wx.BufferedPaintDC`` as it will `Blit` the buffer to the window
    automatically when it is destroyed.  For example::

        def OnPaint(self, event):
            dc = wx.BufferedPaintDC(self, self.buffer)



    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBufferedPaintDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window window, Bitmap buffer=NullBitmap) -> BufferedPaintDC

        Create a buffered paint DC.  As with `wx.BufferedDC`, you may either
        provide the bitmap to be used for buffering or let this object create
        one internally (in the latter case, the size of the client part of the
        window is automatically used).


        """
        newobj = _gdi_.new_BufferedPaintDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class BufferedPaintDCPtr(BufferedPaintDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BufferedPaintDC
_gdi_.BufferedPaintDC_swigregister(BufferedPaintDCPtr)

#---------------------------------------------------------------------------

class ScreenDC(DC):
    """
    A wxScreenDC can be used to paint anywhere on the screen. This should
    normally be constructed as a temporary stack object; don't store a
    wxScreenDC object.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxScreenDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self) -> ScreenDC

        A wxScreenDC can be used to paint anywhere on the screen. This should
        normally be constructed as a temporary stack object; don't store a
        wxScreenDC object.

        """
        newobj = _gdi_.new_ScreenDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def StartDrawingOnTopWin(*args, **kwargs):
        """
        StartDrawingOnTopWin(self, Window window) -> bool

        Specify that the area of the screen to be drawn upon coincides with
        the given window.

        :see: `EndDrawingOnTop`
        """
        return _gdi_.ScreenDC_StartDrawingOnTopWin(*args, **kwargs)

    def StartDrawingOnTop(*args, **kwargs):
        """
        StartDrawingOnTop(self, Rect rect=None) -> bool

        Specify that the area is the given rectangle, or the whole screen if
        ``None`` is passed.

        :see: `EndDrawingOnTop`
        """
        return _gdi_.ScreenDC_StartDrawingOnTop(*args, **kwargs)

    def EndDrawingOnTop(*args, **kwargs):
        """
        EndDrawingOnTop(self) -> bool

        Use this in conjunction with `StartDrawingOnTop` or
        `StartDrawingOnTopWin` to ensure that drawing to the screen occurs on
        top of existing windows. Without this, some window systems (such as X)
        only allow drawing to take place underneath other windows.

        You might use this pair of functions when implementing a drag feature,
        for example as in the `wx.SplitterWindow` implementation.

        These functions are probably obsolete since the X implementations
        allow drawing directly on the screen now. However, the fact that this
        function allows the screen to be refreshed afterwards may be useful
        to some applications.
        """
        return _gdi_.ScreenDC_EndDrawingOnTop(*args, **kwargs)


class ScreenDCPtr(ScreenDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ScreenDC
_gdi_.ScreenDC_swigregister(ScreenDCPtr)

#---------------------------------------------------------------------------

class ClientDC(DC):
    """
    A wx.ClientDC must be constructed if an application wishes to paint on
    the client area of a window from outside an EVT_PAINT event. This should
    normally be constructed as a temporary stack object; don't store a
    wx.ClientDC object long term.

    To draw on a window from within an EVT_PAINT handler, construct a
    `wx.PaintDC` object.

    To draw on the whole window including decorations, construct a
    `wx.WindowDC` object (Windows only).

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxClientDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window win) -> ClientDC

        Constructor. Pass the window on which you wish to paint.
        """
        newobj = _gdi_.new_ClientDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class ClientDCPtr(ClientDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ClientDC
_gdi_.ClientDC_swigregister(ClientDCPtr)

#---------------------------------------------------------------------------

class PaintDC(DC):
    """
    A wx.PaintDC must be constructed if an application wishes to paint on
    the client area of a window from within an EVT_PAINT event
    handler. This should normally be constructed as a temporary stack
    object; don't store a wx.PaintDC object. If you have an EVT_PAINT
    handler, you **must** create a wx.PaintDC object within it even if you
    don't actually use it.

    Using wx.PaintDC within EVT_PAINT handlers is important because it
    automatically sets the clipping area to the damaged area of the
    window. Attempts to draw outside this area do not appear.

    To draw on a window from outside EVT_PAINT handlers, construct a
    `wx.ClientDC` object.

    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPaintDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window win) -> PaintDC

        Constructor. Pass the window on which you wish to paint.
        """
        newobj = _gdi_.new_PaintDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class PaintDCPtr(PaintDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PaintDC
_gdi_.PaintDC_swigregister(PaintDCPtr)

#---------------------------------------------------------------------------

class WindowDC(DC):
    """
    A wx.WindowDC must be constructed if an application wishes to paint on
    the whole area of a window (client and decorations). This should
    normally be constructed as a temporary stack object; don't store a
    wx.WindowDC object.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxWindowDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, Window win) -> WindowDC

        Constructor. Pass the window on which you wish to paint.
        """
        newobj = _gdi_.new_WindowDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class WindowDCPtr(WindowDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = WindowDC
_gdi_.WindowDC_swigregister(WindowDCPtr)

#---------------------------------------------------------------------------

class MirrorDC(DC):
    """
    wx.MirrorDC is a simple wrapper class which is always associated with a
    real `wx.DC` object and either forwards all of its operations to it
    without changes (no mirroring takes place) or exchanges x and y
    coordinates which makes it possible to reuse the same code to draw a
    figure and its mirror -- i.e. reflection related to the diagonal line
    x == y.
    """
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMirrorDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, DC dc, bool mirror) -> MirrorDC

        Creates a mirrored DC associated with the real *dc*.  Everything drawn
        on the wx.MirrorDC will appear on the *dc*, and will be mirrored if
        *mirror* is True.
        """
        newobj = _gdi_.new_MirrorDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class MirrorDCPtr(MirrorDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MirrorDC
_gdi_.MirrorDC_swigregister(MirrorDCPtr)

#---------------------------------------------------------------------------

class PostScriptDC(DC):
    """This is a `wx.DC` that can write to PostScript files on any platform."""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPostScriptDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, wxPrintData printData) -> PostScriptDC

        Constructs a PostScript printer device context from a `wx.PrintData`
        object.
        """
        newobj = _gdi_.new_PostScriptDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetPrintData(*args, **kwargs):
        """GetPrintData(self) -> wxPrintData"""
        return _gdi_.PostScriptDC_GetPrintData(*args, **kwargs)

    def SetPrintData(*args, **kwargs):
        """SetPrintData(self, wxPrintData data)"""
        return _gdi_.PostScriptDC_SetPrintData(*args, **kwargs)

    def SetResolution(*args, **kwargs):
        """
        SetResolution(int ppi)

        Set resolution (in pixels per inch) that will be used in PostScript
        output. Default is 720ppi.
        """
        return _gdi_.PostScriptDC_SetResolution(*args, **kwargs)

    SetResolution = staticmethod(SetResolution)
    def GetResolution(*args, **kwargs):
        """
        GetResolution() -> int

        Return resolution used in PostScript output.
        """
        return _gdi_.PostScriptDC_GetResolution(*args, **kwargs)

    GetResolution = staticmethod(GetResolution)

class PostScriptDCPtr(PostScriptDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PostScriptDC
_gdi_.PostScriptDC_swigregister(PostScriptDCPtr)

def PostScriptDC_SetResolution(*args, **kwargs):
    """
    PostScriptDC_SetResolution(int ppi)

    Set resolution (in pixels per inch) that will be used in PostScript
    output. Default is 720ppi.
    """
    return _gdi_.PostScriptDC_SetResolution(*args, **kwargs)

def PostScriptDC_GetResolution(*args, **kwargs):
    """
    PostScriptDC_GetResolution() -> int

    Return resolution used in PostScript output.
    """
    return _gdi_.PostScriptDC_GetResolution(*args, **kwargs)

#---------------------------------------------------------------------------

class MetaFile(_core.Object):
    """Proxy of C++ MetaFile class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMetaFile instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, String filename=EmptyString) -> MetaFile"""
        newobj = _gdi_.new_MetaFile(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_MetaFile):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _gdi_.MetaFile_Ok(*args, **kwargs)

    def SetClipboard(*args, **kwargs):
        """SetClipboard(self, int width=0, int height=0) -> bool"""
        return _gdi_.MetaFile_SetClipboard(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self) -> Size"""
        return _gdi_.MetaFile_GetSize(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _gdi_.MetaFile_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return _gdi_.MetaFile_GetHeight(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 

class MetaFilePtr(MetaFile):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MetaFile
_gdi_.MetaFile_swigregister(MetaFilePtr)

class MetaFileDC(DC):
    """Proxy of C++ MetaFileDC class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxMetaFileDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """
        __init__(self, String filename=EmptyString, int width=0, int height=0, 
            String description=EmptyString) -> MetaFileDC
        """
        newobj = _gdi_.new_MetaFileDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def Close(*args, **kwargs):
        """Close(self) -> MetaFile"""
        return _gdi_.MetaFileDC_Close(*args, **kwargs)


class MetaFileDCPtr(MetaFileDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = MetaFileDC
_gdi_.MetaFileDC_swigregister(MetaFileDCPtr)

class PrinterDC(DC):
    """Proxy of C++ PrinterDC class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPrinterDC instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, wxPrintData printData) -> PrinterDC"""
        newobj = _gdi_.new_PrinterDC(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown

class PrinterDCPtr(PrinterDC):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PrinterDC
_gdi_.PrinterDC_swigregister(PrinterDCPtr)

#---------------------------------------------------------------------------

IMAGELIST_DRAW_NORMAL = _gdi_.IMAGELIST_DRAW_NORMAL
IMAGELIST_DRAW_TRANSPARENT = _gdi_.IMAGELIST_DRAW_TRANSPARENT
IMAGELIST_DRAW_SELECTED = _gdi_.IMAGELIST_DRAW_SELECTED
IMAGELIST_DRAW_FOCUSED = _gdi_.IMAGELIST_DRAW_FOCUSED
IMAGE_LIST_NORMAL = _gdi_.IMAGE_LIST_NORMAL
IMAGE_LIST_SMALL = _gdi_.IMAGE_LIST_SMALL
IMAGE_LIST_STATE = _gdi_.IMAGE_LIST_STATE
class ImageList(_core.Object):
    """Proxy of C++ ImageList class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxImageList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self, int width, int height, int mask=True, int initialCount=1) -> ImageList"""
        newobj = _gdi_.new_ImageList(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_ImageList):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Add(*args, **kwargs):
        """Add(self, Bitmap bitmap, Bitmap mask=NullBitmap) -> int"""
        return _gdi_.ImageList_Add(*args, **kwargs)

    def AddWithColourMask(*args, **kwargs):
        """AddWithColourMask(self, Bitmap bitmap, Colour maskColour) -> int"""
        return _gdi_.ImageList_AddWithColourMask(*args, **kwargs)

    def AddIcon(*args, **kwargs):
        """AddIcon(self, Icon icon) -> int"""
        return _gdi_.ImageList_AddIcon(*args, **kwargs)

    def Replace(*args, **kwargs):
        """Replace(self, int index, Bitmap bitmap) -> bool"""
        return _gdi_.ImageList_Replace(*args, **kwargs)

    def Draw(*args, **kwargs):
        """
        Draw(self, int index, DC dc, int x, int x, int flags=IMAGELIST_DRAW_NORMAL, 
            bool solidBackground=False) -> bool
        """
        return _gdi_.ImageList_Draw(*args, **kwargs)

    def GetImageCount(*args, **kwargs):
        """GetImageCount(self) -> int"""
        return _gdi_.ImageList_GetImageCount(*args, **kwargs)

    def Remove(*args, **kwargs):
        """Remove(self, int index) -> bool"""
        return _gdi_.ImageList_Remove(*args, **kwargs)

    def RemoveAll(*args, **kwargs):
        """RemoveAll(self) -> bool"""
        return _gdi_.ImageList_RemoveAll(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize() -> (width,height)"""
        return _gdi_.ImageList_GetSize(*args, **kwargs)


class ImageListPtr(ImageList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ImageList
_gdi_.ImageList_swigregister(ImageListPtr)

#---------------------------------------------------------------------------

class PenList(_core.Object):
    """Proxy of C++ PenList class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxPenList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def AddPen(*args, **kwargs):
        """AddPen(self, Pen pen)"""
        return _gdi_.PenList_AddPen(*args, **kwargs)

    def FindOrCreatePen(*args, **kwargs):
        """FindOrCreatePen(self, Colour colour, int width, int style) -> Pen"""
        return _gdi_.PenList_FindOrCreatePen(*args, **kwargs)

    def RemovePen(*args, **kwargs):
        """RemovePen(self, Pen pen)"""
        return _gdi_.PenList_RemovePen(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount(self) -> int"""
        return _gdi_.PenList_GetCount(*args, **kwargs)


class PenListPtr(PenList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = PenList
_gdi_.PenList_swigregister(PenListPtr)
cvar = _gdi_.cvar
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

class BrushList(_core.Object):
    """Proxy of C++ BrushList class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxBrushList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def AddBrush(*args, **kwargs):
        """AddBrush(self, Brush brush)"""
        return _gdi_.BrushList_AddBrush(*args, **kwargs)

    def FindOrCreateBrush(*args, **kwargs):
        """FindOrCreateBrush(self, Colour colour, int style) -> Brush"""
        return _gdi_.BrushList_FindOrCreateBrush(*args, **kwargs)

    def RemoveBrush(*args, **kwargs):
        """RemoveBrush(self, Brush brush)"""
        return _gdi_.BrushList_RemoveBrush(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount(self) -> int"""
        return _gdi_.BrushList_GetCount(*args, **kwargs)


class BrushListPtr(BrushList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = BrushList
_gdi_.BrushList_swigregister(BrushListPtr)

class ColourDatabase(_core.Object):
    """Proxy of C++ ColourDatabase class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxColourDatabase instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> ColourDatabase"""
        newobj = _gdi_.new_ColourDatabase(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def __del__(self, destroy=_gdi_.delete_ColourDatabase):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def Find(*args, **kwargs):
        """Find(self, String name) -> Colour"""
        return _gdi_.ColourDatabase_Find(*args, **kwargs)

    def FindName(*args, **kwargs):
        """FindName(self, Colour colour) -> String"""
        return _gdi_.ColourDatabase_FindName(*args, **kwargs)

    FindColour = Find 
    def AddColour(*args, **kwargs):
        """AddColour(self, String name, Colour colour)"""
        return _gdi_.ColourDatabase_AddColour(*args, **kwargs)

    def Append(*args, **kwargs):
        """Append(self, String name, int red, int green, int blue)"""
        return _gdi_.ColourDatabase_Append(*args, **kwargs)


class ColourDatabasePtr(ColourDatabase):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = ColourDatabase
_gdi_.ColourDatabase_swigregister(ColourDatabasePtr)

class FontList(_core.Object):
    """Proxy of C++ FontList class"""
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxFontList instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def AddFont(*args, **kwargs):
        """AddFont(self, Font font)"""
        return _gdi_.FontList_AddFont(*args, **kwargs)

    def FindOrCreateFont(*args, **kwargs):
        """
        FindOrCreateFont(self, int point_size, int family, int style, int weight, 
            bool underline=False, String facename=EmptyString, 
            int encoding=FONTENCODING_DEFAULT) -> Font
        """
        return _gdi_.FontList_FindOrCreateFont(*args, **kwargs)

    def RemoveFont(*args, **kwargs):
        """RemoveFont(self, Font font)"""
        return _gdi_.FontList_RemoveFont(*args, **kwargs)

    def GetCount(*args, **kwargs):
        """GetCount(self) -> int"""
        return _gdi_.FontList_GetCount(*args, **kwargs)


class FontListPtr(FontList):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = FontList
_gdi_.FontList_swigregister(FontListPtr)

#---------------------------------------------------------------------------

NullColor = NullColour 
#---------------------------------------------------------------------------

class Effects(_core.Object):
    """Proxy of C++ Effects class"""
    def __repr__(self):
        return "<%s.%s; proxy of C++ wxEffects instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args, **kwargs):
        """__init__(self) -> Effects"""
        newobj = _gdi_.new_Effects(*args, **kwargs)
        self.this = newobj.this
        self.thisown = 1
        del newobj.thisown
    def GetHighlightColour(*args, **kwargs):
        """GetHighlightColour(self) -> Colour"""
        return _gdi_.Effects_GetHighlightColour(*args, **kwargs)

    def GetLightShadow(*args, **kwargs):
        """GetLightShadow(self) -> Colour"""
        return _gdi_.Effects_GetLightShadow(*args, **kwargs)

    def GetFaceColour(*args, **kwargs):
        """GetFaceColour(self) -> Colour"""
        return _gdi_.Effects_GetFaceColour(*args, **kwargs)

    def GetMediumShadow(*args, **kwargs):
        """GetMediumShadow(self) -> Colour"""
        return _gdi_.Effects_GetMediumShadow(*args, **kwargs)

    def GetDarkShadow(*args, **kwargs):
        """GetDarkShadow(self) -> Colour"""
        return _gdi_.Effects_GetDarkShadow(*args, **kwargs)

    def SetHighlightColour(*args, **kwargs):
        """SetHighlightColour(self, Colour c)"""
        return _gdi_.Effects_SetHighlightColour(*args, **kwargs)

    def SetLightShadow(*args, **kwargs):
        """SetLightShadow(self, Colour c)"""
        return _gdi_.Effects_SetLightShadow(*args, **kwargs)

    def SetFaceColour(*args, **kwargs):
        """SetFaceColour(self, Colour c)"""
        return _gdi_.Effects_SetFaceColour(*args, **kwargs)

    def SetMediumShadow(*args, **kwargs):
        """SetMediumShadow(self, Colour c)"""
        return _gdi_.Effects_SetMediumShadow(*args, **kwargs)

    def SetDarkShadow(*args, **kwargs):
        """SetDarkShadow(self, Colour c)"""
        return _gdi_.Effects_SetDarkShadow(*args, **kwargs)

    def Set(*args, **kwargs):
        """
        Set(self, Colour highlightColour, Colour lightShadow, Colour faceColour, 
            Colour mediumShadow, Colour darkShadow)
        """
        return _gdi_.Effects_Set(*args, **kwargs)

    def DrawSunkenEdge(*args, **kwargs):
        """DrawSunkenEdge(self, DC dc, Rect rect, int borderSize=1)"""
        return _gdi_.Effects_DrawSunkenEdge(*args, **kwargs)

    def TileBitmap(*args, **kwargs):
        """TileBitmap(self, Rect rect, DC dc, Bitmap bitmap) -> bool"""
        return _gdi_.Effects_TileBitmap(*args, **kwargs)


class EffectsPtr(Effects):
    def __init__(self, this):
        self.this = this
        if not hasattr(self,"thisown"): self.thisown = 0
        self.__class__ = Effects
_gdi_.Effects_swigregister(EffectsPtr)
TheFontList = cvar.TheFontList
ThePenList = cvar.ThePenList
TheBrushList = cvar.TheBrushList
TheColourDatabase = cvar.TheColourDatabase


