# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.

import _gdi_
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

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
        if (name == "thisown"): return self.this.own(value)
        if hasattr(self,name) or (name == "this"):
            set(self,name,value)
        else:
            raise AttributeError("You cannot add attributes to %s" % self)
    return set_attr


import _core
wx = _core 
#---------------------------------------------------------------------------

class GDIObject(_core.Object):
    """Proxy of C++ GDIObject class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> GDIObject"""
        _gdi_.GDIObject_swiginit(self,_gdi_.new_GDIObject(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_GDIObject
    __del__ = lambda self : None;
    def IsNull(*args, **kwargs):
        """IsNull(self) -> bool"""
        return _gdi_.GDIObject_IsNull(*args, **kwargs)

_gdi_.GDIObject_swigregister(GDIObject)

#---------------------------------------------------------------------------

C2S_NAME = _gdi_.C2S_NAME
C2S_CSS_SYNTAX = _gdi_.C2S_CSS_SYNTAX
C2S_HTML_SYNTAX = _gdi_.C2S_HTML_SYNTAX
ALPHA_TRANSPARENT = _gdi_.ALPHA_TRANSPARENT
ALPHA_OPAQUE = _gdi_.ALPHA_OPAQUE
class Colour(_core.Object):
    """
    A colour is an object representing a combination of Red, Green, and
    Blue (RGB) intensity values, and is used to determine drawing colours,
    window colours, etc.  Valid RGB values are in the range 0 to 255.

    In wxPython there are typemaps that will automatically convert from a
    colour name, from a '#RRGGBB' colour hex value string, or from a 3
    integer tuple to a wx.Colour object when calling C++ methods that
    expect a wxColour.  This means that the following are all
    equivallent::

        win.SetBackgroundColour(wxColour(0,0,255))
        win.SetBackgroundColour('BLUE')
        win.SetBackgroundColour('#0000FF')
        win.SetBackgroundColour((0,0,255))

    Additional colour names and their coresponding values can be added
    using `wx.ColourDatabase`.  Various system colours (as set in the
    user's system preferences) can be retrieved with
    `wx.SystemSettings.GetColour`.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, byte red=0, byte green=0, byte blue=0, byte alpha=ALPHA_OPAQUE) -> Colour

        Constructs a colour from red, green and blue values.

        :see: Alternate constructors `wx.NamedColour` and `wx.ColourRGB`.

        """
        _gdi_.Colour_swiginit(self,_gdi_.new_Colour(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Colour
    __del__ = lambda self : None;
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

    def Alpha(*args, **kwargs):
        """
        Alpha(self) -> byte

        Returns the Alpha value.
        """
        return _gdi_.Colour_Alpha(*args, **kwargs)

    def Ok(*args, **kwargs):
        """
        Ok(self) -> bool

        Returns True if the colour object is valid (the colour has been
        initialised with RGB values).
        """
        return _gdi_.Colour_Ok(*args, **kwargs)

    def Set(*args, **kwargs):
        """
        Set(self, byte red, byte green, byte blue, byte alpha=ALPHA_OPAQUE)

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

    def GetAsString(*args, **kwargs):
        """
        GetAsString(self, long flags=wxC2S_NAME|wxC2S_CSS_SYNTAX) -> String

        Return the colour as a string.  Acceptable flags are:

                    =================== ==================================
                    wx.C2S_NAME          return colour name, when possible
                    wx.C2S_CSS_SYNTAX    return colour in rgb(r,g,b) syntax
                    wx.C2S_HTML_SYNTAX   return colour in #rrggbb syntax     
                    =================== ==================================
        """
        return _gdi_.Colour_GetAsString(*args, **kwargs)

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
        __eq__(self, PyObject other) -> bool

        Compare colours for equality.
        """
        return _gdi_.Colour___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """
        __ne__(self, PyObject other) -> bool

        Compare colours for inequality.
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
    def __str__(self):                  return str(self.Get(True))
    def __repr__(self):                 return 'wx.Colour' + str(self.Get(True))
    def __nonzero__(self):              return self.Ok()
    __safe_for_unpickling__ = True
    def __reduce__(self):               return (Colour, self.Get(True))

_gdi_.Colour_swigregister(Colour)

def NamedColour(*args, **kwargs):
    """
    NamedColour(String colorName) -> Colour

    Constructs a colour object using a colour name listed in
    ``wx.TheColourDatabase``.
    """
    val = _gdi_.new_NamedColour(*args, **kwargs)
    return val

def ColourRGB(*args, **kwargs):
    """
    ColourRGB(unsigned long colRGB) -> Colour

    Constructs a colour from a packed RGB value.
    """
    val = _gdi_.new_ColourRGB(*args, **kwargs)
    return val

Color = Colour
NamedColor = NamedColour
ColorRGB = ColourRGB

class Palette(GDIObject):
    """Proxy of C++ Palette class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int n, unsigned char red, unsigned char green, unsigned char blue) -> Palette"""
        _gdi_.Palette_swiginit(self,_gdi_.new_Palette(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Palette
    __del__ = lambda self : None;
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
_gdi_.Palette_swigregister(Palette)

#---------------------------------------------------------------------------

class Pen(GDIObject):
    """Proxy of C++ Pen class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Colour colour, int width=1, int style=SOLID) -> Pen"""
        _gdi_.Pen_swiginit(self,_gdi_.new_Pen(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Pen
    __del__ = lambda self : None;
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

    def GetDashCount(*args, **kwargs):
        """GetDashCount(self) -> int"""
        return _gdi_.Pen_GetDashCount(*args, **kwargs)

    def GetStipple(*args, **kwargs):
        """GetStipple(self) -> Bitmap"""
        return _gdi_.Pen_GetStipple(*args, **kwargs)

    def SetStipple(*args, **kwargs):
        """SetStipple(self, Bitmap stipple)"""
        return _gdi_.Pen_SetStipple(*args, **kwargs)

    def __eq__(*args, **kwargs):
        """__eq__(self, Pen other) -> bool"""
        return _gdi_.Pen___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, Pen other) -> bool"""
        return _gdi_.Pen___ne__(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
_gdi_.Pen_swigregister(Pen)

#---------------------------------------------------------------------------

class Brush(GDIObject):
    """
    A brush is a drawing tool for filling in areas. It is used for
    painting the background of rectangles, ellipses, etc. when drawing on
    a `wx.DC`.  It has a colour and a style.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Colour colour, int style=SOLID) -> Brush

        Constructs a brush from a `wx.Colour` object and a style.
        """
        _gdi_.Brush_swiginit(self,_gdi_.new_Brush(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Brush
    __del__ = lambda self : None;
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

    def __nonzero__(self): return self.Ok() 
_gdi_.Brush_swigregister(Brush)

def BrushFromBitmap(*args, **kwargs):
    """
    BrushFromBitmap(Bitmap stippleBitmap) -> Brush

    Constructs a stippled brush using a bitmap.
    """
    val = _gdi_.new_BrushFromBitmap(*args, **kwargs)
    return val

class Bitmap(GDIObject):
    """
    The wx.Bitmap class encapsulates the concept of a platform-dependent
    bitmap.  It can be either monochrome or colour, and either loaded from
    a file or created dynamically.  A bitmap can be selected into a memory
    device context (instance of `wx.MemoryDC`). This enables the bitmap to
    be copied to a window or memory device context using `wx.DC.Blit`, or
    to be used as a drawing surface.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String name, int type=BITMAP_TYPE_ANY) -> Bitmap

        Loads a bitmap from a file.
        """
        _gdi_.Bitmap_swiginit(self,_gdi_.new_Bitmap(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Bitmap
    __del__ = lambda self : None;
    def GetHandle(*args, **kwargs):
        """GetHandle(self) -> long"""
        return _gdi_.Bitmap_GetHandle(*args, **kwargs)

    def SetHandle(*args, **kwargs):
        """SetHandle(self, long handle)"""
        return _gdi_.Bitmap_SetHandle(*args, **kwargs)

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

    def SetPalette(*args, **kwargs):
        """SetPalette(self, Palette palette)"""
        return _gdi_.Bitmap_SetPalette(*args, **kwargs)

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

    def CopyFromCursor(*args, **kwargs):
        """CopyFromCursor(self, Cursor cursor) -> bool"""
        return _gdi_.Bitmap_CopyFromCursor(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
    def __eq__(*args, **kwargs):
        """__eq__(self, Bitmap other) -> bool"""
        return _gdi_.Bitmap___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, Bitmap other) -> bool"""
        return _gdi_.Bitmap___ne__(*args, **kwargs)

_gdi_.Bitmap_swigregister(Bitmap)

def EmptyBitmap(*args, **kwargs):
    """
    EmptyBitmap(int width, int height, int depth=-1) -> Bitmap

    Creates a new bitmap of the given size.  A depth of -1 indicates the
    depth of the current screen or visual. Some platforms only support 1
    for monochrome and -1 for the current display depth.
    """
    val = _gdi_.new_EmptyBitmap(*args, **kwargs)
    return val

def BitmapFromIcon(*args, **kwargs):
    """
    BitmapFromIcon(Icon icon) -> Bitmap

    Create a new bitmap from a `wx.Icon` object.
    """
    val = _gdi_.new_BitmapFromIcon(*args, **kwargs)
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
    return val

def BitmapFromXPMData(*args, **kwargs):
    """
    BitmapFromXPMData(PyObject listOfStrings) -> Bitmap

    Construct a Bitmap from a list of strings formatted as XPM data.
    """
    val = _gdi_.new_BitmapFromXPMData(*args, **kwargs)
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
    return val


def _BitmapFromBufferAlpha(*args, **kwargs):
  """_BitmapFromBufferAlpha(int width, int height, buffer data, buffer alpha) -> Bitmap"""
  return _gdi_._BitmapFromBufferAlpha(*args, **kwargs)

def _BitmapFromBuffer(*args, **kwargs):
  """_BitmapFromBuffer(int width, int height, buffer data) -> Bitmap"""
  return _gdi_._BitmapFromBuffer(*args, **kwargs)
def BitmapFromBuffer(width, height, dataBuffer, alphaBuffer=None):
    """
    Creates a `wx.Bitmap` from the data in dataBuffer.  The dataBuffer
    parameter must be a Python object that implements the buffer interface, or
    is convertable to a buffer object, such as a string, array, etc.  The
    dataBuffer object is expected to contain a series of RGB bytes and be
    width*height*3 bytes long.  A buffer object can optionally be supplied for
    the image's alpha channel data, and it is expected to be width*height
    bytes long.  On Windows the RGB values are 'premultiplied' by the alpha
    values.  (The other platforms appear to already be premultiplying the
    alpha.)

    Unlike `wx.ImageFromBuffer` the bitmap created with this function does not
    share the memory buffer with the buffer object.  This is because the
    native pixel buffer format varies on different platforms, and so instead
    an efficient as possible copy of the data is made from the buffer objects
    to the bitmap's native pixel buffer.  For direct access to a bitmap's
    pixel buffer see `wx.NativePixelData` and `wx.AlphaPixelData`.

    :see: `wx.Bitmap`, `wx.BitmapFromBufferRGBA`, `wx.NativePixelData`,
          `wx.AlphaPixelData`, `wx.ImageFromBuffer`
    """
    if not isinstance(dataBuffer, buffer):
        dataBuffer = buffer(dataBuffer)
    if alphaBuffer is not None and not isinstance(alphaBuffer, buffer):
        alphaBuffer = buffer(alphaBuffer)
    if alphaBuffer is not None:
        return _gdi_._BitmapFromBufferAlpha(width, height, dataBuffer, alphaBuffer)
    else:
        return _gdi_._BitmapFromBuffer(width, height, dataBuffer)


def _BitmapFromBufferRGBA(*args, **kwargs):
  """_BitmapFromBufferRGBA(int width, int height, buffer data) -> Bitmap"""
  return _gdi_._BitmapFromBufferRGBA(*args, **kwargs)
def BitmapFromBufferRGBA(width, height, dataBuffer):
    """
    Creates a `wx.Bitmap` from the data in dataBuffer.  The dataBuffer
    parameter must be a Python object that implements the buffer interface, or
    is convertable to a buffer object, such as a string, array, etc.  The
    dataBuffer object is expected to contain a series of RGBA bytes (red,
    green, blue and alpha) and be width*height*4 bytes long.  On Windows the
    RGB values are 'premultiplied' by the alpha values.  (The other platforms
    appear to already be premultiplying the alpha.)

    Unlike `wx.ImageFromBuffer` the bitmap created with this function does not
    share the memory buffer with the buffer object.  This is because the
    native pixel buffer format varies on different platforms, and so instead
    an efficient as possible copy of the data is made from the buffer object
    to the bitmap's native pixel buffer.  For direct access to a bitmap's
    pixel buffer see `wx.NativePixelData` and `wx.AlphaPixelData`.

    :see: `wx.Bitmap`, `wx.BitmapFromBuffer`, `wx.NativePixelData`,
          `wx.AlphaPixelData`, `wx.ImageFromBuffer`
    """
    if not isinstance(dataBuffer, buffer):
        dataBuffer = buffer(dataBuffer)
    return _gdi_._BitmapFromBufferRGBA(width, height, dataBuffer)

class PixelDataBase(object):
    """Proxy of C++ PixelDataBase class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def GetOrigin(*args, **kwargs):
        """GetOrigin(self) -> Point"""
        return _gdi_.PixelDataBase_GetOrigin(*args, **kwargs)

    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _gdi_.PixelDataBase_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return _gdi_.PixelDataBase_GetHeight(*args, **kwargs)

    def GetSize(*args, **kwargs):
        """GetSize(self) -> Size"""
        return _gdi_.PixelDataBase_GetSize(*args, **kwargs)

    def GetRowStride(*args, **kwargs):
        """GetRowStride(self) -> int"""
        return _gdi_.PixelDataBase_GetRowStride(*args, **kwargs)

_gdi_.PixelDataBase_swigregister(PixelDataBase)

class NativePixelData(PixelDataBase):
    """Proxy of C++ NativePixelData class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, Bitmap bmp) -> NativePixelData
        __init__(self, Bitmap bmp, Rect rect) -> NativePixelData
        __init__(self, Bitmap bmp, Point pt, Size sz) -> NativePixelData
        """
        _gdi_.NativePixelData_swiginit(self,_gdi_.new_NativePixelData(*args))
    __swig_destroy__ = _gdi_.delete_NativePixelData
    __del__ = lambda self : None;
    def GetPixels(*args, **kwargs):
        """GetPixels(self) -> NativePixelData_Accessor"""
        return _gdi_.NativePixelData_GetPixels(*args, **kwargs)

    def UseAlpha(*args, **kwargs):
        """UseAlpha(self)"""
        return _gdi_.NativePixelData_UseAlpha(*args, **kwargs)

    def __nonzero__(*args, **kwargs):
        """__nonzero__(self) -> bool"""
        return _gdi_.NativePixelData___nonzero__(*args, **kwargs)

    def __iter__(self):
        """Create and return an iterator object for this pixel data object."""
        return self.PixelIterator(self)

    class PixelIterator(object):
        """
        Sequential iterator which returns pixel accessor objects starting at the
        the top-left corner, and going row-by-row until the bottom-right
        corner is reached.
        """

        class PixelAccessor(object):
            """
            This class is what is returned by the iterator and allows the pixel
            to be Get or Set.
            """
            def __init__(self, data, pixels, x, y):
                self.data = data
                self.pixels = pixels
                self.x = x
                self.y = y
            def Set(self, *args, **kw):
                self.pixels.MoveTo(self.data, self.x, self.y)
                return self.pixels.Set(*args, **kw)
            def Get(self):
                self.pixels.MoveTo(self.data, self.x, self.y)
                return self.pixels.Get()

        def __init__(self, pixelData):
            self.x = self.y = 0
            self.w = pixelData.GetWidth()
            self.h = pixelData.GetHeight()
            self.data = pixelData
            self.pixels = pixelData.GetPixels()

        def __iter__(self):
            return self

        def next(self):
            if self.y >= self.h:
                raise StopIteration
            p = self.PixelAccessor(self.data, self.pixels, self.x, self.y)
            self.x += 1
            if self.x >= self.w:
                self.x = 0
                self.y += 1
            return p

_gdi_.NativePixelData_swigregister(NativePixelData)

class NativePixelData_Accessor(object):
    """Proxy of C++ NativePixelData_Accessor class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, NativePixelData data) -> NativePixelData_Accessor
        __init__(self, Bitmap bmp, NativePixelData data) -> NativePixelData_Accessor
        __init__(self) -> NativePixelData_Accessor
        """
        _gdi_.NativePixelData_Accessor_swiginit(self,_gdi_.new_NativePixelData_Accessor(*args))
    __swig_destroy__ = _gdi_.delete_NativePixelData_Accessor
    __del__ = lambda self : None;
    def Reset(*args, **kwargs):
        """Reset(self, NativePixelData data)"""
        return _gdi_.NativePixelData_Accessor_Reset(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _gdi_.NativePixelData_Accessor_IsOk(*args, **kwargs)

    def nextPixel(*args, **kwargs):
        """nextPixel(self)"""
        return _gdi_.NativePixelData_Accessor_nextPixel(*args, **kwargs)

    def Offset(*args, **kwargs):
        """Offset(self, NativePixelData data, int x, int y)"""
        return _gdi_.NativePixelData_Accessor_Offset(*args, **kwargs)

    def OffsetX(*args, **kwargs):
        """OffsetX(self, NativePixelData data, int x)"""
        return _gdi_.NativePixelData_Accessor_OffsetX(*args, **kwargs)

    def OffsetY(*args, **kwargs):
        """OffsetY(self, NativePixelData data, int y)"""
        return _gdi_.NativePixelData_Accessor_OffsetY(*args, **kwargs)

    def MoveTo(*args, **kwargs):
        """MoveTo(self, NativePixelData data, int x, int y)"""
        return _gdi_.NativePixelData_Accessor_MoveTo(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(self, byte red, byte green, byte blue)"""
        return _gdi_.NativePixelData_Accessor_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """Get(self) -> PyObject"""
        return _gdi_.NativePixelData_Accessor_Get(*args, **kwargs)

_gdi_.NativePixelData_Accessor_swigregister(NativePixelData_Accessor)

class AlphaPixelData(PixelDataBase):
    """Proxy of C++ AlphaPixelData class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, Bitmap bmp) -> AlphaPixelData
        __init__(self, Bitmap bmp, Rect rect) -> AlphaPixelData
        __init__(self, Bitmap bmp, Point pt, Size sz) -> AlphaPixelData
        """
        _gdi_.AlphaPixelData_swiginit(self,_gdi_.new_AlphaPixelData(*args))
        self.UseAlpha()

    __swig_destroy__ = _gdi_.delete_AlphaPixelData
    __del__ = lambda self : None;
    def GetPixels(*args, **kwargs):
        """GetPixels(self) -> AlphaPixelData_Accessor"""
        return _gdi_.AlphaPixelData_GetPixels(*args, **kwargs)

    def UseAlpha(*args, **kwargs):
        """UseAlpha(self)"""
        return _gdi_.AlphaPixelData_UseAlpha(*args, **kwargs)

    def __nonzero__(*args, **kwargs):
        """__nonzero__(self) -> bool"""
        return _gdi_.AlphaPixelData___nonzero__(*args, **kwargs)

    def __iter__(self):
        """Create and return an iterator object for this pixel data object."""
        return self.PixelIterator(self)

    class PixelIterator(object):
        """
        Sequential iterator which returns pixel accessor objects starting at the
        the top-left corner, and going row-by-row until the bottom-right
        corner is reached.
        """

        class PixelAccessor(object):
            """
            This class is what is returned by the iterator and allows the pixel
            to be Get or Set.
            """
            def __init__(self, data, pixels, x, y):
                self.data = data
                self.pixels = pixels
                self.x = x
                self.y = y
            def Set(self, *args, **kw):
                self.pixels.MoveTo(self.data, self.x, self.y)
                return self.pixels.Set(*args, **kw)
            def Get(self):
                self.pixels.MoveTo(self.data, self.x, self.y)
                return self.pixels.Get()

        def __init__(self, pixelData):
            self.x = self.y = 0
            self.w = pixelData.GetWidth()
            self.h = pixelData.GetHeight()
            self.data = pixelData
            self.pixels = pixelData.GetPixels()

        def __iter__(self):
            return self

        def next(self):
            if self.y >= self.h:
                raise StopIteration
            p = self.PixelAccessor(self.data, self.pixels, self.x, self.y)
            self.x += 1
            if self.x >= self.w:
                self.x = 0
                self.y += 1
            return p

_gdi_.AlphaPixelData_swigregister(AlphaPixelData)

class AlphaPixelData_Accessor(object):
    """Proxy of C++ AlphaPixelData_Accessor class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, AlphaPixelData data) -> AlphaPixelData_Accessor
        __init__(self, Bitmap bmp, AlphaPixelData data) -> AlphaPixelData_Accessor
        __init__(self) -> AlphaPixelData_Accessor
        """
        _gdi_.AlphaPixelData_Accessor_swiginit(self,_gdi_.new_AlphaPixelData_Accessor(*args))
    __swig_destroy__ = _gdi_.delete_AlphaPixelData_Accessor
    __del__ = lambda self : None;
    def Reset(*args, **kwargs):
        """Reset(self, AlphaPixelData data)"""
        return _gdi_.AlphaPixelData_Accessor_Reset(*args, **kwargs)

    def IsOk(*args, **kwargs):
        """IsOk(self) -> bool"""
        return _gdi_.AlphaPixelData_Accessor_IsOk(*args, **kwargs)

    def nextPixel(*args, **kwargs):
        """nextPixel(self)"""
        return _gdi_.AlphaPixelData_Accessor_nextPixel(*args, **kwargs)

    def Offset(*args, **kwargs):
        """Offset(self, AlphaPixelData data, int x, int y)"""
        return _gdi_.AlphaPixelData_Accessor_Offset(*args, **kwargs)

    def OffsetX(*args, **kwargs):
        """OffsetX(self, AlphaPixelData data, int x)"""
        return _gdi_.AlphaPixelData_Accessor_OffsetX(*args, **kwargs)

    def OffsetY(*args, **kwargs):
        """OffsetY(self, AlphaPixelData data, int y)"""
        return _gdi_.AlphaPixelData_Accessor_OffsetY(*args, **kwargs)

    def MoveTo(*args, **kwargs):
        """MoveTo(self, AlphaPixelData data, int x, int y)"""
        return _gdi_.AlphaPixelData_Accessor_MoveTo(*args, **kwargs)

    def Set(*args, **kwargs):
        """Set(self, byte red, byte green, byte blue, byte alpha)"""
        return _gdi_.AlphaPixelData_Accessor_Set(*args, **kwargs)

    def Get(*args, **kwargs):
        """Get(self) -> PyObject"""
        return _gdi_.AlphaPixelData_Accessor_Get(*args, **kwargs)

_gdi_.AlphaPixelData_Accessor_swigregister(AlphaPixelData_Accessor)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
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
        _gdi_.Mask_swiginit(self,_gdi_.new_Mask(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Mask
    __del__ = lambda self : None;
_gdi_.Mask_swigregister(Mask)

MaskColour = wx._deprecated(Mask, "wx.MaskColour is deprecated, use `wx.Mask` instead.") 
class Icon(GDIObject):
    """Proxy of C++ Icon class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String name, int type, int desiredWidth=-1, int desiredHeight=-1) -> Icon"""
        _gdi_.Icon_swiginit(self,_gdi_.new_Icon(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Icon
    __del__ = lambda self : None;
    def LoadFile(*args, **kwargs):
        """LoadFile(self, String name, int type) -> bool"""
        return _gdi_.Icon_LoadFile(*args, **kwargs)

    def GetHandle(*args, **kwargs):
        """GetHandle(self) -> long"""
        return _gdi_.Icon_GetHandle(*args, **kwargs)

    def SetHandle(*args, **kwargs):
        """SetHandle(self, long handle)"""
        return _gdi_.Icon_SetHandle(*args, **kwargs)

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

    def SetSize(*args, **kwargs):
        """SetSize(self, Size size)"""
        return _gdi_.Icon_SetSize(*args, **kwargs)

    def CopyFromBitmap(*args, **kwargs):
        """CopyFromBitmap(self, Bitmap bmp)"""
        return _gdi_.Icon_CopyFromBitmap(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
_gdi_.Icon_swigregister(Icon)

def EmptyIcon(*args, **kwargs):
    """EmptyIcon() -> Icon"""
    val = _gdi_.new_EmptyIcon(*args, **kwargs)
    return val

def IconFromLocation(*args, **kwargs):
    """IconFromLocation(IconLocation loc) -> Icon"""
    val = _gdi_.new_IconFromLocation(*args, **kwargs)
    return val

def IconFromBitmap(*args, **kwargs):
    """IconFromBitmap(Bitmap bmp) -> Icon"""
    val = _gdi_.new_IconFromBitmap(*args, **kwargs)
    return val

def IconFromXPMData(*args, **kwargs):
    """IconFromXPMData(PyObject listOfStrings) -> Icon"""
    val = _gdi_.new_IconFromXPMData(*args, **kwargs)
    return val

class IconLocation(object):
    """Proxy of C++ IconLocation class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String filename=&wxPyEmptyString, int num=0) -> IconLocation"""
        _gdi_.IconLocation_swiginit(self,_gdi_.new_IconLocation(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_IconLocation
    __del__ = lambda self : None;
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

_gdi_.IconLocation_swigregister(IconLocation)

class IconBundle(object):
    """Proxy of C++ IconBundle class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> IconBundle"""
        _gdi_.IconBundle_swiginit(self,_gdi_.new_IconBundle(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_IconBundle
    __del__ = lambda self : None;
    def AddIcon(*args, **kwargs):
        """AddIcon(self, Icon icon)"""
        return _gdi_.IconBundle_AddIcon(*args, **kwargs)

    def AddIconFromFile(*args, **kwargs):
        """AddIconFromFile(self, String file, long type)"""
        return _gdi_.IconBundle_AddIconFromFile(*args, **kwargs)

    def GetIcon(*args, **kwargs):
        """GetIcon(self, Size size) -> Icon"""
        return _gdi_.IconBundle_GetIcon(*args, **kwargs)

_gdi_.IconBundle_swigregister(IconBundle)

def IconBundleFromFile(*args, **kwargs):
    """IconBundleFromFile(String file, long type) -> IconBundle"""
    val = _gdi_.new_IconBundleFromFile(*args, **kwargs)
    return val

def IconBundleFromIcon(*args, **kwargs):
    """IconBundleFromIcon(Icon icon) -> IconBundle"""
    val = _gdi_.new_IconBundleFromIcon(*args, **kwargs)
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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String cursorName, long type, int hotSpotX=0, int hotSpotY=0) -> Cursor

        Construct a Cursor from a file.  Specify the type of file using
        wx.BITAMP_TYPE* constants, and specify the hotspot if not using a .cur
        file.
        """
        _gdi_.Cursor_swiginit(self,_gdi_.new_Cursor(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Cursor
    __del__ = lambda self : None;
    def GetHandle(*args, **kwargs):
        """
        GetHandle(self) -> long

        Get the MS Windows handle for the cursor
        """
        return _gdi_.Cursor_GetHandle(*args, **kwargs)

    def SetHandle(*args, **kwargs):
        """
        SetHandle(self, long handle)

        Set the MS Windows handle to use for the cursor
        """
        return _gdi_.Cursor_SetHandle(*args, **kwargs)

    def Ok(*args, **kwargs):
        """Ok(self) -> bool"""
        return _gdi_.Cursor_Ok(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
    def GetWidth(*args, **kwargs):
        """GetWidth(self) -> int"""
        return _gdi_.Cursor_GetWidth(*args, **kwargs)

    def GetHeight(*args, **kwargs):
        """GetHeight(self) -> int"""
        return _gdi_.Cursor_GetHeight(*args, **kwargs)

    def GetDepth(*args, **kwargs):
        """GetDepth(self) -> int"""
        return _gdi_.Cursor_GetDepth(*args, **kwargs)

    def SetWidth(*args, **kwargs):
        """SetWidth(self, int w)"""
        return _gdi_.Cursor_SetWidth(*args, **kwargs)

    def SetHeight(*args, **kwargs):
        """SetHeight(self, int h)"""
        return _gdi_.Cursor_SetHeight(*args, **kwargs)

    def SetDepth(*args, **kwargs):
        """SetDepth(self, int d)"""
        return _gdi_.Cursor_SetDepth(*args, **kwargs)

    def SetSize(*args, **kwargs):
        """SetSize(self, Size size)"""
        return _gdi_.Cursor_SetSize(*args, **kwargs)

_gdi_.Cursor_swigregister(Cursor)

def StockCursor(*args, **kwargs):
    """
    StockCursor(int id) -> Cursor

    Create a cursor using one of the stock cursors.  Note that not all
    stock cursors are available on all platforms.
    """
    val = _gdi_.new_StockCursor(*args, **kwargs)
    return val

def CursorFromImage(*args, **kwargs):
    """
    CursorFromImage(Image image) -> Cursor

    Constructs a cursor from a `wx.Image`. The mask (if any) will be used
    for setting the transparent portions of the cursor.
    """
    val = _gdi_.new_CursorFromImage(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

OutRegion = _gdi_.OutRegion
PartRegion = _gdi_.PartRegion
InRegion = _gdi_.InRegion
class Region(GDIObject):
    """Proxy of C++ Region class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int x=0, int y=0, int width=0, int height=0) -> Region"""
        _gdi_.Region_swiginit(self,_gdi_.new_Region(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Region
    __del__ = lambda self : None;
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

_gdi_.Region_swigregister(Region)

def RegionFromBitmap(*args, **kwargs):
    """RegionFromBitmap(Bitmap bmp) -> Region"""
    val = _gdi_.new_RegionFromBitmap(*args, **kwargs)
    return val

def RegionFromBitmapColour(*args, **kwargs):
    """RegionFromBitmapColour(Bitmap bmp, Colour transColour, int tolerance=0) -> Region"""
    val = _gdi_.new_RegionFromBitmapColour(*args, **kwargs)
    return val

def RegionFromPoints(*args, **kwargs):
    """RegionFromPoints(int points, int fillStyle=WINDING_RULE) -> Region"""
    val = _gdi_.new_RegionFromPoints(*args, **kwargs)
    return val

class RegionIterator(_core.Object):
    """Proxy of C++ RegionIterator class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, Region region) -> RegionIterator"""
        _gdi_.RegionIterator_swiginit(self,_gdi_.new_RegionIterator(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_RegionIterator
    __del__ = lambda self : None;
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

_gdi_.RegionIterator_swigregister(RegionIterator)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> NativeFontInfo"""
        _gdi_.NativeFontInfo_swiginit(self,_gdi_.new_NativeFontInfo(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_NativeFontInfo
    __del__ = lambda self : None;
    def Init(*args, **kwargs):
        """Init(self)"""
        return _gdi_.NativeFontInfo_Init(*args, **kwargs)

    def InitFromFont(*args, **kwargs):
        """InitFromFont(self, Font font)"""
        return _gdi_.NativeFontInfo_InitFromFont(*args, **kwargs)

    def GetPointSize(*args, **kwargs):
        """GetPointSize(self) -> int"""
        return _gdi_.NativeFontInfo_GetPointSize(*args, **kwargs)

    def GetPixelSize(*args, **kwargs):
        """GetPixelSize(self) -> Size"""
        return _gdi_.NativeFontInfo_GetPixelSize(*args, **kwargs)

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

    def SetPixelSize(*args, **kwargs):
        """SetPixelSize(self, Size pixelSize)"""
        return _gdi_.NativeFontInfo_SetPixelSize(*args, **kwargs)

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
        """SetFaceName(self, String facename) -> bool"""
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

_gdi_.NativeFontInfo_swigregister(NativeFontInfo)

class NativeEncodingInfo(object):
    """Proxy of C++ NativeEncodingInfo class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    facename = property(_gdi_.NativeEncodingInfo_facename_get, _gdi_.NativeEncodingInfo_facename_set)
    encoding = property(_gdi_.NativeEncodingInfo_encoding_get, _gdi_.NativeEncodingInfo_encoding_set)
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> NativeEncodingInfo"""
        _gdi_.NativeEncodingInfo_swiginit(self,_gdi_.new_NativeEncodingInfo(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_NativeEncodingInfo
    __del__ = lambda self : None;
    def FromString(*args, **kwargs):
        """FromString(self, String s) -> bool"""
        return _gdi_.NativeEncodingInfo_FromString(*args, **kwargs)

    def ToString(*args, **kwargs):
        """ToString(self) -> String"""
        return _gdi_.NativeEncodingInfo_ToString(*args, **kwargs)

_gdi_.NativeEncodingInfo_swigregister(NativeEncodingInfo)


def GetNativeFontEncoding(*args, **kwargs):
  """GetNativeFontEncoding(int encoding) -> NativeEncodingInfo"""
  return _gdi_.GetNativeFontEncoding(*args, **kwargs)

def TestFontEncoding(*args, **kwargs):
  """TestFontEncoding(NativeEncodingInfo info) -> bool"""
  return _gdi_.TestFontEncoding(*args, **kwargs)
#---------------------------------------------------------------------------

class FontMapper(object):
    """Proxy of C++ FontMapper class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> FontMapper"""
        _gdi_.FontMapper_swiginit(self,_gdi_.new_FontMapper(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_FontMapper
    __del__ = lambda self : None;
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

_gdi_.FontMapper_swigregister(FontMapper)

def FontMapper_Get(*args):
  """FontMapper_Get() -> FontMapper"""
  return _gdi_.FontMapper_Get(*args)

def FontMapper_Set(*args, **kwargs):
  """FontMapper_Set(FontMapper mapper) -> FontMapper"""
  return _gdi_.FontMapper_Set(*args, **kwargs)

def FontMapper_GetSupportedEncodingsCount(*args):
  """FontMapper_GetSupportedEncodingsCount() -> size_t"""
  return _gdi_.FontMapper_GetSupportedEncodingsCount(*args)

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

def FontMapper_GetDefaultConfigPath(*args):
  """FontMapper_GetDefaultConfigPath() -> String"""
  return _gdi_.FontMapper_GetDefaultConfigPath(*args)

#---------------------------------------------------------------------------

class Font(GDIObject):
    """
    A font is an object which determines the appearance of text. Fonts are
    used for drawing text to a device context, and setting the appearance
    of a window's text.

    You can retrieve the current system font settings with `wx.SystemSettings`.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int pointSize, int family, int style, int weight, bool underline=False, 
            String face=EmptyString, 
            int encoding=FONTENCODING_DEFAULT) -> Font

        Creates a font object with the specified attributes.

            :param pointSize:  The size of the font in points.

            :param family: Font family.  A generic way of referring to fonts
                without specifying actual facename.  It can be One of 
                the ``wx.FONTFAMILY_xxx`` constants.

            :param style: One of the ``wx.FONTSTYLE_xxx`` constants.

            :param weight: Font weight, sometimes also referred to as font
                boldness. One of the ``wx.FONTWEIGHT_xxx`` constants.

            :param underline: The value can be ``True`` or ``False`` and
                indicates whether the font will include an underline.  This
                may not be supported on all platforms.

            :param face: An optional string specifying the actual typeface to
                be used. If it is an empty string, a default typeface will be
                chosen based on the family.

            :param encoding: An encoding which may be one of the
                ``wx.FONTENCODING_xxx`` constants.  If the specified encoding isn't
                available, no font is created.

        :see: `wx.FFont`, `wx.FontFromPixelSize`, `wx.FFontFromPixelSize`,
            `wx.FontFromNativeInfoString`, `wx.FontFromNativeInfo`

        """
        if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
        _gdi_.Font_swiginit(self,_gdi_.new_Font(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Font
    __del__ = lambda self : None;
    def Ok(*args, **kwargs):
        """
        Ok(self) -> bool

        Returns ``True`` if this font was successfully created.
        """
        return _gdi_.Font_Ok(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
    def __eq__(*args, **kwargs):
        """__eq__(self, Font other) -> bool"""
        return _gdi_.Font___eq__(*args, **kwargs)

    def __ne__(*args, **kwargs):
        """__ne__(self, Font other) -> bool"""
        return _gdi_.Font___ne__(*args, **kwargs)

    def GetPointSize(*args, **kwargs):
        """
        GetPointSize(self) -> int

        Gets the point size.
        """
        return _gdi_.Font_GetPointSize(*args, **kwargs)

    def GetPixelSize(*args, **kwargs):
        """
        GetPixelSize(self) -> Size

        Returns the size in pixels if the font was constructed with a pixel
        size.
        """
        return _gdi_.Font_GetPixelSize(*args, **kwargs)

    def IsUsingSizeInPixels(*args, **kwargs):
        """
        IsUsingSizeInPixels(self) -> bool

        Returns ``True`` if the font is using a pixelSize.
        """
        return _gdi_.Font_IsUsingSizeInPixels(*args, **kwargs)

    def GetFamily(*args, **kwargs):
        """
        GetFamily(self) -> int

        Gets the font family. 
        """
        return _gdi_.Font_GetFamily(*args, **kwargs)

    def GetStyle(*args, **kwargs):
        """
        GetStyle(self) -> int

        Gets the font style.
        """
        return _gdi_.Font_GetStyle(*args, **kwargs)

    def GetWeight(*args, **kwargs):
        """
        GetWeight(self) -> int

        Gets the font weight. 
        """
        return _gdi_.Font_GetWeight(*args, **kwargs)

    def GetUnderlined(*args, **kwargs):
        """
        GetUnderlined(self) -> bool

        Returns ``True`` if the font is underlined, ``False`` otherwise.
        """
        return _gdi_.Font_GetUnderlined(*args, **kwargs)

    def GetFaceName(*args, **kwargs):
        """
        GetFaceName(self) -> String

        Returns the typeface name associated with the font, or the empty
        string if there is no typeface information.
        """
        return _gdi_.Font_GetFaceName(*args, **kwargs)

    def GetEncoding(*args, **kwargs):
        """
        GetEncoding(self) -> int

        Get the font's encoding.
        """
        return _gdi_.Font_GetEncoding(*args, **kwargs)

    def GetNativeFontInfo(*args, **kwargs):
        """
        GetNativeFontInfo(self) -> NativeFontInfo

        Constructs a `wx.NativeFontInfo` object from this font.
        """
        return _gdi_.Font_GetNativeFontInfo(*args, **kwargs)

    def IsFixedWidth(*args, **kwargs):
        """
        IsFixedWidth(self) -> bool

        Returns true if the font is a fixed width (or monospaced) font, false
        if it is a proportional one or font is invalid.
        """
        return _gdi_.Font_IsFixedWidth(*args, **kwargs)

    def GetNativeFontInfoDesc(*args, **kwargs):
        """
        GetNativeFontInfoDesc(self) -> String

        Returns the platform-dependent string completely describing this font
        or an empty string if the font isn't valid.
        """
        return _gdi_.Font_GetNativeFontInfoDesc(*args, **kwargs)

    def GetNativeFontInfoUserDesc(*args, **kwargs):
        """
        GetNativeFontInfoUserDesc(self) -> String

        Returns a human readable version of `GetNativeFontInfoDesc`.
        """
        return _gdi_.Font_GetNativeFontInfoUserDesc(*args, **kwargs)

    def SetPointSize(*args, **kwargs):
        """
        SetPointSize(self, int pointSize)

        Sets the point size.
        """
        return _gdi_.Font_SetPointSize(*args, **kwargs)

    def SetPixelSize(*args, **kwargs):
        """
        SetPixelSize(self, Size pixelSize)

        Sets the size in pixels rather than points.  If there is platform API
        support for this then it is used, otherwise a font with the closest
        size is found using a binary search.
        """
        return _gdi_.Font_SetPixelSize(*args, **kwargs)

    def SetFamily(*args, **kwargs):
        """
        SetFamily(self, int family)

        Sets the font family.
        """
        return _gdi_.Font_SetFamily(*args, **kwargs)

    def SetStyle(*args, **kwargs):
        """
        SetStyle(self, int style)

        Sets the font style.
        """
        return _gdi_.Font_SetStyle(*args, **kwargs)

    def SetWeight(*args, **kwargs):
        """
        SetWeight(self, int weight)

        Sets the font weight.
        """
        return _gdi_.Font_SetWeight(*args, **kwargs)

    def SetFaceName(*args, **kwargs):
        """
        SetFaceName(self, String faceName) -> bool

        Sets the facename for the font.  The facename, which should be a valid
        font installed on the end-user's system.

        To avoid portability problems, don't rely on a specific face, but
        specify the font family instead or as well. A suitable font will be
        found on the end-user's system. If both the family and the facename
        are specified, wxWidgets will first search for the specific face, and
        then for a font belonging to the same family.
        """
        return _gdi_.Font_SetFaceName(*args, **kwargs)

    def SetUnderlined(*args, **kwargs):
        """
        SetUnderlined(self, bool underlined)

        Sets underlining.
        """
        return _gdi_.Font_SetUnderlined(*args, **kwargs)

    def SetEncoding(*args, **kwargs):
        """
        SetEncoding(self, int encoding)

        Set the font encoding.
        """
        return _gdi_.Font_SetEncoding(*args, **kwargs)

    def SetNativeFontInfo(*args, **kwargs):
        """
        SetNativeFontInfo(self, NativeFontInfo info)

        Set the font's attributes from a `wx.NativeFontInfo` object.
        """
        return _gdi_.Font_SetNativeFontInfo(*args, **kwargs)

    def SetNativeFontInfoFromString(*args, **kwargs):
        """
        SetNativeFontInfoFromString(self, String info) -> bool

        Set the font's attributes from string representation of a
        `wx.NativeFontInfo` object.
        """
        return _gdi_.Font_SetNativeFontInfoFromString(*args, **kwargs)

    def SetNativeFontInfoUserDesc(*args, **kwargs):
        """
        SetNativeFontInfoUserDesc(self, String info) -> bool

        Set the font's attributes from a string formerly returned from
        `GetNativeFontInfoDesc`.
        """
        return _gdi_.Font_SetNativeFontInfoUserDesc(*args, **kwargs)

    def GetFamilyString(*args, **kwargs):
        """
        GetFamilyString(self) -> String

        Returns a string representation of the font family.
        """
        return _gdi_.Font_GetFamilyString(*args, **kwargs)

    def GetStyleString(*args, **kwargs):
        """
        GetStyleString(self) -> String

        Returns a string representation of the font style.
        """
        return _gdi_.Font_GetStyleString(*args, **kwargs)

    def GetWeightString(*args, **kwargs):
        """
        GetWeightString(self) -> String

        Return a string representation of the font weight.
        """
        return _gdi_.Font_GetWeightString(*args, **kwargs)

    def SetNoAntiAliasing(*args, **kwargs):
        """SetNoAntiAliasing(self, bool no=True)"""
        return _gdi_.Font_SetNoAntiAliasing(*args, **kwargs)

    def GetNoAntiAliasing(*args, **kwargs):
        """GetNoAntiAliasing(self) -> bool"""
        return _gdi_.Font_GetNoAntiAliasing(*args, **kwargs)

    def GetDefaultEncoding(*args, **kwargs):
        """
        GetDefaultEncoding() -> int

        Returns the encoding used for all fonts created with an encoding of
        ``wx.FONTENCODING_DEFAULT``.
        """
        return _gdi_.Font_GetDefaultEncoding(*args, **kwargs)

    GetDefaultEncoding = staticmethod(GetDefaultEncoding)
    def SetDefaultEncoding(*args, **kwargs):
        """
        SetDefaultEncoding(int encoding)

        Sets the default font encoding.
        """
        return _gdi_.Font_SetDefaultEncoding(*args, **kwargs)

    SetDefaultEncoding = staticmethod(SetDefaultEncoding)
_gdi_.Font_swigregister(Font)

def FontFromNativeInfo(*args, **kwargs):
    """
    FontFromNativeInfo(NativeFontInfo info) -> Font

    Construct a `wx.Font` from a `wx.NativeFontInfo` object.
    """
    if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
    val = _gdi_.new_FontFromNativeInfo(*args, **kwargs)
    return val

def FontFromNativeInfoString(*args, **kwargs):
    """
    FontFromNativeInfoString(String info) -> Font

    Construct a `wx.Font` from the string representation of a
    `wx.NativeFontInfo` object.
    """
    if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
    val = _gdi_.new_FontFromNativeInfoString(*args, **kwargs)
    return val

def FFont(*args, **kwargs):
    """
    FFont(int pointSize, int family, int flags=FONTFLAG_DEFAULT, 
        String face=EmptyString, int encoding=FONTENCODING_DEFAULT) -> Font

    A bit of a simpler way to create a `wx.Font` using flags instead of
    individual attribute settings.  The value of flags can be a
    combination of the following:

        ============================  ==
        wx.FONTFLAG_DEFAULT
        wx.FONTFLAG_ITALIC
        wx.FONTFLAG_SLANT
        wx.FONTFLAG_LIGHT
        wx.FONTFLAG_BOLD
        wx.FONTFLAG_ANTIALIASED
        wx.FONTFLAG_NOT_ANTIALIASED
        wx.FONTFLAG_UNDERLINED
        wx.FONTFLAG_STRIKETHROUGH
        ============================  ==

    :see: `wx.Font.__init__`
    """
    if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
    val = _gdi_.new_FFont(*args, **kwargs)
    return val

def FontFromPixelSize(*args, **kwargs):
    """
    FontFromPixelSize(Size pixelSize, int family, int style, int weight, 
        bool underlined=False, String face=wxEmptyString, 
        int encoding=FONTENCODING_DEFAULT) -> Font

    Creates a font using a size in pixels rather than points.  If there is
    platform API support for this then it is used, otherwise a font with
    the closest size is found using a binary search.

    :see: `wx.Font.__init__`
    """
    if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
    val = _gdi_.new_FontFromPixelSize(*args, **kwargs)
    return val

def FFontFromPixelSize(*args, **kwargs):
    """
    FFontFromPixelSize(Size pixelSize, int family, int flags=FONTFLAG_DEFAULT, 
        String face=wxEmptyString, int encoding=FONTENCODING_DEFAULT) -> Font

    Creates a font using a size in pixels rather than points.  If there is
    platform API support for this then it is used, otherwise a font with
    the closest size is found using a binary search.

    :see: `wx.Font.__init__`, `wx.FFont`
    """
    if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']
    val = _gdi_.new_FFontFromPixelSize(*args, **kwargs)
    return val

def Font_GetDefaultEncoding(*args):
  """
    Font_GetDefaultEncoding() -> int

    Returns the encoding used for all fonts created with an encoding of
    ``wx.FONTENCODING_DEFAULT``.
    """
  return _gdi_.Font_GetDefaultEncoding(*args)

def Font_SetDefaultEncoding(*args, **kwargs):
  """
    Font_SetDefaultEncoding(int encoding)

    Sets the default font encoding.
    """
  return _gdi_.Font_SetDefaultEncoding(*args, **kwargs)

Font2 = wx._deprecated(FFont, "Use `wx.FFont` instead.") 
#---------------------------------------------------------------------------

class FontEnumerator(object):
    """Proxy of C++ FontEnumerator class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> FontEnumerator"""
        _gdi_.FontEnumerator_swiginit(self,_gdi_.new_FontEnumerator(*args, **kwargs))
        self._setCallbackInfo(self, FontEnumerator, 0)

    __swig_destroy__ = _gdi_.delete_FontEnumerator
    __del__ = lambda self : None;
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
        """GetEncodings() -> PyObject"""
        return _gdi_.FontEnumerator_GetEncodings(*args, **kwargs)

    GetEncodings = staticmethod(GetEncodings)
    def GetFacenames(*args, **kwargs):
        """GetFacenames() -> PyObject"""
        return _gdi_.FontEnumerator_GetFacenames(*args, **kwargs)

    GetFacenames = staticmethod(GetFacenames)
    def IsValidFacename(*args, **kwargs):
        """
        IsValidFacename(String str) -> bool

        Convenience function that returns true if the given face name exist in
        the user's system
        """
        return _gdi_.FontEnumerator_IsValidFacename(*args, **kwargs)

    IsValidFacename = staticmethod(IsValidFacename)
_gdi_.FontEnumerator_swigregister(FontEnumerator)

def FontEnumerator_GetEncodings(*args):
  """FontEnumerator_GetEncodings() -> PyObject"""
  return _gdi_.FontEnumerator_GetEncodings(*args)

def FontEnumerator_GetFacenames(*args):
  """FontEnumerator_GetFacenames() -> PyObject"""
  return _gdi_.FontEnumerator_GetFacenames(*args)

def FontEnumerator_IsValidFacename(*args, **kwargs):
  """
    FontEnumerator_IsValidFacename(String str) -> bool

    Convenience function that returns true if the given face name exist in
    the user's system
    """
  return _gdi_.FontEnumerator_IsValidFacename(*args, **kwargs)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    Language = property(_gdi_.LanguageInfo_Language_get, _gdi_.LanguageInfo_Language_set)
    CanonicalName = property(_gdi_.LanguageInfo_CanonicalName_get, _gdi_.LanguageInfo_CanonicalName_set)
    Description = property(_gdi_.LanguageInfo_Description_get, _gdi_.LanguageInfo_Description_set)
_gdi_.LanguageInfo_swigregister(LanguageInfo)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int language=-1, int flags=wxLOCALE_LOAD_DEFAULT|wxLOCALE_CONV_ENCODING) -> Locale"""
        _gdi_.Locale_swiginit(self,_gdi_.new_Locale(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_Locale
    __del__ = lambda self : None;
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

_gdi_.Locale_swigregister(Locale)

def Locale_GetSystemLanguage(*args):
  """Locale_GetSystemLanguage() -> int"""
  return _gdi_.Locale_GetSystemLanguage(*args)

def Locale_GetSystemEncoding(*args):
  """Locale_GetSystemEncoding() -> int"""
  return _gdi_.Locale_GetSystemEncoding(*args)

def Locale_GetSystemEncodingName(*args):
  """Locale_GetSystemEncodingName() -> String"""
  return _gdi_.Locale_GetSystemEncodingName(*args)

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

class PyLocale(Locale):
    """Proxy of C++ PyLocale class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int language=-1, int flags=wxLOCALE_LOAD_DEFAULT|wxLOCALE_CONV_ENCODING) -> PyLocale"""
        _gdi_.PyLocale_swiginit(self,_gdi_.new_PyLocale(*args, **kwargs))
        self._setCallbackInfo(self, PyLocale)

    __swig_destroy__ = _gdi_.delete_PyLocale
    __del__ = lambda self : None;
    def _setCallbackInfo(*args, **kwargs):
        """_setCallbackInfo(self, PyObject self, PyObject _class)"""
        return _gdi_.PyLocale__setCallbackInfo(*args, **kwargs)

    def GetSingularString(*args, **kwargs):
        """GetSingularString(self, wxChar szOrigString, wxChar szDomain=None) -> wxChar"""
        return _gdi_.PyLocale_GetSingularString(*args, **kwargs)

    def GetPluralString(*args, **kwargs):
        """
        GetPluralString(self, wxChar szOrigString, wxChar szOrigString2, size_t n, 
            wxChar szDomain=None) -> wxChar
        """
        return _gdi_.PyLocale_GetPluralString(*args, **kwargs)

_gdi_.PyLocale_swigregister(PyLocale)


def GetLocale(*args):
  """GetLocale() -> Locale"""
  return _gdi_.GetLocale(*args)
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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> EncodingConverter"""
        _gdi_.EncodingConverter_swiginit(self,_gdi_.new_EncodingConverter(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_EncodingConverter
    __del__ = lambda self : None;
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
_gdi_.EncodingConverter_swigregister(EncodingConverter)

def GetTranslation(*args):
  """
    GetTranslation(String str) -> String
    GetTranslation(String str, String domain) -> String
    GetTranslation(String str, String strPlural, size_t n) -> String
    GetTranslation(String str, String strPlural, size_t n, String domain) -> String
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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    __swig_destroy__ = _gdi_.delete_DC
    __del__ = lambda self : None;
    # These have been deprecated in wxWidgets.  Since they never
    # really did anything to begin with, just make them be NOPs.
    def BeginDrawing(self):  pass
    def EndDrawing(self):  pass

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

    def GradientFillConcentric(*args, **kwargs):
        """
        GradientFillConcentric(self, Rect rect, Colour initialColour, Colour destColour, 
            Point circleCenter)

        Fill the area specified by rect with a radial gradient, starting from
        initialColour in the center of the circle and fading to destColour on
        the outside of the circle.  The circleCenter argument is the relative
        coordinants of the center of the circle in the specified rect.

        Note: Currently this function is very slow, don't use it for real-time
        drawing.
        """
        return _gdi_.DC_GradientFillConcentric(*args, **kwargs)

    def GradientFillLinear(*args, **kwargs):
        """
        GradientFillLinear(self, Rect rect, Colour initialColour, Colour destColour, 
            int nDirection=EAST)

        Fill the area specified by rect with a linear gradient, starting from
        initialColour and eventually fading to destColour. The nDirection
        parameter specifies the direction of the colour change, default is to
        use initialColour on the left part of the rectangle and destColour on
        the right side.
        """
        return _gdi_.DC_GradientFillLinear(*args, **kwargs)

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
           (width, height, lineHeight)

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
        than the generic implementation then it will be used instead.
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

        Resolution in pixels per inch
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
    def GetHDC(*args, **kwargs):
        """GetHDC(self) -> long"""
        return _gdi_.DC_GetHDC(*args, **kwargs)

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

_gdi_.DC_swigregister(DC)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> MemoryDC

        Constructs a new memory device context.

        Use the Ok member to test whether the constructor was successful in
        creating a usable device context. Don't forget to select a bitmap into
        the DC before drawing on it.
        """
        _gdi_.MemoryDC_swiginit(self,_gdi_.new_MemoryDC(*args, **kwargs))
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

_gdi_.MemoryDC_swigregister(MemoryDC)

def MemoryDCFromDC(*args, **kwargs):
    """
    MemoryDCFromDC(DC oldDC) -> MemoryDC

    Creates a DC that is compatible with the oldDC.
    """
    val = _gdi_.new_MemoryDCFromDC(*args, **kwargs)
    return val

#---------------------------------------------------------------------------

BUFFER_VIRTUAL_AREA = _gdi_.BUFFER_VIRTUAL_AREA
BUFFER_CLIENT_AREA = _gdi_.BUFFER_CLIENT_AREA
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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, DC dc, Bitmap buffer=NullBitmap, int style=BUFFER_CLIENT_AREA) -> BufferedDC
        __init__(self, DC dc, Size area, int style=BUFFER_CLIENT_AREA) -> BufferedDC

        Constructs a buffered DC.
        """
        _gdi_.BufferedDC_swiginit(self,_gdi_.new_BufferedDC(*args))
        self.__dc = args[0] # save a ref so the other dc will not be deleted before self

    __swig_destroy__ = _gdi_.delete_BufferedDC
    __del__ = lambda self : None;
    def UnMask(*args, **kwargs):
        """
        UnMask(self)

        Blits the buffer to the dc, and detaches the dc from the buffer (so it
        can be effectively used once only).  This is usually only called in
        the destructor.
        """
        return _gdi_.BufferedDC_UnMask(*args, **kwargs)

_gdi_.BufferedDC_swigregister(BufferedDC)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window window, Bitmap buffer=NullBitmap, int style=BUFFER_CLIENT_AREA) -> BufferedPaintDC

        Create a buffered paint DC.  As with `wx.BufferedDC`, you may either
        provide the bitmap to be used for buffering or let this object create
        one internally (in the latter case, the size of the client part of the
        window is automatically used).


        """
        _gdi_.BufferedPaintDC_swiginit(self,_gdi_.new_BufferedPaintDC(*args, **kwargs))
_gdi_.BufferedPaintDC_swigregister(BufferedPaintDC)

#---------------------------------------------------------------------------

class ScreenDC(DC):
    """
    A wxScreenDC can be used to paint anywhere on the screen. This should
    normally be constructed as a temporary stack object; don't store a
    wxScreenDC object.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> ScreenDC

        A wxScreenDC can be used to paint anywhere on the screen. This should
        normally be constructed as a temporary stack object; don't store a
        wxScreenDC object.

        """
        _gdi_.ScreenDC_swiginit(self,_gdi_.new_ScreenDC(*args, **kwargs))
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

_gdi_.ScreenDC_swigregister(ScreenDC)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window win) -> ClientDC

        Constructor. Pass the window on which you wish to paint.
        """
        _gdi_.ClientDC_swiginit(self,_gdi_.new_ClientDC(*args, **kwargs))
_gdi_.ClientDC_swigregister(ClientDC)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window win) -> PaintDC

        Constructor. Pass the window on which you wish to paint.
        """
        _gdi_.PaintDC_swiginit(self,_gdi_.new_PaintDC(*args, **kwargs))
_gdi_.PaintDC_swigregister(PaintDC)

#---------------------------------------------------------------------------

class WindowDC(DC):
    """
    A wx.WindowDC must be constructed if an application wishes to paint on
    the whole area of a window (client and decorations). This should
    normally be constructed as a temporary stack object; don't store a
    wx.WindowDC object.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, Window win) -> WindowDC

        Constructor. Pass the window on which you wish to paint.
        """
        _gdi_.WindowDC_swiginit(self,_gdi_.new_WindowDC(*args, **kwargs))
_gdi_.WindowDC_swigregister(WindowDC)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, DC dc, bool mirror) -> MirrorDC

        Creates a mirrored DC associated with the real *dc*.  Everything drawn
        on the wx.MirrorDC will appear on the *dc*, and will be mirrored if
        *mirror* is True.
        """
        _gdi_.MirrorDC_swiginit(self,_gdi_.new_MirrorDC(*args, **kwargs))
_gdi_.MirrorDC_swigregister(MirrorDC)

#---------------------------------------------------------------------------

class PostScriptDC(DC):
    """This is a `wx.DC` that can write to PostScript files on any platform."""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, wxPrintData printData) -> PostScriptDC

        Constructs a PostScript printer device context from a `wx.PrintData`
        object.
        """
        _gdi_.PostScriptDC_swiginit(self,_gdi_.new_PostScriptDC(*args, **kwargs))
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
_gdi_.PostScriptDC_swigregister(PostScriptDC)

def PostScriptDC_SetResolution(*args, **kwargs):
  """
    PostScriptDC_SetResolution(int ppi)

    Set resolution (in pixels per inch) that will be used in PostScript
    output. Default is 720ppi.
    """
  return _gdi_.PostScriptDC_SetResolution(*args, **kwargs)

def PostScriptDC_GetResolution(*args):
  """
    PostScriptDC_GetResolution() -> int

    Return resolution used in PostScript output.
    """
  return _gdi_.PostScriptDC_GetResolution(*args)

#---------------------------------------------------------------------------

class MetaFile(_core.Object):
    """Proxy of C++ MetaFile class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, String filename=EmptyString) -> MetaFile"""
        _gdi_.MetaFile_swiginit(self,_gdi_.new_MetaFile(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_MetaFile
    __del__ = lambda self : None;
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

    def GetFileName(*args, **kwargs):
        """GetFileName(self) -> String"""
        return _gdi_.MetaFile_GetFileName(*args, **kwargs)

    def __nonzero__(self): return self.Ok() 
_gdi_.MetaFile_swigregister(MetaFile)

class MetaFileDC(DC):
    """Proxy of C++ MetaFileDC class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, String filename=EmptyString, int width=0, int height=0, 
            String description=EmptyString) -> MetaFileDC
        """
        _gdi_.MetaFileDC_swiginit(self,_gdi_.new_MetaFileDC(*args, **kwargs))
    def Close(*args, **kwargs):
        """Close(self) -> MetaFile"""
        return _gdi_.MetaFileDC_Close(*args, **kwargs)

_gdi_.MetaFileDC_swigregister(MetaFileDC)

class PrinterDC(DC):
    """Proxy of C++ PrinterDC class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, wxPrintData printData) -> PrinterDC"""
        _gdi_.PrinterDC_swiginit(self,_gdi_.new_PrinterDC(*args, **kwargs))
_gdi_.PrinterDC_swigregister(PrinterDC)

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
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self, int width, int height, int mask=True, int initialCount=1) -> ImageList"""
        _gdi_.ImageList_swiginit(self,_gdi_.new_ImageList(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_ImageList
    __del__ = lambda self : None;
    def Add(*args, **kwargs):
        """Add(self, Bitmap bitmap, Bitmap mask=NullBitmap) -> int"""
        return _gdi_.ImageList_Add(*args, **kwargs)

    def AddWithColourMask(*args, **kwargs):
        """AddWithColourMask(self, Bitmap bitmap, Colour maskColour) -> int"""
        return _gdi_.ImageList_AddWithColourMask(*args, **kwargs)

    def AddIcon(*args, **kwargs):
        """AddIcon(self, Icon icon) -> int"""
        return _gdi_.ImageList_AddIcon(*args, **kwargs)

    def GetBitmap(*args, **kwargs):
        """GetBitmap(self, int index) -> Bitmap"""
        return _gdi_.ImageList_GetBitmap(*args, **kwargs)

    def GetIcon(*args, **kwargs):
        """GetIcon(self, int index) -> Icon"""
        return _gdi_.ImageList_GetIcon(*args, **kwargs)

    def Replace(*args, **kwargs):
        """Replace(self, int index, Bitmap bitmap, Bitmap mask=NullBitmap) -> bool"""
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
        """GetSize(index) -> (width,height)"""
        return _gdi_.ImageList_GetSize(*args, **kwargs)

_gdi_.ImageList_swigregister(ImageList)

#---------------------------------------------------------------------------

class StockGDI(object):
    """Proxy of C++ StockGDI class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    BRUSH_BLACK = _gdi_.StockGDI_BRUSH_BLACK
    BRUSH_BLUE = _gdi_.StockGDI_BRUSH_BLUE
    BRUSH_CYAN = _gdi_.StockGDI_BRUSH_CYAN
    BRUSH_GREEN = _gdi_.StockGDI_BRUSH_GREEN
    BRUSH_GREY = _gdi_.StockGDI_BRUSH_GREY
    BRUSH_LIGHTGREY = _gdi_.StockGDI_BRUSH_LIGHTGREY
    BRUSH_MEDIUMGREY = _gdi_.StockGDI_BRUSH_MEDIUMGREY
    BRUSH_RED = _gdi_.StockGDI_BRUSH_RED
    BRUSH_TRANSPARENT = _gdi_.StockGDI_BRUSH_TRANSPARENT
    BRUSH_WHITE = _gdi_.StockGDI_BRUSH_WHITE
    COLOUR_BLACK = _gdi_.StockGDI_COLOUR_BLACK
    COLOUR_BLUE = _gdi_.StockGDI_COLOUR_BLUE
    COLOUR_CYAN = _gdi_.StockGDI_COLOUR_CYAN
    COLOUR_GREEN = _gdi_.StockGDI_COLOUR_GREEN
    COLOUR_LIGHTGREY = _gdi_.StockGDI_COLOUR_LIGHTGREY
    COLOUR_RED = _gdi_.StockGDI_COLOUR_RED
    COLOUR_WHITE = _gdi_.StockGDI_COLOUR_WHITE
    CURSOR_CROSS = _gdi_.StockGDI_CURSOR_CROSS
    CURSOR_HOURGLASS = _gdi_.StockGDI_CURSOR_HOURGLASS
    CURSOR_STANDARD = _gdi_.StockGDI_CURSOR_STANDARD
    FONT_ITALIC = _gdi_.StockGDI_FONT_ITALIC
    FONT_NORMAL = _gdi_.StockGDI_FONT_NORMAL
    FONT_SMALL = _gdi_.StockGDI_FONT_SMALL
    FONT_SWISS = _gdi_.StockGDI_FONT_SWISS
    PEN_BLACK = _gdi_.StockGDI_PEN_BLACK
    PEN_BLACKDASHED = _gdi_.StockGDI_PEN_BLACKDASHED
    PEN_CYAN = _gdi_.StockGDI_PEN_CYAN
    PEN_GREEN = _gdi_.StockGDI_PEN_GREEN
    PEN_GREY = _gdi_.StockGDI_PEN_GREY
    PEN_LIGHTGREY = _gdi_.StockGDI_PEN_LIGHTGREY
    PEN_MEDIUMGREY = _gdi_.StockGDI_PEN_MEDIUMGREY
    PEN_RED = _gdi_.StockGDI_PEN_RED
    PEN_TRANSPARENT = _gdi_.StockGDI_PEN_TRANSPARENT
    PEN_WHITE = _gdi_.StockGDI_PEN_WHITE
    ITEMCOUNT = _gdi_.StockGDI_ITEMCOUNT
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> StockGDI"""
        _gdi_.StockGDI_swiginit(self,_gdi_.new_StockGDI(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_StockGDI
    __del__ = lambda self : None;
    def DeleteAll(*args, **kwargs):
        """DeleteAll()"""
        return _gdi_.StockGDI_DeleteAll(*args, **kwargs)

    DeleteAll = staticmethod(DeleteAll)
    def instance(*args, **kwargs):
        """instance() -> StockGDI"""
        return _gdi_.StockGDI_instance(*args, **kwargs)

    instance = staticmethod(instance)
    def GetBrush(*args, **kwargs):
        """GetBrush(int item) -> Brush"""
        return _gdi_.StockGDI_GetBrush(*args, **kwargs)

    GetBrush = staticmethod(GetBrush)
    def GetColour(*args, **kwargs):
        """GetColour(int item) -> Colour"""
        return _gdi_.StockGDI_GetColour(*args, **kwargs)

    GetColour = staticmethod(GetColour)
    def GetCursor(*args, **kwargs):
        """GetCursor(int item) -> Cursor"""
        return _gdi_.StockGDI_GetCursor(*args, **kwargs)

    GetCursor = staticmethod(GetCursor)
    def GetPen(*args, **kwargs):
        """GetPen(int item) -> Pen"""
        return _gdi_.StockGDI_GetPen(*args, **kwargs)

    GetPen = staticmethod(GetPen)
    def GetFont(*args, **kwargs):
        """GetFont(self, int item) -> Font"""
        return _gdi_.StockGDI_GetFont(*args, **kwargs)

    def _initStockObjects():
        import wx
        wx.ITALIC_FONT.this  = StockGDI.instance().GetFont(StockGDI.FONT_ITALIC).this
        wx.NORMAL_FONT.this  = StockGDI.instance().GetFont(StockGDI.FONT_NORMAL).this
        wx.SMALL_FONT.this   = StockGDI.instance().GetFont(StockGDI.FONT_SMALL).this
        wx.SWISS_FONT.this   = StockGDI.instance().GetFont(StockGDI.FONT_SWISS).this
                                              
        wx.BLACK_DASHED_PEN.this  = StockGDI.GetPen(StockGDI.PEN_BLACKDASHED).this
        wx.BLACK_PEN.this         = StockGDI.GetPen(StockGDI.PEN_BLACK).this
        wx.CYAN_PEN.this          = StockGDI.GetPen(StockGDI.PEN_CYAN).this
        wx.GREEN_PEN.this         = StockGDI.GetPen(StockGDI.PEN_GREEN).this
        wx.GREY_PEN.this          = StockGDI.GetPen(StockGDI.PEN_GREY).this
        wx.LIGHT_GREY_PEN.this    = StockGDI.GetPen(StockGDI.PEN_LIGHTGREY).this
        wx.MEDIUM_GREY_PEN.this   = StockGDI.GetPen(StockGDI.PEN_MEDIUMGREY).this
        wx.RED_PEN.this           = StockGDI.GetPen(StockGDI.PEN_RED).this
        wx.TRANSPARENT_PEN.this   = StockGDI.GetPen(StockGDI.PEN_TRANSPARENT).this
        wx.WHITE_PEN.this         = StockGDI.GetPen(StockGDI.PEN_WHITE).this

        wx.BLACK_BRUSH.this        = StockGDI.GetBrush(StockGDI.BRUSH_BLACK).this
        wx.BLUE_BRUSH.this         = StockGDI.GetBrush(StockGDI.BRUSH_BLUE).this
        wx.CYAN_BRUSH.this         = StockGDI.GetBrush(StockGDI.BRUSH_CYAN).this
        wx.GREEN_BRUSH.this        = StockGDI.GetBrush(StockGDI.BRUSH_GREEN).this
        wx.GREY_BRUSH.this         = StockGDI.GetBrush(StockGDI.BRUSH_GREY).this
        wx.LIGHT_GREY_BRUSH.this   = StockGDI.GetBrush(StockGDI.BRUSH_LIGHTGREY).this
        wx.MEDIUM_GREY_BRUSH.this  = StockGDI.GetBrush(StockGDI.BRUSH_MEDIUMGREY).this
        wx.RED_BRUSH.this          = StockGDI.GetBrush(StockGDI.BRUSH_RED).this
        wx.TRANSPARENT_BRUSH.this  = StockGDI.GetBrush(StockGDI.BRUSH_TRANSPARENT).this
        wx.WHITE_BRUSH.this        = StockGDI.GetBrush(StockGDI.BRUSH_WHITE).this

        wx.BLACK.this       = StockGDI.GetColour(StockGDI.COLOUR_BLACK).this
        wx.BLUE.this        = StockGDI.GetColour(StockGDI.COLOUR_BLUE).this
        wx.CYAN.this        = StockGDI.GetColour(StockGDI.COLOUR_CYAN).this
        wx.GREEN.this       = StockGDI.GetColour(StockGDI.COLOUR_GREEN).this
        wx.LIGHT_GREY.this  = StockGDI.GetColour(StockGDI.COLOUR_LIGHTGREY).this
        wx.RED.this         = StockGDI.GetColour(StockGDI.COLOUR_RED).this
        wx.WHITE.this       = StockGDI.GetColour(StockGDI.COLOUR_WHITE).this

        wx.CROSS_CURSOR.this      = StockGDI.GetCursor(StockGDI.CURSOR_CROSS).this
        wx.HOURGLASS_CURSOR.this  = StockGDI.GetCursor(StockGDI.CURSOR_HOURGLASS).this
        wx.STANDARD_CURSOR.this   = StockGDI.GetCursor(StockGDI.CURSOR_STANDARD).this

        wx.TheFontList.this       = _wxPyInitTheFontList().this
        wx.ThePenList.this        = _wxPyInitThePenList().this
        wx.TheBrushList.this      = _wxPyInitTheBrushList().this
        wx.TheColourDatabase.this = _wxPyInitTheColourDatabase().this

        
    _initStockObjects = staticmethod(_initStockObjects)

_gdi_.StockGDI_swigregister(StockGDI)

def StockGDI_DeleteAll(*args):
  """StockGDI_DeleteAll()"""
  return _gdi_.StockGDI_DeleteAll(*args)

def StockGDI_instance(*args):
  """StockGDI_instance() -> StockGDI"""
  return _gdi_.StockGDI_instance(*args)

def StockGDI_GetBrush(*args, **kwargs):
  """StockGDI_GetBrush(int item) -> Brush"""
  return _gdi_.StockGDI_GetBrush(*args, **kwargs)

def StockGDI_GetColour(*args, **kwargs):
  """StockGDI_GetColour(int item) -> Colour"""
  return _gdi_.StockGDI_GetColour(*args, **kwargs)

def StockGDI_GetCursor(*args, **kwargs):
  """StockGDI_GetCursor(int item) -> Cursor"""
  return _gdi_.StockGDI_GetCursor(*args, **kwargs)

def StockGDI_GetPen(*args, **kwargs):
  """StockGDI_GetPen(int item) -> Pen"""
  return _gdi_.StockGDI_GetPen(*args, **kwargs)

# Create an uninitialized instance for the stock objects, they will
# be initialized later when the wx.App object is created.
ITALIC_FONT  = Font.__new__(Font)
NORMAL_FONT  = Font.__new__(Font)
SMALL_FONT   = Font.__new__(Font)
SWISS_FONT   = Font.__new__(Font)
                                   
BLACK_DASHED_PEN  = Pen.__new__(Pen)
BLACK_PEN         = Pen.__new__(Pen)
CYAN_PEN          = Pen.__new__(Pen)
GREEN_PEN         = Pen.__new__(Pen)
GREY_PEN          = Pen.__new__(Pen)
LIGHT_GREY_PEN    = Pen.__new__(Pen)
MEDIUM_GREY_PEN   = Pen.__new__(Pen)
RED_PEN           = Pen.__new__(Pen)
TRANSPARENT_PEN   = Pen.__new__(Pen)
WHITE_PEN         = Pen.__new__(Pen)

BLACK_BRUSH        = Brush.__new__(Brush)
BLUE_BRUSH         = Brush.__new__(Brush)
CYAN_BRUSH         = Brush.__new__(Brush)
GREEN_BRUSH        = Brush.__new__(Brush)
GREY_BRUSH         = Brush.__new__(Brush)
LIGHT_GREY_BRUSH   = Brush.__new__(Brush)
MEDIUM_GREY_BRUSH  = Brush.__new__(Brush)
RED_BRUSH          = Brush.__new__(Brush)
TRANSPARENT_BRUSH  = Brush.__new__(Brush)
WHITE_BRUSH        = Brush.__new__(Brush)

BLACK       = Colour.__new__(Colour)
BLUE        = Colour.__new__(Colour)
CYAN        = Colour.__new__(Colour)
GREEN       = Colour.__new__(Colour)
LIGHT_GREY  = Colour.__new__(Colour)
RED         = Colour.__new__(Colour)
WHITE       = Colour.__new__(Colour)

CROSS_CURSOR      = Cursor.__new__(Cursor)
HOURGLASS_CURSOR  = Cursor.__new__(Cursor)
STANDARD_CURSOR   = Cursor.__new__(Cursor)

class GDIObjListBase(object):
    """Proxy of C++ GDIObjListBase class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> GDIObjListBase"""
        _gdi_.GDIObjListBase_swiginit(self,_gdi_.new_GDIObjListBase(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_GDIObjListBase
    __del__ = lambda self : None;
_gdi_.GDIObjListBase_swigregister(GDIObjListBase)
cvar = _gdi_.cvar
NullBitmap = cvar.NullBitmap
NullIcon = cvar.NullIcon
NullCursor = cvar.NullCursor
NullPen = cvar.NullPen
NullBrush = cvar.NullBrush
NullPalette = cvar.NullPalette
NullFont = cvar.NullFont
NullColour = cvar.NullColour

class PenList(GDIObjListBase):
    """Proxy of C++ PenList class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def FindOrCreatePen(*args, **kwargs):
        """FindOrCreatePen(self, Colour colour, int width, int style) -> Pen"""
        return _gdi_.PenList_FindOrCreatePen(*args, **kwargs)

    def AddPen(*args, **kwargs):
        """AddPen(self, Pen pen)"""
        return _gdi_.PenList_AddPen(*args, **kwargs)

    def RemovePen(*args, **kwargs):
        """RemovePen(self, Pen pen)"""
        return _gdi_.PenList_RemovePen(*args, **kwargs)

    AddPen = wx._deprecated(AddPen)
    RemovePen = wx._deprecated(RemovePen)

_gdi_.PenList_swigregister(PenList)

class BrushList(GDIObjListBase):
    """Proxy of C++ BrushList class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def FindOrCreateBrush(*args, **kwargs):
        """FindOrCreateBrush(self, Colour colour, int style=SOLID) -> Brush"""
        return _gdi_.BrushList_FindOrCreateBrush(*args, **kwargs)

    def AddBrush(*args, **kwargs):
        """AddBrush(self, Brush brush)"""
        return _gdi_.BrushList_AddBrush(*args, **kwargs)

    def RemoveBrush(*args, **kwargs):
        """RemoveBrush(self, Brush brush)"""
        return _gdi_.BrushList_RemoveBrush(*args, **kwargs)

    AddBrush = wx._deprecated(AddBrush)
    RemoveBrush = wx._deprecated(RemoveBrush)

_gdi_.BrushList_swigregister(BrushList)

class FontList(GDIObjListBase):
    """Proxy of C++ FontList class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def FindOrCreateFont(*args, **kwargs):
        """
        FindOrCreateFont(self, int point_size, int family, int style, int weight, 
            bool underline=False, String facename=EmptyString, 
            int encoding=FONTENCODING_DEFAULT) -> Font
        """
        return _gdi_.FontList_FindOrCreateFont(*args, **kwargs)

    def AddFont(*args, **kwargs):
        """AddFont(self, Font font)"""
        return _gdi_.FontList_AddFont(*args, **kwargs)

    def RemoveFont(*args, **kwargs):
        """RemoveFont(self, Font font)"""
        return _gdi_.FontList_RemoveFont(*args, **kwargs)

    AddFont = wx._deprecated(AddFont)
    RemoveFont = wx._deprecated(RemoveFont)

_gdi_.FontList_swigregister(FontList)

class ColourDatabase(object):
    """Proxy of C++ ColourDatabase class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> ColourDatabase"""
        _gdi_.ColourDatabase_swiginit(self,_gdi_.new_ColourDatabase(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_ColourDatabase
    __del__ = lambda self : None;
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

_gdi_.ColourDatabase_swigregister(ColourDatabase)

#---------------------------------------------------------------------------


def _wxPyInitTheFontList(*args):
  """_wxPyInitTheFontList() -> FontList"""
  return _gdi_._wxPyInitTheFontList(*args)

def _wxPyInitThePenList(*args):
  """_wxPyInitThePenList() -> PenList"""
  return _gdi_._wxPyInitThePenList(*args)

def _wxPyInitTheBrushList(*args):
  """_wxPyInitTheBrushList() -> BrushList"""
  return _gdi_._wxPyInitTheBrushList(*args)

def _wxPyInitTheColourDatabase(*args):
  """_wxPyInitTheColourDatabase() -> ColourDatabase"""
  return _gdi_._wxPyInitTheColourDatabase(*args)
# Create an uninitialized instance for the stock objects, they will
# be initialized later when the wx.App object is created.
TheFontList       = FontList.__new__(FontList)
ThePenList        = PenList.__new__(PenList)
TheBrushList      = BrushList.__new__(BrushList)
TheColourDatabase = ColourDatabase.__new__(ColourDatabase)

NullColor = NullColour 
#---------------------------------------------------------------------------

class Effects(_core.Object):
    """Proxy of C++ Effects class"""
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """__init__(self) -> Effects"""
        _gdi_.Effects_swiginit(self,_gdi_.new_Effects(*args, **kwargs))
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

_gdi_.Effects_swigregister(Effects)

#---------------------------------------------------------------------------

CONTROL_DISABLED = _gdi_.CONTROL_DISABLED
CONTROL_FOCUSED = _gdi_.CONTROL_FOCUSED
CONTROL_PRESSED = _gdi_.CONTROL_PRESSED
CONTROL_ISDEFAULT = _gdi_.CONTROL_ISDEFAULT
CONTROL_ISSUBMENU = _gdi_.CONTROL_ISSUBMENU
CONTROL_EXPANDED = _gdi_.CONTROL_EXPANDED
CONTROL_CURRENT = _gdi_.CONTROL_CURRENT
CONTROL_SELECTED = _gdi_.CONTROL_SELECTED
CONTROL_CHECKED = _gdi_.CONTROL_CHECKED
CONTROL_CHECKABLE = _gdi_.CONTROL_CHECKABLE
CONTROL_UNDETERMINED = _gdi_.CONTROL_UNDETERMINED
CONTROL_FLAGS_MASK = _gdi_.CONTROL_FLAGS_MASK
CONTROL_DIRTY = _gdi_.CONTROL_DIRTY
class SplitterRenderParams(object):
    """
    This is just a simple struct used as a return value of
    `wx.RendererNative.GetSplitterParams` and contains some platform
    specific metrics about splitters.

        * widthSash: the width of the splitter sash.
        * border: the width of the border of the splitter window.
        * isHotSensitive: ``True`` if the splitter changes its
          appearance when the mouse is over it.


    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int widthSash_, int border_, bool isSens_) -> SplitterRenderParams

        This is just a simple struct used as a return value of
        `wx.RendererNative.GetSplitterParams` and contains some platform
        specific metrics about splitters.

            * widthSash: the width of the splitter sash.
            * border: the width of the border of the splitter window.
            * isHotSensitive: ``True`` if the splitter changes its
              appearance when the mouse is over it.


        """
        _gdi_.SplitterRenderParams_swiginit(self,_gdi_.new_SplitterRenderParams(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_SplitterRenderParams
    __del__ = lambda self : None;
    widthSash = property(_gdi_.SplitterRenderParams_widthSash_get)
    border = property(_gdi_.SplitterRenderParams_border_get)
    isHotSensitive = property(_gdi_.SplitterRenderParams_isHotSensitive_get)
_gdi_.SplitterRenderParams_swigregister(SplitterRenderParams)

class RendererVersion(object):
    """
    This simple struct represents the `wx.RendererNative` interface
    version and is only used as the return value of
    `wx.RendererNative.GetVersion`.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self, int version_, int age_) -> RendererVersion

        This simple struct represents the `wx.RendererNative` interface
        version and is only used as the return value of
        `wx.RendererNative.GetVersion`.
        """
        _gdi_.RendererVersion_swiginit(self,_gdi_.new_RendererVersion(*args, **kwargs))
    __swig_destroy__ = _gdi_.delete_RendererVersion
    __del__ = lambda self : None;
    Current_Version = _gdi_.RendererVersion_Current_Version
    Current_Age = _gdi_.RendererVersion_Current_Age
    def IsCompatible(*args, **kwargs):
        """IsCompatible(RendererVersion ver) -> bool"""
        return _gdi_.RendererVersion_IsCompatible(*args, **kwargs)

    IsCompatible = staticmethod(IsCompatible)
    version = property(_gdi_.RendererVersion_version_get)
    age = property(_gdi_.RendererVersion_age_get)
_gdi_.RendererVersion_swigregister(RendererVersion)

def RendererVersion_IsCompatible(*args, **kwargs):
  """RendererVersion_IsCompatible(RendererVersion ver) -> bool"""
  return _gdi_.RendererVersion_IsCompatible(*args, **kwargs)

class RendererNative(object):
    """
    One of the design principles of wxWidgets is to use the native widgets
    on every platform in order to be as close to the native look and feel
    on every platform.  However there are still cases when some generic
    widgets are needed for various reasons, but it can sometimes take a
    lot of messy work to make them conform to the native LnF.

    The wx.RendererNative class is a collection of functions that have
    platform-specific implementations for drawing certain parts of
    genereic controls in ways that are as close to the native look as
    possible.

    Note that each drawing function restores the `wx.DC` attributes if it
    changes them, so it is safe to assume that the same pen, brush and
    colours that were active before the call to this function are still in
    effect after it.

    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    def __init__(self): raise AttributeError, "No constructor defined"
    __repr__ = _swig_repr
    def DrawHeaderButton(*args, **kwargs):
        """
        DrawHeaderButton(self, Window win, DC dc, Rect rect, int flags=0)

        Draw the header control button (such as what is used by `wx.ListCtrl`
        in report mode.)
        """
        return _gdi_.RendererNative_DrawHeaderButton(*args, **kwargs)

    def DrawTreeItemButton(*args, **kwargs):
        """
        DrawTreeItemButton(self, Window win, DC dc, Rect rect, int flags=0)

        Draw the expanded/collapsed icon for a tree control item.
        """
        return _gdi_.RendererNative_DrawTreeItemButton(*args, **kwargs)

    def DrawSplitterBorder(*args, **kwargs):
        """
        DrawSplitterBorder(self, Window win, DC dc, Rect rect, int flags=0)

        Draw the border for a sash window: this border must be such that the
        sash drawn by `DrawSplitterSash` blends into it well.
        """
        return _gdi_.RendererNative_DrawSplitterBorder(*args, **kwargs)

    def DrawSplitterSash(*args, **kwargs):
        """
        DrawSplitterSash(self, Window win, DC dc, Size size, int position, int orient, 
            int flags=0)

        Draw a sash. The orient parameter defines whether the sash should be
        vertical or horizontal and how the position should be interpreted.
        """
        return _gdi_.RendererNative_DrawSplitterSash(*args, **kwargs)

    def DrawComboBoxDropButton(*args, **kwargs):
        """
        DrawComboBoxDropButton(self, Window win, DC dc, Rect rect, int flags=0)

        Draw a button like the one used by `wx.ComboBox` to show a drop down
        window. The usual appearance is a downwards pointing arrow.

        The ``flags`` parameter may have the ``wx.CONTROL_PRESSED`` or
        ``wx.CONTROL_CURRENT`` bits set.
        """
        return _gdi_.RendererNative_DrawComboBoxDropButton(*args, **kwargs)

    def DrawDropArrow(*args, **kwargs):
        """
        DrawDropArrow(self, Window win, DC dc, Rect rect, int flags=0)

        Draw a drop down arrow that is suitable for use outside a combo
        box. Arrow will have a transparent background.

        ``rect`` is not entirely filled by the arrow. Instead, you should use
        bounding rectangle of a drop down button which arrow matches the size
        you need. ``flags`` may have the ``wx.CONTROL_PRESSED`` or
        ``wx.CONTROL_CURRENT`` bit set.
        """
        return _gdi_.RendererNative_DrawDropArrow(*args, **kwargs)

    def DrawCheckBox(*args, **kwargs):
        """
        DrawCheckBox(self, Window win, DC dc, Rect rect, int flags=0)

        Draw a check button.  Flags may use wx.CONTROL_CHECKED,
        wx.CONTROL_UNDETERMINED and wx.CONTROL_CURRENT.
        """
        return _gdi_.RendererNative_DrawCheckBox(*args, **kwargs)

    def DrawPushButton(*args, **kwargs):
        """
        DrawPushButton(self, Window win, DC dc, Rect rect, int flags=0)

        Draw a blank button.  Flags may be wx.CONTROL_PRESSED, wx.CONTROL_CURRENT and
        wx.CONTROL_ISDEFAULT
        """
        return _gdi_.RendererNative_DrawPushButton(*args, **kwargs)

    def DrawItemSelectionRect(*args, **kwargs):
        """
        DrawItemSelectionRect(self, Window win, DC dc, Rect rect, int flags=0)

        Draw rectangle indicating that an item in e.g. a list control has been
        selected or focused

        The flags parameter may be:

            ====================  ============================================
            wx.CONTROL_SELECTED   item is selected, e.g. draw background
            wx.CONTROL_CURRENT    item is the current item, e.g. dotted border
            wx.CONTROL_FOCUSED    the whole control has focus, e.g. blue
                                  background vs. grey otherwise
            ====================  ============================================

        """
        return _gdi_.RendererNative_DrawItemSelectionRect(*args, **kwargs)

    def GetSplitterParams(*args, **kwargs):
        """
        GetSplitterParams(self, Window win) -> SplitterRenderParams

        Get the splitter parameters, see `wx.SplitterRenderParams`.
        """
        return _gdi_.RendererNative_GetSplitterParams(*args, **kwargs)

    def Get(*args, **kwargs):
        """
        Get() -> RendererNative

        Return the currently used renderer
        """
        return _gdi_.RendererNative_Get(*args, **kwargs)

    Get = staticmethod(Get)
    def GetGeneric(*args, **kwargs):
        """
        GetGeneric() -> RendererNative

        Return the generic implementation of the renderer. Under some
        platforms, this is the default renderer implementation, others have
        platform-specific default renderer which can be retrieved by calling
        `wx.RendererNative.GetDefault`.
        """
        return _gdi_.RendererNative_GetGeneric(*args, **kwargs)

    GetGeneric = staticmethod(GetGeneric)
    def GetDefault(*args, **kwargs):
        """
        GetDefault() -> RendererNative

        Return the default (native) implementation for this platform -- this
        is also the one used by default but this may be changed by calling
        `wx.RendererNative.Set` in which case the return value of this method
        may be different from the return value of `wx.RendererNative.Get`.
        """
        return _gdi_.RendererNative_GetDefault(*args, **kwargs)

    GetDefault = staticmethod(GetDefault)
    def Set(*args, **kwargs):
        """
        Set(RendererNative renderer) -> RendererNative

        Set the renderer to use, passing None reverts to using the default
        renderer.  Returns the previous renderer used with Set or None.
        """
        return _gdi_.RendererNative_Set(*args, **kwargs)

    Set = staticmethod(Set)
    def GetVersion(*args, **kwargs):
        """
        GetVersion(self) -> RendererVersion

        Returns the version of the renderer.  Will be used for ensuring
        compatibility of dynamically loaded renderers.
        """
        return _gdi_.RendererNative_GetVersion(*args, **kwargs)

_gdi_.RendererNative_swigregister(RendererNative)

def RendererNative_Get(*args):
  """
    RendererNative_Get() -> RendererNative

    Return the currently used renderer
    """
  return _gdi_.RendererNative_Get(*args)

def RendererNative_GetGeneric(*args):
  """
    RendererNative_GetGeneric() -> RendererNative

    Return the generic implementation of the renderer. Under some
    platforms, this is the default renderer implementation, others have
    platform-specific default renderer which can be retrieved by calling
    `wx.RendererNative.GetDefault`.
    """
  return _gdi_.RendererNative_GetGeneric(*args)

def RendererNative_GetDefault(*args):
  """
    RendererNative_GetDefault() -> RendererNative

    Return the default (native) implementation for this platform -- this
    is also the one used by default but this may be changed by calling
    `wx.RendererNative.Set` in which case the return value of this method
    may be different from the return value of `wx.RendererNative.Get`.
    """
  return _gdi_.RendererNative_GetDefault(*args)

def RendererNative_Set(*args, **kwargs):
  """
    RendererNative_Set(RendererNative renderer) -> RendererNative

    Set the renderer to use, passing None reverts to using the default
    renderer.  Returns the previous renderer used with Set or None.
    """
  return _gdi_.RendererNative_Set(*args, **kwargs)

#---------------------------------------------------------------------------

class PseudoDC(_core.Object):
    """
    A PseudoDC is an object that can be used as if it were a `wx.DC`.  All
    commands issued to the PseudoDC are stored in a list.  You can then
    play these commands back to a real DC object using the DrawToDC
    method.  Commands in the command list are indexed by ID.  You can use
    this to clear the operations associated with a single ID and then
    re-draw the object associated with that ID.
    """
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc='The membership flag')
    __repr__ = _swig_repr
    def __init__(self, *args, **kwargs): 
        """
        __init__(self) -> PseudoDC

        Constructs a new Pseudo device context for recording dc operations
        """
        _gdi_.PseudoDC_swiginit(self,_gdi_.new_PseudoDC(*args, **kwargs))
    def BeginDrawing(*args, **kwargs):
        """
        BeginDrawing(self)

        Allows for optimization of drawing code on platforms that need it.  On
        other platforms this is just an empty function and is harmless.  To
        take advantage of this postential optimization simply enclose each
        group of calls to the drawing primitives within calls to
        `BeginDrawing` and `EndDrawing`.
        """
        return _gdi_.PseudoDC_BeginDrawing(*args, **kwargs)

    def EndDrawing(*args, **kwargs):
        """
        EndDrawing(self)

        Ends the group of drawing primitives started with `BeginDrawing`, and
        invokes whatever optimization is available for this DC type on the
        current platform.
        """
        return _gdi_.PseudoDC_EndDrawing(*args, **kwargs)

    __swig_destroy__ = _gdi_.delete_PseudoDC
    __del__ = lambda self : None;
    def RemoveAll(*args, **kwargs):
        """
        RemoveAll(self)

        Removes all objects and operations from the recorded list.
        """
        return _gdi_.PseudoDC_RemoveAll(*args, **kwargs)

    def GetLen(*args, **kwargs):
        """
        GetLen(self) -> int

        Returns the number of operations in the recorded list.
        """
        return _gdi_.PseudoDC_GetLen(*args, **kwargs)

    def SetId(*args, **kwargs):
        """
        SetId(self, int id)

        Sets the id to be associated with subsequent operations.
        """
        return _gdi_.PseudoDC_SetId(*args, **kwargs)

    def ClearId(*args, **kwargs):
        """
        ClearId(self, int id)

        Removes all operations associated with id so the object can be redrawn.
        """
        return _gdi_.PseudoDC_ClearId(*args, **kwargs)

    def RemoveId(*args, **kwargs):
        """
        RemoveId(self, int id)

        Remove the object node (and all operations) associated with an id.
        """
        return _gdi_.PseudoDC_RemoveId(*args, **kwargs)

    def TranslateId(*args, **kwargs):
        """
        TranslateId(self, int id, int dx, int dy)

        Translate the operations of id by dx,dy.
        """
        return _gdi_.PseudoDC_TranslateId(*args, **kwargs)

    def DrawIdToDC(*args, **kwargs):
        """
        DrawIdToDC(self, int id, DC dc)

        Draw recorded operations of id to dc.
        """
        return _gdi_.PseudoDC_DrawIdToDC(*args, **kwargs)

    def SetIdBounds(*args, **kwargs):
        """
        SetIdBounds(self, int id, Rect rect)

        Set the bounding rect of a given object.  This will create 
        an object node if one doesn't exist.
        """
        return _gdi_.PseudoDC_SetIdBounds(*args, **kwargs)

    def GetIdBounds(*args, **kwargs):
        """
        GetIdBounds(self, int id) -> Rect

        Returns the bounding rectangle previouly set with SetIdBounds.  If
        no bounds have been set, it returns wx.Rect(0,0,0,0).
        """
        return _gdi_.PseudoDC_GetIdBounds(*args, **kwargs)

    def DrawToDCClipped(*args, **kwargs):
        """
        DrawToDCClipped(self, DC dc, Rect rect)

        Draws the recorded operations to dc unless the operation is known to
        be outside rect.
        """
        return _gdi_.PseudoDC_DrawToDCClipped(*args, **kwargs)

    def DrawToDCClippedRgn(*args, **kwargs):
        """
        DrawToDCClippedRgn(self, DC dc, Region region)

        Draws the recorded operations to dc unless the operation is known to
        be outside rect.
        """
        return _gdi_.PseudoDC_DrawToDCClippedRgn(*args, **kwargs)

    def DrawToDC(*args, **kwargs):
        """
        DrawToDC(self, DC dc)

        Draws the recorded operations to dc.
        """
        return _gdi_.PseudoDC_DrawToDC(*args, **kwargs)

    def FloodFill(*args, **kwargs):
        """
        FloodFill(self, int x, int y, Colour col, int style=FLOOD_SURFACE)

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
        return _gdi_.PseudoDC_FloodFill(*args, **kwargs)

    def FloodFillPoint(*args, **kwargs):
        """
        FloodFillPoint(self, Point pt, Colour col, int style=FLOOD_SURFACE)

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
        return _gdi_.PseudoDC_FloodFillPoint(*args, **kwargs)

    def DrawLine(*args, **kwargs):
        """
        DrawLine(self, int x1, int y1, int x2, int y2)

        Draws a line from the first point to the second. The current pen is
        used for drawing the line. Note that the second point is *not* part of
        the line and is not drawn by this function (this is consistent with
        the behaviour of many other toolkits).
        """
        return _gdi_.PseudoDC_DrawLine(*args, **kwargs)

    def DrawLinePoint(*args, **kwargs):
        """
        DrawLinePoint(self, Point pt1, Point pt2)

        Draws a line from the first point to the second. The current pen is
        used for drawing the line. Note that the second point is *not* part of
        the line and is not drawn by this function (this is consistent with
        the behaviour of many other toolkits).
        """
        return _gdi_.PseudoDC_DrawLinePoint(*args, **kwargs)

    def CrossHair(*args, **kwargs):
        """
        CrossHair(self, int x, int y)

        Displays a cross hair using the current pen. This is a vertical and
        horizontal line the height and width of the window, centred on the
        given point.
        """
        return _gdi_.PseudoDC_CrossHair(*args, **kwargs)

    def CrossHairPoint(*args, **kwargs):
        """
        CrossHairPoint(self, Point pt)

        Displays a cross hair using the current pen. This is a vertical and
        horizontal line the height and width of the window, centred on the
        given point.
        """
        return _gdi_.PseudoDC_CrossHairPoint(*args, **kwargs)

    def DrawArc(*args, **kwargs):
        """
        DrawArc(self, int x1, int y1, int x2, int y2, int xc, int yc)

        Draws an arc of a circle, centred on the *center* point (xc, yc), from
        the first point to the second. The current pen is used for the outline
        and the current brush for filling the shape.

        The arc is drawn in an anticlockwise direction from the start point to
        the end point.
        """
        return _gdi_.PseudoDC_DrawArc(*args, **kwargs)

    def DrawArcPoint(*args, **kwargs):
        """
        DrawArcPoint(self, Point pt1, Point pt2, Point center)

        Draws an arc of a circle, centred on the *center* point (xc, yc), from
        the first point to the second. The current pen is used for the outline
        and the current brush for filling the shape.

        The arc is drawn in an anticlockwise direction from the start point to
        the end point.
        """
        return _gdi_.PseudoDC_DrawArcPoint(*args, **kwargs)

    def DrawCheckMark(*args, **kwargs):
        """
        DrawCheckMark(self, int x, int y, int width, int height)

        Draws a check mark inside the given rectangle.
        """
        return _gdi_.PseudoDC_DrawCheckMark(*args, **kwargs)

    def DrawCheckMarkRect(*args, **kwargs):
        """
        DrawCheckMarkRect(self, Rect rect)

        Draws a check mark inside the given rectangle.
        """
        return _gdi_.PseudoDC_DrawCheckMarkRect(*args, **kwargs)

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
        return _gdi_.PseudoDC_DrawEllipticArc(*args, **kwargs)

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
        return _gdi_.PseudoDC_DrawEllipticArcPointSize(*args, **kwargs)

    def DrawPoint(*args, **kwargs):
        """
        DrawPoint(self, int x, int y)

        Draws a point using the current pen.
        """
        return _gdi_.PseudoDC_DrawPoint(*args, **kwargs)

    def DrawPointPoint(*args, **kwargs):
        """
        DrawPointPoint(self, Point pt)

        Draws a point using the current pen.
        """
        return _gdi_.PseudoDC_DrawPointPoint(*args, **kwargs)

    def DrawRectangle(*args, **kwargs):
        """
        DrawRectangle(self, int x, int y, int width, int height)

        Draws a rectangle with the given top left corner, and with the given
        size. The current pen is used for the outline and the current brush
        for filling the shape.
        """
        return _gdi_.PseudoDC_DrawRectangle(*args, **kwargs)

    def DrawRectangleRect(*args, **kwargs):
        """
        DrawRectangleRect(self, Rect rect)

        Draws a rectangle with the given top left corner, and with the given
        size. The current pen is used for the outline and the current brush
        for filling the shape.
        """
        return _gdi_.PseudoDC_DrawRectangleRect(*args, **kwargs)

    def DrawRectanglePointSize(*args, **kwargs):
        """
        DrawRectanglePointSize(self, Point pt, Size sz)

        Draws a rectangle with the given top left corner, and with the given
        size. The current pen is used for the outline and the current brush
        for filling the shape.
        """
        return _gdi_.PseudoDC_DrawRectanglePointSize(*args, **kwargs)

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
        return _gdi_.PseudoDC_DrawRoundedRectangle(*args, **kwargs)

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
        return _gdi_.PseudoDC_DrawRoundedRectangleRect(*args, **kwargs)

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
        return _gdi_.PseudoDC_DrawRoundedRectanglePointSize(*args, **kwargs)

    def DrawCircle(*args, **kwargs):
        """
        DrawCircle(self, int x, int y, int radius)

        Draws a circle with the given center point and radius.  The current
        pen is used for the outline and the current brush for filling the
        shape.
        """
        return _gdi_.PseudoDC_DrawCircle(*args, **kwargs)

    def DrawCirclePoint(*args, **kwargs):
        """
        DrawCirclePoint(self, Point pt, int radius)

        Draws a circle with the given center point and radius.  The current
        pen is used for the outline and the current brush for filling the
        shape.
        """
        return _gdi_.PseudoDC_DrawCirclePoint(*args, **kwargs)

    def DrawEllipse(*args, **kwargs):
        """
        DrawEllipse(self, int x, int y, int width, int height)

        Draws an ellipse contained in the specified rectangle. The current pen
        is used for the outline and the current brush for filling the shape.
        """
        return _gdi_.PseudoDC_DrawEllipse(*args, **kwargs)

    def DrawEllipseRect(*args, **kwargs):
        """
        DrawEllipseRect(self, Rect rect)

        Draws an ellipse contained in the specified rectangle. The current pen
        is used for the outline and the current brush for filling the shape.
        """
        return _gdi_.PseudoDC_DrawEllipseRect(*args, **kwargs)

    def DrawEllipsePointSize(*args, **kwargs):
        """
        DrawEllipsePointSize(self, Point pt, Size sz)

        Draws an ellipse contained in the specified rectangle. The current pen
        is used for the outline and the current brush for filling the shape.
        """
        return _gdi_.PseudoDC_DrawEllipsePointSize(*args, **kwargs)

    def DrawIcon(*args, **kwargs):
        """
        DrawIcon(self, Icon icon, int x, int y)

        Draw an icon on the display (does nothing if the device context is
        PostScript). This can be the simplest way of drawing bitmaps on a
        window.
        """
        return _gdi_.PseudoDC_DrawIcon(*args, **kwargs)

    def DrawIconPoint(*args, **kwargs):
        """
        DrawIconPoint(self, Icon icon, Point pt)

        Draw an icon on the display (does nothing if the device context is
        PostScript). This can be the simplest way of drawing bitmaps on a
        window.
        """
        return _gdi_.PseudoDC_DrawIconPoint(*args, **kwargs)

    def DrawBitmap(*args, **kwargs):
        """
        DrawBitmap(self, Bitmap bmp, int x, int y, bool useMask=False)

        Draw a bitmap on the device context at the specified point. If
        *transparent* is true and the bitmap has a transparency mask, (or
        alpha channel on the platforms that support it) then the bitmap will
        be drawn transparently.
        """
        return _gdi_.PseudoDC_DrawBitmap(*args, **kwargs)

    def DrawBitmapPoint(*args, **kwargs):
        """
        DrawBitmapPoint(self, Bitmap bmp, Point pt, bool useMask=False)

        Draw a bitmap on the device context at the specified point. If
        *transparent* is true and the bitmap has a transparency mask, (or
        alpha channel on the platforms that support it) then the bitmap will
        be drawn transparently.
        """
        return _gdi_.PseudoDC_DrawBitmapPoint(*args, **kwargs)

    def DrawText(*args, **kwargs):
        """
        DrawText(self, String text, int x, int y)

        Draws a text string at the specified point, using the current text
        font, and the current text foreground and background colours.

        The coordinates refer to the top-left corner of the rectangle bounding
        the string. See `wx.DC.GetTextExtent` for how to get the dimensions of
        a text string, which can be used to position the text more precisely,
        (you will need to use a real DC with GetTextExtent as wx.PseudoDC does
        not implement it.)

        **NOTE**: under wxGTK the current logical function is used by this
        function but it is ignored by wxMSW. Thus, you should avoid using
        logical functions with this function in portable programs.
        """
        return _gdi_.PseudoDC_DrawText(*args, **kwargs)

    def DrawTextPoint(*args, **kwargs):
        """
        DrawTextPoint(self, String text, Point pt)

        Draws a text string at the specified point, using the current text
        font, and the current text foreground and background colours.

        The coordinates refer to the top-left corner of the rectangle bounding
        the string. See `wx.DC.GetTextExtent` for how to get the dimensions of
        a text string, which can be used to position the text more precisely,
        (you will need to use a real DC with GetTextExtent as wx.PseudoDC does
        not implement it.)

        **NOTE**: under wxGTK the current logical function is used by this
        function but it is ignored by wxMSW. Thus, you should avoid using
        logical functions with this function in portable programs.
        """
        return _gdi_.PseudoDC_DrawTextPoint(*args, **kwargs)

    def DrawRotatedText(*args, **kwargs):
        """
        DrawRotatedText(self, String text, int x, int y, double angle)

        Draws the text rotated by *angle* degrees, if supported by the platform.

        **NOTE**: Under Win9x only TrueType fonts can be drawn by this
        function. In particular, a font different from ``wx.NORMAL_FONT``
        should be used as the it is not normally a TrueType
        font. ``wx.SWISS_FONT`` is an example of a font which is.
        """
        return _gdi_.PseudoDC_DrawRotatedText(*args, **kwargs)

    def DrawRotatedTextPoint(*args, **kwargs):
        """
        DrawRotatedTextPoint(self, String text, Point pt, double angle)

        Draws the text rotated by *angle* degrees, if supported by the platform.

        **NOTE**: Under Win9x only TrueType fonts can be drawn by this
        function. In particular, a font different from ``wx.NORMAL_FONT``
        should be used as the it is not normally a TrueType
        font. ``wx.SWISS_FONT`` is an example of a font which is.
        """
        return _gdi_.PseudoDC_DrawRotatedTextPoint(*args, **kwargs)

    def DrawLines(*args, **kwargs):
        """
        DrawLines(self, List points, int xoffset=0, int yoffset=0)

        Draws lines using a sequence of `wx.Point` objects, adding the
        optional offset coordinate. The current pen is used for drawing the
        lines.
        """
        return _gdi_.PseudoDC_DrawLines(*args, **kwargs)

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
        return _gdi_.PseudoDC_DrawPolygon(*args, **kwargs)

    def DrawLabel(*args, **kwargs):
        """
        DrawLabel(self, String text, Rect rect, int alignment=wxALIGN_LEFT|wxALIGN_TOP, 
            int indexAccel=-1)

        Draw *text* within the specified rectangle, abiding by the alignment
        flags.  Will additionally emphasize the character at *indexAccel* if
        it is not -1.
        """
        return _gdi_.PseudoDC_DrawLabel(*args, **kwargs)

    def DrawImageLabel(*args, **kwargs):
        """
        DrawImageLabel(self, String text, Bitmap image, Rect rect, int alignment=wxALIGN_LEFT|wxALIGN_TOP, 
            int indexAccel=-1)

        Draw *text* and an image (which may be ``wx.NullBitmap`` to skip
        drawing it) within the specified rectangle, abiding by the alignment
        flags.  Will additionally emphasize the character at *indexAccel* if
        it is not -1.
        """
        return _gdi_.PseudoDC_DrawImageLabel(*args, **kwargs)

    def DrawSpline(*args, **kwargs):
        """
        DrawSpline(self, List points)

        Draws a spline between all given control points, (a list of `wx.Point`
        objects) using the current pen. The spline is drawn using a series of
        lines, using an algorithm taken from the X drawing program 'XFIG'.
        """
        return _gdi_.PseudoDC_DrawSpline(*args, **kwargs)

    def Clear(*args, **kwargs):
        """
        Clear(self)

        Clears the device context using the current background brush.
        """
        return _gdi_.PseudoDC_Clear(*args, **kwargs)

    def SetFont(*args, **kwargs):
        """
        SetFont(self, Font font)

        Sets the current font for the DC. It must be a valid font, in
        particular you should not pass ``wx.NullFont`` to this method.
        """
        return _gdi_.PseudoDC_SetFont(*args, **kwargs)

    def SetPen(*args, **kwargs):
        """
        SetPen(self, Pen pen)

        Sets the current pen for the DC.

        If the argument is ``wx.NullPen``, the current pen is selected out of the
        device context, and the original pen restored.
        """
        return _gdi_.PseudoDC_SetPen(*args, **kwargs)

    def SetBrush(*args, **kwargs):
        """
        SetBrush(self, Brush brush)

        Sets the current brush for the DC.

        If the argument is ``wx.NullBrush``, the current brush is selected out
        of the device context, and the original brush restored, allowing the
        current brush to be destroyed safely.
        """
        return _gdi_.PseudoDC_SetBrush(*args, **kwargs)

    def SetBackground(*args, **kwargs):
        """
        SetBackground(self, Brush brush)

        Sets the current background brush for the DC.
        """
        return _gdi_.PseudoDC_SetBackground(*args, **kwargs)

    def SetBackgroundMode(*args, **kwargs):
        """
        SetBackgroundMode(self, int mode)

        *mode* may be one of ``wx.SOLID`` and ``wx.TRANSPARENT``. This setting
        determines whether text will be drawn with a background colour or
        not.
        """
        return _gdi_.PseudoDC_SetBackgroundMode(*args, **kwargs)

    def SetPalette(*args, **kwargs):
        """
        SetPalette(self, Palette palette)

        If this is a window DC or memory DC, assigns the given palette to the
        window or bitmap associated with the DC. If the argument is
        ``wx.NullPalette``, the current palette is selected out of the device
        context, and the original palette restored.
        """
        return _gdi_.PseudoDC_SetPalette(*args, **kwargs)

    def SetTextForeground(*args, **kwargs):
        """
        SetTextForeground(self, Colour colour)

        Sets the current text foreground colour for the DC.
        """
        return _gdi_.PseudoDC_SetTextForeground(*args, **kwargs)

    def SetTextBackground(*args, **kwargs):
        """
        SetTextBackground(self, Colour colour)

        Sets the current text background colour for the DC.
        """
        return _gdi_.PseudoDC_SetTextBackground(*args, **kwargs)

    def SetLogicalFunction(*args, **kwargs):
        """
        SetLogicalFunction(self, int function)

        Sets the current logical function for the device context. This
        determines how a source pixel (from a pen or brush colour, combines
        with a destination pixel in the current device context.

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
        return _gdi_.PseudoDC_SetLogicalFunction(*args, **kwargs)

_gdi_.PseudoDC_swigregister(PseudoDC)



