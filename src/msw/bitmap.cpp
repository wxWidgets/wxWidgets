////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "bitmap.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <stdio.h>

    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/palette.h"
    #include "wx/dcmemory.h"
    #include "wx/bitmap.h"
    #include "wx/icon.h"
#endif

#include "wx/msw/private.h"
#include "wx/log.h"

#if !defined(__WXMICROWIN__)
#include "wx/msw/dib.h"
#endif

#include "wx/image.h"
#include "wx/xpmdecod.h"

// missing from mingw32 header
#ifndef CLR_INVALID
    #define CLR_INVALID ((COLORREF)-1)
#endif // no CLR_INVALID

// ----------------------------------------------------------------------------
// Bitmap data
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxBitmapRefData : public wxGDIImageRefData
{
public:
    wxBitmapRefData();
    virtual ~wxBitmapRefData() { Free(); }

    virtual void Free();

    // set the mask object to use as the mask, we take ownership of it
    void SetMask(wxMask *mask)
    {
        delete m_bitmapMask;
        m_bitmapMask = mask;
    }

    // set the HBITMAP to use as the mask
    void SetMask(HBITMAP hbmpMask)
    {
        SetMask(new wxMask((WXHBITMAP)hbmpMask));
    }

    // return the mask
    wxMask *GetMask() const { return m_bitmapMask; }

public:
    int           m_numColors;
#if wxUSE_PALETTE
    wxPalette     m_bitmapPalette;
#endif // wxUSE_PALETTE

    // MSW-specific
    // ------------

    // this field is solely for error checking: we detect selecting a bitmap
    // into more than one DC at once or deleting a bitmap still selected into a
    // DC (both are serious programming errors under Windows)
    wxDC         *m_selectedInto;

#if wxUSE_DIB_FOR_BITMAP
    // file mapping handle for large DIB's
    HANDLE        m_hFileMap;
#endif // wxUSE_DIB_FOR_BITMAP

private:
    // optional mask for transparent drawing
    wxMask       *m_bitmapMask;

    DECLARE_NO_COPY_CLASS(wxBitmapRefData)
};

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler, wxObject)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxBitmapRefData
// ----------------------------------------------------------------------------

wxBitmapRefData::wxBitmapRefData()
{
    m_selectedInto = NULL;
    m_numColors = 0;
    m_bitmapMask = NULL;
    m_hBitmap = (WXHBITMAP) NULL;
#if wxUSE_DIB_FOR_BITMAP
    m_hFileMap = 0;
#endif // wxUSE_DIB_FOR_BITMAP
}

void wxBitmapRefData::Free()
{
    wxASSERT_MSG( !m_selectedInto,
                  wxT("deleting bitmap still selected into wxMemoryDC") );

    if ( m_hBitmap)
    {
        if ( !::DeleteObject((HBITMAP)m_hBitmap) )
        {
            wxLogLastError(wxT("DeleteObject(hbitmap)"));
        }
    }

#if wxUSE_DIB_FOR_BITMAP
    if ( m_hFileMap )
    {
      ::CloseHandle(m_hFileMap);

      m_hFileMap = 0;
    }
#endif // wxUSE_DIB_FOR_BITMAP

    delete m_bitmapMask;
    m_bitmapMask = NULL;
}

// ----------------------------------------------------------------------------
// wxBitmap creation
// ----------------------------------------------------------------------------

// this function should be called from all wxBitmap ctors
void wxBitmap::Init()
{
    // m_refData = NULL; done in the base class ctor

}

wxGDIImageRefData *wxBitmap::CreateData() const
{
    return new wxBitmapRefData;
}

#ifdef __WIN32__

bool wxBitmap::CopyFromIconOrCursor(const wxGDIImage& icon)
{
#ifndef __WXMICROWIN__
    // it may be either HICON or HCURSOR
    HICON hicon = (HICON)icon.GetHandle();

    ICONINFO iconInfo;
    if ( !::GetIconInfo(hicon, &iconInfo) )
    {
        wxLogLastError(wxT("GetIconInfo"));

        return FALSE;
    }

    wxBitmapRefData *refData = new wxBitmapRefData;
    m_refData = refData;

    int w = icon.GetWidth(),
        h = icon.GetHeight();

    refData->m_width = w;
    refData->m_height = h;
    refData->m_depth = wxDisplayDepth();

    refData->m_hBitmap = (WXHBITMAP)iconInfo.hbmColor;

    // the mask returned by GetIconInfo() is inversed compared to the usual
    // wxWin convention
    refData->SetMask(wxInvertMask(iconInfo.hbmMask, w, h));


    // delete the old one now as we don't need it any more
    ::DeleteObject(iconInfo.hbmMask);

#if WXWIN_COMPATIBILITY_2
    refData->m_ok = TRUE;
#endif // WXWIN_COMPATIBILITY_2

    return TRUE;
#else
    return FALSE;
#endif
}

#endif // Win32

bool wxBitmap::CopyFromCursor(const wxCursor& cursor)
{
    UnRef();

    if ( !cursor.Ok() )
        return FALSE;

#ifdef __WIN16__
    wxFAIL_MSG( _T("don't know how to convert cursor to bitmap") );

    return FALSE;
#else
    return CopyFromIconOrCursor(cursor);
#endif // Win16
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    UnRef();

    if ( !icon.Ok() )
        return FALSE;

    // GetIconInfo() doesn't exist under Win16 and I don't know any other way
    // to create a bitmap from icon there - but using this way we won't have
    // the mask (FIXME)
#ifdef __WIN16__
    int width = icon.GetWidth(),
        height = icon.GetHeight();

    // copy the icon to the bitmap
    ScreenHDC hdcScreen;
    HDC hdc = ::CreateCompatibleDC(hdcScreen);
    HBITMAP hbitmap = ::CreateCompatibleBitmap(hdcScreen, width, height);
    HBITMAP hbmpOld = (HBITMAP)::SelectObject(hdc, hbitmap);

    ::DrawIcon(hdc, 0, 0, GetHiconOf(icon));

    ::SelectObject(hdc, hbmpOld);
    ::DeleteDC(hdc);

    wxBitmapRefData *refData = new wxBitmapRefData;
    m_refData = refData;

    refData->m_width = width;
    refData->m_height = height;
    refData->m_depth = wxDisplayDepth();

    refData->m_hBitmap = (WXHBITMAP)hbitmap;

#if WXWIN_COMPATIBILITY_2
    refData->m_ok = TRUE;
#endif // WXWIN_COMPATIBILITY_2

    return TRUE;
#else // Win32
    return CopyFromIconOrCursor(icon);
#endif // Win16/Win32
}

wxBitmap::~wxBitmap()
{
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    Init();

#ifndef __WXMICROWIN__
    wxBitmapRefData *refData = new wxBitmapRefData;
    m_refData = refData;

    refData->m_width = width;
    refData->m_height = height;
    refData->m_depth = depth;
    refData->m_numColors = 0;
    refData->m_selectedInto = NULL;

    char *data;
    if ( depth == 1 )
    {
        // we assume that it is in XBM format which is not quite the same as
        // the format CreateBitmap() wants because the order of bytes in the
        // line is inversed!
        const size_t bytesPerLine = (width + 7) / 8;
        const size_t padding = bytesPerLine % 2;
        const size_t len = height * ( padding + bytesPerLine );
        data = (char *)malloc(len);
        const char *src = bits;
        char *dst = data;

        for ( int rows = 0; rows < height; rows++ )
        {
            for ( size_t cols = 0; cols < bytesPerLine; cols++ )
            {
                unsigned char val = *src++;
                unsigned char reversed = 0;

                for ( int bits = 0; bits < 8; bits++)
                {
                    reversed <<= 1;
                    reversed |= (val & 0x01);
                    val >>= 1;
                }
                *dst++ = reversed;
            }

            if ( padding )
                *dst++ = 0;
        }
    }
    else
    {
        // bits should already be in Windows standard format
        data = (char *)bits;    // const_cast is harmless
    }

    HBITMAP hbmp = ::CreateBitmap(width, height, 1, depth, data);
    if ( !hbmp )
    {
        wxLogLastError(wxT("CreateBitmap"));
    }

    if ( data != bits )
    {
        free(data);
    }

    SetHBITMAP((WXHBITMAP)hbmp);
#endif
}

// Create from XPM data
bool wxBitmap::CreateFromXpm(const char **data)
{
#if wxUSE_IMAGE && wxUSE_XPM
    Init();

    wxCHECK_MSG( data != NULL, FALSE, wxT("invalid bitmap data") )

    wxXPMDecoder decoder;
    wxImage img = decoder.ReadData(data);
    wxCHECK_MSG( img.Ok(), FALSE, wxT("invalid bitmap data") )

    *this = wxBitmap(img);
    return TRUE;
#else
    return FALSE;
#endif
}

wxBitmap::wxBitmap(int w, int h, int d)
{
    Init();

    (void)Create(w, h, d);
}

wxBitmap::wxBitmap(void *data, long type, int width, int height, int depth)
{
    Init();

    (void)Create(data, type, width, height, depth);
}

wxBitmap::wxBitmap(const wxString& filename, wxBitmapType type)
{
    Init();

    LoadFile(filename, (int)type);
}

bool wxBitmap::Create(int w, int h, int d)
{
    UnRef();

    m_refData = new wxBitmapRefData;

#if wxUSE_DIB_FOR_BITMAP
    if ( w && h && d >= 16 )
    {
        if ( !CreateDIB(w, h, d) )
           return FALSE;
    }
    else
#endif // wxUSE_DIB_FOR_BITMAP
    {
        GetBitmapData()->m_width = w;
        GetBitmapData()->m_height = h;
        GetBitmapData()->m_depth = d;

        HBITMAP hbmp;
#ifndef __WXMICROWIN__
        if ( d > 0 )
        {
            hbmp = ::CreateBitmap(w, h, 1, d, NULL);
            if ( !hbmp )
            {
                wxLogLastError(wxT("CreateBitmap"));
            }
        }
        else
#endif // !__WXMICROWIN__
        {
            ScreenHDC dc;
            hbmp = ::CreateCompatibleBitmap(dc, w, h);
            if ( !hbmp )
            {
                wxLogLastError(wxT("CreateCompatibleBitmap"));
            }

            GetBitmapData()->m_depth = wxDisplayDepth();
        }

        SetHBITMAP((WXHBITMAP)hbmp);

#if WXWIN_COMPATIBILITY_2
        GetBitmapData()->m_ok = hbmp != 0;
#endif // WXWIN_COMPATIBILITY_2
    }

    return Ok();
}

#if wxUSE_DIB_FOR_BITMAP

void *wxBitmap::CreateDIB(int width, int height, int depth)
{
    void *dibBits;
    const int infosize = sizeof(BITMAPINFOHEADER);

    BITMAPINFO *info = (BITMAPINFO *)malloc(infosize);
    if ( info )
    {
        memset(info, 0, infosize);

        info->bmiHeader.biSize = infosize;
        info->bmiHeader.biWidth = width;
        info->bmiHeader.biHeight = height;
        info->bmiHeader.biPlanes = 1;
        info->bmiHeader.biBitCount = depth;
        info->bmiHeader.biCompression = BI_RGB;
        info->bmiHeader.biSizeImage =
            (((width * (depth/8)) + sizeof(DWORD) - 1) /
                sizeof(DWORD) * sizeof(DWORD)) * height;
        info->bmiHeader.biXPelsPerMeter = 0;
        info->bmiHeader.biYPelsPerMeter = 0;
        info->bmiHeader.biClrUsed = 0;
        info->bmiHeader.biClrImportant = 0;
        GetBitmapData()->m_hFileMap = ::CreateFileMapping
                                        (
                                            INVALID_HANDLE_VALUE,
                                            0,
                                            PAGE_READWRITE | SEC_COMMIT,
                                            0,
                                            info->bmiHeader.biSizeImage,
                                            0
                                        );

        // No need to report an error here.  If it fails, we just won't use a
        // file mapping and CreateDIBSection will just allocate memory for us.
        GetBitmapData()->m_handle =
            (WXHANDLE)::CreateDIBSection
                        (
                            0,
                            info,
                            DIB_RGB_COLORS,
                            &dibBits,
                            GetBitmapData()->m_hFileMap,
                            0
                        );

        if ( !GetBitmapData()->m_handle )
          wxLogLastError(wxT("CreateDIBSection"));

        SetWidth(width);
        SetHeight(height);
        SetDepth(depth);

        free(info);
    }
    else
    {
        wxFAIL_MSG( wxT("could not allocate memory for DIB header") );

        dibBits = NULL;
    }

    return dibBits;
}

#endif // wxUSE_DIB_FOR_BITMAP

// ----------------------------------------------------------------------------
// wxImage to/from conversions
// ----------------------------------------------------------------------------

#if wxUSE_IMAGE

bool wxBitmap::CreateFromImage( const wxImage& image, int depth )
{
#ifdef __WXMICROWIN__

  // Set this to 1 to experiment with mask code,
  // which currently doesn't work
#define USE_MASKS 0

    m_refData = new wxBitmapRefData();

    // Initial attempt at a simple-minded implementation.
    // The bitmap will always be created at the screen depth,
    // so the 'depth' argument is ignored.

    HDC hScreenDC = ::GetDC(NULL);
    int screenDepth = ::GetDeviceCaps(hScreenDC, BITSPIXEL);

    HBITMAP hBitmap = ::CreateCompatibleBitmap(hScreenDC, image.GetWidth(), image.GetHeight());
    HBITMAP hMaskBitmap = NULL;
    HBITMAP hOldMaskBitmap = NULL;
    HDC hMaskDC = NULL;
    unsigned char maskR = 0;
    unsigned char maskG = 0;
    unsigned char maskB = 0;

    //    printf("Created bitmap %d\n", (int) hBitmap);
    if (hBitmap == NULL)
    {
        ::ReleaseDC(NULL, hScreenDC);
        return FALSE;
    }
    HDC hMemDC = ::CreateCompatibleDC(hScreenDC);

    HBITMAP hOldBitmap = ::SelectObject(hMemDC, hBitmap);
    ::ReleaseDC(NULL, hScreenDC);

    // created an mono-bitmap for the possible mask
    bool hasMask = image.HasMask();

    if ( hasMask )
    {
#if USE_MASKS
        // FIXME: we should be able to pass bpp = 1, but
        // GdBlit can't handle a different depth
#if 0
        hMaskBitmap = ::CreateBitmap( (WORD)image.GetWidth(), (WORD)image.GetHeight(), 1, 1, NULL );
#else
        hMaskBitmap = ::CreateCompatibleBitmap( hMemDC, (WORD)image.GetWidth(), (WORD)image.GetHeight());
#endif
        maskR = image.GetMaskRed();
        maskG = image.GetMaskGreen();
        maskB = image.GetMaskBlue();

        if (!hMaskBitmap)
        {
            hasMask = FALSE;
        }
        else
        {
            hScreenDC = ::GetDC(NULL);
            hMaskDC = ::CreateCompatibleDC(hScreenDC);
           ::ReleaseDC(NULL, hScreenDC);

            hOldMaskBitmap = ::SelectObject( hMaskDC, hMaskBitmap);
        }
#else
        hasMask = FALSE;
#endif
    }

    int i, j;
    for (i = 0; i < image.GetWidth(); i++)
    {
        for (j = 0; j < image.GetHeight(); j++)
        {
            unsigned char red = image.GetRed(i, j);
            unsigned char green = image.GetGreen(i, j);
            unsigned char blue = image.GetBlue(i, j);

            ::SetPixel(hMemDC, i, j, PALETTERGB(red, green, blue));

            if (hasMask)
            {
                // scan the bitmap for the transparent colour and set the corresponding
                // pixels in the mask to BLACK and the rest to WHITE
                if (maskR == red && maskG == green && maskB == blue)
                    ::SetPixel(hMaskDC, i, j, PALETTERGB(0, 0, 0));
                else
                    ::SetPixel(hMaskDC, i, j, PALETTERGB(255, 255, 255));
            }
        }
    }

    ::SelectObject(hMemDC, hOldBitmap);
    ::DeleteDC(hMemDC);
    if (hasMask)
    {
        ::SelectObject(hMaskDC, hOldMaskBitmap);
        ::DeleteDC(hMaskDC);

        ((wxBitmapRefData*)m_refData)->SetMask(hMaskBitmap);
    }

    SetWidth(image.GetWidth());
    SetHeight(image.GetHeight());
    SetDepth(screenDepth);
    SetHBITMAP( (WXHBITMAP) hBitmap );

#if wxUSE_PALETTE
    // Copy the palette from the source image
    SetPalette(image.GetPalette());
#endif // wxUSE_PALETTE

#if WXWIN_COMPATIBILITY_2
    // check the wxBitmap object
    GetBitmapData()->SetOk();
#endif // WXWIN_COMPATIBILITY_2

    return TRUE;

#else // !__WXMICROWIN__
    wxCHECK_MSG( image.Ok(), FALSE, wxT("invalid image") )

    m_refData = new wxBitmapRefData();

#if wxUSE_DIB_FOR_BITMAP
    int h = image.GetHeight();
    int w = image.GetWidth();
    unsigned char *dibBits = (unsigned char*)CreateDIB(w, h, 24);
    if ( !dibBits )
        return FALSE;

    // DIBs are stored in bottom to top order so we need to copy bits line by
    // line and starting from the end
    const int srcBytesPerLine = w * 3;
    const int dstBytesPerLine = (srcBytesPerLine + sizeof(DWORD) - 1) /
                                    sizeof(DWORD) * sizeof(DWORD);
    const unsigned char *src = image.GetData() + ((h - 1) * srcBytesPerLine);
    for ( int i = 0; i < h; i++ )
    {
        // copy one DIB line
        int x = w;
        const unsigned char *rgbBits = src;
        while ( x-- )
        {
            // also, the order of RGB is inversed for DIBs
            *dibBits++ = rgbBits[2];
            *dibBits++ = rgbBits[1];
            *dibBits++ = rgbBits[0];

            rgbBits += 3;
        }

        // pass to the next line
        src -= srcBytesPerLine;
        dibBits += dstBytesPerLine - srcBytesPerLine;
    }

    if ( image.HasMask() )
    {
        SetMask(new wxMask(*this, wxColour(image.GetMaskRed(),
                                           image.GetMaskGreen(),
                                           image.GetMaskBlue())));
    }
#else // wxUSE_DIB_FOR_BITMAP
    // sizeLimit is the MS upper limit for the DIB size
#ifdef  WIN32
    int sizeLimit = 1024*768*3;
#else
    int sizeLimit = 0x7fff;
#endif

    // width and height of the device-dependent bitmap
    int width = image.GetWidth();
    int bmpHeight = image.GetHeight();

    // calc the number of bytes per scanline and padding
    int bytePerLine = width*3;
    int sizeDWORD = sizeof( DWORD );
    int lineBoundary = bytePerLine % sizeDWORD;
    int padding = 0;
    if( lineBoundary > 0 )
    {
        padding = sizeDWORD - lineBoundary;
        bytePerLine += padding;
    }
    // calc the number of DIBs and heights of DIBs
    int numDIB = 1;
    int hRemain = 0;
    int height = sizeLimit/bytePerLine;
    if( height >= bmpHeight )
        height = bmpHeight;
    else
    {
        numDIB =  bmpHeight / height;
        hRemain = bmpHeight % height;
        if( hRemain >0 )  numDIB++;
    }

    // set bitmap parameters
    wxCHECK_MSG( image.Ok(), FALSE, wxT("invalid image") );
    SetWidth( width );
    SetHeight( bmpHeight );
    if (depth == -1) depth = wxDisplayDepth();
    SetDepth( depth );

#if wxUSE_PALETTE
    // Copy the palette from the source image
    SetPalette(image.GetPalette());
#endif // wxUSE_PALETTE

    // create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    BITMAPINFO *lpDIBh = (BITMAPINFO *) malloc( headersize );
    wxCHECK_MSG( lpDIBh, FALSE, wxT("could not allocate memory for DIB header") );
    // Fill in the DIB header
    lpDIBh->bmiHeader.biSize = headersize;
    lpDIBh->bmiHeader.biWidth = (DWORD)width;
    lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
    lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
    //   the general formula for biSizeImage:
    //      ( ( ( ((DWORD)width*24) +31 ) & ~31 ) >> 3 ) * height;
    lpDIBh->bmiHeader.biPlanes = 1;
    lpDIBh->bmiHeader.biBitCount = 24;
    lpDIBh->bmiHeader.biCompression = BI_RGB;
    lpDIBh->bmiHeader.biClrUsed = 0;
    // These seem not really needed for our purpose here.
    lpDIBh->bmiHeader.biClrImportant = 0;
    lpDIBh->bmiHeader.biXPelsPerMeter = 0;
    lpDIBh->bmiHeader.biYPelsPerMeter = 0;
    // memory for DIB data
    unsigned char *lpBits;
    lpBits = (unsigned char *)malloc( lpDIBh->bmiHeader.biSizeImage );
    if( !lpBits )
    {
        wxFAIL_MSG( wxT("could not allocate memory for DIB") );
        free( lpDIBh );
        return FALSE;
    }

    // create and set the device-dependent bitmap
    HDC hdc = ::GetDC(NULL);
    HDC memdc = ::CreateCompatibleDC( hdc );
    HBITMAP hbitmap;
    hbitmap = ::CreateCompatibleBitmap( hdc, width, bmpHeight );
    ::SelectObject( memdc, hbitmap);

#if wxUSE_PALETTE
    HPALETTE hOldPalette = 0;
    if (image.GetPalette().Ok())
    {
        hOldPalette = ::SelectPalette(memdc, (HPALETTE) image.GetPalette().GetHPALETTE(), FALSE);
        ::RealizePalette(memdc);
    }
#endif // wxUSE_PALETTE

    // copy image data into DIB data and then into DDB (in a loop)
    unsigned char *data = image.GetData();
    int i, j, n;
    int origin = 0;
    unsigned char *ptdata = data;
    unsigned char *ptbits;

    for( n=0; n<numDIB; n++ )
    {
        if( numDIB > 1 && n == numDIB-1 && hRemain > 0 )
        {
            // redefine height and size of the (possibly) last smaller DIB
            // memory is not reallocated
            height = hRemain;
            lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
            lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
        }
        ptbits = lpBits;

        for( j=0; j<height; j++ )
        {
            for( i=0; i<width; i++ )
            {
                *(ptbits++) = *(ptdata+2);
                *(ptbits++) = *(ptdata+1);
                *(ptbits++) = *(ptdata  );
                ptdata += 3;
            }
            for( i=0; i< padding; i++ )   *(ptbits++) = 0;
        }
        ::StretchDIBits( memdc, 0, origin, width, height,\
            0, 0, width, height, lpBits, lpDIBh, DIB_RGB_COLORS, SRCCOPY);
        origin += height;
        // if numDIB = 1,  lines below can also be used
        //    hbitmap = CreateDIBitmap( hdc, &(lpDIBh->bmiHeader), CBM_INIT, lpBits, lpDIBh, DIB_RGB_COLORS );
        // The above line is equivalent to the following two lines.
        //    hbitmap = ::CreateCompatibleBitmap( hdc, width, height );
        //    ::SetDIBits( hdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS);
        // or the following lines
        //    hbitmap = ::CreateCompatibleBitmap( hdc, width, height );
        //    HDC memdc = ::CreateCompatibleDC( hdc );
        //    ::SelectObject( memdc, hbitmap);
        //    ::SetDIBitsToDevice( memdc, 0, 0, width, height,
        //            0, 0, 0, height, (void *)lpBits, lpDIBh, DIB_RGB_COLORS);
        //    ::SelectObject( memdc, 0 );
        //    ::DeleteDC( memdc );
    }
    SetHBITMAP( (WXHBITMAP) hbitmap );

#if wxUSE_PALETTE
    if (hOldPalette)
        SelectPalette(memdc, hOldPalette, FALSE);
#endif // wxUSE_PALETTE

    // similarly, created an mono-bitmap for the possible mask
    if( image.HasMask() )
    {
        hbitmap = ::CreateBitmap( (WORD)width, (WORD)bmpHeight, 1, 1, NULL );
        HGDIOBJ hbmpOld = ::SelectObject( memdc, hbitmap);
        if( numDIB == 1 )   height = bmpHeight;
        else                height = sizeLimit/bytePerLine;
        lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
        lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
        origin = 0;
        unsigned char r = image.GetMaskRed();
        unsigned char g = image.GetMaskGreen();
        unsigned char b = image.GetMaskBlue();
        unsigned char zero = 0, one = 255;
        ptdata = data;
        for( n=0; n<numDIB; n++ )
        {
            if( numDIB > 1 && n == numDIB - 1 && hRemain > 0 )
            {
                // redefine height and size of the (possibly) last smaller DIB
                // memory is not reallocated
                height = hRemain;
                lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
                lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
            }
            ptbits = lpBits;
            for( int j=0; j<height; j++ )
            {
                for(i=0; i<width; i++ )
                {
                    // was causing a code gen bug in cw : if( ( cr !=r) || (cg!=g) || (cb!=b) )
                    unsigned char cr = (*(ptdata++)) ;
                    unsigned char cg = (*(ptdata++)) ;
                    unsigned char cb = (*(ptdata++)) ;

                    if( ( cr !=r) || (cg!=g) || (cb!=b) )
                    {
                        *(ptbits++) = one;
                        *(ptbits++) = one;
                        *(ptbits++) = one;
                    }
                    else
                    {
                        *(ptbits++) = zero;
                        *(ptbits++) = zero;
                        *(ptbits++) = zero;
                    }
                }
                for( i=0; i< padding; i++ )   *(ptbits++) = zero;
            }
            ::StretchDIBits( memdc, 0, origin, width, height,\
                0, 0, width, height, lpBits, lpDIBh, DIB_RGB_COLORS, SRCCOPY);
            origin += height;
        }
        // create a wxMask object
        wxMask *mask = new wxMask();
        mask->SetMaskBitmap( (WXHBITMAP) hbitmap );
        SetMask( mask );
        // It will be deleted when the wxBitmap object is deleted (as of 01/1999)
        /* The following can also be used but is slow to run
        wxColour colour( GetMaskRed(), GetMaskGreen(), GetMaskBlue());
        wxMask *mask = new wxMask( *this, colour );
        SetMask( mask );
        */

        ::SelectObject( memdc, hbmpOld );
    }

    // free allocated resources
    ::DeleteDC( memdc );
    ::ReleaseDC(NULL, hdc);
    free(lpDIBh);
    free(lpBits);
#endif // wxUSE_DIB_FOR_BITMAP

#if WXWIN_COMPATIBILITY_2
    // check the wxBitmap object
    GetBitmapData()->SetOk();
#endif // WXWIN_COMPATIBILITY_2

    return TRUE;
#endif // __WXMICROWIN__/!__WXMICROWIN__
}

wxImage wxBitmap::ConvertToImage() const
{
#ifdef __WXMICROWIN__
    // Initial attempt at a simple-minded implementation.
    // The bitmap will always be created at the screen depth,
    // so the 'depth' argument is ignored.
    // TODO: transparency (create a mask image)

    if (!Ok())
    {
        wxFAIL_MSG( wxT("bitmap is invalid") );
        return wxNullImage;
    }

    wxImage image;

    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    // create an wxImage object
    int width = GetWidth();
    int height = GetHeight();
    image.Create( width, height );
    unsigned char *data = image.GetData();
    if( !data )
    {
        wxFAIL_MSG( wxT("could not allocate data for image") );
        return wxNullImage;
    }

    HDC hScreenDC = ::GetDC(NULL);

    HDC hMemDC = ::CreateCompatibleDC(hScreenDC);
    ::ReleaseDC(NULL, hScreenDC);

    HBITMAP hBitmap = (HBITMAP) GetHBITMAP();

    HBITMAP hOldBitmap = ::SelectObject(hMemDC, hBitmap);

    int i, j;
    for (i = 0; i < GetWidth(); i++)
    {
        for (j = 0; j < GetHeight(); j++)
        {
            COLORREF color = ::GetPixel(hMemDC, i, j);
            unsigned char red = GetRValue(color);
            unsigned char green = GetGValue(color);
            unsigned char blue = GetBValue(color);

            image.SetRGB(i, j, red, green, blue);
        }
    }

    ::SelectObject(hMemDC, hOldBitmap);
    ::DeleteDC(hMemDC);

#if wxUSE_PALETTE
    // Copy the palette from the source image
    if (GetPalette())
        image.SetPalette(* GetPalette());
#endif // wxUSE_PALETTE

    return image;

#else // !__WXMICROWIN__
    wxImage image;

    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    // create an wxImage object
    int width = GetWidth();
    int height = GetHeight();
    image.Create( width, height );
    unsigned char *data = image.GetData();
    if( !data )
    {
        wxFAIL_MSG( wxT("could not allocate data for image") );
        return wxNullImage;
    }

    // calc the number of bytes per scanline and padding in the DIB
    int bytePerLine = width*3;
    int sizeDWORD = sizeof( DWORD );
    int lineBoundary =  bytePerLine % sizeDWORD;
    int padding = 0;
    if( lineBoundary > 0 )
    {
        padding = sizeDWORD - lineBoundary;
        bytePerLine += padding;
    }

    // create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    BITMAPINFO *lpDIBh = (BITMAPINFO *) malloc( headersize );
    if( !lpDIBh )
    {
        wxFAIL_MSG( wxT("could not allocate data for DIB header") );
        free( data );
        return wxNullImage;
    }
    // Fill in the DIB header
    lpDIBh->bmiHeader.biSize = headersize;
    lpDIBh->bmiHeader.biWidth = width;
    lpDIBh->bmiHeader.biHeight = -height;
    lpDIBh->bmiHeader.biSizeImage = bytePerLine * height;
    lpDIBh->bmiHeader.biPlanes = 1;
    lpDIBh->bmiHeader.biBitCount = 24;
    lpDIBh->bmiHeader.biCompression = BI_RGB;
    lpDIBh->bmiHeader.biClrUsed = 0;
    // These seem not really needed for our purpose here.
    lpDIBh->bmiHeader.biClrImportant = 0;
    lpDIBh->bmiHeader.biXPelsPerMeter = 0;
    lpDIBh->bmiHeader.biYPelsPerMeter = 0;
    // memory for DIB data
    unsigned char *lpBits;
    lpBits = (unsigned char *) malloc( lpDIBh->bmiHeader.biSizeImage );
    if( !lpBits )
    {
        wxFAIL_MSG( wxT("could not allocate data for DIB") );
        free( data );
        free( lpDIBh );
        return wxNullImage;
    }

    // copy data from the device-dependent bitmap to the DIB
    HDC hdc = ::GetDC(NULL);
    HBITMAP hbitmap;
    hbitmap = (HBITMAP) GetHBITMAP();
    ::GetDIBits( hdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS );

    // copy DIB data into the wxImage object
    int i, j;
    unsigned char *ptdata = data;
    unsigned char *ptbits = lpBits;
    for( i=0; i<height; i++ )
    {
        for( j=0; j<width; j++ )
        {
            *(ptdata++) = *(ptbits+2);
            *(ptdata++) = *(ptbits+1);
            *(ptdata++) = *(ptbits  );
            ptbits += 3;
        }
        ptbits += padding;
    }

    // similarly, set data according to the possible mask bitmap
    if( GetMask() && GetMask()->GetMaskBitmap() )
    {
        hbitmap = (HBITMAP) GetMask()->GetMaskBitmap();
        // memory DC created, color set, data copied, and memory DC deleted
        HDC memdc = ::CreateCompatibleDC( hdc );
        ::SetTextColor( memdc, RGB( 0, 0, 0 ) );
        ::SetBkColor( memdc, RGB( 255, 255, 255 ) );
        ::GetDIBits( memdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS );
        ::DeleteDC( memdc );
        // background color set to RGB(16,16,16) in consistent with wxGTK
        unsigned char r=16, g=16, b=16;
        ptdata = data;
        ptbits = lpBits;
        for( i=0; i<height; i++ )
        {
            for( j=0; j<width; j++ )
            {
                if( *ptbits != 0 )
                    ptdata += 3;
                else
                {
                    *(ptdata++)  = r;
                    *(ptdata++)  = g;
                    *(ptdata++)  = b;
                }
                ptbits += 3;
            }
            ptbits += padding;
        }
        image.SetMaskColour( r, g, b );
        image.SetMask( TRUE );
    }
    else
    {
        image.SetMask( FALSE );
    }
    // free allocated resources
    ::ReleaseDC(NULL, hdc);
    free(lpDIBh);
    free(lpBits);

    return image;
#endif // __WXMICROWIN__/!__WXMICROWIN__
}

#endif // wxUSE_IMAGE

bool wxBitmap::LoadFile(const wxString& filename, long type)
{
    UnRef();

    wxBitmapHandler *handler = wxDynamicCast(FindHandler(type), wxBitmapHandler);

    if ( handler )
    {
        m_refData = new wxBitmapRefData;

        return handler->LoadFile(this, filename, type, -1, -1);
    }
#if wxUSE_IMAGE
    else
    {
        wxImage image;
        if ( image.LoadFile( filename, type ) && image.Ok() )
        {
            *this = wxBitmap(image);

            return TRUE;
        }
    }
#endif // wxUSE_IMAGE

    return FALSE;
}

bool wxBitmap::Create(void *data, long type, int width, int height, int depth)
{
    UnRef();

    wxBitmapHandler *handler = wxDynamicCast(FindHandler(type), wxBitmapHandler);

    if ( !handler )
    {
        wxLogDebug(wxT("Failed to create bitmap: no bitmap handler for type %ld defined."), type);

        return FALSE;
    }

    m_refData = new wxBitmapRefData;

    return handler->Create(this, data, type, width, height, depth);
}

bool wxBitmap::SaveFile(const wxString& filename,
                        int type,
                        const wxPalette *palette)
{
    wxBitmapHandler *handler = wxDynamicCast(FindHandler(type), wxBitmapHandler);

    if ( handler )
    {
        return handler->SaveFile(this, filename, type, palette);
    }
#if wxUSE_IMAGE
    else
    {
        // FIXME what about palette? shouldn't we use it?
        wxImage image = ConvertToImage();
        if ( image.Ok() )
        {
            return image.SaveFile(filename, type);
        }
    }
#endif // wxUSE_IMAGE

    return FALSE;
}

// ----------------------------------------------------------------------------
// sub bitmap extraction
// ----------------------------------------------------------------------------

wxBitmap wxBitmap::GetSubBitmap( const wxRect& rect) const
{
    wxCHECK_MSG( Ok() &&
                 (rect.x >= 0) && (rect.y >= 0) &&
                 (rect.x+rect.width <= GetWidth()) &&
                 (rect.y+rect.height <= GetHeight()),
                 wxNullBitmap, wxT("Invalid bitmap or bitmap region") );

    wxBitmap ret( rect.width, rect.height, GetDepth() );
    wxASSERT_MSG( ret.Ok(), wxT("GetSubBitmap error") );

#ifndef __WXMICROWIN__
    // copy bitmap data
    MemoryHDC dcSrc, dcDst;

    {
        SelectInHDC selectSrc(dcSrc, GetHbitmap()),
                    selectDst(dcDst, GetHbitmapOf(ret));

        if ( !::BitBlt(dcDst, 0, 0, rect.width, rect.height,
                       dcSrc, rect.x, rect.y, SRCCOPY) )
        {
            wxLogLastError(_T("BitBlt"));
        }
    }

    // copy mask if there is one
    if ( GetMask() )
    {
        HBITMAP hbmpMask = ::CreateBitmap(rect.width, rect.height, 1, 1, 0);

        SelectInHDC selectSrc(dcSrc, (HBITMAP) GetMask()->GetMaskBitmap()),
                    selectDst(dcDst, hbmpMask);

        if ( !::BitBlt(dcDst, 0, 0, rect.width, rect.height,
                       dcSrc, rect.x, rect.y, SRCCOPY) )
        {
            wxLogLastError(_T("BitBlt"));
        }

        wxMask *mask = new wxMask((WXHBITMAP) hbmpMask);
        ret.SetMask(mask);
    }
#endif // !__WXMICROWIN__

    return ret;
}

// ----------------------------------------------------------------------------
// wxBitmap accessors
// ----------------------------------------------------------------------------

wxPalette* wxBitmap::GetPalette() const
{
    return GetBitmapData() ? &GetBitmapData()->m_bitmapPalette
                           : (wxPalette *) NULL;
}

wxMask *wxBitmap::GetMask() const
{
    return GetBitmapData() ? GetBitmapData()->GetMask() : (wxMask *) NULL;
}

wxDC *wxBitmap::GetSelectedInto() const
{
    return GetBitmapData() ? GetBitmapData()->m_selectedInto : (wxDC *) NULL;
}

#if wxUSE_DIB_FOR_BITMAP

bool wxBitmap::IsDIB() const
{
    return GetBitmapData() && GetBitmapData()->m_hFileMap != NULL;
}

#endif // wxUSE_DIB_FOR_BITMAP

#if WXWIN_COMPATIBILITY_2_4

int wxBitmap::GetQuality() const
{
    return 0;
}

#endif // WXWIN_COMPATIBILITY_2_4

// ----------------------------------------------------------------------------
// wxBitmap setters
// ----------------------------------------------------------------------------

void wxBitmap::SetSelectedInto(wxDC *dc)
{
    if ( GetBitmapData() )
        GetBitmapData()->m_selectedInto = dc;
}

#if wxUSE_PALETTE

void wxBitmap::SetPalette(const wxPalette& palette)
{
    EnsureHasData();

    GetBitmapData()->m_bitmapPalette = palette;
}

#endif // wxUSE_PALETTE

void wxBitmap::SetMask(wxMask *mask)
{
    EnsureHasData();

    GetBitmapData()->SetMask(mask);
}

#if WXWIN_COMPATIBILITY_2

void wxBitmap::SetOk(bool isOk)
{
    EnsureHasData();

    GetBitmapData()->m_ok = isOk;
}

#endif // WXWIN_COMPATIBILITY_2

#if WXWIN_COMPATIBILITY_2_4

void wxBitmap::SetQuality(int WXUNUSED(quality))
{
}

#endif // WXWIN_COMPATIBILITY_2_4

// ----------------------------------------------------------------------------
// TODO: to be replaced by something better
// ----------------------------------------------------------------------------

// Creates a bitmap that matches the device context, from
// an arbitray bitmap. At present, the original bitmap must have an
// associated palette. TODO: use a default palette if no palette exists.
// Contributed by Frederic Villeneuve <frederic.villeneuve@natinst.com>
wxBitmap wxBitmap::GetBitmapForDC(wxDC& dc) const
{
#ifdef __WXMICROWIN__
    return *this;
#else
    wxMemoryDC      memDC;
    wxBitmap        tmpBitmap(GetWidth(), GetHeight(), dc.GetDepth());
    HPALETTE        hPal = (HPALETTE) NULL;
    LPBITMAPINFO    lpDib;
    void            *lpBits = (void*) NULL;

#if wxUSE_PALETTE
    if( GetPalette() && GetPalette()->Ok() )
    {
        tmpBitmap.SetPalette(*GetPalette());
        memDC.SelectObject(tmpBitmap);
        memDC.SetPalette(*GetPalette());
        hPal = (HPALETTE)GetPalette()->GetHPALETTE();
    }
    else
    {
        hPal = (HPALETTE) ::GetStockObject(DEFAULT_PALETTE);
        wxPalette palette;
        palette.SetHPALETTE( (WXHPALETTE)hPal );
        tmpBitmap.SetPalette( palette );
        memDC.SelectObject(tmpBitmap);
        memDC.SetPalette( palette );
    }
#else // !wxUSE_PALETTE
    hPal = (HPALETTE) ::GetStockObject(DEFAULT_PALETTE);
#endif // wxUSE_PALETTE/!wxUSE_PALETTE

    // set the height negative because in a DIB the order of the lines is
    // reversed
    if ( !wxCreateDIB(GetWidth(), -GetHeight(), GetDepth(), hPal, &lpDib) )
    {
        return wxNullBitmap;
    }

    lpBits = malloc(lpDib->bmiHeader.biSizeImage);

    ::GetBitmapBits(GetHbitmap(), lpDib->bmiHeader.biSizeImage, lpBits);

    ::SetDIBitsToDevice(GetHdcOf(memDC), 0, 0,
                        GetWidth(), GetHeight(),
                        0, 0, 0, GetHeight(),
                        lpBits, lpDib, DIB_RGB_COLORS);

    free(lpBits);

    wxFreeDIB(lpDib);

    return tmpBitmap;
#endif
}

// ----------------------------------------------------------------------------
// wxMask
// ----------------------------------------------------------------------------

wxMask::wxMask()
{
    m_maskBitmap = 0;
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    m_maskBitmap = 0;
    Create(bitmap, colour);
}

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
    m_maskBitmap = 0;
    Create(bitmap, paletteIndex);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
{
    m_maskBitmap = 0;
    Create(bitmap);
}

wxMask::~wxMask()
{
    if ( m_maskBitmap )
        ::DeleteObject((HBITMAP) m_maskBitmap);
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxBitmap& bitmap)
{
#ifndef __WXMICROWIN__
    wxCHECK_MSG( bitmap.Ok() && bitmap.GetDepth() == 1, FALSE,
                 _T("can't create mask from invalid or not monochrome bitmap") );

    if ( m_maskBitmap )
    {
        ::DeleteObject((HBITMAP) m_maskBitmap);
        m_maskBitmap = 0;
    }

    m_maskBitmap = (WXHBITMAP) CreateBitmap(
                                            bitmap.GetWidth(),
                                            bitmap.GetHeight(),
                                            1, 1, 0
                                           );
    HDC srcDC = CreateCompatibleDC(0);
    SelectObject(srcDC, (HBITMAP) bitmap.GetHBITMAP());
    HDC destDC = CreateCompatibleDC(0);
    SelectObject(destDC, (HBITMAP) m_maskBitmap);
    BitBlt(destDC, 0, 0, bitmap.GetWidth(), bitmap.GetHeight(), srcDC, 0, 0, SRCCOPY);
    SelectObject(srcDC, 0);
    DeleteDC(srcDC);
    SelectObject(destDC, 0);
    DeleteDC(destDC);
    return TRUE;
#else
    return FALSE;
#endif
}

// Create a mask from a bitmap and a palette index indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, int paletteIndex)
{
    if ( m_maskBitmap )
    {
        ::DeleteObject((HBITMAP) m_maskBitmap);
        m_maskBitmap = 0;
    }

#if wxUSE_PALETTE
    if (bitmap.Ok() && bitmap.GetPalette()->Ok())
    {
        unsigned char red, green, blue;
        if (bitmap.GetPalette()->GetRGB(paletteIndex, &red, &green, &blue))
        {
            wxColour transparentColour(red, green, blue);
            return Create(bitmap, transparentColour);
        }
    }
#endif // wxUSE_PALETTE

    return FALSE;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
#ifndef __WXMICROWIN__
    wxCHECK_MSG( bitmap.Ok(), FALSE, _T("invalid bitmap in wxMask::Create") );

    if ( m_maskBitmap )
    {
        ::DeleteObject((HBITMAP) m_maskBitmap);
        m_maskBitmap = 0;
    }

    int width = bitmap.GetWidth(),
        height = bitmap.GetHeight();

    // scan the bitmap for the transparent colour and set the corresponding
    // pixels in the mask to BLACK and the rest to WHITE
    COLORREF maskColour = wxColourToPalRGB(colour);
    m_maskBitmap = (WXHBITMAP)::CreateBitmap(width, height, 1, 1, 0);

    HDC srcDC = ::CreateCompatibleDC(NULL);
    HDC destDC = ::CreateCompatibleDC(NULL);
    if ( !srcDC || !destDC )
    {
        wxLogLastError(wxT("CreateCompatibleDC"));
    }

    bool ok = TRUE;

    // SelectObject() will fail
    wxASSERT_MSG( !bitmap.GetSelectedInto(),
                  _T("bitmap can't be selected in another DC") );

    HGDIOBJ hbmpSrcOld = ::SelectObject(srcDC, GetHbitmapOf(bitmap));
    if ( !hbmpSrcOld )
    {
        wxLogLastError(wxT("SelectObject"));

        ok = FALSE;
    }

    HGDIOBJ hbmpDstOld = ::SelectObject(destDC, (HBITMAP)m_maskBitmap);
    if ( !hbmpDstOld )
    {
        wxLogLastError(wxT("SelectObject"));

        ok = FALSE;
    }

    if ( ok )
    {
        // this will create a monochrome bitmap with 0 points for the pixels
        // which have the same value as the background colour and 1 for the
        // others
        ::SetBkColor(srcDC, maskColour);
        ::BitBlt(destDC, 0, 0, width, height, srcDC, 0, 0, NOTSRCCOPY);
    }

    ::SelectObject(srcDC, hbmpSrcOld);
    ::DeleteDC(srcDC);
    ::SelectObject(destDC, hbmpDstOld);
    ::DeleteDC(destDC);

    return ok;
#else // __WXMICROWIN__
    return FALSE;
#endif // __WXMICROWIN__/!__WXMICROWIN__
}

// ----------------------------------------------------------------------------
// wxBitmapHandler
// ----------------------------------------------------------------------------

bool wxBitmapHandler::Create(wxGDIImage *image,
                             void *data,
                             long flags,
                             int width, int height, int depth)
{
    wxBitmap *bitmap = wxDynamicCast(image, wxBitmap);

    return bitmap ? Create(bitmap, data, flags, width, height, depth) : FALSE;
}

bool wxBitmapHandler::Load(wxGDIImage *image,
                           const wxString& name,
                           long flags,
                           int width, int height)
{
    wxBitmap *bitmap = wxDynamicCast(image, wxBitmap);

    return bitmap ? LoadFile(bitmap, name, flags, width, height) : FALSE;
}

bool wxBitmapHandler::Save(wxGDIImage *image,
                           const wxString& name,
                           int type)
{
    wxBitmap *bitmap = wxDynamicCast(image, wxBitmap);

    return bitmap ? SaveFile(bitmap, name, type) : FALSE;
}

bool wxBitmapHandler::Create(wxBitmap *WXUNUSED(bitmap),
                             void *WXUNUSED(data),
                             long WXUNUSED(type),
                             int WXUNUSED(width),
                             int WXUNUSED(height),
                             int WXUNUSED(depth))
{
    return FALSE;
}

bool wxBitmapHandler::LoadFile(wxBitmap *WXUNUSED(bitmap),
                               const wxString& WXUNUSED(name),
                               long WXUNUSED(type),
                               int WXUNUSED(desiredWidth),
                               int WXUNUSED(desiredHeight))
{
    return FALSE;
}

bool wxBitmapHandler::SaveFile(wxBitmap *WXUNUSED(bitmap),
                               const wxString& WXUNUSED(name),
                               int WXUNUSED(type),
                               const wxPalette *WXUNUSED(palette))
{
    return FALSE;
}

// ----------------------------------------------------------------------------
// DIB functions
// ----------------------------------------------------------------------------

#ifndef __WXMICROWIN__
bool wxCreateDIB(long xSize, long ySize, long bitsPerPixel,
                 HPALETTE hPal, LPBITMAPINFO* lpDIBHeader)
{
   unsigned long   i, headerSize;
   LPBITMAPINFO    lpDIBheader = NULL;
   LPPALETTEENTRY  lpPe = NULL;


   // Allocate space for a DIB header
   headerSize = (sizeof(BITMAPINFOHEADER) + (256 * sizeof(PALETTEENTRY)));
   lpDIBheader = (BITMAPINFO *) malloc(headerSize);
   lpPe = (PALETTEENTRY *)((BYTE*)lpDIBheader + sizeof(BITMAPINFOHEADER));

   GetPaletteEntries(hPal, 0, 256, lpPe);

   memset(lpDIBheader, 0x00, sizeof(BITMAPINFOHEADER));

   // Fill in the static parts of the DIB header
   lpDIBheader->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   lpDIBheader->bmiHeader.biWidth = xSize;
   lpDIBheader->bmiHeader.biHeight = ySize;
   lpDIBheader->bmiHeader.biPlanes = 1;

   // this value must be 1, 4, 8 or 24 so PixelDepth can only be
   lpDIBheader->bmiHeader.biBitCount = (WORD)(bitsPerPixel);
   lpDIBheader->bmiHeader.biCompression = BI_RGB;
   lpDIBheader->bmiHeader.biSizeImage = xSize * abs(ySize) * bitsPerPixel >> 3;
   lpDIBheader->bmiHeader.biClrUsed = 256;


   // Initialize the DIB palette
   for (i = 0; i < 256; i++) {
      lpDIBheader->bmiColors[i].rgbReserved = lpPe[i].peFlags;
      lpDIBheader->bmiColors[i].rgbRed = lpPe[i].peRed;
      lpDIBheader->bmiColors[i].rgbGreen = lpPe[i].peGreen;
      lpDIBheader->bmiColors[i].rgbBlue = lpPe[i].peBlue;
   }

   *lpDIBHeader = lpDIBheader;

   return TRUE;
}

void wxFreeDIB(LPBITMAPINFO lpDIBHeader)
{
    free(lpDIBHeader);
}
#endif

// ----------------------------------------------------------------------------
// other helper functions
// ----------------------------------------------------------------------------

extern HBITMAP wxInvertMask(HBITMAP hbmpMask, int w, int h)
{
#ifndef __WXMICROWIN__
    wxCHECK_MSG( hbmpMask, 0, _T("invalid bitmap in wxInvertMask") );

    // get width/height from the bitmap if not given
    if ( !w || !h )
    {
        BITMAP bm;
        ::GetObject(hbmpMask, sizeof(BITMAP), (LPVOID)&bm);
        w = bm.bmWidth;
        h = bm.bmHeight;
    }

    HDC hdcSrc = ::CreateCompatibleDC(NULL);
    HDC hdcDst = ::CreateCompatibleDC(NULL);
    if ( !hdcSrc || !hdcDst )
    {
        wxLogLastError(wxT("CreateCompatibleDC"));
    }

    HBITMAP hbmpInvMask = ::CreateBitmap(w, h, 1, 1, 0);
    if ( !hbmpInvMask )
    {
        wxLogLastError(wxT("CreateBitmap"));
    }

    ::SelectObject(hdcSrc, hbmpMask);
    ::SelectObject(hdcDst, hbmpInvMask);
    if ( !::BitBlt(hdcDst, 0, 0, w, h,
                   hdcSrc, 0, 0,
                   NOTSRCCOPY) )
    {
        wxLogLastError(wxT("BitBlt"));
    }

    ::DeleteDC(hdcSrc);
    ::DeleteDC(hdcDst);

    return hbmpInvMask;
#else
    return 0;
#endif
}
