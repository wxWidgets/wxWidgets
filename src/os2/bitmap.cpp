/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      David Webster
// Modified by:
// Created:     08/08/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

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

#include "wx/os2/private.h"
#include "wx/log.h"

//#include "wx/msw/dib.h"
#include "wx/image.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler, wxObject)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxBitmapRefData
// ----------------------------------------------------------------------------

wxBitmapRefData::wxBitmapRefData()
{
    m_nQuality      = 0;
    m_pSelectedInto = NULL;
    m_nNumColors    = 0;
    m_pBitmapMask   = NULL;
}

void wxBitmapRefData::Free()
{
    wxASSERT_MSG( !m_pSelectedInto,
                  wxT("deleting bitmap still selected into wxMemoryDC") );

    if (m_hBitmap)
    {
        if ( !::GpiDeleteBitmap((HBITMAP)m_hBitmap) )
        {
            wxLogLastError("GpiDeleteBitmap(hbitmap)");
        }
    }

    delete m_pBitmapMask;
    m_pBitmapMask = NULL;
}

// ----------------------------------------------------------------------------
// wxBitmap creation
// ----------------------------------------------------------------------------

// this function should be called from all wxBitmap ctors
void wxBitmap::Init()
{
    // m_refData = NULL; done in the base class ctor

    if (wxTheBitmapList)
        wxTheBitmapList->AddBitmap(this);
}

bool wxBitmap::CopyFromIconOrCursor(
  const wxGDIImage&                 rIcon
)
{
    wxBitmapRefData*                pRefData = new wxBitmapRefData;

    m_refData = pRefData;

    refData->m_width = rIcon.GetWidth();
    refData->m_height = rIcon.GetHeight();
    refData->m_depth = wxDisplayDepth();

    refData->m_hBitmap = (WXHBITMAP)rIcon.GetHandle();
    // no mask???
    refData->m_bitmapMask = new wxMask();

#if WXWIN_COMPATIBILITY_2
    refData->m_ok = TRUE;
#endif // WXWIN_COMPATIBILITY_2

    return(TRUE);
}

bool wxBitmap::CopyFromCursor(
  const wxCursor&                   rCursor
)
{
    UnRef();

    if (!rCursor.Ok())
        return(FALSE);
    return CopyFromIconOrCursor(wxGDIImage)rCursor);
}

bool wxBitmap::CopyFromIcon(
  const wxIcon&                     rIcon
)
{
    UnRef();

    if (!rIcon.Ok())
        return(FALSE);

#if WXWIN_COMPATIBILITY_2
    refData->m_ok = TRUE;
#endif // WXWIN_COMPATIBILITY_2

    return CopyFromIconOrCursor(icon);
}

wxBitmap::~wxBitmap()
{
    if (wxTheBitmapList)
        wxTheBitmapList->DeleteObject(this);
}

wxBitmap::wxBitmap(
  const char                        zBits[]
, int                               nTheWidth
, int                               nTheHeight
, int                               nNoBits
)
{
    Init();

    wxBitmapRefData*                pRefData = new wxBitmapRefData;
    BITMAPINFOHEADER2               vHeader;
    BITMAPINFO2                     vInfo;
    HDC                             hDc;
    HPS                             hPs;
    DEVOPENSTRUCT                   vDop = { NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    SIZEL                           vSize = {0, 0};

    wxAssert(vHabmain != NULL);

    hDc = ::DevOpenDC(vHabmain, OD_MEMORY, (PSZ)"*", 1L, (PDEVOPENDATA)&vDop, 0L);

    vHeader.cbFix           = sizeof(vHeader);
    vHeader.cx              = (USHORT)nTheWidth;
    vHeader.cy              = (USHORT)nTheHeight;
    vHeader.cPlanes         = 1L;
    vHeader.cBitCount       = nNoBits;
    vHeader.ulCompression   = BCA_UNCOMP;
    vHeader.cxResolution    = 0;
    vHeader.cyResolution    = 0;
    vHeader.cclrUsed        = 0;
    vHeader.cclrImportant   = 0;
    vHeader.usUnits         = BRU_METRIC;
    vHeader.usRecording     = BRA_BOTTOMUP;
    vHeader.usRendering     = BRH_NOTHALFTONED;
    vHeader.cSize1          = 0;
    vHeader.cSize2          = 0;
    vHeader.ulColorEncoding = 0;
    vHeader.ulIdentifier    = 0;

    vhPs = ::GpiCreatePS(habMain, hdc, &vSize, GPIA_ASSOC | PU_PELS);
    if (vhPs == 0)
    {
        wxLogLastError("GpiCreatePS Failure");
    }

    m_refData = pRefData;

    refData->m_width = nTheWidth;
    refData->m_height = nTheHeight;
    refData->m_depth = nNoBits;
    refData->m_numColors = 0;
    refData->m_selectedInto = NULL;

    HBITMAP hBmp = ::GpiCreateBitmap(hPs, &vHeader, 0L, NULL, &vInfo);
    if ( !hbmp )
    {
        wxLogLastError("CreateBitmap");
    }
    SetHBITMAP((WXHBITMAP)hbmp);
}

// Create from XPM data
wxBitmap::wxBitmap(
  char**                            ppData
, wxControl*                        WXUNUSED(pAnItem))
{
    Init();

F    (void)Create( (void *)ppData
                 ,wxBITMAP_TYPE_XPM_DATA
                 ,0
                 ,0
                 ,0
                );
}

wxBitmap::wxBitmap(
  int                               nW
, int                               nH
, int                               nD
)
{
    Init();

    (void)Create( nW
                 ,nH
                 ,nD
                );
}

wxBitmap::wxBitmap(
  void*                             pData
, long                              lType
, int                               nWidth
, int                               nHeight
, int                               nDepth
)
{
    Init();

    (void)Create( pData
                 ,lType
                 ,nWidth
                 ,nHeight
                 ,nDepth
                );
}

wxBitmap::wxBitmap(
  const wxString&                   rFilename
, long                              lType
)
{
    Init();

    LoadFile( rFilename
             ,(int)lType
            );
}

bool wxBitmap::Create(
  int                               nW
, int                               nH
, int                               nD
)
{
    HBITMAP                         hBmp;
    BITMAPINFOHEADER2               vHeader;
    BITMAPINFO2                     vInfo;
    HDC                             hDc;
    HPS                             hPs;
    DEVOPENSTRUCT                   vDop = { NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    SIZEL                           vSize = {0, 0};

    wxAssert(vHabmain != NULL);

    hDc = ::DevOpenDC(vHabmain, OD_MEMORY, (PSZ)"*", 1L, (PDEVOPENDATA)&vDop, 0L);

    vHeader.cbFix           = sizeof(vHeader);
    vHeader.cx              = (USHORT)nW;
    vHeader.cy              = (USHORT)nH;
    vHeader.cPlanes         = (USHORT)nD;
    vHeader.cBitCount       = 24;
    vHeader.ulCompression   = BCA_UNCOMP;
    vHeader.cxResolution    = 0;
    vHeader.cyResolution    = 0;
    vHeader.cclrUsed        = 0;
    vHeader.cclrImportant   = 0;
    vHeader.usUnits         = BRU_METRIC;
    vHeader.usRecording     = BRA_BOTTOMUP;
    vHeader.usRendering     = BRH_NOTHALFTONED;
    vHeader.cSize1          = 0;
    vHeader.cSize2          = 0;
    vHeader.ulColorEncoding = 0;
    vHeader.ulIdentifier    = 0;

    vhPs = ::GpiCreatePS(habMain, hdc, &vSize, GPIA_ASSOC | PU_PELS);
    if (vhPs == 0)
    {
        wxLogLastError("GpiCreatePS Failure");
    }


    UnRef();
    m_refData = new wxBitmapRefData;

    GetBitmapData()->m_width = nW;
    GetBitmapData()->m_height = nH;
    GetBitmapData()->m_depth = nD;

    if (nD > 0)
    {
        hBmp = ::GpiCreateBitmap(hPs, &vHeader, 0L, NULL, &vInfo);
        if (!hBmp)
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


