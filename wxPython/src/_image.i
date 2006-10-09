/////////////////////////////////////////////////////////////////////////////
// Name:        _image.i
// Purpose:     SWIG definitions for wxImage and such
//
// Author:      Robin Dunn
//
// Created:     25-Sept-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include "wx/wxPython/pyistream.h"
%}

//---------------------------------------------------------------------------

enum {
    wxIMAGE_ALPHA_TRANSPARENT,
    wxIMAGE_ALPHA_THRESHOLD,
    wxIMAGE_ALPHA_OPAQUE
};


// Constants for wxImage::Scale() for determining the level of quality
enum
{
    wxIMAGE_QUALITY_NORMAL = 0,
    wxIMAGE_QUALITY_HIGH = 1
};

//---------------------------------------------------------------------------
%newgroup

DocStr(wxImageHandler,
"This is the base class for implementing image file loading/saving, and
image creation from data. It is used within `wx.Image` and is not
normally seen by the application.", "");
class wxImageHandler : public wxObject {
public:
    // wxImageHandler();    Abstract Base Class
    wxString GetName();
    wxString GetExtension();
    long GetType();
    wxString GetMimeType();

    //bool LoadFile(wxImage* image, wxInputStream& stream);
    //bool SaveFile(wxImage* image, wxOutputStream& stream);
    //virtual int GetImageCount( wxInputStream& stream );

    bool CanRead( const wxString& name );
    %Rename(CanReadStream, bool, CanRead( wxInputStream& stream ));
    
    void SetName(const wxString& name);
    void SetExtension(const wxString& extension);
    void SetType(long type);
    void SetMimeType(const wxString& mimetype);

    %property(Extension, GetExtension, SetExtension, doc="See `GetExtension` and `SetExtension`");
    %property(MimeType, GetMimeType, SetMimeType, doc="See `GetMimeType` and `SetMimeType`");
    %property(Name, GetName, SetName, doc="See `GetName` and `SetName`");
    %property(Type, GetType, SetType, doc="See `GetType` and `SetType`");
};


//---------------------------------------------------------------------------


DocStr(wxPyImageHandler,
"This is the base class for implementing image file loading/saving, and
image creation from data, all written in Python.  To create a custom
image handler derive a new class from wx.PyImageHandler and provide
the following methods::

    def DoCanRead(self, stream) --> bool
        '''Check if this handler can read the image on the stream'''

    def LoadFile(self, image, stream, verbose, index) --> bool
        '''Load image data from the stream and load it into image.'''

    def SaveFile(self, image, stream, verbose) --> bool
        '''Save the iamge data in image to the stream using
           this handler's image file format.'''

    def GetImageCount(self, stream) --> int
        '''If this image format can hold more than one image,
           how many does the image on the stream have?'''

To activate your handler create an instance of it and pass it to
`wx.Image_AddHandler`.  Be sure to call `SetName`, `SetType`, and
`SetExtension` from your constructor.
", "");

class wxPyImageHandler: public wxImageHandler {
public:
    %pythonAppend wxPyImageHandler() "self._SetSelf(self)"
    wxPyImageHandler();
    void _SetSelf(PyObject *self);
};


//---------------------------------------------------------------------------


class wxImageHistogram /* : public wxImageHistogramBase */
{
public:
    wxImageHistogram();

    DocStr(MakeKey, "Get the key in the histogram for the given RGB values", "");
    static unsigned long MakeKey(byte r,
                                 byte g,
                                 byte b);

    DocDeclAStr(
        bool, FindFirstUnusedColour(byte *OUTPUT,
                                    byte *OUTPUT,
                                    byte *OUTPUT,
                                    byte startR = 1,
                                    byte startG = 0,
                                    byte startB = 0 ) const,
        "FindFirstUnusedColour(int startR=1, int startG=0, int startB=0) -> (success, r, g, b)",
        "Find first colour that is not used in the image and has higher RGB
values than startR, startG, startB.  Returns a tuple consisting of a
success flag and rgb values.", "");

    %extend {
        DocStr(GetCount,
               "Returns the pixel count for the given key.  Use `MakeKey` to create a
key value from a RGB tripple.", "");
        unsigned long GetCount(unsigned long key) {
            wxImageHistogramEntry e = (*self)[key];
            return e.value;
        }

        DocStr(GetCountRGB,
               "Returns the pixel count for the given RGB values.", "");
        unsigned long GetCountRGB(byte r,
                                  byte g,
                                  byte b) {
            unsigned long key = wxImageHistogram::MakeKey(r, g, b);
            wxImageHistogramEntry e = (*self)[key];
            return e.value;
        }
        
        DocStr(GetCountColour,
               "Returns the pixel count for the given `wx.Colour` value.", "");
        unsigned long GetCountColour(const wxColour& colour) {
            unsigned long key = wxImageHistogram::MakeKey(colour.Red(),
                                                          colour.Green(),
                                                          colour.Blue());
            wxImageHistogramEntry e = (*self)[key];
            return e.value;
        }
    }
    
};


//---------------------------------------------------------------------------

DocStr(wxImage,
"A platform-independent image class.  An image can be created from
data, or using `wx.Bitmap.ConvertToImage`, or loaded from a file in a
variety of formats.  Functions are available to set and get image
bits, so it can be used for basic image manipulation.

A wx.Image cannot be drawn directly to a `wx.DC`.  Instead, a
platform-specific `wx.Bitmap` object must be created from it using the
`wx.BitmapFromImage` constructor. This bitmap can then be drawn in a
device context, using `wx.DC.DrawBitmap`.

One colour value of the image may be used as a mask colour which will
lead to the automatic creation of a `wx.Mask` object associated to the
bitmap object.

wx.Image supports alpha channel data, that is in addition to a byte
for the red, green and blue colour components for each pixel it also
stores a byte representing the pixel opacity. An alpha value of 0
corresponds to a transparent pixel (null opacity) while a value of 255
means that the pixel is 100% opaque.

Unlike RGB data, not all images have an alpha channel and before using
`GetAlpha` you should check if this image contains an alpha channel
with `HasAlpha`. Note that currently only images loaded from PNG files
with transparency information will have an alpha channel.", "");


%{
// Pull the nested class out to the top level for SWIG's sake
#define wxImage_RGBValue wxImage::RGBValue
#define wxImage_HSVValue wxImage::HSVValue
%}

DocStr(wxImage_RGBValue,
"An object that contains values for red, green and blue which represent
the value of a color. It is used by `wx.Image.HSVtoRGB` and
`wx.Image.RGBtoHSV`, which converts between HSV color space and RGB
color space.", "");       
class wxImage_RGBValue
{
public:
    DocCtorStr(
        wxImage_RGBValue(byte r=0, byte g=0, byte b=0),
        "Constructor.", "");
    byte red;  
    byte green;
    byte blue;
};


DocStr(wxImage_HSVValue,
"An object that contains values for hue, saturation and value which
represent the value of a color.  It is used by `wx.Image.HSVtoRGB` and
`wx.Image.RGBtoHSV`, which +converts between HSV color space and RGB
color space.", "");
class wxImage_HSVValue
{
public:
    DocCtorStr(
        wxImage_HSVValue(double h=0.0, double s=0.0, double v=0.0),
        "Constructor.", "");
    double hue;  
    double saturation;
    double value;
};


class wxImage : public wxObject {
public:
    %typemap(out) wxImage*;    // turn off this typemap

    DocCtorStr(
        wxImage( const wxString& name, long type = wxBITMAP_TYPE_ANY, int index = -1 ),
        "Loads an image from a file.",
        "
    :param name:  Name of the file from which to load the image.

    :param type: May be one of the following:

        ====================    =======================================
        wx.BITMAP_TYPE_BMP      Load a Windows bitmap file.
        wx.BITMAP_TYPE_GIF      Load a GIF bitmap file.
        wx.BITMAP_TYPE_JPEG     Load a JPEG bitmap file.
        wx.BITMAP_TYPE_PNG      Load a PNG bitmap file.
        wx.BITMAP_TYPE_PCX      Load a PCX bitmap file.
        wx.BITMAP_TYPE_PNM      Load a PNM bitmap file.
        wx.BITMAP_TYPE_TIF      Load a TIFF bitmap file.
        wx.BITMAP_TYPE_XPM      Load a XPM bitmap file.
        wx.BITMAP_TYPE_ICO      Load a Windows icon file (ICO).
        wx.BITMAP_TYPE_CUR      Load a Windows cursor file (CUR).
        wx.BITMAP_TYPE_ANI      Load a Windows animated cursor file (ANI).
        wx.BITMAP_TYPE_ANY      Will try to autodetect the format. 
        ====================    =======================================

    :param index: Index of the image to load in the case that the
        image file contains multiple images. This is only used by GIF,
        ICO and TIFF handlers. The default value (-1) means to choose
        the default image and is interpreted as the first image (the
        one with index=0) by the GIF and TIFF handler and as the
        largest and most colourful one by the ICO handler.

:see: `wx.ImageFromMime`, `wx.ImageFromStream`, `wx.ImageFromStreamMime`,
      `wx.EmptyImage`, `wx.ImageFromBitmap`, `wx.ImageFromBuffer`,
      `wx.ImageFromData`, `wx.ImageFromDataWithAlpha`
");
    
    ~wxImage();

    // Alternate constructors
    DocCtorStrName(
        wxImage(const wxString& name, const wxString& mimetype, int index = -1),
        "Loads an image from a file, using a MIME type string (such as
'image/jpeg') to specify image type.", "

:see: `wx.Image`",
        ImageFromMime);
    
    DocCtorStrName(
        wxImage(wxInputStream& stream, long type = wxBITMAP_TYPE_ANY, int index = -1),
        "Loads an image from an input stream, or any readable Python file-like
object.", "

:see: `wx.Image`",
        ImageFromStream);
    
    DocCtorStrName(
        wxImage(wxInputStream& stream, const wxString& mimetype, int index = -1 ),
        "Loads an image from an input stream, or any readable Python file-like
object, specifying the image format with a MIME type string.", "

:see: `wx.Image`",
        ImageFromStreamMime);
    
    %extend {
        %RenameDocCtor(
            EmptyImage,
            "Construct an empty image of a given size, optionally setting all
pixels to black.", "

:see: `wx.Image`",
            wxImage(int width=0, int height=0, bool clear = true))
            {
                if (width > 0 && height > 0)
                    return new wxImage(width, height, clear);
                else
                    return new wxImage;
            }

        
       MustHaveApp(wxImage(const wxBitmap &bitmap));
        
        %RenameDocCtor(
            ImageFromBitmap,
            "Construct an Image from a `wx.Bitmap`.", "

:see: `wx.Image`",
            wxImage(const wxBitmap &bitmap))
            {
                return new wxImage(bitmap.ConvertToImage());
            }

        %RenameDocCtor(
            ImageFromData,
            "Construct an Image from a buffer of RGB bytes.  Accepts either a
string or a buffer object holding the data and the length of the data
must be width*height*3.", "

:see: `wx.Image`",
            wxImage(int width, int height, buffer data, int DATASIZE))
            {
                if (DATASIZE != width*height*3) {
                    wxPyErr_SetString(PyExc_ValueError, "Invalid data buffer size.");
                    return NULL;
                }
            
                // Copy the source data so the wxImage can clean it up later
                buffer copy = (buffer)malloc(DATASIZE);
                if (copy == NULL) {
                    wxPyBLOCK_THREADS(PyErr_NoMemory());
                    return NULL;
                }            
                memcpy(copy, data, DATASIZE);
                return new wxImage(width, height, copy, false);
            }

        
        %RenameDocCtor(
            ImageFromDataWithAlpha,
            "Construct an Image from a buffer of RGB bytes with an Alpha channel.
Accepts either a string or a buffer object holding the data and the
length of the data must be width*height*3 bytes, and the length of the
alpha data must be width*height bytes.", "

:see: `wx.Image`",
            wxImage(int width, int height, buffer data, int DATASIZE, buffer alpha, int ALPHASIZE))
            {
                if (DATASIZE != width*height*3) {
                    wxPyErr_SetString(PyExc_ValueError, "Invalid data buffer size.");
                    return NULL;
                }
                if (ALPHASIZE != width*height) {
                    wxPyErr_SetString(PyExc_ValueError, "Invalid alpha buffer size.");
                    return NULL;
                }

                // Copy the source data so the wxImage can clean it up later
                buffer dcopy = (buffer)malloc(DATASIZE);
                if (dcopy == NULL) {
                    wxPyBLOCK_THREADS(PyErr_NoMemory());
                    return NULL;
                }
                memcpy(dcopy, data, DATASIZE);
            
                buffer acopy = (buffer)malloc(ALPHASIZE);
                if (acopy == NULL) {
                    wxPyBLOCK_THREADS(PyErr_NoMemory());
                    return NULL;
                }
                memcpy(acopy, alpha, ALPHASIZE);
            
                return new wxImage(width, height, dcopy, acopy, false);
            }
    }

    // TODO: wxImage( char** xpmData );

    // Turn the typemap back on again
    %typemap(out) wxImage* { $result = wxPyMake_wxObject($1, $owner); }


    DocDeclStr(
        void , Create( int width, int height, bool clear=true ),
        "Creates a fresh image.  If clear is ``True``, the new image will be
initialized to black. Otherwise, the image data will be uninitialized.", "");
    
    DocDeclStr(
        void , Destroy(),
        "Destroys the image data.", "");
    

    DocDeclStr(
        wxImage , Scale( int width, int height, int quality = wxIMAGE_QUALITY_NORMAL ),
        "Returns a scaled version of the image. This is also useful for scaling
bitmaps in general as the only other way to scale bitmaps is to blit a
`wx.MemoryDC` into another `wx.MemoryDC`.  The ``quality`` parameter
specifies what method to use for resampling the image.  It can be
either wx.IMAGE_QUALITY_NORMAL, which uses the normal default scaling
method of pixel replication, or wx.IMAGE_QUALITY_HIGH which uses
bicubic and box averaging resampling methods for upsampling and
downsampling respectively.", "

It should be noted that although using wx.IMAGE_QUALITY_HIGH produces
much nicer looking results it is a slower method.  Downsampling will
use the box averaging method which seems to operate very fast.  If you
are upsampling larger images using this method you will most likely
notice that it is a bit slower and in extreme cases it will be quite
substantially slower as the bicubic algorithm has to process a lot of
data.

It should also be noted that the high quality scaling may not work as
expected when using a single mask colour for transparency, as the
scaling will blur the image and will therefore remove the mask
partially. Using the alpha channel will work.

:see: `Rescale`");

    
    wxImage ResampleBox(int width, int height) const;
    wxImage ResampleBicubic(int width, int height) const;

    DocDeclStr(
        wxImage , Blur(int radius),
        "Blurs the image in both horizontal and vertical directions by the
specified pixel ``radius``. This should not be used when using a
single mask colour for transparency.", "");
    
    DocDeclStr(
        wxImage , BlurHorizontal(int radius),
        "Blurs the image in the horizontal direction only. This should not be
used when using a single mask colour for transparency.
", "");
    
    DocDeclStr(
        wxImage , BlurVertical(int radius),
        "Blurs the image in the vertical direction only. This should not be
used when using a single mask colour for transparency.", "");
    

    
    DocDeclStr(
        wxImage , ShrinkBy( int xFactor , int yFactor ) const ,
        "Return a version of the image scaled smaller by the given factors.", "");
    
    DocDeclStr(
        wxImage& , Rescale(int width, int height, int quality = wxIMAGE_QUALITY_NORMAL),
        "Changes the size of the image in-place by scaling it: after a call to
this function, the image will have the given width and height.

Returns the (modified) image itself.", "

:see: `Scale`");
    

    // resizes the image in place
    DocDeclStr(
        wxImage& , Resize( const wxSize& size, const wxPoint& pos, 
                           int r = -1, int g = -1, int b = -1 ),
        "Changes the size of the image in-place without scaling it, by adding
either a border with the given colour or cropping as necessary. The
image is pasted into a new image with the given size and background
colour at the position pos relative to the upper left of the new
image. If red = green = blue = -1 then use either the current mask
colour if set or find, use, and set a suitable mask colour for any
newly exposed areas.

Returns the (modified) image itself.", "

:see: `Size`");
    
    
    DocDeclStr(
        void , SetRGB( int x, int y, byte r, byte g, byte b ),
        "Sets the pixel at the given coordinate. This routine performs
bounds-checks for the coordinate so it can be considered a safe way to
manipulate the data, but in some cases this might be too slow so that
the data will have to be set directly. In that case you will have to
get access to the image data using the `GetData` method.", "");
    

    DocDeclStrName(
            void, SetRGB( const wxRect& rect,
                          byte r, byte g, byte b ),
            "Sets the colour of the pixels within the given rectangle. This routine
performs bounds-checks for the rectangle so it can be considered a
safe way to manipulate the data.", "",
            SetRGBRect);

    DocDeclStr(
        byte , GetRed( int x, int y ),
        "Returns the red intensity at the given coordinate.", "");
    
    DocDeclStr(
        byte , GetGreen( int x, int y ),
        "Returns the green intensity at the given coordinate.", "");
    
    DocDeclStr(
        byte , GetBlue( int x, int y ),
        "Returns the blue intensity at the given coordinate.", "");
    

    DocDeclStr(
        void , SetAlpha(int x, int y, byte alpha),
        "Sets the alpha value for the given pixel. This function should only be
called if the image has alpha channel data, use `HasAlpha` to check
for this.", "");
    
    DocDeclStr(
        byte , GetAlpha(int x, int y),
        "Returns the alpha value for the given pixel. This function may only be
called for the images with alpha channel, use `HasAlpha` to check for
this.

The returned value is the *opacity* of the image, i.e. the value of 0
corresponds to the fully transparent pixels while the value of 255 to
the fully opaque pixels.", "");
    
    DocDeclStr(
        bool , HasAlpha(),
        "Returns true if this image has alpha channel, false otherwise.", "

:see: `GetAlpha`, `SetAlpha`");
    

    DocDeclStr(
        void , InitAlpha(),
        "Initializes the image alpha channel data. It is an error to call it if
the image already has alpha data. If it doesn't, alpha data will be by
default initialized to all pixels being fully opaque. But if the image
has a a mask colour, all mask pixels will be completely transparent.", "");


    DocDeclStr(
        bool , IsTransparent(int x, int y,
                             byte threshold = wxIMAGE_ALPHA_THRESHOLD) const,
        "Returns ``True`` if this pixel is masked or has an alpha value less
than the spcified threshold.", "");
    
    
    // find first colour that is not used in the image and has higher
    // RGB values than <startR,startG,startB>
    DocDeclAStr(
        bool, FindFirstUnusedColour( byte *OUTPUT, byte *OUTPUT, byte *OUTPUT,
                                     byte startR = 0, byte startG = 0, byte startB = 0 ) const,
        "FindFirstUnusedColour(int startR=1, int startG=0, int startB=0) -> (success, r, g, b)",
        "Find first colour that is not used in the image and has higher RGB
values than startR, startG, startB.  Returns a tuple consisting of a
success flag and rgb values.", "");

    
    DocDeclStr(
        bool , ConvertAlphaToMask(byte threshold = wxIMAGE_ALPHA_THRESHOLD),
        "If the image has alpha channel, this method converts it to mask. All
pixels with alpha value less than ``threshold`` are replaced with the
mask colour and the alpha channel is removed. The mask colour is
chosen automatically using `FindFirstUnusedColour`.

If the image image doesn't have alpha channel, ConvertAlphaToMask does
nothing.", "");
    

    DocDeclStr(
        bool , ConvertColourToAlpha( byte r, byte g, byte b ),
        "This method converts an image where the original alpha information is
only available as a shades of a colour (actually shades of grey)
typically when you draw anti-aliased text into a bitmap. The DC
drawing routines draw grey values on the black background although
they actually mean to draw white with differnt alpha values.  This
method reverses it, assuming a black (!) background and white text.
The method will then fill up the whole image with the colour given.", "");
    

    
    DocDeclStr(
        bool , SetMaskFromImage(const wxImage & mask,
                                byte mr, byte mg, byte mb),
        "Sets the image's mask so that the pixels that have RGB value of
``(mr,mg,mb)`` in ``mask`` will be masked in this image. This is done
by first finding an unused colour in the image, setting this colour as
the mask colour and then using this colour to draw all pixels in the
image who corresponding pixel in mask has given RGB value.

Returns ``False`` if ``mask`` does not have same dimensions as the
image or if there is no unused colour left. Returns ``True`` if the
mask was successfully applied.

Note that this method involves computing the histogram, which is
computationally intensive operation.", "");
    

//      void DoFloodFill (wxCoord x, wxCoord y,
//          const wxBrush & fillBrush,
//          const wxColour& testColour,
//          int style = wxFLOOD_SURFACE,
//          int LogicalFunction = wxCOPY /* currently unused */ ) ;

    DocDeclStr(
        static bool , CanRead( const wxString& filename ),
        "Returns True if the image handlers can read this file.", "");
    
    DocDeclStr(
        static int , GetImageCount( const wxString& filename, long type = wxBITMAP_TYPE_ANY ),
        "If the image file contains more than one image and the image handler
is capable of retrieving these individually, this function will return
the number of available images.", "");
    

    DocDeclStr(
        bool , LoadFile( const wxString& name, long type = wxBITMAP_TYPE_ANY, int index = -1 ),
        "Loads an image from a file. If no handler type is provided, the
library will try to autodetect the format.", "");
    
    DocDeclStrName(
        bool , LoadFile( const wxString& name, const wxString& mimetype, int index = -1 ),
        "Loads an image from a file, specifying the image type with a MIME type
string.", "",
        LoadMimeFile);
    

    DocDeclStr(
        bool , SaveFile( const wxString& name, int type ),
        "Saves an image in the named file.", "");

    
    DocDeclStrName(
        bool , SaveFile( const wxString& name, const wxString& mimetype ),
        "Saves an image in the named file.", "",
        SaveMimeFile);
    

    DocDeclStrName(
        static bool , CanRead( wxInputStream& stream ),
        "Returns True if the image handlers can read an image file from the
data currently on the input stream, or a readable Python file-like
object.", "",
        CanReadStream);


    DocDeclStrName(
        bool , LoadFile( wxInputStream& stream, long type = wxBITMAP_TYPE_ANY, int index = -1 ),
        "Loads an image from an input stream or a readable Python file-like
object. If no handler type is provided, the library will try to
autodetect the format.", "",
        LoadStream);
    
    
    DocDeclStrName(
        bool , LoadFile( wxInputStream& stream, const wxString& mimetype, int index = -1 ),
        "Loads an image from an input stream or a readable Python file-like
object, using a MIME type string to specify the image file format.", "",
        LoadMimeStream);
    

    DocDeclStr(
        bool , IsOk(),
        "Returns true if image data is present.", "");
    %pythoncode { Ok = IsOk }
    
    DocDeclStr(
        int , GetWidth(),
        "Gets the width of the image in pixels.", "");
    
    DocDeclStr(
        int , GetHeight(),
        "Gets the height of the image in pixels.", "");
    

    %extend {
        DocStr(GetSize,
               "Returns the size of the image in pixels.", "");
        wxSize GetSize() {
            wxSize size(self->GetWidth(), self->GetHeight());
            return size;
        }
    }

    
    DocDeclStr(
        wxImage , GetSubImage(const wxRect& rect),
        "Returns a sub image of the current one as long as the rect belongs
entirely to the image.", "");
    

    DocDeclStr(
        wxImage , Size( const wxSize& size, const wxPoint& pos, 
                        int r = -1, int g = -1, int b = -1 ) const,
        "Returns a resized version of this image without scaling it by adding
either a border with the given colour or cropping as necessary. The
image is pasted into a new image with the given size and background
colour at the position ``pos`` relative to the upper left of the new
image. If red = green = blue = -1 then use either the current mask
colour if set or find, use, and set a suitable mask colour for any
newly exposed areas.", "

:see: `Resize`");
    
    
    DocDeclStr(
        wxImage , Copy(),
        "Returns an identical copy of the image.", "");
    
    DocDeclStr(
        void , Paste( const wxImage &image, int x, int y ),
        "Pastes ``image`` into this instance and takes care of the mask colour
and any out of bounds problems.", "");
    

    //unsigned char *GetData();
    //void SetData( unsigned char *data );

    %extend {
        DocStr(GetData,
               "Returns a string containing a copy of the RGB bytes of the image.", "");
        PyObject* GetData()
        {
            buffer data = self->GetData();
            int len = self->GetWidth() * self->GetHeight() * 3;
            PyObject* rv;
            wxPyBLOCK_THREADS( rv = PyString_FromStringAndSize((char*)data, len));
            return rv;
        }
        DocStr(SetData,
               "Resets the Image's RGB data from a buffer of RGB bytes.  Accepts
either a string or a buffer object holding the data and the length of
the data must be width*height*3.", "");
        void SetData(buffer data, int DATASIZE)
        {
            if (DATASIZE != self->GetWidth() * self->GetHeight() * 3) {
                wxPyErr_SetString(PyExc_ValueError, "Invalid data buffer size.");
                return;
            }            
            buffer copy = (buffer)malloc(DATASIZE);
            if (copy == NULL) {
                wxPyBLOCK_THREADS(PyErr_NoMemory());
                return;
            }            
            memcpy(copy, data, DATASIZE);
            self->SetData(copy, false);
            // wxImage takes ownership of copy...
        }


        DocStr(GetDataBuffer,
               "Returns a writable Python buffer object that is pointing at the RGB
image data buffer inside the wx.Image. You need to ensure that you do
not use this buffer object after the image has been destroyed.", "");
        PyObject* GetDataBuffer()
        {
            buffer data = self->GetData();
            int len = self->GetWidth() * self->GetHeight() * 3;
            PyObject* rv;
            wxPyBLOCK_THREADS( rv = PyBuffer_FromReadWriteMemory(data, len) );
            return rv;
        }

        DocStr(SetDataBuffer,
               "Sets the internal image data pointer to point at a Python buffer
object.  This can save making an extra copy of the data but you must
ensure that the buffer object lives longer than the wx.Image does.", "");
        void SetDataBuffer(buffer data, int DATASIZE)
        {
            if (DATASIZE != self->GetWidth() * self->GetHeight() * 3) {
                wxPyErr_SetString(PyExc_ValueError, "Invalid data buffer size.");
                return;
            }
            self->SetData(data, true);
        }



        DocStr(GetAlphaData,
               "Returns a string containing a copy of the alpha bytes of the image.", "");
        PyObject* GetAlphaData() {
            buffer data = self->GetAlpha();
            if (! data) {
                RETURN_NONE();
            } else {
                int len = self->GetWidth() * self->GetHeight();
                PyObject* rv;
                wxPyBLOCK_THREADS( rv = PyString_FromStringAndSize((char*)data, len) );
                return rv;
            }
        }

        DocStr(SetAlphaData,
               "Resets the Image's alpha data from a buffer of bytes.  Accepts either
a string or a buffer object holding the data and the length of the
data must be width*height.", ""); 
        void SetAlphaData(buffer alpha, int ALPHASIZE)
        {
            if (ALPHASIZE != self->GetWidth() * self->GetHeight()) {
                wxPyErr_SetString(PyExc_ValueError, "Invalid alpha buffer size.");
                return;
            }
            buffer acopy = (buffer)malloc(ALPHASIZE);
            if (acopy == NULL) {
                wxPyBLOCK_THREADS(PyErr_NoMemory());
                return;
            }
            memcpy(acopy, alpha, ALPHASIZE);
            self->SetAlpha(acopy, false);
            // wxImage takes ownership of acopy...
        }


        
        DocStr(GetAlphaBuffer,
               "Returns a writable Python buffer object that is pointing at the Alpha
data buffer inside the wx.Image. You need to ensure that you do not
use this buffer object after the image has been destroyed.", "");
        PyObject* GetAlphaBuffer()
        {
            buffer data = self->GetAlpha();
            int len = self->GetWidth() * self->GetHeight();
            PyObject* rv;
            wxPyBLOCK_THREADS( rv = PyBuffer_FromReadWriteMemory(data, len) );
            return rv;
        }

        
        DocStr(SetAlphaBuffer,
               "Sets the internal image alpha pointer to point at a Python buffer
object.  This can save making an extra copy of the data but you must
ensure that the buffer object lives as long as the wx.Image does.", "");
        void SetAlphaBuffer(buffer alpha, int ALPHASIZE)
        {
            if (ALPHASIZE != self->GetWidth() * self->GetHeight()) {
                wxPyErr_SetString(PyExc_ValueError, "Invalid alpha buffer size.");
                return;
            }
            self->SetAlpha(alpha, true);
        }
    }


    
    DocDeclStr(
        void , SetMaskColour( byte r, byte g, byte b ),
        "Sets the mask colour for this image (and tells the image to use the
mask).", "");
    

    DocDeclAStr(
        /*bool*/ void , GetOrFindMaskColour( byte *OUTPUT,
                                             byte *OUTPUT,
                                             byte *OUTPUT ) const,
        "GetOrFindMaskColour() -> (r,g,b)",
        "Get the current mask colour or find a suitable colour.", "");
    

    DocDeclStr(
        byte , GetMaskRed(),
        "Gets the red component of the mask colour.", "");
    
    DocDeclStr(
        byte , GetMaskGreen(),
        "Gets the green component of the mask colour.", "");
    
    DocDeclStr(
        byte , GetMaskBlue(),
        "Gets the blue component of the mask colour.", "");
    
    DocDeclStr(
        void , SetMask( bool mask = true ),
        "Specifies whether there is a mask or not. The area of the mask is
determined by the current mask colour.", "");
    
    DocDeclStr(
        bool , HasMask(),
        "Returns ``True`` if there is a mask active, ``False`` otherwise.", "");
    

    DocDeclStr(
        wxImage , Rotate(double angle, const wxPoint & centre_of_rotation,
                         bool interpolating = true, wxPoint * offset_after_rotation = NULL) const ,
        "Rotates the image about the given point, by ``angle`` radians. Passing
``True`` to ``interpolating`` results in better image quality, but is
slower. If the image has a mask, then the mask colour is used for the
uncovered pixels in the rotated image background. Otherwise, black
will be used as the fill colour.

Returns the rotated image, leaving this image intact.", "");
    
    DocDeclStr(
        wxImage , Rotate90( bool clockwise = true ) ,
        "Returns a copy of the image rotated 90 degrees in the direction
indicated by ``clockwise``.", "");
    
    DocDeclStr(
        wxImage , Mirror( bool horizontally = true ) ,
        "Returns a mirrored copy of the image. The parameter ``horizontally``
indicates the orientation.", "");
    

    DocDeclStr(
        void , Replace( byte r1, byte g1, byte b1,
                        byte r2, byte g2, byte b2 ),
        "Replaces the colour specified by ``(r1,g1,b1)`` by the colour
``(r2,g2,b2)``.", "");

    DocDeclStr(
        wxImage , ConvertToGreyscale( double lr = 0.299,
                                      double lg = 0.587,
                                      double lb = 0.114 ) const,
        "Convert to greyscale image. Uses the luminance component (Y) of the
image.  The luma value (YUV) is calculated using (R * lr) + (G * lg) + (B * lb),
defaults to ITU-T BT.601", "");
    

    DocDeclStr(
        wxImage , ConvertToMono( byte r, byte g, byte b ) const,
        "Returns monochromatic version of the image. The returned image has
white colour where the original has ``(r,g,b)`` colour and black
colour everywhere else.", "");
    

    DocDeclStr(
        void , SetOption(const wxString& name, const wxString& value),
        "Sets an image handler defined option.  For example, when saving as a
JPEG file, the option ``wx.IMAGE_OPTION_QUALITY`` is used, which is a
number between 0 and 100 (0 is terrible, 100 is very good).", "

    ================================= ===
    wx.IMAGE_OPTION_BMP_FORMAT
    wx.IMAGE_OPTION_CUR_HOTSPOT_X
    wx.IMAGE_OPTION_CUR_HOTSPOT_Y
    wx.IMAGE_OPTION_RESOLUTION
    wx.IMAGE_OPTION_RESOLUTIONX
    wx.IMAGE_OPTION_RESOLUTIONY
    wx.IMAGE_OPTION_RESOLUTIONUNIT
    wx.IMAGE_OPTION_QUALITY
    wx.IMAGE_OPTION_BITSPERSAMPLE
    wx.IMAGE_OPTION_SAMPLESPERPIXEL
    wx.IMAGE_OPTION_COMPRESSION
    wx.IMAGE_OPTION_IMAGEDESCRIPTOR
    wx.IMAGE_OPTION_PNG_FORMAT
    wx.IMAGE_OPTION_PNG_BITDEPTH
    ================================= ===

:see: `HasOption`, `GetOption`, `GetOptionInt`, `SetOptionInt`");
    
    DocDeclStrName(
        void,  SetOption(const wxString& name, int value),
        "Sets an image option as an integer.", "

:see: `HasOption`, `GetOption`, `GetOptionInt`, `SetOption`",
        SetOptionInt);
    
    DocDeclStr(
        wxString , GetOption(const wxString& name) const,
        "Gets the value of an image handler option.", "

:see: `HasOption`, `GetOptionInt`, `SetOption`, `SetOptionInt`");
    
    DocDeclStr(
        int , GetOptionInt(const wxString& name) const,
        "Gets the value of an image handler option as an integer.  If the given
option is not present, the function returns 0.", "

:see: `HasOption`, `GetOption`, `SetOptionInt`, `SetOption`");
    
    DocDeclStr(
        bool , HasOption(const wxString& name) const,
        "Returns true if the given option is present.", "

:see: `GetOption`, `GetOptionInt`, `SetOption`, `SetOptionInt`");
    

    unsigned long CountColours( unsigned long stopafter = (unsigned long) -1 );
    unsigned long ComputeHistogram( wxImageHistogram& h );

    static void AddHandler( wxImageHandler *handler );
    static void InsertHandler( wxImageHandler *handler );
    static bool RemoveHandler( const wxString& name );
    %extend {
        static PyObject* GetHandlers() {
            wxList& list = wxImage::GetHandlers();
            return wxPy_ConvertList(&list);
        }
    }
    
    DocDeclStr(
        static wxString , GetImageExtWildcard(),
        "Iterates all registered wxImageHandler objects, and returns a string
containing file extension masks suitable for passing to file open/save
dialog boxes.", "");
    
    

MustHaveApp(ConvertToBitmap);
MustHaveApp(ConvertToMonoBitmap);

    %extend {
        wxBitmap ConvertToBitmap(int depth=-1) {
            wxBitmap bitmap(*self, depth);
            return bitmap;
        }

        wxBitmap ConvertToMonoBitmap( byte red,
                                      byte green,
                                      byte blue ) {
            wxImage mono = self->ConvertToMono( red, green, blue );
            wxBitmap bitmap( mono, 1 );
            return bitmap;
        }
    }

    
    DocDeclStr(
        void , RotateHue(double angle),
        "Rotates the hue of each pixel of the image. Hue is a double in the
range -1.0..1.0 where -1.0 is -360 degrees and 1.0 is 360 degrees", "");
        
    DocDeclStr(
        static wxImage_HSVValue , RGBtoHSV(wxImage_RGBValue rgb),
        "Converts a color in RGB color space to HSV color space.", "");
    
    DocDeclStr(
        static wxImage_RGBValue , HSVtoRGB(wxImage_HSVValue hsv),
        "Converts a color in HSV color space to RGB color space.", "");
    

    %pythoncode { def __nonzero__(self): return self.IsOk() }
    
    %property(AlphaBuffer, GetAlphaBuffer, SetAlphaBuffer, doc="See `GetAlphaBuffer` and `SetAlphaBuffer`");
    %property(AlphaData, GetAlphaData, SetAlphaData, doc="See `GetAlphaData` and `SetAlphaData`");
    %property(Data, GetData, SetData, doc="See `GetData` and `SetData`");
    %property(DataBuffer, GetDataBuffer, SetDataBuffer, doc="See `GetDataBuffer` and `SetDataBuffer`");
    %property(Height, GetHeight, doc="See `GetHeight`");
    %property(MaskBlue, GetMaskBlue, doc="See `GetMaskBlue`");
    %property(MaskGreen, GetMaskGreen, doc="See `GetMaskGreen`");
    %property(MaskRed, GetMaskRed, doc="See `GetMaskRed`");
    %property(Size, GetSize, doc="See `GetSize`");
    %property(Width, GetWidth, doc="See `GetWidth`");
};



// Make an image from buffer objects.  Not that this is here instead of in the
// wxImage class (as a constructor) because there is already another one with
// the exact same signature, so there woudl be ambiguities in the generated
// C++.  Doing it as an independent factory function like this accomplishes
// the same thing however.
%newobject _ImageFromBuffer;
%inline %{
    wxImage* _ImageFromBuffer(int width, int height,
                              buffer data, int DATASIZE,
                              buffer alpha=NULL, int ALPHASIZE=0)
    {
        if (DATASIZE != width*height*3) {
            wxPyErr_SetString(PyExc_ValueError, "Invalid data buffer size.");
            return NULL;
        }
        if (alpha != NULL) {
            if (ALPHASIZE != width*height) {
                wxPyErr_SetString(PyExc_ValueError, "Invalid alpha buffer size.");
                return NULL;
            }
            return new wxImage(width, height, data, alpha, true);
        }                
        return new wxImage(width, height, data, true);
    }                              
%}

%pythoncode {
def ImageFromBuffer(width, height, dataBuffer, alphaBuffer=None):
    """
    Creates a `wx.Image` from the data in dataBuffer.  The dataBuffer
    parameter must be a Python object that implements the buffer interface,
    such as a string, array, etc.  The dataBuffer object is expected to
    contain a series of RGB bytes and be width*height*3 bytes long.  A buffer
    object can optionally be supplied for the image's alpha channel data, and
    it is expected to be width*height bytes long.

    The wx.Image will be created with its data and alpha pointers initialized
    to the memory address pointed to by the buffer objects, thus saving the
    time needed to copy the image data from the buffer object to the wx.Image.
    While this has advantages, it also has the shoot-yourself-in-the-foot
    risks associated with sharing a C pointer between two objects.

    To help alleviate the risk a reference to the data and alpha buffer
    objects are kept with the wx.Image, so that they won't get deleted until
    after the wx.Image is deleted.  However please be aware that it is not
    guaranteed that an object won't move its memory buffer to a new location
    when it needs to resize its contents.  If that happens then the wx.Image
    will end up referring to an invalid memory location and could cause the
    application to crash.  Therefore care should be taken to not manipulate
    the objects used for the data and alpha buffers in a way that would cause
    them to change size.
    """
    image = _core_._ImageFromBuffer(width, height, dataBuffer, alphaBuffer)
    image._buffer = dataBuffer
    image._alpha = alphaBuffer
    return image
}


///void wxInitAllImageHandlers();

%pythoncode {
    def InitAllImageHandlers():
        """
        The former functionality of InitAllImageHanders is now done internal to
        the _core_ extension module and so this function has become a simple NOP.
        """
        pass
}



%immutable;
const wxImage    wxNullImage;
%mutable;

//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(IMAGE_OPTION_FILENAME);
MAKE_CONST_WXSTRING(IMAGE_OPTION_BMP_FORMAT);
MAKE_CONST_WXSTRING(IMAGE_OPTION_CUR_HOTSPOT_X);
MAKE_CONST_WXSTRING(IMAGE_OPTION_CUR_HOTSPOT_Y);
MAKE_CONST_WXSTRING(IMAGE_OPTION_RESOLUTION);
MAKE_CONST_WXSTRING(IMAGE_OPTION_RESOLUTIONX);
MAKE_CONST_WXSTRING(IMAGE_OPTION_RESOLUTIONY);
MAKE_CONST_WXSTRING(IMAGE_OPTION_RESOLUTIONUNIT);
MAKE_CONST_WXSTRING(IMAGE_OPTION_QUALITY);

enum
{
    wxIMAGE_RESOLUTION_INCHES = 1,
    wxIMAGE_RESOLUTION_CM = 2
};


MAKE_CONST_WXSTRING(IMAGE_OPTION_BITSPERSAMPLE);
MAKE_CONST_WXSTRING(IMAGE_OPTION_SAMPLESPERPIXEL); 
MAKE_CONST_WXSTRING(IMAGE_OPTION_COMPRESSION);
MAKE_CONST_WXSTRING(IMAGE_OPTION_IMAGEDESCRIPTOR);

MAKE_CONST_WXSTRING(IMAGE_OPTION_PNG_FORMAT);
MAKE_CONST_WXSTRING(IMAGE_OPTION_PNG_BITDEPTH);

enum
{
    wxPNG_TYPE_COLOUR = 0,
    wxPNG_TYPE_GREY = 2,
    wxPNG_TYPE_GREY_RED = 3
};

enum
{
    wxBMP_24BPP        = 24, // default, do not need to set
    //wxBMP_16BPP      = 16, // wxQuantize can only do 236 colors?
    wxBMP_8BPP         =  8, // 8bpp, quantized colors
    wxBMP_8BPP_GREY    =  9, // 8bpp, rgb averaged to greys
    wxBMP_8BPP_GRAY    =  wxBMP_8BPP_GREY,
    wxBMP_8BPP_RED     = 10, // 8bpp, red used as greyscale
    wxBMP_8BPP_PALETTE = 11, // 8bpp, use the wxImage's palette
    wxBMP_4BPP         =  4, // 4bpp, quantized colors
    wxBMP_1BPP         =  1, // 1bpp, quantized "colors"
    wxBMP_1BPP_BW      =  2  // 1bpp, black & white from red
};


DocStr(wxBMPHandler,
"A `wx.ImageHandler` for \*.bmp bitmap files.", "");
class wxBMPHandler : public wxImageHandler {
public:
    wxBMPHandler();
};

DocStr(wxICOHandler,
"A `wx.ImageHandler` for \*.ico icon files.", "");
class wxICOHandler : public wxBMPHandler {
public:
    wxICOHandler();
};

DocStr(wxCURHandler,
"A `wx.ImageHandler` for \*.cur cursor files.", "");
class wxCURHandler : public wxICOHandler {
public:
    wxCURHandler();
};

DocStr(wxANIHandler,
"A `wx.ImageHandler` for \*.ani animated cursor files.", "");
class wxANIHandler : public wxCURHandler {
public:
    wxANIHandler();
};


//---------------------------------------------------------------------------

DocStr(wxPNGHandler,
"A `wx.ImageHandler` for PNG image files.", "");
class wxPNGHandler : public wxImageHandler {
public:
    wxPNGHandler();
};


DocStr(wxGIFHandler,
"A `wx.ImageHandler` for GIF image files.", "");
class wxGIFHandler : public wxImageHandler {
public:
    wxGIFHandler();
};


DocStr(wxPCXHandler,
"A `wx.ImageHandler` for PCX imager files.", "");
class wxPCXHandler : public wxImageHandler {
public:
    wxPCXHandler();
};


DocStr(wxJPEGHandler,
"A `wx.ImageHandler` for JPEG/JPG image files.", "");
class wxJPEGHandler : public wxImageHandler {
public:
    wxJPEGHandler();
};


DocStr(wxPNMHandler,
"A `wx.ImageHandler` for PNM image files.", "");
class wxPNMHandler : public wxImageHandler {
public:
    wxPNMHandler();
};

DocStr(wxXPMHandler,
"A `wx.ImageHandler` for XPM image.", "");
class wxXPMHandler : public wxImageHandler {
public:
    wxXPMHandler();
};

DocStr(wxTIFFHandler,
"A `wx.ImageHandler` for TIFF image files.", "");
class wxTIFFHandler : public wxImageHandler {
public:
    wxTIFFHandler();
};


#if wxUSE_IFF
DocStr(wxIFFHandler,
"A `wx.ImageHandler` for IFF image files.", "");
class wxIFFHandler : public wxImageHandler {
public:
    wxIFFHandler();
};
#endif

//---------------------------------------------------------------------------

%{
#include <wx/quantize.h>
%}

enum {
    wxQUANTIZE_INCLUDE_WINDOWS_COLOURS,
//    wxQUANTIZE_RETURN_8BIT_DATA,
    wxQUANTIZE_FILL_DESTINATION_IMAGE
};


DocStr(wxQuantize,
       "Performs quantization, or colour reduction, on a wxImage.", "");

class wxQuantize /*: public wxObject */
{
public:
    
    %extend {
        DocStr(
            Quantize,
            "Reduce the colours in the source image and put the result into the
destination image, setting the palette in the destination if
needed. Both images may be the same, to overwrite the source image.", "
:todo: Create a version that returns the wx.Palette used.");
    
        static bool Quantize(const wxImage& src, wxImage& dest, int desiredNoColours = 236,
                             int flags = wxQUANTIZE_INCLUDE_WINDOWS_COLOURS|wxQUANTIZE_FILL_DESTINATION_IMAGE)
        {
                return wxQuantize::Quantize(src, dest, 
                                            //NULL, // palette
                                            desiredNoColours,
                                            NULL, // eightBitData
                                            flags);
        }
    }
};


//---------------------------------------------------------------------------
