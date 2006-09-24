/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/gifdecod.cpp
// Purpose:     wxGIFDecoder, GIF reader for wxImage and wxAnimation
// Author:      Guillermo Rodriguez Garcia <guille@iies.es>
// Version:     3.04
// RCS-ID:      $Id$
// Copyright:   (c) Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STREAMS && wxUSE_GIF

#ifndef WX_PRECOMP
    #include "wx/palette.h"
#endif

#include <stdlib.h>
#include <string.h>
#include "wx/gifdecod.h"


//---------------------------------------------------------------------------
// GIFImage constructor
//---------------------------------------------------------------------------
GIFImage::GIFImage()
{
    w = 0;
    h = 0;
    left = 0;
    top = 0;
    transparent = 0;
    disposal = 0;
    delay = -1;
    p = (unsigned char *) NULL;
    pal = (unsigned char *) NULL;
    ncolours = 0;
    next = (GIFImage *) NULL;
    prev = (GIFImage *) NULL;
}

//---------------------------------------------------------------------------
// wxGIFDecoder constructor and destructor
//---------------------------------------------------------------------------

wxGIFDecoder::wxGIFDecoder(wxInputStream *s, bool anim)
{
    m_f    = s;
    m_anim = anim;

    m_background = -1;
    m_screenw = 0;
    m_screenh = 0;

    m_pimage  = NULL;
    m_pfirst  = NULL;
    m_plast   = NULL;
    m_image   = 0;
    m_nimages = 0;
}

wxGIFDecoder::~wxGIFDecoder()
{
    Destroy();
}

void wxGIFDecoder::Destroy()
{
    GIFImage *pimg, *paux;

    pimg = m_pfirst;

    while (pimg != NULL)
    {
        paux = pimg->next;
        free(pimg->p);
        free(pimg->pal);
        delete pimg;
        pimg = paux;
    }

    m_pimage  = NULL;
    m_pfirst  = NULL;
    m_plast   = NULL;
    m_image   = 0;
    m_nimages = 0;
}


//---------------------------------------------------------------------------
// Convert this image to a wxImage object
//---------------------------------------------------------------------------

// This function was designed by Vaclav Slavik

bool wxGIFDecoder::ConvertToImage(wxImage *image) const
{
    unsigned char *src, *dst, *pal;
    unsigned long i;
    int      transparent;

    /* just in case... */
    image->Destroy();

    /* create the image */
    image->Create(GetWidth(), GetHeight());

    if (!image->Ok())
        return false;

    pal = GetPalette();
    src = GetData();
    dst = image->GetData();
    transparent = GetTransparentColour();

    /* set transparent colour mask */
    if (transparent != -1)
    {
        for (i = 0; i < GetNcolours(); i++)
        {
            if ((pal[3 * i + 0] == 255) &&
                (pal[3 * i + 1] == 0) &&
                (pal[3 * i + 2] == 255))
            {
                pal[3 * i + 2] = 254;
            }
        }

        pal[3 * transparent + 0] = 255,
        pal[3 * transparent + 1] = 0,
        pal[3 * transparent + 2] = 255;

        image->SetMaskColour(255, 0, 255);
    }
    else
        image->SetMask(false);

#if wxUSE_PALETTE
    unsigned char r[256];
    unsigned char g[256];
    unsigned char b[256];

    for (i = 0; i < 256; i++)
    {
        r[i] = pal[3*i + 0];
        g[i] = pal[3*i + 1];
        b[i] = pal[3*i + 2];
    }

    image->SetPalette(wxPalette(GetNcolours(), r, g, b));
#endif // wxUSE_PALETTE

    /* copy image data */
    for (i = 0; i < (GetWidth() * GetHeight()); i++, src++)
    {
        *(dst++) = pal[3 * (*src) + 0];
        *(dst++) = pal[3 * (*src) + 1];
        *(dst++) = pal[3 * (*src) + 2];
    }

    return true;
}


//---------------------------------------------------------------------------
// Data accessors
//---------------------------------------------------------------------------

// Get data for current frame

int wxGIFDecoder::GetFrameIndex() const         { return m_image; }
unsigned char* wxGIFDecoder::GetData() const    { return (m_pimage->p); }
unsigned char* wxGIFDecoder::GetPalette() const { return (m_pimage->pal); }
unsigned int wxGIFDecoder::GetNcolours() const  { return (m_pimage->ncolours); }
unsigned int wxGIFDecoder::GetWidth() const     { return (m_pimage->w); }
unsigned int wxGIFDecoder::GetHeight() const    { return (m_pimage->h); }
unsigned int wxGIFDecoder::GetTop() const       { return (m_pimage->top); }
unsigned int wxGIFDecoder::GetLeft() const      { return (m_pimage->left); }
int wxGIFDecoder::GetTransparentColour() const  { return (m_pimage->transparent); }
int wxGIFDecoder::GetDisposalMethod() const     { return (m_pimage->disposal); }
long wxGIFDecoder::GetDelay() const             { return (m_pimage->delay); }

// Get global data

unsigned int wxGIFDecoder::GetLogicalScreenWidth() const    { return m_screenw; }
unsigned int wxGIFDecoder::GetLogicalScreenHeight() const   { return m_screenh; }
int wxGIFDecoder::GetBackgroundColour() const   { return m_background; }
int wxGIFDecoder::GetNumberOfFrames() const     { return m_nimages; }
bool wxGIFDecoder::IsAnimation() const          { return (m_nimages > 1); }


//---------------------------------------------------------------------------
// Functions to move through the animation
//---------------------------------------------------------------------------

bool wxGIFDecoder::GoFirstFrame()
{
    if (!IsAnimation())
        return false;

    m_image = 1;
    m_pimage = m_pfirst;
    return true;
}

bool wxGIFDecoder::GoLastFrame()
{
    if (!IsAnimation())
        return false;

    m_image = m_nimages;
    m_pimage = m_plast;
    return true;
}

bool wxGIFDecoder::GoNextFrame(bool cyclic)
{
    if (!IsAnimation())
        return false;

    if ((m_image < m_nimages) || (cyclic))
    {
        m_pimage = m_pimage->next;
        m_image++;

        if (!m_pimage)
        {
            m_image = 1;
            m_pimage = m_pfirst;
        }

        return true;
    }
    else
        return false;
}

bool wxGIFDecoder::GoPrevFrame(bool cyclic)
{
    if (!IsAnimation())
        return false;

    if ((m_image > 1) || (cyclic))
    {
        m_pimage = m_pimage->prev;
        m_image--;

        if (!m_pimage)
        {
            m_image = m_nimages;
            m_pimage = m_plast;
        }

        return true;
    }
    else
        return false;
}

bool wxGIFDecoder::GoFrame(int which)
{
    if (!IsAnimation())
        return false;

    if ((which >= 1) && (which <= m_nimages))
    {
        m_image = 1;
        m_pimage = m_pfirst;

        while (m_image < which)
        {
            m_image++;
            m_pimage = m_pimage->next;
        }

        return true;
    }
    else
        return false;
}


//---------------------------------------------------------------------------
// GIF reading and decoding
//---------------------------------------------------------------------------

// getcode:
//  Reads the next code from the file stream, with size 'bits'
//
int wxGIFDecoder::getcode(int bits, int ab_fin)
{
    unsigned int mask;          /* bit mask */
    unsigned int code;          /* code (result) */


    /* get remaining bits from last byte read */
    mask = (1 << bits) - 1;
    code = (m_lastbyte >> (8 - m_restbits)) & mask;

    /* keep reading new bytes while needed */
    while (bits > m_restbits)
    {
        /* if no bytes left in this block, read the next block */
        if (m_restbyte == 0)
        {
            m_restbyte = (unsigned char)m_f->GetC();

            /* Some encoders are a bit broken: instead of issuing
             * an end-of-image symbol (ab_fin) they come up with
             * a zero-length subblock!! We catch this here so
             * that the decoder sees an ab_fin code.
             */
            if (m_restbyte == 0)
            {
                code = ab_fin;
                break;
            }

            /* prefetch data */
            m_f->Read((void *) m_buffer, m_restbyte);
            if (m_f->LastRead() != m_restbyte)
            {
                code = ab_fin;
                return code;
            }
            m_bufp = m_buffer;
        }

        /* read next byte and isolate the bits we need */
        m_lastbyte = (unsigned char) (*m_bufp++);
        mask       = (1 << (bits - m_restbits)) - 1;
        code       = code + ((m_lastbyte & mask) << m_restbits);
        m_restbyte--;

        /* adjust total number of bits extracted from the buffer */
        m_restbits = m_restbits + 8;
    }

    /* find number of bits remaining for next code */
    m_restbits = (m_restbits - bits);

    return code;
}


// dgif:
//  GIF decoding function. The initial code size (aka root size)
//  is 'bits'. Supports interlaced images (interl == 1).
//  Returns wxGIF_OK (== 0) on success, or an error code if something
// fails (see header file for details)
int wxGIFDecoder::dgif(GIFImage *img, int interl, int bits)
{
    static const int allocSize = 4096 + 1;
    int *ab_prefix = new int[allocSize]; /* alphabet (prefixes) */
    if (ab_prefix == NULL)
    {
        return wxGIF_MEMERR;
    }

    int *ab_tail = new int[allocSize];   /* alphabet (tails) */
    if (ab_tail == NULL)
    {
        delete[] ab_prefix;
        return wxGIF_MEMERR;
    }

    int *stack = new int[allocSize];     /* decompression stack */
    if (stack == NULL)
    {
        delete[] ab_prefix;
        delete[] ab_tail;
        return wxGIF_MEMERR;
    }

    int ab_clr;                     /* clear code */
    int ab_fin;                     /* end of info code */
    int ab_bits;                    /* actual symbol width, in bits */
    int ab_free;                    /* first free position in alphabet */
    int ab_max;                     /* last possible character in alphabet */
    int pass;                       /* pass number in interlaced images */
    int pos;                        /* index into decompresion stack */
    unsigned int x, y;              /* position in image buffer */

    int code, readcode, lastcode, abcabca;

    /* these won't change */
    ab_clr = (1 << bits);
    ab_fin = (1 << bits) + 1;

    /* these will change through the decompression proccess */
    ab_bits  = bits + 1;
    ab_free  = (1 << bits) + 2;
    ab_max   = (1 << ab_bits) - 1;
    lastcode = -1;
    abcabca  = -1;
    pass     = 1;
    pos = x = y = 0;

    /* reset decoder vars */
    m_restbits = 0;
    m_restbyte = 0;
    m_lastbyte = 0;

    do
    {
        /* get next code */
        readcode = code = getcode(ab_bits, ab_fin);

        /* end of image? */
        if (code == ab_fin) break;

        /* reset alphabet? */
        if (code == ab_clr)
        {
            /* reset main variables */
            ab_bits  = bits + 1;
            ab_free  = (1 << bits) + 2;
            ab_max   = (1 << ab_bits) - 1;
            lastcode = -1;
            abcabca  = -1;

            /* skip to next code */
            continue;
        }

        /* unknown code: special case (like in ABCABCA) */
        if (code >= ab_free)
        {
            code = lastcode;            /* take last string */
            stack[pos++] = abcabca;     /* add first character */
        }

        /* build the string for this code in the stack */
        while (code > ab_clr)
        {
            stack[pos++] = ab_tail[code];
            code         = ab_prefix[code];

            // Don't overflow. This shouldn't happen with normal
            // GIF files, the allocSize of 4096+1 is enough. This
            // will only happen with badly formed GIFs.
            if (pos >= allocSize)
            {
                delete[] ab_prefix;
                delete[] ab_tail;
                delete[] stack;
                return wxGIF_INVFORMAT;
            }
        }

        if (pos >= allocSize)
        {
            delete[] ab_prefix;
            delete[] ab_tail;
            delete[] stack;
            return wxGIF_INVFORMAT;
        }

        stack[pos] = code;              /* push last code into the stack */
        abcabca    = code;              /* save for special case */

        /* make new entry in alphabet (only if NOT just cleared) */
        if (lastcode != -1)
        {
            // Normally, after the alphabet is full and can't grow any
            // further (ab_free == 4096), encoder should (must?) emit CLEAR
            // to reset it. This checks whether we really got it, otherwise
            // the GIF is damaged.
            if (ab_free > ab_max)
            {
                delete[] ab_prefix;
                delete[] ab_tail;
                delete[] stack;
                return wxGIF_INVFORMAT;
            }

            // This assert seems unnecessary since the condition above
            // eliminates the only case in which it went false. But I really
            // don't like being forced to ask "Who in .text could have
            // written there?!" And I wouldn't have been forced to ask if
            // this line had already been here.
            wxASSERT(ab_free < allocSize);

            ab_prefix[ab_free] = lastcode;
            ab_tail[ab_free]   = code;
            ab_free++;

            if ((ab_free > ab_max) && (ab_bits < 12))
            {
                ab_bits++;
                ab_max = (1 << ab_bits) - 1;
            }
        }

        /* dump stack data to the image buffer */
        while (pos >= 0)
        {
            (img->p)[x + (y * (img->w))] = (char) stack[pos];
            pos--;

            if (++x >= (img->w))
            {
                x = 0;

                if (interl)
                {
                    /* support for interlaced images */
                    switch (pass)
                    {
                        case 1: y += 8; break;
                        case 2: y += 8; break;
                        case 3: y += 4; break;
                        case 4: y += 2; break;
                    }

                    /* loop until a valid y coordinate has been
                    found, Or if the maximum number of passes has
                    been reached, exit the loop, and stop image
                    decoding (At this point the image is successfully
                    decoded).
                    If we don't loop, but merely set y to some other
                    value, that new value might still be invalid depending
                    on the height of the image. This would cause out of
                    bounds writing.
                    */
                    while (y >= (img->h))
                    {
                        switch (++pass)
                        {
                            case 2: y = 4; break;
                            case 3: y = 2; break;
                            case 4: y = 1; break;

                            default:
                                /*
                                It's possible we arrive here. For example this
                                happens when the image is interlaced, and the
                                height is 1. Looking at the above cases, the
                                lowest possible y is 1. While the only valid
                                one would be 0 for an image of height 1. So
                                'eventually' the loop will arrive here.
                                This case makes sure this while loop is
                                exited, as well as the 2 other ones.
                                */

                                // Set y to a valid coordinate so the local
                                // while loop will be exited. (y = 0 always
                                // is >= img->h since if img->h == 0 the
                                // image is never decoded)
                                y = 0;

                                // This will exit the other outer while loop
                                pos = -1;

                                // This will halt image decoding.
                                code = ab_fin;

                                break;
                        }
                    }
                }
                else
                {
                    /* non-interlaced */
                    y++;
/*
Normally image decoding is finished when an End of Information code is
encountered (code == ab_fin) however some broken encoders write wrong
"block byte counts" (The first byte value after the "code size" byte),
being one value too high. It might very well be possible other variants
of this problem occur as well. The only sensible solution seems to
be to check for clipping.
Example of wrong encoding:
(1 * 1 B/W image, raster data stream follows in hex bytes)

02  << B/W images have a code size of 2
02  << Block byte count
44  << LZW packed
00  << Zero byte count (terminates data stream)

Because the block byte count is 2, the zero byte count is used in the
decoding process, and decoding is continued after this byte. (While it
should signal an end of image)

It should be:
02
02
44
01  << When decoded this correctly includes the End of Information code
00

Or (Worse solution):
02
01
44
00
(The 44 doesn't include an End of Information code, but at least the
decoder correctly skips to 00 now after decoding, and signals this
as an End of Information itself)
*/
                    if (y >= img->h)
                    {
                        code = ab_fin;
                        break;
                    }
                }
            }
        }

        pos = 0;
        lastcode = readcode;
    }
    while (code != ab_fin);

    delete [] ab_prefix ;
    delete [] ab_tail ;
    delete [] stack ;

    return wxGIF_OK;
}


// CanRead:
//  Returns true if the file looks like a valid GIF, false otherwise.
//
bool wxGIFDecoder::CanRead()
{
    unsigned char buf[3];

    if ( !m_f->Read(buf, WXSIZEOF(buf)) )
        return false;

    m_f->SeekI(-(wxFileOffset)WXSIZEOF(buf), wxFromCurrent);

    return memcmp(buf, "GIF", WXSIZEOF(buf)) == 0;
}


// ReadGIF:
//  Reads and decodes one or more GIF images, depending on whether
//  animated GIF support is enabled. Can read GIFs with any bit
//  size (color depth), but the output images are always expanded
//  to 8 bits per pixel. Also, the image palettes always contain
//  256 colors, although some of them may be unused. Returns wxGIF_OK
//  (== 0) on success, or an error code if something fails (see
//  header file for details)
//
int wxGIFDecoder::ReadGIF()
{
    unsigned int  global_ncolors = 0;
    int           bits, interl, transparent, disposal, i;
    long          size;
    long          delay;
    unsigned char type = 0;
    unsigned char pal[768];
    unsigned char buf[16];
    GIFImage      **ppimg;
    GIFImage      *pimg, *pprev;

    /* check GIF signature */
    if (!CanRead())
        return wxGIF_INVFORMAT;

    /* check for animated GIF support (ver. >= 89a) */

    static const size_t headerSize = (3 + 3);
    m_f->Read(buf, headerSize);
    if (m_f->LastRead() != headerSize)
    {
        return wxGIF_INVFORMAT;
    }

    if (memcmp(buf + 3, "89a", 3) < 0)
    {
        m_anim = false;
    }

    /* read logical screen descriptor block (LSDB) */
    static const size_t lsdbSize = (2 + 2 + 1 + 1 + 1);
    m_f->Read(buf, lsdbSize);
    if (m_f->LastRead() != lsdbSize)
    {
        return wxGIF_INVFORMAT;
    }

    m_screenw = buf[0] + 256 * buf[1];
    m_screenh = buf[2] + 256 * buf[3];

    if ((m_screenw == 0) || (m_screenh == 0))
    {
        return wxGIF_INVFORMAT;
    }

    /* load global color map if available */
    if ((buf[4] & 0x80) == 0x80)
    {
        m_background = buf[5];

        global_ncolors = 2 << (buf[4] & 0x07);
        size_t numBytes = 3 * global_ncolors;
        m_f->Read(pal, numBytes);
        if (m_f->LastRead() != numBytes)
        {
            return wxGIF_INVFORMAT;
        }
    }

    /* transparent colour, disposal method and delay default to unused */
    transparent = -1;
    disposal = -1;
    delay = -1;

    /* read images */
    ppimg = &m_pfirst;
    pprev = NULL;
    pimg  = NULL;

    bool done = false;

    while (!done)
    {
        type = (unsigned char)m_f->GetC();

        /*
        If the end of file has been reached (or an error) and a ";"
        (0x3B) hasn't been encountered yet, exit the loop. (Without this
        check the while loop would loop endlessly.) Later on, in the next while
        loop, the file will be treated as being truncated (But still
        be decoded as far as possible). returning wxGIF_TRUNCATED is not
        possible here since some init code is done after this loop.
        */
        if (m_f->Eof())// || !m_f->IsOk())
        {
            /*
            type is set to some bogus value, so there's no
            need to continue evaluating it.
            */
            break; // Alternative : "return wxGIF_INVFORMAT;"
        }

        /* end of data? */
        if (type == 0x3B)
        {
            done = true;
        }
        else
        /* extension block? */
        if (type == 0x21)
        {
            if (((unsigned char)m_f->GetC()) == 0xF9)
            /* graphics control extension, parse it */
            {
                static const size_t gceSize = 6;
                m_f->Read(buf, gceSize);
                if (m_f->LastRead() != gceSize)
                {
                    Destroy();
                    return wxGIF_INVFORMAT;
                }

                /* read delay and convert from 1/100 of a second to ms */
                delay = 10 * (buf[2] + 256 * buf[3]);

                /* read transparent colour index, if used */
                if (buf[1] & 0x01)
                    transparent = buf[4];

                /* read disposal method */
                disposal = ((buf[1] & 0x1C) >> 2) - 1;
            }
            else
            /* other extension, skip */
            {
                while ((i = (unsigned char)m_f->GetC()) != 0)
                {
                    m_f->SeekI(i, wxFromCurrent);
                    if (m_f->Eof())
                    {
                        done = true;
                        break;
                    }
                }
            }
        }
        else
        /* image descriptor block? */
        if (type == 0x2C)
        {
            /* allocate memory for IMAGEN struct */
            pimg = (*ppimg) = new GIFImage();

            if (pimg == NULL)
            {
                Destroy();
                return wxGIF_MEMERR;
            }

            /* fill in the data */
            static const size_t idbSize = (2 + 2 + 2 + 2 + 1);
            m_f->Read(buf, idbSize);
            if (m_f->LastRead() != idbSize)
            {
                Destroy();
                return wxGIF_INVFORMAT;
            }

            pimg->left = buf[0] + 256 * buf[1];
            pimg->top = buf[2] + 256 * buf[3];
/*
            pimg->left = buf[4] + 256 * buf[5];
            pimg->top = buf[4] + 256 * buf[5];
*/
            pimg->w = buf[4] + 256 * buf[5];
            pimg->h = buf[6] + 256 * buf[7];

            if ((pimg->w == 0) || (pimg->w > m_screenw) || (pimg->h == 0) || (pimg->h > m_screenh))
            {
                Destroy();
                return wxGIF_INVFORMAT;
            }

            interl = ((buf[8] & 0x40)? 1 : 0);
            size = pimg->w * pimg->h;

            pimg->transparent = transparent;
            pimg->disposal = disposal;
            pimg->delay = delay;
            pimg->next = NULL;
            pimg->prev = pprev;
            pprev = pimg;
            ppimg = &pimg->next;

            /* allocate memory for image and palette */
            pimg->p   = (unsigned char *) malloc((size_t)size);
            pimg->pal = (unsigned char *) malloc(768);

            if ((!pimg->p) || (!pimg->pal))
            {
                Destroy();
                return wxGIF_MEMERR;
            }

            /* load local color map if available, else use global map */
            if ((buf[8] & 0x80) == 0x80)
            {
                unsigned int local_ncolors = 2 << (buf[8] & 0x07);
                size_t numBytes = 3 * local_ncolors;
                m_f->Read(pimg->pal, numBytes);
                pimg->ncolours = local_ncolors;
                if (m_f->LastRead() != numBytes)
                {
                    Destroy();
                    return wxGIF_INVFORMAT;
                }
            }
            else
            {
                memcpy(pimg->pal, pal, 768);
                pimg->ncolours = global_ncolors;
            }

            /* get initial code size from first byte in raster data */
            bits = (unsigned char)m_f->GetC();
            if (bits == 0)
            {
                Destroy();
                return wxGIF_INVFORMAT;
            }

            /* decode image */
            int result = dgif(pimg, interl, bits);
            if (result != wxGIF_OK)
            {
                Destroy();
                return result;
            }
            m_nimages++;

            /* if this is not an animated GIF, exit after first image */
            if (!m_anim)
                done = true;
        }
    }

    if (m_nimages <= 0)
    {
        Destroy();
        return wxGIF_INVFORMAT;
    }

    /* setup image pointers */
    m_image = 1;
    m_plast = pimg;
    m_pimage = m_pfirst;

    /* try to read to the end of the stream */
    while (type != 0x3B)
    {
        if (!m_f->IsOk())
            return wxGIF_TRUNCATED;

        type = (unsigned char)m_f->GetC();

        if (type == 0x21)
        {
            /* extension type */
            (void) m_f->GetC();

            /* skip all data */
            while ((i = (unsigned char)m_f->GetC()) != 0)
            {
                m_f->SeekI(i, wxFromCurrent);
            }
        }
        else if (type == 0x2C)
        {
            /* image descriptor block */
            static const size_t idbSize = (2 + 2 + 2 + 2 + 1);
            m_f->Read(buf, idbSize);
            if (m_f->LastRead() != idbSize)
            {
                Destroy();
                return wxGIF_INVFORMAT;
            }

            /* local color map */
            if ((buf[8] & 0x80) == 0x80)
            {
                unsigned int local_ncolors = 2 << (buf[8] & 0x07);
                wxFileOffset pos = m_f->TellI();
                wxFileOffset numBytes = 3 * local_ncolors;
                m_f->SeekI(numBytes, wxFromCurrent);
                if (m_f->TellI() != (pos + numBytes))
                {
                    Destroy();
                    return wxGIF_INVFORMAT;
                }
            }

            /* initial code size */
            (void) m_f->GetC();

            /* skip all data */
            while ((i = (unsigned char)m_f->GetC()) != 0)
            {
                m_f->SeekI(i, wxFromCurrent);
            }
        }
        else if ((type != 0x3B) && (type != 00)) /* testing */
        {
            /* images are OK, but couldn't read to the end of the stream */
            return wxGIF_TRUNCATED;
        }
    }

    return wxGIF_OK;
}

#endif // wxUSE_STREAMS && wxUSE_GIF
