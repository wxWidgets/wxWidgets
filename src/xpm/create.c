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
* create.c:                                                                   *
*                                                                             *
*  XPM library                                                                *
*  Create an X image and possibly its related shape mask                      *
*  from the given XpmImage.                                                   *
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
#include <ctype.h>

LFUNC(xpmVisualType, int, (Visual *visual));

LFUNC(AllocColor, int, (Display *display, Colormap colormap,
			char *colorname, XColor *xcolor, void *closure));
LFUNC(FreeColors, int, (Display *display, Colormap colormap,
			Pixel *pixels, int n, void *closure));

#ifndef FOR_MSW
LFUNC(SetCloseColor, int, (Display *display, Colormap colormap,
			   Visual *visual, XColor *col,
			   Pixel *image_pixel, Pixel *mask_pixel,
			   Pixel *alloc_pixels, unsigned int *nalloc_pixels,
			   XpmAttributes *attributes, XColor *cols, int ncols,
			   XpmAllocColorFunc allocColor, void *closure));
#else
/* let the window system take care of close colors */
#endif

LFUNC(SetColor, int, (Display *display, Colormap colormap, Visual *visual,
		      char *colorname, unsigned int color_index,
		      Pixel *image_pixel, Pixel *mask_pixel,
		      unsigned int *mask_pixel_index,
		      Pixel *alloc_pixels, unsigned int *nalloc_pixels,
		      Pixel *used_pixels, unsigned int *nused_pixels,
		      XpmAttributes *attributes, XColor *cols, int ncols,
		      XpmAllocColorFunc allocColor, void *closure));

LFUNC(CreateXImage, int, (Display *display, Visual *visual,
			  unsigned int depth, int format, unsigned int width,
                          unsigned int height, XImage **image_return));

LFUNC(CreateColors, int, (Display *display, XpmAttributes *attributes,
                          XpmColor *colors, unsigned int ncolors,
                          Pixel *image_pixels, Pixel *mask_pixels,
                          unsigned int *mask_pixel_index,
                          Pixel *alloc_pixels, unsigned int *nalloc_pixels,
                          Pixel *used_pixels, unsigned int *nused_pixels));

#ifndef FOR_MSW
LFUNC(ParseAndPutPixels, int, (xpmData *data, unsigned int width,
			       unsigned int height, unsigned int ncolors,
			       unsigned int cpp, XpmColor *colorTable,
			       xpmHashTable *hashtable,
			       XImage *image, Pixel *image_pixels,
			       XImage *mask, Pixel *mask_pixels));
#else  /* FOR_MSW */
LFUNC(ParseAndPutPixels, int, (Display *dc, xpmData *data, unsigned int width,
			       unsigned int height, unsigned int ncolors,
			       unsigned int cpp, XpmColor *colorTable,
			       xpmHashTable *hashtable,
			       XImage *image, Pixel *image_pixels,
			       XImage *mask, Pixel *mask_pixels));
#endif

#ifndef FOR_MSW
# ifndef AMIGA
/* XImage pixel routines */
LFUNC(PutImagePixels, void, (XImage *image, unsigned int width,
			     unsigned int height, unsigned int *pixelindex,
			     Pixel *pixels));

LFUNC(PutImagePixels32, void, (XImage *image, unsigned int width,
			       unsigned int height, unsigned int *pixelindex,
			       Pixel *pixels));

LFUNC(PutImagePixels16, void, (XImage *image, unsigned int width,
			       unsigned int height, unsigned int *pixelindex,
			       Pixel *pixels));

LFUNC(PutImagePixels8, void, (XImage *image, unsigned int width,
			      unsigned int height, unsigned int *pixelindex,
			      Pixel *pixels));

LFUNC(PutImagePixels1, void, (XImage *image, unsigned int width,
			      unsigned int height, unsigned int *pixelindex,
			      Pixel *pixels));

LFUNC(PutPixel1, int, (XImage *ximage, int x, int y, unsigned long pixel));
LFUNC(PutPixel, int, (XImage *ximage, int x, int y, unsigned long pixel));
LFUNC(PutPixel32, int, (XImage *ximage, int x, int y, unsigned long pixel));
LFUNC(PutPixel32MSB, int, (XImage *ximage, int x, int y, unsigned long pixel));
LFUNC(PutPixel32LSB, int, (XImage *ximage, int x, int y, unsigned long pixel));
LFUNC(PutPixel16MSB, int, (XImage *ximage, int x, int y, unsigned long pixel));
LFUNC(PutPixel16LSB, int, (XImage *ximage, int x, int y, unsigned long pixel));
LFUNC(PutPixel8, int, (XImage *ximage, int x, int y, unsigned long pixel));
LFUNC(PutPixel1MSB, int, (XImage *ximage, int x, int y, unsigned long pixel));
LFUNC(PutPixel1LSB, int, (XImage *ximage, int x, int y, unsigned long pixel));

# else /* AMIGA */
LFUNC(APutImagePixels, void, (XImage *ximage, unsigned int width,
			      unsigned int height, unsigned int *pixelindex,
			      Pixel *pixels));
# endif/* AMIGA */
#else  /* FOR_MSW */
/* FOR_MSW pixel routine */
#ifdef __OS2__
LFUNC(MSWPutImagePixels, void, (
  HPS           hps
, Display*      dc
, XImage*       image
, unsigned int  width
, unsigned int  height
, unsigned int* pixelindex
, Pixel*        pixels
));
#else
LFUNC(MSWPutImagePixels, void, (Display *dc, XImage *image,
				unsigned int width, unsigned int height,
				unsigned int *pixelindex, Pixel *pixels));
#endif
#endif /* FOR_MSW */

#ifdef NEED_STRCASECMP
FUNC(xpmstrcasecmp, int, (char *s1, char *s2));

/*
 * in case strcasecmp is not provided by the system here is one
 * which does the trick
 */
#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
int xpmstrcasecmp(register char* s1, register char* s2)
#else
int
xpmstrcasecmp(s1, s2)
    register char *s1, *s2;
#endif
{
    register int c1, c2;

    while (*s1 && *s2) {
	c1 = tolower(*s1);
	c2 = tolower(*s2);
	if (c1 != c2)
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}

#endif

/*
 * return the default color key related to the given visual
 */
#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int xpmVisualType(Visual* visual)
#else
static int
xpmVisualType(visual)
    Visual *visual;
#endif
{
#ifndef FOR_MSW
# ifndef AMIGA
    switch (visual->class) {
    case StaticGray:
    case GrayScale:
	switch (visual->map_entries) {
	case 2:
	    return (XPM_MONO);
	case 4:
	    return (XPM_GRAY4);
	default:
	    return (XPM_GRAY);
	}
    default:
	return (XPM_COLOR);
    }
# else
    /* set the key explicitly in the XpmAttributes to override this */
    return (XPM_COLOR);
# endif
#else
    /* there should be a similar switch for MSW */
    return (XPM_COLOR);
#endif
}


typedef struct {
    int cols_index;
    long closeness;
}      CloseColor;

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int closeness_cmp(Const void* a, Const void* b)
#else
static int
closeness_cmp(a, b)
    Const void *a, *b;
#endif
{
    CloseColor *x = (CloseColor *) a, *y = (CloseColor *) b;

    /* cast to int as qsort requires */
    return (int) (x->closeness - y->closeness);
}


/* default AllocColor function:
 *   call XParseColor if colorname is given, return negative value if failure
 *   call XAllocColor and return 0 if failure, positive otherwise
 */
#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int
AllocColor(
  Display*      display
, Colormap      colormap
, char*         colorname
, XColor*       xcolor
, void*         closure
)
#else
static int
AllocColor(display, colormap, colorname, xcolor, closure)
    Display *display;
    Colormap* colormap;
    char *colorname;
    XColor *xcolor;
    void *closure;		/* not used */
#endif
{
    int status;
    if (colorname)
#ifdef __OS2__
	if (!XParseColor(display, &colormap, colorname, xcolor))
	    return -1;
    status = XAllocColor(display, &colormap, xcolor);
#else
	if (!XParseColor(display, colormap, colorname, xcolor))
	    return -1;
    status = XAllocColor(display, colormap, xcolor);
#endif
    return status != 0 ? 1 : 0;
}


#ifndef FOR_MSW
/*
 * set a close color in case the exact one can't be set
 * return 0 if success, 1 otherwise.
 */

static int
SetCloseColor(display, colormap, visual, col, image_pixel, mask_pixel,
	      alloc_pixels, nalloc_pixels, attributes, cols, ncols,
	      allocColor, closure)
    Display *display;
    Colormap colormap;
    Visual *visual;
    XColor *col;
    Pixel *image_pixel, *mask_pixel;
    Pixel *alloc_pixels;
    unsigned int *nalloc_pixels;
    XpmAttributes *attributes;
    XColor *cols;
    int ncols;
    XpmAllocColorFunc allocColor;
    void *closure;
{

    /*
     * Allocation failed, so try close colors. To get here the visual must
     * be GreyScale, PseudoColor or DirectColor (or perhaps StaticColor?
     * What about sharing systems like QDSS?). Beware: we have to treat
     * DirectColor differently.
     */


    long int red_closeness, green_closeness, blue_closeness;
    int n;
    Bool alloc_color;

    if (attributes && (attributes->valuemask & XpmCloseness))
	red_closeness = green_closeness = blue_closeness =
	    attributes->closeness;
    else {
	red_closeness = attributes->red_closeness;
	green_closeness = attributes->green_closeness;
	blue_closeness = attributes->blue_closeness;
    }
    if (attributes && (attributes->valuemask & XpmAllocCloseColors))
	alloc_color = attributes->alloc_close_colors;
    else
	alloc_color = True;

    /*
     * We sort the colormap by closeness and try to allocate the color
     * closest to the target. If the allocation of this close color fails,
     * which almost never happens, then one of two scenarios is possible.
     * Either the colormap must have changed (since the last close color
     * allocation or possibly while we were sorting the colormap), or the
     * color is allocated as Read/Write by some other client. (Note: X
     * _should_ allow clients to check if a particular color is Read/Write,
     * but it doesn't! :-( ). We cannot determine which of these scenarios
     * occurred, so we try the next closest color, and so on, until no more
     * colors are within closeness of the target. If we knew that the
     * colormap had changed, we could skip this sequence.
     *
     * If _none_ of the colors within closeness of the target can be allocated,
     * then we can finally be pretty sure that the colormap has actually
     * changed. In this case we try to allocate the original color (again),
     * then try the closecolor stuff (again)...
     *
     * In theory it would be possible for an infinite loop to occur if another
     * process kept changing the colormap every time we sorted it, so we set
     * a maximum on the number of iterations. After this many tries, we use
     * XGrabServer() to ensure that the colormap remains unchanged.
     *
     * This approach gives particularly bad worst case performance - as many as
     * <MaximumIterations> colormap reads and sorts may be needed, and as
     * many as <MaximumIterations> * <ColormapSize> attempted allocations
     * may fail. On an 8-bit system, this means as many as 3 colormap reads,
     * 3 sorts and 768 failed allocations per execution of this code!
     * Luckily, my experiments show that in general use in a typical 8-bit
     * color environment only about 1 in every 10000 allocations fails to
     * succeed in the fastest possible time. So virtually every time what
     * actually happens is a single sort followed by a successful allocate.
     * The very first allocation also costs a colormap read, but no further
     * reads are usually necessary.
     */

#define ITERATIONS 2			/* more than one is almost never
					 * necessary */

    for (n = 0; n <= ITERATIONS; ++n) {
	CloseColor *closenesses =
	    (CloseColor *) XpmCalloc(ncols, sizeof(CloseColor));
	int i, c;

	for (i = 0; i < ncols; ++i) {	/* build & sort closenesses table */
#define COLOR_FACTOR       3
#define BRIGHTNESS_FACTOR  1

	    closenesses[i].cols_index = i;
	    closenesses[i].closeness =
		COLOR_FACTOR * (abs((long) col->red - (long) cols[i].red)
				+ abs((long) col->green - (long) cols[i].green)
				+ abs((long) col->blue - (long) cols[i].blue))
		+ BRIGHTNESS_FACTOR * abs(((long) col->red +
					   (long) col->green +
					   (long) col->blue)
					   - ((long) cols[i].red +
					      (long) cols[i].green +
					      (long) cols[i].blue));
	}
	qsort(closenesses, ncols, sizeof(CloseColor), closeness_cmp);

	i = 0;
	c = closenesses[i].cols_index;
	while ((long) cols[c].red >= (long) col->red - red_closeness &&
	       (long) cols[c].red <= (long) col->red + red_closeness &&
	       (long) cols[c].green >= (long) col->green - green_closeness &&
	       (long) cols[c].green <= (long) col->green + green_closeness &&
	       (long) cols[c].blue >= (long) col->blue - blue_closeness &&
	       (long) cols[c].blue <= (long) col->blue + blue_closeness) {
	    if (alloc_color) {
		if ((*allocColor)(display, colormap, NULL, &cols[c], closure)){
		    if (n == ITERATIONS)
			XUngrabServer(display);
		    XpmFree(closenesses);
		    *image_pixel = cols[c].pixel;
		    *mask_pixel = 1;
		    alloc_pixels[(*nalloc_pixels)++] = cols[c].pixel;
		    return (0);
		} else {
		    ++i;
		    if (i == ncols)
			break;
		    c = closenesses[i].cols_index;
		}
	    } else {
		if (n == ITERATIONS)
		    XUngrabServer(display);
		XpmFree(closenesses);
		*image_pixel = cols[c].pixel;
		*mask_pixel = 1;
		return (0);
	    }
	}

	/* Couldn't allocate _any_ of the close colors! */

	if (n == ITERATIONS)
	    XUngrabServer(display);
	XpmFree(closenesses);

	if (i == 0 || i == ncols)	/* no color close enough or cannot */
	    return (1);			/* alloc any color (full of r/w's) */

	if ((*allocColor)(display, colormap, NULL, col, closure)) {
	    *image_pixel = col->pixel;
	    *mask_pixel = 1;
	    alloc_pixels[(*nalloc_pixels)++] = col->pixel;
	    return (0);
	} else {			/* colormap has probably changed, so
					 * re-read... */
	    if (n == ITERATIONS - 1)
		XGrabServer(display);

#if 0
	    if (visual->class == DirectColor) {
		/* TODO */
	    } else
#endif
		XQueryColors(display, colormap, cols, ncols);
	}
    }
    return (1);
}

#define USE_CLOSECOLOR attributes && \
(((attributes->valuemask & XpmCloseness) && attributes->closeness != 0) \
 || ((attributes->valuemask & XpmRGBCloseness) && \
     (attributes->red_closeness != 0 \
      || attributes->green_closeness != 0 \
      || attributes->blue_closeness != 0)))

#else
    /* FOR_MSW part */
    /* nothing to do here, the window system does it */
#endif

/*
 * set the color pixel related to the given colorname,
 * return 0 if success, 1 otherwise.
 */

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int
SetColor(
  Display*          display
, Colormap          colormap
, Visual*           visual
, char*             colorname
, unsigned int      color_index
, Pixel*            image_pixel
, Pixel*            mask_pixel
, unsigned int*     mask_pixel_index
, Pixel*            alloc_pixels
, unsigned int*     nalloc_pixels
, Pixel*            used_pixels
, unsigned int*     nused_pixels
, XpmAttributes*    attributes
, XColor*           cols
, int               ncols
, XpmAllocColorFunc allocColor
, void*             closure
)
#else
static int
SetColor(display, colormap, visual, colorname, color_index,
	 image_pixel, mask_pixel, mask_pixel_index,
	 alloc_pixels, nalloc_pixels, used_pixels, nused_pixels,
	 attributes, cols, ncols, allocColor, closure)
    Display *display;
    Colormap colormap;
    Visual *visual;
    char *colorname;
    unsigned int color_index;
    Pixel *image_pixel, *mask_pixel;
    unsigned int *mask_pixel_index;
    Pixel *alloc_pixels;
    unsigned int *nalloc_pixels;
    Pixel *used_pixels;
    unsigned int *nused_pixels;
    XpmAttributes *attributes;
    XColor *cols;
    int ncols;
    XpmAllocColorFunc allocColor;
    void *closure;
#endif
{
    XColor xcolor;
    int status;

    if (xpmstrcasecmp(colorname, TRANSPARENT_COLOR)) {
	status = (*allocColor)(display, colormap, colorname, &xcolor, closure);
	if (status < 0)		/* parse color failed */
	    return (1);

	if (status == 0) {
#ifndef FOR_MSW
	    if (USE_CLOSECOLOR)
		return (SetCloseColor(display, colormap, visual, &xcolor,
				      image_pixel, mask_pixel,
				      alloc_pixels, nalloc_pixels,
				      attributes, cols, ncols,
				      allocColor, closure));
	    else
#endif /* ndef FOR_MSW */
		return (1);
	} else
	    alloc_pixels[(*nalloc_pixels)++] = xcolor.pixel;
	*image_pixel = xcolor.pixel;
#ifndef FOR_MSW
	*mask_pixel = 1;
#else
#ifdef __OS2__
	*mask_pixel = OS2RGB(0,0,0);
#else
	*mask_pixel = RGB(0,0,0);
#endif
#endif
	used_pixels[(*nused_pixels)++] = xcolor.pixel;
    } else {
	*image_pixel = 0;
#ifndef FOR_MSW
	*mask_pixel = 0;
#else
#ifdef __OS2__
	*mask_pixel = OS2RGB(0,0,0);
#else
	*mask_pixel = RGB(0,0,0);
#endif
#endif
	/* store the color table index */
	*mask_pixel_index = color_index;
    }
    return (0);
}

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int
CreateColors(
  Display*          display
, XpmAttributes*    attributes
, XpmColor*         colors
, unsigned int      ncolors
, Pixel*            image_pixels
, Pixel*            mask_pixels
, unsigned int*     mask_pixel_index
, Pixel*            alloc_pixels
, unsigned int*     nalloc_pixels
, Pixel*            used_pixels
, unsigned int*     nused_pixels
)
#else
static int
CreateColors(display, attributes, colors, ncolors, image_pixels, mask_pixels,
             mask_pixel_index, alloc_pixels, nalloc_pixels,
             used_pixels, nused_pixels)
    Display *display;
    XpmAttributes *attributes;
    XpmColor *colors;
    unsigned int ncolors;
    Pixel *image_pixels;
    Pixel *mask_pixels;
    unsigned int *mask_pixel_index;
    Pixel *alloc_pixels;
    unsigned int *nalloc_pixels;
    Pixel *used_pixels;
    unsigned int *nused_pixels;
#endif
{
    /* variables stored in the XpmAttributes structure */
    Visual *visual;
    Colormap colormap;
    XpmColorSymbol *colorsymbols;
    unsigned int numsymbols;
    XpmAllocColorFunc allocColor;
    void *closure;

    char *colorname;
    unsigned int color, key;
    Bool pixel_defined;
    XpmColorSymbol *symbol;
    char **defaults;
    int ErrorStatus = XpmSuccess;
    char *s;
    int default_index;

    XColor *cols = NULL;
    unsigned int ncols = 0;

    /*
     * retrieve information from the XpmAttributes
     */
    if (attributes && attributes->valuemask & XpmColorSymbols) {
	colorsymbols = attributes->colorsymbols;
	numsymbols = attributes->numsymbols;
    } else
	numsymbols = 0;

    if (attributes && attributes->valuemask & XpmVisual)
	visual = attributes->visual;
    else
	visual = XDefaultVisual(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmColormap))
	colormap = attributes->colormap;
    else
	colormap = XDefaultColormap(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmColorKey))
	key = attributes->color_key;
    else
	key = xpmVisualType(visual);

    if (attributes && (attributes->valuemask & XpmAllocColor))
	allocColor = attributes->alloc_color;
    else
	allocColor = AllocColor;
    if (attributes && (attributes->valuemask & XpmColorClosure))
	closure = attributes->color_closure;
    else
	closure = NULL;

#ifndef FOR_MSW
    if (USE_CLOSECOLOR) {
	/* originally from SetCloseColor */
#if 0
	if (visual->class == DirectColor) {

	    /*
	     * TODO: Implement close colors for DirectColor visuals. This is
	     * difficult situation. Chances are that we will never get here,
	     * because any machine that supports DirectColor will probably
	     * also support TrueColor (and probably PseudoColor). Also,
	     * DirectColor colormaps can be very large, so looking for close
	     * colors may be too slow.
	     */
	} else {
#endif
	    int i;

#ifndef AMIGA
	    ncols = visual->map_entries;
#else
	    ncols = colormap->Count;
#endif
	    cols = (XColor *) XpmCalloc(ncols, sizeof(XColor));
	    for (i = 0; i < ncols; ++i)
		cols[i].pixel = i;
	    XQueryColors(display, colormap, cols, ncols);
#if 0
	}
#endif
    }
#endif /* ndef FOR_MSW */

    switch (key) {
    case XPM_MONO:
	default_index = 2;
	break;
    case XPM_GRAY4:
	default_index = 3;
	break;
    case XPM_GRAY:
	default_index = 4;
	break;
    case XPM_COLOR:
    default:
	default_index = 5;
	break;
    }

    for (color = 0; color < ncolors; color++, colors++,
					 image_pixels++, mask_pixels++) {
	colorname = NULL;
	pixel_defined = False;
	defaults = (char **) colors;

	/*
	 * look for a defined symbol
	 */
	if (numsymbols) {

	    unsigned int n;

	    s = defaults[1];
	    for (n = 0, symbol = colorsymbols; n < numsymbols; n++, symbol++) {
		if (symbol->name && s && !strcmp(symbol->name, s))
		    /* override name */
		    break;
		if (!symbol->name && symbol->value) {	/* override value */
		    int def_index = default_index;

		    while (defaults[def_index] == NULL)	/* find defined
							 * colorname */
			--def_index;
		    if (def_index < 2) {/* nothing towards mono, so try
					 * towards color */
			def_index = default_index + 1;
			while (def_index <= 5 && defaults[def_index] == NULL)
			    ++def_index;
		    }
		    if (def_index >= 2 && defaults[def_index] != NULL &&
			!xpmstrcasecmp(symbol->value, defaults[def_index]))
			break;
		}
	    }
	    if (n != numsymbols) {
		if (symbol->name && symbol->value)
		    colorname = symbol->value;
		else
		    pixel_defined = True;
	    }
	}
	if (!pixel_defined) {		/* pixel not given as symbol value */

	    unsigned int k;

	    if (colorname) {		/* colorname given as symbol value */
		if (!SetColor(display, colormap, visual, colorname, color,
			      image_pixels, mask_pixels, mask_pixel_index,
			      alloc_pixels, nalloc_pixels, used_pixels,
			      nused_pixels, attributes, cols, ncols,
			      allocColor, closure))
		    pixel_defined = True;
		else
		    ErrorStatus = XpmColorError;
	    }
	    k = key;
	    while (!pixel_defined && k > 1) {
		if (defaults[k]) {
		    if (!SetColor(display, colormap, visual, defaults[k],
				  color, image_pixels, mask_pixels,
				  mask_pixel_index, alloc_pixels,
				  nalloc_pixels, used_pixels, nused_pixels,
				  attributes, cols, ncols,
				  allocColor, closure)) {
			pixel_defined = True;
			break;
		    } else
			ErrorStatus = XpmColorError;
		}
		k--;
	    }
	    k = key + 1;
	    while (!pixel_defined && k < NKEYS + 1) {
		if (defaults[k]) {
		    if (!SetColor(display, colormap, visual, defaults[k],
				  color, image_pixels, mask_pixels,
				  mask_pixel_index, alloc_pixels,
				  nalloc_pixels, used_pixels, nused_pixels,
				  attributes, cols, ncols,
				  allocColor, closure)) {
			pixel_defined = True;
			break;
		    } else
			ErrorStatus = XpmColorError;
		}
		k++;
	    }
	    if (!pixel_defined) {
		if (cols)
		    XpmFree(cols);
		return (XpmColorFailed);
	    }
	} else {
	    /* simply use the given pixel */
	    *image_pixels = symbol->pixel;
	    /* the following makes the mask to be built even if none
	       is given a particular pixel */
	    if (symbol->value
		&& !xpmstrcasecmp(symbol->value, TRANSPARENT_COLOR)) {
		*mask_pixels = 0;
		*mask_pixel_index = color;
	    } else
		*mask_pixels = 1;
	    used_pixels[(*nused_pixels)++] = *image_pixels;
	}
    }
    if (cols)
	XpmFree(cols);
    return (ErrorStatus);
}


/* default FreeColors function, simply call XFreeColors */
#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int
FreeColors(
  Display*      display
, Colormap      colormap
, Pixel*        pixels
, int           n
, void*         closure
)
#else
static int
FreeColors(display, colormap, pixels, n, closure)
    Display *display;
    Colormap colormap;
    Pixel *pixels;
    int n;
    void *closure;		/* not used */
#endif
{
    return XFreeColors(display, colormap, pixels, n, 0);
}


/* function call in case of error */
#undef RETURN
#define RETURN(status) \
{ \
      ErrorStatus = status; \
      goto error; \
}

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
int XpmCreateImageFromXpmImage(
  Display*       display
, XpmImage*      image
, XImage**       image_return
, XImage**       shapeimage_return
, XpmAttributes* attributes
)
#else
int
XpmCreateImageFromXpmImage(display, image,
			   image_return, shapeimage_return, attributes)
    Display *display;
    XpmImage *image;
    XImage **image_return;
    XImage **shapeimage_return;
    XpmAttributes *attributes;
#endif
{
#ifdef __OS2__
     HAB          hab;
     HPS          hps;
     SIZEL        sizl = {0, 0};
#endif
    /* variables stored in the XpmAttributes structure */
    Visual *visual;
    Colormap colormap;
    unsigned int depth;
    int bitmap_format;
    XpmFreeColorsFunc freeColors;
    void *closure;

    /* variables to return */
    XImage *ximage = NULL;
    XImage *shapeimage = NULL;
    unsigned int mask_pixel_index = XpmUndefPixel;
    int ErrorStatus;

    /* calculation variables */
    Pixel *image_pixels = NULL;
    Pixel *mask_pixels = NULL;
    Pixel *alloc_pixels = NULL;
    Pixel *used_pixels = NULL;
    unsigned int nalloc_pixels = 0;
    unsigned int nused_pixels = 0;

    /* initialize return values */
    if (image_return)
	*image_return = NULL;
    if (shapeimage_return)
	*shapeimage_return = NULL;

    /* retrieve information from the XpmAttributes */
    if (attributes && (attributes->valuemask & XpmVisual))
	visual = attributes->visual;
    else
	visual = XDefaultVisual(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmColormap))
	colormap = attributes->colormap;
    else
	colormap = XDefaultColormap(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmDepth))
	depth = attributes->depth;
    else
	depth = XDefaultDepth(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmBitmapFormat))
	bitmap_format = attributes->bitmap_format;
    else
	bitmap_format = ZPixmap;

    if (attributes && (attributes->valuemask & XpmFreeColors))
	freeColors = attributes->free_colors;
    else
	freeColors = FreeColors;
    if (attributes && (attributes->valuemask & XpmColorClosure))
	closure = attributes->color_closure;
    else
	closure = NULL;

    ErrorStatus = XpmSuccess;

    /* malloc pixels index tables */
    image_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * image->ncolors);
    if (!image_pixels)
	return (XpmNoMemory);

    mask_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * image->ncolors);
    if (!mask_pixels)
	RETURN(XpmNoMemory);

    /* maximum of allocated pixels will be the number of colors */
    alloc_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * image->ncolors);
    if (!alloc_pixels)
	RETURN(XpmNoMemory);

    /* maximum of allocated pixels will be the number of colors */
    used_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * image->ncolors);
    if (!used_pixels)
	RETURN(XpmNoMemory);

    /* get pixel colors, store them in index tables */
    ErrorStatus = CreateColors(display, attributes, image->colorTable,
			       image->ncolors, image_pixels, mask_pixels,
			       &mask_pixel_index, alloc_pixels, &nalloc_pixels,
			       used_pixels, &nused_pixels);

    if (ErrorStatus != XpmSuccess
	&& (ErrorStatus < 0 || (attributes
				&& (attributes->valuemask & XpmExactColors)
				&& attributes->exactColors)))
	RETURN(ErrorStatus);

    /* create the ximage */
    if (image_return) {
	ErrorStatus = CreateXImage(display, visual, depth,
				   (depth == 1 ? bitmap_format : ZPixmap),
				   image->width, image->height, &ximage);
	if (ErrorStatus != XpmSuccess)
	    RETURN(ErrorStatus);

#ifndef FOR_MSW
# ifndef AMIGA

	/*
	 * set the ximage data using optimized functions for ZPixmap
	 */

	if (ximage->bits_per_pixel == 8)
	    PutImagePixels8(ximage, image->width, image->height,
			    image->data, image_pixels);
	else if (((ximage->bits_per_pixel | ximage->depth) == 1) &&
		 (ximage->byte_order == ximage->bitmap_bit_order))
	    PutImagePixels1(ximage, image->width, image->height,
			    image->data, image_pixels);
	else if (ximage->bits_per_pixel == 16)
	    PutImagePixels16(ximage, image->width, image->height,
			     image->data, image_pixels);
	else if (ximage->bits_per_pixel == 32)
	    PutImagePixels32(ximage, image->width, image->height,
			     image->data, image_pixels);
	else
	    PutImagePixels(ximage, image->width, image->height,
			   image->data, image_pixels);
# else /* AMIGA */
	APutImagePixels(ximage, image->width, image->height,
			image->data, image_pixels);
# endif
#else  /* FOR_MSW */
#ifdef __OS2__
 hps = GpiCreatePS(hab, *display, &sizl, GPIA_ASSOC|PU_PELS);
	MSWPutImagePixels(hps, display, ximage, image->width, image->height,
			  image->data, image_pixels);
#else
	MSWPutImagePixels(display, ximage, image->width, image->height,
			  image->data, image_pixels);
#endif
#endif
    }
    /* create the shape mask image */
    if (mask_pixel_index != XpmUndefPixel && shapeimage_return) {
	ErrorStatus = CreateXImage(display, visual, 1, bitmap_format,
				   image->width, image->height, &shapeimage);
	if (ErrorStatus != XpmSuccess)
	    RETURN(ErrorStatus);

#ifndef FOR_MSW
# ifndef AMIGA
	PutImagePixels1(shapeimage, image->width, image->height,
			image->data, mask_pixels);
# else /* AMIGA */
	APutImagePixels(shapeimage, image->width, image->height,
			image->data, mask_pixels);
# endif
#else  /* FOR_MSW */
#ifdef __OS2__
 hps = GpiCreatePS(hab, *display, &sizl, GPIA_ASSOC|PU_PELS);
	MSWPutImagePixels(hps, display, shapeimage, image->width, image->height,
			  image->data, mask_pixels);
#else
	MSWPutImagePixels(display, shapeimage, image->width, image->height,
			  image->data, mask_pixels);
#endif
#endif

    }
    XpmFree(image_pixels);
    XpmFree(mask_pixels);

    /* if requested return used pixels in the XpmAttributes structure */
    if (attributes && (attributes->valuemask & XpmReturnPixels ||
/* 3.2 backward compatibility code */
	attributes->valuemask & XpmReturnInfos)) {
/* end 3.2 bc */
	attributes->pixels = used_pixels;
	attributes->npixels = nused_pixels;
	attributes->mask_pixel = mask_pixel_index;
    } else
	XpmFree(used_pixels);

    /* if requested return alloc'ed pixels in the XpmAttributes structure */
    if (attributes && (attributes->valuemask & XpmReturnAllocPixels)) {
	attributes->alloc_pixels = alloc_pixels;
	attributes->nalloc_pixels = nalloc_pixels;
    } else
	XpmFree(alloc_pixels);

    /* return created images */
    if (image_return)
	*image_return = ximage;
    if (shapeimage_return)
	*shapeimage_return = shapeimage;

    return (ErrorStatus);

/* exit point in case of error, free only locally allocated variables */
error:
    if (ximage)
	XDestroyImage(ximage);
    if (shapeimage)
	XDestroyImage(shapeimage);
    if (image_pixels)
	XpmFree(image_pixels);
    if (mask_pixels)
	XpmFree(mask_pixels);
    if (nalloc_pixels)
	(*freeColors)(display, colormap, alloc_pixels, nalloc_pixels, NULL);
    if (alloc_pixels)
	XpmFree(alloc_pixels);
    if (used_pixels)
	XpmFree(used_pixels);

    return (ErrorStatus);
}


/*
 * Create an XImage with its data
 */
#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int CreateXImage(
  Display*      display
, Visual*       visual
, unsigned int  depth
, int           format
, unsigned int  width
, unsigned int  height
, XImage**      image_return
)
#else
static int
CreateXImage(display, visual, depth, format, width, height, image_return)
    Display *display;
    Visual *visual;
    unsigned int depth;
    int format;
    unsigned int width;
    unsigned int height;
    XImage **image_return;
#endif
{
    int bitmap_pad;

    /* first get bitmap_pad */
    if (depth > 16)
	bitmap_pad = 32;
    else if (depth > 8)
	bitmap_pad = 16;
    else
	bitmap_pad = 8;

    /* then create the XImage with data = NULL and bytes_per_line = 0 */
    *image_return = XCreateImage(display, visual, depth, format, 0, 0,
				 width, height, bitmap_pad, 0);
    if (!*image_return)
	return (XpmNoMemory);

#if !defined(FOR_MSW) && !defined(AMIGA)
    /* now that bytes_per_line must have been set properly alloc data */
    (*image_return)->data =
	(char *) XpmMalloc((*image_return)->bytes_per_line * height);

    if (!(*image_return)->data) {
	XDestroyImage(*image_return);
	*image_return = NULL;
	return (XpmNoMemory);
    }
#else
    /* under FOR_MSW and AMIGA XCreateImage has done it all */
#endif
    return (XpmSuccess);
}

#ifndef FOR_MSW
# ifndef AMIGA
/*
 * The functions below are written from X11R5 MIT's code (XImUtil.c)
 *
 * The idea is to have faster functions than the standard XPutPixel function
 * to build the image data. Indeed we can speed up things by suppressing tests
 * performed for each pixel. We do the same tests but at the image level.
 * We also assume that we use only ZPixmap images with null offsets.
 */

LFUNC(_putbits, void, (register char *src, int dstoffset,
		       register int numbits, register char *dst));

LFUNC(_XReverse_Bytes, int, (register unsigned char *bpt, register int nb));

static unsigned char Const _reverse_byte[0x100] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int
_XReverse_Bytes(register unsigned char* bpt, register int nb)
#else
static int
_XReverse_Bytes(bpt, nb)
    register unsigned char *bpt;
    register int nb;
#endif
{
    do {
	*bpt = _reverse_byte[*bpt];
	bpt++;
    } while (--nb > 0);
    return 0;
}


#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
void xpm_xynormalizeimagebits(register unsigned char* bp, register XImage* img)
#else
void
xpm_xynormalizeimagebits(bp, img)
    register unsigned char *bp;
    register XImage *img;
#endif
{
    register unsigned char c;

    if (img->byte_order != img->bitmap_bit_order) {
	switch (img->bitmap_unit) {

	case 16:
	    c = *bp;
	    *bp = *(bp + 1);
	    *(bp + 1) = c;
	    break;

	case 32:
	    c = *(bp + 3);
	    *(bp + 3) = *bp;
	    *bp = c;
	    c = *(bp + 2);
	    *(bp + 2) = *(bp + 1);
	    *(bp + 1) = c;
	    break;
	}
    }
    if (img->bitmap_bit_order == MSBFirst)
	_XReverse_Bytes(bp, img->bitmap_unit >> 3);
}

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
void xpm_znormalizeimagebits(register unsigned char* bp, register XImage* img)
#else
void
xpm_znormalizeimagebits(bp, img)
    register unsigned char *bp;
    register XImage *img;
#endif
{
    register unsigned char c;

    switch (img->bits_per_pixel) {

    case 2:
	_XReverse_Bytes(bp, 1);
	break;

    case 4:
	*bp = ((*bp >> 4) & 0xF) | ((*bp << 4) & ~0xF);
	break;

    case 16:
	c = *bp;
	*bp = *(bp + 1);
	*(bp + 1) = c;
	break;

    case 24:
	c = *(bp + 2);
	*(bp + 2) = *bp;
	*bp = c;
	break;

    case 32:
	c = *(bp + 3);
	*(bp + 3) = *bp;
	*bp = c;
	c = *(bp + 2);
	*(bp + 2) = *(bp + 1);
	*(bp + 1) = c;
	break;
    }
}

static unsigned char Const _lomask[0x09] = {
0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
static unsigned char Const _himask[0x09] = {
0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00};

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static void _putbits(
  register char* src
, int            dstoffset
, register int   numbits
, register char* dst
)
#else
static void
_putbits(src, dstoffset, numbits, dst)
    register char *src;			/* address of source bit string */
    int dstoffset;			/* bit offset into destination;
					 * range is 0-31 */
    register int numbits;		/* number of bits to copy to
					 * destination */
    register char *dst;			/* address of destination bit string */
#endif
{
    register unsigned char chlo, chhi;
    int hibits;

    dst = dst + (dstoffset >> 3);
    dstoffset = dstoffset & 7;
    hibits = 8 - dstoffset;
    chlo = *dst & _lomask[dstoffset];
    for (;;) {
	chhi = (*src << dstoffset) & _himask[dstoffset];
	if (numbits <= hibits) {
	    chhi = chhi & _lomask[dstoffset + numbits];
	    *dst = (*dst & _himask[dstoffset + numbits]) | chlo | chhi;
	    break;
	}
	*dst = chhi | chlo;
	dst++;
	numbits = numbits - hibits;
	chlo = (unsigned char) (*src & _himask[hibits]) >> hibits;
	src++;
	if (numbits <= dstoffset) {
	    chlo = chlo & _lomask[numbits];
	    *dst = (*dst & _himask[numbits]) | chlo;
	    break;
	}
	numbits = numbits - dstoffset;
    }
}

/*
 * Default method to write pixels into a Z image data structure.
 * The algorithm used is:
 *
 *	copy the destination bitmap_unit or Zpixel to temp
 *	normalize temp if needed
 *	copy the pixel bits into the temp
 *	renormalize temp if needed
 *	copy the temp back into the destination image data
 */

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static void PutImagePixels(
, XImage*       image
, unsigned int  width
, unsigned int  height
, unsigned int* pixelindex
, Pixel*        pixels
)
#else
static void
PutImagePixels(image, width, height, pixelindex, pixels)
    XImage *image;
    unsigned int width;
    unsigned int height;
    unsigned int *pixelindex;
    Pixel *pixels;
#endif
{
    register char *src;
    register char *dst;
    register unsigned int *iptr;
    register int x, y, i;
    register char *data;
    Pixel pixel, px;
    int nbytes, depth, ibu, ibpp;

    data = image->data;
    iptr = pixelindex;
    depth = image->depth;
    if (depth == 1) {
	ibu = image->bitmap_unit;
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		pixel = pixels[*iptr];
		for (i = 0, px = pixel; i < sizeof(unsigned long);
		     i++, px >>= 8)
		    ((unsigned char *) &pixel)[i] = px;
		src = &data[XYINDEX(x, y, image)];
		dst = (char *) &px;
		px = 0;
		nbytes = ibu >> 3;
		for (i = nbytes; --i >= 0;)
		    *dst++ = *src++;
		XYNORMALIZE(&px, image);
		_putbits((char *) &pixel, (x % ibu), 1, (char *) &px);
		XYNORMALIZE(&px, image);
		src = (char *) &px;
		dst = &data[XYINDEX(x, y, image)];
		for (i = nbytes; --i >= 0;)
		    *dst++ = *src++;
	    }
    } else {
	ibpp = image->bits_per_pixel;
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		pixel = pixels[*iptr];
		if (depth == 4)
		    pixel &= 0xf;
		for (i = 0, px = pixel; i < sizeof(unsigned long); i++,
		     px >>= 8)
		    ((unsigned char *) &pixel)[i] = px;
		src = &data[ZINDEX(x, y, image)];
		dst = (char *) &px;
		px = 0;
		nbytes = (ibpp + 7) >> 3;
		for (i = nbytes; --i >= 0;)
		    *dst++ = *src++;
		ZNORMALIZE(&px, image);
		_putbits((char *) &pixel, (x * ibpp) & 7, ibpp, (char *) &px);
		ZNORMALIZE(&px, image);
		src = (char *) &px;
		dst = &data[ZINDEX(x, y, image)];
		for (i = nbytes; --i >= 0;)
		    *dst++ = *src++;
	    }
    }
}

/*
 * write pixels into a 32-bits Z image data structure
 */

#if !defined(WORD64) && !defined(LONG64)
/* this item is static but deterministic so let it slide; doesn't
 * hurt re-entrancy of this library. Note if it is actually const then would
 * be OK under rules of ANSI-C but probably not C++ which may not
 * want to allocate space for it.
 */
static unsigned long byteorderpixel = MSBFirst << 24;

#endif

/*
   WITHOUT_SPEEDUPS is a flag to be turned on if you wish to use the original
   3.2e code - by default you get the speeded-up version.
*/

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static void
PutImagePixels32(
, XImage*       image
, unsigned int  width
, unsigned int  height
, unsigned int* pixelindex
, Pixel*        pixels
)
#else
static void
PutImagePixels32(image, width, height, pixelindex, pixels)
    XImage *image;
    unsigned int width;
    unsigned int height;
    unsigned int *pixelindex;
    Pixel *pixels;
#endif
{
    unsigned char *data;
    unsigned int *iptr;
    int y;
    Pixel pixel;

#ifdef WITHOUT_SPEEDUPS

    int x;
    unsigned char *addr;

    data = (unsigned char *) image->data;
    iptr = pixelindex;
#if !defined(WORD64) && !defined(LONG64)
    if (*((char *) &byteorderpixel) == image->byte_order) {
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &data[ZINDEX32(x, y, image)];
		*((unsigned long *) addr) = pixels[*iptr];
	    }
    } else
#endif
    if (image->byte_order == MSBFirst)
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &data[ZINDEX32(x, y, image)];
		pixel = pixels[*iptr];
		addr[0] = pixel >> 24;
		addr[1] = pixel >> 16;
		addr[2] = pixel >> 8;
		addr[3] = pixel;
	    }
    else
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &data[ZINDEX32(x, y, image)];
		pixel = pixels[*iptr];
		addr[0] = pixel;
		addr[1] = pixel >> 8;
		addr[2] = pixel >> 16;
		addr[3] = pixel >> 24;
	    }

#else  /* WITHOUT_SPEEDUPS */

    int bpl = image->bytes_per_line;
    unsigned char *data_ptr, *max_data;

    data = (unsigned char *) image->data;
    iptr = pixelindex;
#if !defined(WORD64) && !defined(LONG64)
    if (*((char *) &byteorderpixel) == image->byte_order) {
	for (y = 0; y < height; y++) {
	    data_ptr = data;
	    max_data = data_ptr + (width << 2);

	    while (data_ptr < max_data) {
		*((unsigned long *) data_ptr) = pixels[*(iptr++)];
		data_ptr += (1 << 2);
	    }
	    data += bpl;
	}
    } else
#endif
    if (image->byte_order == MSBFirst)
	for (y = 0; y < height; y++) {
	    data_ptr = data;
	    max_data = data_ptr + (width << 2);

	    while (data_ptr < max_data) {
		pixel = pixels[*(iptr++)];

		*data_ptr++ = pixel >> 24;
		*data_ptr++ = pixel >> 16;
		*data_ptr++ = pixel >> 8;
		*data_ptr++ = pixel;

	    }
	    data += bpl;
	}
    else
	for (y = 0; y < height; y++) {
	    data_ptr = data;
	    max_data = data_ptr + (width << 2);

	    while (data_ptr < max_data) {
		pixel = pixels[*(iptr++)];

		*data_ptr++ = pixel;
		*data_ptr++ = pixel >> 8;
		*data_ptr++ = pixel >> 16;
		*data_ptr++ = pixel >> 24;
	    }
	    data += bpl;
	}

#endif /* WITHOUT_SPEEDUPS */
}

/*
 * write pixels into a 16-bits Z image data structure
 */

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static void PutImagePixels16(
, XImage*       image
, unsigned int  width
, unsigned int  height
, unsigned int* pixelindex
, Pixel*        pixels
)
#else
static void
PutImagePixels16(image, width, height, pixelindex, pixels)
    XImage *image;
    unsigned int width;
    unsigned int height;
    unsigned int *pixelindex;
    Pixel *pixels;
#endif
{
    unsigned char *data;
    unsigned int *iptr;
    int y;

#ifdef WITHOUT_SPEEDUPS

    int x;
    unsigned char *addr;

    data = (unsigned char *) image->data;
    iptr = pixelindex;
    if (image->byte_order == MSBFirst)
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &data[ZINDEX16(x, y, image)];
		addr[0] = pixels[*iptr] >> 8;
		addr[1] = pixels[*iptr];
	    }
    else
	for (y = 0; y < height; y++)
	    for (x = 0; x < width; x++, iptr++) {
		addr = &data[ZINDEX16(x, y, image)];
		addr[0] = pixels[*iptr];
		addr[1] = pixels[*iptr] >> 8;
	    }

#else  /* WITHOUT_SPEEDUPS */

    Pixel pixel;

    int bpl = image->bytes_per_line;
    unsigned char *data_ptr, *max_data;

    data = (unsigned char *) image->data;
    iptr = pixelindex;
    if (image->byte_order == MSBFirst)
	for (y = 0; y < height; y++) {
	    data_ptr = data;
	    max_data = data_ptr + (width << 1);

	    while (data_ptr < max_data) {
		pixel = pixels[*(iptr++)];

		data_ptr[0] = pixel >> 8;
		data_ptr[1] = pixel;

		data_ptr += (1 << 1);
	    }
	    data += bpl;
	}
    else
	for (y = 0; y < height; y++) {
	    data_ptr = data;
	    max_data = data_ptr + (width << 1);

	    while (data_ptr < max_data) {
		pixel = pixels[*(iptr++)];

		data_ptr[0] = pixel;
		data_ptr[1] = pixel >> 8;

		data_ptr += (1 << 1);
	    }
	    data += bpl;
	}

#endif /* WITHOUT_SPEEDUPS */
}

/*
 * write pixels into a 8-bits Z image data structure
 */

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static void PutImagePixels8(
, XImage*       image
, unsigned int  width
, unsigned int  height
, unsigned int* pixelindex
, Pixel*        pixels
)
#else
static void
PutImagePixels8(image, width, height, pixelindex, pixels)
    XImage *image;
    unsigned int width;
    unsigned int height;
    unsigned int *pixelindex;
    Pixel *pixels;
#endif
{
    char *data;
    unsigned int *iptr;
    int y;

#ifdef WITHOUT_SPEEDUPS

    int x;

    data = image->data;
    iptr = pixelindex;
    for (y = 0; y < height; y++)
	for (x = 0; x < width; x++, iptr++)
	    data[ZINDEX8(x, y, image)] = pixels[*iptr];

#else  /* WITHOUT_SPEEDUPS */

    int bpl = image->bytes_per_line;
    char *data_ptr, *max_data;

    data = image->data;
    iptr = pixelindex;

    for (y = 0; y < height; y++) {
	data_ptr = data;
	max_data = data_ptr + width;

	while (data_ptr < max_data)
	    *(data_ptr++) = pixels[*(iptr++)];

	data += bpl;
    }

#endif /* WITHOUT_SPEEDUPS */
}

/*
 * write pixels into a 1-bit depth image data structure and **offset null**
 */

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static void PutImagePixels1(
, XImage*       image
, unsigned int  width
, unsigned int  height
, unsigned int* pixelindex
, Pixel*        pixels
)
#else
static void
PutImagePixels1(image, width, height, pixelindex, pixels)
    XImage *image;
    unsigned int width;
    unsigned int height;
    unsigned int *pixelindex;
    Pixel *pixels;
#endif
{
    if (image->byte_order != image->bitmap_bit_order)
	PutImagePixels(image, width, height, pixelindex, pixels);
    else {
	unsigned int *iptr;
	int y;
	char *data;

#ifdef WITHOUT_SPEEDUPS

	int x;

	data = image->data;
	iptr = pixelindex;
	if (image->bitmap_bit_order == MSBFirst)
	    for (y = 0; y < height; y++)
		for (x = 0; x < width; x++, iptr++) {
		    if (pixels[*iptr] & 1)
			data[ZINDEX1(x, y, image)] |= 0x80 >> (x & 7);
		    else
			data[ZINDEX1(x, y, image)] &= ~(0x80 >> (x & 7));
		}
	else
	    for (y = 0; y < height; y++)
		for (x = 0; x < width; x++, iptr++) {
		    if (pixels[*iptr] & 1)
			data[ZINDEX1(x, y, image)] |= 1 << (x & 7);
		    else
			data[ZINDEX1(x, y, image)] &= ~(1 << (x & 7));
		}

#else  /* WITHOUT_SPEEDUPS */

	char value;
	char *data_ptr, *max_data;
	int bpl = image->bytes_per_line;
	int diff, count;

	data = image->data;
	iptr = pixelindex;

	diff = width & 7;
	width >>= 3;

	if (image->bitmap_bit_order == MSBFirst)
	    for (y = 0; y < height; y++) {
		data_ptr = data;
		max_data = data_ptr + width;
		while (data_ptr < max_data) {
		    value = 0;

		    value = (value << 1) | (pixels[*(iptr++)] & 1);
		    value = (value << 1) | (pixels[*(iptr++)] & 1);
		    value = (value << 1) | (pixels[*(iptr++)] & 1);
		    value = (value << 1) | (pixels[*(iptr++)] & 1);
		    value = (value << 1) | (pixels[*(iptr++)] & 1);
		    value = (value << 1) | (pixels[*(iptr++)] & 1);
		    value = (value << 1) | (pixels[*(iptr++)] & 1);
		    value = (value << 1) | (pixels[*(iptr++)] & 1);

		    *(data_ptr++) = value;
		}
		if (diff) {
		    value = 0;
		    for (count = 0; count < diff; count++) {
			if (pixels[*(iptr++)] & 1)
			    value |= (0x80 >> count);
		    }
		    *(data_ptr) = value;
		}
		data += bpl;
	    }
	else
	    for (y = 0; y < height; y++) {
		data_ptr = data;
		max_data = data_ptr + width;
		while (data_ptr < max_data) {
		    value = 0;
		    iptr += 8;

		    value = (value << 1) | (pixels[*(--iptr)] & 1);
		    value = (value << 1) | (pixels[*(--iptr)] & 1);
		    value = (value << 1) | (pixels[*(--iptr)] & 1);
		    value = (value << 1) | (pixels[*(--iptr)] & 1);
		    value = (value << 1) | (pixels[*(--iptr)] & 1);
		    value = (value << 1) | (pixels[*(--iptr)] & 1);
		    value = (value << 1) | (pixels[*(--iptr)] & 1);
		    value = (value << 1) | (pixels[*(--iptr)] & 1);

		    iptr += 8;
		    *(data_ptr++) = value;
		}
		if (diff) {
		    value = 0;
		    for (count = 0; count < diff; count++) {
			if (pixels[*(iptr++)] & 1)
			    value |= (1 << count);
		    }
		    *(data_ptr) = value;
		}
		data += bpl;
	    }

#endif /* WITHOUT_SPEEDUPS */
    }
}

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
int XpmCreatePixmapFromXpmImage(
, Display*       display
, Drawable       d
, XpmImage*      image
, Pixmap*        pixmap_return
, Pixmap*        shapemask_return
, XpmAttributes* attributes
#else
int
XpmCreatePixmapFromXpmImage(display, d, image,
			    pixmap_return, shapemask_return, attributes)
    Display *display;
    Drawable d;
    XpmImage *image;
    Pixmap *pixmap_return;
    Pixmap *shapemask_return;
    XpmAttributes *attributes;
#endif
{
    XImage *ximage, *shapeimage;
    int ErrorStatus;

    /* initialize return values */
    if (pixmap_return)
	*pixmap_return = 0;
    if (shapemask_return)
	*shapemask_return = 0;

    /* create the ximages */
    ErrorStatus = XpmCreateImageFromXpmImage(display, image,
					     (pixmap_return ? &ximage : NULL),
					     (shapemask_return ?
					      &shapeimage : NULL),
					     attributes);
    if (ErrorStatus < 0)
	return (ErrorStatus);

    /* create the pixmaps and destroy images */
    if (pixmap_return && ximage) {
	xpmCreatePixmapFromImage(display, d, ximage, pixmap_return);
	XDestroyImage(ximage);
    }
    if (shapemask_return && shapeimage) {
	xpmCreatePixmapFromImage(display, d, shapeimage, shapemask_return);
	XDestroyImage(shapeimage);
    }
    return (ErrorStatus);
}

# else /* AMIGA */

static void
APutImagePixels (
    XImage        *image,
    unsigned int   width,
    unsigned int   height,
    unsigned int  *pixelindex,
    Pixel         *pixels)
{
    unsigned int   *data = pixelindex;
    unsigned int    x, y;
    unsigned char  *array;
    XImage         *tmp_img;
    BOOL            success = FALSE;

    array = XpmMalloc ((((width+15)>>4)<<4)*sizeof (*array));
    if (array != NULL)
    {
	tmp_img = AllocXImage ((((width+15)>>4)<<4), 1,
			       image->rp->BitMap->Depth);
	if (tmp_img != NULL)
	{
	    for (y = 0; y < height; ++y)
	    {
		for (x = 0; x < width; ++x)
		    array[x] = pixels[*(data++)];
		WritePixelLine8 (image->rp, 0, y, width, array, tmp_img->rp);
	    }
	    FreeXImage (tmp_img);
	    success = TRUE;
	}
	XpmFree (array);
    }

    if (!success)
    {
	for (y = 0; y < height; ++y)
	    for (x = 0; x < width; ++x)
		XPutPixel (image, x, y, pixels[*(data++)]);
    }
}

# endif/* AMIGA */
#else  /* FOR_MSW part follows */

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static void MSWPutImagePixels(
  HPS           hps
, Display*      dc
, XImage*       image
, unsigned int  width
, unsigned int  height
, unsigned int* pixelindex
, Pixel*        pixels
)
#else
static void
MSWPutImagePixels(dc, image, width, height, pixelindex, pixels)
    Display *dc;
    XImage *image;
    unsigned int width;
    unsigned int height;
    unsigned int *pixelindex;
    Pixel *pixels;
#endif
{
    unsigned int *data = pixelindex;
    unsigned int x, y;
    HBITMAP obm;

#ifdef __OS2__
    POINTL                                point;

    obm = GpiSetBitmap(hps, image->bitmap);
#else
    obm = SelectObject(*dc, image->bitmap);
#endif

    for (y = 0; y < height; y++) {
	for (x = 0; x < width; x++) {
#ifdef __OS2__
     point.x = x;
     point.y = y;
     GpiSetColor(hps, (LONG)pixels[*(data++)]);
     GpiSetPel(hps, &point);
#else

	    SetPixel(*dc, x, y, pixels[*(data++)]); /* data is [x+y*width] */
#endif
	}
    }
#ifdef __OS2__
    GpiSetBitmap(hps, obm);
#else
    SelectObject(*dc, obm);
#endif
}

#endif /* FOR_MSW */



#if !defined(FOR_MSW) && !defined(AMIGA)

static int
PutPixel1(ximage, x, y, pixel)
    register XImage *ximage;
    int x;
    int y;
    unsigned long pixel;
{
    register char *src;
    register char *dst;
    register int i;
    register char *data;
    Pixel px;
    int nbytes;

    for (i=0, px=pixel; i<sizeof(unsigned long); i++, px>>=8)
	((unsigned char *)&pixel)[i] = px;
    src = &ximage->data[XYINDEX(x, y, ximage)];
    dst = (char *)&px;
    px = 0;
    nbytes = ximage->bitmap_unit >> 3;
    for (i = nbytes; --i >= 0; ) *dst++ = *src++;
    XYNORMALIZE(&px, ximage);
    i = ((x + ximage->xoffset) % ximage->bitmap_unit);
    _putbits ((char *)&pixel, i, 1, (char *)&px);
    XYNORMALIZE(&px, ximage);
    src = (char *) &px;
    dst = &ximage->data[XYINDEX(x, y, ximage)];
    for (i = nbytes; --i >= 0; )
	*dst++ = *src++;

    return 1;
}

static int
PutPixel(ximage, x, y, pixel)
    register XImage *ximage;
    int x;
    int y;
    unsigned long pixel;
{
    register char *src;
    register char *dst;
    register int i;
    register char *data;
    Pixel px;
    int nbytes, ibpp;

    ibpp = ximage->bits_per_pixel;
    if (ximage->depth == 4)
	pixel &= 0xf;
    for (i = 0, px = pixel; i < sizeof(unsigned long); i++, px >>= 8)
	((unsigned char *) &pixel)[i] = px;
    src = &ximage->data[ZINDEX(x, y, ximage)];
    dst = (char *) &px;
    px = 0;
    nbytes = (ibpp + 7) >> 3;
    for (i = nbytes; --i >= 0;)
	*dst++ = *src++;
    ZNORMALIZE(&px, ximage);
    _putbits((char *) &pixel, (x * ibpp) & 7, ibpp, (char *) &px);
    ZNORMALIZE(&px, ximage);
    src = (char *) &px;
    dst = &ximage->data[ZINDEX(x, y, ximage)];
    for (i = nbytes; --i >= 0;)
	*dst++ = *src++;

    return 1;
}

static int
PutPixel32(ximage, x, y, pixel)
    register XImage *ximage;
    int x;
    int y;
    unsigned long pixel;
{
    unsigned char *addr;

    addr = &((unsigned char *)ximage->data) [ZINDEX32(x, y, ximage)];
    *((unsigned long *)addr) = pixel;
    return 1;
}

static int
PutPixel32MSB(ximage, x, y, pixel)
    register XImage *ximage;
    int x;
    int y;
    unsigned long pixel;
{
    unsigned char *addr;

    addr = &((unsigned char *)ximage->data) [ZINDEX32(x, y, ximage)];
    addr[0] = pixel >> 24;
    addr[1] = pixel >> 16;
    addr[2] = pixel >> 8;
    addr[3] = pixel;
    return 1;
}

static int
PutPixel32LSB(ximage, x, y, pixel)
    register XImage *ximage;
    int x;
    int y;
    unsigned long pixel;
{
    unsigned char *addr;

    addr = &((unsigned char *)ximage->data) [ZINDEX32(x, y, ximage)];
    addr[3] = pixel >> 24;
    addr[2] = pixel >> 16;
    addr[1] = pixel >> 8;
    addr[0] = pixel;
    return 1;
}

static int
PutPixel16MSB(ximage, x, y, pixel)
    register XImage *ximage;
    int x;
    int y;
    unsigned long pixel;
{
    unsigned char *addr;

    addr = &((unsigned char *)ximage->data) [ZINDEX16(x, y, ximage)];
    addr[0] = pixel >> 8;
    addr[1] = pixel;
    return 1;
}

static int
PutPixel16LSB(ximage, x, y, pixel)
    register XImage *ximage;
    int x;
    int y;
    unsigned long pixel;
{
    unsigned char *addr;

    addr = &((unsigned char *)ximage->data) [ZINDEX16(x, y, ximage)];
    addr[1] = pixel >> 8;
    addr[0] = pixel;
    return 1;
}

static int
PutPixel8(ximage, x, y, pixel)
    register XImage *ximage;
    int x;
    int y;
    unsigned long pixel;
{
    ximage->data[ZINDEX8(x, y, ximage)] = pixel;
    return 1;
}

static int
PutPixel1MSB(ximage, x, y, pixel)
    register XImage *ximage;
    int x;
    int y;
    unsigned long pixel;
{
    if (pixel & 1)
	ximage->data[ZINDEX1(x, y, ximage)] |= 0x80 >> (x & 7);
    else
	ximage->data[ZINDEX1(x, y, ximage)] &= ~(0x80 >> (x & 7));
    return 1;
}

static int
PutPixel1LSB(ximage, x, y, pixel)
    register XImage *ximage;
    int x;
    int y;
    unsigned long pixel;
{
    if (pixel & 1)
	ximage->data[ZINDEX1(x, y, ximage)] |= 1 << (x & 7);
    else
	ximage->data[ZINDEX1(x, y, ximage)] &= ~(1 << (x & 7));
    return 1;
}

#endif /* not FOR_MSW && not AMIGA */

/*
 * This function parses an Xpm file or data and directly create an XImage
 */
#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
int xpmParseDataAndCreate(
  Display*       display
, xpmData*       data
, XImage**       image_return
, XImage**       shapeimage_return
, XpmImage*      image
, XpmInfo*       info
, XpmAttributes* attributes
)
#else
int
xpmParseDataAndCreate(display, data, image_return, shapeimage_return,
		      image, info, attributes)
    Display *display;
    xpmData *data;
    XImage **image_return;
    XImage **shapeimage_return;
    XpmImage *image;
    XpmInfo *info;
    XpmAttributes *attributes;
#endif
{
    /* variables stored in the XpmAttributes structure */
    Visual *visual;
    Colormap colormap;
    unsigned int depth;
    int bitmap_format;
    XpmFreeColorsFunc freeColors;
    void *closure;

    /* variables to return */
    XImage *ximage = NULL;
    XImage *shapeimage = NULL;
    unsigned int mask_pixel_index = XpmUndefPixel;

    /* calculation variables */
    Pixel *image_pixels = NULL;
    Pixel *mask_pixels = NULL;
    Pixel *alloc_pixels = NULL;
    Pixel *used_pixels = NULL;
    unsigned int nalloc_pixels = 0;
    unsigned int nused_pixels = 0;
    unsigned int width, height, ncolors, cpp;
    unsigned int x_hotspot, y_hotspot, hotspot = 0, extensions = 0;
    XpmColor *colorTable = NULL;
    char *hints_cmt = NULL;
    char *colors_cmt = NULL;
    char *pixels_cmt = NULL;

    unsigned int cmts;
    int ErrorStatus;
    xpmHashTable hashtable;


    /* initialize return values */
    if (image_return)
	*image_return = NULL;
    if (shapeimage_return)
	*shapeimage_return = NULL;


    /* retrieve information from the XpmAttributes */
    if (attributes && (attributes->valuemask & XpmVisual))
	visual = attributes->visual;
    else
	visual = XDefaultVisual(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmColormap))
	colormap = attributes->colormap;
    else
	colormap = XDefaultColormap(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmDepth))
	depth = attributes->depth;
    else
	depth = XDefaultDepth(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmBitmapFormat))
	bitmap_format = attributes->bitmap_format;
    else
	bitmap_format = ZPixmap;

    if (attributes && (attributes->valuemask & XpmFreeColors))
	freeColors = attributes->free_colors;
    else
	freeColors = FreeColors;
    if (attributes && (attributes->valuemask & XpmColorClosure))
	closure = attributes->color_closure;
    else
	closure = NULL;

    cmts = info && (info->valuemask & XpmReturnComments);

    /*
     * parse the header
     */
    ErrorStatus = xpmParseHeader(data);
    if (ErrorStatus != XpmSuccess)
	return (ErrorStatus);

    /*
     * read values
     */
    ErrorStatus = xpmParseValues(data, &width, &height, &ncolors, &cpp,
				 &x_hotspot, &y_hotspot, &hotspot,
				 &extensions);
    if (ErrorStatus != XpmSuccess)
	return (ErrorStatus);

    /*
     * store the hints comment line
     */
    if (cmts)
	xpmGetCmt(data, &hints_cmt);

    /*
     * init the hastable
     */
    if (USE_HASHTABLE) {
	ErrorStatus = xpmHashTableInit(&hashtable);
	if (ErrorStatus != XpmSuccess)
	    return (ErrorStatus);
    }

    /*
     * read colors
     */
    ErrorStatus = xpmParseColors(data, ncolors, cpp, &colorTable, &hashtable);
    if (ErrorStatus != XpmSuccess)
	RETURN(ErrorStatus);

    /*
     * store the colors comment line
     */
    if (cmts)
	xpmGetCmt(data, &colors_cmt);

    /* malloc pixels index tables */
    image_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * ncolors);
    if (!image_pixels)
	RETURN(XpmNoMemory);

    mask_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * ncolors);
    if (!mask_pixels)
	RETURN(XpmNoMemory);

    /* maximum of allocated pixels will be the number of colors */
    alloc_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * ncolors);
    if (!alloc_pixels)
	RETURN(XpmNoMemory);

    /* maximum of allocated pixels will be the number of colors */
    used_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * ncolors);
    if (!used_pixels)
	RETURN(XpmNoMemory);

    /* get pixel colors, store them in index tables */
    ErrorStatus = CreateColors(display, attributes, colorTable, ncolors,
			       image_pixels, mask_pixels, &mask_pixel_index,
			       alloc_pixels, &nalloc_pixels, used_pixels,
			       &nused_pixels);

    if (ErrorStatus != XpmSuccess
	&& (ErrorStatus < 0 || (attributes
				&& (attributes->valuemask & XpmExactColors)
				&& attributes->exactColors)))
	RETURN(ErrorStatus);

    /* now create the ximage */
    if (image_return) {
	ErrorStatus = CreateXImage(display, visual, depth,
				   (depth == 1 ? bitmap_format : ZPixmap),
				   width, height, &ximage);
	if (ErrorStatus != XpmSuccess)
	    RETURN(ErrorStatus);

#if !defined(FOR_MSW) && !defined(AMIGA)

	/*
	 * set the XImage pointer function, to be used with XPutPixel,
	 * to an internal optimized function
	 */

	if (ximage->bits_per_pixel == 8)
	    ximage->f.put_pixel = PutPixel8;
	else if (((ximage->bits_per_pixel | ximage->depth) == 1) &&
		 (ximage->byte_order == ximage->bitmap_bit_order))
	    if (ximage->bitmap_bit_order == MSBFirst)
		ximage->f.put_pixel = PutPixel1MSB;
	    else
		ximage->f.put_pixel = PutPixel1LSB;
	else if (ximage->bits_per_pixel == 16)
	    if (ximage->bitmap_bit_order == MSBFirst)
		ximage->f.put_pixel = PutPixel16MSB;
	    else
		ximage->f.put_pixel = PutPixel16LSB;
	else if (ximage->bits_per_pixel == 32)
#if !defined(WORD64) && !defined(LONG64)
	    if (*((char *)&byteorderpixel) == ximage->byte_order)
		ximage->f.put_pixel = PutPixel32;
	    else
#endif
		if (ximage->bitmap_bit_order == MSBFirst)
		    ximage->f.put_pixel = PutPixel32MSB;
		else
		    ximage->f.put_pixel = PutPixel32LSB;
	else if ((ximage->bits_per_pixel | ximage->depth) == 1)
	    ximage->f.put_pixel = PutPixel1;
	else
	    ximage->f.put_pixel = PutPixel;
#endif /* not FOR_MSW && not AMIGA */
    }

    /* create the shape mask image */
    if (mask_pixel_index != XpmUndefPixel && shapeimage_return) {
	ErrorStatus = CreateXImage(display, visual, 1, bitmap_format,
				   width, height, &shapeimage);
	if (ErrorStatus != XpmSuccess)
	    RETURN(ErrorStatus);

#if !defined(FOR_MSW) && !defined(AMIGA)
	if (shapeimage->bitmap_bit_order == MSBFirst)
	    shapeimage->f.put_pixel = PutPixel1MSB;
	else
	    shapeimage->f.put_pixel = PutPixel1LSB;
#endif
    }

    /*
     * read pixels and put them in the XImage
     */
    ErrorStatus = ParseAndPutPixels(
#ifdef FOR_MSW
				    display,
#endif
				    data, width, height, ncolors, cpp,
				    colorTable, &hashtable,
				    ximage, image_pixels,
				    shapeimage, mask_pixels);
    XpmFree(image_pixels);
    image_pixels = NULL;
    XpmFree(mask_pixels);
    mask_pixels = NULL;

    /*
     * free the hastable
     */
    if (ErrorStatus != XpmSuccess)
	RETURN(ErrorStatus)
    else if (USE_HASHTABLE)
	xpmHashTableFree(&hashtable);

    /*
     * store the pixels comment line
     */
    if (cmts)
	xpmGetCmt(data, &pixels_cmt);

    /*
     * parse extensions
     */
    if (info && (info->valuemask & XpmReturnExtensions))
	if (extensions) {
	    ErrorStatus = xpmParseExtensions(data, &info->extensions,
					     &info->nextensions);
	    if (ErrorStatus != XpmSuccess)
		RETURN(ErrorStatus);
	} else {
	    info->extensions = NULL;
	    info->nextensions = 0;
	}

    /*
     * store found informations in the XpmImage structure
     */
    image->width = width;
    image->height = height;
    image->cpp = cpp;
    image->ncolors = ncolors;
    image->colorTable = colorTable;
    image->data = NULL;

    if (info) {
	if (cmts) {
	    info->hints_cmt = hints_cmt;
	    info->colors_cmt = colors_cmt;
	    info->pixels_cmt = pixels_cmt;
	}
	if (hotspot) {
	    info->x_hotspot = x_hotspot;
	    info->y_hotspot = y_hotspot;
	    info->valuemask |= XpmHotspot;
	}
    }
    /* if requested return used pixels in the XpmAttributes structure */
    if (attributes && (attributes->valuemask & XpmReturnPixels ||
/* 3.2 backward compatibility code */
	attributes->valuemask & XpmReturnInfos)) {
/* end 3.2 bc */
	attributes->pixels = used_pixels;
	attributes->npixels = nused_pixels;
	attributes->mask_pixel = mask_pixel_index;
    } else
	XpmFree(used_pixels);

    /* if requested return alloc'ed pixels in the XpmAttributes structure */
    if (attributes && (attributes->valuemask & XpmReturnAllocPixels)) {
	attributes->alloc_pixels = alloc_pixels;
	attributes->nalloc_pixels = nalloc_pixels;
    } else
	XpmFree(alloc_pixels);

    /* return created images */
    if (image_return)
	*image_return = ximage;
    if (shapeimage_return)
	*shapeimage_return = shapeimage;

    return (XpmSuccess);

/* exit point in case of error, free only locally allocated variables */
error:
    if (USE_HASHTABLE)
	xpmHashTableFree(&hashtable);
    if (colorTable)
	xpmFreeColorTable(colorTable, ncolors);
    if (hints_cmt)
	XpmFree(hints_cmt);
    if (colors_cmt)
	XpmFree(colors_cmt);
    if (pixels_cmt)
	XpmFree(pixels_cmt);
    if (ximage)
	XDestroyImage(ximage);
    if (shapeimage)
	XDestroyImage(shapeimage);
    if (image_pixels)
	XpmFree(image_pixels);
    if (mask_pixels)
	XpmFree(mask_pixels);
    if (nalloc_pixels)
	(*freeColors)(display, colormap, alloc_pixels, nalloc_pixels, NULL);
    if (alloc_pixels)
	XpmFree(alloc_pixels);
    if (used_pixels)
	XpmFree(used_pixels);

    return (ErrorStatus);
}

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
static int ParseAndPutPixels(
  Display*      dc
, xpmData*      data
, unsigned int  width
, unsigned int  height
, unsigned int  ncolors
, unsigned int  cpp
, XpmColor*     colorTable
, xpmHashTable* hashtable
, XImage*       image
, Pixel*        image_pixels
, XImage*       shapeimage
, Pixel*        shape_pixels
)
#else
static int
ParseAndPutPixels(
#ifdef FOR_MSW
		  dc,
#endif
		  data, width, height, ncolors, cpp, colorTable, hashtable,
		  image, image_pixels, shapeimage, shape_pixels)
#ifdef FOR_MSW
    Display *dc;
#endif
    xpmData *data;
    unsigned int width;
    unsigned int height;
    unsigned int ncolors;
    unsigned int cpp;
    XpmColor *colorTable;
    xpmHashTable *hashtable;
    XImage *image;
    Pixel *image_pixels;
    XImage *shapeimage;
    Pixel *shape_pixels;
#endif
{
    unsigned int a, x, y;
#ifdef __OS2__
     HAB          hab;
     HPS          hps;
     DEVOPENSTRUC dop = {NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL};
     SIZEL        sizl = {0, 0};
     POINTL       point;
#endif

    switch (cpp) {

    case (1):				/* Optimize for single character
					 * colors */
	{
	    unsigned short colidx[256];
#ifdef FOR_MSW
	    HDC shapedc;
	    HBITMAP obm, sobm;

	    if ( shapeimage ) {
#ifdef __OS2__
      shapedc = DevOpenDC(hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE);
      hps = GpiCreatePS(hab, *dc, &sizl, GPIA_ASSOC | PU_PELS);
      sobm = GpiSetBitmap(hps, shapeimage->bitmap);
#else
		shapedc = CreateCompatibleDC(*dc);
		sobm = SelectObject(shapedc, shapeimage->bitmap);
#endif
	    } else {
	        shapedc = NULL;
	    }
#ifdef __OS2__
     obm = GpiSetBitmap(hps, image->bitmap);
#else
	    obm = SelectObject(*dc, image->bitmap);
#endif

#endif


	    bzero((char *)colidx, 256 * sizeof(short));
	    for (a = 0; a < ncolors; a++)
		colidx[(unsigned char)colorTable[a].string[0]] = a + 1;

	    for (y = 0; y < height; y++) {
		xpmNextString(data);
		for (x = 0; x < width; x++) {
		    int c = xpmGetC(data);

		    if (c > 0 && c < 256 && colidx[c] != 0) {
#ifndef FOR_MSW
			XPutPixel(image, x, y, image_pixels[colidx[c] - 1]);
			if (shapeimage)
			    XPutPixel(shapeimage, x, y,
				      shape_pixels[colidx[c] - 1]);
#else
#ifdef __OS2__
     point.x = x;
     point.y = y;
     GpiSetColor(hps, (LONG)image_pixels[colidx[c] - 1]);
     GpiSetPel(hps, &point);
#else
			SetPixel(*dc, x, y, image_pixels[colidx[c] - 1]);
#endif
			if (shapedc) {
#ifdef __OS2__
     point.x = x;
     point.y = y;
     GpiSetColor(hps, (LONG)shape_pixels[colidx[c] - 1]);
     GpiSetPel(hps, &point);
#else
			    SetPixel(shapedc, x, y, shape_pixels[colidx[c] - 1]);
#endif
			}
#endif
		    } else
			return (XpmFileInvalid);
		}
	    }
#ifdef FOR_MSW
	    if ( shapedc ) {
#ifdef __OS2__
         GpiSetBitmap(hps, sobm);
         DevCloseDC(shapedc);
#else
	        SelectObject(shapedc, sobm);
		DeleteDC(shapedc);
#endif
	    }
#ifdef __OS2__
     GpiSetBitmap(hps, obm);
#else
	    SelectObject(*dc, obm);
#endif
#endif
	}
	break;

    case (2):				/* Optimize for double character
					 * colors */
	{
/* free all allocated pointers at all exits */
#define FREE_CIDX {int f; for (f = 0; f < 256; f++) \
if (cidx[f]) XpmFree(cidx[f]);}

	    /* array of pointers malloced by need */
	    unsigned short *cidx[256];
	    int char1;

	    bzero((char *)cidx, 256 * sizeof(unsigned short *)); /* init */
	    for (a = 0; a < ncolors; a++) {
		char1 = colorTable[a].string[0];
		if (cidx[char1] == NULL) { /* get new memory */
		    cidx[char1] = (unsigned short *)
			XpmCalloc(256, sizeof(unsigned short));
		    if (cidx[char1] == NULL) { /* new block failed */
			FREE_CIDX;
			return (XpmNoMemory);
		    }
		}
		cidx[char1][(unsigned char)colorTable[a].string[1]] = a + 1;
	    }

	    for (y = 0; y < height; y++) {
		xpmNextString(data);
		for (x = 0; x < width; x++) {
		    int cc1 = xpmGetC(data);
		    if (cc1 > 0 && cc1 < 256) {
			int cc2 = xpmGetC(data);
			if (cc2 > 0 && cc2 < 256 &&
			    cidx[cc1] && cidx[cc1][cc2] != 0) {
#ifndef FOR_MSW
			    XPutPixel(image, x, y,
				      image_pixels[cidx[cc1][cc2] - 1]);
			    if (shapeimage)
				XPutPixel(shapeimage, x, y,
					  shape_pixels[cidx[cc1][cc2] - 1]);
#else
#ifdef __OS2__
     *dc = DevOpenDC(hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE);
     hps = GpiCreatePS(hab, *dc, &sizl, GPIA_ASSOC | PU_PELS);

     GpiSetBitmap(hps, image->bitmap);
     point.x = x;
     point.y = y;
     GpiSetColor(hps, (LONG)image_pixels[cidx[cc1][cc2] - 1]);
     GpiSetPel(hps, &point);
#else
			SelectObject(*dc, image->bitmap);
			SetPixel(*dc, x, y, image_pixels[cidx[cc1][cc2] - 1]);
#endif
			if (shapeimage) {
#ifdef __OS2__
     GpiSetBitmap(hps, shapeimage->bitmap);
     point.x = x;
     point.y = y;
     GpiSetColor(hps, (LONG)shape_pixels[cidx[cc1][cc2] - 1]);
     GpiSetPel(hps, &point);
#else
			    SelectObject(*dc, shapeimage->bitmap);
			    SetPixel(*dc, x, y,
				     shape_pixels[cidx[cc1][cc2] - 1]);
#endif
			}
#endif
			} else {
			    FREE_CIDX;
			    return (XpmFileInvalid);
			}
		    } else {
			FREE_CIDX;
			return (XpmFileInvalid);
		    }
		}
	    }
	    FREE_CIDX;
	}
	break;

    default:				/* Non-optimized case of long color
					 * names */
	{
	    char *s;
	    char buf[BUFSIZ];

	    buf[cpp] = '\0';
	    if (USE_HASHTABLE) {
		xpmHashAtom *slot;

		for (y = 0; y < height; y++) {
		    xpmNextString(data);
		    for (x = 0; x < width; x++) {
			for (a = 0, s = buf; a < cpp; a++, s++)
			    *s = xpmGetC(data);
			slot = xpmHashSlot(hashtable, buf);
			if (!*slot)	/* no color matches */
			    return (XpmFileInvalid);
#ifndef FOR_MSW
			XPutPixel(image, x, y,
				  image_pixels[HashColorIndex(slot)]);
			if (shapeimage)
			    XPutPixel(shapeimage, x, y,
				      shape_pixels[HashColorIndex(slot)]);
#else
#ifdef __OS2__
     *dc = DevOpenDC(hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE);
     hps = GpiCreatePS(hab, *dc, &sizl, GPIA_ASSOC | PU_PELS);

     GpiSetBitmap(hps, image->bitmap);
     point.x = x;
     point.y = y;
     GpiSetColor(hps, (LONG)image_pixels[HashColorIndex(slot)]);
     GpiSetPel(hps, &point);
#else
			SelectObject(*dc, image->bitmap);
			SetPixel(*dc, x, y,
				 image_pixels[HashColorIndex(slot)]);
#endif
			if (shapeimage) {
#ifdef __OS2__
     GpiSetBitmap(hps, shapeimage->bitmap);
     point.x = x;
     point.y = y;
     GpiSetColor(hps, (LONG)shape_pixels[HashColorIndex(slot)]);
     GpiSetPel(hps, &point);
#else
			    SelectObject(*dc, shapeimage->bitmap);
			    SetPixel(*dc, x, y,
				     shape_pixels[HashColorIndex(slot)]);
#endif
			}
#endif
		    }
		}
	    } else {
		for (y = 0; y < height; y++) {
		    xpmNextString(data);
		    for (x = 0; x < width; x++) {
			for (a = 0, s = buf; a < cpp; a++, s++)
			    *s = xpmGetC(data);
			for (a = 0; a < ncolors; a++)
			    if (!strcmp(colorTable[a].string, buf))
				break;
			if (a == ncolors)	/* no color matches */
			    return (XpmFileInvalid);
#ifndef FOR_MSW
			XPutPixel(image, x, y, image_pixels[a]);
			if (shapeimage)
			    XPutPixel(shapeimage, x, y, shape_pixels[a]);
#else
#ifdef __OS2__
     *dc = DevOpenDC(hab, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dop, NULLHANDLE);
     hps = GpiCreatePS(hab, *dc, &sizl, GPIA_ASSOC | PU_PELS);

     GpiSetBitmap(hps, image->bitmap);
     point.x = x;
     point.y = y;
     GpiSetColor(hps, (LONG)image_pixels[a]);
     GpiSetPel(hps, &point);
#else
			SelectObject(*dc, image->bitmap);
			SetPixel(*dc, x, y, image_pixels[a]);
#endif
			if (shapeimage) {
#ifdef __OS2__
     GpiSetBitmap(hps, image->bitmap);
     point.x = x;
     point.y = y;
     GpiSetColor(hps, (LONG)shape_pixels[a]);
     GpiSetPel(hps, &point);
#else
			    SelectObject(*dc, shapeimage->bitmap);
			    SetPixel(*dc, x, y, shape_pixels[a]);
#endif
			}
#endif
		    }
		}
	    }
	}
	break;
    }
    return (XpmSuccess);
}
