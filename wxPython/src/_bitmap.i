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


class wxBitmap : public wxGDIObject
{
public:
    DocCtorStr(
        wxBitmap(const wxString& name, wxBitmapType type=wxBITMAP_TYPE_ANY),
        "Loads a bitmap from a file.");
        
    ~wxBitmap();

    DocCtorStrName(
        wxBitmap(int width, int height, int depth=-1),
        "Creates a new bitmap of the given size.  A depth of -1 indicates the depth of\n"
        "the current screen or visual. Some platforms only support 1 for monochrome and\n"
        "-1 for the current colour setting.",
        EmptyBitmap);

    DocCtorStrName(
        wxBitmap(const wxIcon& icon),
        "Create a new bitmap from an Icon object.",
        BitmapFromIcon);

    DocCtorStrName(
        wxBitmap(const wxImage& image, int depth=-1),
        "Creates bitmap object from the image. This has to be done to actually display\n"
        "an image as you cannot draw an image directly on a window. The resulting\n"
        "bitmap will use the provided colour depth (or that of the current system if\n"
        "depth is -1) which entails that a colour reduction has to take place.",
        BitmapFromImage);

    
    %extend {
        DocStr(wxBitmap(PyObject* listOfStrings),
               "Construct a Bitmap from a list of strings formatted as XPM data.");
        %name(BitmapFromXPMData) wxBitmap(PyObject* listOfStrings) {
            char**    cArray = NULL;
            wxBitmap* bmp;

            cArray = ConvertListOfStrings(listOfStrings);
            if (! cArray)
                return NULL;
            bmp = new wxBitmap(cArray);
            delete [] cArray;
            return bmp;
        }

        DocStr(wxBitmap(PyObject* bits, int width, int height, int depth=1 ),
               "Creates a bitmap from an array of bits.  You should only use this function for\n"
               "monochrome bitmaps (depth 1) in portable programs: in this case the bits\n"
               "parameter should contain an XBM image.  For other bit depths, the behaviour is\n"
               "platform dependent.");
        %name(BitmapFromBits) wxBitmap(PyObject* bits, int width, int height, int depth=1 ) {
            char* buf;
            int   length;
            PyString_AsStringAndSize(bits, &buf, &length);
            return new wxBitmap(buf, width, height, depth);
        }
    }    

    
#ifdef __WXMSW__
    void SetPalette(wxPalette& palette);
#endif

    // wxGDIImage methods
#ifdef __WXMSW__
    long GetHandle();
    void SetHandle(long handle);
#endif

    bool Ok();
    
    DocStr(GetWidth, "Gets the width of the bitmap in pixels.");
    int GetWidth();

    DocStr(GetHeight, "Gets the height of the bitmap in pixels.");
    int GetHeight();

    DocStr(GetDepth,
           "Gets the colour depth of the bitmap. A value of 1 indicates a\n"
           "monochrome bitmap.");
    int GetDepth();

    DocStr(ConvertToImage,
           "Creates a platform-independent image from a platform-dependent bitmap. This\n"
           "preserves mask information so that bitmaps and images can be converted back\n"
           "and forth without loss in that respect.");
    virtual wxImage ConvertToImage() const;

    DocStr(GetMask,
           "Gets the associated mask (if any) which may have been loaded from a file\n"
           "or explpicitly set for the bitmap.");
    virtual wxMask* GetMask() const;

    DocStr(SetMask,
           "Sets the mask for this bitmap.");
    virtual void SetMask(wxMask* mask);
    
    %extend {
        DocStr(SetMaskColour,
               "Create a Mask based on a specified colour in the Bitmap.");
        void SetMaskColour(const wxColour& colour) {
            wxMask *mask = new wxMask(*self, colour);
            self->SetMask(mask);
        }
    }

    DocStr(GetSubBitmap,
           "Returns a sub bitmap of the current one as long as the rect belongs entirely\n"
           "to the bitmap. This function preserves bit depth and mask information.");
    virtual wxBitmap GetSubBitmap(const wxRect& rect) const;

    DocStr(SaveFile, "Saves a bitmap in the named file.");
    virtual bool SaveFile(const wxString &name, wxBitmapType type,
                          wxPalette *palette = (wxPalette *)NULL);

    DocStr(LoadFile, "Loads a bitmap from a file");
    virtual bool LoadFile(const wxString &name, wxBitmapType type);

    
#if wxUSE_PALETTE
    virtual wxPalette *GetPalette() const;
    virtual void SetPalette(const wxPalette& palette);
#endif
    
    
    virtual bool CopyFromIcon(const wxIcon& icon);

    DocStr(SetHeight, "Set the height property (does not affect the bitmap data).")
    virtual void SetHeight(int height);
    
    DocStr(SetWidth, "Set the width property (does not affect the bitmap data).")
    virtual void SetWidth(int width);

    DocStr(SetDepth, "Set the depth property (does not affect the bitmap data).")
    virtual void SetDepth(int depth);

    
#ifdef __WXMSW__
    bool CopyFromCursor(const wxCursor& cursor);
    int GetQuality();
    void SetQuality(int q);
#endif

    %pythoncode { def __nonzero__(self): return self.Ok() }
};


//---------------------------------------------------------------------------

DocStr(wxMask,
       "This class encapsulates a monochrome mask bitmap, where the masked area is\n"
       "black and the unmasked area is white. When associated with a bitmap and drawn\n"
       "in a device context, the unmasked area of the bitmap will be drawn, and the\n"
       "masked area will not be drawn.");

class wxMask : public wxObject {
public:
    DocCtorStr(
        wxMask(const wxBitmap& bitmap),
        "Constructs a mask from a monochrome bitmap.");

    DocCtorStrName(
        wxMask(const wxBitmap& bitmap, const wxColour& colour),
        "Constructs a mask from a bitmap and a colour in that bitmap that indicates the\n"
        "background.",
        MaskColour);
    
    //~wxMask();

};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
