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
    wxBitmap(const wxString& name, wxBitmapType type=wxBITMAP_TYPE_ANY);
    ~wxBitmap();

    // alternate constructors
    %name(EmptyBitmap) wxBitmap(int width, int height, int depth=-1);
    %name(BitmapFromIcon) wxBitmap(const wxIcon& icon);
    %name(BitmapFromImage) wxBitmap(const wxImage& image, int depth=-1);
    %extend {
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

        %name(BitmapFromBits) wxBitmap(PyObject* bits, int width, int height, int depth = 1 ) {
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
    
    int GetWidth();
    int GetHeight();
    int GetDepth();

    virtual wxImage ConvertToImage() const;

    virtual wxMask* GetMask() const;
    virtual void SetMask(wxMask* mask);
    %extend {
        void SetMaskColour(const wxColour& colour) {
            wxMask *mask = new wxMask(*self, colour);
            self->SetMask(mask);
        }
    }
//     def SetMaskColour(self, colour):
//         mask = wxMaskColour(self, colour)
//         self.SetMask(mask)

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const;

    virtual bool SaveFile(const wxString &name, wxBitmapType type,
                          wxPalette *palette = (wxPalette *)NULL);
    virtual bool LoadFile(const wxString &name, wxBitmapType type);

    
#if wxUSE_PALETTE
    virtual wxPalette *GetPalette() const;
    virtual void SetPalette(const wxPalette& palette);
#endif
    
    // copies the contents and mask of the given (colour) icon to the bitmap
    virtual bool CopyFromIcon(const wxIcon& icon);
    
    virtual void SetHeight(int height);
    virtual void SetWidth(int width);
    virtual void SetDepth(int depth);

#ifdef __WXMSW__
    bool CopyFromCursor(const wxCursor& cursor);
    int GetQuality();
    void SetQuality(int q);
#endif

    %pythoncode { def __nonzero__(self): return self.Ok() }
};


//---------------------------------------------------------------------------

class wxMask : public wxObject {
public:
    wxMask(const wxBitmap& bitmap);
    %name(MaskColour) wxMask(const wxBitmap& bitmap, const wxColour& colour);
    
    //~wxMask();

};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
