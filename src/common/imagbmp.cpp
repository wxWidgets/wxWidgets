/////////////////////////////////////////////////////////////////////////////
// Name:        imagbmp.cpp
// Purpose:     wxImage BMP handler
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
   We don't put pragma implement in this file because it is already present in
   src/common/image.cpp
*/

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/image.h"
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

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBMPHandler,wxImageHandler)
#endif

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

bool wxBMPHandler::LoadFile( wxImage *image, wxInputStream& stream, bool WXUNUSED(verbose), int WXUNUSED(index) )
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
        wxLogError( wxT("Image width > 32767 pixels for file.") );
        return FALSE;
    }
    if (height > 32767)
    {
        wxLogError( wxT("Image height > 32767 pixels for file.") );
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
        wxLogError( wxT("unknown bitdepth in file.") );
        return FALSE;
    }

    stream.Read( dbuf, 4 * 4 );
    int comp = (int)wxINT32_SWAP_ON_BE( dbuf[0] );
    if (comp != BI_RGB && comp != BI_RLE4 && comp != BI_RLE8 && comp != BI_BITFIELDS)
    {
        wxLogError( wxT("unknown encoding in Windows BMP file.") );
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
        wxLogError( wxT("encoding of BMP doesn't match bitdepth.") );
        return FALSE;
    }
    if (bpp < 16)
    {
        cmap = (struct _cmap *)malloc(sizeof(struct _cmap) * ncolors);
        if (!cmap)
        {
            wxLogError( wxT("Cannot allocate RAM for color map in BMP file.") );
            return FALSE;
        }
    }
    else
        cmap = NULL;

    image->Create( width, height );
    unsigned char *ptr = image->GetData();
    if (!ptr)
    {
        wxLogError( wxT("Cannot allocate RAM for RGB data in file.") );
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
                        wxLogError( wxT("Can't deal with 4bit encoded yet.") );
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


