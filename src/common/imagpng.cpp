/////////////////////////////////////////////////////////////////////////////
// Name:        imagepng.cpp
// Purpose:     wxImage PNG handler
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "imagpng.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
#endif

#if wxUSE_LIBPNG

#include "wx/imagpng.h"
#include "wx/bitmap.h"
#include "wx/debug.h"
#include "wx/log.h"
#include "wx/app.h"
#include "png.h"
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
// wxPNGHandler
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPNGHandler,wxImageHandler)

#if wxUSE_LIBPNG

#if defined(__VISAGECPP__)
#define LINKAGEMODE _Optlink
#else
#define LINKAGEMODE
#endif

static void LINKAGEMODE _PNG_stream_reader( png_structp png_ptr, png_bytep data, png_size_t length )
{
    ((wxInputStream*) png_get_io_ptr( png_ptr )) -> Read(data, length);
}

static void LINKAGEMODE _PNG_stream_writer( png_structp png_ptr, png_bytep data, png_size_t length )
{
    ((wxOutputStream*) png_get_io_ptr( png_ptr )) -> Write(data, length);
}

// from pngerror.c
// so that the libpng doesn't send anything on stderr
void
LINKAGEMODE png_silent_error(png_structp png_ptr, png_const_charp WXUNUSED(message))
{
#ifdef USE_FAR_KEYWORD
   {
      jmp_buf jmpbuf;
      png_memcpy(jmpbuf,png_ptr->jmpbuf,sizeof(jmp_buf));
      longjmp(jmpbuf, 1);
   }
#else
   longjmp(png_ptr->jmpbuf, 1);
#endif
}

void
LINKAGEMODE png_silent_warning(png_structp WXUNUSED(png_ptr), png_const_charp WXUNUSED(message))
{
}

bool wxPNGHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose, int WXUNUSED(index) )
{
    // VZ: as this function uses setjmp() the only fool proof error handling
    //     method is to use goto (setjmp is not really C++ dtors friendly...)

    unsigned char **lines;
    unsigned int i;
    png_infop info_ptr = (png_infop) NULL;

    image->Destroy();

    png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
        (voidp) NULL,
        (png_error_ptr) NULL,
        (png_error_ptr) NULL );
    if (!png_ptr)
        goto error_nolines;

    // the file example.c explain how to guess if the stream is a png image
    if (!verbose) png_set_error_fn(png_ptr, (png_voidp)NULL, png_silent_error, png_silent_warning);

    info_ptr = png_create_info_struct( png_ptr );
    if (!info_ptr)
        goto error_nolines;

    if (setjmp(png_ptr->jmpbuf))
        goto error_nolines;

    if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
        goto error_nolines;

    png_set_read_fn( png_ptr, &stream, _PNG_stream_reader);

    png_uint_32 width,height;
    int bit_depth,color_type,interlace_type;

    png_read_info( png_ptr, info_ptr );
    png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, (int*) NULL, (int*) NULL );

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand( png_ptr );

    png_set_strip_16( png_ptr );
    png_set_packing( png_ptr );
    if (png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand( png_ptr );
    png_set_filler( png_ptr, 0xff, PNG_FILLER_AFTER );

    image->Create( (int)width, (int)height );

    if (!image->Ok())
        goto error_nolines;

    lines = (unsigned char **)malloc( (size_t)(height * sizeof(unsigned char *)) );
    if (lines == NULL)
        goto error_nolines;

    for (i = 0; i < height; i++)
    {
        if ((lines[i] = (unsigned char *)malloc( (size_t)(width * (sizeof(unsigned char) * 4)))) == NULL)
        {
            for ( unsigned int n = 0; n < i; n++ )
                free( lines[n] );
            goto error;
        }
    }

    // loaded successfully!
    {
        int transp = 0;
        png_read_image( png_ptr, lines );
        png_read_end( png_ptr, info_ptr );
        png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp) NULL );
        unsigned char *ptr = image->GetData();
        if ((color_type == PNG_COLOR_TYPE_GRAY) ||
            (color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
        {
            for (unsigned int y = 0; y < height; y++)
            {
                unsigned char *ptr2 = lines[y];
                for (unsigned int x = 0; x < width; x++)
                {
                    unsigned char r = *ptr2++;
                    unsigned char a = *ptr2++;
                    if (a < 128)
                    {
                        *ptr++ = 255;
                        *ptr++ = 0;
                        *ptr++ = 255;
                        transp = 1;
                    }
                    else
                    {
                        *ptr++ = r;
                        *ptr++ = r;
                        *ptr++ = r;
                    }
                }
            }
        }
        else
        {
            for (unsigned int y = 0; y < height; y++)
            {
                unsigned char *ptr2 = lines[y];
                for (unsigned int x = 0; x < width; x++)
                {
                    unsigned char r = *ptr2++;
                    unsigned char g = *ptr2++;
                    unsigned char b = *ptr2++;
                    unsigned char a = *ptr2++;
                    if (a < 128)
                    {
                        *ptr++ = 255;
                        *ptr++ = 0;
                        *ptr++ = 255;
                        transp = 1;
                    }
                    else
                    {
                        if ((r == 255) && (g == 0) && (b == 255)) r = 254;
                        *ptr++ = r;
                        *ptr++ = g;
                        *ptr++ = b;
                    }
                }
            }
        }

        for ( unsigned int j = 0; j < height; j++ )
            free( lines[j] );
        free( lines );

        if (transp)
        {
            image->SetMaskColour( 255, 0, 255 );
        }
        else
        {
            image->SetMask( FALSE );
        }
    }

    return TRUE;

 error_nolines:
    lines = NULL; // called from before it was set
 error:
    if (verbose)
       wxLogError(_("Couldn't load a PNG image - file is corrupted or not enough memory."));

    if ( image->Ok() )
    {
        image->Destroy();
    }

    if ( lines )
    {
        free( lines );
    }

    if ( png_ptr )
    {
        if ( info_ptr )
        {
            png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp) NULL );
            free(info_ptr);
        }
        else
            png_destroy_read_struct( &png_ptr, (png_infopp) NULL, (png_infopp) NULL );
    }
    return FALSE;
}


bool wxPNGHandler::SaveFile( wxImage *image, wxOutputStream& stream, bool verbose )
{
    {
        png_structp png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr)
        {
            return FALSE;
        }

        if (!verbose) png_set_error_fn(png_ptr, (png_voidp)NULL, png_silent_error, png_silent_warning);

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == NULL)
        {
            png_destroy_write_struct( &png_ptr, (png_infopp)NULL );
            return FALSE;
        }

        if (setjmp(png_ptr->jmpbuf))
        {
            png_destroy_write_struct( &png_ptr, (png_infopp)NULL );
            return FALSE;
        }

        png_set_write_fn( png_ptr, &stream, _PNG_stream_writer, NULL);

        png_set_IHDR( png_ptr, info_ptr, image->GetWidth(), image->GetHeight(), 8,
            PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_color_8 sig_bit;
        sig_bit.red = 8;
        sig_bit.green = 8;
        sig_bit.blue = 8;
        sig_bit.alpha = 8;
        png_set_sBIT( png_ptr, info_ptr, &sig_bit );
        png_write_info( png_ptr, info_ptr );
        png_set_shift( png_ptr, &sig_bit );
        png_set_packing( png_ptr );

        unsigned char *data = (unsigned char *)malloc( image->GetWidth()*4 );
        if (!data)
        {
            png_destroy_write_struct( &png_ptr, (png_infopp)NULL );
            return FALSE;
        }

        for (int y = 0; y < image->GetHeight(); y++)
        {
            unsigned char *ptr = image->GetData() + (y * image->GetWidth() * 3);
            for (int x = 0; x < image->GetWidth(); x++)
            {
                data[(x << 2) + 0] = *ptr++;
                data[(x << 2) + 1] = *ptr++;
                data[(x << 2) + 2] = *ptr++;
                if (( !image->HasMask() ) || \
                    (data[(x << 2) + 0] != image->GetMaskRed()) || \
                    (data[(x << 2) + 1] != image->GetMaskGreen()) || \
                    (data[(x << 2) + 2] != image->GetMaskBlue()))
                {
                    data[(x << 2) + 3] = 255;
                }
                else
                {
                    data[(x << 2) + 3] = 0;
                }
            }
            png_bytep row_ptr = data;
            png_write_rows( png_ptr, &row_ptr, 1 );
        }

        free(data);
        png_write_end( png_ptr, info_ptr );
        png_destroy_write_struct( &png_ptr, (png_infopp)&info_ptr );
    }
    return TRUE;
}

bool wxPNGHandler::DoCanRead( wxInputStream& stream )
{
    unsigned char hdr[4];

    stream.Read(&hdr, 4);
    stream.SeekI(-4, wxFromCurrent);
    return (hdr[0] == 0x89 && hdr[1] == 'P' && hdr[2] == 'N' && hdr[3] == 'G');
}

#endif  // wxUSE_STREAMS

#endif  // wxUSE_LIBPNG
