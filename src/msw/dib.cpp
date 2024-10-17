///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dib.cpp
// Purpose:     implements wxDIB class
// Author:      Vadim Zeitlin
// Created:     03.03.03 (replaces the old file with the same name)
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
    TODO: support for palettes is very incomplete, several functions simply
          ignore them (we should select and realize the palette, if any, before
          caling GetDIBits() in the DC we use with it.
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_WXDIB

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/bitmap.h"
    #include "wx/image.h"
#endif //WX_PRECOMP

#include "wx/file.h"
#include "wx/quantize.h"
#include "wx/scopedarray.h"

#include <stdio.h>
#include <stdlib.h>

#include <memory.h>

#include <memory>

#include "wx/msw/dib.h"

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

namespace
{

// calculate the number of palette entries needed for the bitmap with this
// number of bits per pixel
inline WORD GetNumberOfColours(WORD bitsPerPixel)
{
    // only 1, 4 and 8bpp bitmaps use palettes (well, they could be used with
    // 24bpp ones too but we don't support this as I think it's quite uncommon)
    return (WORD)(bitsPerPixel <= 8 ? 1 << bitsPerPixel : 0);
}

// wrapper around ::GetObject() for DIB sections
inline bool GetDIBSection(HBITMAP hbmp, DIBSECTION *ds)
{
    // note that GetObject() may return sizeof(DIBSECTION) for a bitmap
    // which is *not* a DIB section and the way to check for it is
    // by looking at the bits pointer
    return ::GetObject(hbmp, sizeof(DIBSECTION), ds) == sizeof(DIBSECTION) &&
                ds->dsBm.bmBits;
}

// for monochrome bitmaps, need bit twiddling functions to get at pixels
inline bool MonochromeLineReadBit(const unsigned char* srcLineStart, int index)
{
    const unsigned char* byte = srcLineStart + (index >> 3);
    int bit = 7 - (index & 7);
    unsigned char mask = 1 << bit;
    return (*byte & mask) != 0;
}

inline void MonochromeLineWriteBit(unsigned char* dstLineStart, int index, bool value)
{
    unsigned char* byte = dstLineStart + (index >> 3);
    int bit = 7 - (index & 7);
    unsigned char mask = ~(1 << bit);
    unsigned char newValue = value << bit;
    (*byte &= mask) |= newValue;
}

}
// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDIB creation
// ----------------------------------------------------------------------------

bool wxDIB::Create(int width, int height, int depth)
{
    // we don't support formats using palettes right now so we only create
    // either 24bpp (RGB) or 32bpp (RGBA) bitmaps
    wxASSERT_MSG( depth, wxT("invalid image depth in wxDIB::Create()") );
    if ( depth != 1 && depth < 24 )
        depth = 24;

    // allocate memory for bitmap structures
    BITMAPINFO info;
    wxZeroMemory(info);

    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;

    // we use positive height here which corresponds to a DIB with normal, i.e.
    // bottom to top, order -- normally using negative height (which means
    // reversed for MS and hence natural for all the normal people top to
    // bottom line scan order) could be used to avoid the need for the image
    // reversal in Create(image) but this doesn't work under NT, only Win9x!
    info.bmiHeader.biHeight = height;

    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = (WORD)depth;
    info.bmiHeader.biSizeImage = GetLineSize(width, depth)*height;

    m_handle = ::CreateDIBSection
                 (
                    0,              // hdc (unused with DIB_RGB_COLORS)
                    &info,          // bitmap description
                    DIB_RGB_COLORS, // use RGB, not palette
                    &m_data,        // [out] DIB bits
                    nullptr,        // don't use file mapping
                    0               // file mapping offset (not used here)
                 );

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

bool wxDIB::Create(HBITMAP hbmp, int depth /* = -1 */)
{
    wxCHECK_MSG( hbmp, false, wxT("wxDIB::Create(): invalid bitmap") );

    // this bitmap could already be a DIB section in which case we don't need
    // to convert it to DIB
    DIBSECTION ds;
    if ( GetDIBSection(hbmp, &ds) )
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
        // prepare all the info we need
        BITMAP bm;
        if ( !::GetObject(hbmp, sizeof(bm), &bm) )
        {
            wxLogLastError(wxT("GetObject(bitmap)"));

            return false;
        }

        int d = depth >= 1 ? depth : bm.bmBitsPixel;
        if ( d <= 0 )
            d = wxDisplayDepth();

        if ( !Create(bm.bmWidth, bm.bmHeight, d) || !CopyFromDDB(hbmp) )
            return false;
    }

    return true;
}

bool wxDIB::CopyFromDDB(HBITMAP hbmp)
{
    DIBSECTION ds;
    if ( !GetDIBSection(m_handle, &ds) )
    {
        // we're sure that our handle is a DIB section, so this should work
        wxFAIL_MSG( wxT("GetObject(DIBSECTION) unexpectedly failed") );

        return false;
    }

    if ( !::GetDIBits
            (
                ScreenHDC(),                // the DC to use
                hbmp,                       // the source DDB
                0,                          // first scan line
                m_height,                   // number of lines to copy
                ds.dsBm.bmBits,             // pointer to the buffer
                (BITMAPINFO *)&ds.dsBmih,   // bitmap header
                DIB_RGB_COLORS              // and not DIB_PAL_COLORS
            ) )
    {
        wxLogLastError(wxT("GetDIBits()"));

        return false;
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
                            filename.t_str(),
                            IMAGE_BITMAP,
                            0, 0, // don't specify the size
                            LR_CREATEDIBSECTION | LR_LOADFROMFILE
                         );

    if ( !m_handle )
    {
        wxLogLastError(wxT("Loading DIB from file"));

        return false;
    }

    return true;
}

bool wxDIB::Save(const wxString& filename)
{
    wxCHECK_MSG( m_handle, false, wxT("wxDIB::Save(): invalid object") );

#if wxUSE_FILE
    wxFile file(filename, wxFile::write);
    bool ok = file.IsOpened();
    if ( ok )
    {
        DIBSECTION ds;
        if ( !GetDIBSection(m_handle, &ds) )
        {
            wxLogLastError(wxT("GetObject(hDIB)"));
        }
        else
        {
            BITMAPFILEHEADER bmpHdr;
            wxZeroMemory(bmpHdr);

            const size_t sizeHdr = ds.dsBmih.biSize;
            const size_t sizeImage = ds.dsBmih.biSizeImage;

            // provide extra space so we can verify that
            // monochrome DIB's color table is size 2
            RGBQUAD monoBmiColors[3];
            UINT nColors = 0;
            if ( ds.dsBmih.biBitCount == 1 )
            {
                MemoryHDC hDC;
                SelectInHDC sDC(hDC, m_handle);
                nColors = GetDIBColorTable(hDC, 0, WXSIZEOF(monoBmiColors), monoBmiColors);
                if ( nColors != 2 )
                {
                    wxLogLastError(wxT("GetDIBColorTable"));
                    return false;
                }
            }
            const size_t colorTableSize = ds.dsBmih.biBitCount == 1
                                            ? nColors * sizeof(monoBmiColors[0])
                                            : 0;

            bmpHdr.bfType = 0x4d42;    // 'BM' in little endian
            bmpHdr.bfOffBits = sizeof(BITMAPFILEHEADER);
            bmpHdr.bfOffBits += ds.dsBmih.biSize;
            bmpHdr.bfOffBits += colorTableSize;
            bmpHdr.bfSize = bmpHdr.bfOffBits + sizeImage;

            // first write the file header, then the bitmap header and finally the
            // bitmap data itself
            ok = file.Write(&bmpHdr, sizeof(bmpHdr)) == sizeof(bmpHdr) &&
                    file.Write(&ds.dsBmih, sizeHdr) == sizeHdr &&
                        (!colorTableSize || file.Write(monoBmiColors, colorTableSize)) &&
                            file.Write(ds.dsBm.bmBits, sizeImage) == sizeImage;
        }
    }
#else // !wxUSE_FILE
    bool ok = false;
#endif // wxUSE_FILE/!wxUSE_FILE

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
        if ( !GetDIBSection(m_handle, &ds) )
        {
            wxLogLastError(wxT("GetObject(hDIB)"));
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
    wxCHECK_MSG( m_handle, 0, wxT("wxDIB::CreateDDB(): invalid object") );

    DIBSECTION ds;
    if ( !GetDIBSection(m_handle, &ds) )
    {
        wxLogLastError(wxT("GetObject(hDIB)"));

        return 0;
    }

    // how many colours are we going to have in the palette?
    DWORD biClrUsed = ds.dsBmih.biClrUsed;
    if ( !biClrUsed )
    {
        // biClrUsed field might not be set
        biClrUsed = GetNumberOfColours(ds.dsBmih.biBitCount);
    }

    if ( !biClrUsed )
    {
        return ConvertToBitmap((BITMAPINFO *)&ds.dsBmih, hdc, ds.dsBm.bmBits);
    }
    else
    {
        // fake a BITMAPINFO w/o bits, just the palette info
        wxCharBuffer bmi(sizeof(BITMAPINFO) + (biClrUsed - 1)*sizeof(RGBQUAD));
        BITMAPINFO *pBmi = (BITMAPINFO *)bmi.data();
        MemoryHDC hDC;
        // get the colour table
        SelectInHDC sDC(hDC, m_handle);
        ::GetDIBColorTable(hDC, 0, biClrUsed, pBmi->bmiColors);
        memcpy(&pBmi->bmiHeader, &ds.dsBmih, ds.dsBmih.biSize);

        return ConvertToBitmap(pBmi, hdc, ds.dsBm.bmBits);
    }
}

/* static */
HBITMAP wxDIB::ConvertToBitmap(const BITMAPINFO *pbmi, HDC hdc, const void *bits)
{
    wxCHECK_MSG( pbmi, 0, wxT("invalid DIB in ConvertToBitmap") );

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
                // with a classic BITMAPINFOHEADER, there are 3 colour-mask DWORDs
                // after the header. Otherwise the masks are part of the header
                numColors = pbmih->biSize == sizeof(BITMAPINFOHEADER) ? 3 : 0;
                break;

            case BI_RGB:
            case BI_RLE8:
            case BI_RLE4:
                // biClrUsed has the number of colors but it may be not initialized at
                // all
                numColors = pbmih->biClrUsed;
                if ( !numColors )
                {
                    numColors = GetNumberOfColours(pbmih->biBitCount);
                }
                break;

            default:
                // no idea how it should be calculated for the other cases
                numColors = 0;
        }

        // pbmih->biSize might not be the same as sizeof(BITMAPINFOHEADER)
        // (such as in the case of a BITMAPV4HEADER or BITMAPV5HEADER);
        // we need to advance by the number of bytes actually present
        bits = reinterpret_cast<const char*>(pbmih) + pbmih->biSize + numColors * sizeof(RGBQUAD);
    }

    HBITMAP hbmp = ::CreateDIBitmap
                     (
                        hdc
                            ? hdc           // create bitmap compatible
                            : pbmih->biBitCount == 1
                                ? (HDC) MemoryHDC()
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

    const bool wantSizeOnly = pbi == nullptr;
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
    DWORD dwLen = bi.biSize + GetNumberOfColours(bm.bmBitsPixel) * sizeof(RGBQUAD);

    // get either just the image size or the image bits
    if ( !::GetDIBits
            (
                ScreenHDC(),                        // the DC to use
                hbmp,                               // the source DDB
                0,                                  // first scan line
                h,                                  // number of lines to copy
                wantSizeOnly ? nullptr              // pointer to the buffer or
                             : (char *)pbi + dwLen, // nullptr if we don't have it
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
    const size_t size = ConvertFromBitmap(nullptr, hbmp);
    if ( !size )
    {
        // conversion to DDB failed?
        return nullptr;
    }

    HGLOBAL hDIB = ::GlobalAlloc(GMEM_MOVEABLE, size);
    if ( !hDIB )
    {
        // this is an error which the user may understand so let him
        // know about it
        wxLogError(_("Failed to allocate %luKb of memory for bitmap data."),
                   (unsigned long)(size / 1024));

        return nullptr;
    }

    if ( !ConvertFromBitmap((BITMAPINFO *)(void *)GlobalPtrLock(hDIB), hbmp) )
    {
        // this really shouldn't happen... it worked the first time, why not
        // now?
        wxFAIL_MSG( wxT("wxDIB::ConvertFromBitmap() unexpectedly failed") );

        return nullptr;
    }

    return hDIB;
}

// ----------------------------------------------------------------------------
// palette support
// ----------------------------------------------------------------------------

#if defined(__WXMSW__) && wxUSE_PALETTE

wxPalette *wxDIB::CreatePalette() const
{
    wxCHECK_MSG( m_handle, nullptr, wxT("wxDIB::CreatePalette(): invalid object") );

    DIBSECTION ds;
    if ( !GetDIBSection(m_handle, &ds) )
    {
        wxLogLastError(wxT("GetObject(hDIB)"));

        return 0;
    }

    // how many colours are we going to have in the palette?
    DWORD biClrUsed = ds.dsBmih.biClrUsed;
    if ( !biClrUsed )
    {
        // biClrUsed field might not be set
        biClrUsed = GetNumberOfColours(ds.dsBmih.biBitCount);
    }

    if ( !biClrUsed )
    {
        // bitmaps of this depth don't have palettes at all
        //
        // NB: another possibility would be to return
        //     GetStockObject(DEFAULT_PALETTE) or even CreateHalftonePalette()?
        return nullptr;
    }

    MemoryHDC hDC;

    // LOGPALETTE struct has only 1 element in palPalEntry array, we're
    // going to have biClrUsed of them so add necessary space
    LOGPALETTE *pPalette = (LOGPALETTE *)
        malloc(sizeof(LOGPALETTE) + (biClrUsed - 1)*sizeof(PALETTEENTRY));
    wxCHECK_MSG( pPalette, nullptr, wxT("out of memory") );

    // initialize the palette header
    pPalette->palVersion = 0x300;  // magic number, not in docs but works
    pPalette->palNumEntries = (WORD)biClrUsed;

    // and the colour table
    wxCharBuffer rgb(sizeof(RGBQUAD) * biClrUsed);
    RGBQUAD *pRGB = (RGBQUAD*)rgb.data();
    SelectInHDC selectHandle(hDC, m_handle);
    ::GetDIBColorTable(hDC, 0, biClrUsed, pRGB);
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
        wxLogLastError(wxT("CreatePalette"));

        return nullptr;
    }

    wxPalette *palette = new wxPalette;
    palette->SetHPALETTE((WXHPALETTE)hPalette);

    return palette;
}

#endif // defined(__WXMSW__) && wxUSE_PALETTE

// ----------------------------------------------------------------------------
// wxImage support
// ----------------------------------------------------------------------------

#if wxUSE_IMAGE

bool wxDIB::Create(const wxImage& image, PixelFormat pf, int dstDepth)
{
    wxCHECK_MSG( image.IsOk(), false, wxT("invalid wxImage in wxDIB ctor") );
#if !wxUSE_PALETTE
    wxCHECK_MSG(dstDepth != 1, false,
        "wxImage conversion to monochrome bitmap requires wxUSE_PALETTE");
#endif

    const int h = image.GetHeight();
    const int w = image.GetWidth();

    // if we have alpha channel, we need to create a 32bpp RGBA DIB, otherwise
    // a 24bpp RGB is sufficient
    // but use monochrome if requested (to support wxMask)
    const bool hasAlpha = image.HasAlpha();
    wxCHECK_MSG(!hasAlpha || dstDepth != 1, false, "alpha not supported in monochrome bitmaps");
    const int srcBpp = hasAlpha ? 32 : 24;
    dstDepth = dstDepth != -1 ? dstDepth : srcBpp;

    if ( !Create(w, h, dstDepth) )
        return false;

    // if requested, convert wxImage's content to monochrome
    wxScopedArray<unsigned char> eightBitData;
#if wxUSE_PALETTE
    if ( dstDepth == 1 )
    {
        wxImage quantized;
        wxPalette* tempPalette;
        unsigned char* tempEightBitData;
        if ( !wxQuantize::Quantize(
            image,
            quantized,
            &tempPalette,
            2,
            &tempEightBitData,
            wxQUANTIZE_RETURN_8BIT_DATA) )
        {
            return false;
        }
        std::unique_ptr<wxPalette> palette(tempPalette);
        eightBitData.reset(tempEightBitData);

        // use palette's colors in result bitmap
        MemoryHDC hDC;
        SelectInHDC sDC(hDC, m_handle);
        RGBQUAD colorTable[2];
        for ( UINT i = 0; i < WXSIZEOF(colorTable); ++i )
        {
            if ( !palette->GetRGB(i,
                                    &colorTable[i].rgbRed,
                                    &colorTable[i].rgbGreen,
                                    &colorTable[i].rgbBlue) )
            {
                return false;
            }
            colorTable[i].rgbReserved = 0;
        }
        UINT rc = SetDIBColorTable(hDC, 0, WXSIZEOF(colorTable), colorTable);
        if ( rc != WXSIZEOF(colorTable))
        {
            wxLogLastError(wxT("SetDIBColorTable"));
            return false;
        }
    }
#endif // wxUSE_PALETTE

    // DIBs are stored in bottom to top order (see also the comment above in
    // Create()) so we need to copy bits line by line and starting from the end
    // N.B.:  srcBytesPerLine varies with dstDepth because dstDepth == 1 uses quantized input
    const int srcBytesPerLine = dstDepth != 1 ? w * 3 : w;
    const int dstBytesPerLine = GetLineSize(w, dstDepth);
    const unsigned char *src = (dstDepth != 1 ? image.GetData() : eightBitData.get()) + ((h - 1) * srcBytesPerLine);
    const unsigned char *alpha = hasAlpha ? image.GetAlpha() + (h - 1)*w
                                          : nullptr;
    unsigned char *dstLineStart = (unsigned char *)m_data;
    for ( int y = 0; y < h; y++ )
    {
        // Copy one DIB line. Note that RGB components order is reversed in
        // Windows bitmaps compared to wxImage and is actually BGR.
        unsigned char *dst = dstLineStart;
        if ( alpha )
        {
            int x;

            switch ( pf )
            {
                case PixelFormat_PreMultiplied:
                    // Pre-multiply pixel values so that the DIB could be used
                    // with ::AlphaBlend().
                    for ( x = 0; x < w; x++ )
                    {
                        const unsigned char a = *alpha++;
                        *dst++ = (unsigned char)((src[2] * a + 127) / 255);
                        *dst++ = (unsigned char)((src[1] * a + 127) / 255);
                        *dst++ = (unsigned char)((src[0] * a + 127) / 255);
                        *dst++ = a;
                        src += 3;
                    }
                    break;

                case PixelFormat_NotPreMultiplied:
                    // Just copy pixel data without changing it.
                    for ( x = 0; x < w; x++ )
                    {
                        *dst++ = src[2];
                        *dst++ = src[1];
                        *dst++ = src[0];

                        *dst++ = *alpha++;
                        src += 3;
                    }
                    break;
            }

        }
        else // no alpha channel
        {
            if ( dstDepth != 1 )
            {
                for ( int x = 0; x < w; x++ )
                {
                    *dst++ = src[2];
                    *dst++ = src[1];
                    *dst++ = src[0];

                    // If source has no alpha, but the DIB does have it, we
                    // need to still fill it, even if there is no real
                    // transparency.
                    if ( dstDepth == 32 )
                        *dst++ = wxALPHA_OPAQUE;

                    src += 3;
                }
            }
            else
            {
                for ( int x = 0; x < w; x++ )
                {
                    MonochromeLineWriteBit(dstLineStart, x, src[0] != 0);
                    ++src;
                }
            }
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

wxImage wxDIB::ConvertToImage(ConversionFlags flags) const
{
    wxCHECK_MSG( IsOk(), wxNullImage,
                    wxT("can't convert invalid DIB to wxImage") );

    // create the wxImage object
    const int w = GetWidth();
    const int h = GetHeight();
    wxImage image(w, h, false /* don't bother clearing memory */);
    if ( !image.IsOk() )
    {
        wxFAIL_MSG( wxT("could not allocate data for image") );
        return wxNullImage;
    }

    const int bpp = GetDepth();

    // Remember if we have any "real" transparency, i.e. either any partially
    // transparent pixels or not all pixels are fully opaque or fully
    // transparent.
    bool hasAlpha = false;
    bool hasOpaque = false;
    bool hasTransparent = false;

    if ( bpp == 32 )
    {
        // 32 bit bitmaps may be either 0RGB or ARGB and we don't know in
        // advance which one do we have so suppose we have alpha of them and
        // get rid of it later if it turns out we didn't.
        image.SetAlpha();
    }

    // this is the same loop as in Create() just above but with copy direction
    // reversed
    const int dstBytesPerLine = w * 3;
    const int srcBytesPerLine = GetLineSize(w, bpp);
    unsigned char *dst = image.GetData() + ((h - 1) * dstBytesPerLine);
    unsigned char *alpha = image.HasAlpha() ? image.GetAlpha() + (h - 1)*w
                                            : nullptr;
    const unsigned char *srcLineStart = (unsigned char *)GetData();
    for ( int y = 0; y < h; y++ )
    {
        // copy one DIB line
        const unsigned char *src = srcLineStart;
        if ( bpp != 1 )
        {
            for ( int x = 0; x < w; x++ )
            {
                dst[2] = *src++;
                dst[1] = *src++;
                dst[0] = *src++;

                if ( bpp == 32 )
                {
                    // wxImage uses non premultiplied alpha so undo
                    // premultiplication done in Create() above
                    const unsigned char a = *src;
                    *alpha++ = a;

                    // Check what kind of alpha do we have.
                    switch ( a )
                    {
                        case 0:
                            hasTransparent = true;
                            break;

                        default:
                            // Anything in between means we have real transparency
                            // and must use alpha channel.
                            hasAlpha = true;
                            break;

                        case 255:
                            hasOpaque = true;
                            break;
                    }

                    if ( a > 0 )
                    {
                        dst[0] = (dst[0] * 255) / a;
                        dst[1] = (dst[1] * 255) / a;
                        dst[2] = (dst[2] * 255) / a;
                    }

                    src++;
                }

                dst += 3;
            }
        }
        else
        {
            for ( int x = 0; x < w; x++ )
            {
                unsigned char value = MonochromeLineReadBit(srcLineStart, x)
                                        ? 255
                                        : 0;
                dst[2] = value;
                dst[1] = value;
                dst[0] = value;

                dst += 3;
            }
        }

        // pass to the previous line in the image
        dst -= 2*dstBytesPerLine;
        if ( alpha )
            alpha -= 2*w;

        // and to the next one in the DIB
        srcLineStart += srcBytesPerLine;
    }

    if ( hasOpaque && hasTransparent )
        hasAlpha = true;

    if ( !hasAlpha && image.HasAlpha() && flags == Convert_AlphaAuto )
        image.ClearAlpha();

    return image;
}

#endif // wxUSE_IMAGE

#endif // wxUSE_WXDIB
