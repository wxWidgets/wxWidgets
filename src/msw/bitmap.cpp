////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
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

#ifdef wxHAVE_RAW_BITMAP
#include "wx/rawbmp.h"
#endif

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
#if wxUSE_PALETTE
    wxPalette     m_bitmapPalette;
#endif // wxUSE_PALETTE

    // MSW-specific
    // ------------

#ifdef __WXDEBUG__
    // this field is solely for error checking: we detect selecting a bitmap
    // into more than one DC at once or deleting a bitmap still selected into a
    // DC (both are serious programming errors under Windows)
    wxDC         *m_selectedInto;
#endif // __WXDEBUG__

    // when GetRawData() is called for a DDB we need to convert it to a DIB
    // first to be able to provide direct access to it and we cache that DIB
    // here and convert it back to DDB when UngetRawData() is called
    wxDIB *m_dib;

    // true if we have alpha transparency info and can be drawn using
    // AlphaBlend()
    bool m_hasAlpha;

    // true if our HBITMAP is a DIB section, false if it is a DDB
    bool m_isDIB;

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
// helper functions
// ----------------------------------------------------------------------------

// decide whether we should create a DIB or a DDB for the given parameters
static bool wxShouldCreateDIB(int w, int h, int d, WXHDC hdc)
{
    // here is the logic:
    //
    //  (a) if hdc is specified, the caller explicitly wants DDB
    //  (b) otherwise, create a DIB if depth >= 24 (we don't support 16bpp or
    //      less DIBs anyhow)
    //  (c) finally, create DIBs under Win9x even if the depth hasn't been
    //      explicitly specified but the current display depth is 24 or more
    //      and the image is "big", i.e. > 16Mb which is the theoretical limit
    //      for DDBs under Win9x
    //
    // consequences (all of which seem to make sense):
    //
    //  (i)     by default, DDBs are created (depth == -1 usually)
    //  (ii)    DIBs can be created by explicitly specifying the depth
    //  (iii)   using a DC always forces creating a DDB
    return !hdc &&
            (d >= 24 ||
                (d == -1 &&
                    wxDIB::GetLineSize(w, wxDisplayDepth())*h > 16*1024*1024));
}

// ----------------------------------------------------------------------------
// wxBitmapRefData
// ----------------------------------------------------------------------------

wxBitmapRefData::wxBitmapRefData()
{
#ifdef __WXDEBUG__
    m_selectedInto = NULL;
#endif
    m_bitmapMask = NULL;

    m_hBitmap = (WXHBITMAP) NULL;
    m_dib = NULL;

    m_isDIB =
    m_hasAlpha = FALSE;
}

void wxBitmapRefData::Free()
{
    wxASSERT_MSG( !m_selectedInto,
                  wxT("deleting bitmap still selected into wxMemoryDC") );

    wxASSERT_MSG( !m_dib, _T("forgot to call wxBitmap::UngetRawData()!") );

    if ( m_hBitmap)
    {
        if ( !::DeleteObject((HBITMAP)m_hBitmap) )
        {
            wxLogLastError(wxT("DeleteObject(hbitmap)"));
        }
    }

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

bool wxBitmap::CopyFromDIB(const wxDIB& dib)
{
    wxCHECK_MSG( dib.IsOk(), FALSE, _T("invalid DIB in CopyFromDIB") );

    HBITMAP hbitmap = dib.CreateDDB();
    if ( !hbitmap )
        return FALSE;

    UnRef();

    wxBitmapRefData *refData = new wxBitmapRefData;
    m_refData = refData;

    refData->m_width = dib.GetWidth();
    refData->m_height = dib.GetHeight();
    refData->m_depth = dib.GetDepth();

    refData->m_hBitmap = (WXHBITMAP)hbitmap;

#if wxUSE_PALETTE
    wxPalette *palette = dib.CreatePalette();
    if ( palette )
    {
        refData->m_bitmapPalette = *palette;
    }

    delete palette;
#endif // wxUSE_PALETTE

    return TRUE;
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

wxBitmap::wxBitmap(int w, int h, const wxDC& dc)
{
    Init();

    (void)Create(w, h, dc);
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

bool wxBitmap::Create(int width, int height, int depth)
{
    return DoCreate(width, height, depth, 0);
}

bool wxBitmap::Create(int width, int height, const wxDC& dc)
{
    wxCHECK_MSG( dc.Ok(), FALSE, _T("invalid HDC in wxBitmap::Create()") );

    return DoCreate(width, height, -1, dc.GetHDC());
}

bool wxBitmap::DoCreate(int w, int h, int d, WXHDC hdc)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    GetBitmapData()->m_width = w;
    GetBitmapData()->m_height = h;

    HBITMAP hbmp;

    if ( wxShouldCreateDIB(w, h, d, hdc) )
    {
        if ( d == -1 )
        {
            // create DIBs without alpha channel by default
            d = 24;
        }

        wxDIB dib(w, h, d);
        if ( !dib.IsOk() )
           return FALSE;

        // don't delete the DIB section in dib object dtor
        hbmp = dib.Detach();

        GetBitmapData()->m_isDIB = TRUE;
        GetBitmapData()->m_depth = d;
    }
    else // create a DDB
    {
        if ( d == -1 )
            d = wxDisplayDepth();

        GetBitmapData()->m_depth = d;

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
    }

    SetHBITMAP((WXHBITMAP)hbmp);

#if WXWIN_COMPATIBILITY_2
    GetBitmapData()->m_ok = hbmp != 0;
#endif // WXWIN_COMPATIBILITY_2

    return Ok();
}

#if wxUSE_IMAGE

// ----------------------------------------------------------------------------
// wxImage to/from conversions for Microwin
// ----------------------------------------------------------------------------

// Microwin versions are so different from normal ones that it really doesn't
// make sense to use #ifdefs inside the function bodies
#ifdef __WXMICROWIN__

bool wxBitmap::CreateFromImage(const wxImage& image, int depth, const wxDC& dc)
{
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
}

wxImage wxBitmap::ConvertToImage() const
{
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
}

#endif // __WXMICROWIN__

// ----------------------------------------------------------------------------
// wxImage to/from conversions
// ----------------------------------------------------------------------------

bool wxBitmap::CreateFromImage(const wxImage& image, int depth)
{
    return CreateFromImage(image, depth, 0);
}

bool wxBitmap::CreateFromImage(const wxImage& image, const wxDC& dc)
{
    wxCHECK_MSG( dc.Ok(), FALSE,
                    _T("invalid HDC in wxBitmap::CreateFromImage()") );

    return CreateFromImage(image, -1, dc.GetHDC());
}

bool wxBitmap::CreateFromImage(const wxImage& image, int depth, WXHDC hdc )
{
    wxCHECK_MSG( image.Ok(), FALSE, wxT("invalid image") );

    UnRef();

    // first convert the image to DIB
    const int h = image.GetHeight();
    const int w = image.GetWidth();

    wxDIB dib(image);
    if ( !dib.IsOk() )
        return FALSE;


    // store the bitmap parameters
    wxBitmapRefData *refData = new wxBitmapRefData;
    refData->m_width = w;
    refData->m_height = h;
    refData->m_hasAlpha = image.HasAlpha();

    m_refData = refData;


    // next either store DIB as is or create a DDB from it
    HBITMAP hbitmap;

    // are we going to use DIB?
    //
    // NB: DDBs don't support alpha so if we have alpha channel we must use DIB
    if ( image.HasAlpha() || wxShouldCreateDIB(w, h, depth, hdc) )
    {
        // don't delete the DIB section in dib object dtor
        hbitmap = dib.Detach();

        refData->m_isDIB = TRUE;
        refData->m_depth = dib.GetDepth();
    }
    else // we need to convert DIB to DDB
    {
        hbitmap = dib.CreateDDB((HDC)hdc);

        refData->m_depth = depth == -1 ? wxDisplayDepth() : depth;
    }

    // validate this object
    SetHBITMAP((WXHBITMAP)hbitmap);

#if WXWIN_COMPATIBILITY_2
    m_refData->m_ok = TRUE;
#endif // WXWIN_COMPATIBILITY_2

    // finally also set the mask if we have one
    if ( image.HasMask() )
    {
        SetMask(new wxMask(*this, wxColour(image.GetMaskRed(),
                                           image.GetMaskGreen(),
                                           image.GetMaskBlue())));
    }

    return TRUE;
}

wxImage wxBitmap::ConvertToImage() const
{
    // the colour used as transparent one in wxImage and the one it is replaced
    // with when it really occurs in the bitmap
    static const int MASK_RED = 1;
    static const int MASK_GREEN = 2;
    static const int MASK_BLUE = 3;
    static const int MASK_BLUE_REPLACEMENT = 2;

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
        // background color set to mask colour
        unsigned char r=MASK_RED, g=MASK_GREEN, b=MASK_BLUE;
        ptdata = data;
        ptbits = lpBits;
        for( i=0; i<height; i++ )
        {
            for( j=0; j<width; j++ )
            {
                // is this pixel transparent?
                if ( *ptbits != 0 )
                {
                    if ( (ptdata[0] == MASK_RED) &&
                            (ptdata[1] == MASK_GREEN) &&
                                (ptdata[2] == MASK_BLUE) )
                    {
                        // we have to fudge the colour a bit to prevent this
                        // pixel from appearing transparent
                        ptdata[2] = MASK_BLUE_REPLACEMENT;
                    }
                    ptdata += 3;
                }
                else // masked pixel
                {
                    *(ptdata++)  = MASK_RED;
                    *(ptdata++)  = MASK_GREEN;
                    *(ptdata++)  = MASK_BLUE;
                }
                ptbits += 3;
            }
            ptbits += padding;
        }

        image.SetMaskColour( MASK_RED, MASK_GREEN, MASK_BLUE );
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
}

#endif // wxUSE_IMAGE

// ----------------------------------------------------------------------------
// loading and saving bitmaps
// ----------------------------------------------------------------------------

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

    wxBitmap ret( rect.width, rect.height );
    wxASSERT_MSG( ret.Ok(), wxT("GetSubBitmap error") );

#ifndef __WXMICROWIN__
    // TODO: copy alpha channel data if any

    // copy bitmap data
    MemoryHDC dcSrc,
              dcDst;

    {
        SelectInHDC selectSrc(dcSrc, GetHbitmap()),
                    selectDst(dcDst, GetHbitmapOf(ret));

        if ( !selectSrc || !selectDst )
        {
            wxLogLastError(_T("SelectObjct(hBitmap)"));
        }

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

#if wxUSE_PALETTE
wxPalette* wxBitmap::GetPalette() const
{
    return GetBitmapData() ? &GetBitmapData()->m_bitmapPalette
                           : (wxPalette *) NULL;
}
#endif

wxMask *wxBitmap::GetMask() const
{
    return GetBitmapData() ? GetBitmapData()->GetMask() : (wxMask *) NULL;
}

#ifdef __WXDEBUG__

wxDC *wxBitmap::GetSelectedInto() const
{
    return GetBitmapData() ? GetBitmapData()->m_selectedInto : (wxDC *) NULL;
}

#endif

#if WXWIN_COMPATIBILITY_2_4

int wxBitmap::GetQuality() const
{
    return 0;
}

#endif // WXWIN_COMPATIBILITY_2_4

void wxBitmap::UseAlpha()
{
    if ( GetBitmapData() )
        GetBitmapData()->m_hasAlpha = true;
}

bool wxBitmap::HasAlpha() const
{
    return GetBitmapData() && GetBitmapData()->m_hasAlpha;
}

// ----------------------------------------------------------------------------
// wxBitmap setters
// ----------------------------------------------------------------------------

#ifdef __WXDEBUG__

void wxBitmap::SetSelectedInto(wxDC *dc)
{
    if ( GetBitmapData() )
        GetBitmapData()->m_selectedInto = dc;
}

#endif

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
// raw bitmap access support
// ----------------------------------------------------------------------------

#ifdef wxHAVE_RAW_BITMAP
void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    if ( !Ok() )
    {
        // no bitmap, no data (raw or otherwise)
        return NULL;
    }

    // if we're already a DIB we can access our data directly, but if not we
    // need to convert this DDB to a DIB section and use it for raw access and
    // then convert it back
    HBITMAP hDIB;
    if ( !GetBitmapData()->m_isDIB )
    {
        wxCHECK_MSG( !GetBitmapData()->m_dib, FALSE,
                        _T("GetRawData() may be called only once") );

        wxDIB *dib = new wxDIB(*this);
        if ( !dib->IsOk() )
        {
            delete dib;

            return NULL;
        }

        // we'll free it in UngetRawData()
        GetBitmapData()->m_dib = dib;

        hDIB = dib->GetHandle();
    }
    else // we're a DIB
    {
        hDIB = GetHbitmap();
    }

    DIBSECTION ds;
    if ( ::GetObject(hDIB, sizeof(ds), &ds) != sizeof(DIBSECTION) )
    {
        wxFAIL_MSG( _T("failed to get DIBSECTION from a DIB?") );

        return NULL;
    }

    // check that the bitmap is in correct format
    if ( ds.dsBm.bmBitsPixel != bpp )
    {
        wxFAIL_MSG( _T("incorrect bitmap type in wxBitmap::GetRawData()") );

        return NULL;
    }

    // ok, store the relevant info in wxPixelDataBase
    const LONG h = ds.dsBm.bmHeight;

    data.m_width = ds.dsBm.bmWidth;
    data.m_height = h;

    // remember that DIBs are stored in top to bottom order!
    const LONG bytesPerRow = ds.dsBm.bmWidthBytes;
    data.m_stride = -bytesPerRow;

    char *bits = (char *)ds.dsBm.bmBits;
    if ( h > 1 )
    {
        bits += (h - 1)*bytesPerRow;
    }

    return bits;
}

void wxBitmap::UngetRawData(wxPixelDataBase& dataBase)
{
    if ( !Ok() )
        return;

    // the cast is ugly but we can't do without it and without making this
    // function template (and hence inline) unfortunately
    typedef wxPixelData<wxBitmap, wxAlphaPixelFormat> PixelData;
    PixelData& data = (PixelData &)dataBase;

    if ( !data )
    {
        // invalid data, don't crash -- but don't assert neither as we're
        // called automatically from wxPixelDataBase dtor and so there is no
        // way to prevent this from happening
        return;
    }

    if ( GetBitmapData()->m_hasAlpha )
    {
        // AlphaBlend() wants to have premultiplied source alpha but
        // wxRawBitmap API uses normal, not premultiplied, colours, so adjust
        // them here now
        PixelData::Iterator p(data);

        const int w = data.GetWidth();
        const int h = data.GetHeight();

        for ( int y = 0; y < h; y++ )
        {
            PixelData::Iterator rowStart = p;

            for ( int x = 0; x < w; x++ )
            {
                const unsigned alpha = p.Alpha();

                p.Red() = (p.Red() * alpha + 127) / 255;
                p.Blue() = (p.Blue() * alpha + 127) / 255;
                p.Green() = (p.Green() * alpha + 127) / 255;

                ++p;
            }

            p = rowStart;
            p.OffsetY(data, 1);
        }

        // if we're a DDB we need to convert DIB back to DDB now to make the
        // changes made via raw bitmap access effective
        if ( !GetBitmapData()->m_isDIB )
        {
            wxDIB *dib = GetBitmapData()->m_dib;
            GetBitmapData()->m_dib = NULL;

            // TODO: convert

            delete dib;
        }
    }
}
#endif // #ifdef wxHAVE_RAW_BITMAP

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
// global helper functions implemented here
// ----------------------------------------------------------------------------

// helper of wxBitmapToHICON/HCURSOR
static
HICON wxBitmapToIconOrCursor(const wxBitmap& bmp,
                             bool iconWanted,
                             int hotSpotX,
                             int hotSpotY)
{
    if ( !bmp.Ok() )
    {
        // we can't create an icon/cursor form nothing
        return 0;
    }

    wxMask *mask = bmp.GetMask();
    if ( !mask )
    {
        // we must have a mask for an icon, so even if it's probably incorrect,
        // do create it (grey is the "standard" transparent colour)
        mask = new wxMask(bmp, *wxLIGHT_GREY);
    }

    ICONINFO iconInfo;
    iconInfo.fIcon = iconWanted;  // do we want an icon or a cursor?
    if ( !iconWanted )
    {
        iconInfo.xHotspot = hotSpotX;
        iconInfo.yHotspot = hotSpotY;
    }

    iconInfo.hbmMask = wxInvertMask((HBITMAP)mask->GetMaskBitmap());
    iconInfo.hbmColor = GetHbitmapOf(bmp);

    // black out the transparent area to preserve background colour, because
    // Windows blits the original bitmap using SRCINVERT (XOR) after applying
    // the mask to the dest rect.
    {
        MemoryHDC dcSrc, dcDst;
        SelectInHDC selectMask(dcSrc, (HBITMAP)mask->GetMaskBitmap()),
                    selectBitmap(dcDst, iconInfo.hbmColor);

        if ( !::BitBlt(dcDst, 0, 0, bmp.GetWidth(), bmp.GetHeight(),
                       dcSrc, 0, 0, SRCAND) )
        {
            wxLogLastError(_T("BitBlt"));
        }
    }

    HICON hicon = ::CreateIconIndirect(&iconInfo);

    if ( !bmp.GetMask() )
    {
        // we created the mask, now delete it
        delete mask;
    }

    // delete the inverted mask bitmap we created as well
    ::DeleteObject(iconInfo.hbmMask);

    return hicon;
}

HICON wxBitmapToHICON(const wxBitmap& bmp)
{
    return wxBitmapToIconOrCursor(bmp, TRUE, 0, 0);
}

HCURSOR wxBitmapToHCURSOR(const wxBitmap& bmp, int hotSpotX, int hotSpotY)
{
    return (HCURSOR)wxBitmapToIconOrCursor(bmp, FALSE, hotSpotX, hotSpotY);
}

HBITMAP wxInvertMask(HBITMAP hbmpMask, int w, int h)
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
