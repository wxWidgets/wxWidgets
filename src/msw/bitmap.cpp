/////////////////////////////////////////////////////////////////////////////
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

#include "wx/msw/dib.h"
#include "wx/image.h"

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
    m_quality = 0;
    m_selectedInto = NULL;
    m_numColors = 0;
    m_bitmapMask = NULL;
    m_hBitmap = (WXHBITMAP) NULL;
}

void wxBitmapRefData::Free()
{
    wxASSERT_MSG( !m_selectedInto,
                  wxT("deleting bitmap still selected into wxMemoryDC") );

    if ( m_hBitmap)
    {
        if ( !::DeleteObject((HBITMAP)m_hBitmap) )
        {
            wxLogLastError("DeleteObject(hbitmap)");
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

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

#ifdef __WIN32__

bool wxBitmap::CopyFromIconOrCursor(const wxGDIImage& icon)
{
    // it may be either HICON or HCURSOR
    HICON hicon = (HICON)icon.GetHandle();

    ICONINFO iconInfo;
    if ( !::GetIconInfo(hicon, &iconInfo) )
    {
        wxLogLastError("GetIconInfo");

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
    HBITMAP hbmpMask = ::CreateBitmap(w, h, 1, 1, 0);

    // the icons mask is opposite to the usual wxWin convention
    HDC dcSrc = ::CreateCompatibleDC(NULL);
    HDC dcDst = ::CreateCompatibleDC(NULL);
    (void)SelectObject(dcSrc, iconInfo.hbmMask);
    (void)SelectObject(dcDst, hbmpMask);

    HBRUSH brush = ::CreateSolidBrush(RGB(255, 255, 255));
    RECT rect = { 0, 0, w, h };
    FillRect(dcDst, &rect, brush);

    BitBlt(dcDst, 0, 0, w, h, dcSrc, 0, 0, SRCINVERT);

    SelectObject(dcDst, NULL);
    SelectObject(dcSrc, NULL);
    DeleteDC(dcDst);
    DeleteDC(dcSrc);

    refData->m_bitmapMask = new wxMask((WXHBITMAP)hbmpMask);

#if WXWIN_COMPATIBILITY_2
    refData->m_ok = TRUE;
#endif // WXWIN_COMPATIBILITY_2

    return TRUE;
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
#endif // Win16

    return CopyFromIconOrCursor(cursor);
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
    if (wxTheBitmapList)
        wxTheBitmapList->DeleteObject(this);
}

wxBitmap::wxBitmap(const char bits[], int the_width, int the_height, int no_bits)
{
    Init();

    wxBitmapRefData *refData = new wxBitmapRefData;
    m_refData = refData;

    refData->m_width = the_width;
    refData->m_height = the_height;
    refData->m_depth = no_bits;
    refData->m_numColors = 0;
    refData->m_selectedInto = NULL;

    HBITMAP hbmp = ::CreateBitmap(the_width, the_height, 1, no_bits, bits);
    if ( !hbmp )
    {
        wxLogLastError("CreateBitmap");
    }

    SetHBITMAP((WXHBITMAP)hbmp);
}

// Create from XPM data
wxBitmap::wxBitmap(char **data, wxControl *WXUNUSED(anItem))
{
    Init();

    (void)Create((void *)data, wxBITMAP_TYPE_XPM_DATA, 0, 0, 0);
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

wxBitmap::wxBitmap(const wxString& filename, long type)
{
    Init();

    LoadFile(filename, (int)type);
}

bool wxBitmap::Create(int w, int h, int d)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    GetBitmapData()->m_width = w;
    GetBitmapData()->m_height = h;
    GetBitmapData()->m_depth = d;

    HBITMAP hbmp;

    if ( d > 0 )
    {
        hbmp = ::CreateBitmap(w, h, 1, d, NULL);
        if ( !hbmp )
        {
            wxLogLastError("CreateBitmap");
        }
    }
    else
    {
        ScreenHDC dc;
        hbmp = ::CreateCompatibleBitmap(dc, w, h);
        if ( !hbmp )
        {
            wxLogLastError("CreateCompatibleBitmap");
        }

        GetBitmapData()->m_depth = wxDisplayDepth();
    }

    SetHBITMAP((WXHBITMAP)hbmp);

#if WXWIN_COMPATIBILITY_2
    GetBitmapData()->m_ok = hbmp != 0;
#endif // WXWIN_COMPATIBILITY_2

    return Ok();
}

bool wxBitmap::LoadFile(const wxString& filename, long type)
{
    UnRef();

    wxBitmapHandler *handler = wxDynamicCast(FindHandler(type), wxBitmapHandler);

    if ( handler )
    {
        m_refData = new wxBitmapRefData;

        return handler->LoadFile(this, filename, type, -1, -1);
    }
    else
    {
        wxImage image;
        if ( !image.LoadFile( filename, type ) || !image.Ok() )
            return FALSE;

        *this = image.ConvertToBitmap();

        return TRUE;
    }
}

bool wxBitmap::Create(void *data, long type, int width, int height, int depth)
{
    UnRef();

    wxBitmapHandler *handler = wxDynamicCast(FindHandler(type), wxBitmapHandler);

    if ( !handler )
    {
        wxLogDebug(wxT("Failed to create bitmap: no bitmap handler for "
                       "type %d defined."), type);

        return FALSE;
    }

    m_refData = new wxBitmapRefData;

    return handler->Create(this, data, type, width, height, depth);
}

bool wxBitmap::SaveFile(const wxString& filename, int type, const wxPalette *palette)
{
    wxBitmapHandler *handler = wxDynamicCast(FindHandler(type), wxBitmapHandler);

    if ( handler )
    {
        return handler->SaveFile(this, filename, type, palette);
    }
    else
    {
        // FIXME what about palette? shouldn't we use it?
        wxImage image( *this );
        if (!image.Ok())
            return FALSE;

        return image.SaveFile( filename, type );
    }
}

// ----------------------------------------------------------------------------
// wxBitmap accessors
// ----------------------------------------------------------------------------

void wxBitmap::SetQuality(int q)
{
    EnsureHasData();

    GetBitmapData()->m_quality = q;
}

#if WXWIN_COMPATIBILITY_2
void wxBitmap::SetOk(bool isOk)
{
    EnsureHasData();

    GetBitmapData()->m_ok = isOk;
}
#endif // WXWIN_COMPATIBILITY_2

void wxBitmap::SetPalette(const wxPalette& palette)
{
    EnsureHasData();

    GetBitmapData()->m_bitmapPalette = palette;
}

void wxBitmap::SetMask(wxMask *mask)
{
    EnsureHasData();

    GetBitmapData()->m_bitmapMask = mask;
}

// Creates a bitmap that matches the device context, from
// an arbitray bitmap. At present, the original bitmap must have an
// associated palette. TODO: use a default palette if no palette exists.
// Contributed by Frederic Villeneuve <frederic.villeneuve@natinst.com>
wxBitmap wxBitmap::GetBitmapForDC(wxDC& dc) const
{
    wxMemoryDC      memDC;
    wxBitmap        tmpBitmap(this->GetWidth(), this->GetHeight(), dc.GetDepth());
    HPALETTE        hPal = (HPALETTE) NULL;
    LPBITMAPINFO    lpDib;
    void            *lpBits = (void*) NULL;

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
    if ( m_maskBitmap )
    {
        ::DeleteObject((HBITMAP) m_maskBitmap);
        m_maskBitmap = 0;
    }
    if (!bitmap.Ok() || bitmap.GetDepth() != 1)
    {
        return FALSE;
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
    if (bitmap.Ok() && bitmap.GetPalette()->Ok())
    {
        unsigned char red, green, blue;
        if (bitmap.GetPalette()->GetRGB(paletteIndex, &red, &green, &blue))
        {
            wxColour transparentColour(red, green, blue);
            return Create(bitmap, transparentColour);
        }
    }
    return FALSE;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
    if ( m_maskBitmap )
    {
        ::DeleteObject((HBITMAP) m_maskBitmap);
        m_maskBitmap = 0;
    }
    if (!bitmap.Ok())
    {
        return FALSE;
    }

    // scan the bitmap for the transparent colour and set
    // the corresponding pixels in the mask to BLACK and
    // the rest to WHITE
    COLORREF maskColour = RGB(colour.Red(), colour.Green(), colour.Blue());
    m_maskBitmap = (WXHBITMAP) ::CreateBitmap(
            bitmap.GetWidth(),
            bitmap.GetHeight(),
            1, 1, 0
                                             );
    HDC srcDC = ::CreateCompatibleDC(0);
    ::SelectObject(srcDC, (HBITMAP) bitmap.GetHBITMAP());
    HDC destDC = ::CreateCompatibleDC(0);
    ::SelectObject(destDC, (HBITMAP) m_maskBitmap);

    // this is not very efficient, but I can't think
    // of a better way of doing it
    for (int w = 0; w < bitmap.GetWidth(); w++)
    {
        for (int h = 0; h < bitmap.GetHeight(); h++)
        {
            COLORREF col = GetPixel(srcDC, w, h);
            if (col == maskColour)
            {
                ::SetPixel(destDC, w, h, RGB(0, 0, 0));
            }
            else
            {
                ::SetPixel(destDC, w, h, RGB(255, 255, 255));
            }
        }
    }
    ::SelectObject(srcDC, 0);
    ::DeleteDC(srcDC);
    ::SelectObject(destDC, 0);
    ::DeleteDC(destDC);
    return TRUE;
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

    return bitmap ? Create(bitmap, data, width, height, depth) : FALSE;
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


