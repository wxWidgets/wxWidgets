/*
 * Copyright (C) 1989-95 GROUPE BULL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * GROUPE BULL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of GROUPE BULL shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from GROUPE BULL.
 */

/*****************************************************************************\
* scan.c:                                                                     *
*                                                                             *
*  XPM library                                                                *
*  Scanning utility for XPM file format                                       *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

/*
 * The code related to FOR_MSW has been added by
 * HeDu (hedu@cul-ipn.uni-kiel.de) 4/94
 */

/*
 * The code related to AMIGA has been added by
 * Lorens Younes (d93-hyo@nada.kth.se) 4/96
 */

#include "XpmI.h"

#define MAXPRINTABLE 92			/* number of printable ascii chars
					 * minus \ and " for string compat
					 * and ? to avoid ANSI trigraphs. */

static char *printable =
" .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjklzxcvbnmMNBVCZ\
ASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

/*
 * printable begin with a space, so in most case, due to my algorithm, when
 * the number of different colors is less than MAXPRINTABLE, it will give a
 * char follow by "nothing" (a space) in the readable xpm file
 */


typedef struct {
    Pixel *pixels;
    unsigned int *pixelindex;
    unsigned int size;
    unsigned int ncolors;
    unsigned int mask_pixel;		/* whether there is or not */
}      PixelsMap;

LFUNC(storePixel, int, (Pixel pixel, PixelsMap *pmap,
			unsigned int *index_return));

LFUNC(storeMaskPixel, int, (Pixel pixel, PixelsMap *pmap,
			    unsigned int *index_return));

#ifndef FOR_MSW
# ifndef AMIGA
LFUNC(GetImagePixels, int, (XImage *image, unsigned int width,
			    unsigned int height, PixelsMap *pmap));

LFUNC(GetImagePixels32, int, (XImage *image, unsigned int width,
			      unsigned int height, PixelsMap *pmap));

LFUNC(GetImagePixels16, int, (XImage *image, unsigned int width,
			      unsigned int height, PixelsMap *pmap));

LFUNC(GetImagePixels8, int, (XImage *image, unsigned int width,
			     unsigned int height, PixelsMap *pmap));

LFUNC(GetImagePixels1, int, (XImage *image, unsigned int width,
			     unsigned int height, PixelsMap *pmap,
			     int (*storeFunc) ()));
# else /* AMIGA */
LFUNC(AGetImagePixels, int, (XImage *image, unsigned int width,
			     unsigned int height, PixelsMap *pmap,
			     int (*storeFunc) ()));
# endif/* AMIGA */
#else  /* ndef FOR_MSW */
LFUNC(MSWGetImagePixels, int, (Display *d, XImage *image, unsigned int width,
			       unsigned int height, PixelsMap *pmap,
			       int (*storeFunc) ()));
#endif
LFUNC(ScanTransparentColor, int, (XpmColor *color, unsigned int cpp,
				  XpmAttributes *attributes));

LFUNC(ScanOtherColors, int, (Display *display, XpmColor *colors, int ncolors,
			     Pixel *pixels, unsigned int mask,
			     unsigned int cpp, XpmAttributes *attributes));

/*
 * This function stores the given pixel in the given arrays which are grown
 * if not large enough.
 */
#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int storePixel(Pixel pixel, PixelsMap* pmap, unsigned int* index_return)
#else
static int
storePixel(pixel, pmap, index_return)
    Pixel pixel;
    PixelsMap *pmap;
    unsigned int *index_return;
#endif
{
    unsigned int i;
    Pixel *p;
    unsigned int ncolors;

    if (*index_return) {		/* this is a transparent pixel! */
	*index_return = 0;
	return 0;
    }
    ncolors = pmap->ncolors;
    p = pmap->pixels + pmap->mask_pixel;
    for (i = pmap->mask_pixel; i < ncolors; i++, p++)
	if (*p == pixel)
	    break;
    if (i == ncolors) {
	if (ncolors >= pmap->size) {
	    pmap->size *= 2;
	    p = (Pixel *) XpmRealloc(pmap->pixels, sizeof(Pixel) * pmap->size);
	    if (!p)
		return (1);
	    pmap->pixels = p;

	}
	(pmap->pixels)[ncolors] = pixel;
	pmap->ncolors++;
    }
    *index_return = i;
    return 0;
}

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int storeMaskPixel(Pixel pixel, PixelsMap* pmap, unsigned int* index_return)
#else
static int
storeMaskPixel(pixel, pmap, index_return)
    Pixel pixel;
    PixelsMap *pmap;
    unsigned int *index_return;
#endif
{
    if (!pixel) {
	if (!pmap->ncolors) {
	    pmap->ncolors = 1;
	    (pmap->pixels)[0] = 0;
	    pmap->mask_pixel = 1;
	}
	*index_return = 1;
    } else
	*index_return = 0;
    return 0;
}

/* function call in case of error */
#undef RETURN
#define RETURN(status) \
{ \
      ErrorStatus = status; \
      goto error; \
}

/*
 * This function scans the given image and stores the found informations in
 * the given XpmImage structure.
 */
#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
int XpmCreateXpmImageFromImage(
  Display*       display
, XImage*        image
, XImage*        shapeimage
, XpmImage*      xpmimage
, XpmAttributes* attributes
)
#else
int
XpmCreateXpmImageFromImage(display, image, shapeimage,
			   xpmimage, attributes)
    Display *display;
    XImage *image;
    XImage *shapeimage;
    XpmImage *xpmimage;
    XpmAttributes *attributes;
#endif
{
    /* variables stored in the XpmAttributes structure */
    unsigned int cpp;

    /* variables to return */
    PixelsMap pmap;
    XpmColor *colorTable = NULL;
    int ErrorStatus;

    /* calculation variables */
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int cppm;			/* minimum chars per pixel */
    unsigned int c;

    /* initialize pmap */
    pmap.pixels = NULL;
    pmap.pixelindex = NULL;
    pmap.size = 256;			/* should be enough most of the time */
    pmap.ncolors = 0;
    pmap.mask_pixel = 0;

    /*
     * get geometry
     */
    if (image) {
	width = image->width;
	height = image->height;
    } else if (shapeimage) {
	width = shapeimage->width;
	height = shapeimage->height;
    }

    /*
     * retrieve information from the XpmAttributes
     */
    if (attributes && (attributes->valuemask & XpmCharsPerPixel
/* 3.2 backward compatibility code */
		       || attributes->valuemask & XpmInfos))
/* end 3.2 bc */
	cpp = attributes->cpp;
    else
	cpp = 0;

    pmap.pixelindex =
	(unsigned int *) XpmCalloc(width * height, sizeof(unsigned int));
    if (!pmap.pixelindex)
	RETURN(XpmNoMemory);

    pmap.pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * pmap.size);
    if (!pmap.pixels)
	RETURN(XpmNoMemory);

    /*
     * scan shape mask if any
     */
    if (shapeimage)
    {
#ifndef FOR_MSW
# ifndef AMIGA
	    ErrorStatus = GetImagePixels1(shapeimage, width, height, &pmap,
		    		      storeMaskPixel);
# else
	    ErrorStatus = AGetImagePixels(shapeimage, width, height, &pmap,
		    		      storeMaskPixel);
# endif /* AMIGA */
#else

#ifndef __OS2__
	    ErrorStatus = MSWGetImagePixels(display, shapeimage, width, height,
		    			&pmap, storeMaskPixel);
/* calling convention all messed up OS/2 -- figure out later */
#endif

#endif /* ndef for FOR_MSW */

	if (ErrorStatus != XpmSuccess)
	    RETURN(ErrorStatus);
    }

    /*
     * scan the image data
     *
     * In case depth is 1 or bits_per_pixel is 4, 6, 8, 24 or 32 use optimized
     * functions, otherwise use slower but sure general one.
     *
     */

    if (image)
    {
#ifndef FOR_MSW
# ifndef AMIGA
	    if (((image->bits_per_pixel | image->depth) == 1)  &&
	        (image->byte_order == image->bitmap_bit_order))
    	    ErrorStatus = GetImagePixels1(image, width, height, &pmap,
	    				  storePixel);
	    else if (image->format == ZPixmap)
    	{
	        if (image->bits_per_pixel == 8)
    		    ErrorStatus = GetImagePixels8(image, width, height, &pmap);
	        else if (image->bits_per_pixel == 16)
		        ErrorStatus = GetImagePixels16(image, width, height, &pmap);
    	    else if (image->bits_per_pixel == 32)
	        	ErrorStatus = GetImagePixels32(image, width, height, &pmap);
    	}
	    else
	        ErrorStatus = GetImagePixels(image, width, height, &pmap);
# else
	    ErrorStatus = AGetImagePixels(image, width, height, &pmap,
		    		      storePixel);
# endif /* AMIGA */
#else

#ifndef __VISAGECPP30__
	    ErrorStatus = MSWGetImagePixels(display, image, width, height, &pmap,
		    			storePixel);
#endif

#endif

	    if (ErrorStatus != XpmSuccess)
	        RETURN(ErrorStatus);
    }

    /*
     * get rgb values and a string of char, and possibly a name for each
     * color
     */

    colorTable = (XpmColor *) XpmCalloc(pmap.ncolors, sizeof(XpmColor));
    if (!colorTable)
	RETURN(XpmNoMemory);

    /* compute the minimal cpp */
    for (cppm = 1, c = MAXPRINTABLE; pmap.ncolors > c; cppm++)
	c *= MAXPRINTABLE;
    if (cpp < cppm)
	cpp = cppm;

    if (pmap.mask_pixel) {
	ErrorStatus = ScanTransparentColor(colorTable, cpp, attributes);
	if (ErrorStatus != XpmSuccess)
	    RETURN(ErrorStatus);
    }

    ErrorStatus = ScanOtherColors(display, colorTable, pmap.ncolors,
				  pmap.pixels, pmap.mask_pixel, cpp,
				  attributes);
    if (ErrorStatus != XpmSuccess)
	RETURN(ErrorStatus);

    /*
     * store found informations in the XpmImage structure
     */
    xpmimage->width = width;
    xpmimage->height = height;
    xpmimage->cpp = cpp;
    xpmimage->ncolors = pmap.ncolors;
    xpmimage->colorTable = colorTable;
    xpmimage->data = pmap.pixelindex;

    XpmFree(pmap.pixels);
    return (XpmSuccess);

/* exit point in case of error, free only locally allocated variables */
error:
    if (pmap.pixelindex)
	XpmFree(pmap.pixelindex);
    if (pmap.pixels)
	XpmFree(pmap.pixels);
    if (colorTable)
	xpmFreeColorTable(colorTable, pmap.ncolors);

    return (ErrorStatus);
}

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int ScanTransparentColor(XpmColor* color, unsigned int cpp, XpmAttributes* attributes)
#else
static int
ScanTransparentColor(color, cpp, attributes)
    XpmColor *color;
    unsigned int cpp;
    XpmAttributes *attributes;
#endif
{
    char *s;
    unsigned int a, b, c;

    /* first get a character string */
    a = 0;
    if (!(s = color->string = (char *) XpmMalloc(cpp + 1)))
	return (XpmNoMemory);
    *s++ = printable[c = a % MAXPRINTABLE];
    for (b = 1; b < cpp; b++, s++)
	*s = printable[c = ((a - c) / MAXPRINTABLE) % MAXPRINTABLE];
    *s = '\0';

    /* then retreive related info from the attributes if any */
    if (attributes && (attributes->valuemask & XpmColorTable
/* 3.2 backward compatibility code */
		       || attributes->valuemask & XpmInfos)
/* end 3.2 bc */
	&& attributes->mask_pixel != XpmUndefPixel) {

	unsigned int key;
	char **defaults = (char **) color;
	char **mask_defaults;

/* 3.2 backward compatibility code */
	if (attributes->valuemask & XpmColorTable)
/* end 3.2 bc */
	    mask_defaults = (char **) (
		attributes->colorTable + attributes->mask_pixel);
/* 3.2 backward compatibility code */
	else
	    mask_defaults = (char **)
		((XpmColor **) attributes->colorTable)[attributes->mask_pixel];
/* end 3.2 bc */
	for (key = 1; key <= NKEYS; key++) {
	    if (s = mask_defaults[key]) {
		defaults[key] = (char *) xpmstrdup(s);
		if (!defaults[key])
		    return (XpmNoMemory);
	    }
	}
    } else {
	color->c_color = (char *) xpmstrdup(TRANSPARENT_COLOR);
	if (!color->c_color)
	    return (XpmNoMemory);
    }
    return (XpmSuccess);
}

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int ScanOtherColors(
  Display*       display
, XpmColor*      colors
, int            ncolors
, Pixel*         pixels
, unsigned int   mask
, unsigned int   cpp
, XpmAttributes* attributes
)
#else
static int
ScanOtherColors(display, colors, ncolors, pixels, mask, cpp, attributes)
    Display *display;
    XpmColor *colors;
    int ncolors;
    Pixel *pixels;
    unsigned int mask;
    unsigned int cpp;
    XpmAttributes *attributes;
#endif
{
    /* variables stored in the XpmAttributes structure */
    Colormap colormap;
    char *rgb_fname;

#ifndef FOR_MSW
    xpmRgbName rgbn[MAX_RGBNAMES];
#else
    xpmRgbName *rgbn = NULL;
#endif
    int rgbn_max = 0;
    unsigned int i, j, c, i2;
    XpmColor *color;
    XColor *xcolors = NULL, *xcolor;
    char *colorname, *s;
    XpmColor *colorTable, **oldColorTable = NULL;
    unsigned int ancolors = 0;
    Pixel *apixels;
    unsigned int mask_pixel;
    Bool found;

    /* retrieve information from the XpmAttributes */
    if (attributes && (attributes->valuemask & XpmColormap))
	colormap = attributes->colormap;
    else
	colormap = XDefaultColormap(display, XDefaultScreen(display));
    if (attributes && (attributes->valuemask & XpmRgbFilename))
	rgb_fname = attributes->rgb_fname;
    else
	rgb_fname = NULL;

    /* start from the right element */
    if (mask) {
	colors++;
	ncolors--;
	pixels++;
    }

    /* first get character strings and rgb values */
    xcolors = (XColor *) XpmMalloc(sizeof(XColor) * ncolors);
    if (!xcolors)
	return (XpmNoMemory);

    for (i = 0, i2 = mask, color = colors, xcolor = xcolors;
	 i < ncolors; i++, i2++, color++, xcolor++, pixels++) {

	if (!(s = color->string = (char *) XpmMalloc(cpp + 1))) {
	    XpmFree(xcolors);
	    return (XpmNoMemory);
	}
	*s++ = printable[c = i2 % MAXPRINTABLE];
	for (j = 1; j < cpp; j++, s++)
	    *s = printable[c = ((i2 - c) / MAXPRINTABLE) % MAXPRINTABLE];
	*s = '\0';

	xcolor->pixel = *pixels;
    }
#ifdef __OS2__
    XQueryColors(display, &colormap, xcolors, ncolors);
#else
    XQueryColors(display, colormap, xcolors, ncolors);
#endif

#ifndef FOR_MSW
    /* read the rgb file if any was specified */
    if (rgb_fname)
	rgbn_max = xpmReadRgbNames(attributes->rgb_fname, rgbn);
#else
    /* FOR_MSW: rgb names and values are hardcoded in rgbtab.h */
    rgbn_max = xpmReadRgbNames(NULL, NULL);
#endif

    if (attributes && attributes->valuemask & XpmColorTable) {
	colorTable = attributes->colorTable;
	ancolors = attributes->ncolors;
	apixels = attributes->pixels;
	mask_pixel = attributes->mask_pixel;
    }
/* 3.2 backward compatibility code */
    else if (attributes && attributes->valuemask & XpmInfos) {
	oldColorTable = (XpmColor **) attributes->colorTable;
	ancolors = attributes->ncolors;
	apixels = attributes->pixels;
	mask_pixel = attributes->mask_pixel;
    }
/* end 3.2 bc */

    for (i = 0, color = colors, xcolor = xcolors; i < ncolors;
						  i++, color++, xcolor++) {

	/* look for related info from the attributes if any */
	found = False;
	if (ancolors) {
	    unsigned int offset = 0;

	    for (j = 0; j < ancolors; j++) {
		if (j == mask_pixel) {
		    offset = 1;
		    continue;
		}
		if (apixels[j - offset] == xcolor->pixel)
		    break;
	    }
	    if (j != ancolors) {
		unsigned int key;
		char **defaults = (char **) color;
		char **adefaults;

/* 3.2 backward compatibility code */
		if (oldColorTable)
		    adefaults = (char **) oldColorTable[j];
		else
/* end 3.2 bc */
		    adefaults = (char **) (colorTable + j);

		found = True;
		for (key = 1; key <= NKEYS; key++) {
		    if (s = adefaults[key])
			defaults[key] = (char *) xpmstrdup(s);
		}
	    }
	}
	if (!found) {
	    /* if nothing found look for a color name */
	    colorname = NULL;
	    if (rgbn_max)
		colorname = xpmGetRgbName(rgbn, rgbn_max, xcolor->red,
					  xcolor->green, xcolor->blue);
	    if (colorname)
		color->c_color = (char *) xpmstrdup(colorname);
	    else {
		/* at last store the rgb value */
		char buf[BUFSIZ];
#ifndef FOR_MSW
		sprintf(buf, "#%04X%04X%04X",
			xcolor->red, xcolor->green, xcolor->blue);
#else
		sprintf(buf, "#%02x%02x%02x",
			xcolor->red, xcolor->green, xcolor->blue);
#endif			
		color->c_color = (char *) xpmstrdup(buf);
	    }
	    if (!color->c_color) {
		XpmFree(xcolors);
		xpmFreeRgbNames(rgbn, rgbn_max);
		return (XpmNoMemory);
	    }
	}
    }

    XpmFree(xcolors);
    xpmFreeRgbNames(rgbn, rgbn_max);
    return (XpmSuccess);
}

#ifndef FOR_MSW
# ifndef AMIGA
/*
 * The functions below are written from X11R5 MIT's code (XImUtil.c)
 *
 * The idea is to have faster functions than the standard XGetPixel function
 * to scan the image data. Indeed we can speed up things by suppressing tests
 * performed for each pixel. We do exactly the same tests but at the image
 * level.
 */

static unsigned long Const low_bits_table[] = {
    0x00000000, 0x00000001, 0x00000003, 0x00000007,
    0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
    0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
    0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
    0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
    0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
    0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
    0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
    0xffffffff
};

/*
 * Default method to scan pixels of an image data structure.
 * The algorithm used is:
 *
 *	copy the source bitmap_unit or Zpixel into temp
 *	normalize temp if needed
 *	extract the pixel bits into return value
 *
 */

static int
GetImagePixels(image, width, height, pmap)
    XImage *image;
    unsigned int width;
    unsigned int height;
    PixelsMap *pmap;
{
    char *src;
    char *dst;
    unsigned int *iptr;
    char *data;
    int x, y, i;
    int bits, depth, ibu, ibpp, offset;
    unsigned long lbt;
    Pixel pixel, px;

    data = image->data;
    iptr = pmap->pixelindex;
    depth = image->depth;
    lbt = low_bits_table[depth];
    ibpp = image->bits_per_pixel;
    offset = image->xoffset;

    if ((image->bits_per_pixel | image->depth) == 1) {
	ibu = image->bitmap_unit;
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		src = &data[XYINDEX(x, y, image)];
		dst = (char *) &pixel;
		pixel = 0;
		for (i = ibu >> 3; --i >= 0;)
		    *dst++ = *src++;
		XYNORMALIZE(&pixel, image);
		bits = (x + offset) % ibu;
		pixel = ((((char *) &pixel)[bits >> 3]) >> (bits & 7)) & 1;
		if (ibpp != depth)
		    pixel &= lbt;
		if (storePixel(pixel, pmap, iptr))
		    return (XpmNoMemory);
	    }
    } else if (image->format == XYPixmap) {
	int nbytes, bpl, j;
	long plane = 0;
	ibu = image->bitmap_unit;
	nbytes = ibu >> 3;
	bpl = image->bytes_per_line;
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		pixel = 0;
		plane = 0;
		for (i = depth; --i >= 0;) {
		    src = &data[XYINDEX(x, y, image) + plane];
		    dst = (char *) &px;
		    px = 0;
		    for (j = nbytes; --j >= 0;)
			*dst++ = *src++;
		    XYNORMALIZE(&px, image);
		    bits = (x + offset) % ibu;
		    pixel = (pixel << 1) |
			    (((((char *) &px)[bits >> 3]) >> (bits & 7)) & 1);
		    plane = plane + (bpl * height);
		}
		if (ibpp != depth)
		    pixel &= lbt;
		if (storePixel(pixel, pmap, iptr))
		    return (XpmNoMemory);
	    }
    } else if (image->format == ZPixmap) {
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		src = &data[ZINDEX(x, y, image)];
		dst = (char *) &px;
		px = 0;
		for (i = (ibpp + 7) >> 3; --i >= 0;)
		    *dst++ = *src++;
		ZNORMALIZE(&px, image);
		pixel = 0;
		for (i = sizeof(unsigned long); --i >= 0;)
		    pixel = (pixel << 8) | ((unsigned char *) &px)[i];
		if (ibpp == 4) {
		    if (x & 1)
			pixel >>= 4;
		    else
			pixel &= 0xf;
		}
		if (ibpp != depth)
		    pixel &= lbt;
		if (storePixel(pixel, pmap, iptr))
		    return (XpmNoMemory);
	    }
    } else
	return (XpmColorError); /* actually a bad image */
    return (XpmSuccess);
}

/*
 * scan pixels of a 32-bits Z image data structure
 */

#if !defined(WORD64) && !defined(LONG64)
static unsigned long byteorderpixel = MSBFirst << 24;
#endif

static int
GetImagePixels32(image, width, height, pmap)
    XImage *image;
    unsigned int width;
    unsigned int height;
    PixelsMap *pmap;
{
    unsigned char *addr;
    unsigned char *data;
    unsigned int *iptr;
    int x, y;
    unsigned long lbt;
    Pixel pixel;
    int depth;

    data = (unsigned char *) image->data;
    iptr = pmap->pixelindex;
    depth = image->depth;
    lbt = low_bits_table[depth];
#if !defined(WORD64) && !defined(LONG64)
    if (*((char *) &byteorderpixel) == image->byte_order) {
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &data[ZINDEX32(x, y, image)];
		pixel = *((unsigned long *) addr);
		if (depth != 32)
		    pixel &= lbt;
		if (storePixel(pixel, pmap, iptr))
		    return (XpmNoMemory);
	    }
    } else
#endif
    if (image->byte_order == MSBFirst)
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &data[ZINDEX32(x, y, image)];
		pixel = ((unsigned long) addr[0] << 24 |
			 (unsigned long) addr[1] << 16 |
			 (unsigned long) addr[2] << 8 |
			 addr[3]);
		if (depth != 32)
		    pixel &= lbt;
		if (storePixel(pixel, pmap, iptr))
		    return (XpmNoMemory);
	    }
    else
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &data[ZINDEX32(x, y, image)];
		pixel = (addr[0] |
			 (unsigned long) addr[1] << 8 |
			 (unsigned long) addr[2] << 16 |
			 (unsigned long) addr[3] << 24);
		if (depth != 32)
		    pixel &= lbt;
		if (storePixel(pixel, pmap, iptr))
		    return (XpmNoMemory);
	    }
    return (XpmSuccess);
}

/*
 * scan pixels of a 16-bits Z image data structure
 */

static int
GetImagePixels16(image, width, height, pmap)
    XImage *image;
    unsigned int width;
    unsigned int height;
    PixelsMap *pmap;
{
    unsigned char *addr;
    unsigned char *data;
    unsigned int *iptr;
    int x, y;
    unsigned long lbt;
    Pixel pixel;
    int depth;

    data = (unsigned char *) image->data;
    iptr = pmap->pixelindex;
    depth = image->depth;
    lbt = low_bits_table[depth];
    if (image->byte_order == MSBFirst)
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &data[ZINDEX16(x, y, image)];
		pixel = addr[0] << 8 | addr[1];
		if (depth != 16)
		    pixel &= lbt;
		if (storePixel(pixel, pmap, iptr))
		    return (XpmNoMemory);
	    }
    else
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &data[ZINDEX16(x, y, image)];
		pixel = addr[0] | addr[1] << 8;
		if (depth != 16)
		    pixel &= lbt;
		if (storePixel(pixel, pmap, iptr))
		    return (XpmNoMemory);
	    }
    return (XpmSuccess);
}

/*
 * scan pixels of a 8-bits Z image data structure
 */

static int
GetImagePixels8(image, width, height, pmap)
    XImage *image;
    unsigned int width;
    unsigned int height;
    PixelsMap *pmap;
{
    unsigned int *iptr;
    unsigned char *data;
    int x, y;
    unsigned long lbt;
    Pixel pixel;
    int depth;

    data = (unsigned char *) image->data;
    iptr = pmap->pixelindex;
    depth = image->depth;
    lbt = low_bits_table[depth];
    for (y = 0; y < height; y++)
	for (x = 0; x < width; x++, iptr++) {
	    pixel = data[ZINDEX8(x, y, image)];
	    if (depth != 8)
		pixel &= lbt;
	    if (storePixel(pixel, pmap, iptr))
		return (XpmNoMemory);
	}
    return (XpmSuccess);
}

/*
 * scan pixels of a 1-bit depth Z image data structure
 */

static int
GetImagePixels1(image, width, height, pmap, storeFunc)
    XImage *image;
    unsigned int width;
    unsigned int height;
    PixelsMap *pmap;
    int (*storeFunc) ();
{
    unsigned int *iptr;
    int x, y;
    char *data;
    Pixel pixel;
    int xoff, yoff, offset, bpl;

    data = image->data;
    iptr = pmap->pixelindex;
    offset = image->xoffset;
    bpl = image->bytes_per_line;

    if (image->bitmap_bit_order == MSBFirst)
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		xoff = x + offset;
		yoff = y * bpl + (xoff >> 3);
		xoff &= 7;
		pixel = (data[yoff] & (0x80 >> xoff)) ? 1 : 0;
		if ((*storeFunc) (pixel, pmap, iptr))
		    return (XpmNoMemory);
	    }
    else
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		xoff = x + offset;
		yoff = y * bpl + (xoff >> 3);
		xoff &= 7;
		pixel = (data[yoff] & (1 << xoff)) ? 1 : 0;
		if ((*storeFunc) (pixel, pmap, iptr))
		    return (XpmNoMemory);
	    }
    return (XpmSuccess);
}

# else /* AMIGA */

#define CLEAN_UP(status) \
{\
    if (pixels) XpmFree (pixels);\
    if (tmp_img) FreeXImage (tmp_img);\
    return (status);\
}

static int
AGetImagePixels (
    XImage        *image,
    unsigned int   width,
    unsigned int   height,
    PixelsMap     *pmap,
    int          (*storeFunc) ())
{
    unsigned int   *iptr;
    unsigned int    x, y;
    unsigned char  *pixels;
    XImage         *tmp_img;

    pixels = XpmMalloc ((((width+15)>>4)<<4)*sizeof (*pixels));
    if (pixels == NULL)
	return XpmNoMemory;

    tmp_img = AllocXImage ((((width+15)>>4)<<4), 1, image->rp->BitMap->Depth);
    if (tmp_img == NULL)
	CLEAN_UP (XpmNoMemory)

    iptr = pmap->pixelindex;
    for (y = 0; y < height; ++y)
    {
	ReadPixelLine8 (image->rp, 0, y, width, pixels, tmp_img->rp);
	for (x = 0; x < width; ++x, ++iptr)
	{
	    if ((*storeFunc) (pixels[x], pmap, iptr))
		CLEAN_UP (XpmNoMemory)
	}
    }

    CLEAN_UP (XpmSuccess)
}

#undef CLEAN_UP

# endif/* AMIGA */
#else  /* ndef FOR_MSW */

#ifdef __OS2__

#ifdef __VISAGECPP30__
static int MSWGetImagePixels(
  Display*     display
, XImage*      image
, unsigned int width
, unsigned int height
, PixelsMap*   pmap
, int          (*storeFunc) (Pixel, PixelsMap*, unsigned int*)
)
#else
static int MSWGetImagePixels(
  Display*     display
, XImage*      image
, unsigned int width
, unsigned int height
, PixelsMap*   pmap
, int          (*storeFunc) ()
)
#endif

#else
static int
MSWGetImagePixels(display, image, width, height, pmap, storeFunc)
    Display *display;
    XImage *image;
    unsigned int width;
    unsigned int height;
    PixelsMap *pmap;
    int (*storeFunc) ();
#endif
{
    unsigned int *iptr;
    unsigned int x, y;
    Pixel pixel;
#ifdef __OS2__
     HAB          hab;
     HPS          hps;
     DEVOPENSTRUC dop = {NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL};
     SIZEL        sizl = {0, 0};
     POINTL       point;
#endif

    iptr = pmap->pixelindex;

#ifdef __OS2__
    hps = GpiCreatePS(hab, *display, &sizl, GPIA_ASSOC | PU_PELS);
    GpiSetBitmap(hps, image->bitmap);
#else
    SelectObject(*display, image->bitmap);
#endif

    for (y = 0; y < height; y++) {
	for (x = 0; x < width; x++, iptr++) {
#ifdef __OS2__
     point.x = x;
     point.y = y;
     pixel = GpiQueryPel(hps, &point);
#else
	    pixel = GetPixel(*display, x, y);
#endif

	    if ((*storeFunc) (pixel, pmap, iptr))
		return (XpmNoMemory);
	}
    }
    return (XpmSuccess);
}

#endif

#ifndef FOR_MSW
# ifndef AMIGA
int
XpmCreateXpmImageFromPixmap(display, pixmap, shapemask,
			    xpmimage, attributes)
    Display *display;
    Pixmap pixmap;
    Pixmap shapemask;
    XpmImage *xpmimage;
    XpmAttributes *attributes;
{
    XImage *ximage = NULL;
    XImage *shapeimage = NULL;
    unsigned int width = 0;
    unsigned int height = 0;
    int ErrorStatus;

    /* get geometry */
    if (attributes && attributes->valuemask & XpmSize) {
	width = attributes->width;
	height = attributes->height;
    }
    /* get the ximages */
    if (pixmap)
	xpmCreateImageFromPixmap(display, pixmap, &ximage, &width, &height);
    if (shapemask)
	xpmCreateImageFromPixmap(display, shapemask, &shapeimage,
				 &width, &height);

    /* create the related XpmImage */
    ErrorStatus = XpmCreateXpmImageFromImage(display, ximage, shapeimage,
					     xpmimage, attributes);

    /* destroy the ximages */
    if (ximage)
	XDestroyImage(ximage);
    if (shapeimage)
	XDestroyImage(shapeimage);

    return (ErrorStatus);
}

# endif/* not AMIGA */
#endif /* ndef FOR_MSW */
