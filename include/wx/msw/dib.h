/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/dib.h
// Purpose:     wxDIB class representing Win32 device independent bitmaps
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.03.03 (replaces the old file with the same name)
// RCS-ID:      $Id$
// Copyright:   (c) 1997-2003 wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_DIB_H_
#define _WX_MSW_DIB_H_

class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxPalette;

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// wxDIB: represents a DIB section
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDIB
{
public:
    // ctors and such
    // --------------

    // create an uninitialized DIB with the given width, height and depth (only
    // 24 and 32 bpp DIBs are currently supported)
    //
    // after using this ctor, GetData() and GetHandle() may be used if IsOk()
    // returns true
    wxDIB(int width, int height, int depth)
        { Init(); (void)Create(width, height, depth); }

    // load a DIB from file (any depth is supoprted here unlike above)
    //
    // as above, use IsOk() to see if the bitmap was loaded successfully
    wxDIB(const wxString& filename)
        { Init(); (void)Load(filename); }

    // same as the corresponding ctors but with return value
    bool Create(int width, int height, int depth);
    bool Load(const wxString& filename);

    // dtor is not virtual, this class is not meant to be used polymorphically
    ~wxDIB();


    // operations
    // ----------

    // create a bitmap compatiblr with the given HDC (or screen by default) and
    // return its handle, the caller is responsible for freeing it (using
    // DeleteObject())
    HBITMAP CreateDDB(HDC hdc = NULL) const;

    // get the handle from the DIB and reset it, i.e. this object won't destroy
    // the DIB after this (but the caller should do it)
    HBITMAP Detach() { HBITMAP hbmp = m_handle; m_handle = 0; return hbmp; }

#if wxUSE_PALETTE
    // create a palette for this DIB (always a trivial/default one for 24bpp)
    wxPalette *CreatePalette() const;
#endif // wxUSE_PALETTE


    // accessors
    // ---------

    // return true if DIB was successfully created, false otherwise
    bool IsOk() const { return m_handle != 0; }

    // get the bitmap size
    wxSize GetSize() const { DoGetObject(); return wxSize(m_width, m_height); }
    int GetWidth() const { DoGetObject(); return m_width; }
    int GetHeight() const { DoGetObject(); return m_height; }

    // get the number of bits per pixel, or depth
    int GetDepth() const { DoGetObject(); return m_depth; }

    // get the DIB handle
    HBITMAP GetHandle() const { return m_handle; }

    // get raw pointer to bitmap bits, you should know what you do if you
    // decide to use it
    void *GetData() const { DoGetObject(); return m_data; }


    // wxImage conversion
    // ------------------

#if wxUSE_IMAGE
    // create a DIB from the given image, the DIB will be either 24 or 32 (if
    // the image has alpha channel) bpp
    wxDIB(const wxImage& image) { Init(); (void)Create(image); }

    // same as the above ctor but with the return code
    bool Create(const wxImage& image);

    // create wxImage having the same data as this DIB
    wxImage ConvertToImage() const;
#endif // wxUSE_IMAGE


    // helper functions
    // ----------------

    // return the size of one line in a DIB with given width and depth: the
    // point here is that as the scan lines need to be DWORD aligned so we may
    // need to add some padding
    static unsigned long GetLineSize(int width, int depth)
    {
        return ((width*depth + 31) & ~31) >> 3;
    }

private:
    // common part of all ctors
    void Init()
    {
        m_handle = 0;

        m_data = NULL;

        m_width =
        m_height =
        m_depth = 0;
    }

    // free resources
    void Free()
    {
        if ( m_handle )
        {
            if ( !::DeleteObject(m_handle) )
            {
                wxLogLastError(wxT("DeleteObject(hDIB)"));
            }

            Init();
        }
    }

    // the DIB section handle, 0 if invalid
    HBITMAP m_handle;

    // NB: we could store only m_handle and not any of the other fields as
    //     we may always retrieve them from it using ::GetObject(), but we
    //     decide to still store them for efficiency concerns -- however if we
    //     don't have them from the very beginning (e.g. DIB constructed from a
    //     bitmap), we only retrieve them when necessary and so these fields
    //     should *never* be accessed directly, even from inside wxDIB code

    // function which must be called before accessing any members and which
    // gets their values from m_handle, if not done yet
    void DoGetObject() const;

    // pointer to DIB bits, may be NULL
    void *m_data;

    // size and depth of the image
    int m_width,
        m_height,
        m_depth;


    // DIBs can't be copied
    wxDIB(const wxDIB&);
    wxDIB& operator=(const wxDIB&);
};

// ----------------------------------------------------------------------------
// inline functions implementation
// ----------------------------------------------------------------------------

inline wxDIB::~wxDIB()
{
    Free();
}

// ----------------------------------------------------------------------------
// Functions for working with DIBs
// ----------------------------------------------------------------------------

// WARNING: these functions are private to wxWindows and shouldn't be used
//          by the user code, they risk to disappear in the next versions!

// VZ: we have 3 different sets of functions: from bitmap.cpp (wxCreateDIB and
//     wxFreeDIB), from dib.cpp and from dataobj.cpp - surely there is some
//     redundancy between them? (FIXME)

// defined in bitmap.cpp
extern bool wxCreateDIB(long xSize, long ySize, long bitsPerPixel,
                       HPALETTE hPal, LPBITMAPINFO* lpDIBHeader);
extern void wxFreeDIB(LPBITMAPINFO lpDIBHeader);

// defined in ole/dataobj.cpp
extern WXDLLEXPORT size_t wxConvertBitmapToDIB(LPBITMAPINFO pbi, const wxBitmap& bitmap);
extern WXDLLEXPORT wxBitmap wxConvertDIBToBitmap(const LPBITMAPINFO pbi);

// the rest is defined in dib.cpp

// Save (device dependent) wxBitmap as a DIB
bool wxSaveBitmap(wxChar *filename, wxBitmap *bitmap, wxPalette *palette = NULL);

// Load device independent bitmap into device dependent bitmap
wxBitmap *wxLoadBitmap(wxChar *filename, wxPalette **palette = NULL);

// Load into existing bitmap;
bool wxLoadIntoBitmap(wxChar *filename, wxBitmap *bitmap, wxPalette **pal = NULL);

HANDLE wxBitmapToDIB (HBITMAP hBitmap, HPALETTE hPal);
bool   wxReadDIB(LPTSTR lpFileName, HBITMAP *bitmap, HPALETTE *palette);
HANDLE wxReadDIB2(LPTSTR lpFileName);
LPSTR wxFindDIBBits (LPSTR lpbi);
HPALETTE wxMakeDIBPalette(LPBITMAPINFOHEADER lpInfo);

#endif // _WX_MSW_DIB_H_

