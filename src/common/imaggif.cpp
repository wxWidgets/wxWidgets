/////////////////////////////////////////////////////////////////////////////
// Name:        imaggif.cpp
// Purpose:     wxGIFHandler
// Author:      Guillermo Rodriguez Garcia
//              wxWindows adaptation by Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
   We don't put pragma implement in this file because it is already present in
   src/common/image.cpp
*/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
#endif

#include "wx/image.h"
#include "wx/wfstream.h"
#include "wx/module.h"
#include "wx/log.h"

IMPLEMENT_DYNAMIC_CLASS(wxGIFHandler,wxImageHandler)

#if wxUSE_STREAMS

/*

FOLLOWING CODE IS BY G.R.G. :
(except wxInputStream stuff)

*/

/************************************************************************
  READGIF.H - Reads a GIF file
 ------------------------------------------------------------------------
  Guillermo Rodriguez Garcia
  <guille@iies.es>

  Version: 2.1
*************************************************************************/


typedef struct
{
    int w;                  /* width */
    int h;                  /* height */
    int transparent;        /* transparent color (-1 = none) */
    unsigned char *p;       /* bitmap */
    unsigned char *pal;     /* palette */
} IMAGEN;



/************************************************************************
  READGIF.C - Reads a GIF file
 ------------------------------------------------------------------------
  Guillermo Rodriguez Garcia
  <guille@iies.es>

  Version: 2.1
*************************************************************************/


#include <stdlib.h>
#include <string.h>


/* error codes */
#define E_OK        0               /* everything was OK */
#define E_ARCHIVO   -1              /* error opening file */
#define E_FORMATO   -2              /* error in gif header */
#define E_MEMORIA   -3              /* error allocating memory */


/* This class binding is by VS, so all bugs in it are mine ;-) */

class gifDecoder
{
    private:
        /* globals */
        int            restbits;            /* remaining valid bits */
        unsigned int   restbyte;            /* remaining bytes in this block */
        unsigned int   lastbyte;            /* last byte read */

        wxInputStream  *f;                  /* input file */

	public:
		gifDecoder(wxInputStream *s) { f = s; }
		int getcode(int bits);
		int dgif(IMAGEN *img, int interl, int bits);
		int readgif(IMAGEN *img);
};



/* getcode:
 *  Reads the next code from the file, with size 'bits'
 */
int gifDecoder::getcode(int bits)
{
    unsigned int    mask;           /* bit mask */
    unsigned int    code;           /* code (result) */


    /* get remaining bits from last byte read */
    mask = (1 << bits) - 1;
    code = (lastbyte >> (8 - restbits)) & mask;

    /* keep reading new bytes while needed */
    while (bits > restbits)
    {
        /* if no bytes left in this block, read the next block */
        if (restbyte == 0)
            restbyte = (unsigned char)f->GetC();

        /* read next byte and isolate the bits we need */
        lastbyte = (unsigned char)f->GetC();
        mask     = (1 << (bits - restbits)) - 1;
        code     = code + ((lastbyte & mask) << restbits);
        restbyte--;

        /* adjust total number of bits extracted from the buffer */
        restbits = restbits + 8;
    }
    
    /* find number of bits reamining for next code */
    restbits = (restbits - bits);

    return code;
}



/* dgif:
 *  GIF decoding function. The initial code size (aka root size)
 *  is 'bits'. Supports interlaced images (interl == 1).
 */
int gifDecoder::dgif(IMAGEN *img, int interl, int bits)
{
    int ab_prefix[4096];        /* alphabet (prefixes) */
    int ab_tail[4096];          /* alphabet (tails) */
    int stack[4096];            /* decompression stack */

    int ab_clr;                 /* clear code */
    int ab_fin;                 /* end of info code */
    int ab_bits;                /* actual symbol width, in bits */
    int ab_free;                /* first free position in alphabet */
    int ab_max;                 /* last possible character in alphabet */
    int pass;                   /* pass number in interlaced images */
    int pos;                    /* index into decompresion stack */
    int x, y;                   /* position in image buffer */

    int code, readcode, lastcode, abcabca;

    /* these won't change */
    ab_clr  = (1 << bits);
    ab_fin  = (1 << bits) + 1;

    /* these will change through the decompression proccess */
    ab_bits  = bits + 1;
    ab_free  = (1 << bits) + 2;
    ab_max   = (1 << ab_bits) - 1;
    lastcode = -1;
    abcabca  = -1;
    pass     = 1;
    pos = x = y = 0;

    /* reset static globals */
    restbits = 0;
    restbyte = 0;
    lastbyte = 0;

    do
    {
        /* get next code */
        readcode = code = getcode(ab_bits);

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
        }
        stack[pos] = code;              /* push last code into the stack */
        abcabca    = code;              /* save for special case */

        /* make new entry in alphabet (only if NOT just cleared) */
        if (lastcode != -1)
        {
            ab_prefix[ab_free] = lastcode;
            ab_tail[ab_free]   = code;
            ab_free++;

            if ((ab_free > ab_max) && (ab_bits < 12))
            {
                ab_bits++;
                ab_max = (1 << ab_bits) - 1;
            }
        }

        /* dump stack data to the buffer */
        while (pos >= 0)
        {
            (img->p)[x + (y * (img->w))] = (char)stack[pos--];

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
                    if (y >= (img->h))
                    {
                        switch (++pass)
                        {
                            case 2: y = 4; break;
                            case 3: y = 2; break;
                            case 4: y = 1; break;
                        }
                    }
                }
                else
                {
                    /* non-interlaced */
                    y++;
                }
            }
        }

        pos = 0;
        lastcode = readcode;
    }
    while (code != ab_fin);

    return 0;
}



/* readgif:
 *  Reads a GIF image from the file with filename 'nombre' in the
 *  IMAGEN structure pointed by 'img'. Can read GIFs with any bit
 *  size (color depth), but the output image is always expanded
 *  to 8 bits per pixel. Also, the image palette always contains
 *  256 colors, although some of them may be unused. Returns E_OK
 *  (== 0) on success, or an error code if something fails. Error
 *  codes are E_ARCHIVO, E_FORMATO, E_MEMORIA (see above).
 */
int gifDecoder::readgif(IMAGEN *img)
{
    int             size, ncolors, bits, interl, i;
    unsigned char   pal[768];
    unsigned char   buf[16];


    /* read header and logical screen descriptor block (LSDB) */
    f->Read(buf, 13);

    /* check GIF signature */
    if (memcmp(buf, "GIF", 3) != 0) return E_FORMATO;

    /* load global color map if available */
    if ((buf[10] & 0x80) == 0x80)
    {
        ncolors = 2 << (buf[10] & 0x07);
        f->Read(pal, 3 * ncolors);
    }

    /* assume no transparent color */
    img->transparent = -1;

    /* skip most extensions */
    while (((unsigned char)f->GetC()) == 0x21)               /* separator */
    {
        if (((unsigned char)f->GetC()) == 0xF9)              /* graphic control ext. */
        {
            f->Read(buf, 6);
            if (buf[1] & 0x01)
                img->transparent = buf[4];
        }
        else
            while ((i = (unsigned char)f->GetC()) != 0)    /* byte count */
                f->SeekI(i, wxFromCurrent);
    }

    /* read image descriptor block (IDB) */
    f->Read(buf, 9);
    img->w = buf[4] + 256 * buf[5];
    img->h = buf[6] + 256 * buf[7];
    size   = img->w * img->h;
    interl = ((buf[8] & 0x40)? 1 : 0);

    /* load local color map if available */
    if ((buf[8] & 0x80) == 0x80)
    {
        ncolors = 2 << (buf[8] & 0x07);
        f->Read(pal, 3 * ncolors);
    }

    /* get initial code size from first byte in raster data */
    bits = (unsigned char)f->GetC();

    /* allocate memory for image and palette */
    if ((img->p   = (unsigned char*) malloc(size)) == NULL) return E_MEMORIA;
    if ((img->pal = (unsigned char*) malloc(768))  == NULL) return E_MEMORIA;

    /* shift palette to fit VGA 6-bit format */
    for (i = 0; i < 768; i++)
        (img->pal)[i] = (unsigned char)pal[i]; /* >> 2 not in wxWin */

    /* decode GIF */
    dgif(img, interl, bits);

    /* finish successfully :-) */
    return E_OK;
}



/*

FOLLOWING CODE IS BY V.S. :

*/

//-----------------------------------------------------------------------------
// wxGIFHandler
//-----------------------------------------------------------------------------

bool wxGIFHandler::LoadFile( wxImage *image, wxInputStream& stream )
{
    unsigned char *ptr, *src, *pal;
    IMAGEN igif;
    int i;
    gifDecoder *decod;

    image->Destroy();

    decod = new gifDecoder(&stream);

    if (decod->readgif(&igif) != E_OK) {
        wxLogDebug(_T("Error reading GIF"));
        delete decod;
        return FALSE;
    }
    delete decod;

    image->Create(igif.w, igif.h);
    if (!image->Ok()) {
        free(igif.pal);
        free(igif.p);
        return FALSE;
    }

    ptr = image->GetData();
    src = igif.p;
    pal = igif.pal;

    if (igif.transparent != -1) {
        for (i = 0; i < 256; i++)
            if ((pal[3 * i + 0] == 255) && (pal[3 * i + 1] == 0) && (pal[3 * i + 2] == 255)) pal[3 * i + 2] = 254;
        pal[3 * (igif.transparent) + 0] = 255,
        pal[3 * (igif.transparent) + 1] = 0,
        pal[3 * (igif.transparent) + 2] = 255;
        image->SetMaskColour(255, 0, 255);
    }
    else image->SetMask(FALSE);

    for (i = 0; i < igif.w * igif.h; i++, src++) {
        *(ptr++) = pal[3 * (*src) + 0];
        *(ptr++) = pal[3 * (*src) + 1];
        *(ptr++) = pal[3 * (*src) + 2];
    }

    free(igif.pal);
    free(igif.p);
    return TRUE;
}

bool wxGIFHandler::SaveFile( wxImage * WXUNUSED(image),
                             wxOutputStream& WXUNUSED(stream) )
{
    wxLogDebug(_T("wxGIFHandler is read-only!!"));
    return FALSE;
}

#endif
