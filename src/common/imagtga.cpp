/////////////////////////////////////////////////////////////////////////////
// Name:        imagtga.cpp
// Purpose:     wxImage TGA handler
// Author:      Seth Jackson
// CVS-ID:      $Id$
// Copyright:   (c) 2005 Seth Jackson
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#if wxUSE_IMAGE && wxUSE_TGA

#ifndef WX_PRECOMP
    #include "wx/palette.h"
#endif

#include "wx/imagtga.h"
#include "wx/log.h"
#include "wx/scopeguard.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// TGA error codes.
enum
{
    wxTGA_OK,
    wxTGA_INVFORMAT,
    wxTGA_MEMERR,
    wxTGA_IOERR
};

// TGA header bytes.
enum
{
    HDR_OFFSET = 0,
    HDR_COLORTYPE  = 1,
    HDR_IMAGETYPE = 2,
    HDR_PALETTESTART = 3,
    HDR_PALETTELENGTH = 5,
    HDR_PALETTEBITS = 7,
    HDR_XORIGIN = 8,
    HDR_YORIGIN = 10,
    HDR_WIDTH = 12,
    HDR_HEIGHT = 14,
    HDR_BPP = 16,
    HDR_ORIENTATION = 17,
    HDR_SIZE
};

// TGA color types.
enum
{
    wxTGA_UNMAPPED = 0,
    wxTGA_MAPPED = 1
};

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxTGAHandler, wxImageHandler)

#if wxUSE_STREAMS

// ----------------------------------------------------------------------------
// worker functions
// ----------------------------------------------------------------------------

static
void FlipTGA(unsigned char* imageData, int width, int height, short pixelSize)
{
    int lineLength = width * pixelSize;
    unsigned char *line1 = imageData;
    unsigned char *line2 = &imageData[lineLength * (height - 1)];

    unsigned char temp;
    for ( ; line1 < line2; line2 -= (lineLength * 2))
    {
        for (int index = 0; index < lineLength; line1++, line2++, index++)
        {
            temp = *line1;
            *line1 = *line2;
            *line2 = temp;
        }
    }
}

// return wxTGA_OK or wxTGA_IOERR
static
int DecodeRLE(unsigned char* imageData, unsigned long imageSize,
               short pixelSize, wxInputStream& stream)
{
    unsigned long index = 0;
    unsigned char current;
    unsigned int length;
    unsigned char buf[4];

    while (index < imageSize)
    {
        int ch = stream.GetC();
        if ( ch == wxEOF )
            return wxTGA_IOERR;

        current = ch;

        // RLE packet.
        if ( current & 0x80 )
        {
            // Get the run length of the packet.
            current &= 0x7f;

            current++;

            length = current;

            index += current * pixelSize;

            // Repeat the pixel length times.
            if ( !stream.Read(buf, pixelSize) )
                return wxTGA_IOERR;

            for (unsigned int i = 0; i < length; i++)
            {
                memcpy(imageData, buf, pixelSize);

                imageData += pixelSize;
            }
        }
        else // Raw packet.
        {
            // Get the run length of the packet.
            current++;

            length = current * pixelSize;

            index += length;

            // Write the next length pixels directly to the image data.
            if ( !stream.Read(imageData, length) )
                return wxTGA_IOERR;

            imageData += length;
        }
    }

    return wxTGA_OK;
}

static
int ReadTGA(wxImage* image, wxInputStream& stream)
{
    // Read in the TGA header
    unsigned char hdr[HDR_SIZE];
    stream.Read(hdr, HDR_SIZE);

    short offset = hdr[HDR_OFFSET] + HDR_SIZE;
    short colorType = hdr[HDR_COLORTYPE];
    short imageType = hdr[HDR_IMAGETYPE];
    int paletteLength = hdr[HDR_PALETTELENGTH] + 256 * hdr[HDR_PALETTELENGTH + 1];
    int width = (hdr[HDR_WIDTH] + 256 * hdr[HDR_WIDTH + 1]) -
                (hdr[HDR_XORIGIN] + 256 * hdr[HDR_XORIGIN + 1]);
    int height = (hdr[HDR_HEIGHT] + 256 * hdr[HDR_HEIGHT + 1]) -
                 (hdr[HDR_YORIGIN] + 256 * hdr[HDR_YORIGIN + 1]);
    short bpp = hdr[HDR_BPP];
    short orientation = hdr[HDR_ORIENTATION] & 0x20;

    image->Create(width, height);

    if (!image->Ok())
    {
        return wxTGA_MEMERR;
    }

    const short pixelSize = bpp / 8;

    const unsigned long imageSize = width * height * pixelSize;

    unsigned char *imageData = (unsigned char* )malloc(imageSize);

    if (!imageData)
    {
        return wxTGA_MEMERR;
    }

    wxON_BLOCK_EXIT1(free, imageData);

    unsigned char *dst = image->GetData();

    unsigned char* alpha = NULL;
    if (bpp == 16 || bpp == 32)
    {
        image->SetAlpha();

        alpha = image->GetAlpha();
    }

    // Seek from the offset we got from the TGA header.
    if (stream.SeekI(offset, wxFromStart) == wxInvalidOffset)
        return wxTGA_INVFORMAT;

    // Load a palette if we have one.
    if (colorType == wxTGA_MAPPED)
    {
        unsigned char buf[3];

        unsigned char* r = new unsigned char[paletteLength];
        unsigned char* g = new unsigned char[paletteLength];
        unsigned char* b = new unsigned char[paletteLength];

        for (int i = 0; i < paletteLength; i++)
        {
            stream.Read(buf, 3);

            r[i] = buf[2];
            g[i] = buf[1];
            b[i] = buf[0];
        }

#if wxUSE_PALETTE
        // Set the palette of the image.
        image->SetPalette(wxPalette(paletteLength, r, g, b));
#endif // wxUSE_PALETTE

        delete[] r;
        delete[] g;
        delete[] b;
    }

    // Handle the various TGA formats we support.

    switch (imageType)
    {
#if wxUSE_PALETTE
        // Raw indexed.

        case 1:
        {
            const wxPalette& palette = image->GetPalette();
            unsigned char r;
            unsigned char g;
            unsigned char b;

            // No compression read the data directly to imageData.

            stream.Read(imageData, imageSize);

            // If orientation == 0, then the image is stored upside down.
            // We need to store it right side up.

            if (orientation == 0)
            {
                FlipTGA(imageData, width, height, pixelSize);
            }

            // Handle the different pixel depths.

            switch (bpp)
            {
                // 8 bpp.

                case 8:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        palette.GetRGB(imageData[index], &r, &g, &b);

                        *(dst++) = r;
                        *(dst++) = g;
                        *(dst++) = b;
                    }
                }
                break;

                // 16 bpp.

                case 16:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        palette.GetRGB(imageData[index], &r, &g, &b);

                        *(dst++) = r;
                        *(dst++) = g;
                        *(dst++) = b;
                        *(alpha++) = (imageData[index + 1] & 0x80) ? 0 : 255;
                    }
                }
                break;

                default:
                    return wxTGA_INVFORMAT;
            }
        }
        break;
#endif // wxUSE_PALETTE

        // Raw RGB.

        case 2:
        {
            // No compression read the data directly to imageData.

            stream.Read(imageData, imageSize);

            // If orientation == 0, then the image is stored upside down.
            // We need to store it right side up.

            if (orientation == 0)
            {
                FlipTGA(imageData, width, height, pixelSize);
            }

            // Handle the different pixel depths.

            switch (bpp)
            {
                //16 bpp.

                case 16:
                {
                    unsigned char temp;

                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        temp = (imageData[index + 1] & 0x7c) << 1;
                        temp |= temp >> 5;
                        *(dst++) = temp;

                        temp = ((imageData[index + 1] & 0x03) << 6) | ((imageData[index] & 0xe0) >> 2);
                        temp |= temp >> 5;
                        *(dst++) = temp;

                        temp = (imageData[index] & 0x1f) << 3;
                        temp |= temp >> 5;
                        *(dst++) = temp;

                        *(alpha++) = (imageData[index + 1] & 0x80) ? 0 : 255;
                    }
                }
                break;

                // 24 bpp.

                case 24:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        *(dst++) = imageData[index + 2];
                        *(dst++) = imageData[index + 1];
                        *(dst++) = imageData[index];
                    }
                }
                break;

                // 32 bpp.

                case 32:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        *(dst++) = imageData[index + 2];
                        *(dst++) = imageData[index + 1];
                        *(dst++) = imageData[index];
                        *(alpha++) = imageData[index + 3];
                    }
                }
                break;

                default:
                    return wxTGA_INVFORMAT;
            }
        }
        break;

        // Raw grayscale.

        case 3:
        {
            // No compression read the data directly to imageData.

            stream.Read(imageData, imageSize);

            // If orientation == 0, then the image is stored upside down.
            // We need to store it right side up.

            if (orientation == 0)
            {
                FlipTGA(imageData, width, height, pixelSize);
            }

            // Handle the different pixel depths.

            switch (bpp)
            {
                // 8 bpp.

                case 8:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        *(dst++) = imageData[index];
                        *(dst++) = imageData[index];
                        *(dst++) = imageData[index];
                    }
                }
                break;

                // 16 bpp.

                case 16:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        *(dst++) = imageData[index];
                        *(dst++) = imageData[index];
                        *(dst++) = imageData[index];
                        *(alpha++) = imageData[index + 1];
                    }
                }
                break;

                default:
                    return wxTGA_INVFORMAT;
            }
        }
        break;

#if wxUSE_PALETTE
        // RLE indexed.

        case 9:
        {
            const wxPalette& palette = image->GetPalette();
            unsigned char r;
            unsigned char g;
            unsigned char b;

            // Decode the RLE data.

            int rc =  DecodeRLE(imageData, imageSize, pixelSize, stream);
            if ( rc != wxTGA_OK )
                return rc;

            // If orientation == 0, then the image is stored upside down.
            // We need to store it right side up.

            if (orientation == 0)
            {
                FlipTGA(imageData, width, height, pixelSize);
            }

            // Handle the different pixel depths.

            switch (bpp)
            {
                // 8 bpp.

                case 8:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        palette.GetRGB(imageData[index], &r, &g, &b);

                        *(dst++) = r;
                        *(dst++) = g;
                        *(dst++) = b;
                    }
                }
                break;

                // 16 bpp.

                case 16:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        palette.GetRGB(imageData[index], &r, &g, &b);

                        *(dst++) = r;
                        *(dst++) = g;
                        *(dst++) = b;
                        *(alpha++) = (imageData[index + 1] & 0x80) ? 0 : 255;
                    }
                }
                break;

                default:
                    return wxTGA_INVFORMAT;
            }
        }
        break;
#endif // wxUSE_PALETTE

        // RLE RGB.

        case 10:
        {
            // Decode the RLE data.

            int rc = DecodeRLE(imageData, imageSize, pixelSize, stream);
            if ( rc != wxTGA_OK )
                return rc;

            // If orientation == 0, then the image is stored upside down.
            // We need to store it right side up.

            if (orientation == 0)
            {
                FlipTGA(imageData, width, height, pixelSize);
            }

            // Handle the different pixel depths.

            switch (bpp)
            {
                //16 bpp.

                case 16:
                {
                    unsigned char temp;

                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        temp = (imageData[index + 1] & 0x7c) << 1;
                        temp |= temp >> 5;
                        *(dst++) = temp;

                        temp = ((imageData[index + 1] & 0x03) << 6) | ((imageData[index] & 0xe0) >> 2);
                        temp |= temp >> 5;
                        *(dst++) = temp;

                        temp = (imageData[index] & 0x1f) << 3;
                        temp |= temp >> 5;
                        *(dst++) = temp;

                        *(alpha++) = (imageData[index + 1] & 0x80) ? 0 : 255;
                    }
                }
                break;

                // 24 bpp.

                case 24:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        *(dst++) = imageData[index + 2];
                        *(dst++) = imageData[index + 1];
                        *(dst++) = imageData[index];
                    }
                }
                break;

                // 32 bpp.

                case 32:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        *(dst++) = imageData[index + 2];
                        *(dst++) = imageData[index + 1];
                        *(dst++) = imageData[index];
                        *(alpha++) = imageData[index + 3];
                    }
                }
                break;

                default:
                    return wxTGA_INVFORMAT;
            }
        }
        break;

        // RLE grayscale.

        case 11:
        {
            // Decode the RLE data.

            int rc = DecodeRLE(imageData, imageSize, pixelSize, stream);
            if ( rc != wxTGA_OK )
                return rc;

            // If orientation == 0, then the image is stored upside down.
            // We need to store it right side up.

            if (orientation == 0)
            {
                FlipTGA(imageData, width, height, pixelSize);
            }

            // Handle the different pixel depths.

            switch (bpp)
            {
                // 8 bpp.

                case 8:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        *(dst++) = imageData[index];
                        *(dst++) = imageData[index];
                        *(dst++) = imageData[index];
                    }
                }
                break;

                // 16 bpp.

                case 16:
                {
                    for (unsigned long index = 0; index < imageSize; index += pixelSize)
                    {
                        *(dst++) = imageData[index];
                        *(dst++) = imageData[index];
                        *(dst++) = imageData[index];
                        *(alpha++) = imageData[index + 1];
                    }
                }
                break;

                default:
                    return wxTGA_INVFORMAT;
            }
        }
        break;

        default:
            return wxTGA_INVFORMAT;
    }

    return wxTGA_OK;
}

static
int SaveTGA(const wxImage& image, wxOutputStream *stream)
{
    bool hasAlpha = image.HasAlpha();
    unsigned bytesPerPixel = 3 + (hasAlpha ? 1 : 0);
    wxSize size = image.GetSize();
    size_t scanlineSize = size.x * bytesPerPixel;
    unsigned char *scanlineData = (unsigned char *) malloc(scanlineSize);
    if (!scanlineData)
    {
        return wxTGA_MEMERR;
    }

    wxON_BLOCK_EXIT1(free, scanlineData);

    // Compose and write the TGA header
    unsigned char hdr[HDR_SIZE];
    (void) memset(&hdr, 0, HDR_SIZE);

    hdr[HDR_COLORTYPE] = wxTGA_UNMAPPED;
    hdr[HDR_IMAGETYPE] = 2 /* Uncompressed truecolour */;

    hdr[HDR_WIDTH] =  size.x & 0xFF;
    hdr[HDR_WIDTH + 1] =  (size.x >> 8) & 0xFF;

    hdr[HDR_HEIGHT] =  size.y & 0xFF;
    hdr[HDR_HEIGHT + 1] =  (size.y >> 8) & 0xFF;

    hdr[HDR_BPP] = hasAlpha ? 32 : 24;
    hdr[HDR_ORIENTATION] = 1 << 5; // set bit to indicate top-down order
    if (hasAlpha)
    {
        hdr[HDR_ORIENTATION] |= 8; // number of alpha bits
    }

    if ( !stream->Write(hdr, HDR_SIZE) )
    {
        return wxTGA_IOERR;
    }


    // Write image data, converting RGB to BGR and adding alpha if applicable

    unsigned char *src = image.GetData();
    unsigned char *alpha = image.GetAlpha();
    for (int y = 0; y < size.y; ++y)
    {
        unsigned char *dst = scanlineData;
        for (int x = 0; x < size.x; ++x)
        {
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            if (alpha)
            {
                dst[3] = *(alpha++);
            }
            src += 3;
            dst += bytesPerPixel;
        }
        if ( !stream->Write(scanlineData, scanlineSize) )
        {
            return wxTGA_IOERR;
        }
    }

    return wxTGA_OK;
}

// ----------------------------------------------------------------------------
// wxTGAHandler
// ----------------------------------------------------------------------------

bool wxTGAHandler::LoadFile(wxImage* image,
                            wxInputStream& stream,
                            bool verbose,
                            int WXUNUSED(index))
{
    if ( !CanRead(stream) )
    {
        if ( verbose )
        {
            wxLogError(wxT("TGA: this is not a TGA file."));
        }

        return false;
    }

    image->Destroy();

    int error = ReadTGA(image, stream);
    if ( error != wxTGA_OK )
    {
        if ( verbose )
        {
            switch ( error )
            {
                case wxTGA_INVFORMAT:
                    wxLogError(wxT("TGA: image format unsupported."));
                    break;

                case wxTGA_MEMERR:
                    wxLogError(wxT("TGA: couldn't allocate memory."));
                    break;

                case wxTGA_IOERR:
                    wxLogError(wxT("TGA: couldn't read image data."));
                    break;

                default:
                    wxLogError(wxT("TGA: unknown error!"));
            }
        }

        image->Destroy();

        return false;
    }

    return true;
}

bool wxTGAHandler::SaveFile(wxImage* image, wxOutputStream& stream, bool verbose)
{
    int error = SaveTGA(*image, &stream);

    if ( error != wxTGA_OK )
    {
        if ( verbose )
        {
            switch ( error )
            {
                case wxTGA_MEMERR:
                    wxLogError(wxT("TGA: couldn't allocate memory."));
                    break;

                case wxTGA_IOERR:
                    wxLogError(wxT("TGA: couldn't write image data."));
                    break;

                default:
                    wxLogError(wxT("TGA: unknown error!"));
            }
        }

        return false;
    }

    return true;
}

bool wxTGAHandler::DoCanRead(wxInputStream& stream)
{
    // read the fixed-size TGA headers
    unsigned char hdr[HDR_SIZE];
    stream.Read(hdr, HDR_SIZE);     // it's ok to modify the stream position here

    // Check wether we can read the file or not.

    short colorType = hdr[HDR_COLORTYPE];
    if ( colorType != wxTGA_UNMAPPED && colorType != wxTGA_MAPPED )
    {
        return false;
    }

    short imageType = hdr[HDR_IMAGETYPE];
    if ( imageType == 0 || imageType == 32 || imageType == 33 )
    {
        return false;
    }

    short bpp = hdr[HDR_BPP];
    if ( bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32 )
    {
        return false;
    }

    return true;
}

#endif // wxUSE_STREAMS

#endif // wxUSE_IMAGE && wxUSE_TGA
