///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dib.cpp
// Purpose:     implements wxDIB class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.03.03 (replaces the old file with the same name)
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
    TODO: support for palettes is very incomplete, several functions simply
          ignore them (we should select and realize the palette, if any, before
          caling GetDIBits() in the DC we use with it and we shouldn't use
          GetBitmapBits() at all because we can't do it with it)
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
#endif //WX_PRECOMP

#include "wx/bitmap.h"
#include "wx/intl.h"
#include "wx/file.h"

#include <stdio.h>
#include <stdlib.h>

#if !defined(__MWERKS__) && !defined(__SALFORDC__)
#include <memory.h>
#endif

#ifdef __GNUWIN32_OLD__
    #include "wx/msw/gnuwin32/extra.h"
#endif

#include "wx/image.h"
#include "wx/msw/dib.h"

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// calculate the number of palette entries needed for the bitmap with this
// number of bits per pixel
static WORD wxGetNumOfBitmapColors(WORD bitsPerPixel)
{
    // only 1, 4 and 8bpp bitmaps use palettes (well, they could be used with
    // 24bpp ones too but we don't support this as I think it's quite uncommon)
    return bitsPerPixel <= 8 ? 1 << bitsPerPixel : 0;
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDIB creation
// ----------------------------------------------------------------------------

bool wxDIB::Create(int width, int height, int depth)
{
    // we don't handle the palette yet
    wxASSERT_MSG( depth == 24 || depth == 32,
                    _T("unsupported image depth in wxDIB::Create()") );

    static const int infosize = sizeof(BITMAPINFOHEADER);

    BITMAPINFO *info = (BITMAPINFO *)malloc(infosize);
    wxCHECK_MSG( info, false, _T("malloc(BITMAPINFO) failed") );

    memset(info, 0, infosize);

    info->bmiHeader.biSize = infosize;
    info->bmiHeader.biWidth = width;

    // we use positive height here which corresponds to a DIB with normal, i.e.
    // bottom to top, order -- normally using negative height (which means
    // reversed for MS and hence natural for all the normal people top to
    // bottom line scan order) could be used to avoid the need for the image
    // reversal in Create(image) but this doesn't work under NT, only Win9x!
    info->bmiHeader.biHeight = height;

    info->bmiHeader.biPlanes = 1;
    info->bmiHeader.biBitCount = depth;
    info->bmiHeader.biSizeImage = GetLineSize(width, depth)*height;

    m_handle = ::CreateDIBSection
                 (
                    0,              // hdc (unused with DIB_RGB_COLORS)
                    info,           // bitmap description
                    DIB_RGB_COLORS, // use RGB, not palette
                    &m_data,        // [out] DIB bits
                    NULL,           // don't use file mapping
                    0               // file mapping offset (not used here)
                 );

    free(info);

    if ( !m_handle )
    {
        wxLogLastError(wxT("CreateDIBSection"));

        return false;
    }

    m_width = width;
    m_height = height;
    m_depth = depth;

    return true;
}

bool wxDIB::Create(const wxBitmap& bmp)
{
    wxCHECK_MSG( bmp.Ok(), false, _T("wxDIB::Create(): invalid bitmap") );

    // this bitmap could already be a DIB section in which case we don't need
    // to convert it to DIB
    HBITMAP hbmp = GetHbitmapOf(bmp);

    DIBSECTION ds;
    if ( ::GetObject(hbmp, sizeof(ds), &ds) == sizeof(ds) )
    {
        m_handle = hbmp;

        // wxBitmap will free it, not we
        m_ownsHandle = false;

        // copy all the bitmap parameters too as we have them now anyhow
        m_width = ds.dsBm.bmWidth;
        m_height = ds.dsBm.bmHeight;
        m_depth = ds.dsBm.bmBitsPixel;

        m_data = ds.dsBm.bmBits;
    }
    else // no, it's a DDB -- convert it to DIB
    {
        const int w = bmp.GetWidth();
        const int h = bmp.GetHeight();
        int d = bmp.GetDepth();
        if ( d == -1 )
            d = wxDisplayDepth();

        if ( !Create(w, h, d) )
            return false;

        // we could have used GetDIBits() too but GetBitmapBits() is simpler
        if ( !::GetBitmapBits
                (
                    GetHbitmapOf(bmp),      // the source DDB
                    GetLineSize(w, d)*h,    // the number of bytes to copy
                    m_data                  // the pixels will be copied here
                ) )
        {
            wxLogLastError(wxT("GetDIBits()"));

            return 0;
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
// Loading/saving the DIBs
// ----------------------------------------------------------------------------

bool wxDIB::Load(const wxString& filename)
{
    m_handle = (HBITMAP)::LoadImage
                         (
                            wxGetInstance(),
                            filename,
                            IMAGE_BITMAP,
                            0, 0, // don't specify the size
                            LR_CREATEDIBSECTION | LR_LOADFROMFILE
                         );
    if ( !m_handle )
    {
        wxLogLastError(_T("LoadImage(LR_CREATEDIBSECTION | LR_LOADFROMFILE)"));

        return false;
    }

    return true;
}

bool wxDIB::Save(const wxString& filename)
{
    wxCHECK_MSG( m_handle, false, _T("wxDIB::Save(): invalid object") );

    wxFile file(filename, wxFile::write);
    bool ok = file.IsOpened();
    if ( ok )
    {
        DIBSECTION ds;
        if ( !::GetObject(m_handle, sizeof(ds), &ds) )
        {
            wxLogLastError(_T("GetObject(hDIB)"));
        }
        else
        {
            BITMAPFILEHEADER bmpHdr;
            wxZeroMemory(bmpHdr);

            const size_t sizeHdr = ds.dsBmih.biSize;
            const size_t sizeImage = ds.dsBmih.biSizeImage;

            bmpHdr.bfType = 0x4d42;    // 'BM' in little endian
            bmpHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + ds.dsBmih.biSize;
            bmpHdr.bfSize = bmpHdr.bfOffBits + sizeImage;

            // first write the file header, then the bitmap header and finally the
            // bitmap data itself
            ok = file.Write(&bmpHdr, sizeof(bmpHdr)) == sizeof(bmpHdr) &&
                    file.Write(&ds.dsBmih, sizeHdr) == sizeHdr &&
                        file.Write(ds.dsBm.bmBits, sizeImage) == sizeImage;
        }
    }

    if ( !ok )
    {
        wxLogError(_("Failed to save the bitmap image to file \"%s\"."),
                   filename.c_str());
    }

    return ok;
}

// ----------------------------------------------------------------------------
// wxDIB accessors
// ----------------------------------------------------------------------------

void wxDIB::DoGetObject() const
{
    // only do something if we have a valid DIB but we don't [yet] have valid
    // data
    if ( m_handle && !m_data )
    {
        // although all the info we need is in BITMAP and so we don't really
        // need DIBSECTION we still ask for it as modifying the bit values only
        // works for the real DIBs and not for the bitmaps and it's better to
        // check for this now rather than trying to find out why it doesn't
        // work later
        DIBSECTION ds;
        if ( !::GetObject(m_handle, sizeof(ds), &ds) )
        {
            wxLogLastError(_T("GetObject(hDIB)"));
            return;
        }

        wxDIB *self = wxConstCast(this, wxDIB);

        self->m_width = ds.dsBm.bmWidth;
        self->m_height = ds.dsBm.bmHeight;
        self->m_depth = ds.dsBm.bmBitsPixel;
        self->m_data = ds.dsBm.bmBits;
    }
}

// ----------------------------------------------------------------------------
// DDB <-> DIB conversions
// ----------------------------------------------------------------------------

HBITMAP wxDIB::CreateDDB(HDC hdc) const
{
    wxCHECK_MSG( m_handle, 0, _T("wxDIB::CreateDDB(): invalid object") );

    DIBSECTION ds;
    if ( !::GetObject(m_handle, sizeof(ds), &ds) )
    {
        wxLogLastError(_T("GetObject(hDIB)"));

        return 0;
    }

    return ConvertToBitmap((BITMAPINFO *)&ds.dsBmih, hdc, ds.dsBm.bmBits);
}

/* static */
HBITMAP wxDIB::ConvertToBitmap(const BITMAPINFO *pbmi, HDC hdc, void *bits)
{
    wxCHECK_MSG( pbmi, 0, _T("invalid DIB in ConvertToBitmap") );

    // here we get BITMAPINFO struct followed by the actual bitmap bits and
    // BITMAPINFO starts with BITMAPINFOHEADER followed by colour info
    const BITMAPINFOHEADER *pbmih = &pbmi->bmiHeader;

    // get the pointer to the start of the real image data if we have a plain
    // DIB and not a DIB section (in the latter case the pointer must be passed
    // to us by the caller)
    if ( !bits )
    {
        // we must skip over the colour table to get to the image data
        //
        // colour table either has the real colour data in which case its
        // number of entries is given by biClrUsed or is used for masks to be
        // used for extracting colour information from true colour bitmaps in
        // which case it always have exactly 3 DWORDs
        int numColors;
        switch ( pbmih->biCompression )
        {
            case BI_BITFIELDS:
                numColors = 3;
                break;

            case BI_RGB:
                // biClrUsed has the number of colors but it may be not initialized at
                // all
                numColors = pbmih->biClrUsed;
                if ( !numColors )
                {
                    numColors = wxGetNumOfBitmapColors(pbmih->biBitCount);
                }
                break;

            default:
                // no idea how it should be calculated for the other cases
                numColors = 0;
        }

        bits = (char *)pbmih + sizeof(*pbmih) + numColors*sizeof(RGBQUAD);
    }

    HBITMAP hbmp = ::CreateDIBitmap
                     (
                        hdc ? hdc           // create bitmap compatible
                            : (HDC) ScreenHDC(),  //  with this DC
                        pbmih,              // used to get size &c
                        CBM_INIT,           // initialize bitmap bits too
                        bits,               // ... using this data
                        pbmi,               // this is used for palette only
                        DIB_RGB_COLORS      // direct or indexed palette?
                     );

    if ( !hbmp )
    {
        wxLogLastError(wxT("CreateDIBitmap"));
    }

    return hbmp;
}

/* static */
size_t wxDIB::ConvertFromBitmap(BITMAPINFO *pbi, HBITMAP hbmp)
{
    wxASSERT_MSG( hbmp, wxT("invalid bmp can't be converted to DIB") );

    // prepare all the info we need
    BITMAP bm;
    if ( !::GetObject(hbmp, sizeof(bm), &bm) )
    {
        wxLogLastError(wxT("GetObject(bitmap)"));

        return 0;
    }

    // we need a BITMAPINFO anyhow and if we're not given a pointer to it we
    // use this one
    BITMAPINFO bi2;

    const bool wantSizeOnly = pbi == NULL;
    if ( wantSizeOnly )
        pbi = &bi2;

    // just for convenience
    const int h = bm.bmHeight;

    // init the header
    BITMAPINFOHEADER& bi = pbi->bmiHeader;
    wxZeroMemory(bi);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bm.bmWidth;
    bi.biHeight = h;
    bi.biPlanes = 1;
    bi.biBitCount = bm.bmBitsPixel;

    // memory we need for BITMAPINFO only
    DWORD dwLen = bi.biSize + wxGetNumOfBitmapColors(bm.bmBitsPixel) * sizeof(RGBQUAD);

    // get either just the image size or the image bits
    if ( !::GetDIBits
            (
                ScreenHDC(),                        // the DC to use
                hbmp,                               // the source DDB
                0,                                  // first scan line
                h,                                  // number of lines to copy
                wantSizeOnly ? NULL                 // pointer to the buffer or
                             : (char *)pbi + dwLen, // NULL if we don't have it
                pbi,                                // bitmap header
                DIB_RGB_COLORS                      // or DIB_PAL_COLORS
            ) )
    {
        wxLogLastError(wxT("GetDIBits()"));

        return 0;
    }

    // return the total size
    return dwLen + bi.biSizeImage;
}

/* static */
HGLOBAL wxDIB::ConvertFromBitmap(HBITMAP hbmp)
{
    // first calculate the size needed
    const size_t size = ConvertFromBitmap(NULL, hbmp);
    if ( !size )
    {
        // conversion to DDB failed?
        return NULL;
    }

    HGLOBAL hDIB = ::GlobalAlloc(GMEM_MOVEABLE, size);
    if ( !hDIB )
    {
        // this is an error which does risk to happen especially under Win9x
        // and which the user may understand so let him know about it
        wxLogError(_("Failed to allocated %luKb of memory for bitmap data."),
                   (unsigned long)(size / 1024));

        return NULL;
    }

    if ( !ConvertFromBitmap((BITMAPINFO *)GlobalPtr(hDIB), hbmp) )
    {
        // this really shouldn't happen... it worked the first time, why not
        // now?
        wxFAIL_MSG( _T("wxDIB::ConvertFromBitmap() unexpectedly failed") );

        return NULL;
    }

    return hDIB;
}

// ----------------------------------------------------------------------------
// palette support
// ----------------------------------------------------------------------------

#if wxUSE_PALETTE

wxPalette *wxDIB::CreatePalette() const
{
    wxCHECK_MSG( m_handle, NULL, _T("wxDIB::CreatePalette(): invalid object") );

    DIBSECTION ds;
    if ( !::GetObject(m_handle, sizeof(ds), &ds) )
    {
        wxLogLastError(_T("GetObject(hDIB)"));

        return 0;
    }

    // how many colours are we going to have in the palette?
    DWORD biClrUsed = ds.dsBmih.biClrUsed;
    if ( !biClrUsed )
    {
        // biClrUsed field might not be set
        biClrUsed = wxGetNumOfBitmapColors(ds.dsBmih.biBitCount);
    }

    if ( !biClrUsed )
    {
        // bitmaps of this depth don't have palettes at all
        //
        // NB: another possibility would be to return
        //     GetStockObject(DEFAULT_PALETTE) or even CreateHalftonePalette()?
        return NULL;
    }

    // LOGPALETTE struct has only 1 element in palPalEntry array, we're
    // going to have biClrUsed of them so add necessary space
    LOGPALETTE *pPalette = (LOGPALETTE *)
        malloc(sizeof(LOGPALETTE) + (biClrUsed - 1)*sizeof(PALETTEENTRY));
    wxCHECK_MSG( pPalette, NULL, _T("out of memory") );

    // initialize the palette header
    pPalette->palVersion = 0x300;  // magic number, not in docs but works
    pPalette->palNumEntries = biClrUsed;

    // and the colour table (it starts right after the end of the header)
    const RGBQUAD *pRGB = (RGBQUAD *)((char *)&ds.dsBmih + ds.dsBmih.biSize);
    for ( DWORD i = 0; i < biClrUsed; i++, pRGB++ )
    {
        pPalette->palPalEntry[i].peRed = pRGB->rgbRed;
        pPalette->palPalEntry[i].peGreen = pRGB->rgbGreen;
        pPalette->palPalEntry[i].peBlue = pRGB->rgbBlue;
        pPalette->palPalEntry[i].peFlags = 0;
    }

    HPALETTE hPalette = ::CreatePalette(pPalette);

    free(pPalette);

    if ( !hPalette )
    {
        wxLogLastError(_T("CreatePalette"));

        return NULL;
    }

    wxPalette *palette = new wxPalette;
    palette->SetHPALETTE((WXHPALETTE)hPalette);

    return palette;
}

#endif // wxUSE_PALETTE

// ----------------------------------------------------------------------------
// wxImage support
// ----------------------------------------------------------------------------

#if wxUSE_IMAGE

bool wxDIB::Create(const wxImage& image)
{
    wxCHECK_MSG( image.Ok(), false, _T("invalid wxImage in wxDIB ctor") );

    const int h = image.GetHeight();
    const int w = image.GetWidth();

    // if we have alpha channel, we need to create a 32bpp RGBA DIB, otherwise
    // a 24bpp RGB is sufficient
    const bool hasAlpha = image.HasAlpha();
    const int bpp = hasAlpha ? 32 : 24;

    if ( !Create(w, h, bpp) )
        return false;

    // DIBs are stored in bottom to top order (see also the comment above in
    // Create()) so we need to copy bits line by line and starting from the end
    const int srcBytesPerLine = w * 3;
    const int dstBytesPerLine = GetLineSize(w, bpp);
    const unsigned char *src = image.GetData() + ((h - 1) * srcBytesPerLine);
    const unsigned char *alpha = hasAlpha ? image.GetAlpha() + (h - 1)*w : NULL;
    unsigned char *dstLineStart = (unsigned char *)m_data;
    for ( int y = 0; y < h; y++ )
    {
        // copy one DIB line
        unsigned char *dst = dstLineStart;
        for ( int x = 0; x < w; x++ )
        {
            // also, the order of RGB is inversed for DIBs
            *dst++ = src[2];
            *dst++ = src[1];
            *dst++ = src[0];

            src += 3;

            if ( alpha )
                *dst++ = *alpha++;
        }

        // pass to the previous line in the image
        src -= 2*srcBytesPerLine;
        if ( alpha )
            alpha -= 2*w;

        // and to the next one in the DIB
        dstLineStart += dstBytesPerLine;
    }

    return true;
}

#endif // wxUSE_IMAGE

