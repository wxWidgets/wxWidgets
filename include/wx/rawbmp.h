///////////////////////////////////////////////////////////////////////////////
// Name:        wx/rawbmp.h
// Purpose:     macros for fast, raw bitmap data access
// Author:      Eric Kidd, Vadim Zeitlin
// Modified by:
// Created:     10.03.03
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RAWBMP_H_BASE_
#define _WX_RAWBMP_H_BASE_

// ----------------------------------------------------------------------------
// Abstract Pixel API
//
// We need to access our raw bitmap data (1) portably and (2) efficiently.
// We do this using a two-dimensional "iteration" interface.  Performance
// is extremely important here: these functions will be called hundreds
// of thousands of times in a row, and even small inefficiencies will
// make applications seem slow.
//
// We can't always rely on inline functions, because not all compilers actually
// bother to inline them unless we crank the optimization levels way up.
// Therefore, we also provide macros to wring maximum speed out of compiler
// unconditionally (e.g. even in debug builds). Of course, if the performance
// isn't absolutely crucial for you you shouldn't be using them but the inline
// functions instead.
// ----------------------------------------------------------------------------

/*
   Usage example:

    wxBitmap bmp;
    wxRawBitmapData data(bitmap);
    if ( !data )
    {
        ... raw access to bitmap data unavailable, do something else ...
        return;
    }

    if ( data.m_width < 20 || data.m_height < 20 )
    {
        ... complain: the bitmap it too small ...
        return;
    }

    wxRawBitmapIterator p(data);

    // we draw a (10, 10)-(20, 20) rect manually using the given r, g, b
    p.Offset(10, 10);

    for ( int y = 0; y < 10; ++y )
    {
        wxRawBitmapIterator rowStart = p;

        for ( int x = 0; x < 10; ++x, ++p )
        {
            p.Red() = r;
            p.Green() = g;
            p.Blue() = b;
        }

        p = rowStart;
        p.OffsetY(1);
    }
 */

// this struct represents a pointer to raw bitmap data
class wxRawBitmapData
{
public:
    // ctor associates this pointer with a bitmap and locks the bitmap for raw
    // access, it will be unlocked only by our dtor and so these objects should
    // normally be only created on the stack, i.e. have limited life-time
    wxRawBitmapData(wxBitmap bmp) : m_bmp(bmp)
    {
        if ( !bmp.GetRawData(this) )
            m_pixels = NULL;
    }

    // we evaluate to true only if we could get access to bitmap data
    // successfully
    operator bool() const { return m_pixels != NULL; }

    // dtor unlocks the bitmap
    ~wxRawBitmapData()
    {
        m_bmp.UngetRawData(this);
    }

    // call this to indicate that we should use the alpha channel
    void UseAlpha() { m_bmp.UseAlpha(); }

    // accessors
    unsigned char *GetPixels() const { return m_pixels; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetByPP() const { return m_bypp; }
    int GetBPP() const { return 8*GetByPP(); }
    int GetRowStride() const { return m_stride; }

// private: -- public because accessed by the macros below but still mustn't be
//             used directly

    // the bitmap we're associated with
    wxBitmap m_bmp;

    // pointer to the start of the data
    unsigned char *m_pixels;

    // the size of the image we address, in pixels
    int m_width,
        m_height;

    // number of bytes (NOT bits) per pixel, including alpha channel if any
    int m_bypp;

    // this parameter is the offset of the start of the (N+1)st row from the
    // Nth one and can be different from m_bypp*width in some cases:
    //  a) the most usual one is to force 32/64 bit alignment of rows
    //  b) another one is for bottom-to-top images where it's negative
    //  c) finally, it could conceivably be 0 for the images with all
    //     lines being identical
    int m_stride;
};

// this is the type for the iterator over raw bitmap data
class wxRawBitmapIterator
{
public:
    // ctors and such
    // --------------

    // we must be associated/initialized with some bitmap data object
    wxRawBitmapIterator(const wxRawBitmapData& data) : m_data(&data)
    {
        m_ptr = m_data->GetPixels();
    }

    // default copy ctor, assignment operator and dtor are ok

    
    // navigation
    // ----------

    // move x pixels to the right and y down
    //
    // note that the rows don't wrap!
    void Offset(int x, int y)
    {
        m_ptr += m_data->GetRowStride()*y + m_data->GetByPP()*x;
    }

    // move x pixels to the right (again, no row wrapping)
    void OffsetX(int x)
    {
        m_ptr += m_data->GetByPP()*x;
    }

    // move y rows to the bottom
    void OffsetY(int y)
    {
        m_ptr += m_data->GetRowStride()*y;
    }

    // go back to (0, 0)
    void Reset()
    {
        m_ptr = m_data->GetPixels();
    }

    // go to the given position
    void MoveTo(int x, int y)
    {
        Reset();
        Offset(x, y);
    }

    // same as OffsetX(1) for convenience
    wxRawBitmapIterator& operator++()
    {
        OffsetX(1);
        return *this;
    }

    // postfix (hence less efficient) version
    wxRawBitmapIterator operator++(int)
    {
        wxRawBitmapIterator p(*this);
        OffsetX(1);
        return p;
    }

    // data access
    // -----------

    // DIBs store data in BGR format, i.e. "little endian" RGB
    enum
    {
#ifdef __WXMSW__
        BLUE, GREEN, RED,
#else // !__WXMSW__
        RED, GREEN, BLUE,
#endif // __WXMSW__/!__WXMSW__
        ALPHA
    };

    // access to invidividual colour components
    unsigned char& Red() { return m_ptr[RED]; }
    unsigned char& Green() { return m_ptr[GREEN]; }
    unsigned char& Blue() { return m_ptr[BLUE]; }
    unsigned char& Alpha() { return m_ptr[ALPHA]; }

    // address the pixel contents directly
    //
    // warning: the format is platform dependent
    wxUint32& Data() { return *(wxUint32 *)m_ptr; }

// private: -- don't access these fields directly, same as as above
    unsigned char *m_ptr;

    const wxRawBitmapData *m_data;
};


// these macros are used to change the current location in the bitmap
// ------------------------------------------------------------------

// move x pixels to the right and y down
//
// note that the rows don't wrap!
#define wxBMP_OFFSET(p, x, y) \
    p.m_ptr += p.m_data->m_stride * (y) + p.m_data->m_bypp * (x)

// move x pixels to the right (again, no row wrapping)
#define wxBMP_OFFSET_X(p, x) p.m_ptr += p.m_data->m_bypp * (x)

// move y rows to the bottom
#define wxBMP_OFFSET_Y(p, y) p.m_ptr += p.m_data->m_stride * (y)



// these macros are used to work with the pixel values
//
// all of them can be used as either lvalues or rvalues.
// ----------------------------------------------------

#define wxBMP_RED(p)         (p.m_ptr[wxRawBitmapIterator::RED])
#define wxBMP_GREEN(p)       (p.m_ptr[wxRawBitmapIterator::GREEN])
#define wxBMP_BLUE(p)        (p.m_ptr[wxRawBitmapIterator::BLUE])

#define wxBMP_ALPHA(p)       (p.m_ptr[wxRawBitmapIterator::ALPHA])

// these macros are most efficient but return the buffer contents in
// platform-specific format, e.g. RGB on all sane platforms and BGR under Win32
#define wxBMP_RGB(p)     *(wxUint32 *)(p.m_ptr)
#define wxBMP_RGBA(p)    *(wxUint32 *)(p.m_ptr)

#endif // _WX_RAWBMP_H_BASE_

