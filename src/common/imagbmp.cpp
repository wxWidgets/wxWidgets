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


#if wxUSE_STREAMS


bool wxBMPHandler::SaveFile(wxImage *image,
                            wxOutputStream& stream,
                            bool verbose)
{
    wxCHECK_MSG( image, FALSE, _T("invalid pointer in wxBMPHandler::SaveFile") );

    if (!image->Ok())
    {
        if (verbose) wxLogError(_("BMP: Couldn't save invalid image."));
        return FALSE;
    }

    unsigned width = image->GetWidth();
    unsigned row_width = width * 3 + 
                         (((width % 4) == 0) ? 0 : (4 - (width * 3) % 4));
                         // each row must be aligned to dwords  
    struct
    {
        // BitmapHeader:
        wxUint16  magic;          // format magic, always 'BM'
        wxUint32  filesize;       // total file size, inc. headers
        wxUint32  reserved;       // for future use
        wxUint32  data_offset;    // image data offset in the file
        
        // BitmapInfoHeader:
        wxUint32  bih_size;       // 2nd part's size
        wxUint32  width, height;  // bitmap's dimensions
        wxUint16  planes;         // num of planes
        wxUint16  bpp;            // bits per pixel
        wxUint32  compression;    // compression method
        wxUint32  size_of_bmp;    // size of the bitmap
        wxUint32  h_res, v_res;   // image resolution in dpi
        wxUint32  num_clrs;       // number of colors used
        wxUint32  num_signif_clrs;// number of significant colors
    } hdr;
    wxUint32 hdr_size = 14/*BitmapHeader*/ + 40/*BitmapInfoHeader*/;

    hdr.magic = wxUINT16_SWAP_ON_BE(0x4D42/*'BM'*/);
    hdr.filesize = wxUINT32_SWAP_ON_BE(
                   hdr_size + 
                   row_width * image->GetHeight()
                   );
    hdr.reserved = 0;
    hdr.data_offset = wxUINT32_SWAP_ON_BE(hdr_size);
    
    hdr.bih_size = wxUINT32_SWAP_ON_BE(hdr_size - 14);
    hdr.width = wxUINT32_SWAP_ON_BE(image->GetWidth());
    hdr.height = wxUINT32_SWAP_ON_BE(image->GetHeight());
    hdr.planes = wxUINT16_SWAP_ON_BE(1); // always 1 plane
    hdr.bpp = wxUINT16_SWAP_ON_BE(24); // always TrueColor
    hdr.compression = 0; // RGB uncompressed
    hdr.size_of_bmp = wxUINT32_SWAP_ON_BE(row_width * image->GetHeight()); 
    hdr.h_res = hdr.v_res = wxUINT32_SWAP_ON_BE(72); // 72dpi is standard
    hdr.num_clrs = 0; // maximal possible = 2^24
    hdr.num_signif_clrs = 0; // all colors are significant

    if (// VS: looks ugly but compilers tend to do ugly things with structs,
        //     like aligning hdr.filesize's ofset to dword :(
        !stream.Write(&hdr.magic, 2) ||
        !stream.Write(&hdr.filesize, 4) ||
        !stream.Write(&hdr.reserved, 4) ||
        !stream.Write(&hdr.data_offset, 4) ||
        !stream.Write(&hdr.bih_size, 4) ||
        !stream.Write(&hdr.width, 4) ||
        !stream.Write(&hdr.height, 4) ||
        !stream.Write(&hdr.planes, 2) ||
        !stream.Write(&hdr.bpp, 2) ||
        !stream.Write(&hdr.compression, 4) ||
        !stream.Write(&hdr.size_of_bmp, 4) ||
        !stream.Write(&hdr.h_res, 4) ||
        !stream.Write(&hdr.v_res, 4) ||
        !stream.Write(&hdr.num_clrs, 4) ||
        !stream.Write(&hdr.num_signif_clrs, 4)
       ) 
    {
        if (verbose)
            wxLogError(_("BMP: Couldn't write the file header."));
        return FALSE;
    }

    wxUint8 *data = (wxUint8*) image->GetData();
    wxUint8 *buffer = new wxUint8[row_width];
    wxUint8 tmpvar;
    memset(buffer, 0, row_width);
    int y; unsigned x;

    for (y = image->GetHeight() -1 ; y >= 0; y--)
    {
        memcpy(buffer, data + y * 3 * width, 3 * width);
        for (x = 0; x < width; x++)
        {
            tmpvar = buffer[3 * x + 0];
            buffer[3 * x + 0] = buffer[3 * x + 2];
            buffer[3 * x + 2] = tmpvar;
        }
        
        if (!stream.Write(buffer, row_width))
        {
            if (verbose)
                wxLogError(_("BMP: Couldn't write data."));
            delete[] buffer;
            return FALSE;
        }
    }
    delete[] buffer;

    return TRUE;
}




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
    if (start_offset == wxInvalidOffset) start_offset = 0;

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


