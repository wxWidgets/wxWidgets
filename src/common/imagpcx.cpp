/////////////////////////////////////////////////////////////////////////////
// Name:        imagpcx.cpp
// Purpose:     wxImage PCX handler
// Author:      Guillermo Rodriguez Garcia <guille@iies.es>
// Version:     1.00
// CVS-ID:      $Id$
// Copyright:   (c) 1999 Guillermo Rodriguez Garcia
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

#ifndef WX_PRECOMP
#  include "wx/defs.h"
#endif

#if wxUSE_STREAMS && wxUSE_PCX

#include "wx/image.h"
#include "wx/wfstream.h"
#include "wx/module.h"
#include "wx/log.h"


//-----------------------------------------------------------------------------
// PCX decoding
//-----------------------------------------------------------------------------

void RLEencode(unsigned char *WXUNUSED(p), unsigned int WXUNUSED(size), wxOutputStream& WXUNUSED(s))
{
}

void RLEdecode(unsigned char *p, unsigned int size, wxInputStream& s)
{
    unsigned int i, data, cont;

    // Read 'size' bytes. The PCX official specs say there will be
    // a decoding break at the end of each scanline (but not at the
    // end of each plane inside a scanline). Only use this function
    // to read one or more _complete_ scanlines. Else, more than
    // 'size' bytes might be read and the buffer might overflow.
    //
    while (size > 0)
    {
        data = (unsigned char)s.GetC();

        // If ((data & 0xC0) != 0xC0), then the value read is a data
        // byte. Else, it is a counter (cont = val & 0x3F) and the
        // next byte is the data byte.
        //
        if ((data & 0xC0) != 0xC0)
        {
            *(p++) = data;
            size--;
        }
        else
        {
            cont = data & 0x3F;
            data = (unsigned char)s.GetC();
            for (i = 1; i <= cont; i++)
                *(p++) = data;
            size -= cont;
        }
    }
}


/* PCX header */
#define HDR_VERSION         1
#define HDR_ENCODING        2
#define HDR_BITSPERPIXEL    3
#define HDR_XMIN            4
#define HDR_YMIN            6
#define HDR_XMAX            8
#define HDR_YMAX            10
#define HDR_NPLANES         65
#define HDR_BYTESPERLINE    66

/* image formats */
#define IMAGE_8BIT  0       // 8 bpp, 1 plane (8 bit)
#define IMAGE_24BIT 1       // 8 bpp, 3 planes (24 bit)

/* error codes */
#define E_OK        0       // everything was OK
#define E_FORMATO   1       // error in pcx file format
#define E_MEMORIA   2       // error allocating memory
#define E_VERSION   3       // error in pcx version number


// ReadPCX:
//  Loads a PCX file into the wxImage object pointed by image.
//  Returns E_OK on success, or an error code otherwise (see
//  above for error codes)
//
int ReadPCX(wxImage *image, wxInputStream& stream)
{
    unsigned char hdr[128];         // PCX header
    unsigned char pal[768];         // palette for 8 bit images
    unsigned char *p;               // space to store one scanline
    unsigned char *dst;             // pointer into wxImage data
    unsigned int width, height;     // size of the image
    unsigned int bytesperline;      // bytes per line (each plane)
    int bitsperpixel;               // bits per pixel (each plane)
    int nplanes;                    // number of planes
    int encoding;                   // is the image RLE encoded?
    int format;                     // image format (8 bit, 24 bit)
    unsigned int i;
    off_t pos;

    // Read PCX header and check the version number (it must
    // be at least 5 or higher for 8 bit and 24 bit images).
    //
    stream.Read(hdr, 128);

    if (hdr[HDR_VERSION] < 5) return E_VERSION;

    // Extract all image info from the PCX header.
    //
    encoding     = hdr[HDR_ENCODING];
    nplanes      = hdr[HDR_NPLANES];
    bitsperpixel = hdr[HDR_BITSPERPIXEL];
    bytesperline = hdr[HDR_BYTESPERLINE] + 256 * hdr[HDR_BYTESPERLINE + 1];
    width        = (hdr[HDR_XMAX] + 256 * hdr[HDR_XMAX + 1]) -
                   (hdr[HDR_XMIN] + 256 * hdr[HDR_XMIN + 1]) + 1;
    height       = (hdr[HDR_YMAX] + 256 * hdr[HDR_YMAX + 1]) -
                   (hdr[HDR_YMIN] + 256 * hdr[HDR_YMIN + 1]) + 1;

    // Check image format. Currently supported formats are
    // 8 bits (8 bpp, 1 plane) and 24 bits (8 bpp, 3 planes).
    //
    if ((nplanes == 3) && (bitsperpixel == 8))
        format = IMAGE_24BIT;
    else if ((nplanes == 1) && (bitsperpixel == 8))
        format = IMAGE_8BIT;
    else
        return E_FORMATO;

    // If the image is of type IMAGE_8BIT, then there is a
    // palette at the end of the file. Read it now before
    // proceeding.
    //
    if (format == IMAGE_8BIT)
    {
        pos = stream.TellI();
        stream.SeekI(-769, wxFromEnd);

        if (stream.GetC() != 12)
            return E_FORMATO;

        stream.Read(pal, 768);
        stream.SeekI(pos, wxFromStart);
    }

    // Allocate memory for a scanline and resize the image.
    //
    image->Create(width, height);

    if (!image->Ok())
        return E_MEMORIA;

    if ((p = (unsigned char *) malloc(bytesperline * nplanes)) == NULL)
        return E_MEMORIA;

    // Now start reading the file, line by line, and store
    // the data in the format required by wxImage.
    //
    dst = image->GetData();

    for (; height; height--)
    {
        if (encoding)
            RLEdecode(p, bytesperline * nplanes, stream);
        else
            stream.Read(p, bytesperline * nplanes);

        switch (format)
        {
            case IMAGE_8BIT:
            {
                for (i = 0; i < width; i++)
                {
                    *(dst++) = pal[ 3 * (p[i]) ];
                    *(dst++) = pal[ 3 * (p[i]) + 1];
                    *(dst++) = pal[ 3 * (p[i]) + 2];
                }
                break;
            }
            case IMAGE_24BIT:
            {
                for (i = 0; i < width; i++)
                {
                    *(dst++) = p[i];
                    *(dst++) = p[i + bytesperline];
                    *(dst++) = p[i + 2 * bytesperline];
                }
                break;
            }
        }
    }

    free(p);

    return E_OK;
}


//-----------------------------------------------------------------------------
// wxPCXHandler
//-----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxPCXHandler,wxImageHandler)
#endif

bool wxPCXHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose )
{
    int error;

    if (!CanRead(stream))
    {
        if (verbose)
            wxLogError(wxT("wxPCXHandler: this is not a PCX file"));

        return FALSE;
    }

    image->Destroy();

    if ((error = ReadPCX(image, stream)) != E_OK)
    {
        if (verbose)
        {
            switch (error)
            {
                case E_FORMATO: wxLogError(wxT("wxPCXHandler: image format unsupported")); break;
                case E_MEMORIA: wxLogError(wxT("wxPCXHandler: couldn't allocate memory")); break;
                case E_VERSION: wxLogError(wxT("wxPCXHandler: version number too low")); break;
                default:        wxLogError(wxT("wxPCXHandler: unknown error !!!"));
            }
        }
        image->Destroy();
        return FALSE;
    }

    return TRUE;
}

bool wxPCXHandler::SaveFile( wxImage *WXUNUSED(image), wxOutputStream& WXUNUSED(stream), bool verbose )
{
    if (verbose)
        wxLogError(wxT("wxPCXHandler::SaveFile still not implemented"));

    return FALSE;
}

bool wxPCXHandler::DoCanRead( wxInputStream& stream )
{
    unsigned char c;
    off_t pos;

    pos = stream.TellI();
    stream.SeekI(0, wxFromStart);
    c = stream.GetC();
    stream.SeekI(pos, wxFromStart);

    // not very safe, but this is all we can get from PCX header :-(
    return (c == 10);
}

#endif // wxUSE_STREAMS && wxUSE_PCX

