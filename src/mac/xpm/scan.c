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

LFUNC(PlatformGetImagePixels, int, (Display *d, XImage *image, unsigned int width,
			       unsigned int height, PixelsMap *pmap,
			       int (*storeFunc) ()));
LFUNC(ScanTransparentColor, int, (XpmColor *color, unsigned int cpp,
				  XpmAttributes *attributes));

LFUNC(ScanOtherColors, int, (Display *display, XpmColor *colors, int ncolors,
			     Pixel *pixels, unsigned int mask,
			     unsigned int cpp, XpmAttributes *attributes));

/*
 * This function stores the given pixel in the given arrays which are grown
 * if not large enough.
 */
static int
storePixel(pixel, pmap, index_return)
    Pixel pixel;
    PixelsMap *pmap;
    unsigned int *index_return;
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
	if ( IS_EQUAL_PIXEL(*p , pixel) )
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

static int
storeMaskPixel(pixel, pmap, index_return)
    Pixel pixel;
    PixelsMap *pmap;
    unsigned int *index_return;
{
    if (IS_ZERO_PIXEL(pixel)) {
	if (!pmap->ncolors) {
	    pmap->ncolors = 1;
	    SET_ZERO_PIXEL((pmap->pixels)[0]);
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
int
XpmCreateXpmImageFromImage(display, image, shapeimage,
			   xpmimage, attributes)
    Display *display;
    XImage *image;
    XImage *shapeimage;
    XpmImage *xpmimage;
    XpmAttributes *attributes;
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
    if (shapeimage) {

	ErrorStatus = PlatformGetImagePixels(display, shapeimage, width, height,
					&pmap, storeMaskPixel);

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

    if (image) {

	ErrorStatus = PlatformGetImagePixels(display, image, width, height, &pmap,
					storePixel);

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

static int
ScanTransparentColor(color, cpp, attributes)
    XpmColor *color;
    unsigned int cpp;
    XpmAttributes *attributes;
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

static int
ScanOtherColors(display, colors, ncolors, pixels, mask, cpp, attributes)
    Display *display;
    XpmColor *colors;
    int ncolors;
    Pixel *pixels;
    unsigned int mask;
    unsigned int cpp;
    XpmAttributes *attributes;
{
    /* variables stored in the XpmAttributes structure */
    Colormap colormap;
    char *rgb_fname;

    xpmRgbName *rgbn = NULL; 

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
    XQueryColors(display, colormap, xcolors, ncolors);

    rgbn_max = xpmReadRgbNames(NULL, NULL);

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
		if (IS_EQUAL_PIXEL( apixels[j - offset] , xcolor->pixel) )
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
//		sprintf(buf, "#%04X%04X%04X",
		sprintf(buf, "#%02x%02x%02x",
			xcolor->red, xcolor->green, xcolor->blue);

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


static int
PlatformGetImagePixels(display, image, width, height, pmap, storeFunc)
    Display *display;
    XImage *image;
    unsigned int width;
    unsigned int height;
    PixelsMap *pmap;
    int (*storeFunc) ();
{
	#if FOR_MSW
    unsigned int *iptr;
    unsigned int x, y;
    Pixel pixel;

    iptr = pmap->pixelindex;

    SelectObject(*display, image->bitmap);
    for (y = 0; y < height; y++) {
	for (x = 0; x < width; x++, iptr++) {
	    pixel = GetPixel(*display, x, y);
	    if ((*storeFunc) (pixel, pmap, iptr))
		return (XpmNoMemory);
	}
    }
    #elif macintosh
    #endif
    return (XpmSuccess);
}
