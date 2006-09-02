/////////////////////////////////////////////////////////////////////////////
// Name:        _bitmap.i
// Purpose:     SWIG interface for wxBitmap and wxMask
//
// Author:      Robin Dunn
//
// Created:     7-July-1997
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module

%{
#include <wx/rawbmp.h>
%}


// Turn off the aquisition of the Global Interpreter Lock for this file
%threadWrapperOff

//---------------------------------------------------------------------------

%{
#include <wx/image.h>
    
    static char** ConvertListOfStrings(PyObject* listOfStrings) {
        char**    cArray = NULL;
        int       count;

        if (!PyList_Check(listOfStrings)) {
            PyErr_SetString(PyExc_TypeError, "Expected a list of strings.");
            return NULL;
        }
        count = PyList_Size(listOfStrings);
        cArray = new char*[count];

        for(int x=0; x<count; x++) {
            // TODO: Need some validation and error checking here
            cArray[x] = PyString_AsString(PyList_GET_ITEM(listOfStrings, x));
        }
        return cArray;
    }

%}

//---------------------------------------------------------------------------

// TODO:  When the API stabalizes and is available on other platforms, add
// wrappers for the new wxBitmap, wxRawBitmap, wxDIB stuff...

DocStr(wxBitmap,
"The wx.Bitmap class encapsulates the concept of a platform-dependent
bitmap.  It can be either monochrome or colour, and either loaded from
a file or created dynamically.  A bitmap can be selected into a memory
device context (instance of `wx.MemoryDC`). This enables the bitmap to
be copied to a window or memory device context using `wx.DC.Blit`, or
to be used as a drawing surface.", "

The BMP and XMP image file formats are supported on all platforms by
wx.Bitmap.  Other formats are automatically loaded by `wx.Image` and
converted to a wx.Bitmap, so any image file format supported by
`wx.Image` can be used.

:todo: Add wrappers and support for raw bitmap data access.  Can this
       be be put into Python without losing the speed benefits of the
       teplates and iterators in rawbmp.h?

:todo: Find a way to do very efficient PIL Image <--> wx.Bitmap
       converstions.

:see: `wx.EmptyBitmap`, `wx.BitmapFromIcon`, `wx.BitmapFromImage`,
      `wx.BitmapFromXPMData`, `wx.BitmapFromBits`, `wx.BitmapFromBuffer`,
      `wx.BitmapFromBufferRGBA`, `wx.Image`
");


MustHaveApp(wxBitmap);

class wxBitmap : public wxGDIObject
{
public:
    DocCtorStr(
        wxBitmap(const wxString& name, wxBitmapType type=wxBITMAP_TYPE_ANY),
        "Loads a bitmap from a file.",
        "
    :param name:  Name of the file to load the bitmap from.
    :param type: The type of image to expect.  Can be one of the following
        constants (assuming that the neccessary `wx.Image` handlers are
        loaded):

        * wx.BITMAP_TYPE_ANY
        * wx.BITMAP_TYPE_BMP
        * wx.BITMAP_TYPE_ICO
        * wx.BITMAP_TYPE_CUR
        * wx.BITMAP_TYPE_XBM
        * wx.BITMAP_TYPE_XPM
        * wx.BITMAP_TYPE_TIF
        * wx.BITMAP_TYPE_GIF
        * wx.BITMAP_TYPE_PNG
        * wx.BITMAP_TYPE_JPEG
        * wx.BITMAP_TYPE_PNM
        * wx.BITMAP_TYPE_PCX
        * wx.BITMAP_TYPE_PICT
        * wx.BITMAP_TYPE_ICON
        * wx.BITMAP_TYPE_ANI
        * wx.BITMAP_TYPE_IFF

:see: Alternate constructors `wx.EmptyBitmap`, `wx.BitmapFromIcon`,
      `wx.BitmapFromImage`, `wx.BitmapFromXPMData`, `wx.BitmapFromBits`,
      `wx.BitmapFromBuffer`, `wx.BitmapFromBufferRGBA`,
");
        
    ~wxBitmap();

    DocCtorStrName(
        wxBitmap(int width, int height, int depth=-1),
        "Creates a new bitmap of the given size.  A depth of -1 indicates the
depth of the current screen or visual. Some platforms only support 1
for monochrome and -1 for the current display depth.", "",
        EmptyBitmap);

    DocCtorStrName(
        wxBitmap(const wxIcon& icon),
        "Create a new bitmap from a `wx.Icon` object.", "",
        BitmapFromIcon);

    DocCtorStrName(
        wxBitmap(const wxImage& image, int depth=-1),
        "Creates bitmap object from a `wx.Image`. This has to be done to
actually display a `wx.Image` as you cannot draw an image directly on
a window. The resulting bitmap will use the provided colour depth (or
that of the current screen colour depth if depth is -1) which entails
that a colour reduction may have to take place.", "",
        BitmapFromImage);

    
    %extend {
        %RenameDocCtor(
            BitmapFromXPMData,
            "Construct a Bitmap from a list of strings formatted as XPM data.", "",
            wxBitmap(PyObject* listOfStrings))
            {
                char**    cArray = NULL;
                wxBitmap* bmp;

                cArray = ConvertListOfStrings(listOfStrings);
                if (! cArray)
                    return NULL;
                bmp = new wxBitmap(cArray);
                delete [] cArray;
                return bmp;
            }

       
        %RenameDocCtor(
            BitmapFromBits,
            "Creates a bitmap from an array of bits.  You should only use this
function for monochrome bitmaps (depth 1) in portable programs: in
this case the bits parameter should contain an XBM image.  For other
bit depths, the behaviour is platform dependent.", "",
            wxBitmap(PyObject* bits, int width, int height, int depth=1 ))
            {
                char*      buf;
                Py_ssize_t length;
                PyString_AsStringAndSize(bits, &buf, &length);
                return new wxBitmap(buf, width, height, depth);
            }
    }    

    
    // wxGDIImage methods
#ifdef __WXMSW__
    long GetHandle();
    %extend {
        void SetHandle(long handle) { self->SetHandle((WXHANDLE)handle); }
    }
#endif

    bool Ok();
    
    DocDeclStr(
        int , GetWidth(),
        "Gets the width of the bitmap in pixels.", "");
    

    DocDeclStr(
        int , GetHeight(),
        "Gets the height of the bitmap in pixels.", "");
    

    DocDeclStr(
        int , GetDepth(),
        "Gets the colour depth of the bitmap. A value of 1 indicates a
monochrome bitmap.", "");
    


    %extend {
        DocStr(GetSize, "Get the size of the bitmap.", "");
        wxSize GetSize() {
            wxSize size(self->GetWidth(), self->GetHeight());
            return size;
        }
    }

    
    DocDeclStr(
        virtual wxImage , ConvertToImage() const,
        "Creates a platform-independent image from a platform-dependent
bitmap. This preserves mask information so that bitmaps and images can
be converted back and forth without loss in that respect.", "");
    

    DocDeclStr(
        virtual wxMask* , GetMask() const,
        "Gets the associated mask (if any) which may have been loaded from a
file or explpicitly set for the bitmap.

:see: `SetMask`, `wx.Mask`
", "");

    // MSW only?    wxBitmap GetMaskBitmap() const;

    %disownarg(wxMask*);
    DocDeclStr(
        virtual void , SetMask(wxMask* mask),
        "Sets the mask for this bitmap.

:see: `GetMask`, `wx.Mask`
", "");
    %cleardisown(wxMask*);
    
    %extend {
        DocStr(SetMaskColour,
               "Create a Mask based on a specified colour in the Bitmap.", "");
        void SetMaskColour(const wxColour& colour) {
            wxMask *mask = new wxMask(*self, colour);
            self->SetMask(mask);
        }
    }


    DocDeclStr(
        virtual wxBitmap , GetSubBitmap(const wxRect& rect) const,
        "Returns a sub-bitmap of the current one as long as the rect belongs
entirely to the bitmap. This function preserves bit depth and mask
information.", "");
    

    DocDeclStr(
        virtual bool , SaveFile(const wxString &name, wxBitmapType type,
                                wxPalette *palette = NULL),
        "Saves a bitmap in the named file.  See `__init__` for a description of
the ``type`` parameter.", "");
    

    DocDeclStr(
        virtual bool , LoadFile(const wxString &name, wxBitmapType type),
        "Loads a bitmap from a file.  See `__init__` for a description of the
``type`` parameter.", "");
    

    
    virtual wxPalette *GetPalette() const;
#ifdef __WXMSW__
    virtual void SetPalette(const wxPalette& palette);
#endif
    
    
    virtual bool CopyFromIcon(const wxIcon& icon);

    DocDeclStr(
        virtual void , SetHeight(int height),
        "Set the height property (does not affect the existing bitmap data).", "");
    
    
    DocDeclStr(
        virtual void , SetWidth(int width),
        "Set the width property (does not affect the existing bitmap data).", "");
    

    DocDeclStr(
        virtual void , SetDepth(int depth),
        "Set the depth property (does not affect the existing bitmap data).", "");
    

    %extend {
        DocStr(SetSize, "Set the bitmap size (does not affect the existing bitmap data).", "");
        void SetSize(const wxSize& size) {
            self->SetWidth(size.x);
            self->SetHeight(size.y);
        }
    }
    
#ifdef __WXMSW__
    bool CopyFromCursor(const wxCursor& cursor);

// WXWIN_COMPATIBILITY_2_4
  #if 0
    int GetQuality();
    void SetQuality(int q);
    %pythoncode { GetQuality = wx._deprecated(GetQuality) }
    %pythoncode { SetQuality = wx._deprecated(SetQuality) }
  #endif
#endif

    %pythoncode { def __nonzero__(self): return self.Ok() }

    %extend {
        bool __eq__(const wxBitmap* other) { return other ? (*self == *other) : false; }
        bool __ne__(const wxBitmap* other) { return other ? (*self != *other) : true;  }
    }

    %property(Depth, GetDepth, SetDepth, doc="See `GetDepth` and `SetDepth`");
    %property(Height, GetHeight, SetHeight, doc="See `GetHeight` and `SetHeight`");
    %property(Mask, GetMask, SetMask, doc="See `GetMask` and `SetMask`");
    %property(Palette, GetPalette, doc="See `GetPalette`");
    %property(Size, GetSize, SetSize, doc="See `GetSize` and `SetSize`");
    %property(SubBitmap, GetSubBitmap, doc="See `GetSubBitmap`");
    %property(Width, GetWidth, SetWidth, doc="See `GetWidth` and `SetWidth`");

};


//---------------------------------------------------------------------------
// Factory functions for creating wxBitmaps from Python buffer objects.  They
// use the Abstract Pixel API to be able to set RGB and A bytes directly into
// the wxBitmap's pixel buffer.

%{
// See http://tinyurl.com/e5adr for what premultiplying alpha means.  It
// appears to me that the other platforms are already doing it, so I'll just
// automatically do it for wxMSW here.
#ifdef __WXMSW__
#define wxPy_premultiply(p, a)   ((p) * (a) / 0xff)
#define wxPy_unpremultiply(p, a) ((a) ? ((p) * 0xff / (a)) : (p))    
#else
#define wxPy_premultiply(p, a)   (p)
#define wxPy_unpremultiply(p, a) (p)    
#endif
%}


%newobject _BitmapFromBufferAlpha;
%newobject _BitmapFromBuffer;
%inline %{
    wxBitmap* _BitmapFromBufferAlpha(int width, int height,
                                    buffer data, int DATASIZE,
                                    buffer alpha, int ALPHASIZE)
    {
        if (DATASIZE != width*height*3) {
            wxPyErr_SetString(PyExc_ValueError, "Invalid data buffer size.");
            return NULL;
        }

        if (ALPHASIZE != width*height) {
            wxPyErr_SetString(PyExc_ValueError, "Invalid alpha buffer size.");
            return NULL;
        }

        wxBitmap* bmp = new wxBitmap(width, height, 32);
        wxAlphaPixelData pixData(*bmp, wxPoint(0,0), wxSize(width,height));
        if (! pixData) {
            // raise an exception...
            wxPyErr_SetString(PyExc_RuntimeError,
                              "Failed to gain raw access to bitmap data.");
            return NULL;
        }
                
        pixData.UseAlpha();
        wxAlphaPixelData::Iterator p(pixData);
        for (int y=0; y<height; y++) {
            wxAlphaPixelData::Iterator rowStart = p;
            for (int x=0; x<width; x++) {
                byte a = *(alpha++);
                p.Red()   = wxPy_premultiply(*(data++), a);
                p.Green() = wxPy_premultiply(*(data++), a);
                p.Blue()  = wxPy_premultiply(*(data++), a);
                p.Alpha() = a;
                ++p; 
            }
            p = rowStart;
            p.OffsetY(pixData, 1);
        }
        return bmp;
    }        
        
    wxBitmap* _BitmapFromBuffer(int width, int height, buffer data, int DATASIZE)
    {
        if (DATASIZE != width*height*3) {
            wxPyErr_SetString(PyExc_ValueError, "Invalid data buffer size.");
            return NULL;
        }

        wxBitmap* bmp = new wxBitmap(width, height, 24);
        wxNativePixelData pixData(*bmp, wxPoint(0,0), wxSize(width,height));
        if (! pixData) {
            // raise an exception...
            wxPyErr_SetString(PyExc_RuntimeError,
                              "Failed to gain raw access to bitmap data.");
            return NULL;
        }
                
        wxNativePixelData::Iterator p(pixData);
        for (int y=0; y<height; y++) {
            wxNativePixelData::Iterator rowStart = p;
            for (int x=0; x<width; x++) {
                p.Red()   = *(data++);
                p.Green() = *(data++);
                p.Blue()  = *(data++);
                ++p; 
            }
            p = rowStart;
            p.OffsetY(pixData, 1);
        }
        return bmp;
    }
%}    


%pythoncode {
def BitmapFromBuffer(width, height, dataBuffer, alphaBuffer=None):
    """
    Creates a `wx.Bitmap` from the data in dataBuffer.  The dataBuffer
    parameter must be a Python object that implements the buffer
    interface, such as a string, array, etc.  The dataBuffer object is
    expected to contain a series of RGB bytes and be width*height*3
    bytes long.  A buffer object can optionally be supplied for the
    image's alpha channel data, and it is expected to be width*height
    bytes long.  On Windows the RGB values are 'premultiplied' by the
    alpha values.  (The other platforms do the multiplication
    themselves.)

    Unlike `wx.ImageFromBuffer` the bitmap created with this function
    does not share the memory buffer with the buffer object.  This is
    because the native pixel buffer format varies on different
    platforms, and so instead an efficient as possible copy of the
    data is made from the buffer objects to the bitmap's native pixel
    buffer.  For direct access to a bitmap's pixel buffer see
    `wx.NativePixelData` and `wx.AlphaPixelData`.

    :see: `wx.Bitmap`, `wx.BitmapFromBufferRGBA`, `wx.NativePixelData`,
          `wx.AlphaPixelData`, `wx.ImageFromBuffer`
    """
    if alphaBuffer is not None:
        return _gdi_._BitmapFromBufferAlpha(width, height, dataBuffer, alphaBuffer)
    else:
        return _gdi_._BitmapFromBuffer(width, height, dataBuffer)
}



%newobject _BitmapFromBufferRGBA;
%inline %{
    wxBitmap* _BitmapFromBufferRGBA(int width, int height, buffer data, int DATASIZE)
    {
        if (DATASIZE != width*height*4) {
            wxPyErr_SetString(PyExc_ValueError, "Invalid data buffer size.");
            return NULL;
        }

        wxBitmap* bmp = new wxBitmap(width, height, 32);
        wxAlphaPixelData pixData(*bmp, wxPoint(0,0), wxSize(width,height));
        if (! pixData) {
            // raise an exception...
            wxPyErr_SetString(PyExc_RuntimeError,
                              "Failed to gain raw access to bitmap data.");
            return NULL;
        }
               
        pixData.UseAlpha();
        wxAlphaPixelData::Iterator p(pixData);
        for (int y=0; y<height; y++) {
            wxAlphaPixelData::Iterator rowStart = p;
            for (int x=0; x<width; x++) {
                byte a = data[3];
                p.Red()   = wxPy_premultiply(*(data++), a);
                p.Green() = wxPy_premultiply(*(data++), a);
                p.Blue()  = wxPy_premultiply(*(data++), a);
                p.Alpha() = a; data++;
                ++p; 
            }
            p = rowStart;
            p.OffsetY(pixData, 1);
        }
        return bmp;
    }        
%}

%pythoncode {
def BitmapFromBufferRGBA(width, height, dataBuffer):
    """
    Creates a `wx.Bitmap` from the data in dataBuffer.  The dataBuffer
    parameter must be a Python object that implements the buffer
    interface, such as a string, array, etc.  The dataBuffer object is
    expected to contain a series of RGBA bytes (red, green, blue and
    alpha) and be width*height*4 bytes long.  On Windows the RGB
    values are 'premultiplied' by the alpha values.  (The other
    platforms do the multiplication themselves.)

    Unlike `wx.ImageFromBuffer` the bitmap created with this function
    does not share the memory buffer with the buffer object.  This is
    because the native pixel buffer format varies on different
    platforms, and so instead an efficient as possible copy of the
    data is made from the buffer object to the bitmap's native pixel
    buffer.  For direct access to a bitmap's pixel buffer see
    `wx.NativePixelData` and `wx.AlphaPixelData`.

    :see: `wx.Bitmap`, `wx.BitmapFromBuffer`, `wx.NativePixelData`,
          `wx.AlphaPixelData`, `wx.ImageFromBuffer`
    """
    return _gdi_._BitmapFromBufferRGBA(width, height, dataBuffer)
}


//---------------------------------------------------------------------------

class wxPixelDataBase
{
public:
    // origin of the rectangular region we represent
    wxPoint GetOrigin() const { return m_ptOrigin; }

    // width and height of the region we represent
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    wxSize GetSize() const { return wxSize(m_width, m_height); }

    // the distance between two rows
    int GetRowStride() const { return m_stride; }

};


// Both wxNativePixelData and wxAlphaPixelData have the same interface, so
// make a macro to declare them both.

%define PIXELDATA(PixelData)
%{
    typedef PixelData##::Iterator PixelData##_Accessor;
%}
class PixelData##_Accessor;
class PixelData : public wxPixelDataBase
{
public:
    %nokwargs PixelData;
    
    PixelData(wxBitmap& bmp);
    PixelData(wxBitmap& bmp, const wxRect& rect);
    PixelData(wxBitmap& bmp, const wxPoint& pt, const wxSize& sz);

    ~PixelData();

    PixelData##_Accessor GetPixels() const;
    void UseAlpha();

    %extend {
        bool __nonzero__() { return self->operator bool(); }
    }

    %pythoncode {
    def __iter__(self):
        """
        Create and return an iterator object for this pixel data
        object.  (It's really a generator but I won't tell if you
        don't tell.)
        """
        width  = self.GetWidth()
        height = self.GetHeight()
        pixels = self.GetPixels()
        
        # This class is a facade over the pixels object (using the one
        # in the enclosing scope) that only allows Get() and Set() to
        # be called.
        class PixelFacade(object):
            def Get(self):
                return pixels.Get()
            def Set(self, *args, **kw):
                return pixels.Set(*args, **kw)
            def __str__(self):
                return str(self.Get())
            def __repr__(self):
                return 'pixel(%d,%d): %s' % (x,y,self.Get())
            X = property(lambda self: x)
            Y = property(lambda self: y)
            
        pf = PixelFacade()        
        for y in xrange(height):
            for x in xrange(width):
                # We always generate the same pf instance, but it
                # accesses the pixels object which we use to iterate
                # over the pixel buffer.
                yield pf    
                pixels.nextPixel()
            pixels.MoveTo(self, 0, y)
    }

    %property(Pixels, GetPixels, doc="See `GetPixels`");
};



class PixelData##_Accessor
{
public:
    %nokwargs PixelData##_Accessor;
    
    PixelData##_Accessor(PixelData& data);
    PixelData##_Accessor(wxBitmap& bmp, PixelData& data);
    PixelData##_Accessor();

    ~PixelData##_Accessor();

    void Reset(const PixelData& data);
    bool IsOk() const;

    %extend {
        // PixelData##_Accessor& nextPixel() { return ++(*self); }
        void nextPixel() { ++(*self); }
    }

    void Offset(const PixelData& data, int x, int y);
    void OffsetX(const PixelData& data, int x);
    void OffsetY(const PixelData& data, int y);
    void MoveTo(const PixelData& data, int x, int y);

// NOTE: For now I'm not wrapping the Red, Green, Blue and Alpha
// functions because I can't hide the premultiplying needed on wxMSW
// if only the individual components are wrapped, plus it would mean 3
// or 4 trips per pixel from Python to C++ instead of just one.
// Instead I've added the Set and Get functions and put the
// premultiplying in there.
    
//     %extend {
//         byte _get_Red()   { return self->Red(); }
//         byte _get_Green() { return self->Green(); }
//         byte _get_Blue()  { return self->Blue(); }

//         void _set_Red(byte val)   { self->Red() = val; }
//         void _set_Green(byte val) { self->Green() = val; }
//         void _set_Blue(byte val)  { self->Blue() = val; }
//     }

//     %pythoncode {
//         Red   = property(_get_Red,   _set_Red)
//         Green = property(_get_Green, _set_Green)
//         Blue  = property(_get_Blue,  _set_Blue)
//     }
};
%enddef


%pythonAppend wxAlphaPixelData::wxAlphaPixelData "self.UseAlpha()"

// Make the classes
PIXELDATA(wxNativePixelData)
PIXELDATA(wxAlphaPixelData)    


// Add in a few things that are different between the wxNativePixelData and
// wxAlphaPixelData and the iterator classes and so are not included in our
// macro...

%extend wxNativePixelData_Accessor {
    void Set(byte red, byte green, byte blue) {
        self->Red()   = red;
        self->Green() = green;
        self->Blue()  = blue;
    }
    
    PyObject* Get() {
        PyObject* rv = PyTuple_New(3);
        PyTuple_SetItem(rv, 0, PyInt_FromLong(self->Red()));
        PyTuple_SetItem(rv, 1, PyInt_FromLong(self->Green()));
        PyTuple_SetItem(rv, 2, PyInt_FromLong(self->Blue()));
        return rv;            
    }    
}

%extend wxAlphaPixelData_Accessor {
//     byte _get_Alpha()         { return self->Alpha(); }
//     void _set_Alpha(byte val) { self->Alpha() = val; }
    
//     %pythoncode {
//         Alpha = property(_get_Alpha, _set_Alpha)
//     }

    void Set(byte red, byte green, byte blue, byte alpha) {
        self->Red()   = wxPy_premultiply(red,   alpha);
        self->Green() = wxPy_premultiply(green, alpha);
        self->Blue()  = wxPy_premultiply(blue,  alpha);
        self->Alpha() = alpha;
    }
    
    PyObject* Get() {
        PyObject* rv = PyTuple_New(4);
        int red   = self->Red();
        int green = self->Green();
        int blue  = self->Blue();
        int alpha = self->Alpha();
            
        PyTuple_SetItem(rv, 0, PyInt_FromLong( wxPy_unpremultiply(red,   alpha) ));
        PyTuple_SetItem(rv, 1, PyInt_FromLong( wxPy_unpremultiply(green, alpha) ));
        PyTuple_SetItem(rv, 2, PyInt_FromLong( wxPy_unpremultiply(blue,  alpha) ));
        PyTuple_SetItem(rv, 3, PyInt_FromLong( alpha ));
        return rv;            
    }
}


//---------------------------------------------------------------------------

DocStr(wxMask,
"This class encapsulates a monochrome mask bitmap, where the masked
area is black and the unmasked area is white. When associated with a
bitmap and drawn in a device context, the unmasked area of the bitmap
will be drawn, and the masked area will not be drawn.

A mask may be associated with a `wx.Bitmap`. It is used in
`wx.DC.DrawBitmap` or `wx.DC.Blit` when the source device context is a
`wx.MemoryDC` with a `wx.Bitmap` selected into it that contains a
mask.", "");

MustHaveApp(wxMask);

class wxMask : public wxObject {
public:

    DocStr(wxMask,
           "Constructs a mask from a `wx.Bitmap` and a `wx.Colour` in that bitmap
that indicates the transparent portions of the mask.  In other words,
the pixels in ``bitmap`` that match ``colour`` will be the transparent
portions of the mask.  If no ``colour`` or an invalid ``colour`` is
passed then BLACK is used.

:see: `wx.Bitmap`, `wx.Colour`", "");
    
    %extend {
        wxMask(const wxBitmap& bitmap, const wxColour& colour = wxNullColour) {
            if ( !colour.Ok() )
                return new wxMask(bitmap, *wxBLACK);
            else
                return new wxMask(bitmap, colour);
        }
    }
    
    ~wxMask();
};

%pythoncode { MaskColour = wx._deprecated(Mask, "wx.MaskColour is deprecated, use `wx.Mask` instead.") }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
%threadWrapperOn
