/////////////////////////////////////////////////////////////////////////////
// Name:        imagbmp.cpp
// Purpose:     wxImage BMP handler
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "imagbmp.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/imagbmp.h"
#include "wx/bitmap.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/app.h"
#include "wx/filefn.h"
#include "wx/wfstream.h"
#include "wx/intl.h"
#include "wx/module.h"

// For memcpy
#include <string.h>

#ifdef __SALFORDC__
#ifdef FAR
#undef FAR
#endif
#endif

#ifdef __WXMSW__
#include <windows.h>
#endif

//-----------------------------------------------------------------------------
// wxBMPHandler
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxBMPHandler,wxImageHandler)

#ifdef __WIN32__

#include <wx/msw/private.h>

static void OutOfMemory(size_t nBytesNeeded)
{
    wxLogError(_("Failed to allocate %u bytes of memory."), nBytesNeeded);
}

// VZ: I'm not an expert in bitmaps, this is a quick and dirty C function I
//     wrote some time ago and it probably doesn't deal properly with all
//     bitmaps - any corrections/improvements are more than welcome.
bool wxBMPHandler::SaveFile(wxImage *image,
                            wxOutputStream& stream,
                            bool verbose)
{
    // get HBITMAP first
    wxCHECK_MSG( image, FALSE, _T("invalid pointer in wxBMPHandler::SaveFile") );

    wxBitmap bitmap = image->ConvertToBitmap();
    HBITMAP hBmp = GetHbitmapOf(bitmap);
    wxCHECK_MSG( hBmp, FALSE, _T("can't save invalid bitmap") );

    // actual C code starts here
    BITMAPFILEHEADER hdr;
    BITMAPINFOHEADER *pbih;
    BITMAP bmp;
    WORD nClrBits;
    size_t sizeOfBmi, sizeColTable;
    BITMAPINFO *pbmi = NULL;
    void *bmpData = NULL;
    HDC hdc = NULL;
    int rc = 0;

    /*
       create and initialize BITMAPINFO
     */

    /* get the number of bitmap colours and its size */
    if ( !GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp) ) {
        wxLogLastError(_T("GetObject"));

        goto cleanup;
    }

    /* calc the number of colour bits needed */
    nClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if  ( nClrBits == 1 ) nClrBits = 1;
    else if ( nClrBits <= 4 ) nClrBits = 4;
    else if ( nClrBits <= 8 ) nClrBits = 8;
    else if ( nClrBits <= 16 ) nClrBits = 16;
    else if ( nClrBits <= 24 ) nClrBits = 24;
    else nClrBits = 32;

    /* alloc memory knowing that all bitmaps except 24bpp need extra RGBQUAD
     * table */
    sizeOfBmi = sizeof(BITMAPINFOHEADER);
    if ( nClrBits != 24 )
        sizeOfBmi += sizeof(RGBQUAD) * ( 1 << nClrBits);

    pbmi = (BITMAPINFO *)calloc(sizeOfBmi, 1);
    if ( !pbmi ) {
        OutOfMemory(sizeOfBmi);

        goto cleanup;
    }

    /* initialize the fields in the BITMAPINFO structure */
    pbih = (BITMAPINFOHEADER *)pbmi;
    pbih->biSize = sizeof(BITMAPINFOHEADER);
    pbih->biWidth = bmp.bmWidth;
    pbih->biHeight = bmp.bmHeight;
    pbih->biPlanes = bmp.bmPlanes;
    pbih->biBitCount = bmp.bmBitsPixel;
    if ( nClrBits < 24 )
        pbih->biClrUsed = 1 << nClrBits;
    pbih->biCompression = BI_RGB;
    pbih->biSizeImage = 0;
    pbih->biClrImportant = 0;

    /* let GetDIBits fill the size field, calc it ourselves if it didn't */
    hdc = CreateCompatibleDC(NULL);
    if ( !hdc ) {
        wxLogLastError(_T("CreateCompatibleDC(NULL)"));

        goto cleanup;
    }

    (void)GetDIBits(hdc, hBmp, 0, pbih->biHeight, NULL, pbmi, DIB_RGB_COLORS);
    if ( !pbih->biSizeImage )
        pbih->biSizeImage = (pbih->biWidth + 15) / 16 * pbih->biHeight * nClrBits;

    /*
       get the data from the bitmap
     */
    bmpData = malloc(pbih->biSizeImage);
    if ( !bmpData ) {
        OutOfMemory(pbih->biSizeImage);

        goto cleanup;
    }

    if ( !GetDIBits(hdc, hBmp,
                    0, pbih->biHeight,
                    bmpData, pbmi, DIB_RGB_COLORS) ) {
        wxLogLastError(_T("GetDIBits"));

        goto cleanup;
    }

    /*
       write the data to the file
     */

    /* write the file header */
    sizeColTable = pbih->biClrUsed * sizeof(RGBQUAD);
    hdr.bfType = 0x4d42; /* "BM" string */
    hdr.bfSize = sizeof(BITMAPFILEHEADER) +
                 pbih->biSize + sizeColTable + pbih->biSizeImage;
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;
    hdr.bfOffBits = hdr.bfSize - pbih->biSizeImage;

    if ( !stream.Write(&hdr, sizeof(hdr)) ) {
        if ( verbose )
            wxLogError(_("Couldn't write the BMP file header."));

        goto cleanup;
    }

    /* write the bitmap header to the file */
    if ( !stream.Write(pbih, sizeof(BITMAPINFOHEADER) + sizeColTable) ) {
        if ( verbose )
            wxLogError(_("Couldn't write the bitmap header."));

        goto cleanup;
    }

    /* write the data to the file */
    if ( !stream.Write(bmpData, pbih->biSizeImage) ) {
        if ( verbose )
            wxLogError(_("Couldn't write bitmap data."));

        goto cleanup;
    }

    /* success */
    rc = 1;

cleanup:
    free(bmpData);
    free(pbmi);
    if ( hdc )
        DeleteDC(hdc);

    if ( !rc ) {
        wxLogError(_("Failed to save the bitmap."));

        return FALSE;
    }

    return TRUE;
}

#endif // __WIN32__

#if wxUSE_STREAMS

#ifndef BI_RGB
#define BI_RGB       0
#define BI_RLE8      1
#define BI_RLE4      2
#endif

#ifndef BI_BITFIELDS
#define BI_BITFIELDS 3
#endif

#define poffset (line * width * 3 + column * 3)

bool wxBMPHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose, int WXUNUSED(index) )
{
    int             rshift = 0, gshift = 0, bshift = 0;
    wxUint8         aByte;
    wxUint16        aWord;
    wxInt32         dbuf[4], aDword,
                    rmask = 0, gmask = 0, bmask = 0;
    wxInt8          bbuf[4];
    struct _cmap {
        unsigned char r, g, b;
    } *cmap = NULL;

    off_t start_offset = stream.TellI();

    image->Destroy();

    /*
     * Read the BMP header
     */

    stream.Read( &bbuf, 2 );
    stream.Read( dbuf, 4 * 4 );

#if 0 // unused
    wxInt32 size = wxINT32_SWAP_ON_BE( dbuf[0] );
#endif
    wxInt32 offset = wxINT32_SWAP_ON_BE( dbuf[2] );

    stream.Read(dbuf, 4 * 2);
    int width = (int)wxINT32_SWAP_ON_BE( dbuf[0] );
    int height = (int)wxINT32_SWAP_ON_BE( dbuf[1] );
    if (width > 32767)
    {
        if (verbose)
            wxLogError( _("BMP: Image width > 32767 pixels for file.") );
        return FALSE;
    }
    if (height > 32767)
    {
        if (verbose)
            wxLogError( _("BMP: Image height > 32767 pixels for file.") );
        return FALSE;
    }

    stream.Read( &aWord, 2 );
/*
    TODO
    int planes = (int)wxUINT16_SWAP_ON_BE( aWord );
*/
    stream.Read( &aWord, 2 );
    int bpp = (int)wxUINT16_SWAP_ON_BE( aWord );
    if (bpp != 1 && bpp != 4 && bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32)
    {
        if (verbose)
            wxLogError( _("BMP: Unknown bitdepth in file.") );
        return FALSE;
    }

    stream.Read( dbuf, 4 * 4 );
    int comp = (int)wxINT32_SWAP_ON_BE( dbuf[0] );
    if (comp != BI_RGB && comp != BI_RLE4 && comp != BI_RLE8 && comp != BI_BITFIELDS)
    {
        if (verbose)
            wxLogError( _("BMP: Unknown encoding in file.") );
        return FALSE;
    }

    stream.Read( dbuf, 4 * 2 );
    int ncolors = (int)wxINT32_SWAP_ON_BE( dbuf[0] );
    if (ncolors == 0)
        ncolors = 1 << bpp;
    /* some more sanity checks */
    if (((comp == BI_RLE4) && (bpp != 4)) ||
        ((comp == BI_RLE8) && (bpp != 8)) ||
        ((comp == BI_BITFIELDS) && (bpp != 16 && bpp != 32)))
    {
        if (verbose)
            wxLogError( _("BMP: Encoding doesn't match bitdepth.") );
        return FALSE;
    }
    if (bpp < 16)
    {
        cmap = (struct _cmap *)malloc(sizeof(struct _cmap) * ncolors);
        if (!cmap)
        {
            if (verbose)
                wxLogError( _("BMP: Couldn't allocate memory.") );
            return FALSE;
        }
    }
    else
        cmap = NULL;

    image->Create( width, height );
    unsigned char *ptr = image->GetData();
    if (!ptr)
    {
        if (verbose)
            wxLogError( _("BMP: Couldn't allocate memory.") );
        if (cmap)
            free(cmap);
        return FALSE;
    }

    /*
     * Reading the palette, if it exists.
     */
    if (bpp < 16 && ncolors != 0)
    {
        for (int j = 0; j < ncolors; j++)
        {
            stream.Read( bbuf, 4 );
            cmap[j].b = bbuf[0];
            cmap[j].g = bbuf[1];
            cmap[j].r = bbuf[2];
        }
    }
    else if (bpp == 16 || bpp == 32)
    {
        if (comp == BI_BITFIELDS)
        {
            int bit = 0;
            stream.Read( dbuf, 4 * 3 );
            bmask = wxINT32_SWAP_ON_BE( dbuf[0] );
            gmask = wxINT32_SWAP_ON_BE( dbuf[1] );
            rmask = wxINT32_SWAP_ON_BE( dbuf[2] );
            /* find shift amount.. ugly, but i can't think of a better way */
            for (bit = 0; bit < bpp; bit++)
            {
                if (bmask & (1 << bit))
                    bshift = bit;
                if (gmask & (1 << bit))
                    gshift = bit;
                if (rmask & (1 << bit))
                    rshift = bit;
            }
        }
        else if (bpp == 16)
        {
            rmask = 0x7C00;
            gmask = 0x03E0;
            bmask = 0x001F;
            rshift = 10;
            gshift = 5;
            bshift = 0;
        }
        else if (bpp == 32)
        {
            rmask = 0x00FF0000;
            gmask = 0x0000FF00;
            bmask = 0x000000FF;
            rshift = 16;
            gshift = 8;
            bshift = 0;
        }
    }

    /*
     * Reading the image data
     */
    stream.SeekI( start_offset + offset );
    unsigned char *data = ptr;

    /* set the whole image to the background color */
    if (bpp < 16 && (comp == BI_RLE4 || comp == BI_RLE8))
    {
        for (int i = 0; i < width * height; i++)
        {
            *ptr++ = cmap[0].r;
            *ptr++ = cmap[0].g;
            *ptr++ = cmap[0].b;
        }
        ptr = data;
    }

    int line = 0;
    int column = 0;
    int linesize = ((width * bpp + 31) / 32) * 4;

    /* BMPs are stored upside down */
    for (line = (height - 1); line >= 0; line--)
    {
        int linepos = 0;
        for (column = 0; column < width;)
        {
            if (bpp < 16)
            {
                int index = 0;
                linepos++;
                aByte = stream.GetC();
                if (bpp == 1)
                {
                    int bit = 0;
                    for (bit = 0; bit < 8; bit++)
                    {
                        index = ((aByte & (0x80 >> bit)) ? 1 : 0);
                        ptr[poffset] = cmap[index].r;
                        ptr[poffset + 1] = cmap[index].g;
                        ptr[poffset + 2] = cmap[index].b;
                        column++;
                    }
                }
                else if (bpp == 4)
                {
                    if (comp == BI_RLE4)
                    {
                        if (verbose)
                            wxLogError( _("BMP: Cannot deal with 4bit encoded yet.") );
                        image->Destroy();
                        free(cmap);
                        return FALSE;
                    }
                    else
                    {
                        int nibble = 0;
                        for (nibble = 0; nibble < 2; nibble++)
                        {
                            index = ((aByte & (0xF0 >> nibble * 4)) >> (!nibble * 4));
                            if (index >= 16)
                                index = 15;
                            ptr[poffset] = cmap[index].r;
                            ptr[poffset + 1] = cmap[index].g;
                            ptr[poffset + 2] = cmap[index].b;
                            column++;
                        }
                    }
                }
                else if (bpp == 8)
                {
                    if (comp == BI_RLE8)
                    {
                        unsigned char first;
                        first = aByte;
                        aByte = stream.GetC();
                        if (first == 0)
                        {
                            if (aByte == 0)
                            {
                                /* column = width; */
                            }
                            else if (aByte == 1)
                            {
                                column = width;
                                line = -1;
                            }
                            else if (aByte == 2)
                            {
                                aByte = stream.GetC();
                                column += aByte;
                                linepos = column * bpp / 8;
                                aByte = stream.GetC();
                                line += aByte;
                            }
                            else
                            {
                                int absolute = aByte;
                                for (int k = 0; k < absolute; k++)
                                {
                                    linepos++;
                                    aByte = stream.GetC();
                                    ptr[poffset    ] = cmap[aByte].r;
                                    ptr[poffset + 1] = cmap[aByte].g;
                                    ptr[poffset + 2] = cmap[aByte].b;
                                    column++;
                                }
                                if (absolute & 0x01)
                                    aByte = stream.GetC();
                            }
                        }
                        else
                        {
                            for (int l = 0; l < first; l++)
                            {
                                ptr[poffset    ] = cmap[aByte].r;
                                ptr[poffset + 1] = cmap[aByte].g;
                                ptr[poffset + 2] = cmap[aByte].b;
                                column++;
                                linepos++;
                            }
                        }
                    }
                    else
                    {
                        ptr[poffset    ] = cmap[aByte].r;
                        ptr[poffset + 1] = cmap[aByte].g;
                        ptr[poffset + 2] = cmap[aByte].b;
                        column++;
                        // linepos += size;    seems to be wrong, RR
                    }
                }
               }
               else if (bpp == 24)
               {
                   stream.Read( &bbuf, 3 );
                   linepos += 3;
                   ptr[poffset    ] = (unsigned char)bbuf[2];
                   ptr[poffset + 1] = (unsigned char)bbuf[1];
                   ptr[poffset + 2] = (unsigned char)bbuf[0];
                   column++;
               }
               else if (bpp == 16)
               {
                   unsigned char temp;
                   stream.Read( &aWord, 2 );
                   aWord = wxUINT16_SWAP_ON_BE( aWord );
                   linepos += 2;
                   temp = (aWord & rmask) >> rshift;
                   ptr[poffset] = temp;
                   temp = (aWord & gmask) >> gshift;
                   ptr[poffset + 1] = temp;
                   temp = (aWord & bmask) >> bshift;
                   ptr[poffset + 2] = temp;
                   column++;
               }
               else
               {
                   unsigned char temp;
                   stream.Read( &aDword, 4 );
                   aDword = wxINT32_SWAP_ON_BE( aDword );
                   linepos += 4;
                   temp = (aDword & rmask) >> rshift;
                   ptr[poffset] = temp;
                   temp = (aDword & gmask) >> gshift;
                   ptr[poffset + 1] = temp;
                   temp = (aDword & bmask) >> bshift;
                   ptr[poffset + 2] = temp;
                   column++;
               }
          }
          while ((linepos < linesize) && (comp != 1) && (comp != 2))
          {
              stream.Read( &aByte, 1 );
              linepos += 1;
              if (stream.LastError() != wxStream_NOERROR)
                  break;
          }
     }
     if (cmap)
       free(cmap);

     image->SetMask( FALSE );

     return TRUE;
}

bool wxBMPHandler::DoCanRead( wxInputStream& stream )
{
    unsigned char hdr[2];

    stream.Read(&hdr, 2);
    stream.SeekI(-2, wxFromCurrent);
    return (hdr[0] == 'B' && hdr[1] == 'M');
}

#endif // wxUSE_STREAMS


