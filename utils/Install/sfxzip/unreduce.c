/*---------------------------------------------------------------------------

  unreduce.c

  The Reducing algorithm is actually a combination of two distinct algorithms.
  The first algorithm compresses repeated byte sequences, and the second al-
  gorithm takes the compressed stream from the first algorithm and applies a
  probabilistic compression method.

     * Copyright 1989 Samuel H. Smith;  All rights reserved
     *
     * Do not distribute modified versions without my permission.
     * Do not remove or alter this notice or any other copyright notice.
     * If you use this in your own program you must distribute source code.
     * Do not use any of this in a commercial product.

  See the accompanying file "COPYING" in UnZip source and binary distributions
  for further information.  This code is NOT used unless USE_SMITH_CODE is
  explicitly defined (==> COPYRIGHT_CLEAN is not defined).

  ---------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "unzip.h"   /* defines COPYRIGHT_CLEAN by default */


#ifndef COPYRIGHT_CLEAN

/**************************************/
/*  UnReduce Defines, Typedefs, etc.  */
/**************************************/

#define DLE    144

typedef uch f_array[64];        /* for followers[256][64] */



/******************************/
/*  UnReduce Local Functions  */
/******************************/

static void LoadFollowers OF((__GPRO__ f_array *followers, uch *Slen));



/*******************************/
/*  UnReduce Global Constants  */
/*******************************/

static ZCONST shrint L_table[] =
{0, 0x7f, 0x3f, 0x1f, 0x0f};

static ZCONST shrint D_shift[] =
{0, 0x07, 0x06, 0x05, 0x04};
static ZCONST shrint D_mask[] =
{0, 0x01, 0x03, 0x07, 0x0f};

static ZCONST shrint B_table[] =
{8, 1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5,
 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6,
 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7,
 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8};





/*************************/
/*  Function unreduce()  */
/*************************/

void unreduce(__G)   /* expand probabilistically reduced data */
     __GDEF
{
    register int lchar = 0;
    shrint nchar;
    shrint ExState = 0;
    shrint V = 0;
    shrint Len = 0;
    long s = G.ucsize;  /* number of bytes left to decompress */
    unsigned w = 0;      /* position in output window slide[] */
    unsigned u = 1;      /* true if slide[] unflushed */
    uch Slen[256];

    f_array *followers = (f_array *)(slide + 0x4000);
    int factor = G.lrec.compression_method - 1;

    LoadFollowers(__G__ followers, Slen);

    while (s > 0 /* && (!zipeof) */) {
        if (Slen[lchar] == 0)
            READBITS(8, nchar)   /* ; */
        else {
            READBITS(1, nchar)   /* ; */
            if (nchar != 0)
                READBITS(8, nchar)       /* ; */
            else {
                shrint follower;
                int bitsneeded = B_table[Slen[lchar]];

                READBITS(bitsneeded, follower)   /* ; */
                nchar = followers[lchar][follower];
            }
        }
        /* expand the resulting byte */
        switch (ExState) {

        case 0:
            if (nchar != DLE) {
                s--;
                slide[w++] = (uch)nchar;
                if (w == 0x4000) {
                    flush(__G__ slide, (ulg)w, 0);
                    w = u = 0;
                }
            }
            else
                ExState = 1;
            break;

        case 1:
            if (nchar != 0) {
                V = nchar;
                Len = V & L_table[factor];
                if (Len == L_table[factor])
                    ExState = 2;
                else
                    ExState = 3;
            } else {
                s--;
                slide[w++] = DLE;
                if (w == 0x4000)
                {
                  flush(__G__ slide, (ulg)w, 0);
                  w = u = 0;
                }
                ExState = 0;
            }
            break;

        case 2:{
                Len += nchar;
                ExState = 3;
            }
            break;

        case 3:{
                register unsigned e;
                register unsigned n = Len + 3;
                register unsigned d = w - ((((V >> D_shift[factor]) &
                               D_mask[factor]) << 8) + nchar + 1);

                s -= n;
                do {
                  n -= (e = (e = 0x4000 - ((d &= 0x3fff) > w ? d : w)) > n ?
                        n : e);
                  if (u && w <= d)
                  {
                    memzero(slide + w, e);
                    w += e;
                    d += e;
                  }
                  else
                    if (w - d < e)      /* (assume unsigned comparison) */
                      do {              /* slow to avoid memcpy() overlap */
                        slide[w++] = slide[d++];
                      } while (--e);
                    else
                    {
                      memcpy(slide + w, slide + d, e);
                      w += e;
                      d += e;
                    }
                  if (w == 0x4000)
                  {
                    flush(__G__ slide, (ulg)w, 0);
                    w = u = 0;
                  }
                } while (n);

                ExState = 0;
            }
            break;
        }

        /* store character for next iteration */
        lchar = nchar;
    }

    /* flush out slide */
    flush(__G__ slide, (ulg)w, 0);
}





/******************************/
/*  Function LoadFollowers()  */
/******************************/

static void LoadFollowers(__G__ followers, Slen)
     __GDEF
     f_array *followers;
     uch *Slen;
{
    register int x;
    register int i;

    for (x = 255; x >= 0; x--) {
        READBITS(6, Slen[x])   /* ; */
        for (i = 0; (uch)i < Slen[x]; i++)
            READBITS(8, followers[x][i])   /* ; */
    }
}

#endif /* !COPYRIGHT_CLEAN */
